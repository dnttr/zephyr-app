function(configure_zephyr_target target)
    target_include_directories(
            ${target}
            PUBLIC
            ${COMMON_INCLUDE_DIRS}
            ${CMAKE_SOURCE_DIR}/extern/include
            ${CMAKE_SOURCE_DIR}/include
    )

    target_link_libraries(${target} PRIVATE ${COMMON_LIBS})

    if(APPLE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_target_properties(
                ${target}
                PROPERTIES
                LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
                OUTPUT_NAME "za"
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