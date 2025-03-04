﻿$TARGET = $args[0]
$projectDir = $($PWD.Path)

$BASEDIR=$projectDir
$BUILDDIR="$BASEDIR\build\${TARGET}"

if ($args.Length -eq 0) 
{
    Write-Host "no target specified" 
    exit 1;
}

if ($TARGET -eq "docker")  
{  
    docker build -t hyperion $env:HYPERION_LIB_DIR\platform\docker
    docker run -it --name hyperion --rm `
        --mount type=bind,source=$env:HYPERION_LIB_DIR,target=/hyperion_lib `
        --mount type=bind,source=$projectDir,target=/project `
        --mount type=bind,source=$projectDir\..\common,target=/common `
        --workdir /project hyperion `
        /hyperion_lib/scripts/build.sh linux
    exit;

} elseif ($TARGET -eq "windows") {
    # $env:HYPERION_LIB_DIR\scripts\certificate\generate.ps1
    $PARAM="-DTARGET=${TARGET}"
} else {
    Write-Host "Invalid target: " + $TARGET
    exit 1;
}

New-Item -ItemType Directory -Force -Path $BUILDDIR
cmake -S "$BASEDIR" -B "$BUILDDIR" $PARAM
cmake --build "$BUILDDIR"




