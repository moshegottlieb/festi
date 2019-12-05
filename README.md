# Work in progress, don't download this yet.  
# Still sorting it out for the general public

# festi
Festive lights for raspberry pi, led lights and GPIO

# Festive ligths on your Raspberry pi!

## Install

These instructions are for the official raspbian distribution.  

### Dependencies

1. `libcurl4-openssl-dev` (or some other `libcurl4-xxx-dev` option)
1. `pigpiod` and `libpigpiod-if-dev`
1. `nlohmann-json-dev`
`sudo apt-get install libcurl4-openssl-dev pigpiod libpigpiod-if-dev nlohmann-json-dev`

Dependencies required for building:  

1. `g++`
1. `make`
1. `git`
`sudo apt-get install g++ make git`

## Build

Just run `make`.  
Your built product should be in `build/festi`  
