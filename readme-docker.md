# Hyperion 2 Docker

## Installation
[Install Docker](https://docs.docker.com/get-docker/) using the instructions on their website

Clone this repo
```
git clone https://github.com/yorenschriever/Hyperion2.git
```

On linux and MacOs: Add this lines to your  ~/.zprofile (and change the path to your folder)
```
source /Users/yoren/repos/Hyperion2/library/scripts/export.sh
```

On windows: you have to run script script on startup: `library/scripts/setupWindows.ps1` TODO: this script is not up to date?


This will setup the `build` and `run` commands, so you can build for docker in the same way as the other platforms.

## Run
Open a terminal on your local dev machine, go to a project folder and build and run:

```
cd examples/hello_world
build docker
run docker
```
