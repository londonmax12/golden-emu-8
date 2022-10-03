#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>

#include <vector>

constexpr unsigned char fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

bool Chip8Core::Initialize()
{
	m_Running = true;

	m_ProgramCounter = 0x200; // Program counter starts at 0x200

	m_Opcode = 0x00;
	m_Index = 0x00;
	m_StackPointer = 0x00;
	m_DelayTimer = 0x00;

	memset(m_Display, 0, static_cast<size_t>(64) * 32);

	for (auto& reg : m_Reg) reg = 0x00;

	for (int i = 0; i < 80; ++i)
		m_Memory[i] = fontset[i];

	return true;
}

void Chip8Core::Cycle()
{
	// Fetch current opcode from memory, to do this first we shift need to shift left 8 bytes and
	// then we need to pipe the next index into it as each opcode is 2bytes
	m_Opcode = m_Memory[m_ProgramCounter] << 8u | m_Memory[m_ProgramCounter + 1];
	m_ProgramCounter += 2;

	int w = (m_Opcode >> 12) & 0xF;
	int x = (m_Opcode & 0x0F00) >> 8;
	int y = (m_Opcode & 0x00F0) >> 4;
	int z = m_Opcode & 0xF;
	int nn = m_Opcode & 0x00FF;
	int nnn = m_Opcode & 0x0FFF;

	m_ShouldDraw = false;
	 
	// Get the first 4 bytes of opcode to figure out what it is
	switch (m_Opcode & 0xF000)
	{
	case 0xA000: // Sets index to the address NNN
	{
		m_Index = m_Opcode & 0x0FFF;
		break;
	}
	case 0xD000: // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; 
		//I value does not change after the execution of this instruction. VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen.
	{
		// Get height from n
		int height = m_Opcode & 0x000F;
		// Width: vx, Height: vy
		int xCoord = m_Reg[x];
		int yCoord = m_Reg[y];

		int ands[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

		// Set carry flag to 0
		m_Reg[15] = 0;

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < 8; j++) {
				// Allows sprite to wrap around screen
				if (xCoord + j == 64) {
					xCoord = -j;
				}
				if (yCoord + i == 32) {
					yCoord = -i;
				}

				// Set carry flag to 1 if a sprite changes from set to unset
				if (m_Display[xCoord + j][yCoord + i] == 1 &&
					((m_Memory[m_Index + i] & ands[j]) >> (8 - j - 1)) == 1) {
					m_Reg[15] = 1;
				}

				// Bitwise operations decode each bit of sprite and XOR with the current pixel on screen
				m_Display[xCoord + j][yCoord + i] = m_Display[xCoord + j][yCoord + i] ^
					((m_Memory[m_Index + i] & ands[j]) >> (8 - j - 1));
			}
			xCoord = m_Reg[x];
			yCoord = m_Reg[y];
		}
		m_ShouldDraw = true;

		break;
	}
	case 0x0000:
	{
		// Opcodes share the same first 4 bytes so we add an additional switch statement
		switch(m_Opcode & 0x000F)
		{
		case 0x0000: // Clears the screen
			memset(m_Display, 0, static_cast<size_t>(64) * 32);
			break;
 
		case 0x000E: // Returns from subroutine  
			m_ProgramCounter = m_Stack[m_StackPointer--];
			break;
 
		default:
			printf ("Unknown opcode [0x0000]: 0x%X\n", m_Opcode);
		}
	}
	case 0x1000: // Jump to nnn address
	{
		m_ProgramCounter = nnn;
		break;
	}
	case 0x2000: // Call subroutine at nnn. 
	{
		m_Stack[m_StackPointer++] = m_ProgramCounter;
		m_ProgramCounter = nnn;
		break;
	}
	case 0x4000:
		if (m_Reg[x] == nn)
			m_ProgramCounter += 2;
	case 0x6000: // Set x to nn
	{
		m_Reg[x] = nn;
		break;
	}
	case 0x9000:
		if (m_Reg[x] == m_Reg[y])
			m_ProgramCounter += 2;
		break;
	case 0xF000:
	{
		switch (w)
		{
		case 15: // Set delay timer to vx
			m_DelayTimer = m_Reg[x];
			break;
		default:
			printf("Unknown opcode [0xF000]: 0x%X\n", m_Opcode);
			break;
		}
	}
  break;
	default:
	{
		printf("Unknown opcode: 0x%X\n", m_Opcode & 0xF000);
	}
	}

	if (m_DelayTimer > 0)
		--m_DelayTimer;

	if (m_SoundTimer > 0)
	{
		if (m_SoundTimer == 1)
			printf("Sound timer triggered\n");
		--m_SoundTimer;
	}
}

void Chip8Core::SetKeys()
{
}

void Chip8Core::LoadProgram(const char* program)
{
	FILE* file;
	int fileSize;

	// open file stream in binary read-only mode
	file = fopen(program, "rb");
	if (file == NULL) 
	{ 
		printf("File unable to load.\n");
	}
	else {
		// find file size and load program
		fseek(file, 0, SEEK_END);
		fileSize = ftell(file);
		printf("Loaded ROM size: %d\n", fileSize);
		rewind(file);

		// Copy buffer to Chip8 memory
		if ((4096 - 512) > fileSize)
		{
			// Read program into memory
			fread(m_Memory + 512, 1, fileSize, file);
		}
		else {
			printf("File is too large to load.\n");
		}

		// Close file
		fclose(file);
	}
}

bool Chip8Core::IsPixelActive(int x, int y)
{
	return m_Display[x][y];
}
