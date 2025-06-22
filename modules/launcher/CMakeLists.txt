file(GLOB_RECURSE LAUNCHER_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(zephyr-launcher STATIC ${LAUNCHER_SOURCES})

target_include_directories(zephyr-launcher
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(zephyr-launcher
        PUBLIC
        zephyr-core
        zephyr-bridge
        zephyr-shared
)

configure_zephyr_target(zephyr-launcher)