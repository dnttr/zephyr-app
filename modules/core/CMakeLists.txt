add_subdirectory(shared)
add_subdirectory(graphics)
add_subdirectory(app)

add_library(zephyr-core INTERFACE)

target_link_libraries(zephyr-core INTERFACE
        zephyr-core-shared
        zephyr-core-graphics
        zephyr-core-app
)