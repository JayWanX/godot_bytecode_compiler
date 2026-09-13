class_name TestRunner
extends RefCounted
## 轻量级测试运行器：收集断言结果并输出汇总。

var _passed_count: int = 0
var _failed_count: int = 0
var _failures: Array[String] = []

## 断言条件成立。
## [param condition] 待断言的条件。
## [param message] 失败时记录的错误提示。
func assert_true(condition: bool, message: String) -> void:
	if condition:
		_passed_count += 1
	else:
		_failed_count += 1
		_failures.append(message)

## 断言条件不成立。
## [param condition] 待断言的条件。
## [param message] 失败时记录的错误提示。
func assert_false(condition: bool, message: String) -> void:
	assert_true(not condition, message)

## 断言两值相等。
## [param actual] 实际值。
## [param expected] 期望值。
## [param message] 失败时记录的错误提示。
func assert_eq(actual: Variant, expected: Variant, message: String) -> void:
	assert_true(actual == expected, "%s（期望 %s，实际 %s）" % [message, str(expected), str(actual)])

## 断言字典键集合与期望一致。
## [param actual] 实际字典。
## [param expected_keys] 期望的键集合。
## [param message] 失败时记录的错误提示。
func assert_keys(actual: Dictionary, expected_keys: Array[String], message: String) -> void:
	var actual_keys: Array = actual.keys()
	actual_keys.sort()
	var sorted_expected: Array = Array(expected_keys.duplicate())
	sorted_expected.sort()
	assert_eq(Array(actual_keys), Array(sorted_expected), message)

## 断言字符串包含指定子串。
## [param haystack] 被搜索的字符串。
## [param needle] 期望包含的子串。
## [param message] 失败时记录的错误提示。
func assert_contains(haystack: String, needle: String, message: String) -> void:
	assert_true(haystack.contains(needle), "%s（期望包含 %s，实际：%s）" % [message, needle, haystack])

## 输出测试汇总到控制台。
func report() -> void:
	if _failed_count == 0:
		print("  通过 %d 项断言，全部通过！" % _passed_count)
	else:
		print("  通过 %d 项断言，失败 %d 项：" % [_passed_count, _failed_count])
		for failure: String in _failures:
			print("    - " + failure)

## 是否全部通过。
## [return] 无失败断言则返回 true。
func is_pass() -> bool:
	return _failed_count == 0
