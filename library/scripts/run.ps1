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
    docker run -it --name=hyperion --rm `
        --mount type=bind,source=$HYPERION_LIB_DIR,target=/hyperion_lib `
        --mount type=bind,source=$projectDir,target=/project `
        --workdir=/project `
        -p 80:80 `
        hyperion `
        /hyperion_lib/scripts/run.sh linux
    exit;

} elseif ($TARGET -eq "windows") {
    . $BUILDDIR\app.exe
} else {
    Write-Host "Invalid target: " + $TARGET
    exit 1;
}
