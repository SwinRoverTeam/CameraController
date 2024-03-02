#ifndef VIDEO_DEVICES_H_
#define VIDEO_DEVICES_H_

typedef struct device_tree {
	char** device_name;
	int num_devices;
} device_tree_t;

void list_devices(device_tree_t* devices);
void clear_device_tree(device_tree_t* devices);

#endif // VIDEO_DEVICES_H_