# Find libraries

find_package(SDL2 REQUIRED)
find_package(SDL2_image)
if(SDL2_image_FOUND)
    add_definitions(-DHAVE_SDLIMAGE)
else()
    add_library(SDL2_image::SDL2_image INTERFACE IMPORTED)
endif()




add_compile_options(-Wall -g -O2 -pedantic)

# Static SDL_console library
add_library(SDL_console_static STATIC
    SDL_console.c
    DT_drawtext.c
    utf8.c
)
target_include_directories(SDL_console_static PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(SDL_console_static PUBLIC
    SDL2::SDL2
    SDL2_image::SDL2_image
)


# Shared SDL_console library
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(SDL_console SHARED
    SDL_console.c
    DT_drawtext.c
    utf8.c
)
set_target_properties(SDL_console PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(SDL_console PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(SDL_console PROPERTIES PUBLIC_HEADER ${CMAKE_SOURCE_DIR}/include/SDL_console.h)
set_property(TARGET SDL_console PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(SDL_console PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(SDL_console PUBLIC
    SDL2::SDL2
    SDL2_image::SDL2_image
)

include(GNUInstallDirs)
install(TARGETS SDL_console SDL_console_static
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SDL
)
