#include "isrofs.h"
#include "util.h"

int isrofs_detect(void *ptr) {
	unsigned int *iptr = ptr;

	return *iptr == ISROFS_MAGIC;
}


void *isrofs_locate_step(void *fsptr, struct IsrofsEntry *entry, char *path) {
	char name_buff[17];
	void *tmp;
	int i;

	for (i = 0; i < 16 && *path != '/' && *path; i++)
		name_buff[i] = *path, path++;
	if (*path == '/')
		path++;
	if (!i)
		return NULL;
	name_buff[i] = 0;
	for (i = 0; entry[i].type != ISROFS_TYPE_EOD; i++) {
		if (entry[i].type == ISROFS_TYPE_DIRECTORY) {
			if ((tmp = isrofs_locate_step(fsptr, fsptr + entry[i].pos, path)))
				return tmp;
		} else if (entry[i].type == ISROFS_TYPE_FILE)
			if (streq(entry[i].name, name_buff, 16))
				return &entry[i];
		/* TODO: Implement support for symlinks */
	}
	
	return NULL;
}


void *isrofs_locate(void *fsptr, char *path) {
	struct IsrofsMainStruct *fs;
	struct IsrofsEntry *entry;

	fs = fsptr;
	if (fs->magic != ISROFS_MAGIC)
		return NULL;
	entry = fsptr + sizeof(struct IsrofsMainStruct);
	if (*path == '/')
		path++;
	return isrofs_locate_step(fsptr, entry, path);
}


int isrofs_locate_file(void *fsptr, char *path, void **data, unsigned int *len) {
	struct IsrofsEntry *entry;

	if (!(entry = isrofs_locate(fsptr, path)))
		return 0;
	*data = fsptr + entry->pos;
	*len = entry->size;
	return 1;
}
