cmake_minimum_required(VERSION 3.20)
project(zephyr-app LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(JNI REQUIRED)
find_package(glm REQUIRED)
find_package(Freetype REQUIRED)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

if(APPLE)
    find_package(OpenGL REQUIRED)
    set(SOURCE_FILES
            modules/graphics/darwin/src/app.cpp
    )

    set(COREGRAPHICS_LIB_PATH "${CMAKE_SOURCE_DIR}/external/libzcg.a")
    set(NATIVEBRIDGE_LIB_PATH "${CMAKE_SOURCE_DIR}/external/libznb.a")

    add_executable(ZephyrApp ${SOURCE_FILES})
    target_link_libraries(ZephyrApp PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework QuartzCore" ${COREGRAPHICS_LIB_PATH} ${NATIVEBRIDGE_LIB_PATH} )
elseif(UNIX)
    set(FRONTEND_SRC src/linux/main.cpp)
    set(PLATFORM_LIBS glfw GL X11 pthread dl)

    add_executable(ZephyrApp ${FRONTEND_SRC})
    target_link_libraries(ZephyrApp PRIVATE ${PLATFORM_LIBS})

else()
    message(FATAL_ERROR "Unsupported platform")
endif()

#temporary
target_include_directories(ZephyrApp PRIVATE ${JNI_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/modules/graphics/darwin/include ${CMAKE_SOURCE_DIR}/external/include ${CMAKE_BINARY_DIR})

target_link_libraries(ZephyrApp PRIVATE ${PLATFORM_LIBS} ${JNI_LIBRARIES} glm::glm Freetype::Freetype)