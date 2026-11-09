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
```

## Requirements

- CMake 3.16 or newer
- A C++17 compiler
- Qt 6 Widgets for the GUI (optional)
- Ninja is optional; any CMake generator works

The CLI has no Qt dependency. The GUI target is built only when Qt 6 Widgets is
found. If CMake prints `Qt6 Widgets not found; skipping vole_gui`, set
`CMAKE_PREFIX_PATH` to your Qt kit or pass `-DVOLE_BUILD_GUI=OFF` to build the
CLI only.

## Linux build

Install the compiler, CMake, Ninja, and the Qt 6 Widgets development package.

Fedora:

```bash
sudo dnf install gcc-c++ cmake ninja-build qt6-qtbase-devel
```

Debian / Ubuntu:

```bash
sudo apt install build-essential cmake ninja-build qt6-base-dev
```

Arch Linux:

```bash
sudo pacman -S base-devel cmake ninja qt6-base
```

Build from the repository root:

```bash
cmake -S . -B build -G Ninja
cmake --build build -j
```

Run:

```bash
./build/vole
./build/vole_gui
```

If Ninja is not installed, omit `-G Ninja` and use the default generator.
Both executables are placed in `build/` for single-config generators.

## Windows build

Qt and the compiler must be the same flavour:

- MinGW Qt must be built with a MinGW compiler.
- MSVC Qt must be built with MSVC.

Mixing them can fail at link time or at startup.

### Option A: MinGW 64-bit (recommended)

This matches the common Qt Online Installer MinGW kit.

1. Install Qt 6 with the Qt Online Installer. Select the MinGW 64-bit desktop
   kit and the Qt 6 Widgets component. Install CMake and Ninja from the Qt
   installer tools, or use the versions already on the machine.
2. Open the Qt command prompt for that kit, for example
   `Qt 6.8.0 (MinGW 11.2.0 64-bit)`. Alternatively, run
   `<Qt-installation-folder>\bin\qtenv2.bat` in a normal command prompt. This
   puts Qt's `bin` and the MinGW tools on `PATH`.
3. From the repository root:

```bat
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/mingw_64"
cmake --build build -j
```

4. Run from the same Qt command prompt:

```bat
build\vole.exe
build\vole_gui.exe
```

The Qt prompt and Qt Creator already add Qt's `bin` directory to `PATH`, so the
executables can find the Qt DLLs while running from that environment.

### Option B: MSVC 2022 64-bit

1. Install Qt 6 with the Qt Online Installer, select the MSVC 2022 64-bit kit,
   and enable Qt 6 Widgets.
2. Open "x64 Native Tools Command Prompt for VS 2022".
3. From the repository root:

```bat
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/msvc2022_64"
cmake --build build
build\vole.exe
build\vole_gui.exe
```

Visual Studio generator alternative:

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/msvc2022_64"
cmake --build build --config Release
build\Release\vole.exe
build\Release\vole_gui.exe
```

Replace `6.8.0` with the installed Qt version. CMake paths use forward slashes
on Windows. If Ninja is used from a normal prompt, it must be the Ninja from
the Visual Studio environment.

### Deploying the GUI for a standalone run

Running `vole_gui.exe` from Qt Creator or a Qt command prompt works because Qt
is on `PATH`. Double-clicking the same executable from Explorer does not add Qt
to `PATH`; the executable then fails with a missing-DLL message.

Deploy the Qt runtime next to the executable with `windeployqt` from the same
Qt kit:

```bat
C:\Qt\6.8.0\mingw_64\bin\windeployqt.exe --release build\vole_gui.exe
```

or, for MSVC:

```bat
C:\Qt\6.8.0\msvc2022_64\bin\windeployqt.exe --release build\vole_gui.exe
```

If `windeployqt` is already on `PATH`, the full path is not needed. For a
Visual Studio build, deploy `build\Release\vole_gui.exe` instead.

`windeployqt` copies the Qt DLLs, the Windows platform plugin
(`platforms\qwindows.dll`), and the compiler runtime. If the compiler runtime
was skipped, add `--compiler-runtime` explicitly.

The CLI executable `vole.exe` does not need Qt DLLs. Only `vole_gui.exe` does.

### Missing-DLL troubleshooting

| Message | Cause and fix |
|---|---|
| `Qt6Core.dll`, `Qt6Gui.dll`, or `Qt6Widgets.dll` not found | Run `windeployqt` from the same Qt kit, or run from the Qt command prompt. |
| `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, or `libwinpthread-1.dll` not found | MinGW runtime missing. Run `windeployqt --compiler-runtime`, or add `C:\Qt\Tools\mingw...\bin` to `PATH`. |
| `VCRUNTIME140.dll` or `MSVCP140.dll` not found | Install the Microsoft Visual C++ Redistributable (x64) for Visual Studio 2015-2022. |
| `0xc000007b` | Architecture or toolchain mismatch: use a 64-bit Qt kit with a 64-bit compiler, and do not mix MinGW and MSVC. |
| `could not find or load the Qt platform plugin "windows"` | Deploy the platform plugin, normally `platforms\qwindows.dll`; `windeployqt` does this. |
| Configure says `Qt6 Widgets not found` | `CMAKE_PREFIX_PATH` points to the wrong kit. Delete the build directory, check the Qt path, and configure again. |

The Qt kit and the compiler must match. A MinGW Qt build with MSVC, or a 32-bit
kit with a 64-bit compiler, will not run.

### Build only the CLI

```bat
cmake -S . -B build -DVOLE_BUILD_GUI=OFF
cmake --build build
```

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

## References

- Qt 6 Windows deployment: https://doc.qt.io/qt-6/windows-deployment.html
- Qt 6 CMake setup: https://doc.qt.io/qt-6/cmake-get-started.html
- Qt 6 command-line CMake builds: https://doc.qt.io/qt-6/cmake-build-on-cmdline.html
