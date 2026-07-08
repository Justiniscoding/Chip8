#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stack>
#include <string>

class Chip8Emulator {
  public:
	Chip8Emulator(std::string filePath);

	uint16_t programCounter;
	uint16_t indexRegister;

	int delayTimer, soundTimer;

	std::array<std::uint8_t, 4096> memory;
	std::array<std::uint8_t, 16> registers;
	std::stack<uint16_t> stack;

	void executeNextInstruction(int *frameBuffer);

  private:
	void loadFileIntoMemory(std::string filePath);
	void loadFontIntoMemory();
};
