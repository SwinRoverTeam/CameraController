# CameraController
- listen port 61000 for cmds
- send back information
- access the video and device drivers directly for speed

commands:
- list -  available uvc devices
- connect - all devices
- patch - a video stream to an HDMI output
- disconnect - a video device to reset it
- exit - cleanly stop

desc:
#
Treat this program like a linux daemon, run from a script so that we're notified if it fails and can restart it.
