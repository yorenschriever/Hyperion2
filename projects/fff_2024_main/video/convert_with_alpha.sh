TMP_FILE="$(mktemp -q /tmp/video_alpha.XXXXXX).MOV"
SRC_FILE=$1
MASK_FILE=$2
SCRIPT_DIR=$(dirname "$0")

echo $TMP_FILE

ffmpeg -i "$SRC_FILE" -vf alphaextract,format=yuva444p12le "$TMP_FILE"

python3 $SCRIPT_DIR/convert_with_alpha.py "$SRC_FILE" "$TMP_FILE" "$MASK_FILE"