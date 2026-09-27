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
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

void ti_clang_create(void) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
}

void ti_clang_compile(char const *source_code) {
  std::string compiler_source = "extern \"C\" int add(int, int);\n#line 1 \"main.cpp\"\n";
  compiler_source += source_code;

  std::unique_ptr<llvm::MemoryBuffer> memory_buffer = llvm::MemoryBuffer::getMemBufferCopy(compiler_source, "main.cpp");

  clang::CompilerInstance compiler_instance;

  compiler_instance.createVirtualFileSystem();
  compiler_instance.createDiagnostics();

  char const *compiler_args[] = {
    "-std=c++20",
    "-xc++",
    "main.cpp",
  };

  if (clang::CompilerInvocation::CreateFromArgs(compiler_instance.getInvocation(), compiler_args, compiler_instance.getDiagnostics()) == false) {
    return;
  }

  compiler_instance.getPreprocessorOpts().RetainRemappedFileBuffers = true;
  compiler_instance.getPreprocessorOpts().addRemappedFile("main.cpp", memory_buffer.get());

  clang::EmitLLVMOnlyAction emit_llvm_only_action;

  if (compiler_instance.ExecuteAction(emit_llvm_only_action) == false) {
    return;
  }

  std::unique_ptr<llvm::Module> module = emit_llvm_only_action.takeModule();

  if (module == 0) {
    return;
  }

  module->print(llvm::outs(), 0);

  llvm::orc::LLJITBuilder jit_builder;
  jit_builder.setDataLayout(module->getDataLayout());

  std::unique_ptr<llvm::orc::LLJIT> jit;

  if (llvm::Error error = jit_builder.create().moveInto(jit)) {
    llvm::errs() << llvm::toString(std::move(error)) << "\n";
    return;
  }

  llvm::orc::ThreadSafeModule thread_safe_module(
    std::move(module),
    std::unique_ptr<llvm::LLVMContext>(emit_llvm_only_action.takeLLVMContext()));

  if (llvm::Error error = jit->addIRModule(std::move(thread_safe_module))) {
    llvm::errs() << llvm::toString(std::move(error)) << "\n";
    return;
  }

  llvm::orc::ExecutorAddr symbol;

  if (llvm::Error error = jit->lookup("add").moveInto(symbol)) {
    llvm::errs() << llvm::toString(std::move(error)) << "\n";
    return;
  }

  int (*add_proc)(int, int) = symbol.toPtr<int (*)(int, int)>();

  add_proc(100, 42);
}

void ti_clang_destroy(void) {
  llvm::llvm_shutdown();
}
