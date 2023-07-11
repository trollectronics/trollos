#ifndef _INCLUDE_SYS_TYPES_H
#define _INCLUDE_SYS_TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef int64_t off_t;
typedef intptr_t ssize_t;

typedef uint32_t dev_t;
typedef uint32_t ino_t; // Maybe this should be 64-bit?
typedef uint16_t mode_t;
typedef uint16_t nlink_t;
typedef int32_t uid_t;
typedef int32_t gid_t;
typedef uint32_t blksize_t;
typedef uint64_t blkcnt_t;
typedef int32_t pid_t;

#define makedev(maj, min) ((dev_t) ((((maj) & 0xFFFF) << 16) | (min & 0xFFFF)))
#define minor(dev) ((dev) & 0xFFFF)
#define	major(dev) ((dev) >> 16)

#endif
