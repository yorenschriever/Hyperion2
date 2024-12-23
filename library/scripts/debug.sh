#!/bin/sh

# This script is executed when you type `build [platform]`
# It will build the project that is in your correct working directory
# for the platform that you provided as argument
# eg: 
# build esp32
# build macos
# will build the project to run on an esp32 or on macos respectively.

# A special case is run docker, this will create and start a docker container,
# in which it will run a linux build

TARGET="$1"
BASEDIR=.
BUILDDIR="$BASEDIR/build/${TARGET}_debug"

[ ! $TARGET ] && echo "no target specified" && exit 1;

if [ $TARGET = 'docker' ]; then
    docker build -t hyperion ${HYPERION_LIB_DIR}/platform/docker
    docker run -it --name=hyperion --rm \
        --mount type=bind,source=${HYPERION_LIB_DIR},target=/hyperion_lib \
        --mount type=bind,source=$PWD,target=/project \
        --workdir=/project hyperion \
        /hyperion_lib/scripts/debug.sh linux
    exit;

elif [ $TARGET = 'macos' ] || [ $TARGET = 'linux' ] || [ $TARGET = 'rpi' ]; then
    ${HYPERION_LIB_DIR}/scripts/certificate/generate.sh
    PARAM="-DTARGET=${TARGET} -DSTARGET=${TARGET}"

elif [ $TARGET = 'esp32' ] || [ $TARGET = 'esp32s3' ] || [ $TARGET = 'hypernode' ]; then
    echo "Debug not supported (yet) on esp32"
    exit 1;
else 
    echo "Invalid target: ${TARGET}"
    exit 1;
fi

# rm -rf $BUILDDIR 
mkdir -p $BUILDDIR
cmake -DCMAKE_BUILD_TYPE=Debug -S "$BASEDIR" -B "$BUILDDIR" $PARAM
cmake --build "$BUILDDIR"

lldb -o run -o "session save $BUILDDIR/log.txt" "$BUILDDIR/app"