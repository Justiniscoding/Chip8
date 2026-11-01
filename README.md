# Chip8 Emulator
Chip8 is an interpreted programming language originally created for the COSMAC VIP microcomputer. It runs on a 64x32 grid and can create basic games in two colors with 16 keys of input! Chip8 emulators have become the standard for learning about emulator development, which led me to create this project as a stepping stone to my eventual goal of making a game boy emulator. 
<img width="1020" height="508" alt="image" src="https://github.com/user-attachments/assets/58655b85-6c91-4850-a897-f04183832393" />


## **Emulator features:**
- Customizable iterations per frame (ipf) for various program speeds.
- All [six Chip8 quirks](https://github.com/Timendus/chip8-test-suite#quirks-test) are toggleable for maximum compatibility with ROMs.
- Customizable foreground and background colours with Dear ImGui colour picker.
- Ability to enable and disable logging at will for debugging purposes.
- Quirk restart button that wipes all past state and loads the ROM from scratch.
- Protection against out of bounds reads and writes with the index register/memory

## Tests
Screenshots from running test ROMs that prove this emulator is working!
<img width="1013" height="503" alt="image" src="https://github.com/user-attachments/assets/3746c511-4b2a-423b-9969-d8fc9cd480ea" />
<img width="1020" height="509" alt="image" src="https://github.com/user-attachments/assets/9ce6818e-f7d7-4858-b764-0bd00cb73a61" />
<img width="1023" height="510" alt="image" src="https://github.com/user-attachments/assets/8a4a33b2-4be7-44d0-a8fb-2b768b892c93" />
<img width="1024" height="514" alt="image" src="https://github.com/user-attachments/assets/1128a7b2-1bd1-4f6c-bcdb-ecbd97127ec9" />


## Installation

This project uses C++ with SDL3 and Dear ImGui and has a pre-built binary for MacOS. It can be found in the releases page [here](https://github.com/Justiniscoding/Chip8/releases/tag/v1.0.0).

To run the binary:
```sh
cd Downloads
chmod +x Chip8MacOS
xattr -d com.apple.quarantine Chip8MacOS # Disables the MacOS security check (Apple just wants me to pay $99/year for a dev license)
./Chip8MacOS
```

For **other platforms**, you will have to compile the source yourself. If you have `clang++`, `SDL3`, and `pkg-config` in your PATH, it should be as simple as:
```sh
git clone https://github.com/Justiniscoding/Chip8
cd Chip8
make
```
__Note that operating systems other than MacOS are untested and may not work.__


Upon running this program, you will be prompted to enter the path to a Chip8 ROM/file, and it will then open a window of the emulator running that page. By default, the background colour is black, and the foreground colour is dodgerblue (#1e90ff). Both of these can be customized in the settings menu, which is open upon first launch and can be toggled by pressing backtick (\`).
