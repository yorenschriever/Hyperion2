
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

    )

    target_compile_options(${COMPONENT_LIB} PUBLIC "-Wno-maybe-uninitialized")

else ()

    file(GLOB PLATFORM_SRC CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/platform/${TARGET}/*.cpp"
    )

    file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS
        "${CORE_SRC_DIR}/*.cpp"
    )

    add_library(Hyperion ${PLATFORM_SRC} ${CORE_SRC})
    target_include_directories(Hyperion PUBLIC ${INCLUDE_DIRS})

    find_package(Threads REQUIRED)
    if(THREADS_HAVE_PTHREAD_ARG)
      target_compile_options(Hyperion PUBLIC "-pthread")
    endif()
    if(CMAKE_THREAD_LIBS_INIT)
      target_link_libraries(Hyperion "${CMAKE_THREAD_LIBS_INIT}")
    endif()

    set(RTMIDI_BUILD_TESTING OFF)
    add_subdirectory($ENV{HYPERION_LIB_DIR}/platform/${TARGET}/rtmidi "rtmidi")
    target_link_libraries(Hyperion PUBLIC rtmidi)
    target_include_directories(Hyperion PUBLIC $ENV{HYPERION_LIB_DIR}/platform/${target}/rtmidi)
   

endif ()