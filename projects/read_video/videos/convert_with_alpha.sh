TMP_FILE="$(mktemp -q /tmp/video_alpha.XXXXXX).MOV"
SRC_FILE=$1
SCRIPT_DIR=$(dirname "$0")

echo $TMP_FILE

ffmpeg -i $SRC_FILE -vf alphaextract,format=yuv420p $TMP_FILE

python3 $SCRIPT_DIR/convert_with_alpha.py "$SRC_FILE" "$TMP_FILE"