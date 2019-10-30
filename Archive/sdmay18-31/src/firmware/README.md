# Firmware

This directory contains the firmware component for Senior Design Team SDMay18-31. The target board for the project is a TI Tiva C Series TM4C123G. This firmware component is designed to read multiple modular signals and relay them to a client over UART. FreeRTOS is the RTOS used for simple threading and scheduling for this software project.

## Dependencies
In order to compile this project you will need the following tools installed and added to your PATH:
* CMake, used as a meta-build system to configure the build-chain
* arm-none-eabi, compiler for this project. The following link may be helpful for installing this specific compiler on a Debian based system: https://launchpad.net/~team-gcc-arm-embedded/+archive/ubuntu/ppa

## Building on UNIX style systems
1. Make a `build` directory
2. Change into the `build` directory
3. Execute the following command: `cmake ..` to create the build configuration
4. Build by running the `make` command
5. A `firmware.elf` binary will be created in the build folder with debugging symbols. A `firmware.bin` file will be created in the build folder that can be loaded onto the Launchpad.

## Loading code onto the Launchpad
The `tools` directory contains two different utilities for flashing binaries onto the Launchpad. The `lmflash` utility is a GUI tool that can be used on Windows systems for flashing binaries. The `lm4flash` utility is a command line tool for UNIX systems for flashing binaries. The `lmicdiusb` is another UNIX command line utility that interfaces with GDB for debugging. The two UNIX utilities can be built by running the `make` command inside of the `tools` directory.

## Debugging using OpenOCD, GDB, and VSCode
**Due to driver issues, the following steps only work on linux and macOS**

The debugging tools require the following utilities to be installed on your computer:
1. The arm-none-eabi toolchain
2. openocd
3. cmake
4. g++
5. vscode, along with the "Native Debugging" extension

If you are using Ubuntu 16.04, you can run the the `setup_linux.sh` script as a super user, using `sudo` to install tools 1-4. The fifth tool, vscode, is an IDE that has a visual debugger that is useful for visual debugging.

1. Make sure that you have built the latest copy of the source code and have loaded the binary onto your launchpad. This step is necessary in order for the debugger to read your debugging symbols correctly. Use the steps in the "Building on UNIX style systems" step to do this.
2. Open a terminal in the firmware root folder.
3. Run the following command: `openocd -f debug.cfg`. This step creates a debugging server that is attached to the launchpad.
5. Open the firmware folder in VSCode. On your machine, there should be a `launch.json` file inside the .vscode folder under the firmware root folder. This file contains the configuration steps needed to start the debugger.
8. You should be able to go to the debug menu and start debugging by using the `Start Debugging` option.