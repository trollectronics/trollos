#ifndef DIRENT_H_
#define	DIRENT_H_

#include <sys/types.h>
#include <syslimits.h>

struct dirent {
	ino_t		d_ino;
	mode_t		d_mode;

	char		d_name[NAME_MAX + 1];
};


#endif
