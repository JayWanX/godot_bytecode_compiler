# GD 字节码编译器（GDBC）

[![Godot Engine 4.x](https://img.shields.io/badge/Godot_Engine-4.x-blue)](https://godotengine.org/)

这是 Godot 引擎 [GDScript token 化器](https://github.com/godotengine/godot/tree/master/modules/gdscript) 的移植：直接复用引擎自带的 `GDScriptTokenizerBuffer`，把文本格式的 GDScript 源码编译为二进制 token。

核心思路是让二进制 token 化能力在编辑器或游戏运行中的任意位置被调用，从而在任意时刻把脚本源码编译成二进制 token。由于它作为内置模块随引擎编译，可无缝访问引擎内部实现，字节码格式与引擎导出的文件完全一致。

## 用法

模块会注册一个名为 `BytecodeCompiler` 的对象。实例化后可将一个 `GDScript` 对象或其源码直接编译为 `PackedByteArray` 字节码（等价于引擎导出的二进制 token）。也可以像导出选项那样用 zstd 压缩二进制 token：

- `BytecodeCompiler.UNCOMPRESSED` 等价于导出选项「二进制 token（加载更快）」。
- `BytecodeCompiler.COMPRESSED` 等价于导出选项「压缩二进制 token（文件更小）」。

默认情况下，编译输出的字节码不经过压缩。

从任意 GDScript 或其源码编译：

```gdscript
# 实例化编译器。
var compiler := BytecodeCompiler.new()

# 编译当前脚本对象。
var bytes := compiler.compile_from_script(get_script())

# 编译当前脚本对象的源码。
var script := get_script()
bytes = compiler.compile_from_string(script.source_code)

# 若希望在编译之后再进行压缩，而不是在编译时压缩。
bytes = compiler.compress(bytes)
```

## 构建（作为内置模块）

将本目录放入 Godot 引擎源码树的 `custom_modules` 目录，随引擎一起用 SCons 构建即可，无需先构建独立的 GDExtension 库：

```
scons platform=<windows/linuxbsd/macos/android/ios> target=<template_debug/template_release/editor> ...
```

模块被引擎检测需同时具备 `register_types.h`、`SCsub`、`config.py` 三个文件。

## 限制：反编译（Decompiling）

本模块只关注编译方向，假定脚本总在引擎内部使用，因此不需要反编译——引擎会在 `load()` / `preload()` 时自行解码/反编译，故解码相关的代码一并被省略。
