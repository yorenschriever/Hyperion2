# Hyperion 2 on Raspberry pi 4

The easiest way to start is with the Hyperion image. This contains everything you need, so you don't need to install anything further. The image is based on raspberry pi os 64 bit, which means it will only work on a raspberry pi 4. Below are instructors to create your own image in case you want to use another base OS. 

It tested everything on a model with 2GB of RAM memory, and this is more than enough to run the largest project i have at the moment. But building the program is a different story. It uses all memory and swap. Building works, but i have to close everything else running on the pi for it to work. Therefore, i recommend going with 4GB model. 

If you want to resolve '*.local' addresses from a windows machine you need to install [Bonjour](https://support.apple.com/kb/DL999?locale=en_GB). You need this to connect through ssh, and also to access the web interface.

## Hyperion image

### Flash to SD card 
Install the [Raspberry pi imager tool](https://www.raspberrypi.com/software/)
Then, download the [Hyperion Raspberry pi image](https://drive.google.com/file/d/1CDTD0Jk2qYgchQpw8rUEoqbvZOGzG-S5/view?usp=sharing)

Click 'choose os' and select 'use custom', and select the hyperion image. Upload it to an SD card.

### Expand filesystem
Now, connect your pi and computer to the same network, and use ssh to connect to the pi
```
ssh hyperion@raspberrypi.local
```
use password `hyperion`.

Once you're in, you might need to expand the filesystem (not sure if raspberry pi os does that automatically for you nowadays, if so you can skip this step)
```
sudo raspi-config
```
then select `Advanced options`, and then `Expand filesystem`. Click OK, and reboot the PI. 
SSH into the pi again for the next steps.

### Git pull
To work with the most recent version, pull the latest changes from github:
```
cd ~/Hyperion2
git pull
```

### Test 
Now test if everything works by building and running the hello world example. 
```
cd ~/Hyperion2/examples/hello_world
build rpi
run rpi
```

## Running and accessing the web interface
if you aren't connected to the pi anymore, ssh into the pi again, using password `hyperion`:
```
ssh hyperion@raspberrypi.local
```

Then build the writing_patterns example
```
cd ~/Hyperion2/examples/writing_patterns
build rpi
run rpi
```

Now, open https://raspberrypi.local:4430/ to view the web interface.

## Remote development in vscode
VScode can connect to the raspberry pi, so you can edit the files and access the shell from vscode on your computer.
More information here: [Remote Development using SSH](https://code.visualstudio.com/docs/remote/ssh)

### SSH key
Instead of typing your username and password over and over again, you can authenticate using keys.

Create a key in your dev machine
```
ssh-keygen -t ed25519
```

add to `.ssh/config` as under the host of the pi
```
IdentityFile ~/.ssh/id_ed25519
```

Then add the key to the raspberry pi by typing this in the terminal of your dev machine
```
ssh-copy-id hyperion@raspberrypi.local
```

## Tips for use in permanent installations
If you want to use the pi in a permanent installtion, you should consider these things:
- Disable logging to reduce wear on the sd card.
- Make SD card (/boot partition) readonly, so it is unlikely to damage when the power is interrupted 
- Backup the sd card twice, and keep one always with the installation, the SD cards are the weakest point.

## Create your own image

### Create sd card

Flash the latest version of raspberry pi os on an sd card. I chose the 64bit lite version, but any other will do as well. 
Before you click the 'write' button the rapsberry pi imager tool, make sure to click the gear icon, and consider these settings:
- Hostname: raspberrypi.local
- Enable ssh, password authentication
- Set username and password: hyperion / hyperion

Once the pi is running with the new os, ssh into it to continue with the installation

### installation
Install the required packages:
```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install cmake make gcc g++ openssl openssl-dev git linux-headers bash libssl-dev libasound2-dev libjack-jackd2-dev
```

Install boost
```
cd ~
curl -L -o boost.tar.gz https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz 
gunzip boost.tar.gz 
tar -xvzf boost.tar.gz boost_1_82_0/
rm boost.tar.gz
```

Configure the environment variables:
```
pico ~/.bashrc
```
And add these lines in the file somewhere. type ctrl+o to save and ctrl+x to close after editing. Change the home path if you chose a different username when flashing the sd card.
```
export BOOST_DIR=/home/hyperion/boost_1_81_0
source /home/hyperion/Hyperion2/library/scripts/export.sh
export HYPERION_LIB_DIR=/home/hyperion/Hyperion2/library/
```

Generate a certificate
```
cd ~/Hyperion2/library/scripts/certificate/
./generate.sh
```

Add user hyperion to the audio group to give it access to midi devices (might not be necessary, not sure)
```
usermod -a -G audio hyperion
```

### Create image
On macos use dd to copy the contents to an image file. Replace /dev/disk4 with the SD card. 
```
sudo dd if=/dev/disk4 of=Hyperion.img 
```
Then use [PiShrink](https://github.com/lisanet/PiShrink-macOS) to remove the unused disk space from the image. And then gzip it to further reduce the space
```
pishrink Hyperion.img
gzip Hyperion.img
```
