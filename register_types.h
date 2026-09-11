#ifndef BYTECODE_COMPILER_REGISTER_TYPES_H
#define BYTECODE_COMPILER_REGISTER_TYPES_H

#include "modules/register_module_types.h"

// 模块初始化入口：在 SCENE 阶段向 ClassDB 注册 BytecodeCompiler 类。
void initialize_bytecode_compiler_module(ModuleInitializationLevel p_level);
// 模块卸载入口：释放模块持有的全局资源（本模块暂无需释放）。
void uninitialize_bytecode_compiler_module(ModuleInitializationLevel p_level);

#endif // BYTECODE_COMPILER_REGISTER_TYPES_H