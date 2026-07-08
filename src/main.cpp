#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstring>
#include <string>

#include "chip8.hpp"
#include "constants.hpp"
#include "sdlrenderer.hpp"

int main() {
	std::string romPath = "./roms/quirks.ch8";

	Chip8Emulator ch8(romPath);
	SDLRenderer renderer;

	while (true) {
		if (ch8.delayTimer > 0) {
			ch8.delayTimer--;
		}
		if (ch8.soundTimer > 0) {
			ch8.soundTimer--;
		}

		for (int _ = 0; _ < ITERATIONS_PER_FRAME; _++) {
			ch8.executeNextInstruction(renderer.frameBuffer);
		}

		renderer.updateDisplay();

		if (renderer.shouldQuit()) {
			break;
		}
	}

	renderer.quit();
}
