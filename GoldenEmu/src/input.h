#pragma once
#include "SDL.h"

/*
Chip-8
Keypad       Keyboard
+-+-+-+-+    +-+-+-+-+
|1|2|3|C|    |1|2|3|4|
+-+-+-+-+    +-+-+-+-+
|4|5|6|D|    |Q|W|E|R|
+-+-+-+-+ => +-+-+-+-+
|7|8|9|E|    |A|S|D|F|
+-+-+-+-+    +-+-+-+-+
|A|0|B|F|    |Z|X|C|V|
+-+-+-+-+    +-+-+-+-+
 */

class Input {
public:
	static bool IsKeyDown(SDL_Scancode key);
	static void Update();

	static unsigned char* GetKeys() { return m_Keypad; };
private:
	inline static unsigned char m_Keypad[16];
};