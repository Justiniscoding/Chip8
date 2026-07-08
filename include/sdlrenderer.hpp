#pragma once

#include <SDL3/SDL.h>

#include <array>

class SDLRenderer {
  public:
	SDLRenderer();

	int *frameBuffer;

	void updateDisplay();
	void quit();
	void handleKeys(std::array<bool, 16> &pressedKeys, bool &shouldQuit);

  private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
};

const int scancodes[] = {
	SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
	SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7,
	SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_A, SDL_SCANCODE_B,
	SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E, SDL_SCANCODE_F};
