#pragma once
#include <windows.h>
#include <memory>

// Mouse Controll
void MouseControll(int flag)
{
	INPUT inp[3];
	memset(inp, 0, sizeof(inp));

	inp[0].type = INPUT_MOUSE;
	inp[0].mi.dwFlags = flag;

	SendInput(3, inp, sizeof(INPUT));
}


// Mouse Controll
void KeyboardInput(char key, bool pressed)
{
	INPUT key_press = { 0 };

	key_press.type = INPUT_KEYBOARD;
	key_press.ki.wVk = VkKeyScanEx(key, GetKeyboardLayout(NULL)) & 0xFF;
	key_press.ki.wScan = 0;
	key_press.ki.dwExtraInfo = 0;
	key_press.ki.dwFlags = (pressed ? 0 : KEYEVENTF_KEYUP);

	SendInput(1, &key_press, sizeof INPUT);
}


// Controlls mouse position
void SetCursorPosition(uint16_t xPos, uint16_t yPos)
{
	xPos *= (65535 / GetSystemMetrics(SM_CXSCREEN));
	yPos *= (65535 / GetSystemMetrics(SM_CYSCREEN));

	INPUT inp[3];
	memset(inp, 0, sizeof(inp));

	inp[0].type = INPUT_MOUSE;
	inp[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	inp[0].mi.dx = xPos;
	inp[0].mi.dy = yPos;

	SendInput(3, inp, sizeof(INPUT));
}