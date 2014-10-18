/* Minimal ROMFS driver by Steven Arnow <s@rdw.se>, 2014 */

#include "romfs.h"
#include "util.h"
#include "boot_term.h"

int romfs_detect(void *ptr) {
	struct RomfsMainStruct *rfs = ptr;

	return (rfs->magic1 == ROMFS_MAGIC1 && rfs->magic2 == ROMFS_MAGIC2);
}


struct RomfsFileDescriptor romfs_locate_step(void *fsptr, struct RomfsFileEntry *entry, char *path) {
	char name_buff[256];
	int i, j;
	struct RomfsFileDescriptor desc;

	desc.filename = NULL;

	while (*path == '/')
		path++;
	for (i = 0; i < 255 && *path != '/' && *path; i++)
		name_buff[i] = *path, path++;
	if (*path == '/')
		path++;
	if (!i)		/* No node name */
		return desc;
	for (;;) {
		if (streq(name_buff, ((void *) entry) + sizeof(*entry), 255)) {
			if ((entry->next_fileheader & 0xF) == 2) {
				if (*path) {
					term_puts(path, 15);
					return desc;
				} else {
					desc.filename = ((void *) entry) + sizeof(*entry);
					desc.size = entry->size;
					j = strlen(desc.filename);
					if (j & 0xF)
						j += 0x10;
					j &= (~0xF);
					desc.data = ((void *) entry) + j + sizeof(*entry);
					return desc;
				}
			} else if ((entry->next_fileheader & 0xF) == 1)
				return romfs_locate_step(fsptr, fsptr + entry->special_info, path);
			else {
				term_puts("Bad filetype\n", 12);
				return desc;
			}
		}

		term_puts("Skipping file ", 15);
		term_puts(((void *) entry) + sizeof(*entry), 15);
		term_puts("\n", 15);

		if (!(entry->next_fileheader & (~0xF))) {
			term_puts("End of list\n", 15);
			return desc;
		}
		entry = fsptr + (entry->next_fileheader & (~0xF));
	}

	return desc;
}


struct RomfsFileDescriptor romfs_locate(void *ptr, char *fname) {
	struct RomfsFileDescriptor desc;
	void *vol_name;
	int i;

	desc.filename = NULL;
	if (!romfs_detect(ptr))
		return desc;
	vol_name = ptr + sizeof(struct RomfsMainStruct);
	i = strlen(vol_name);
	if (i & 0xF)
		i += 0x10;
	i &= (~0xF);
	return romfs_locate_step(ptr, vol_name + i, fname);
}
