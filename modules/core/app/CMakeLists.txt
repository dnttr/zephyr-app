file(GLOB_RECURSE CORE_APP_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(zephyr-core-app STATIC ${CORE_APP_SOURCES})

target_include_directories(zephyr-core-app
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

configure_zephyr_target(zephyr-core-app)