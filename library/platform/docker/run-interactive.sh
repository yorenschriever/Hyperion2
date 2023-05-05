docker run -it --name=hyperion --rm \
    --mount type=bind,source=${HYPERION_LIB_DIR},target=/hyperion_lib \
    --mount type=bind,source=/Users/yoren/repos/Hyperion2/examples/hello_world,target=/project \
    --workdir=/project hyperion \
    /bin/ash