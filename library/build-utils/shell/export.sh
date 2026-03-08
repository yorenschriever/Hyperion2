# This script is executed each time you open a new shell.
# It set some environment variables and aliases, so the
# shell knows which scripts to run, and these script know
# where to find the hyperion code and other important stuff 

# use either $0 or ${BASH_SOURCE[0]} to get the path of the current script, depending on which value is set
SCRIPT_PATH="${BASH_SOURCE[0]:-$0}"
export HYPERION_LIB_DIR="$(cd "$(dirname "$SCRIPT_PATH")/../../"; pwd)"

alias hyper-interactive='$HYPERION_LIB_DIR/platform/docker/run-interactive.sh'

alias hyper='$HYPERION_LIB_DIR/build-utils/shell/hyper.sh'

# echo "HYPERION_LIB_DIR = "$HYPERION_LIB_DIR