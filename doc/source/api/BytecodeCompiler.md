# BytecodeCompiler

继承自：[RefCounted](https://docs.godotengine.org/en/stable/classes/class_refcounted.html)

将 GDScript 源码 token 化并打包为可执行的字节码，供脚本在运行时以二进制形式保存或加载。


该类只负责「源码 → 字节码」的编译以及「字节码 → 压缩字节码」的打包，

并不负责执行字节码，编译结果可直接交给引擎的 GDScript 运行时消费。

## 方法：


返回值                                                                                           | 函数签名                                                                                                                                                                                                                          
--------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
[PackedByteArray](https://docs.godotengine.org/en/stable/classes/class_packedbytearray.html)  | [compile_from_script](#i_compile_from_script) ( [Script](https://docs.godotengine.org/en/stable/classes/class_script.html) source_script, [CompressionMode](BytecodeCompiler.md#enumerations) compression=UNCOMPRESSED (0) )  
[PackedByteArray](https://docs.godotengine.org/en/stable/classes/class_packedbytearray.html)  | [compile_from_string](#i_compile_from_string) ( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) source_code, [CompressionMode](BytecodeCompiler.md#enumerations) compression=UNCOMPRESSED (0) )    
[PackedByteArray](https://docs.godotengine.org/en/stable/classes/class_packedbytearray.html)  | [compress](#i_compress) ( [PackedByteArray](https://docs.godotengine.org/en/stable/classes/class_packedbytearray.html) bytecode )                                                                                             
<p></p>

## 枚举：<span id="enumerations"></span>

枚举 **CompressionMode**：

- <span id="i_UNCOMPRESSED"></span>**UNCOMPRESSED** = **0** --- 不压缩二进制 token，等价于导出选项「二进制 token（加载更快）」。
- <span id="i_COMPRESSED"></span>**COMPRESSED** = **1** --- 使用 Zstandard 压缩二进制 token，等价于导出选项「压缩二进制 token（文件更小）」。


## 方法描述

### [PackedByteArray](https://docs.godotengine.org/en/stable/classes/class_packedbytearray.html)<span id="i_compile_from_script"></span> **compile_from_script**( [Script](https://docs.godotengine.org/en/stable/classes/class_script.html) source_script, [CompressionMode](BytecodeCompiler.md#enumerations) compression=UNCOMPRESSED (0) ) 

从脚本对象读取其源码并生成字节码。


**p_script：** 待编译的脚本，仅接受具有源码的 GDScript，其它脚本类型会被拒绝

**p_compression：** 是否压缩生成的字节码，默认不压缩

**返回：** 生成的字节码；脚本为 null、非 GDScript 或无源码时返回空字节数组。

### [PackedByteArray](https://docs.godotengine.org/en/stable/classes/class_packedbytearray.html)<span id="i_compile_from_string"></span> **compile_from_string**( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) source_code, [CompressionMode](BytecodeCompiler.md#enumerations) compression=UNCOMPRESSED (0) ) 

将 GDScript 源码字符串 token 化并生成字节码。


**p_source_code：** 待编译的 GDScript 源码文本，空文本无法编译

**p_compression：** 是否压缩生成的字节码，默认不压缩

**返回：** 生成的字节码；源码为空或 token 化失败时返回空字节数组。

### [PackedByteArray](https://docs.godotengine.org/en/stable/classes/class_packedbytearray.html)<span id="i_compress"></span> **compress**( [PackedByteArray](https://docs.godotengine.org/en/stable/classes/class_packedbytearray.html) bytecode ) 

校验字节码头并对其负载进行 Zstandard 压缩。


**p_bytecode：** 未压缩的字节码

**返回：** 压缩后的字节码；校验失败时返回空数组，已是压缩态时原样返回。

_生成于 2026-09-12_
