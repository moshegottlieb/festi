# festi
Festive lights for raspberry pi, led lights and GPIO

# Festive ligths on your Raspberry pi!

## Install

These instructions are for the official raspbian distribution.  

### Dependencies

1. `libcurl4-openssl-dev` (or some other `libcurl4-xxx-dev` option)
1. `pigpiod` and `libpigpiod-if-dev`
`sudo apt-get install libcurl4-openssl-dev pigpiod libpigpiod-if-dev`

Dependencies required for building:  

1. `g++`
1. `make`
1. `git`
`sudo apt-get install g++ make git`

## Build

Just run `make`.  
Your built product should be in `build/festi`  

