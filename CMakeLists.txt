cmake_minimum_required(VERSION 3.16)
project(VoleMachine LANGUAGES CXX)

# ---- language / build settings ------------------------------------------
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

# ---- core library: everything except main.cpp ----------------------------
set(VOLE_SOURCES
    src/ALU.cpp
    src/CPU.cpp
    src/Instruction.cpp
    src/Memory.cpp
    src/ProgramLoader.cpp
    src/RegisterFile.cpp
    src/Screen.cpp
    src/VoleMachine.cpp
)

foreach(optional MenuUI StateRenderer)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/${optional}.cpp)
        list(APPEND VOLE_SOURCES src/${optional}.cpp)
    endif()
endforeach()

add_library(vole_core STATIC ${VOLE_SOURCES})
target_include_directories(vole_core PUBLIC src)
target_compile_options(vole_core PRIVATE ${VOLE_WARNINGS})

# ---- application ----------------------------------------------------------
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/MenuUI.cpp)
    add_executable(vole src/main.cpp)
    target_link_libraries(vole PRIVATE vole_core)
    target_compile_options(vole PRIVATE ${VOLE_WARNINGS})
endif()

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

# ---- tests: one executable per track ---------------------------------------
enable_testing()

foreach(track core storage ui integration)
    set(test_src ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_${track}.cpp)
    set(test_size 0)
    if(EXISTS ${test_src})
        file(SIZE ${test_src} test_size)
    endif()

    if(test_size GREATER 0)
        add_executable(test_${track} ${test_src})
        target_link_libraries(test_${track} PRIVATE vole_core)
        target_compile_options(test_${track} PRIVATE ${VOLE_WARNINGS})
        target_compile_definitions(test_${track} PRIVATE
            VOLE_PROGRAMS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/programs")
        add_test(NAME test_${track} COMMAND test_${track}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endforeach()
