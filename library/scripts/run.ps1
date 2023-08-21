function run {
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

    docker run -it --name=hyperion --rm `
        --mount type=bind,source=$HYPERION_LIB_DIR,target=/hyperion_lib `
        --mount type=bind,source=$projectDir,target=/project `
        --workdir=/project `
        -p 80:80 `
        -p 9600-9800:9600-9800 `
        hyperion `
        /hyperion_lib/scripts/run.sh docker inside-container

    Write-Host "Docker Run complete"  
}
