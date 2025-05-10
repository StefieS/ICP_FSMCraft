file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    fsm/*.cpp
    gui/*.cpp
    io/*.cpp
    messages/*.cpp
    controllers/*/*.cpp
    networkHandler/*.cpp
    qtfsm/*.cpp
    main.cpp
)

list(REMOVE_ITEM SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/networkHandler/2main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qtfsm/3main.cpp
)

add_executable(fsmtool ${SOURCES})

target_include_directories(fsmtool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/fsm
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
)

target_link_libraries(fsmtool PRIVATE Qt5::Widgets Qt5::Qml)
