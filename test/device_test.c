#include "../src/video_capture.h"

#include <stdio.h>
#include <stdlib.h>

// compile with 
// gcc test/device_test.c src/video_capture.c -lc

int main()
{
        device_info_t d;
        int len;
        len = list_video_devices(&d);

        if (len == 0) {
                printf("no devices found :(\n");
                return 0;
        }
        return 0;
}