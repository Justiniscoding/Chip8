#include <string>

#include "chip8.hpp"

int main() {
	// std::cout << "Path to the ROM: ";

	std::string romPath = "./roms/IBM.ch8";
	// std::cin >> romPath;

	Chip8Emulator ch8(romPath);
}
