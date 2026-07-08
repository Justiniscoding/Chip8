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

const int iterationsPerFrame = 1;

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
	SDL_Delay(170);
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
			ch8.executeNextInstruction(gFrameBuffer);
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
