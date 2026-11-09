cmake_minimum_required(VERSION 3.16)
project(VoleMachine LANGUAGES CXX)

# ---- language / build settings ------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Warnings for our own code (the Definition of Done says: no warnings)
if(MSVC)
    set(VOLE_WARNINGS /W4)
else()
    set(VOLE_WARNINGS -Wall -Wextra -Wpedantic)
endif()

# ---- core library: everything except main.cpp ----------------------------
# Shared by the application and every test, so each .cpp is compiled once.
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

# Moaaz's files are added automatically as soon as they are pushed.
# Once they exist, move them into the list above.
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

# ---- tests: one executable per track ---------------------------------------
# Run all with:  ctest --test-dir build --output-on-failure
# A test file that is missing or still empty is skipped instead of breaking the build.
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
        # Absolute path, so tests find programs/*.vol from any working directory
        target_compile_definitions(test_${track} PRIVATE
            VOLE_PROGRAMS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/programs")
        add_test(NAME test_${track} COMMAND test_${track}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endforeach()
