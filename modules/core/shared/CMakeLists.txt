file(GLOB_RECURSE SHARED_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(zephyr-core-shared STATIC ${SHARED_SOURCES})

target_include_directories(zephyr-core-shared
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

configure_zephyr_target(zephyr-core-shared)