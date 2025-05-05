cmake_minimum_required(VERSION 3.10)
project(FSMCraft LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Allow override via environment for portability
if(NOT DEFINED ENV{Qt5_DIR})
    set(Qt5_DIR "/usr/local/share/Qt-5.5.1/lib/cmake/Qt5")
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Enable Qt MOC/UIC/RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Source files
add_subdirectory(src)
