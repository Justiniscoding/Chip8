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

const int iterationsPerFrame = 11;

int *gFrameBuffer;
SDL_Window *gSDLWindow;
SDL_Renderer *gSDLRenderer;
SDL_Texture *gSDLTexture;

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

	memcpy(pixels, gFrameBuffer, CHIP8_WIDTH * CHIP8_HEIGHT * 4);

	SDL_UnlockTexture(gSDLTexture);

	SDL_RenderTexture(gSDLRenderer, gSDLTexture, NULL, NULL);
	SDL_RenderPresent(gSDLRenderer);
	SDL_Delay(17);
	return true;
}

int main() {
	// std::cout << "Path to the ROM: ";

	std::string romPath = "./roms/4-flags.ch8";
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
		if (ch8.delayTimer > 0) {
			ch8.delayTimer--;
		}
		if (ch8.soundTimer > 0) {
			ch8.soundTimer--;
		}

		for (int _ = 0; _ < iterationsPerFrame; _++) {
			std::cout << "The program counter is at 0x" << std::hex
					  << ch8.programCounter << "\n";

			uint16_t currentInstruction = ch8.memory[ch8.programCounter] << 8 |
										  ch8.memory[ch8.programCounter + 1];

			std::cout << "The current instruction is 0x" << +currentInstruction
					  << "\n";

			ch8.programCounter += 2;

			uint8_t N = currentInstruction & 0xF;
			uint8_t NN = currentInstruction & 0xFF;
			uint16_t NNN = currentInstruction & 0xFFF;
			uint8_t X = currentInstruction >> 8 & 0xF;
			uint8_t Y = currentInstruction >> 4 & 0xF;

			switch (currentInstruction >> 12) {
				case (0x0):
					if (NN == 0xE0) {
						for (int i = 0; i < CHIP8_HEIGHT * CHIP8_WIDTH; i++) {
							gFrameBuffer[i] = BG_COLOR;
						}
						std::cout << "Clearing the screen\n";
					} else if (NN == 0xEE) {
						uint16_t newProgramCounter = ch8.stack.top();
						ch8.stack.pop();

						std::cout << "Setting the program counter to 0x"
								  << newProgramCounter << "\n";
						ch8.programCounter = newProgramCounter;
					}
					break;
				case (0x1):
					std::cout << "Jumping to address 0x" << NNN << "\n";
					ch8.programCounter = NNN;
					break;
				case (0x2):
					ch8.stack.push(ch8.programCounter);
					std::cout << "Pushed 0x" << ch8.programCounter
							  << " to the stack and jumped to 0x" << NNN
							  << "\n";
					ch8.programCounter = NNN;
					break;
				case (0x3):
					if (ch8.registers[X] == NN) {
						ch8.programCounter += 2;
					}
					break;
				case (0x4):
					if (ch8.registers[X] != NN) {
						ch8.programCounter += 2;
					}
					break;
				case (0x5):
					if (N != 0) {
						std::cout << "Unknown variation of 0x5 received!\n";
						break;
					}
					if (ch8.registers[X] == ch8.registers[Y]) {
						ch8.programCounter += 2;
					}
					break;
				case (0x9):
					if (N != 0) {
						std::cout << "Unknown variation of 0x9 received!\n";
						break;
					}
					if (ch8.registers[X] != ch8.registers[Y]) {
						ch8.programCounter += 2;
					}
					break;
				case (0x6):
					std::cout << "Setting register V" << X << " to 0x" << NN
							  << "\n";
					ch8.registers[X] = NN;
					break;
				case (0x7):
					std::cout << "Adding 0x" << NN << " to V" << X << "\n";
					ch8.registers[X] += NN;
					break;
				case (0x8):
					switch (N) {
						case (0):
							ch8.registers[X] = ch8.registers[Y];
							break;
						case (0x1):
							ch8.registers[X] =
								ch8.registers[X] | ch8.registers[Y];
							break;
						case (0x2):
							ch8.registers[X] =
								ch8.registers[X] & ch8.registers[Y];
							break;
						case (0x3):
							ch8.registers[X] =
								ch8.registers[X] ^ ch8.registers[Y];
							break;
						case (0x4):
							ch8.registers[X] = static_cast<uint8_t>(
								ch8.registers[X] + ch8.registers[Y]);
							// TODO: this might not work so make sure to test :D
							ch8.registers[0xF] =
								ch8.registers[X] < ch8.registers[Y] ? 1 : 0;
							break;
						case (0x5):
							ch8.registers[0xF] =
								ch8.registers[X] >= ch8.registers[Y] ? 0 : 1;
							ch8.registers[X] =
								ch8.registers[X] - ch8.registers[Y];
							break;
						case (0x7):
							ch8.registers[0xF] =
								ch8.registers[Y] >= ch8.registers[X] ? 0 : 1;
							ch8.registers[X] =
								ch8.registers[Y] - ch8.registers[X];
							break;
						case (0x6):
							ch8.registers[X] = ch8.registers[Y] >> 1;
							ch8.registers[0xF] = ch8.registers[X] & 1;
							break;
						case (0xE):
							ch8.registers[X] = ch8.registers[Y] << 1;
							ch8.registers[0xF] =
								ch8.registers[currentInstruction >> 4] >> 7 & 1;
							break;
					}
				case (0xA):
					std::cout << "Setting the index register to 0x" << NNN
							  << "\n";
					ch8.indexRegister = NNN;
					break;
				case (0xB):
					ch8.programCounter = NNN + ch8.registers[0];
					break;
				case (0xC):
					ch8.registers[X] = rand() & NN;
					break;
				case (0xF):
					switch (NN) {
						case (0x7):
							ch8.registers[X] = ch8.delayTimer;
							break;
						case (0x15):
							ch8.delayTimer = ch8.registers[X];
							break;
						case (0x18):
							ch8.soundTimer = ch8.registers[X];
							break;
						case (0x1E):
							ch8.indexRegister += ch8.registers[X];
							break;
						case (0x29):
							ch8.indexRegister = N * 5;
							break;
						case (0x55):
							for (int i = 0; i <= X; i++) {
								ch8.memory[ch8.indexRegister + i] =
									ch8.registers[i];
							}
							break;
						case (0x65):
							for (uint8_t i = 0; i <= X; i++) {
								ch8.registers[i] =
									ch8.memory[ch8.indexRegister + i];
							}
							break;
						case (0x33):
							uint8_t num = ch8.registers[X];
							ch8.memory[ch8.indexRegister] = num / 100;
							ch8.memory[ch8.indexRegister + 1] = num % 100 / 10;
							ch8.memory[ch8.indexRegister + 2] = num % 10;
							break;
					}
					break;
				case (0xD):
					std::cout << "The x register is " << X
							  << " and the y register is " << Y << "\n";
					ch8.registers[0xF] = 0;

					for (int row = 0; row < N; row++) {
						uint8_t pixelData = ch8.memory[ch8.indexRegister++];

						for (int xOffset = 0; xOffset < 8; xOffset++) {
							if ((pixelData >> (7 - xOffset) & 1) > 0) {
								int xPosition =
									(ch8.registers[X] & 63) + xOffset;
								int yPosition = (ch8.registers[Y] & 31) + row;
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
		std::cout << "Register values:\n";
		for (int i = 0; i < 16; i++) {
			std::cout << "Register V" << i << " has a value of 0x"
					  << +ch8.registers[i] << "\n";
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
