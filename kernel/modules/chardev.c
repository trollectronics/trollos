#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <device.h>
#include <errno.h>

#include <sys/types.h>

ssize_t chardev_read(Device *dev, void *buf, size_t count) {
	if (dev->type != DEVICE_TYPE_CHAR)
		return -ENOSYS;

	if (!dev->chardev.read)
		return -ENOSYS;

	return dev->chardev.read(buf, count);
}

ssize_t chardev_write(Device *dev, const void *buf, size_t count) {
	if (dev->type != DEVICE_TYPE_CHAR)
		return -ENOSYS;

	if (dev->chardev.write) {
		return dev->chardev.write(buf, count);
	} else {
		return -ENOSYS;
	}
}

ssize_t chardev_ioctl(Device *dev, unsigned long request, ...) {
	if (dev->type != DEVICE_TYPE_CHAR)
		return -ENOSYS;

	return 0;
}
