cmake_minimum_required(VERSION 3.16)

project(virtualfilesystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(virtualfilesystem main.cpp
    file.h file.cpp
    directory.h directory.cpp
    path.h path.cpp
    errors.h
    filesystem.h filesystem.cpp
    blockdevice.h blockdevice.cpp
    blockdevice.h blockdevice.cpp
    blockdevice.h blockdevice.cpp
    blockdevice.cpp
    fatfilesystem.h fatfilesystem.cpp
    fat_table.h fat_table.cpp)

include(GNUInstallDirs)
install(TARGETS virtualfilesystem
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
