
cmake_minimum_required(VERSION 3.16)

message(STATUS "Building: Hyperion")

set(INCLUDE_DIRS 
    ${CMAKE_CURRENT_LIST_DIR} 
    "${CMAKE_CURRENT_LIST_DIR}/core" 
    "${CMAKE_CURRENT_LIST_DIR}/platform/includes"
    )
set(CORE_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/core" )

if (${ESP_PLATFORM})

    idf_component_register(
        INCLUDE_DIRS ${INCLUDE_DIRS} 
        PRIV_REQUIRES driver esp_eth esp_netif esp_phy esp_timer 
        SRC_DIRS 
            ${CORE_SRC_DIR} 
            "platform/${TARGET}" 
            "platform/${TARGET}/misc"
            "core/generation/patterns/helpers/tempo"
            "misc"
    )

    target_compile_options(${COMPONENT_LIB} PUBLIC "-Wno-maybe-uninitialized")

else ()

    file(GLOB PLATFORM_SRC CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/platform/${TARGET}/*.cpp"
    )

    file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS
        "${CORE_SRC_DIR}/*.cpp"
        "misc/*.cpp"
        "misc/*.c"
    )

    add_library(Hyperion ${PLATFORM_SRC} ${CORE_SRC})
    target_include_directories(Hyperion PUBLIC ${INCLUDE_DIRS})

    find_package(Threads REQUIRED)
    if(THREADS_HAVE_PTHREAD_ARG)
      target_compile_options(Hyperion PUBLIC "-pthread")
    endif()
    if(CMAKE_THREAD_LIBS_INIT)
      target_link_libraries(Hyperion PRIVATE "${CMAKE_THREAD_LIBS_INIT}")
    endif()

    find_package(OpenSSL REQUIRED)

    if ("${TARGET}" STREQUAL "macos" OR "${TARGET}" STREQUAL "rpi")
        set(RTMIDI_BUILD_TESTING OFF)
        add_subdirectory($ENV{HYPERION_LIB_DIR}/platform/macos/rtmidi "rtmidi")
        target_include_directories(Hyperion PUBLIC $ENV{HYPERION_LIB_DIR}/platform/macos/rtmidi)
        set(RTMIDI "rtmidi")

        if ("${TARGET}" STREQUAL "rpi")
            target_compile_definitions(Hyperion PUBLIC __UNIX_JACK__ AVOID_TIMESTAMPING TRIM_MIDI_DEVICE_NAME)
        endif()

    endif()

    target_link_libraries(Hyperion PUBLIC ${RTMIDI} OpenSSL::SSL)
    target_include_directories(Hyperion PRIVATE $ENV{BOOST_DIR})

    #create header files for the certificate and key, so we can import them
    if(NOT EXISTS "$ENV{HYPERION_LIB_DIR}/scripts/certificate/hyperion.key")
        message([FATAL_ERROR] "\n\nYou need to generate a certificate first. Read the hyperion readme.md, and then run the following command to generate the certificates:\n\ncd $ENV{HYPERION_LIB_DIR}/scripts/certificate/\n./generate.sh\n\n")
    endif()
    if("$ENV{HYPERION_LIB_DIR}/scripts/certificate/hyperion.key" IS_NEWER_THAN "${CMAKE_BINARY_DIR}/cert/hyperion.key.h")
        include("$ENV{HYPERION_LIB_DIR}/build-utils/bin2h.cmake")
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/cert)
        bin2h(SOURCE_FILE "$ENV{HYPERION_LIB_DIR}/scripts/certificate/hyperion.crt" HEADER_FILE "${CMAKE_BINARY_DIR}/cert/hyperion.crt.h" VARIABLE_NAME "HYPERION_CERT")
        bin2h(SOURCE_FILE "$ENV{HYPERION_LIB_DIR}/scripts/certificate/hyperion.key" HEADER_FILE "${CMAKE_BINARY_DIR}/cert/hyperion.key.h" VARIABLE_NAME "HYPERION_KEY")
    endif()
    target_include_directories(Hyperion PUBLIC ${CMAKE_BINARY_DIR}/cert)

    target_compile_definitions(Hyperion PUBLIC WEBSERVER_ROOT="$ENV{HYPERION_LIB_DIR}/scripts/web")

endif ()