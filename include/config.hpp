#pragma once

enum Quirk{
	PRESERVE_VF = 1,
	KEEP_INDEX_REGISTER = 2,
	KEEP_EXECUTING_AFTER_DRAW = 4,
	USE_DRAW_WRAPPING = 8,
	SHIFT_VX_REGISTER = 16,
	USE_VX_FOR_JUMP = 32
};

class Config{
	public: 
		Config();

		int quirks;

		int getQuirk(Quirk quirk);
		void setQuirk(Quirk quirk, bool value);

		int getColor(bool fg = false);

		float bgColor[3];
		float fgColor[3];

		int iterationsPerFrame;

		bool logging;

		int restart = false;

		int settingsShown = true;
};
