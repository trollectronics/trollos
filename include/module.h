#ifndef INCLUDE_MODULE_H_
#define INCLUDE_MODULE_H_

#include <stdbool.h>

typedef struct Module Module;
struct Module {
	char name[32];
	const char *description;
	
	const char **depends;
	
	int (*init)(int argc, char **argv);
};

int module_register(Module *module);
int module_load(const char *name, int argc, char **argv);
bool module_is_loaded(const char *name);

#endif
