# Chip8 Emulator
Chip8 is an interpreted programming language originally created for the COSMAC VIP microcomputer. It runs on a 64x32 grid and can create basic games in two colors with 16 keys of input! Chip8 emulators have become the standard for learning about emulator development, which led me to create this project as a stepping stone to my eventual goal of making a game boy emulator. 

Upon running this program, you will be prompted to enter the path to a Chip8 ROM/file, and it will then open a window of the emulator running that page. By default, the background colour is black, and the foreground colour is dodgerblue (#1e90ff). Both of these can be customized in the settings menu, which is open upon first launch and can be toggled by pressing `\``.

**Emulator features:**
- Customizable iterations per frame (ipf) for various program speeds.
- All [six Chip8 quirks](https://github.com/Timendus/chip8-test-suite#quirks-test) are toggleable for maximum compatibility with ROMs.
- Customizable foreground and background colours with Dear ImGui colour picker.
- Ability to enable and disable logging at will for debugging purposes.
- Quirk restart button that wipes all past state and loads the ROM from scratch.

This project uses C++ with SDL3 and Dear ImGui and has a pre-built binary for MacOS. It can be found in the releases page here.

For other platforms, you will have to compile the source yourself. If you have `clang++`, `SDL3`, and `pkg-config` in your PATH, it should be as simple as:
```
git clone https://github.com/Justiniscoding/Chip8
cd Chip8
make
```
Note that operating systems other than MacOS are untested and the feature set may be limited.
