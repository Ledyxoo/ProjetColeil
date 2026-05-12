cmake_minimum_required(VERSION 3.15)
project(ProjetColeil C)

# Standard C et options du compilateur
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Options de compilation pour Windows (MSVC)
if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/W4)
else()
    # Options pour GCC/Clang (Linux, macOS, MinGW)
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Configuration des chemins d'inclusion
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(PROJECT_ASSETS_DIR ${CMAKE_SOURCE_DIR}/Assets)

# Dossiers d'inclusion (include tous les chemins)
include_directories(
    ${PROJECT_INCLUDE_DIR}
    ${PROJECT_INCLUDE_DIR}/Model
    ${PROJECT_INCLUDE_DIR}/VueController
)

# Collecte des fichiers sources
file(GLOB_RECURSE SOURCES 
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

# Recherche de SDL2
# Support pour passer SDL2_INCLUDE_DIR et SDL2_LIBRARY comme variables CMake
if(DEFINED SDL2_INCLUDE_DIR AND DEFINED SDL2_LIBRARY)
    message(STATUS "Using SDL2 from manual CMake variables")
    set(SDL2_FOUND TRUE)
    set(SDL2_INCLUDE_DIRS "${SDL2_INCLUDE_DIR}")
    set(SDL2_LIBRARIES "${SDL2_LIBRARY}")
else()
    find_package(SDL2 CONFIG QUIET)
    if(NOT SDL2_FOUND)
        # Fallback si SDL2 n'a pas de CMake config
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(SDL2 IMPORTED_TARGET sdl2)
        endif()
    endif()
endif()

if(NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 not found. Please install SDL2 development libraries or pass SDL2_INCLUDE_DIR and SDL2_LIBRARY variables.")
endif()

# Création de l'exécutable
add_executable(ProjetColeil ${SOURCES})

# Liaison avec SDL2
if(TARGET SDL2::SDL2)
    target_link_libraries(ProjetColeil PRIVATE SDL2::SDL2)
elseif(TARGET PkgConfig::SDL2)
    target_link_libraries(ProjetColeil PRIVATE PkgConfig::SDL2)
else()
    target_link_libraries(ProjetColeil PRIVATE ${SDL2_LIBRARIES})
    target_include_directories(ProjetColeil PRIVATE ${SDL2_INCLUDE_DIRS})
endif()

# Liaison avec SDL2_gfx et dépendances
# 1) Try CMake config package (vcpkg / config-mode)
find_package(SDL2_gfx CONFIG QUIET)
if(TARGET SDL2_gfx::SDL2_gfx)
    target_link_libraries(ProjetColeil PRIVATE SDL2_gfx::SDL2_gfx)
    message(STATUS "SDL2_gfx found via CMake config (SDL2_gfx::SDL2_gfx)")
elseif(TARGET SDL2_gfx)
    target_link_libraries(ProjetColeil PRIVATE SDL2_gfx)
    message(STATUS "SDL2_gfx found via CMake config (SDL2_gfx)")
else()
    # 2) Try pkg-config
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2_GFX IMPORTED_TARGET QUIET SDL2_gfx)
        if(SDL2_GFX_FOUND)
            target_link_libraries(ProjetColeil PRIVATE PkgConfig::SDL2_GFX)
            message(STATUS "SDL2_gfx found via pkg-config")
        endif()
    endif()

    # 3) Fallback: find library/header paths (useful for manual binaries on Windows)
    if(NOT SDL2_GFX_FOUND)
        if(DEFINED ENV{VCPKG_INSTALLATION_ROOT})
            set(VCPKG_INSTALLED_DIR "$ENV{VCPKG_INSTALLATION_ROOT}/installed/x64-windows")
            find_library(SDL2_GFX_LIB NAMES SDL2_gfx SDL2_gfx-1 SDL2_gfx-3.0
                         PATHS "${VCPKG_INSTALLED_DIR}/lib" NO_DEFAULT_PATH)
            find_path(SDL2_GFX_INCLUDE NAMES SDL2_gfxPrimitives.h
                      PATHS "${VCPKG_INSTALLED_DIR}/include" NO_DEFAULT_PATH)
        else()
            find_library(SDL2_GFX_LIB NAMES SDL2_gfx SDL2_gfx-1 SDL2_gfx-3.0)
            find_path(SDL2_GFX_INCLUDE NAMES SDL2_gfxPrimitives.h)
        endif()
        if(SDL2_GFX_LIB AND SDL2_GFX_INCLUDE)
            target_link_libraries(ProjetColeil PRIVATE ${SDL2_GFX_LIB})
            target_include_directories(ProjetColeil PRIVATE ${SDL2_GFX_INCLUDE})
            message(STATUS "SDL2_gfx found at ${SDL2_GFX_LIB}")
        else()
            message(FATAL_ERROR "SDL2_gfx library not found. Please install libSDL2-gfx development package.\n"
                                "On Ubuntu/Debian: sudo apt-get install libsdl2-gfx-dev\n"
                                "On Fedora/RHEL: sudo dnf install SDL2_gfx-devel\n"
                                "On macOS: brew install sdl2_gfx\n"
                                "On Windows: install via vcpkg (sdl2-gfx) or download prebuilt binaries from https://github.com/libsdl-org/SDL2_gfx/releases")
        endif()
    endif()
endif()

# Liaison avec les bibliothèques mathématiques sur Unix-like systems
if(UNIX AND NOT APPLE)
    target_link_libraries(ProjetColeil PRIVATE m)
endif()

# Copie automatique des Assets au répertoire de build
if(EXISTS ${PROJECT_ASSETS_DIR})
    add_custom_command(TARGET ProjetColeil POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_ASSETS_DIR}"
        "$<TARGET_FILE_DIR:ProjetColeil>/Assets"
        COMMENT "Copie des Assets vers le répertoire de build"
    )
endif()
