# Hyperion 2 on MacOs

## Installation

Clone this repo
```
git clone https://github.com/yorenschriever/Hyperion2.git
```

Add these lines to your  ~/.zprofile (and change the path to your folder)
```
export BOOST_DIR=/Users/yoren/repos/boost_1_81_0
source /Users/yoren/repos/Hyperion2/library/scripts/export.sh
```

Install [Brew],(https://brew.sh/) and run these commands
```
brew install openssl #i used version stable 3.1.0
brew link --force openssl
export OPENSSL_ROOT_DIR=/usr/local/opt/openssl@3 #probably not necessary, but i had to do this on my old mac
```

Download boost (https://www.boost.org/users/download/), place it somewhere on your disk and update the path in ~/.zprofile. I use version 1_81_0

Add these lines to include path of vscode plugin "microsoft c/c++ extension". (also change the path here)
```
/Users/yoren/repos/esp-idf/**
/Users/yoren/repos/Hyperion2/library
```
Close your terminal (and reopen if you still need it)

## Create, install and trust a certificate
In order to use the web interface of hyperion, you need to create a certificate, install it an trust it.
creating and installing can be done by going to
```
cd /Users/yoren/repos/Hyperion2/library/scripts/certificate
./generate.sh
```
this will generate a-self signed certificate. It will also install it on your machine. It will ask your password for permission to do so.

After the certificate is installed you need to trust it. Open spotlight and type 'Keychain Access'
There, in the left menu, select 'system', and go to the tab 'Certificates'. There you see 2 items called hyperion.local. For both items, double click, and in the window that opens, expand the 'trust' section. Set everything to 'always trust'
The generated certificate will be valid for a year. After that you need to create a new one and trust it again. You also need to flash new firmware with the new certificate to all your devices.

After installing and trusting the certificate you need to restart your browser to recognize the new certificates

TODO:
how to install on other devices? like phone/tablet, other machines..

## Running

```
cd examples/hello_world
build macos
run macos
```

