find_package(Java REQUIRED)
find_package(JNI REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(Catch2 REQUIRED)
find_package(harfbuzz REQUIRED)

include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

function(configure_apple_platform)
    if(APPLE)
        set(COREGRAPHICS_LIB_PATH "${CMAKE_SOURCE_DIR}/extern/libcoregraphics.a")

        set(PLATFORM_LIBS
                "-framework Cocoa"
                "-framework OpenGL"
                "-framework QuartzCore"
                ${COREGRAPHICS_LIB_PATH}
                PARENT_SCOPE
        )
    endif()
endfunction()

function(configure_jni_paths)
    set(JNI_HEADER "jni.h")
    set(JNI_MD_HEADER "jni_md.h")

    if (NOT JAVA_FOUND OR NOT JNI_FOUND)
        message(FATAL_ERROR "Java or JNI not found.")
    endif ()

    find_path(JNI_H_PATH ${JNI_HEADER} PATHS ${JNI_INCLUDE_DIRS} REQUIRED)
    find_path(JNI_H_MD_PATH ${JNI_MD_HEADER} PATHS ${JNI_INCLUDE_DIRS} REQUIRED)

    if (NOT JNI_H_PATH OR NOT JNI_H_MD_PATH)
        message(FATAL_ERROR "JNI header not found.")
    endif ()

    message(STATUS "Found JNI.")

    set(JNI_SPECIFIC_INCLUDES
            ${JNI_INCLUDE_DIRS}
            ${JNI_OS_SPECIFIC_INCLUDE_DIR}
            PARENT_SCOPE
    )
endfunction()

function(configure_test)
    if (NOT Catch2_FOUND)
        message(FATAL_ERROR "Catch2 not found.")
    endif ()

    message(STATUS "Found Catch2.")

    include(CTest)
    include(Catch)
endfunction()

configure_apple_platform()
configure_jni_paths()
configure_test()

set(COMMON_INCLUDE_DIRS
        ${JNI_SPECIFIC_INCLUDES}
        ${CMAKE_BINARY_DIR}
)

set(COMMON_LIBS
        ${PLATFORM_LIBS}
        ${JNI_LIBRARIES}
        glm::glm
        Freetype::Freetype
        Catch2::Catch2
        harfbuzz::harfbuzz
        nlohmann_json::nlohmann_json
)