#include "register_types.h"

#include "bytecode_compiler.h"

#include "core/object/class_db.h"

// 模块初始化入口，由引擎构建系统按初始化层级回调。
// GDScript 等基础阶段尚未就绪，故仅在 SCENE 阶段注册类，早于该阶段会因依赖未就绪而失败。
void initialize_bytecode_compiler_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	// 向 ClassDB 注册类，使 GDScript 中可以使用 BytecodeCompiler 类型。
	ClassDB::register_class<BytecodeCompiler>();
}

// 模块卸载入口：本模块不持有全局单例或静态资源，故无需清理，保留空实现以备未来扩展。
void uninitialize_bytecode_compiler_module(ModuleInitializationLevel p_level) {
	// 本模块无全局单例或静态资源需要释放。
}