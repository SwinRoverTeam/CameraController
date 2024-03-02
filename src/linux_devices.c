#include "linux_devices.h"

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#define DEVICE_DIR "/dev"

void list_video_devices(device_tree_t* devices, const char* search_prefix)
{
	DIR* d;
	struct dirent *dev;
	d = opendir(DEVICE_DIR);

	// error but don't crash, allow the user to try again
	if (!d) {
		LOG_ERROR("Failed to find device directory %s\n"
			"Something very wrong\n", DEVICE_DIR);
		return;
	}

	int index = 0;
	while ((dev= readdir(d)) != NULL) {
		if (strncmp(dev->d_name, search_prefix, 4) == 0) {
			// allocate space for name
			devices->device_name = (char*) malloc(sizeof(char) * 10);

			// copy name into device tree
			sprintf(devices->device_name[index], "%s/%s\n", DEVICE_DIR, dev->d_name);
			index++;
		}
	}

	closedir(d);
}

clear_device_tree(device_tree_t* devices)
{
	if (devices == NULL) {
		LOG_ERROR("Cannot clear empty video device list\n");
		return;
	}

	for (size_t i = 0; i < devices->num_devices; i++) {
		free(devices->device_name);
		devices->device_name = NULL;
	}
}