# titan

```
git clone -b llvmorg-23.1.2 https://github.com/llvm/llvm-project

cmake --fresh
  -S ".\llvm-project\llvm"
  -B ".\llvm-project\build"
  -G "Visual Studio 18 2026"
  -A x64
  -DLLVM_ENABLE_PROJECTS="clang;lld"
  -DLLVM_BUILD_LLVM_C_DYLIB=OFF
  -DLLVM_ENABLE_RTTI=ON
  -DLLVM_INCLUDE_TESTS=OFF
  -DLLVM_INCLUDE_BENCHMARKS=OFF

cmake --build ".\llvm-project\build" --config Release --parallel 32
```
