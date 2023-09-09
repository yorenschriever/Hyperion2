cmake_minimum_required(VERSION 3.16)

macro(hyperion_before)

    if ("${PROJECT}" STREQUAL "")
        message(FATAL_ERROR "PROJECT variable is not set")
    endif ()

    if ("$ENV{HYPERION_LIB_DIR}" STREQUAL "")
        message(FATAL_ERROR "Hyperion lib dir not set. run export HYPERION_LIB_DIR=[path_to_hyperion]")
    endif ()
    message("Hyperion lib dir $ENV{HYPERION_LIB_DIR}")
    message("Target: ${TARGET}")

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_FLAGS "-O2")

    string(TIMESTAMP t0)
    add_compile_definitions(PROJECT_NAME="${CMAKE_PROJECT_NAME}")
    add_compile_definitions(BUILD_TIME="${t0}")

    if (NOT ("${TARGET}" STREQUAL "macos" OR "${TARGET}" STREQUAL "linux" OR "${TARGET}" STREQUAL "rpi"))
        config_esp()
    endif()
endmacro()

macro(hyperion_after)
    if ("${TARGET}" STREQUAL "macos" OR "${TARGET}" STREQUAL "linux" OR "${TARGET}" STREQUAL "rpi")
        config_unix()
    endif ()
endmacro()


macro(config_unix)
    add_executable(${PROJECT} "main.cpp")

    message("adding subdir $ENV{HYPERION_LIB_DIR}")
    add_subdirectory($ENV{HYPERION_LIB_DIR} "hyperion")
    target_link_libraries(${PROJECT} PUBLIC Hyperion)
    target_include_directories(${PROJECT} PUBLIC $ENV{HYPERION_LIB_DIR})

    #create a link to the binary under a static name so the run script can find it
    ADD_CUSTOM_TARGET(link_target ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT} "app")
endmacro()

macro(config_esp)

    # incude the hyperion directory as component to be built
    set(EXTRA_COMPONENT_DIRS $ENV{HYPERION_LIB_DIR})

    # esp-idf expects a 'main' component folder. If there is none we will copy one from the library.
    # This folder contains a CMakeLists.txt and main.cpp which includes ../../../main.cpp
    # Now you can keep your project folder clean, and only have a main.cpp file there
    if (NOT EXISTS "main")
        file(COPY "$ENV{HYPERION_LIB_DIR}/build-utils/esp-idf-main-component/main" DESTINATION ${CMAKE_BINARY_DIR})
        set(EXTRA_COMPONENT_DIRS ${EXTRA_COMPONENT_DIRS} "${CMAKE_BINARY_DIR}/main")
    endif()

    # hide the sdkconfig in the build folder
    set (SDKCONFIG "${CMAKE_BINARY_DIR}/sdkconfig")

    # load the defaults for this platform from hyperion library
    set(SDKCONFIG_DEFAULTS "$ENV{HYPERION_LIB_DIR}/platform/${TARGET}/sdkconfig.defaults")
    if(${SDKCONFIG_DEFAULTS} IS_NEWER_THAN ${SDKCONFIG})
        #remove the sdkconfig file so it is regenerated from the .defaults
        file(REMOVE ${SDKCONFIG})
    endif()
    
    # let the user override the defaults in the project file
    set(SDKCONFIG_DEFAULTS_LOCAL "${CMAKE_SOURCE_DIR}/sdkconfig.defaults")
    if(EXISTS ${SDKCONFIG_DEFAULTS_LOCAL})
        set(SDKCONFIG_DEFAULTS "${SDKCONFIG_DEFAULTS};${SDKCONFIG_DEFAULTS_LOCAL}")
        if(${SDKCONFIG_DEFAULTS_LOCAL} IS_NEWER_THAN ${SDKCONFIG})
            file(REMOVE ${SDKCONFIG})
        endif()
    endif()



    include($ENV{IDF_PATH}/tools/cmake/project.cmake)

    #create a link to the elf under a static name so the upload script can find it
    ADD_CUSTOM_TARGET(link_target ALL COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT}.elf" "firmware.elf")
endmacro()