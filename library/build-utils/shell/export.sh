# This script is executed each time you open a new shell.
# It set some environment variables and aliases, so the
# shell knows which scripts to run, and these script know
# where to find the hyperion code and other important stuff 

# extract the library dir name from the location of this script
if [[ -z "${HYPERION_LIB_DIR}" ]]; then
    export HYPERION_LIB_DIR="$(cd "$(dirname "$0")/../../"; pwd)"
fi

alias hyper-interactive='$HYPERION_LIB_DIR/platform/docker/run-interactive.sh'

alias hyper='$HYPERION_LIB_DIR/build-utils/shell/hyper.sh'

# echo "HYPERION_LIB_DIR = "$HYPERION_LIB_DIR