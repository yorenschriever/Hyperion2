# Setup

## General
Clone this repo
```
git clone https://github.com/yorenschriever/Hyperion2.git
```
Add these lines to your  ~/.zprofile (and change the path to your folder)
```
export HYPERION_LIB_DIR=/Users/yoren/repos/Hyperion2/library
alias build='$HYPERION_LIB_DIR/scripts/build.sh'
alias run='$HYPERION_LIB_DIR/scripts/run.sh'
```
Add these lines to include path of vscode plugin "microsoft c/c++ extension". (also change the path here)
```
/Users/yoren/repos/esp-idf/**
${HYPERION_LIB_DIR}
```
Close your terminal (and reopen if you still need it)

## Mac
No additional setup required

## Espressif devices

- Install esp idf following this
[Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/linux-macos-setup.html)


# Build and run

## Mac

```
cd examples/hello_world
build macos
run macos
```

## esp32

```
cd examples/hello_world
get_idf
build esp32
run esp32
```

