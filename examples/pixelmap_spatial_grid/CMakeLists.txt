cmake_minimum_required(VERSION 3.16)

# The name of your project
set(PROJECT pixelmap_spatial_grid)

include($ENV{HYPERION_LIB_DIR}/build-utils/utils.cmake)

hyperion_before()
project(${PROJECT})
hyperion_after()
