# Find libraries

find_package(SDL REQUIRED)
add_library               (external.sdl INTERFACE)
target_include_directories(external.sdl INTERFACE ${SDL_INCLUDE_DIRS})
target_link_libraries     (external.sdl INTERFACE ${SDL_LIBRARIES})

find_package(SDL_image)
add_library                   (external.sdl_image INTERFACE)
IF(SDL_IMAGE_FOUND)
    ADD_DEFINITIONS(-DHAVE_SDLIMAGE)
    target_include_directories(external.sdl_image INTERFACE ${SDL_IMAGE_INCLUDE_DIRS})
    target_link_libraries     (external.sdl_image INTERFACE ${SDL_IMAGE_LIBRARIES})
ENDIF()




add_compile_options(-Wall -g -O2 -pedantic)

# Static SDL_console library
add_library(SDL_console_static STATIC
    SDL_console.c
    internal.c
    DT_drawtext.c
)
target_include_directories(SDL_console_static PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(SDL_console_static
    external.sdl
)

# Shared SDL_console library
add_library(SDL_console SHARED
    SDL_console.c
    internal.c
    DT_drawtext.c
)
set_target_properties(SDL_console PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(SDL_console PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(SDL_console PROPERTIES PUBLIC_HEADER ${CMAKE_SOURCE_DIR}/include/SDL_console.h)
target_include_directories(SDL_console PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(SDL_console
    external.sdl
    external.sdl_image
)


include(GNUInstallDirs)
install(TARGETS SDL_console SDL_console_static
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SDL
)
