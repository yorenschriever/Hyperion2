cmake_minimum_required(VERSION 3.16)

# The name of your project
set(PROJECT combine_inputs)

include($ENV{HYPERION_LIB_DIR}/build-utils/utils.cmake)

hyperion_before()
project(${PROJECT})
hyperion_after()
