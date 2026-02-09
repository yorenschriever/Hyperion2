#!/bin/sh

# usage: hyper [--target=target] [--port=/dev/cu.usbserial-xxxx] [commands]
# commands can be: clean, build, run, debug, monitor
# target can be: esp32, macos, linux, rpi, hypernode, docker
# if no target is provided, the current platform will be used as target.
# if no command is provided, it will build and run the project.

# Parse arguments
TARGET=""
COMMANDS=""
PORT=""

# Detect current platform as default target
detect_platform() {
    case "$(uname -s)" in
        Darwin*) echo "macos" ;;
        Linux*) echo "linux" ;;
        *) echo "unknown" ;;
    esac
}

# Parse command line arguments
for arg in "$@"; do
    case $arg in
        --target=*)
            TARGET="${arg#*=}"
            shift
            ;;
        --port=*)
            PORT="${arg#*=}"
            shift
            ;;
        clean|build|run|debug|monitor)
            COMMANDS="$COMMANDS $arg"
            shift
            ;;
        *)
            echo "Unknown argument: $arg"
            echo "Usage: hyper [--target=target] [--port=/dev/cu.usbserial-xxxx] [commands]"
            echo "Commands: clean, build, run, debug, monitor"
            echo "Targets: esp32, macos, linux, rpi, hypernode, docker"
            exit 1
            ;;
    esac
done

# Set default target if not specified
if [ -z "$TARGET" ]; then
    TARGET=$(detect_platform)
    echo "No target specified, using current platform: ${TARGET}"
fi
IS_ESP=$( [ $TARGET = 'esp32' ] || [ $TARGET = 'hypernode' ] )

# Set default commands if not specified
if [ -z "$COMMANDS" ]; then
    if [ $IS_ESP ]; then COMMANDS="build run monitor"
    else COMMANDS="build run"
    fi
    echo "No commands specified, defaulting to: $COMMANDS"
fi

# if target is docker, delegate everything to this script, but inside docker, and exit
if [ $TARGET = 'docker' ]; then
    docker build -t hyperion ${HYPERION_LIB_DIR}/platform/docker
    docker run -it --name=hyperion --rm \
        --mount type=bind,source=${HYPERION_LIB_DIR},target=/hyperion_lib \
        --mount type=bind,source=$PWD,target=/project \
        --workdir=/project\
        -p 80:80 \
        hyperion \
        sh -c "cd /project && /hyperion_lib/build-utils/shell/hyper.sh --target=linux $COMMANDS"
    exit
fi

BASEDIR=.
BUILDDIR="$BASEDIR/build/${TARGET}"

# If target is esp32 and no port is specified, try to find it automatically
if $IS_ESP && [ -z "$PORT" ]; then
    PORT=$(ls /dev/cu.usbserial-* 2>/dev/null | head -n 1)
fi

# Execute commands
for CMD in $COMMANDS; do
    case $CMD in
        clean)
            echo "Cleaning ${TARGET}..."
            rm -rf "$BUILDDIR"
            ;;
        
        build)
            echo "Building ${TARGET}..."
            
            ${HYPERION_LIB_DIR}/web/certificate/generate.sh

            if [ $TARGET = 'macos' ] || [ $TARGET = 'linux' ] || [ $TARGET = 'rpi' ]; then
                PARAM="-DTARGET=${TARGET} -DSTARGET=${TARGET}"

            elif [ $TARGET = 'esp32' ]; then
                [ ! $IDF_PATH ] && . $IDF_DIR/export.sh
                PARAM="-DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-${TARGET}.cmake -DTARGET=${TARGET} -DSTARGET=${TARGET} -GNinja"

            elif [ $TARGET = 'hypernode' ]; then
                [ ! $IDF_PATH ] && . $IDF_DIR/export.sh
                PARAM="-DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-esp32.cmake -DTARGET=esp32 -DSTARGET=${TARGET} -GNinja"

            else
                echo "Invalid target: ${TARGET}"
                exit 1
            fi

            mkdir -p $BUILDDIR
            cmake -S "$BASEDIR" -B "$BUILDDIR" $PARAM
            cmake --build "$BUILDDIR"
            
            if [ $? -ne 0 ]; then
                echo "Build failed."
                exit 1
            fi
            ;;
        
        run)
            echo "Running ${TARGET}..."
            
            if [ $TARGET = 'macos' ] || [ $TARGET = 'linux' ] || [ $TARGET = 'rpi' ]; then
                "${BASEDIR}/build/${TARGET}/app"

            elif $IS_ESP; then
                [ ! $IDF_PATH ] && . $IDF_DIR/export.sh

                cd ${BASEDIR}/build/${TARGET}
                python3 $IDF_PATH/components/esptool_py/esptool/esptool.py -p ${PORT} -b 460800 write_flash @flash_project_args && \
                # python3 $IDF_PATH/tools/idf_monitor.py -p ${PORT} "firmware.elf"
                cd - > /dev/null

            else
                echo "Invalid target: ${TARGET}"
                exit 1
            fi
            
            if [ $? -ne 0 ]; then
                echo "Run failed."
                exit 1
            fi
            ;;
        
        debug)
            echo "Debugging ${TARGET}..."
            BUILDDIR_DEBUG="$BASEDIR/build/${TARGET}_debug"
            
            if [ $TARGET = 'macos' ] || [ $TARGET = 'linux' ] || [ $TARGET = 'rpi' ]; then
                ${HYPERION_LIB_DIR}/web/certificate/generate.sh
                PARAM="-DTARGET=${TARGET} -DSTARGET=${TARGET}"

            elif $IS_ESP; then
                echo "Debug not supported (yet) on esp32"
                exit 1

            else
                echo "Invalid target: ${TARGET}"
                exit 1
            fi

            mkdir -p $BUILDDIR_DEBUG
            cmake -DCMAKE_BUILD_TYPE=Debug -S "$BASEDIR" -B "$BUILDDIR_DEBUG" $PARAM
            cmake --build "$BUILDDIR_DEBUG"
            
            if [ $? -ne 0 ]; then
                echo "Debug build failed."
                exit 1
            fi
            
            lldb -o run -o "session save $BUILDDIR_DEBUG/log.txt" "$BUILDDIR_DEBUG/app"
            ;;
        
        monitor)
            if $IS_ESP; then
                [ ! $IDF_PATH ] && . $IDF_DIR/export.sh
                echo "Starting monitor on ${PORT}..."
                python3 $IDF_PATH/tools/idf_monitor.py -p ${PORT} "${BUILDDIR}/firmware.elf"
            else
                echo "Monitor command only supported for esp32 and hypernode targets"
                exit 1
            fi
            ;;
        
        *)
            echo "Unknown command: $CMD"
            echo "Valid commands are: clean, build, run, debug, monitor"
            exit 1
            ;;
    esac
done
