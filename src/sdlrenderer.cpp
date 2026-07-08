#include "sdlrenderer.hpp"
#include "constants.hpp"

SDLRenderer::SDLRenderer() {
	frameBuffer = new int[CHIP8_WIDTH * CHIP8_HEIGHT];

	window = SDL_CreateWindow("Chip8Emulator", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, NULL);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
								SDL_TEXTUREACCESS_STREAMING, CHIP8_WIDTH,
								CHIP8_HEIGHT);

	SDL_SetTextureScaleMode(texture, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);

	for (int i = 0; i < CHIP8_HEIGHT * CHIP8_WIDTH; i++) {
		frameBuffer[i] = BG_COLOR;
	}
}

void SDLRenderer::updateDisplay() {
	char *pixels;
	int pitch;

	SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);

	memcpy(pixels, frameBuffer, CHIP8_WIDTH * CHIP8_HEIGHT * 4);

	SDL_UnlockTexture(texture);

	SDL_RenderTexture(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

bool SDLRenderer::shouldQuit() {
	SDL_Event e;

	if (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) {
			return true;
		}
		if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE) {
			return true;
		}
	}

	SDL_Delay(17);
	return false;
}

void SDLRenderer::quit() {
	SDL_DestroyTexture(texture);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}
