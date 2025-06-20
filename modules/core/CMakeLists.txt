add_subdirectory(graphics)
add_subdirectory(app)

add_library(zephyr-core INTERFACE)

target_link_libraries(zephyr-core INTERFACE
        zephyr-core-graphics
        zephyr-core-app
)