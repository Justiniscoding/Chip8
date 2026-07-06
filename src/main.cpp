#include <cstdint>
#include <iostream>
#include <string>

#include "chip8.hpp"

int main() {
	// std::cout << "Path to the ROM: ";

	std::string romPath = "./roms/IBM.ch8";
	// std::cin >> romPath;

	Chip8Emulator ch8(romPath);

	while (true) {
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
				std::cout << "Clearing the screen\n";
			}
			break;
		case (0x1):
			std::cout << "Jumping to address 0x" << (currentInstruction & 0xFFF)
					  << "\n";
			ch8.programCounter = (currentInstruction & 0xFFF);
			break;
		case (0x6):
			std::cout << "Setting register V" << (currentInstruction & 0xF00)
					  << " to 0x" << (currentInstruction & 0xFF) << "\n";
			ch8.registers[currentInstruction & 0xF00] =
				ch8.registers[currentInstruction & 0xFF];
			break;
		case (0x7):
			std::cout << "Adding 0x" << (currentInstruction & 0xFF) << " to V"
					  << (currentInstruction & 0xF00) << "\n";
			ch8.registers[currentInstruction & 0xF00] +=
				currentInstruction & 0xFF;
			break;
		case (0xA):
			std::cout << "Setting the index register to 0x"
					  << (currentInstruction & 0xFFF) << "\n";
			ch8.indexRegister = currentInstruction & 0xFFF;
			break;
		case (0xD):
			int xCoordinate = ch8.registers[currentInstruction & 0xF00] & 63;
			int yCoordinate = ch8.registers[currentInstruction & 0xF0] & 31;
			int height = ch8.registers[currentInstruction & 0xF];

			std::cout << "Drawing image at (" << xCoordinate << ", "
					  << yCoordinate << ") with height 0x" << height << "\n";

			ch8.registers[0xF] = 0;
			break;
		}

		std::cin.get();
	}
}
