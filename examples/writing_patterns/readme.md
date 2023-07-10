# Example: making patterns

This project will explain how to make patterns. You should start this project by running:

```
cd examples/writing_patterns
build docker && run docker
```

Then, view the results in your browser: [http://localhost:4430/].
Here you will se a controller and a monitor. 
The monitor shows a ring of leds that represent the installation. 
This example does not send out led data to physical leds. 

The controller contains a set of example patterns. The patterns will
explain the most important concepts step by step.
Read the code, see the pattern in action, feel free to modify the code,
rerun the example, and see how it affects the pattern.

# Files in the project

Each file contains comments to explain what is going on, 
but this chapter will give you a high level overview.

## 1. CMakeLists.txt

Every project needs instructions how to build it. Luckily most of it is hidden in a template.
You only need to import the template here, and call it. You do have the option to set your project name here.

## 2. mapping/generateHaloMap.py

This python script creates the file mapping/haloMap.hpp. It contains the positions of all the leds in the installation.
This file is used by the monitor to render an emulation of your installation.
You can also use the mapping coordinates to create spatially mapped patterns. There is an example of that in patterns.hpp

## 3. main.cpp

This is the entry point for your projects code. 

## 4. palettes.hpp

This contains the colors we are going to work with. 

## 5. patterns.hpp

All the patterns are located in this file.

Over time, i learned that patterns are tightly coupled to an specific installation. Some patterns work out great in
one installation, but not in others. Even some generic patterns like a simple chaser will in practice have a couple of tweaks
that make it just a little bit better on that installation, depending on scale of installation, type of lights used, type of event it is used for, etc..  That is why i chose not to create reusable patterns in the library. 
As an alternative, all projects have their own set of patterns. You can tweak these to patterns to perfection. If you start a new project, you can simply copy over the patterns that you like and tweak them again for that installation. Now you have 100% creative freedom, and there is full separation between your (and other peoples) projects. 
A lot of patterns will contain similar logic. Think of fades, LFOs, beat detection, etc. All that logic is put into helper classes, so you don't have to re-invent te wheel. If you write you patterns using these helper classes, most patterns have relatively little code, mostly only glue logic.
