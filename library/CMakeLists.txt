
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
        PRIV_REQUIRES driver esp_eth
        SRC_DIRS ${CORE_SRC_DIR} "platform/${TARGET}"
    )

else ()

    file(GLOB PLATFORM_SRC CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/platform/${TARGET}/*.cpp"
        "${CORE_SRC_DIR}/*.cpp"
    )

    add_library(Hyperion ${PLATFORM_SRC})
    target_include_directories(Hyperion PUBLIC ${INCLUDE_DIRS})

endif ()