# This script is executed eachg time you open a new shell.
# It set some environment variables and aliassed, so the
# shell knows which scripts to run, and these script know
# where to find the hyperion code and other important stuff 

# extract the library dir name from the location of this script
export HYPERION_LIB_DIR="$(cd "$(dirname "$0")/../"; pwd)"
alias build='$HYPERION_LIB_DIR/scripts/build.sh'
alias run='$HYPERION_LIB_DIR/scripts/run.sh'
alias debug='$HYPERION_LIB_DIR/scripts/debug.sh'

# echo "HYPERION_LIB_DIR = "$HYPERION_LIB_DIR