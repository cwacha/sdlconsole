add_compile_options(-Wall -g -O2 -pedantic)

# Example app SHARED
add_executable(ConsoleExample
    ConsoleExample.c
    split.c
    ${CMAKE_SOURCE_DIR}/src/DT_drawtext.c
)
target_include_directories(ConsoleExample PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(ConsoleExample LINK_PUBLIC
    external.sdl
    external.sdl_image
    SDL_console
)

# Example app STATIC
add_executable(ConsoleExample_static
    ConsoleExample.c
    split.c
    ${CMAKE_SOURCE_DIR}/src/DT_drawtext.c
)
target_include_directories(ConsoleExample_static PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(ConsoleExample_static LINK_PUBLIC
    external.sdl
    external.sdl_image
    SDL_console_static
)

#[[
include(GNUInstallDirs)
install(TARGETS ConsoleExample ConsoleExample_static
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES ${extra_dist}
        DESTINATION ${CMAKE_INSTALL_BINDIR}
)
]]

# hack to have build/example dir contain the extra files
# taken from https://stackoverflow.com/questions/10671916/how-to-copy-dll-files-into-the-same-folder-as-the-executable-using-cmake
file(GLOB extra_dist *.bmp *.gif *.jpg)
add_custom_command(TARGET ConsoleExample POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${extra_dist} $<TARGET_FILE_DIR:ConsoleExample>
)
add_custom_command(TARGET ConsoleExample_static POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${extra_dist} $<TARGET_FILE_DIR:ConsoleExample>
)

# on windows copy SDL.dll and libSDL_console.dll to build/example dir
file(GLOB extra_dll ${CMAKE_SOURCE_DIR}/*.dll ${CMAKE_BINARY_DIR}/src/*.dll)
if(extra_dll)
add_custom_command(TARGET ConsoleExample ConsoleExample_static POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${extra_dll} $<TARGET_FILE_DIR:ConsoleExample>
)
add_custom_command(TARGET ConsoleExample_static POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${extra_dll} $<TARGET_FILE_DIR:ConsoleExample>
)
endif()