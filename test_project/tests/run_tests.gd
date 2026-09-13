extends Node
## BytecodeCompiler 模块冒烟测试：验证 GDScript 源码按钮化与字节码压缩。

## 用于编译的合法 GDScript 源码。
const VALID_SOURCE: String = "func answer():\n\treturn 42\n"
## 字节码固定魔数 "GDSC"。
const GDSC_HEADER: PackedByteArray = [71, 68, 83, 67]

func _ready() -> void:
	var runner := TestRunner.new()
	_test_compile_from_string(runner)
	_test_compile_from_script(runner)
	_test_compress(runner)
	runner.report()
	var exit_code: int = 0 if runner.is_pass() else 1
	get_tree().quit(exit_code)

## 测试从字符串编译。
func _test_compile_from_string(runner: TestRunner) -> void:
	var compiler := BytecodeCompiler.new()
	var bytecode: PackedByteArray = compiler.compile_from_string(VALID_SOURCE)
	runner.assert_false(bytecode.is_empty(), "有效源码应编译出非空字节码")
	runner.assert_true(bytecode.size() >= 12, "字节码长度不应小于 12 字节头部")
	runner.assert_eq(bytecode.slice(0, 4), GDSC_HEADER, "字节码头应为 GDSC 魔数")
	runner.assert_true(compiler.compile_from_string("").is_empty(), "空源码应返回空字节码")

## 测试从脚本对象编译。
func _test_compile_from_script(runner: TestRunner) -> void:
	var compiler := BytecodeCompiler.new()
	runner.assert_false(compiler.compile_from_script(get_script()).is_empty(), "从当前脚本编译应得到非空字节码")
	runner.assert_true(compiler.compile_from_script(null).is_empty(), "null 脚本应返回空字节码")

## 测试字节码压缩路径。
func _test_compress(runner: TestRunner) -> void:
	var compiler := BytecodeCompiler.new()
	var bytecode: PackedByteArray = compiler.compile_from_string(VALID_SOURCE)
	runner.assert_eq(bytecode.slice(0, 4), GDSC_HEADER, "编译得到的字节码头应为 GDSC")
	var compressed: PackedByteArray = compiler.compress(bytecode)
	runner.assert_eq(compressed.slice(0, 4), GDSC_HEADER, "压缩后仍应保留 GDSC 魔数")
	runner.assert_true(compressed.size() < bytecode.size(), "压缩后体积应小于原始字节码")
	runner.assert_eq(compiler.compress(compressed), compressed, "已压缩字节码再次压缩应原样返回")
	var direct_compressed: PackedByteArray = compiler.compile_from_string(VALID_SOURCE, BytecodeCompiler.COMPRESSED)
	runner.assert_true(direct_compressed.size() < bytecode.size(), "COMPRESSED 模式体积应小于默认模式")