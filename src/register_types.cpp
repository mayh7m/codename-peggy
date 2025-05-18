#include "../inc/register_types.h"

#include "../inc/level.h"
#include "../inc/peggy.h"

using namespace godot;

void initializeCodenamePeggy(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;

	ClassDB::register_class<Level>();
	ClassDB::register_class<Peggy>();
}

void uninitializeCodenamePeggy(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;
}

extern "C" {
	// initialization
	GDExtensionBool GDE_EXPORT codename_peggy_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
		godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

		init_obj.register_initializer(initializeCodenamePeggy);
		init_obj.register_terminator(uninitializeCodenamePeggy);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}