#pragma once

//Log messages
#define LOG(text, ...) log(__FILE__, __LINE__, text, __VA_ARGS__);
void log(const char file[], int line, const char* text, ...);

//Windows size
#define WINDOW_WIDTH    512
#define WINDOW_HEIGHT   480

//Menu GUI
#define BUTTON_PLAY_POS_X	0
#define BUTTON_PLAY_POS_Y	325
#define CURSOR_WIDTH		WINDOW_WIDTH
#define CURSOR_HEIGHT		50
#define CURSOR_COLOR		Fade(WHITE, 0.3f)