cmake_minimum_required(VERSION 3.20)
project(zephyr-app LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include and execute platform and dependency setup scripts
include(${CMAKE_SOURCE_DIR}/tools/Platform.cmake)
define_platform()

include(${CMAKE_SOURCE_DIR}/tools/Dependencies.cmake)

function(configure_zephyr_target target)
    target_include_directories(
            ${target}
            PUBLIC
            ${COMMON_INCLUDE_DIRS}
            ${CMAKE_SOURCE_DIR}/modules/graphics/darwin/include
            ${CMAKE_SOURCE_DIR}/modules/bridge/include
            ${CMAKE_SOURCE_DIR}/modules/app/include
            ${CMAKE_SOURCE_DIR}/include
    )

    target_link_libraries(${target} PRIVATE ${COMMON_LIBS})

    if(APPLE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_target_properties(
                ${target}
                PROPERTIES
                LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
                OUTPUT_NAME "znb"
                INSTALL_RPATH "@loader_path"
        )

        add_custom_command(
                TARGET ${target}
                POST_BUILD
                COMMAND
                ${CMAKE_INSTALL_NAME_TOOL} -change ${OLD_GLM_PATH}
                "@rpath/libglm.dylib" $<TARGET_FILE:${target}>
                COMMAND
                ${CMAKE_INSTALL_NAME_TOOL} -change ${OLD_FREETYPE_PATH}
                "@rpath/libfreetype.6.dylib" $<TARGET_FILE:${target}>
                COMMENT "Fixing dylib dependency paths for relocatability"
                VERBATIM
        )
    endif()
endfunction()

set(SOURCE_FILES
        modules/graphics/darwin/src/app.cpp
        modules/bridge/src/bridge.cpp
        modules/app/src/loader.cpp
)

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building executable for Debug configuration.")
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
else()
    message(STATUS "Building shared library for ${CMAKE_BUILD_TYPE} configuration.")
    add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
endif()

configure_zephyr_target(${PROJECT_NAME})