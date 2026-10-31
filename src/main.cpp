#include <cstring>
#include <iostream>
#include <string>
#include <filesystem>

#include "chip8.hpp"
#include "config.hpp"
#include "sdlrenderer.hpp"

int main() {
	std::string romPath;

	std::cout << "Enter the path to the rom: ";

	std::cin >> romPath;

	while(!std::filesystem::is_regular_file(romPath)){
		std::cout << "No such file found, please try again: ";

		std::cin >> romPath;
	}

	Config config;

	Chip8Emulator ch8(romPath, &config);
	SDLRenderer renderer(&config);

	std::filesystem::exists("check");

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
			renderer.beep();
		} else {
			renderer.stopBeep();
		}

		for (int _ = 0; _ < config.iterationsPerFrame; _++) {
			if(config.restart){
				config.restart = false;

				ch8.restart(romPath);
			}

			if(ch8.executeNextInstruction(renderer.frameBuffer, pressedKeys)){
				break;
			}
		}

		renderer.updateDisplay();
	}

	renderer.quit();
}
