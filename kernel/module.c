#include <stddef.h>
#include <module.h>
#include "util/string.h"
#include "util/log.h"

#define MODULES_MAX 32

static Module *_module_registered[MODULES_MAX];
static Module *_module_loaded[MODULES_MAX];

int module_register(Module *module) {
	int i;
	Module **free_spot = NULL;
	
	if(!module || !module->name)
		return -1;
	
	for(i = 0; i < MODULES_MAX; i++) {
		if(!_module_registered[i]) {
			free_spot = &_module_registered[i];
			continue;
		}
		if(!strcmp(module->name, _module_registered[i]->name)) {
			kprintf(LOG_LEVEL_ERROR, "Module '%s' is already registered in kernel!\n", module->name);
			return -1;
		}
	}
	
	if(!free_spot) {
		kprintf(LOG_LEVEL_ERROR, "Cannot register module, max modules is %i\n", MODULES_MAX);
		return -1;
	}
	
	*free_spot = module;
	kprintf(LOG_LEVEL_INFO, "Registered module %s\n", module->name);
	return 0;
}

bool module_is_loaded(const char *name) {
	int i;
	
	for(i = 0; i < MODULES_MAX; i++) {
		if(!_module_loaded[i]) {
			continue;
		}
		if(!strcmp(name, _module_loaded[i]->name)) {
			return true;
		}
	}
	
	return false;
}

int module_load(const char *name, int argc, char **argv) {
	int i;
	Module *module = NULL;
	Module **free_spot = NULL;
	
	if(!name)
		return -1;
	
	if(module_is_loaded(name)) {
		kprintf(LOG_LEVEL_WARNING, "Module '%s' is already loaded\n", name);
		return -1;
	}
	
	for(i = 0; i < MODULES_MAX; i++) {
		if(!_module_registered[i]) {
			continue;
		}
		if(!strcmp(module->name, _module_registered[i]->name)) {
			module = _module_registered[i];
			break;
		}
	}
	
	if(!module) {
		kprintf(LOG_LEVEL_ERROR, "Module '%s' is not registered in kernel\n", name);
		return -1;
	}
	
	if(module->depends) {
		for(i = 0; module->depends[i]; i++) {
			if(!module_is_loaded(module->depends[i])) {
				kprintf(LOG_LEVEL_ERROR, "Module '%s' is missing dependency '%s'\n", name, module->depends[i]);
				return -1;
			}
		}
	}
	
	for(i = 0; i < MODULES_MAX; i++) {
		if(!_module_loaded[i]) {
			free_spot = &_module_loaded[i];
			break;
		}
	}
	
	if(!free_spot) {
		kprintf(LOG_LEVEL_ERROR, "Cannot load module, max modules is %i\n", MODULES_MAX);
		return -1;
	}
	*free_spot = module;
	
	kprintf(LOG_LEVEL_DEBUG, "Loading module %s\n", name);
	if(module->init(argc, argv) < 0) {
		//TODO: properly deinit and unload module instead
		panic("Module failed to initialize");
	}
	kprintf(LOG_LEVEL_INFO, "Loaded module %s\n", name);
	
	return 0;
}
