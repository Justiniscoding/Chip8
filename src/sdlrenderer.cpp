#include "sdlrenderer.hpp"
#include "constants.hpp"
#include "config.hpp"

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

SDLRenderer::SDLRenderer(Config *config) {
	frameBuffer = new int[CHIP8_WIDTH * CHIP8_HEIGHT];

	window = SDL_CreateWindow("Chip8Emulator", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, NULL);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
								SDL_TEXTUREACCESS_STREAMING, CHIP8_WIDTH,
								CHIP8_HEIGHT);

	this->config = config;

	ImGui::CreateContext();

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	SDL_SetTextureScaleMode(texture, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);

	for (int i = 0; i < CHIP8_HEIGHT * CHIP8_WIDTH; i++) {
		frameBuffer[i] = config->getColor();
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
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();

	ImGui::NewFrame();

	if(config->settingsShown){
		ImGui::Begin("Settings");

		ImGui::ColorEdit3("Foreground Color", (float*)&(config->fgColor));
		ImGui::ColorEdit3("Background Color", (float*)&config->bgColor);

		ImGui::SliderInt("Iterations Per Frame", &config->iterationsPerFrame, 1, 30);

		ImGui::NewLine();

		ImGui::Checkbox("Enable Logging", &config->logging);

		ImGui::Text("Quirks");

		bool perserveVF = config->getQuirk(PRESERVE_VF);
		bool keepIndex = config->getQuirk(KEEP_INDEX_REGISTER);
		bool keepExecuting = config->getQuirk(KEEP_EXECUTING_AFTER_DRAW);
		bool useDrawWrap = config->getQuirk(USE_DRAW_WRAPPING);
		bool shiftVX = config->getQuirk(SHIFT_VX_REGISTER);
		bool useVX = config->getQuirk(USE_VX_FOR_JUMP);

		if(ImGui::Checkbox("Preserve VF", &perserveVF)){
			config->setQuirk(PRESERVE_VF, perserveVF);
		}

		if(ImGui::Checkbox("Keep Index Register", &keepIndex)){
			config->setQuirk(KEEP_INDEX_REGISTER, keepIndex);
		}

		if(ImGui::Checkbox("Keep Executing After Draw", &keepExecuting)){
			config->setQuirk(KEEP_EXECUTING_AFTER_DRAW, keepExecuting);
		}

		if(ImGui::Checkbox("Draw Wrapping", &useDrawWrap)){
			config->setQuirk(USE_DRAW_WRAPPING, useDrawWrap);
		}

		if(ImGui::Checkbox("Shift VX Register", &shiftVX)){
			config->setQuirk(SHIFT_VX_REGISTER, shiftVX);
		}

		if(ImGui::Checkbox("Use VX For Jump", &useVX)){
			config->setQuirk(USE_VX_FOR_JUMP, useVX);
		}

		ImGui::NewLine();

		if(ImGui::Button("Restart ROM")){
			config->restart = true;
		}

		if(ImGui::Button("Hide Menu")){
			config->settingsShown = false;
		}

		ImGui::End();
	}

	char *pixels;
	int pitch;

	SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);

	memcpy(pixels, frameBuffer, CHIP8_WIDTH * CHIP8_HEIGHT * 4);

	SDL_UnlockTexture(texture);

	ImGui::Render();

	SDL_RenderTexture(renderer, texture, NULL, NULL);
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
	SDL_RenderPresent(renderer);
	SDL_Delay(17);
}

void SDLRenderer::handleKeys(std::array<bool, 16> &pressedKeys,
							 bool &shouldQuit) {
	SDL_Event e;

	if (SDL_PollEvent(&e)) {
		ImGui_ImplSDL3_ProcessEvent(&e);
		if (e.type == SDL_EVENT_QUIT) {
			shouldQuit = true;
			return;
		}
		if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE) {
			shouldQuit = true;
			return;
		}

		if(e.type == SDL_EVENT_KEY_UP && e.key.scancode == SDL_SCANCODE_GRAVE){
			config->settingsShown = !config->settingsShown;
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
