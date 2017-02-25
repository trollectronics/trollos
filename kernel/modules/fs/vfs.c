#include <syslimits.h>
#include <limits.h>
#include <errno.h>
#include <stddef.h>
#include "vfs.h"
#include "../../util/mem.h"
#include "../../util/string.h"

struct Vfs vfs_state;

void vfs_init() {
	int i;

	for (i = 0; i < VFS_MOUNT_MAX; i++)
		vfs_state.mount[i].fs_type = VFS_FS_TYPE_INVALID;
	return;
}


static int _merge_path(char *path_in1, char *path_in2, char path_out[PATH_MAX]) {
	int i, j, k;

	for (i = k = 0; path_in1[i] && k < PATH_MAX - 1; i++, k++)
		path_out[k] = path_in1[i];
	path_out[k++] = '/';
	for (j = 0; path_in2[i] && k < PATH_MAX - 1; j++, k++)
		path_out[k] = path_in2[j];
	path_out[k++] = 0;
	return (k < PATH_MAX);
}


static int _path_lookup(char path[PATH_MAX], struct VfsInode *ret) {
	char tmp_path[PATH_MAX], *save, *tok;
	struct VfsInode cur_inode;
	int i;

	path[PATH_MAX - 1] = 0;
	if (path[0] != '/') {
		if (!merge_path("", path, tmp_path))
			return -ENOENT;
		tmp_path[PATH_MAX - 1] = 0;
	}

	memcpy(tmp_path, path, PATH_MAX);
	path++;

	for (tok = strtok_r(tmp_path, "/", &save); tok; tok = strtok_r(NULL, "/", &save)) {
		for (i = 0; i < VFS_MOUNT_MAX; i++) {
			if (vfs_state.mount[i].fs_type == VFS_FS_TYPE_INVALID)
				continue;
			if (cur_inode.mount == vfs_state.mount[i].mounted_on.mount && cur_inode.inode == vfs_state.mount[i].mounted_on.inode) {
				cur_inode = vfs_state.mount[i].root;
				break;
			}
		}
		
		if ((cur_inode.inode = fs_inode_lookup(cur_inode.mount, cur_inode.context_id, cur_inode.inode, tok)) < 0)
			return cur_inode.inode;
	}

	*ret = cur_inode;
	return 0;
}


static int _file_type(char path[PATH_MAX]) {
	int i;
	int best_score = -1;
	int best_mount = -1;
	char *file_path;

	for (i = 0; i < VFS_MOUNT_MAX; i++) {
		if (vfs_state.mount[i].fs_type == VFS_FS_TYPE_INVALID)
			continue;
		else {
			/*tmp = _score_match(vfs_state.mount[i].mount_path, path, MOUNT_PATH_MAX);
			if (tmp > best_score)
				best_score = tmp, best_mount = i;*/
		}
	}
	
	if (best_mount < 0)
		return -ENOENT;
	
	if (best_score != INT_MAX)
		file_path = path + best_score;
	else // Perfect score == matches path exactly
		return S_IFDIR;
	/* TODO: Ask file system about file type */
	return -ENOENT;
}

/* Need to have user and group context in here somewhere */
int vfs_open(void *data, uint32_t flags) {
	char path[PATH_MAX];
	struct VfsInode inode;
	int tmp;
	
	memset(path, 0, PATH_MAX);
	memcpy_from_user(path, data, PATH_MAX - 1);

	if ((tmp = _path_lookup(path, &inode)) < 0)
		return tmp;
	
	/* TODO: Take permissions into account */


}
