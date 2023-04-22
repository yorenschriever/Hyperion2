echo "mounting $1 as project"

docker run -it --name=hyperion --rm \
    --mount type=bind,source=${HYPERION_LIB_DIR},target=/hyperion_lib \
    --mount type=bind,source=$1,target=/project \
    --workdir=/project hyperion \
    /hyperion_lib/scripts/run.sh docker inside-container
