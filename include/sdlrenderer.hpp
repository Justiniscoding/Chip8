#pragma once

#include <SDL3/SDL.h>

class SDLRenderer {
  public:
	SDLRenderer();

	int *frameBuffer;

	void updateDisplay();
	void quit();
	bool shouldQuit();

  private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
};
