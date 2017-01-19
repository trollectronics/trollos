#include <stdint.h>
#include <stddef.h>

#include <errno.h>

static int fs_list_len;

struct FSList {
	char		*name;
	int (*init)();

	int64_t (*inode_lookup)(int context_id, int64_t directory, const char *tok);
};


static struct FSList fs_list[] = {
	{NULL, NULL, NULL},
};


void fs_init() {
	int i;

	for (i = 0; fs_list[i].name; i++)
		fs_list[i].init();
	fs_list_len = i;
}


int64_t fs_inode_lookup(int active_fs, int context, int64_t inode, const char *name) {
	if (active_fs < fs_list_len)
		return -EINVAL;
	if (active_fs < 0)
		return -EINVAL;
	return fs_list[active_fs].inode_lookup(context, inode, name);
}

