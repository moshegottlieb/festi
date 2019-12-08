# festi
Festive lights for raspberry pi, led lights and GPIO

# Festive ligths on your Raspberry pi!

## Install

Installation is super easy! in just 6123 steps you will get a fully functional LED light strip automation for your raspberry pi!  
These instructions are for the official raspbian distribution.  

### Dependencies

1. `libcurl4-openssl-dev` (or some other `libcurl4-xxx-dev` option)
1. `pigpiod` and `libpigpiod-if-dev`
1. `nlohmann-json-dev`
1. `libcctz-dev`
`sudo apt-get install libcurl4-openssl-dev pigpiod libpigpiod-if-dev nlohmann-json-dev libcctz-dev`

Dependencies required for building:  

1. `g++`
1. `make`
1. `git`
`sudo apt-get install g++ make git`

## Build

Just run `make`.  
Your built product should be in `build/festi`  

## Install

### Before you begin

1. You will need an API key from [DarkSky](https://darksky.net/dev), it is used to fetch sunrise and sunset times every day.
1. You will also need your longitude and latitude (can be of your city too) for the DarkSky API so that your sunrise and sunset times are correct
1. Make sure your local timezone is correct, I _may_ fix this sometime (`sudo raspi-config` and follow instructions to set timezone) (update: working on this)

### Install script

You can use the provided install script to automate stuff for you.  
Try it with `./install.sh`  

### Connect PINs to LED strip

I used pin 6 for ground, but you can use any ground pin.  
The second pin is 24 (GPIO24).  
You can change that in `sun.cpp` (look for the CTOR and the number 24).  
I _may_ add this to the config.  
To view your GPIO pin layout, type: `pinout` or check out [https://pinout.xyz/](https://pinout.xyz/).

### Warning

My knowledge in electronics is about the same as Donald Trump's understanding of human rights.  
While I've been running this project at home for a while now, it may burn your raspberry pi, your LED lights, your balcony and start a hurricane in some remote place in the world.

### Why would anyone spend time on developing this project?

Well, I recently moved to Berlin, and by the looks of it, everyone and their dog has a LED light strip hanging out from their balconies.  
Wife said we had to get one, so I did.  
After about a dozen of chains bought on Amazon, she was satisfied with the color.  
Since I'm very very clever, I bought a strip with a solar charger - it charges during the day, and when it gets darks - it automagically turns on.  
I couldn't have been happier.  
Enter _autumn_.  
It appears there is no sun in Berlin for about 10 months a year.  
The lights faintly turned on around 16:00 and died away about 16:10.  
Mere mortals would be running to Amazon to get a new wall socket connected LED light strip.  
Me? nope.  
Time to learn what are these pins hanging out of my raspberry pi and put them to good use!
