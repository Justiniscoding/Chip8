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

	SDL_Init(SDL_INIT_AUDIO);

	SDL_AudioSpec spec;
	spec.channels = 1;
	spec.format = SDL_AUDIO_F32;
	spec.freq = 8000;

	audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
											&spec, NULL, NULL);

	sineSample = 0;
}

void SDLRenderer::updateDisplay() {
	char *pixels;
	int pitch;

	SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);

	memcpy(pixels, frameBuffer, CHIP8_WIDTH * CHIP8_HEIGHT * 4);

	SDL_UnlockTexture(texture);

	SDL_RenderTexture(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_Delay(17);
}

void SDLRenderer::handleKeys(std::array<bool, 16> &pressedKeys,
							 bool &shouldQuit) {
	SDL_Event e;

	if (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) {
			shouldQuit = true;
			return;
		}
		if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE) {
			shouldQuit = true;
			return;
		}

		for (int i = 0; i < 16; i++) {
			if (e.key.scancode == scancodes[i]) {
				if (e.type == SDL_EVENT_KEY_DOWN) {
					pressedKeys[i] = true;
				} else if (e.type == SDL_EVENT_KEY_UP) {
					pressedKeys[i] = false;
				}
			}
		}
	}

	shouldQuit = false;
}

void SDLRenderer::beep() {
	if (SDL_GetAudioStreamQueued(audioStream) < 1000) {
		float samples[512];

		for (unsigned long i = 0; i < SDL_arraysize(samples); i++) {
			int frequency = 600;
			float phase = sineSample * frequency / 8000.0f;
			samples[i] = SDL_sinf(phase * 2 * SDL_PI_F);
			sineSample++;
		}

		sineSample %= 8000;

		SDL_PutAudioStreamData(audioStream, samples, sizeof(samples));
	}

	SDL_ResumeAudioStreamDevice(audioStream);
}

void SDLRenderer::stopBeep() { SDL_PauseAudioStreamDevice(audioStream); }

void SDLRenderer::quit() {
	SDL_DestroyTexture(texture);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyAudioStream(audioStream);
	SDL_Quit();
}
