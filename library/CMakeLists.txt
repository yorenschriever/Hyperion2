
cmake_minimum_required(VERSION 3.16)

message(STATUS "Building: Hyperion")

set(INCLUDE_DIRS 
    ${CMAKE_CURRENT_LIST_DIR} 
    "${CMAKE_CURRENT_LIST_DIR}/core" 
    "${CMAKE_CURRENT_LIST_DIR}/platform/includes"
    )
set(CORE_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/core" )

#create header files for the certificate and key, so we can import them
if(NOT EXISTS "$ENV{HYPERION_LIB_DIR}/scripts/certificate/hyperion.key")
    message([FATAL_ERROR] "\n\nYou need to generate a certificate first. Read the hyperion readme.md, and then run the following command to generate the certificates:\n\ncd $ENV{HYPERION_LIB_DIR}/scripts/certificate/\n./generate.sh\n\n")
endif()

if (${ESP_PLATFORM})

    idf_component_register(
        INCLUDE_DIRS ${INCLUDE_DIRS} 
        PRIV_REQUIRES driver esp_eth esp_netif esp_phy esp_timer vfs spiffs esp_http_server esp_https_server mbedtls nvs_flash fatfs
        SRC_DIRS 
            ${CORE_SRC_DIR} 
            "platform/${TARGET}" 
            "platform/${TARGET}/misc"
            "platform/${TARGET}/misc/mdns"
            "platform/includes"
            "core/generation/patterns/helpers/tempo"
            "misc"
        EMBED_TXTFILES
            "$ENV{HYPERION_LIB_DIR}/scripts/certificate/hyperion.crt"
            "$ENV{HYPERION_LIB_DIR}/scripts/certificate/hyperion.key"
    )

    # the next line is a workaround to a bug in esp-idf 5.1
    # https://github.com/espressif/esp-matter/issues/296
    idf_build_set_property(COMPILE_OPTIONS "-Wno-format-nonliteral;-Wno-format-security;-Wformat=0" APPEND)

    idf_component_optional_requires(PRIVATE espressif__mdns mdns)

    target_compile_options(${COMPONENT_LIB} PUBLIC "-Wno-maybe-uninitialized")

    file(GLOB_RECURSE WWW_FILES RELATIVE $ENV{HYPERION_LIB_DIR}/scripts/web $ENV{HYPERION_LIB_DIR}/scripts/web/*)
    foreach(WWW_FILE ${WWW_FILES})
        list(APPEND GZIP_COMMANDS
                COMMAND gzip -9 ${CMAKE_BINARY_DIR}/spiffs_image/${WWW_FILE} )
    endforeach()
    add_custom_target(copy-and-compress-files ALL DEPENDS $ENV{HYPERION_LIB_DIR}/scripts/web
            # clean up
            COMMAND rm -rf ${CMAKE_BINARY_DIR}/spiffs_image
            # copy spiffs files to build folder
            COMMAND ${CMAKE_COMMAND} -E copy_directory $ENV{HYPERION_LIB_DIR}/scripts/web ${CMAKE_BINARY_DIR}/spiffs_image
            # create gzip encoded www files
            ${GZIP_COMMANDS}
            )
    spiffs_create_partition_image(storage ${CMAKE_BINARY_DIR}/spiffs_image FLASH_IN_PROJECT DEPENDS copy-and-compress-files)


else ()

    file(GLOB PLATFORM_SRC CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/platform/${TARGET}/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/platform/includes/*.cpp"
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
        add_subdirectory($ENV{HYPERION_LIB_DIR}/platform/unix_common/rtmidi "rtmidi")
        target_include_directories(Hyperion PUBLIC $ENV{HYPERION_LIB_DIR}/platform/unix_common/rtmidi)
        set(RTMIDI "rtmidi")

        if ("${TARGET}" STREQUAL "rpi")
            target_compile_definitions(Hyperion PUBLIC __UNIX_JACK__ AVOID_TIMESTAMPING TRIM_MIDI_DEVICE_NAME)
        endif()

    endif()

    target_link_libraries(Hyperion PUBLIC ${RTMIDI} OpenSSL::SSL)
    target_include_directories(Hyperion PRIVATE $ENV{BOOST_DIR})

    target_compile_definitions(Hyperion PUBLIC WEBSERVER_ROOT="$ENV{HYPERION_LIB_DIR}/scripts/web")

    target_include_directories(Hyperion PUBLIC ${CMAKE_BINARY_DIR}/cert)
    if("$ENV{HYPERION_LIB_DIR}/scripts/certificate/hyperion.key" IS_NEWER_THAN "${CMAKE_BINARY_DIR}/cert/hyperion.key.h")
        include("$ENV{HYPERION_LIB_DIR}/build-utils/bin2h.cmake")
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/cert)
        bin2h(SOURCE_FILE "$ENV{HYPERION_LIB_DIR}/scripts/certificate/hyperion.crt" HEADER_FILE "${CMAKE_BINARY_DIR}/cert/hyperion.crt.h" VARIABLE_NAME "HYPERION_CERT" NULL_TERMINATE "1")
        bin2h(SOURCE_FILE "$ENV{HYPERION_LIB_DIR}/scripts/certificate/hyperion.key" HEADER_FILE "${CMAKE_BINARY_DIR}/cert/hyperion.key.h" VARIABLE_NAME "HYPERION_KEY" NULL_TERMINATE "1")
    endif()

endif ()