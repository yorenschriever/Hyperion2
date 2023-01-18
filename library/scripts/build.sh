TARGET="$1"
#BASEDIR=$(dirname "$0")
BASEDIR=.
BUILDDIR="$BASEDIR/build/${TARGET}"

[ ! $TARGET ] && echo "no target specified" && exit 1;

if [ $TARGET = 'macos' ]; then
    PARAM="-DTARGET=${TARGET}"
elif [ $TARGET = 'esp32' ] || [ $TARGET = 'esp32s3' ]; then
    [ ! $IDF_PATH ] && echo "run get_idf first" && exit 1;
    PARAM="-DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-${TARGET}.cmake -DTARGET=${TARGET} -GNinja"
else 
    echo "Invalid target: ${TARGET}"
    exit 1;
fi

# rm -rf $BUILDDIR 
mkdir -p $BUILDDIR
cmake -S "$BASEDIR" -B "$BUILDDIR" $PARAM
cmake --build "$BUILDDIR"
