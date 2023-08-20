# Hyperion 2

## Purpose

Hyperion is an open source soft- and hardware solution for light shows, art installations, and lighting at festivals and clubs. 
It is capable of generating patterns and distributing it to the lights in the venue. 
It can control DMX fixtures, addressable leds like Neopixels, or output raw PWM signals.
Hyperion takes care of all the boring work - that is the same anyway in all installations - , so you can focus on the creative part. 
It does this by providing re-usable and multi purpose hardware modules and a software library that contains all the code to drive the lights.
Hyperion is written as a library, not as a platform, so you are still in control of your project. 
You write the creative part of your installation, but you turn to the hyperion library when you get to the repetitive part. 

Each installation is different, and so are its patterns. You get all the freedom to write them the way you want, but hyperion
also provides a set of helper functions to quickly create effects likes fades and LFOs or sync to the beat. 
You don't have to use them, but if you do, your pattern code will be clean and easily portable to new installations.

![](https://github.com/yorenschriever/Hyperion2/blob/e7e0d0a4356ed15b9a02b37b12cf11b7ddc1bfb7/readme-images/webcontroller.png)

TODO add more example photos of installations and also control software and hardware setups

## Target audience

The target audience is people with a programming background, and ideally also one in electronics. 
Hyperion's purpose is to give artists that would normally create an installation from scratch a set of tools to quickly set up new projects.
You will need programming skills to use it. 
To give you the maximum creative freedom, the patterns are also written in code instead of some kind of editor. 

## Getting started / installation guides

Hyperion runs on multiple platforms. Choose the one that fits your purpose best. Also, have a look at the chapter *hardware configurations* to help you with the choice.
Click on the platform name to go to the installation guide.

|Platform                 |Generate patterns  |Web interface|Midi controllers|Send and receive network|Neopixel channels|PWM channels|DMX out/in|Hardware cost|
|--                       |--                 |--           |--              |--                      |--               |--          |--        |--           |
|[MacOs](readme-macos.md) |yes                |yes          |yes             |yes                     |0                |0           |0         |$300+        |
|[Esp32 / Hyperion hardware](readme-esp.md)|max +/- 1500 lights|limited*     |yes             |yes                     |8                |12          |1/1       |$5-$150      |
|[Raspberry pi 4](readme-rpi.md)           |yes                |yes          |yes             |yes                     |0                |0           |0         |$50-$150     |
|[Any computer (docker)](readme-docker.md)    |yes                |yes          |no              |yes                     |0                |0           |0         |$100+        |

- Memory on the ESP32 is limited, running the web interface does not leave much room for pattern data. It works for small setups, but is not recommended.
- DMX output on macos and raspberry pi is not present yet, but would be easy to add with the use of a dongle
- There is no documentation to run natively on windows yet, but it should be possible. (Nor on linux, but would not be hard to create using the docker and raspberry pi versions.)
- Running Docker is also the best option if you just want to get get a taste of the platform before you want to deal with build dependencies, hardware flashing and soldering. 

## hardware configurations

Below are example configurations for 4 typical use cases. You can use these as inspiration, but the hardware configurations are flexible so you can make anything to want. Be creative.

TODO write out tex for the example, add diagrams.

### Simple standalone setup
- only one box. easy to carry, least points of potential failure
- midi optional

### Standalone with controller in FOH
- control the show from the FOH, have the installation somewhere else in the venue, eg hanging from the ceiling
- hyper could also be RPI
- potentially attach more hypers through a switch

### Advanced show, many patterns and layers
- supports large installations
- have a laptop or raspberry pi do the heavy work
- monitor the output on the screen
- control via midi, or web interface to use laptop tablet or phone

### Writing patterns
- works on any machine
- the same project will compile on any platform, so you can use this to work on patterns for an installation with another hardware setup
- use webcontroller and monitor to see what you are doing without needing the installation (eg is storage or already at location)
- fast build times: quickly iterate, be more creative, create better patterns

## writing patterns
- link to tutorial 


## pipes, inputs and outputs?
- copy from hyperion1?

## architecture diagram of generation

### controller
- central controller
- most important controllers: midi and web

### patterns

### params

### palettes and gradients

