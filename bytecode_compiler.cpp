#include "bytecode_compiler.h"

#include "environment.h"

#include "core/error/error_macros.h"
#include "core/io/compression.h"
#include "core/io/marshalls.h"
#include "core/object/script_language.h"
#include "modules/gdscript/gdscript_tokenizer_buffer.h"

void BytecodeCompiler::_bind_methods() {
	// 将三个编译/压缩方法注册到引擎，供 GDScript 直接调用。
	ClassDB::bind_method(D_METHOD("compile_from_string", "source_code", "compression"),
			&BytecodeCompiler::compile_from_string, DEFVAL(UNCOMPRESSED));
	ClassDB::bind_method(D_METHOD("compile_from_script", "source_script", "compression"),
			&BytecodeCompiler::compile_from_script, DEFVAL(UNCOMPRESSED));
	ClassDB::bind_method(D_METHOD("compress", "bytecode"), &BytecodeCompiler::compress);
	// 注册压缩方式枚举，使 GDScript 中可写 BytecodeCompiler.COMPRESSED 等常量。
	BIND_ENUM_CONSTANT(UNCOMPRESSED);
	BIND_ENUM_CONSTANT(COMPRESSED);
}

PackedByteArray BytecodeCompiler::compile_from_string(const String &p_source_code, CompressionMode p_compression) {
	// 空源码没有可 token 化的内容，直接返回空字节数组并输出错误提示。
	if (p_source_code.is_empty()) {
		ERR_PRINT("Source code can't be empty. The resulting PackedByteArray will be empty.");
		return PackedByteArray();
	}

	// 根据调用方选择的压缩方式，把枚举映射为 GDScript tokenizer 内置的压缩模式。
	GDScriptTokenizerBuffer::CompressMode compress_mode = p_compression == COMPRESSED
			? GDScriptTokenizerBuffer::COMPRESS_ZSTD
			: GDScriptTokenizerBuffer::COMPRESS_NONE;
	// 调用 GDScript tokenizer 把源码解析成二进制的 token 缓冲区，得到字节码。
	PackedByteArray bytes = GDScriptTokenizerBuffer::parse_code_string(p_source_code, compress_mode);

	if (bytes.is_empty()) {
		// token 化失败（通常是语法错误），调用方会拿到空数组，这里给出提示便于排查。
		ERR_PRINT("Bytecode compilation failed. The resulting PackedByteArray will be empty.");
	}
	return bytes;
}

PackedByteArray BytecodeCompiler::compile_from_script(const Script *p_script, CompressionMode p_compression) {
	// 拒绝空指针，避免对无效脚本做空引用解引用。
	if (p_script == nullptr) {
		ERR_PRINT("The script can't be null. The resulting PackedByteArray will be empty.");
		return PackedByteArray();
	}

	// 仅接受 GDScript：其它脚本类型没有可 token 化的源码字符串。
	// 同时要求脚本确实持有源码，例如仅以二进制形式加载的脚本可能不带源码。
	if (String(p_script->get_class()) != String("GDScript") || !p_script->has_source_code()) {
		ERR_PRINT("The provided script is not valid. The resulting PackedByteArray will be empty.");
		return PackedByteArray();
	}

	// 校验通过后，复用「源码字符串」的统一编译路径，避免重复实现。
	return compile_from_string(p_script->get_source_code(), p_compression);
}

PackedByteArray BytecodeCompiler::compress(const PackedByteArray &p_bytecode) {
	// 字节码头固定为 HEADER_SIZE（12 字节），更短的数据不可能是合法字节码，直接拒绝。
	if (p_bytecode.size() < static_cast<int>(HEADER_SIZE)) {
		ERR_PRINT("The bytecode is too small. The resulting PackedByteArray will be empty.");
		return PackedByteArray();
	}

	// 校验魔数（前 4 字节必须为 "GDSC"），避免把无关数据误当作字节码处理。
	if (p_bytecode[0] != 'G' || p_bytecode[1] != 'D' || p_bytecode[2] != 'S' || p_bytecode[3] != 'C') {
		ERR_PRINT("The bytecode seems to be invalid. The resulting PackedByteArray will be empty.");
		return PackedByteArray();
	}
	// 校验 tokenizer 版本（第 4~7 字节），不匹配说明字节码来自其它引擎版本，直接拒绝。
	if (decode_uint32(&p_bytecode[4]) != GDScriptTokenizerBuffer::TOKENIZER_VERSION) {
		ERR_PRINT("The bytecode was generated with a different engine version. The resulting PackedByteArray will be empty.");
		return PackedByteArray();
	}
	// 第 8~11 字节记录解压后的负载尺寸；若大于 0，说明该字节码已是压缩态，无需再次压缩。
	if (decode_uint32(&p_bytecode[8]) > 0) {
		WARN_PRINT("The bytecode is already compressed. Returned the same bytecode.");
		return p_bytecode;
	}

	// 分离固定头部与负载（token 二进制流），仅对负载部分做压缩。
	PackedByteArray contents = p_bytecode.slice(HEADER_SIZE);

	// 按 ZSTD 预估值分配最大缓冲区，写入后按实际压缩结果截断，避免缓冲区溢出。
	const int64_t max_size = Compression::get_max_compressed_buffer_size(contents.size(), Compression::MODE_ZSTD);
	Vector<uint8_t> compressed;
	compressed.resize(max_size);
	const int64_t compressed_size = Compression::compress(compressed.ptrw(), contents.ptr(), contents.size(), Compression::MODE_ZSTD);
	ERR_FAIL_COND_V_MSG(compressed_size < 0, PackedByteArray(), "Error compressing GDScript tokenizer buffer.");
	compressed.resize(compressed_size);

	// 重建字节码头：魔数与版本原样保留，负载尺寸字段写入「解压前的字节数」，便于解压时对回长度。
	PackedByteArray compressed_bytecode;
	compressed_bytecode.resize(HEADER_SIZE);
	uint8_t *p_header = compressed_bytecode.ptrw();
	encode_uint32(decode_uint32(&p_bytecode[0]), &p_header[0]);
	encode_uint32(decode_uint32(&p_bytecode[4]), &p_header[4]);
	encode_uint32(contents.size(), &p_header[8]);
	// 追加压缩后的负载，得到最终的压缩字节码。
	compressed_bytecode.append_array(compressed);
	return compressed_bytecode;
}