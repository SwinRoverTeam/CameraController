#include "../src/serial_ports.h"

// compile with
// gcc test/serial_test.c src/serial_ports.c

int main()
{
	int fd = open_port("/dev/ttyACM0", 9600, "8N1", 0);

	if (fd == 1) {
		printf("failed to read from port :(\n");
		close_port(fd);
		return 0;
	}

#define BUFFER_SIZE 12
	unsigned char buffer[BUFFER_SIZE];
	int err = read_port_blocking(fd, buffer, BUFFER_SIZE);

	if (err == -1) {
		printf("failed to read from port :(\n");
		close_port(fd);
		return 0;
	}

	// add null terminator
	buffer[err] = 0;

	printf("serial read: %s\n", buffer);

	return 0;
}