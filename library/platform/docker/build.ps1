function build {
    [CmdletBinding()]
    param (
        [Parameter(Mandatory=$true, Position=0)]
        [String]$Project
    )
    write-host $PSScriptRoot

    # run this from the project folder root
 
    $projectDir = $($PWD.Path)

    docker build -t hyperion $HYPERION_LIB_DIR\platform\docker
    Write-Host "Docker build complete"

    docker run -it --name hyperion --rm `
        --mount type=bind,source=$HYPERION_LIB_DIR,target=/hyperion_lib `
        --mount type=bind,source=$projectDir,target=/project `
        --workdir /project hyperion `
        /hyperion_lib/scripts/build.sh docker inside-container

    Write-Host "Docker Run complete"  

    docker run -it --name=hyperion --rm `
        --mount type=bind,source=$HYPERION_LIB_DIR,target=/hyperion_lib `
        --mount type=bind,source=$projectDir,target=/project `
        --workdir=/project `
        -p 443:4430 `
        -p 9600-9800:9600-9800 `
        hyperion `
        /hyperion_lib/scripts/run.sh docker inside-container
}
