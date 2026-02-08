# usage: hyper [-target target] [-port COM3] [commands]
# commands can be: clean, build, run, debug, monitor
# target can be: esp32, windows, hypernode
# if no target is provided, windows will be used as target.
# if no command is provided, it will build and run the project.

# Parse arguments
$TARGET = ""
$COMMANDS = @()
$PORT = ""

# Parse command line arguments
for ($i = 0; $i -lt $args.Count; $i++) {
    $arg = $args[$i]
    if ($arg -like "-target") {
        $TARGET = $args[++$i]
    }
    elseif ($arg -like "-port") {
        $PORT = $args[++$i]
    }
    elseif ($arg -in @("clean", "build", "run", "debug", "monitor")) {
        $COMMANDS += $arg
    }
    else {
        Write-Host "Unknown argument: $arg"
        Write-Host "Usage: hyper [-target target] [-port COM3] [commands]"
        Write-Host "Commands: clean, build, run, debug, monitor"
        Write-Host "Targets: esp32, windows, hypernode"
        exit 1
    }
}

# Set default target if not specified
if ([string]::IsNullOrEmpty($TARGET)) {
    $TARGET = "windows"
    Write-Host "No target specified, using default platform: $TARGET"
}

$IS_ESP = ($TARGET -eq "esp32") -or ($TARGET -eq "hypernode")

# Set default commands if not specified
if ($COMMANDS.Count -eq 0) {
    if ($IS_ESP) {
        $COMMANDS = @("build", "run", "monitor")
    }
    else {
        $COMMANDS = @("build", "run")
    }
    Write-Host "No commands specified, defaulting to: $($COMMANDS -join ' ')"
}

if ($TARGET -eq "docker")  
{  
    $projectDir = $($PWD.Path)
    docker build -t hyperion $env:HYPERION_LIB_DIR\platform\docker
    docker run -it --name=hyperion --rm `
        --mount type=bind,source=$env:HYPERION_LIB_DIR,target=/hyperion_lib `
        --mount type=bind,source=$projectDir,target=/project `
        --workdir=/project `
        -p 80:80 `
        hyperion `
        sh -c "cd /project && /hyperion_lib/build-utils/shell/hyper.sh --target=linux $COMMANDS"
    exit;

}

$BASEDIR = "."
$BUILDDIR = "$BASEDIR/build/$TARGET"

# If target is esp32 and no port is specified, try to find it automatically
if ($IS_ESP -and [string]::IsNullOrEmpty($PORT)) {
    $ports = Get-CimInstance -ClassName Win32_SerialPort | Select-Object -ExpandProperty DeviceID
    if ($ports) {
        $PORT = $ports[0]
    }
}

# Execute commands
foreach ($CMD in $COMMANDS) {
    switch ($CMD) {
        "clean" {
            Write-Host "Cleaning $TARGET..."
            if (Test-Path $BUILDDIR) {
                Remove-Item -Recurse -Force $BUILDDIR
            }
        }
        
        "build" {
            Write-Host "Building $TARGET..."
            
            # Generate certificates
            & "$env:HYPERION_LIB_DIR/scripts/certificate/generate.ps1"

            if ($TARGET -eq "windows") {
                $PARAM = "-DTARGET=$TARGET", "-DSTARGET=$TARGET"
            }
            elseif ($TARGET -eq "esp32") {
                if (-not $env:IDF_PATH) {
                    . "$env:IDF_DIR/export.ps1"
                }
                $PARAM = "-DCMAKE_TOOLCHAIN_FILE=$env:IDF_PATH/tools/cmake/toolchain-$TARGET.cmake", "-DTARGET=$TARGET", "-DSTARGET=$TARGET", "-GNinja"
            }
            elseif ($TARGET -eq "hypernode") {
                if (-not $env:IDF_PATH) {
                    . "$env:IDF_DIR/export.ps1"
                }
                $PARAM = "-DCMAKE_TOOLCHAIN_FILE=$env:IDF_PATH/tools/cmake/toolchain-esp32.cmake", "-DTARGET=esp32", "-DSTARGET=$TARGET", "-GNinja"
            }
            else {
                Write-Host "Invalid target: $TARGET"
                exit 1
            }

            if (-not (Test-Path $BUILDDIR)) {
                New-Item -ItemType Directory -Path $BUILDDIR | Out-Null
            }
            
            & cmake -S $BASEDIR -B $BUILDDIR @PARAM
            & cmake --build $BUILDDIR
        }
        
        "run" {
            Write-Host "Running $TARGET..."
            
            if ($TARGET -eq "windows") {
                & "$BASEDIR/build/$TARGET/Debug/app.exe"
            }
            elseif ($IS_ESP) {
                if (-not $env:IDF_PATH) {
                    . "$env:IDF_DIR/export.ps1"
                }

                Push-Location "$BASEDIR/build/$TARGET"
                & python "$env:IDF_PATH/components/esptool_py/esptool/esptool.py" -p $PORT -b 460800 write_flash "@flash_project_args"
                Pop-Location

                if ($LASTEXITCODE -ne 0) {
                    Write-Host "Flash failed."
                    exit 1
                }
            }
            else {
                Write-Host "Invalid target: $TARGET"
                exit 1
            }
        }
        
        "debug" {
            Write-Host "Debugging $TARGET..."
            $BUILDDIR_DEBUG = "$BASEDIR/build/${TARGET}_debug"
            
            if ($TARGET -eq "windows") {
                # Generate certificates
                & "$env:HYPERION_LIB_DIR/scripts/certificate/generate.ps1"
                $PARAM = "-DTARGET=$TARGET", "-DSTARGET=$TARGET"
            }
            elseif ($IS_ESP) {
                Write-Host "Debug not supported (yet) on esp32"
                exit 1
            }
            else {
                Write-Host "Invalid target: $TARGET"
                exit 1
            }

            if (-not (Test-Path $BUILDDIR_DEBUG)) {
                New-Item -ItemType Directory -Path $BUILDDIR_DEBUG | Out-Null
            }
            
            & cmake -DCMAKE_BUILD_TYPE=Debug -S $BASEDIR -B $BUILDDIR_DEBUG @PARAM
            if ($LASTEXITCODE -ne 0) {
                Write-Host "CMake configuration failed."
                exit 1
            }
            
            & cmake --build $BUILDDIR_DEBUG
            if ($LASTEXITCODE -ne 0) {
                Write-Host "Debug build failed."
                exit 1
            }
            
            # Start Visual Studio debugger
            & "$BUILDDIR_DEBUG/Debug/app.exe"
        }
        
        "monitor" {
            if ($IS_ESP) {
                if (-not $env:IDF_PATH) {
                    . "$env:IDF_DIR/export.ps1"
                }
                Write-Host "Starting monitor on $PORT..."
                & python "$env:IDF_PATH/tools/idf_monitor.py" -p $PORT "$BUILDDIR/firmware.elf"
            }
            else {
                Write-Host "Monitor command only supported for esp32 and hypernode targets"
                exit 1
            }
        }
        
        default {
            Write-Host "Unknown command: $CMD"
            Write-Host "Valid commands are: clean, build, run, debug, monitor"
            exit 1
        }
    }
}
