#pragma once
#define _CRT_SECURE_NO_WARNINGS 1 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1 

#include <unordered_map>

class Chip8Core {
public:
	bool Initialize();

	void Cycle();
	void SetKeys();

	void LoadProgram(const char* program);

	bool IsRunning() { return m_Running; }

	bool ShouldDraw() { return m_ShouldDraw; }
	bool IsPixelActive(int x, int y);

	// Chip8 has 4K memory
	unsigned char m_Memory[4096];
private:
	bool m_Running = false;
	bool m_ShouldDraw = false;

	// Current opcode
	unsigned short m_Opcode;

	// CPU registers
	unsigned char m_Reg[16];

	// Index register
	unsigned short m_Index;

	// Program counter (Stores the address of the instruction currently being executed)
	unsigned short m_ProgramCounter;

	// The screen resolution is 64 * 32
	unsigned char m_Display[64][32];

	// Timers that count at 60Hz. When they are set above 0 they will count down till 0
	// When the sound timer reachers 0 the system buzzer will sound
	unsigned char m_DelayTimer;
	unsigned char m_SoundTimer;

	// Stack is implemented as the Chip8 instruction set has opcodes that allow the program to jump
	// to a certain address or subroutine. The stack is used to keep track of the current program counter
	// before a jump is made. The stack has 16 layers, to keep track of what level is used we implement a
	// stack pointer
	unsigned short m_Stack[16];
	unsigned short m_StackPointer;

	// HEX based keypad
	unsigned char m_Key[16];
};