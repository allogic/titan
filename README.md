# Titan

A lightweight, custom Vulkan game engine built entirely in ANSI C, designed with minimal dependencies and full control over the rendering and engine architecture. It features direct Vulkan integration for efficient, low-level graphics rendering without relying on large external frameworks. Entity scripting is powered by LLVM/Clang integration, allowing C-based scripts to be compiled and integrated directly into the engine. The engine also includes a built-in GLSL compiler, providing a streamlined shader workflow without external shader compilation tools.

## Build LLVM

### Clone Repository
```sh
git clone -b llvmorg-23.1.2 https://github.com/llvm/llvm-project
```

### Build Libraries
```sh
cd llvm_project/clang

cmake --fresh
  -S "../llvm"
  -B "../build"
  -G "Visual Studio 18 2026"
  -A x64
  -DLLVM_ENABLE_PROJECTS="clang;lld"
  -DLLVM_BUILD_LLVM_C_DYLIB=OFF
  -DLLVM_ENABLE_RTTI=ON
  -DLLVM_INCLUDE_TESTS=OFF
  -DLLVM_INCLUDE_BENCHMARKS=OFF

cmake --build "./llvm-project/build" --config Release --parallel 8
```

### Missing Headers
```sh
cd llvm_project

find . -type f -name "*.inc"

src="build/tools/clang/include/clang"; dst="../titan/vendor/clang/include/clang"; find "$src" -type f -name '*.inc' -exec sh -c 'f="$1"; rel="${f#"$2"/}"; mkdir -p "$3/$(dirname "$rel")"; cp "$f" "$3/$rel"' _ {} "$src" "$dst" \;
src="build/include/llvm"; dst="../titan/vendor/clang/include/llvm"; find "$src" -type f -name '*.inc' -exec sh -c 'f="$1"; rel="${f#"$2"/}"; mkdir -p "$3/$(dirname "$rel")"; cp "$f" "$3/$rel"' _ {} "$src" "$dst" \;

cp build/include/llvm/Config/*.def ../titan/vendor/clang/include/llvm/Config/
cp build/include/llvm/Config/*.h ../titan/vendor/clang/include/llvm/Config/
```

## Pre Compiled Libraries

### GLSLang-16.6.0 x86-64 Windows Libraries
```
https://mega.nz/file/PdwWBaxY#3fnC1-AxyOqTLBYVYDhrZhb5hqr4b2LPA70FIlHc8SE
https://mega.nz/file/zcRkybJB#2nj2zRwO3ANY-hVFekGJWR6njWSBLBdgdnVLvjs6L3g
```

### LLVM-23.1.2 x86-64 Windows Libraries
```

```

## Static Resources

### Engine Fonts
```
https://mega.nz/file/vVAC1BLS#dAZ0NQQRiQLeNYcMzIQmEGmGGMfh1y3s4bnjK-s0u9M
```

### 3D Models
```
https://mega.nz/file/HFpywRAY#zvOuO1IXkv7bASosi-6llSB8Q-iyr3jbTx5lQI-bbjo
```