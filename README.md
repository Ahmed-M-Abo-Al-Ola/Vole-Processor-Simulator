# Vole Machine Simulator

CS213 Object-Oriented Programming, Assignment 2.

A C++17 simulator for the VoLE machine with a command-line interface and an
optional Qt 6 Widgets GUI. Memory is 256 bytes, registers are R0-RF, PC is
8-bit, IR is 16-bit, and stores to address FF go to the screen instead of RAM.

## Layout

```text
CMakeLists.txt      CLI, core library, optional GUI
src/                all simulator, CLI, and GUI sources
programs/           sample VoLE programs
video_link.txt      GUI demonstration video link
```

## Requirements

- CMake 3.16 or newer
- A C++17 compiler
- Qt 6 Widgets for the GUI (optional)

The GUI target is skipped automatically when Qt 6 Widgets is not found. Pass
`-DVOLE_BUILD_GUI=OFF` to skip it explicitly.

## Linux build

```bash
cmake -S . -B build -G Ninja
cmake --build build -j
```

Run the CLI:

```bash
./build/vole
```

Run the GUI:

```bash
./build/vole_gui
```

Fedora Qt 6 development package:

```bash
sudo dnf install qt6-qtbase-devel
```

## Windows build

Install Qt 6 with the Qt Online Installer, select the MSVC 2022 64-bit kit,
and enable Qt 6 Widgets. Install CMake and Ninja, or use Qt Creator. From the
repository root in "x64 Native Tools Command Prompt for VS 2022":

```bat
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/msvc2022_64"
cmake --build build
build\vole.exe
build\vole_gui.exe
```

Copy the Qt runtime DLLs next to the GUI executable:

```bat
"C:\Qt\6.8.0\msvc2022_64\bin\windeployqt.exe" build\vole_gui.exe
```

Visual Studio generator alternative:

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/msvc2022_64"
cmake --build build --config Release
build\Release\vole.exe
build\Release\vole_gui.exe
```

Replace `6.8.0` with the installed Qt version. CMake paths use forward
slashes on Windows. To build only the CLI, add `-DVOLE_BUILD_GUI=OFF`.

Qt Creator: open the root `CMakeLists.txt`, select a Qt 6 desktop kit, then
configure and run `vole_gui` or `vole`.

## CLI menu

```text
1. Load a program
2. Edit a register
3. Edit memory
4. Display machine state
5. Step
6. Run
7. Exit
```

Load addresses are hexadecimal and must be even, from 00 to FE.

## GUI controls

- Load File: choose a program and load it at the current address
- Set Address: validate and normalize the load address
- Run/Stop: execute until halt or the step limit
- Single Step: execute one instruction
- Clear Memory: set all 256 memory bytes to 00
- Reset CPU: reset CPU, memory, and screen state

The GUI shows a live 16x16 memory matrix, all registers, PC, IR, halt status,
and screen output in hexadecimal and ASCII.

## Sample programs

`programs/program1.vol` adds three values and prints 0F.
`programs/program2.vol` performs a bitwise AND and prints 00.

Program files contain one 4-digit hexadecimal instruction per line.
