#include <cstring>
#include <string>

#include "chip8.hpp"
#include "constants.hpp"
#include "sdlrenderer.hpp"

int main() {
	std::string romPath = "./roms/keypad.ch8";

	Chip8Emulator ch8(romPath);
	SDLRenderer renderer;

	std::array<bool, 16> pressedKeys;

	pressedKeys.fill(false);

	while (true) {
		bool shouldQuit = false;

		renderer.handleKeys(pressedKeys, shouldQuit);

		if (shouldQuit) {
			break;
		}

		if (ch8.delayTimer > 0) {
			ch8.delayTimer--;
		}
		if (ch8.soundTimer > 0) {
			ch8.soundTimer--;
		}

		for (int _ = 0; _ < ITERATIONS_PER_FRAME; _++) {
			ch8.executeNextInstruction(renderer.frameBuffer, pressedKeys);
		}

		renderer.updateDisplay();
	}

	renderer.quit();
}
