#include "input.h"

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

bool Input::IsKeyDown(SDL_Scancode key)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[key])
		return true;
	return false;
}

void Input::Update()
{
	m_Keypad[0] = IsKeyDown(SDL_SCANCODE_1);
	m_Keypad[1] = IsKeyDown(SDL_SCANCODE_2);
	m_Keypad[2] = IsKeyDown(SDL_SCANCODE_3);
	m_Keypad[3] = IsKeyDown(SDL_SCANCODE_4);
	m_Keypad[4] = IsKeyDown(SDL_SCANCODE_Q);
	m_Keypad[5] = IsKeyDown(SDL_SCANCODE_W);
	m_Keypad[6] = IsKeyDown(SDL_SCANCODE_E);
	m_Keypad[7] = IsKeyDown(SDL_SCANCODE_R);
	m_Keypad[8] = IsKeyDown(SDL_SCANCODE_A);
	m_Keypad[9] = IsKeyDown(SDL_SCANCODE_S);
	m_Keypad[10] = IsKeyDown(SDL_SCANCODE_D);
	m_Keypad[11] = IsKeyDown(SDL_SCANCODE_F);
	m_Keypad[12] = IsKeyDown(SDL_SCANCODE_Z);
	m_Keypad[13] = IsKeyDown(SDL_SCANCODE_X);
	m_Keypad[14] = IsKeyDown(SDL_SCANCODE_C);
	m_Keypad[15] = IsKeyDown(SDL_SCANCODE_V);
}
