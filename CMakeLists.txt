cmake_minimum_required(VERSION 3.16)
project(VoleMachine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    set(VOLE_WARNINGS /W4)
else()
    set(VOLE_WARNINGS -Wall -Wextra -Wpedantic)
endif()

set(VOLE_SOURCES
    src/ALU.cpp
    src/CPU.cpp
    src/Instruction.cpp
    src/Memory.cpp
    src/ProgramLoader.cpp
    src/RegisterFile.cpp
    src/MenuUI.cpp
    src/Screen.cpp
    src/StateRenderer.cpp
    src/Text.cpp
    src/VoleMachine.cpp
)

add_library(vole_core STATIC ${VOLE_SOURCES})
target_include_directories(vole_core PUBLIC src)
target_compile_options(vole_core PRIVATE ${VOLE_WARNINGS})

add_executable(vole "src/A2-S43,S44-20250882_20250883_20260963_20250885.cpp")
target_link_libraries(vole PRIVATE vole_core)
target_compile_options(vole PRIVATE ${VOLE_WARNINGS})

option(VOLE_BUILD_GUI "Build the Qt GUI" ON)

if(VOLE_BUILD_GUI)
    find_package(Qt6 QUIET COMPONENTS Widgets)
    if(Qt6Widgets_FOUND)
        add_executable(vole_gui src/gui_main.cpp)
        set_target_properties(vole_gui PROPERTIES AUTOMOC ON)
        target_link_libraries(vole_gui PRIVATE vole_core Qt6::Widgets)
        target_compile_options(vole_gui PRIVATE ${VOLE_WARNINGS})
    else()
        message(STATUS "Qt6 Widgets not found; skipping vole_gui")
    endif()
endif()
