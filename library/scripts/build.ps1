$TARGET = $args[0]
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
    docker build -t hyperion $HYPERION_LIB_DIR\platform\docker
    docker run -it --name hyperion --rm `
        --mount type=bind,source=$HYPERION_LIB_DIR,target=/hyperion_lib `
        --mount type=bind,source=$projectDir,target=/project `
        --workdir /project hyperion `
        /hyperion_lib/scripts/build.sh linux
    exit;

# } elseif ($TARGET -eq "windows") {
#     $PARAM="-DTARGET=${TARGET}"
} else {
    Write-Host "Invalid target: " + $TARGET
    exit 1;
}

New-Item -ItemType Directory -Force -Path $BUILDDIR
cmake -S "$BASEDIR" -B "$BUILDDIR" $PARAM
cmake --build "$BUILDDIR"




