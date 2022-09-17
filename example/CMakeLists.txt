add_compile_options(-Wall -g -O2 -pedantic)

# Example app SHARED
add_executable(ConsoleExample
    ConsoleExample.c
    split.c
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
)
target_include_directories(ConsoleExample_static PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(ConsoleExample_static LINK_PUBLIC
    external.sdl
    external.sdl_image
    SDL_console_static
)
