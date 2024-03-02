#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdio.h>

int open_port(char* dev_name, int baudrate_flag,
		const char* mode, int flowctrl);
int read_port(int port, unsigned char* buffer, int size);
int read_port_blocking(int port, unsigned char* buffer, int size);
int send_char_port(int port, unsigned char c);
int send_buffer_port(int port, unsigned char* buffer, int size);
void close_port(int port);
void send_break(int port, int us);

#endif // SERIAL_H_
