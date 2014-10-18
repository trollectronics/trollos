#ifndef __ISROFS_H__
#define	__ISROFS_H__

/* Incompatible Simple Read Only File System */

#define	ISROFS_MAGIC		0x71BE995F

#define	ISROFS_TYPE_EOD		0x0
#define	ISROFS_TYPE_DIRECTORY	0x1
#define	ISROFS_TYPE_FILE	0x2
#define	ISROFS_TYPE_SYMLINK	0x3
#define	ISROFS_TYPE_ZFILE	0x4	/* ZFILE = zlib compressed */

struct IsrofsMainStruct {
	unsigned int		magic;
	unsigned short		files;
} __attribute__ ((__packed__));

struct IsrofsEntry {
	char			name[16];
	unsigned short		permissions;
	unsigned short		owner;
	unsigned short		group;
	unsigned short		type;
	long long		last_modify;
	unsigned int		size;
	unsigned int		pos;
} __attribute__ ((__packed__));


#endif
