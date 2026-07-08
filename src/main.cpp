#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "chip8.hpp"

const int SCALING_FACTOR = 16;

const int CHIP8_WIDTH = 64;
const int CHIP8_HEIGHT = 32;

const int WINDOW_WIDTH = CHIP8_WIDTH * SCALING_FACTOR;
const int WINDOW_HEIGHT = CHIP8_HEIGHT * SCALING_FACTOR;

const int BG_COLOR = 0x000000ff;
const int FG_COLOR = 0x1e90ffff;

int *gFrameBuffer;
SDL_Window *gSDLWindow;
SDL_Renderer *gSDLRenderer;
SDL_Texture *gSDLTexture;

bool reachedProgramEnd = false;

bool update() {
	SDL_Event e;

	if (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) {
			return false;
		}
		if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE) {
			return false;
		}
	}

	char *pixels;
	int pitch;

	SDL_LockTexture(gSDLTexture, NULL, (void **)&pixels, &pitch);

	// std::cout << "The total is " << (CHIP8_WIDTH * CHIP8_HEIGHT * 4) << "\n";
	memcpy(pixels, gFrameBuffer, CHIP8_WIDTH * CHIP8_HEIGHT * 4);

	SDL_UnlockTexture(gSDLTexture);

	SDL_RenderTexture(gSDLRenderer, gSDLTexture, NULL, NULL);
	SDL_RenderPresent(gSDLRenderer);
	SDL_Delay(17);
	return true;
}

int main() {
	// std::cout << "Path to the ROM: ";

	std::string romPath = "./roms/IBM.ch8";
	// std::cin >> romPath;

	Chip8Emulator ch8(romPath);

	std::cout << "Initializing SDL3...\n";

	gFrameBuffer = new int[CHIP8_WIDTH * CHIP8_HEIGHT];
	gSDLWindow =
		SDL_CreateWindow("Chip8Emulator", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	gSDLRenderer = SDL_CreateRenderer(gSDLWindow, NULL);
	gSDLTexture = SDL_CreateTexture(gSDLRenderer, SDL_PIXELFORMAT_RGBA8888,
									SDL_TEXTUREACCESS_STREAMING, CHIP8_WIDTH,
									CHIP8_HEIGHT);

	SDL_SetTextureScaleMode(gSDLTexture, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);

	for (int i = 0; i < CHIP8_HEIGHT * CHIP8_WIDTH; i++) {
		gFrameBuffer[i] = BG_COLOR;
	}

	while (true) {
		if (reachedProgramEnd) {
			break;
		}
		for (int _ = 0; _ < 11; _++) {

			std::cout << "The program counter is at 0x" << std::hex
					  << ch8.programCounter << "\n";

			uint16_t currentInstruction = ch8.memory[ch8.programCounter] << 8 |
										  ch8.memory[ch8.programCounter + 1];

			std::cout << "The current instruction is 0x" << +currentInstruction
					  << "\n";

			ch8.programCounter += 2;

			switch (currentInstruction >> 12) {
				case (0x0):
					if (currentInstruction == 0x00E0) {
						for (int i = 0; i < CHIP8_HEIGHT * CHIP8_WIDTH; i++) {
							gFrameBuffer[i] = BG_COLOR;
						}
						std::cout << "Clearing the screen\n";
					} else if (currentInstruction == 0x00EE) {
						uint16_t newProgramCounter = ch8.stack.top();
						ch8.stack.pop();

						std::cout << "Setting the program counter to 0x"
								  << newProgramCounter << "\n";
						ch8.programCounter = newProgramCounter;
					}
					break;
				case (0x1):
					std::cout << "Jumping to address 0x"
							  << (currentInstruction & 0xFFF) << "\n";
					if ((currentInstruction & 0xFFF) == ch8.programCounter) {
						reachedProgramEnd = true;
					}
					ch8.programCounter = (currentInstruction & 0xFFF);
					break;
				case (0x2):
					ch8.stack.push(ch8.programCounter);
					std::cout << "Pushed 0x" << ch8.programCounter
							  << " to the stack and jumped to 0x"
							  << (currentInstruction & 0xFFF) << "\n";
					ch8.programCounter = currentInstruction & 0xFFF;
					break;
				case (0x3):
					break;
				case (0x6):
					std::cout << "Setting register V"
							  << (currentInstruction >> 8 & 0xF) << " to 0x"
							  << (currentInstruction & 0xFF) << "\n";
					ch8.registers[currentInstruction >> 8 & 0xF] =
						currentInstruction & 0xFF;
					break;
				case (0x7):
					std::cout << "Adding 0x" << (currentInstruction & 0xFF)
							  << " to V" << (currentInstruction >> 8 & 0xF)
							  << "\n";
					ch8.registers[currentInstruction >> 8 & 0xF] +=
						currentInstruction & 0xFF;
					break;
				case (0xA):
					std::cout << "Setting the index register to 0x"
							  << (currentInstruction & 0xFFF) << "\n";
					ch8.indexRegister = currentInstruction & 0xFFF;
					break;
				case (0xD):
					std::cout << "The x register is "
							  << (currentInstruction >> 8 & 0xF)
							  << " and the y register is "
							  << (currentInstruction >> 4 & 0xF) << "\n";
					uint8_t xCoordinate =
						ch8.registers[currentInstruction >> 8 & 0xF] & 63;
					uint8_t yCoordinate =
						ch8.registers[currentInstruction >> 4 & 0xF] & 31;
					uint8_t height = currentInstruction & 0xF;

					std::cout << "Drawing image at (" << +xCoordinate << ", "
							  << +yCoordinate << ") with height 0x" << +height
							  << "\n";

					ch8.registers[0xF] = 0;

					for (int row = 0; row < height; row++) {
						uint8_t pixelData = ch8.memory[ch8.indexRegister++];

						for (int xOffset = 0; xOffset < 8; xOffset++) {
							if ((pixelData >> (7 - xOffset) & 1) > 0) {
								int xPosition = xCoordinate + xOffset;
								int yPosition = yCoordinate + row;
								int bufferIndex =
									yPosition * CHIP8_WIDTH + xPosition;

								if (gFrameBuffer[bufferIndex] == BG_COLOR) {
									gFrameBuffer[bufferIndex] = FG_COLOR;
								} else {
									gFrameBuffer[bufferIndex] = BG_COLOR;
									ch8.registers[0xF] = 1;
								}
							}
						}
					}
					break;
			}
		}

		if (!update()) {
			break;
		}
	}

	SDL_DestroyTexture(gSDLTexture);
	SDL_DestroyWindow(gSDLWindow);
	SDL_DestroyRenderer(gSDLRenderer);
	SDL_Quit();
}
