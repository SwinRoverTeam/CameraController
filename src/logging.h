#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdio.h>

// file name and location
#define LOG_FILE_NAME "LOG_" __DATE__

#ifndef FILE_PATH
	#define FILE_PATH "/home/eamon/src/CameraController/logs"
#endif

// log just to the terminal - mostly for testing logs
#define LOG_CONSOLE(args...) \
	printf(args) \

// log to a file
#define LOG_FILE(args...) \
{ \
	FILE* fp = NULL; \
        fp = fopen(FILE_PATH "/" LOG_FILE_NAME, "a"); \
	if (fp) { \
		fseek(fp, 0, SEEK_END); \
		fprintf(fp, "<%s:%s:%i> ", __FILE__, __func__, __LINE__); \
		fprintf(fp, args); \
		fclose(fp); \
	} \
}

// log to file and stdout, for status updates
#define LOG_INFO(args...) \
	LOG_FILE(args); \
	printf("<%s:%s:%i> ", __FILE__, __func__, __LINE__); \
	printf(args)

// log to file and stderr, if something goes really wrong
#define LOG_ERROR(args...) \
	LOG_FILE(args); \
	fprintf(stderr, "<%s:%s:%i> ", __FILE__, __func__, __LINE__); \
	fprintf(stderr, args)

// if debug flag not set, set it to false
#ifndef DEBUG
	#define DEBUG 0
#endif

// log to file and stdout, only displays in debug build, use for testing
#define LOG_DEBUG(args...) \
	if (DEBUG) { \
		LOG_INFO(args); \
	}

#endif //LOGGING_H_
