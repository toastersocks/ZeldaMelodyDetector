#!/bin/bash
# This script will start Pd in nogui
# mode and will open the patch
# named "noteDetector-sigmund~.pd"

echo "Starting Pd..."
pd -nogui -send "serialport open 0" -open /home/pi/Desktop/noteDetector-sigmund~.pd &