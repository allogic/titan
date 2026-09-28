#pragma warning(disable : 4146)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)
#pragma warning(disable : 4267)
#pragma warning(disable : 4275)
#pragma warning(disable : 4291)
#pragma warning(disable : 4805)
#pragma warning(disable : 4996)

#include <ti_pch.h>
#include <ti_clang.h>

#undef VERSION_MAJOR
#undef VERSION_MINOR

#include <memory>
#include <string>
#include <utility>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Lex/PreprocessorOptions.h>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/MC/TargetRegistry.h>

static clang::CompilerInstance s_compiler_instance;
static clang::EmitLLVMOnlyAction s_emit_llvm_only_action;

static std::unique_ptr<llvm::orc::LLJIT> s_jit;

uint8_t clang_create(void) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  llvm::orc::LLJITBuilder jit_builder;

  if (llvm::Error error = jit_builder.create().moveInto(s_jit)) {

    llvm::errs() << "failed to create LLJIT: " << llvm::toString(std::move(error)) << "\n";

    return 1;
  }

  return 0;
}
uint8_t clang_compile(char const *source_code, void **buffer, uint64_t *buffer_size) {
  clang::CompilerInstance compiler_instance;

  compiler_instance.createVirtualFileSystem();
  compiler_instance.createDiagnostics();

  char const *compiler_args[] = {
    "-std=c23",
    "main.c",
  };

  if (clang::CompilerInvocation::CreateFromArgs(compiler_instance.getInvocation(), compiler_args, compiler_instance.getDiagnostics()) == false) {

    llvm::errs() << "failed to create CompilerInvocation\n";

    return 1;
  }

  std::unique_ptr<llvm::MemoryBuffer> memory_buffer = llvm::MemoryBuffer::getMemBufferCopy(source_code, "main.c");

  compiler_instance.getPreprocessorOpts().RetainRemappedFileBuffers = true;
  compiler_instance.getPreprocessorOpts().addRemappedFile("main.c", memory_buffer.get());

  clang::EmitLLVMOnlyAction emit_llvm_only_action;

  if (compiler_instance.ExecuteAction(emit_llvm_only_action) == false) {

    llvm::errs() << "clang compilation failed\n";

    return 1;
  }

  std::unique_ptr<llvm::Module> module = emit_llvm_only_action.takeModule();

  if (module == nullptr) {

    llvm::errs() << "clang produced no LLVM module\n";

    return 1;
  }

  llvm::Triple target_triple(llvm::sys::getDefaultTargetTriple());

  module->setTargetTriple(target_triple);

  std::string target_error;

  llvm::Target const *target = llvm::TargetRegistry::lookupTarget(target_triple, target_error);

  if (target == nullptr) {

    llvm::errs() << "failed to find target: " << target_error << "\n";

    return 1;
  }

  llvm::TargetOptions target_options;

  std::unique_ptr<llvm::TargetMachine> target_machine(target->createTargetMachine(
    target_triple,
    "generic",
    "",
    target_options,
    llvm::Reloc::PIC_,
    std::nullopt,
    llvm::CodeGenOptLevel::Default));

  if (target_machine == nullptr) {

    llvm::errs() << "failed to create TargetMachine\n";

    return 1;
  }

  module->setDataLayout(target_machine->createDataLayout());

  llvm::SmallVector<char, 0> object_data;
  llvm::raw_svector_ostream output(object_data);

  llvm::legacy::PassManager pass_manager;

  if (target_machine->addPassesToEmitFile(pass_manager, output, nullptr, llvm::CodeGenFileType::ObjectFile)) {

    llvm::errs() << "target cannot emit object files\n";

    return 1;
  }

  if (pass_manager.run(*module) == false) {

    llvm::errs() << "failed to generate object code\n";

    return 1;
  }

  uint64_t size = object_data.size();

  void *result = TI_ALLOC(size, 0, 0);

  if (result == nullptr) {

    llvm::errs() << "failed to allocate object buffer\n";

    return 1;
  }

  std::memcpy(result, object_data.data(), size);

  *buffer = result;
  *buffer_size = size;

  return 0;
}
uint8_t clang_load(void *buffer, uint64_t buffer_size) {
  if (s_jit == nullptr) {

    llvm::errs() << "JIT has not been created\n";

    return 1;
  }

  if ((buffer == nullptr) || (buffer_size == 0)) {

    llvm::errs() << "invalid object buffer\n";

    return 1;
  }

  std::unique_ptr<llvm::MemoryBuffer> object = llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef((char const *)buffer, buffer_size), "jit-object");

  if (llvm::Error error = s_jit->addObjectFile(std::move(object))) {

    llvm::errs() << "failed to add object file to JIT: " << llvm::toString(std::move(error)) << "\n";

    return 1;
  }

  return 0;
}
uint8_t clang_lookup(char const *symbol, void **function_ptr) {
  if (s_jit == nullptr) {

    llvm::errs() << "JIT has not been created\n";

    return 1;
  }

  llvm::orc::ExecutorAddr sym;

  if (llvm::Error error = s_jit->lookup(symbol).moveInto(sym)) {

    llvm::errs() << "failed to find symbol \"" << symbol << "\" : " << llvm::toString(std::move(error)) << "\n ";

    return 1;
  }

  *function_ptr = sym.toPtr<void *>();

  return 0;
}
void clang_destroy(void) {
  s_jit.reset();

  llvm::llvm_shutdown();
}
