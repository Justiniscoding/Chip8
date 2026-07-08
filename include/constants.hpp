#pragma once

const int SCALING_FACTOR = 16;

const int CHIP8_WIDTH = 64;
const int CHIP8_HEIGHT = 32;

const int WINDOW_WIDTH = CHIP8_WIDTH * SCALING_FACTOR;
const int WINDOW_HEIGHT = CHIP8_HEIGHT * SCALING_FACTOR;

const int BG_COLOR = 0x000000ff;
const int FG_COLOR = 0x1e90ffff;

const int ITERATIONS_PER_FRAME = 11;
