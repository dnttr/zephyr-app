file(GLOB_RECURSE GRAPHICS_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(zephyr-core-graphics STATIC ${GRAPHICS_SOURCES})

target_include_directories(zephyr-core-graphics
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(zephyr-core-graphics PUBLIC)
configure_zephyr_target(zephyr-core-graphics)