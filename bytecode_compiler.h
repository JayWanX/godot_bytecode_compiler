#ifndef BYTECODE_COMPILER_H
#define BYTECODE_COMPILER_H

#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "core/variant/variant.h"

class Script;

/// 将 GDScript 源码 token 化并打包为可执行的字节码，供脚本在运行时以二进制形式保存或加载。
/// 该类只负责「源码 → 字节码」的编译以及「字节码 → 压缩字节码」的打包，
/// 并不负责执行字节码，编译结果可直接交给引擎的 GDScript 运行时消费。
class BytecodeCompiler : public RefCounted {
	GDCLASS(BytecodeCompiler, RefCounted);

public:
	/// 生成字节码时是否进行压缩
	enum CompressionMode {
		UNCOMPRESSED, ///< 不压缩，字节码体积较大但无需解压即可直接使用
		COMPRESSED, ///< 使用 Zstandard 压缩，减小体积但首次加载需要先解压
	};

	/// 将 GDScript 源码字符串 token 化并生成字节码。
	/// [param source_code] 待编译的 GDScript 源码文本，空文本无法编译
	/// [param compression] 是否压缩生成的字节码，默认不压缩
	/// [return] 生成的字节码；源码为空或 token 化失败时返回空字节数组。
	PackedByteArray compile_from_string(const String &p_source_code, CompressionMode p_compression = UNCOMPRESSED);

	/// 从脚本对象读取其源码并生成字节码。
	/// [param script] 待编译的脚本，仅接受具有源码的 GDScript，其它脚本类型会被拒绝
	/// [param compression] 是否压缩生成的字节码，默认不压缩
	/// [return] 生成的字节码；脚本为 null、非 GDScript 或无源码时返回空字节数组。
	PackedByteArray compile_from_script(const Script *p_script, CompressionMode p_compression = UNCOMPRESSED);

	/// 校验字节码头并对其负载进行 Zstandard 压缩。
	/// [param bytecode] 未压缩的字节码
	/// [return] 压缩后的字节码；校验失败时返回空数组，已是压缩态时原样返回。
	PackedByteArray compress(const PackedByteArray &p_bytecode);

protected:
	/// 注册本类的方法与枚举常量到引擎 ClassDB，使其可供 GDScript 调用。
	static void _bind_methods();
};

VARIANT_ENUM_CAST(BytecodeCompiler::CompressionMode);

#endif // BYTECODE_COMPILER_H
