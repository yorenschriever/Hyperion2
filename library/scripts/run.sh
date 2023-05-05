#!/bin/sh

# this script is executed when you type `run [platform]`
# It will run a project that you have built with the build command.

#examples: 
# run macos
# run esp32
# run esp32 /dev/tty.usbserial-110

# Special cases are 
# run docker, which will spin up a docker container and run the linux build inside of it
# run esp32, which also uploads the hexfile to the chip and starts a serial monitor.

#BASEDIR=$(dirname "$0")
BASEDIR=.
TARGET="$1"

[ ! $TARGET ] && echo "no target specified" && exit 1;

if [ $TARGET = 'docker' ]; then
    docker run -it --name=hyperion --rm \
        --mount type=bind,source=${HYPERION_LIB_DIR},target=/hyperion_lib \
        --mount type=bind,source=$PWD,target=/project \
        --workdir=/project \
        -p 4430:4430 \
        -p 9600-9800:9600-9800 \
        hyperion \
        /hyperion_lib/scripts/run.sh linux

    exit;
elif [ $TARGET = 'macos' ] || [ $TARGET = 'linux' ]; then
    "${BASEDIR}/build/${TARGET}/app"

elif [ $TARGET = 'esp32' ]; then
    PORT=${2:-$(ls /dev/tty.usbserial*)}

    [ ! $IDF_PATH ] && . $HOME/repos/esp-idf/export.sh; #TODO get path from variable

    cd ${BASEDIR}/build/${TARGET}
    python3 $IDF_PATH/components/esptool_py/esptool/esptool.py -p ${PORT} -b 460800 write_flash @flash_project_args
    python3 $IDF_PATH/tools/idf_monitor.py -p ${PORT} "firmware.elf"
else 
    echo "Invalid target: ${TARGET}"
    exit 1;
fi
