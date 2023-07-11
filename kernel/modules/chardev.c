#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <device.h>
#include <errno.h>

#include <trollos/process.h>
#include <sys/types.h>
#include "../util/mem.h"


static int _queue_enqueue(DeviceIOQueue **queue, pid_t pid) {
	DeviceIOQueue *item = NULL;

	if (!(item = kmalloc(sizeof(DeviceIOQueue))))
		goto error;

	item->pid = pid;
	item->next = NULL;

	for (; *queue; queue = &(*queue)->next);
	*queue = item;

	return 0;

	error:
	return -ENOMEM;
}

static DeviceIOQueue *_queue_dequeue(DeviceIOQueue **queue) {
	DeviceIOQueue *item;

	if (*queue == NULL)
		return NULL;

	item = *queue;
	*queue = NULL;

	return item;
}

static int _read_callback(Device *dev) {
	DeviceIOQueue *item;
	Process *proc;

	if (!_queue_dequeue(&dev->chardev.queue_read))
		return 0;

	proc = process_from_pid(item->pid);
	proc->state = PROCESS_STATE_RUNNING;

	return 0;
}


ssize_t chardev_read(Device *dev, void *buf, size_t count) {
	int r;
	Process *proc;

	if (dev->type != DEVICE_TYPE_CHAR)
		return -ENOSYS;

	if (!dev->chardev.read)
		return -ENOSYS;

	if ((r = dev->chardev.read(buf, count)) == -EWOULDBLOCK) {
		pid_t current = process_current();

		if (_queue_enqueue(&dev->chardev.queue_read, current) < 0)
			return -ENOMEM;
			
		proc = process_from_pid(current);
		proc->state = PROCESS_STATE_BLOCKED;

		return r;
	}

	return r;
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
