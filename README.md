# v4l2_framebuffer
Maps the frames from a camera to the display framebuffer.

To check the supported resolutions:
```sh
$ v4l2-ctl --list-formats-ext
```

By default, `640x480` is used. You can specify the resolution via commandline paramters. <br>
The first parameters will be the width, the second one will be the height.

To change your desired resolution, edit macro IM_WIDTH, IM_HEIGHT inside video_capture.h
