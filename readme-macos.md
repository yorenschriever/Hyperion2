# Hyperion 2 on MacOs

## Installation

This guide will help you getting started with Hyperion on MacOs. Copy-pasting the snippets into a terminal window should do the trick.

### 1. Repository
This step will clone this repo to a folder of your choice. Then it includes a reference to export.sh in your ~/.zprofile, so you can use the 'build' and 'run' script aliases in your shell. You can change the location of `SOURCE_INSTALL_DIR` if you like.
```sh
SOURCE_INSTALL_DIR=$HOME"/repos/Hyperion2"

mkdir -p $SOURCE_INSTALL_DIR
git clone https://github.com/yorenschriever/Hyperion2.git $SOURCE_INSTALL_DIR
echo "source $SOURCE_INSTALL_DIR/library/scripts/export.sh" >> ~/.zprofile 
```

### 2. OpenSSL
Install [Brew](https://brew.sh/) and run these commands to install OpenSSL. I am using OpenSSL version stable 3.1.0
```sh
brew install openssl
brew link --force openssl
```

### 3. Boost
This snippet downloads [Boost](https://www.boost.org/users/download/), unpacks the required folder, and export its location in your ~/.zprofile. You can change the location of `BOOST_INSTALL_DIR` if you like.

```sh
BOOST_INSTALL_DIR=$HOME"/repos/boost_1_82_0"

mkdir -p $BOOST_INSTALL_DIR
curl -L -o "$BOOST_INSTALL_DIR/boost.tar.gz" https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz
tar xzvf $BOOST_INSTALL_DIR/boost.tar.gz --strip-components=1 -C $BOOST_INSTALL_DIR boost_1_82_0/boost
echo "export BOOST_DIR=$BOOST_INSTALL_DIR" >> ~/.zprofile 
```

### 4. Finished
That's it. Close your terminal and reopen (if you still need it).

## Configure VSCode
[VSCode](https://code.visualstudio.com/) is my editor of choice. Install the plugin "Microsoft c/c++ extension" and add these lines to include path in its settings. These must be absolute paths. Alter them to match your installation directories. 
```
/Users/yoren/repos/Hyperion2/library
/Users/yoren/repos/esp-idf/**
```

## Create, install and trust a certificate (optional)
In order to use the web interface of Hyperion, you need to create a certificate, install it, and trust it.
```sh
$HYPERION_LIB_DIR/scripts/certificate/generate.sh
$HYPERION_LIB_DIR/scripts/certificate/install-mac.sh
```
This will generate a-self signed certificate. It will also install it on your machine. It will ask your password for permission to do so.

After the certificate is installed you need to trust it. Open spotlight (cmd + space) and type 'Keychain Access'
There, in the left menu, select 'system', and go to the tab 'Certificates'. There you see two items called hyperion.local. For both items, double click, and in the window that opens, expand the 'trust' section. Set everything to 'always trust'
The generated certificate will be valid for a year. After that you need to create a new one and trust it again. You also need to flash new firmware with the new certificate to all your devices.

After installing and trusting the certificate you need to restart your browser to recognize the new certificates

TODO:
how to install on other devices? like phone/tablet, other machines..

## Running
Test if your installation is working by running the hello world example.

```sh
cd examples/hello_world
build macos
run macos
```

You should see the text 'initializing' followed by 'running'


# Troubleshooting
You might also need to install command line tools for xcode. Make sure to also accept the license before using it.
```sh
xcode-select --install
sudo xcodebuild -license
```
or alternatively install the required tools manually
```sh
brew install cmake gcc git
```

On some machines i had to explicitly set OPENSSL_ROOT
```sh
export OPENSSL_ROOT=$(brew --prefix openssl)
export OPENSSL_ROOT_DIR=$(brew --prefix openssl)
```

In case the link to boost version 1.82 is no longer working, you can find a backup here
https://drive.google.com/file/d/1PrvEeoaydhL4MYVJdxWWK8Bm0sHBlHyo/view?usp=sharing.
This is a google drive link. You need to download it manually. You can't replace the url in the curl script.

If your project used to build before, but suddenly it doesn't anymore: You can try removing all build files and start from scratch. In your project folder, run:
```sh
rm -r build
build macos
run macos
```

The error `Target "Hyperion" links to: OpenSSL::SSL but the target was not found.` can also be solved by removing the build dir