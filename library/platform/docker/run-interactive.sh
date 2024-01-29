docker build -t hyperion ${HYPERION_LIB_DIR}/platform/docker
docker run -it --name=hyperion --rm \
    --mount type=bind,source=${HYPERION_LIB_DIR},target=/hyperion_lib \
    --mount type=bind,source=${PWD},target=/project \
    --workdir=/project \
    -p 80:80 \
    -p 443:443 \
    hyperion \
    /bin/bash