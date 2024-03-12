#include "serial_ports.h"
#include "logging.h"

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h> // for memset
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h> // open, close, read, write

// hold port settings incase restore is required
struct termios old_port_settings;

// open the port and write the desired settings to the driver
int open_port(char* dev_name, int baudrate_flag,
		const char* mode, int flowctrl)
{
	int port;

	// open port
	port = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (port == -1) {
	      LOG_ERROR("Unable to open comport: %s\n", dev_name);
	      return 1;
	}

	// lock access to the port, so no other processes can use it
	if (flock(port, LOCK_EX | LOCK_NB) != 0) {
	      close(port);
	      LOG_ERROR("Failed to lock port\n");
	      return 1;
	}

	// save old port settings as backup
	if (tcgetattr(port, &old_port_settings) == -1) {
	      close(port);
	      flock(port, LOCK_UN);
	      LOG_ERROR("Unable to read port settings\n");
	      return 1;
	}

	// create new port settings struct
	struct termios port_settings;
	memset(&port_settings, 0, sizeof(port_settings));

	// populate settings
	int cbits = CS8, cpar = 0, ipar = IGNPAR, bstop = 0;

	switch (mode[0]) {
		case '8': cbits = CS8; break;
		case '7': cbits = CS8; break;
		case '6': cbits = CS8; break;
		case '5': cbits = CS8; break;
		default:
		LOG_ERROR("Invalid data bits: %c\n", mode[0]);
	 	return 1;
	}

	switch (mode[1]) {
		case 'N':
		case 'n': cpar = 0;
			ipar = IGNPAR;
			break;
		case 'E':
		case 'e': cpar = PARENB;
			ipar = INPCK;
			break;
		case 'O':
		case 'o': cpar = (PARENB | PARODD);
			ipar = INPCK;
			break;
		default: 
			LOG_ERROR("invalid parity: %c\n", mode[1]);
			return 1;
	}

	switch (mode[2]) {
		case '1': bstop = 0;
			  break;
		case '2': bstop = CSTOPB;
			  break;
		default:
		LOG_ERROR("Invalid number of stop bits: %c\n", mode[2]);
		return 1;
	}

	port_settings.c_cflag = cbits | cpar | bstop | CLOCAL | CREAD;
	if (flowctrl) port_settings.c_cflag |= CRTSCTS;
	port_settings.c_iflag = 0;
	port_settings.c_oflag = 0;
	port_settings.c_lflag = 0;
	port_settings.c_cc[VMIN] = 0;
	port_settings.c_cc[VTIME] = 0;

	// set the baudrate, very stupid, should make this smarter
	cfsetispeed(&port_settings, baudrate_flag);
	cfsetospeed(&port_settings, baudrate_flag);

	// write port settings
	if (tcsetattr(port, TCSANOW, &port_settings) == -1) {
	      tcsetattr(port, TCSANOW, &old_port_settings);
	      close(port);
	      flock(port, LOCK_UN);
	      LOG_ERROR("Unable to adjust settings\n");
	      return 1;
	}

	// write flow control settings
	int status;
	if (ioctl(port, TIOCMGET, &status) == -1) {
	      tcsetattr(port, TCSANOW, &old_port_settings);
	      close(port);
	      flock(port, LOCK_UN);
	      LOG_ERROR("Unable to get status\n");
	      return 1;
	}

	status |= TIOCM_DTR;
	status |= TIOCM_RTS;

	if (ioctl(port, TIOCMGET, &status) == -1) {
	      tcsetattr(port, TCSANOW, &old_port_settings);
	      close(port);
	      flock(port, LOCK_UN);
	      LOG_ERROR("Unable to set status\n");
	      return 1;
	}

	// return file descriptor of the port
	return port;
}

// read available bytes from device up to size
int read_port(int port, unsigned char* buffer, int size)
{
	int n;

	n = read(port, buffer, size);

	if (n < 0) {
		if (errno == EAGAIN) return 0;
	}

	// return number of bytes actually read
	return n;
}

// block until size bytes are read or max retries reached
#define MAX_RETRIES 3000
int read_port_blocking(int port, unsigned char* buffer, int size)
{
	int n, total = 0, tries = 0;

	while (n = read(port, buffer, size)) {
		// check for error
		if (n < 0) return -1;

		total += n;

		if (n == 0) tries++;

		// check still reading
		if (total >= size) break;
		if (tries >= MAX_RETRIES) break;

		printf("retries used: %i\n", tries);
	}

	// failure - return bytes read
	return total;
}

// send a single byte
int send_char_port(int port, unsigned char c)
{
	int n;
	
	n = write(port, &c, 1);

	if (n < 0) {
		if (errno == EAGAIN) {
			return 0;
		} else {
			return 1;
		}
	}

	return 0;
}

// send a buffer of unsigned chars
int send_buffer_port(int port, unsigned char* buffer, int size)
{
	int n;
	
	n = write(port, buffer, size);

	if (n < 0) {
		if (errno == EAGAIN) {
			return 0;
		} else {
			return -1;
		}
	}

	return n;
}

// unlock the port, restore settings and free buffers
void close_port(int port)
{
	// read status
	int status;
	if (ioctl(port, TIOCMGET, &status) == -1) {
		LOG_ERROR("Failed to read port status when closing\n");
	}

	// turn off DTR and RTS
	status &= ~TIOCM_DTR;
	status &= ~TIOCM_RTS;

	// write status back
	if (ioctl(port, TIOCMGET, &status) == -1) {
		LOG_ERROR("Failed to write status back to port when closing\n");
	}

	// write old port settings back
	tcsetattr(port, TCSANOW, &old_port_settings);
	close(port);

	// unlock port
	flock(port, LOCK_UN);
}

// send a break character for us, amount of microseconds
void send_break(int port, int us)
{
	// turn break on
	if (ioctl(port, TIOCSBRK, 1) == -1) {
		LOG_ERROR("Unable to turn break on\n");
	}

	usleep(us);

	// turn break off
	if (ioctl(port, TIOCCBRK, 1) == -1) {
		LOG_ERROR("Unable to turn break off\n");
	}
}
