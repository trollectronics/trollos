#ifndef __KERNEL_H__
#define __KERNEL_H__
#include <stdint.h>

#define MAX_TOTAL_FILES 1024

typedef uint32_t size_t;
typedef int32_t ssize_t;

typedef struct FileModuleMap FileModuleMap;
struct FileModuleMap {
	uint32_t major;
	uint32_t minor;
};

extern FileModuleMap filetable[MAX_TOTAL_FILES];

void panic(const char *message);

#endif
