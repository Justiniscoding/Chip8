#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stack>
#include <string>

enum Flags{
	PRESERVE_VF = 1,
	KEEP_INDEX_REGISTER = 2,
	KEEP_EXECUTING_AFTER_DRAW = 4,
	USE_DRAW_WRAPPING = 8,
	SHIFT_VX_REGISTER = 16,
	USE_VX_FOR_JUMP = 32
};

class Chip8Emulator {
  public:
	Chip8Emulator(std::string filePath, int flags = 0);

	bool executeNextInstruction(int *frameBuffer,
								std::array<bool, 16> &pressedKeys);
	int delayTimer, soundTimer, flags;

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
