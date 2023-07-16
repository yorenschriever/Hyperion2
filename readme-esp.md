# Hyperion 2 on Esp32 / Hyperion hardware

You need to program build the project on a host machine and before you can flash it to the hardware.
This means you need to be able to build the project on your machine first. 
To setup you mac, follow the installation steps in the [Macos guide](readme-macos.md).
For other operating systems there is no documentation yet.

Once you can build the project on your computer, you can follow these steps to build for the esp32 as well. 

## Installation

Install esp-idf following the
[Esp-idf installation guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/linux-macos-setup.html)

Add this line to your  ~/.zprofile (and change the path to esp idf installation folder)
```
export IDF_DIR=/Users/yoren/esp-idf
```

- Follow these steps to checkout version 5.1
[Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/versions.html#updating-release-branch)
```
cd $IDF_PATH
git fetch
git checkout release/v5.1
git pull
git submodule update --init --recursive
```

- I had to run `python esp-idf/tools/idf_tools.py install` after the update. This was mentioned in the error messages.

- If you have built projects before, and now changed the esp-idf version number, you might need to delete the build folder in your project to get rid of errors.

## Running

```
cd examples/hello_world
build esp32
run esp32
```

## Change sdkconfig
You might want to change esp-idf configurations if you are going to write code that access esp-idf directly. 
If you want to do so, follow these steps: C
reate an sdkconfig that works for you. eg by running `idf.py menuconfig` in one of the example projects. Make sure to run `idf.py set-target esp32s3` first to set the correct target. Then run `idf.py save-defconfig` to create an sdkconfig.defaults. Copy the contents of this file and add it to the sdkconfig.defaults in the platform folder if you want these settings to available to all new projects in this platform, or create a new sdkconfig.defaults in your project folder and paste it there. The build system will detect changes automatically and build a new sdkconfig for you.
