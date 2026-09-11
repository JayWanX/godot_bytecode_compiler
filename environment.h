#pragma once

#include <cstdint>

/// 字节码文件头的固定字节数。
///
/// 文件头布局：魔数（"GDSC"）4 字节 + tokenizer 版本 4 字节 + 解压后负载尺寸 4 字节。
/// 读取或写入字节码头时统一使用该常量，避免魔数在代码中散落。
inline constexpr uint32_t HEADER_SIZE = 12;