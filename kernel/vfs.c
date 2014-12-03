#if 0
#include "vfs.h"

struct Vfs vfs_state;



void vfs_init() {
	int i;

	for (i = 0; i < VFS_MOUNT_MAX; i++)
		vfs_state.mount[i].fs_type = VFS_FS_TYPE_INVALID;
	return;
}
#endif
