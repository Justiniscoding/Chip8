#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stack>
#include <string>

class Chip8Emulator {
  public:
	Chip8Emulator(std::string filePath);

	int programCounter;
	uint16_t indexRegister;

	int delayTimer, soundTimer;

	std::array<std::byte, 4096> memory;
	std::array<std::byte, 16> registers;
	std::stack<uint16_t> stack;

  private:
	void loadFileIntoMemory(std::string filePath);
	void loadFontIntoMemory();
};
