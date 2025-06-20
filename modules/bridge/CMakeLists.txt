file(GLOB_RECURSE BRIDGE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(zephyr-bridge STATIC ${BRIDGE_SOURCES})

target_include_directories(zephyr-bridge
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(zephyr-bridge
        PUBLIC
        zephyr-core
)

configure_zephyr_target(zephyr-bridge)