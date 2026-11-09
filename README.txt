# Vole Machine Simulator

CS213 Object-Oriented Programming, Assignment 2.

A C++17 VoLE machine simulator with a command-line interface and an optional
Qt 6 Widgets GUI. Memory is 256 bytes, PC is 8-bit, IR is 16-bit, and stores
to address FF go to the screen instead of RAM.

## Layout

```text
CMakeLists.txt      CLI, core library, optional GUI
src/                simulator, CLI, and GUI sources
programs/           sample VoLE programs
programs2/          extra test programs
screenshots/        GUI runs of the test programs
```

## Linux

Install a C++17 compiler, CMake, Ninja, and Qt 6 Widgets.

Fedora:

```bash
sudo dnf install gcc-c++ cmake ninja-build qt6-qtbase-devel
```

Debian / Ubuntu:

```bash
sudo apt install build-essential cmake ninja-build qt6-base-dev
```

Build and run:

```bash
cmake -S . -B build -G Ninja
cmake --build build -j
./build/vole
./build/vole_gui
```

## Windows

Use a Qt kit and compiler of the same flavour: MinGW Qt with MinGW, MSVC Qt
with MSVC.

### MinGW 64-bit

Install Qt 6 with the MinGW 64-bit kit and Qt 6 Widgets. Open the Qt MinGW
command prompt, or run `<Qt-folder>\bin\qtenv2.bat`.

```bat
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64"
cmake --build build -j
build\vole.exe
build\vole_gui.exe
```

### MSVC 2022 64-bit

Install Qt 6 with the MSVC 2022 64-bit kit and Qt 6 Widgets. Open the
"x64 Native Tools Command Prompt for VS 2022".

```bat
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2022_64"
cmake --build build
build\vole.exe
build\vole_gui.exe
```

Visual Studio generator alternative:

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2022_64"
cmake --build build --config Release
build\Release\vole.exe
build\Release\vole_gui.exe
```

### Running the GUI outside Qt Creator

Running from Qt Creator or the Qt command prompt works because Qt is on
`PATH`. A double-clicked `vole_gui.exe` needs the Qt DLLs deployed next to it:

```bat
C:\Qt\6.x.x\mingw_64\bin\windeployqt.exe --release build\vole_gui.exe
```

Use the matching MSVC path for an MSVC build. `windeployqt` copies the Qt
DLLs, the Windows platform plugin, and the compiler runtime.

If the GUI still fails:

- Missing `Qt6Core.dll`, `Qt6Gui.dll`, or `Qt6Widgets.dll`: run
  `windeployqt`, or run from the Qt command prompt.
- Missing `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, or `libwinpthread-1.dll`:
  the MinGW runtime is missing; add `--compiler-runtime` to `windeployqt`.
- Missing `VCRUNTIME140.dll` or `MSVCP140.dll`: install the Microsoft Visual
  C++ Redistributable (x64).
- `0xc000007b`: 32/64-bit or MinGW/MSVC mismatch.

`vole.exe` does not need Qt DLLs.

### CLI-only build

```bat
cmake -S . -B build -DVOLE_BUILD_GUI=OFF
cmake --build build
```

If `vole_gui` is missing after configuring, Qt was not found; set
`CMAKE_PREFIX_PATH` to the correct Qt kit.

## Usage

CLI menu:

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

GUI controls: Load File, Set Address, Run/Stop, Single Step, Clear Memory,
Reset CPU. The window shows the 16x16 memory matrix, registers, PC, IR, halt
status, and screen output.

`programs/program1.vol` prints `0F`. `programs/program2.vol` prints `00`.
