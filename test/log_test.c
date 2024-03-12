#include "../src/logging.h"

// compile with
// gcc test/log_test.c -DDEBUG

int main(int argc, char* argv[])
{
	LOG_CONSOLE("Starting the serial test program %s\n", argv[0]);
	LOG_ERROR("Oh no something went wrong!!!\n");
	LOG_DEBUG("This is just a debug message\n");
	LOG_INFO("Port opened!\n");

	return 0;
}
