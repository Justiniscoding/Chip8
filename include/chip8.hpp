#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stack>
#include <string>

class Chip8Emulator {
  public:
	Chip8Emulator(std::string filePath);

	void executeNextInstruction(int *frameBuffer,
								std::array<bool, 16> &pressedKeys);
	int delayTimer, soundTimer;

  private:
	// Janitor Raus, you are a genius.
	struct Mem {
		std::array<uint8_t, 4096> raw;
		auto &operator[](std::size_t index) noexcept {
			return raw[index & 0xFFF];
		}
	};

	void loadFileIntoMemory(std::string filePath);
	void loadFontIntoMemory();

	uint16_t programCounter;
	uint16_t indexRegister;

	Mem memory;
	std::array<uint8_t, 16> registers;
	std::stack<uint16_t> stack;

	std::array<bool, 16> lastFrameKeys;
};
