cmake_minimum_required(VERSION 3.10)

project(FSMCraft)

set(CMAKE_CXX_STANDARD 17)

# Correct path to Qt5
set(CMAKE_PREFIX_PATH "/usr/local/share/Qt-5.9.2/5.9.2/gcc_64/lib/cmake")
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Enable Qt automatic features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include paths
include_directories(src)
include_directories(src/gui)
include_directories(src/fsm)

# Explicitly list GUI sources (do NOT include test_main.cpp)
set(SOURCES
    src/main.cpp
    src/fsm/FSM.cpp
    src/fsm/State.cpp
    src/fsm/Transition.cpp
    src/gui/MainWindow.cpp
    src/gui/StateItem.cpp
)

add_executable(fsmtool ${SOURCES})
add_compile_options(-fno-sized-deallocation)

set_target_properties(fsmtool PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "/usr/local/share/Qt-5.9.2/5.9/gcc_64/plugins"
)

target_link_libraries(fsmtool Qt5::Widgets)
