function build {
    [CmdletBinding()]
    param (
        [Parameter(Mandatory=$true, Position=0)]
        [String]$Target
    )

    if($Target -ne "docker")  
    {  
        throw "Only docker builds are supported on windows. "  
    }

    # run this from the project folder root
    $projectDir = $($PWD.Path)

    docker build -t hyperion $HYPERION_LIB_DIR\platform\docker

    docker run -it --name hyperion --rm `
        --mount type=bind,source=$HYPERION_LIB_DIR,target=/hyperion_lib `
        --mount type=bind,source=$projectDir,target=/project `
        --workdir /project hyperion `
        /hyperion_lib/scripts/build.sh linux

    Write-Host "Docker build complete"
}
