#include "config.hpp"

int Config::getQuirk(Quirk quirk){
	return quirks & quirk;
}

void Config::setQuirk(Quirk quirk, bool value){
	if(value){
		quirks |= quirk;
	}else{
		quirks ^= quirk;
	}
}

Config::Config(){
	bgColor[0] = 0;
	bgColor[1] = 1;
	bgColor[2] = 1;

	fgColor[0] = 0.1176470588;
	fgColor[1] = 0.5647058824;
	fgColor[2] = 1;

	quirks = 0;

	iterationsPerFrame = 11;

	logging = false;
}

int Config::getColor(bool fg){
	if(fg){
		return static_cast<int>(fgColor[0] * 255) << 24 | static_cast<int>(fgColor[1] * 255) << 16 | static_cast<int>(fgColor[2] * 255) << 8 | 0xFF;
	}

	return static_cast<int>(bgColor[0] * 255) << 24 | static_cast<int>(bgColor[1] * 255) << 16 | static_cast<int>(bgColor[2] * 255) << 8 | 0xFF;
}
