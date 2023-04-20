# Setup

## General
Clone this repo
```
git clone https://github.com/yorenschriever/Hyperion2.git
```
Add these lines to your  ~/.zprofile (and change the path to your folder)
```
export BOOST_DIR=/Users/yoren/repos/boost_1_81_0
export HYPERION_LIB_DIR=/Users/yoren/repos/Hyperion2/library
alias build='$HYPERION_LIB_DIR/scripts/build.sh'
alias run='$HYPERION_LIB_DIR/scripts/run.sh'
```
Add these lines to include path of vscode plugin "microsoft c/c++ extension". (also change the path here)
```
/Users/yoren/repos/esp-idf/**
/Users/yoren/repos/Hyperion2/library
```
Close your terminal (and reopen if you still need it)

## Linux WSL
```
sudo apt-get install build-essential gdb
```
Check this link for information on configuring brew for WSL on Windows: https://linux.how2shout.com/install-brew-on-wsl-windows-subsystem-for-linux/ 
```
brew install cmake
brew install jack
brew install gcc
sudo apt-get install libasound2-dev
sudo apt install libssl-dev # For openssl 3.1.0
```
download boost (https://www.boost.org/users/download/), place it somewhere on your disk and update the path in ~/.zprofile i use version 1_81_0
```
wget https://boostorg.jfrog.io/artifactory/main/release/1.81.0/source/boost_1_81_0.zip -O $HOME/boost_1_81_0.zip
unzip $HOME/boost_1_81_0.zip
rm $HOME/boost_1_81_0.zip
echo 'export BOOST_DIR="$HOME/boost_1_81_0"' >> ~/.bash_profile

cd ./library/scripts/certificate
./generate.sh
```
TODO : Fix include error: ./Hyperion2/library/platform/macos/ethernet.cpp:11:10: fatal error: net/if_dl.h: No such file or directory
```
sudo wget https://opensource.apple.com/source/xnu/xnu-6153.81.5/bsd/net/if_dl.h -O /usr/include/net/if_dl.h
sudo wget https://opensource.apple.com/source/xnu/xnu-344.49/bsd/sys/appleapiopts.h -O /usr/include/sys/appleapiopts.h
```
Install ESP-IDF
```
sudo apt-get install git wget flex bison gperf python3 python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

mkdir -p $HOME/CodeProjects/esp
cd $HOME/CodeProjects/esp
git clone --recursive https://github.com/espressif/esp-idf.git

cd $HOME/CodeProjects/esp/esp-idf
./install.sh all

echo './export.sh' >> ~/.bash_profile

```

Add these lines to include path of vscode plugin "microsoft c/c++ extension". (also change the path here)
```
/home/lennard/CodeProjects/esp/esp-idf/**
/home/lennard/CodeProjects/Hyperion2/library
```

Upgrade GCC version
```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc
```

## Mac
- brew install python3
- pip3 install pyglet

- brew install openssl (i use version stable 3.1.0)
- brew link --force openssl
- download boost (https://www.boost.org/users/download/), place it somewhere on your disk and update the path in ~/.zprofile i use version 1_81_0

- on old mac i had to set
export OPENSSL_ROOT_DIR=/usr/local/opt/openssl@3
and reopen the shell

## Espressif devices

- Install esp idf following this
[Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/linux-macos-setup.html)

- Add the generated export.sh to ~/.zprofile TODO check if necessary, or if running it once is enough

# Build and run

## Mac

```
cd examples/hello_world
build macos
run macos
```

## esp32

```
cd examples/hello_world
get_idf
build esp32
run esp32
```

### change sdkconfig
create an sdkconfig that works for you. eg by running `idf.py menuconfig` in one of the example projects. make sure to run `idf.py set-target esp32s3` first to set the correct target. Then run `idf.py save-defconfig` to create an sdkconfig.defaults. copy the contents of this file and add it to the sdkconfig.defaults in the platform folder is you want these settings to available to all new projects in this platform, or create a new sdkconfig.defaults in your project folder and paste it there. The build system will detect changes automatically and build a new sdkconfig for you.

# create, install and trust a certificate
in order to use the web interface of hyperion, you need to create a certificate, install it an trust it.
creating and installing can be done by going to
```
cd /Users/yoren/repos/Hyperion2/library/scripts/certificate
./generate.sh
```
this will generate a self signed certificate. It will also install it on your machine. It will ask your permission to do so.
After the certificate is installed you need to trust it. Open spotlight and type
```
Keychain Access
```
There, in the left menu, select 'system', and go to the tab 'Certificates'. There you see 2 items called hyperion.local. For both items, double click, and in the window that opens, expand the 'trust' section. Set everything to 'always trust'

The generated certificate will be valid for a year. After that you need to create a new one and trust it again. You also need to flash new firmware with the new certificate to all your devices.

after installing and trusting the certificate you need to restart your browser to recognize the new certificates

TODO:
how to install on other devices? like phone/tablet, other machines..