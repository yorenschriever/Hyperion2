#examples: 
# ./run macos
# ./run esp32
# ./run esp32 /dev/tty.usbserial-110

#BASEDIR=$(dirname "$0")
BASEDIR=.
TARGET="$1"

[ ! $TARGET ] && echo "no target specified" && exit 1;

if [ $TARGET = 'macos' ]; then
    "${BASEDIR}/build/${TARGET}/app"

elif [ $TARGET = 'esp32' ]; then
    PORT=${2:-$(ls /dev/tty.usbserial*)}

    [ ! $IDF_PATH ] && echo "run get_idf first" && exit 1;

    cd ${BASEDIR}/build/${TARGET}
    python3 $IDF_PATH/components/esptool_py/esptool/esptool.py -p ${PORT} write_flash @flash_project_args
    python3 $IDF_PATH/tools/idf_monitor.py -p ${PORT} "firmware.elf"
else 
    echo "Invalid target: ${TARGET}"
    exit 1;
fi
