#pragma once

#ifndef AUTOCOLIB_H
#define AUTOCOLIB_H

#include <windows.h>
#include <math.h>
#include <vector>
#include <ctime>
#include <memory>

namespace AutoCO
{
	static Vector2D GetCursorPos()
	{
		POINT p;
		GetCursorPos(&p);

		return Vector2D(p.x, p.y);
	}

	static Vector2D GetCursorPos(HWND hwnd)
	{
		POINT p;
		ScreenToClient(hwnd, &p);

		return Vector2D(p.x, p.y);
	}

	static void MouseControll(int flag)
	{
		INPUT inp[3];
		memset(inp, 0, sizeof(inp));

		inp[0].type = INPUT_MOUSE;
		inp[0].mi.dwFlags = flag;

		SendInput(3, inp, sizeof(INPUT));
	}


	static void KeyboardInput(char key, bool pressed)
	{
		INPUT key_press = { 0 };

		key_press.type = INPUT_KEYBOARD;
		key_press.ki.wVk = VkKeyScanEx(key, GetKeyboardLayout(NULL)) & 0xFF;
		key_press.ki.wScan = 0;
		key_press.ki.dwExtraInfo = 0;
		key_press.ki.dwFlags = (pressed ? 0 : KEYEVENTF_KEYUP);

		SendInput(1, &key_press, sizeof INPUT);
	}


	static void SetCursorPos(uint16_t xPos, uint16_t yPos)
	{
		xPos = (xPos * (65535 / GetSystemMetrics(SM_CXSCREEN)));
		yPos = (yPos * (65535 / GetSystemMetrics(SM_CYSCREEN)));

		INPUT inp[3];
		memset(inp, 0, sizeof(inp));

		inp[0].type = INPUT_MOUSE;
		inp[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		inp[0].mi.dx = xPos;
		inp[0].mi.dy = yPos;

		SendInput(3, inp, sizeof(INPUT));
	}


	static WORD VKToChar(BYTE byte)
	{
		std::vector<BYTE> keys(256, 0);
		WORD buffer[1];
		ToAscii(byte, MapVirtualKey(byte, MAPVK_VK_TO_VSC), keys.data(), buffer, 0);

		return buffer[0];
	}
}
#endif /* AUTOCOLIB_H */