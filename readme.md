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