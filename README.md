# Camera Controller
Displays selected v4l2 camera feeds based on mavlink commands
recieved over serial connection from the cubepilot

commands:
- list cameras
- change camera
- start stream
- end stream
- reset
- stack stream (left, right, top, bottom)

Additional functions:
- Produce a streamable SLAM map
- Send map/navigation data through mavlink for the cubepilot to process
