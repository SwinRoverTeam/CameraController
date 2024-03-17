#!/bin/bash

export DISPLAY=:0
# Define the program name and command to start it
PROGRAM="cam_ctrl"
DIR="$( cd "$( dirname "$0" )" && pwd )"

# Function to check if the program is running
is_running() {
    pgrep -x "$PROGRAM" >/dev/null
}

# Function to start the program
start_program() {
    "$DIR/$PROGRAM"
}

# Main loop to check and restart the program if it crashes
while true; do
    if ! is_running; then
        echo "Restarting $PROGRAM..."
        start_program
        sleep 1  # Optional delay before checking again
    fi
    sleep 1  # Adjust the sleep time according to your needs
done
