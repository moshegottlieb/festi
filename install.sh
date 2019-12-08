#!/bin/bash

# Exit on error
set -e

# Install dependencies
sudo apt-get -y update
sudo apt-get -y install libcurl4-openssl-dev pigpiod libpigpiod-if-dev nlohmann-json-dev libcctz-dev

# Build and install program
make
sudo make install

# Enable pigpio daemon (for interacting with the GPIO without root privs)
sudo systemctl enable pigpiod
# Start the pigpiod service
sudo service pigpiod start 
# 
getent passwd festi > /dev/null || sudo adduser --shell /bin/false --disabled-password --gecos "" festi # Create the user __festi__, this user will be running the program.  It is always better to create a per task user.
sudo cp festi.service /lib/systemd/system/ # Copy the systemd unit file so we could start it
sudo systemctl daemon-reload # Reload units
echo Get your free API key from https://darksky.net/dev , and find your location longitude and latitude, these can be extracted from google maps, just look for your address and copy it from the URL.
echo Ready? press enter to edit the configuration file.
read -n 1 -s -r -p "Press any key to continue"
echo ""
cp festi.json.sample /tmp/festi.edit.json
editor /tmp/festi.edit.json
read -n 1 -s -r -p "Ok? hit ctrl-c to break if not, otherwise, press any key to continue"
echo ""
sudo mkdir -p ~festi/.config
sudo chown festi ~festi/.config
sudo mv /tmp/festi.edit.json ~festi/.config/festi.json
sudo chown festi ~festi/.config/festi.json
sudo systemctl enable festi
sudo service festi start
echo "All done"



