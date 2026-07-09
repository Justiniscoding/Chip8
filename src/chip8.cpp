#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include "chip8.hpp"
#include "constants.hpp"

Chip8Emulator::Chip8Emulator(std::string filePath) {
	registers.fill(0);
	memory.fill(0);

	lastFrameKeys.fill(false);

	this->loadFontIntoMemory();
	this->loadFileIntoMemory(filePath);

	delayTimer = 0;
	soundTimer = 0;

	indexRegister = 0;

	programCounter = 0x200;
}

void Chip8Emulator::loadFileIntoMemory(std::string filePath) {
	std::ifstream fin(filePath, std::ios::binary);

	std::cout << "ROM successfully opened\n";

	std::vector<char> bytes((std::istreambuf_iterator<char>(fin)),
							(std::istreambuf_iterator<char>()));

	int index = 0x200;

	for (char c : bytes) {
		this->memory[index++] = c;
	}

	std::cout << "Successfully loaded " << bytes.size()
			  << " bytes into memory\n";

	fin.close();
}

void Chip8Emulator::loadFontIntoMemory() {
	uint16_t font[] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (int i = 0; i < 80; i++) {
		this->memory[i] = font[i];
	}

	std::cout << "Font successfully loaded into memory\n";
}

void Chip8Emulator::executeNextInstruction(int *frameBuffer,
										   std::array<bool, 16> &pressedKeys) {
	std::cout << "The program counter is at 0x" << std::hex << programCounter
			  << "\n";

	uint16_t currentInstruction =
		memory[programCounter] << 8 | memory[programCounter + 1];

	std::cout << "The current instruction is 0x" << +currentInstruction << "\n";

	programCounter += 2;

	uint8_t N = currentInstruction & 0xF;
	uint8_t NN = currentInstruction & 0xFF;
	uint16_t NNN = currentInstruction & 0xFFF;
	uint8_t X = currentInstruction >> 8 & 0xF;
	uint8_t Y = currentInstruction >> 4 & 0xF;

	switch (currentInstruction >> 12) {
		case (0x0):
			if (NNN == 0xE0) {
				for (int i = 0; i < 64 * 32; i++) {
					frameBuffer[i] = BG_COLOR;
				}
				std::cout << "Clearing the screen\n";
			} else if (NNN == 0xEE) {
				uint16_t newProgramCounter = stack.top();
				stack.pop();

				std::cout << "Setting the program counter to 0x"
						  << +newProgramCounter << "\n";
				programCounter = newProgramCounter;
			}
			break;
		case (0x1):
			std::cout << "Jumping to address 0x" << +NNN << "\n";
			programCounter = NNN;
			break;
		case (0x2):
			stack.push(programCounter);
			std::cout << "Pushed 0x" << +programCounter
					  << " to the stack and jumped to 0x" << +NNN << "\n";
			programCounter = NNN;
			break;
		case (0x3):
			if (registers[X] == NN) {
				programCounter += 2;
			}
			break;
		case (0x4):
			if (registers[X] != NN) {
				programCounter += 2;
			}
			break;
		case (0x5):
			if (N != 0) {
				std::cout << "Unknown variation of 0x5 received!\n";
				break;
			}
			if (registers[X] == registers[Y]) {
				programCounter += 2;
			}
			break;
		case (0x9):
			if (N != 0) {
				std::cout << "Unknown variation of 0x9 received!\n";
				break;
			}
			if (registers[X] != registers[Y]) {
				programCounter += 2;
			}
			break;
		case (0x6):
			std::cout << "Setting register V" << +X << " to 0x" << +NN << "\n";
			registers[X] = NN;
			break;
		case (0x7):
			std::cout << "Adding 0x" << +NN << " to V" << +X << "\n";
			registers[X] += NN;
			break;
		case (0x8): {
			uint8_t flag;
			switch (N) {
				case (0):
					registers[X] = registers[Y];
					break;
				case (0x1):
					registers[X] = registers[X] | registers[Y];
					break;
				case (0x2):
					registers[X] = registers[X] & registers[Y];
					break;
				case (0x3):
					registers[X] = registers[X] ^ registers[Y];
					break;
				case (0x4):
					registers[X] =
						static_cast<uint8_t>(registers[X] + registers[Y]);
					// TODO: this might not work so make sure to test :D
					registers[0xF] = registers[X] < registers[Y] ? 1 : 0;
					break;
				case (0x5):
					flag = registers[X] < registers[Y] ? 0 : 1;
					registers[X] = registers[X] - registers[Y];

					registers[0xF] = flag;
					break;
				case (0x7):
					flag = registers[Y] < registers[X] ? 0 : 1;
					registers[X] = registers[Y] - registers[X];

					registers[0xF] = flag;
					break;
				case (0x6):
					flag = registers[Y] & 1;
					registers[X] = registers[Y] >> 1;

					registers[0xF] = flag;
					break;
				case (0xE):
					flag = registers[Y] >> 7 & 1;
					registers[X] = registers[Y] << 1;

					registers[0xF] = flag;
					break;
			}
			break;
		}
		case (0xA):
			std::cout << "Setting the index register to 0x" << +NNN << "\n";
			indexRegister = NNN;
			break;
		case (0xB):
			programCounter = NNN + registers[0];
			break;
		case (0xC):
			registers[X] = rand() & NN;
			break;
		case (0xF):
			switch (NN) {
				case (0x0A):
					programCounter -= 2;
					for (int i = 0; i < 16; i++) {
						if (pressedKeys[i] == false && lastFrameKeys[i]) {
							registers[X] = i;
							programCounter += 2;
						}
					}
					break;
				case (0x7):
					registers[X] = delayTimer;
					break;
				case (0x15):
					delayTimer = registers[X];
					break;
				case (0x18):
					soundTimer = registers[X];
					break;
				case (0x1E):
					indexRegister += registers[X];
					break;
				case (0x29):
					indexRegister = (registers[X] & 0xF) * 5;
					break;
				case (0x55):
					for (int i = 0; i <= X; i++) {
						memory[indexRegister + i] = registers[i];
					}
					break;
				case (0x65):
					for (uint8_t i = 0; i <= X; i++) {
						registers[i] = memory[indexRegister + i];
					}
					break;
				case (0x33):
					uint8_t num = registers[X];
					memory[indexRegister] = num / 100;
					memory[indexRegister + 1] = num % 100 / 10;
					memory[indexRegister + 2] = num % 10;
					break;
			}
			break;
		case (0xD):
			std::cout << "The x register is " << +X << " and the y register is "
					  << +Y << "\n";
			registers[0xF] = 0;

			std::cout << "Drawing image at (0x" << +(registers[X] & 63)
					  << ", 0x" << +(registers[Y] & 31) << ") with height "
					  << +N << "\n";

			for (uint8_t row = 0; row < N; row++) {
				uint8_t pixelData = memory[indexRegister + row];

				for (int xOffset = 0; xOffset < 8; xOffset++) {
					if ((pixelData >> (7 - xOffset) & 1) > 0) {
						int xPosition = (registers[X] & 63) + xOffset;
						int yPosition = (registers[Y] & 31) + row;
						int bufferIndex = yPosition * 64 + xPosition;

						if (frameBuffer[bufferIndex] == BG_COLOR) {
							frameBuffer[bufferIndex] = FG_COLOR;
						} else {
							frameBuffer[bufferIndex] = BG_COLOR;
							registers[0xF] = 1;
						}
					}
				}
			}
			break;
		case (0xE):
			if (NN == 0x9E) {
				if (pressedKeys[registers[X]]) {
					programCounter += 2;
				}
			} else if (NN == 0xA1) {
				if (pressedKeys[registers[X]] == false) {
					programCounter += 2;
				}
			}
			break;
	}

	lastFrameKeys = pressedKeys;
}
