docker build -t hyperion $env:HYPERION_LIB_DIR\platform\docker
docker run -it --name hyperion --rm `
    --mount type=bind,source=$env:HYPERION_LIB_DIR,target=/hyperion_lib `
    --mount type=bind,source=$pwd,target=/project `
    --workdir /project `
    -p 80:80 `
    -p 443:443 `
    hyperion `
    /bin/bash