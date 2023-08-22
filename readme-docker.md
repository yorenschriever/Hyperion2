# Hyperion 2 in Docker

## Installation on a Linux or MacOs host

#### 1. Install Docker and Git
[Install Docker](https://docs.docker.com/get-docker/) using the instructions on their website.
And, if you dan't have it already, [install Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git) as well.

#### 2. Clone this repository
Clone the repository and run a script to configure some environment variables.
```sh
git clone https://github.com/yorenschriever/Hyperion2.git
./Hyperion2/library/scripts/export.sh
```

NB: you need to run the export script every time you open a new terminal. You can also add the script to your zprofile, so it runs automatically each time. See installation step 1 of the [MacOs readme](readme-macos.md) for instructions.

#### 3. Run
Open a terminal on your host machine, go to a project folder and build and run:

```sh
cd Hyperion2/examples/hello_world
build docker
run docker
```


## Installation on a windows host

#### 1. Install Docker and Git
See step 1 above

#### 2. Clone this repository
Clone the repository and run a script to configure some environment variables.
```ps
git clone https://github.com/yorenschriever/Hyperion2.git
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
. .\Hyperion2\library\scripts\export.ps1
```

NB: you need to run the export script every time you open a new terminal. You can also add the script to your profile, so it runs automatically each time. In order to do that, run this script:

```ps
. .\Hyperion2\library\scripts\setupWindows.ps1
```

#### 3. Run
Open a terminal on your host machine, go to a project folder and build and run:

```ps
cd Hyperion2\examples\hello_world
build docker
run docker
```
