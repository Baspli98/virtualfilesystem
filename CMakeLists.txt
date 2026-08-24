cmake_minimum_required(VERSION 3.16)

project(virtualfilesystem
    VERSION 1.0.0
    LANGUAGES CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ============================================
# Library
# ============================================

add_library(virtualfilesystem_lib

    # Core
    src/core/directory.cpp
    src/core/file.cpp
    src/core/filesystem.cpp
    src/core/path.cpp

    # Storage
    src/storage/blockdevice.cpp

    # FAT Filesystem
    src/filesystems/fat/fatfilesystem.cpp
    src/filesystems/fat/fat_table.cpp

    # Inode Filesystem
    src/filesystems/inode/inode.cpp
    src/filesystems/inode/inodefilesystem.cpp
)


# ============================================
# Public Include Directories
# ============================================

target_include_directories(virtualfilesystem_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# ============================================
# Compiler Warnings
# ============================================

target_compile_options(virtualfilesystem_lib
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:
            -Wall
            -Wextra
            -Wpedantic
        >
)


# ============================================
# Example Application
# ============================================

add_executable(virtualfilesystem_example
    examples/main.cpp
)

target_link_libraries(virtualfilesystem_example
    PRIVATE
        virtualfilesystem_lib
)


# ============================================
# Install
# ============================================

include(GNUInstallDirs)

install(
    TARGETS virtualfilesystem_lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)