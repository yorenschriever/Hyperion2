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

![](https://github.com/yorenschriever/Hyperion2/blob/main/docs/images/webcontroller.png?raw=true)

![](https://github.com/yorenschriever/Hyperion2/blob/main/docs/images/intallation-bosburcht.jpeg?raw=true)

![](https://github.com/yorenschriever/Hyperion2/blob/main/docs/images/hyperhyper.png?raw=true)

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
|[MacOs](docs/readme-macos.md) |yes                |yes          |yes             |yes                     |0                |0           |0         |$300+        |
|[Esp32 / Hyperion hardware](docs/readme-esp.md)|max +/- 1500 lights|limited*     |yes             |yes                     |8                |12          |1/1       |$5-$150      |
|[Raspberry pi 4](docs/readme-rpi.md)           |yes                |yes          |yes             |yes                     |0                |0           |0         |$50-$150     |
|[Any computer (docker)](docs/readme-docker.md)    |yes                |yes          |no              |yes                     |0                |0           |0         |$100+        |

- Memory on the ESP32 is limited, running the web interface does not leave much room for pattern data. It works for small setups, but is not recommended.
- DMX output on macos and raspberry pi is not present yet, but would be easy to add with the use of a dongle
- There is no documentation to run natively on windows yet, but it should be possible. (Nor on linux, but would not be hard to create using the docker and raspberry pi versions.)
- Running Docker is also the best option if you just want to get get a taste of the platform before you want to deal with build dependencies, hardware flashing and soldering. 

## Hardware configurations

As you can see above, each hardware platform has its own strengths and weaknesses. Which one is optimal depends on your installation. 
The hyper software can be split up in 2 parts: Pattern generation and Signal distribution. Both parts can run on the same platform, 
but it is also possible to distribute them, so each part can run on the platform that suits the purpose best.

Below are example configurations for 4 typical use cases. You can use these as inspiration, but the hardware configurations are flexible so you can make anything to want. Be creative.

### Simple standalone setup
![](https://github.com/yorenschriever/Hyperion2/blob/main/docs/images/setup-standalone.png?raw=true)

- Only one box. easy to carry, least points of potential failure
- Midi controller optional

### Standalone with controller in FOH
![](https://github.com/yorenschriever/Hyperion2/blob/main/docs/images/setup-split.png?raw=true)
- Control the show from the FOH, have the installation somewhere else in the venue (e.g. hanging from the ceiling)
- Hyper could also be Raspberry pi
- Potentially attach more hypers through a switch

### Advanced show, many patterns and layers
![](https://github.com/yorenschriever/Hyperion2/blob/main/docs/images/setup-laptop.png?raw=true)
- Supports large installations
- Have a laptop or raspberry pi do the heavy work
- Monitor the output on the screen
- Control via midi controller, or the web interface on laptop tablet or phone

### Writing patterns
![](https://github.com/yorenschriever/Hyperion2/blob/main/docs/images/setup-writing-patterns.png?raw=true)
- Work on patterns on your laptop or desktop without hyper-specific hardware
- A visualizer will show what the patterns will look like
- You can control the patterns through the web interface or a midi controller
- Your patterns will work on any platform, so you can use this setup to work on patterns for any installation
- You can work on your patterns when the installation is not available (e.g. it is storage or already at location)
- Fast build times: quickly iterate, be more creative, create better patterns

## Software

### Signal distribution

Signals are routed from *Inputs* to *Outputs* using a Pipe. ON each frame, *Inputs* provide the pixel data to the *Pipe*. The *Pipe* can transform the data and then sends it to the *Output*. The *Output* then sends out the data to hardware drivers.

Example Inputs are DMXInput, ArtnetInput or Inputs that generate pixel data from your patterns.

Example outputs are NeoPixelOutput, DMXOutput and PWMOutput

Your project can contain multiple pipes. Each pipe needs to have 1 *Input* and 1 *Output*. Optionally the pipe can apply color space conversion and apply a Lookup table (*LUT*). Color space conversion can be used to convert between different representations of color. For example from *RGB* to *BGR* or *RGB* to *Monochrome*. Lookup tables can be used to apply gamma correction, color correction, and correct for other non-linearities in the lamps you are driving. 

###  Pattern generation

In the [Examples folder](examples), you will find a couple of example projects to get you started. 
Simply create a copy somewhere on your disk and start tweaking it to your liking.

The [Writing patterns example](examples/writing_patterns/) it a tutorial that will explain how to write patterns step by step. 

### Pattern inputs

Some inputs will get their data from the hardware ports, but there are also inputs that generate the pixel data on the device. A simple example is the patternInput, which continuously calculates the pixel data of one pattern, and sends it to the pipe.

The most complex example in the ControlHubInput. This input takes multiple patterns, and layers them on top of each other. It listens to the control hub to determine which patterns are switched on and what their dim values and other parameters are.

### Control hub

The control hub is the integration points for show control. It is the single source of truth for the all control parameters. All controllers send their input to the hub, and all objects that need to know the status will ask the hub. 

Controllers can be physical midi controllers, but also the web controller. The webController gives good visual feedback of the current status. The webController can
we opened on the machine that the hyper code is running, but you can also access it over the network, eg on a phone or tablet.

## Make your own project

To create your own project you can create a copy of one of the examples. You can place this project anywhere you like. From there you can start adding pipes and patterns.
To build your project you type `build [platform]`, for example:

```sh
build macos
```

When the build is completed, you can run your project in the same way:

```sh
run macos
```

![](https://github.com/yorenschriever/Hyperion2/blob/main/docs/images/installation-wings.jpeg?raw=true)
![](https://github.com/yorenschriever/Hyperion2/blob/main/docs/images/installation-mini-ledster.jpeg?raw=true_)
