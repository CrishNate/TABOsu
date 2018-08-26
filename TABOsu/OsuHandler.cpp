#include "stdafx.h"
#include "OsuHandler.h"
#include "autoco.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "HitCircle.h"
#include "Slider.h"
#include "Spinner.h"

#include "CatmullApproximator.h"

#define FIRST_HIT_BUTTON 'w'
#define SECOND_HIT_BUTTON 'e'

#define TIME_BOUNDS 1000000

#define OSU_FILE_MAX_X 512.0f
#define OSU_FILE_MAX_Y 384.0f

#define OFFSET1 0x11C
#define OFFSET2 0x1F4
#define OFFSET3 0xC0
#define OFFSET4 0x78C
#define OFFSET5 0x220


OsuHandler::OsuHandler()
	: m_pMapParser(new MapParser())
	, m_CursorPos(Vector2D(0, 0))
	, m_CursorVelocity(Vector2D(0, 0))
	, m_AimAccuracy(0)
	, m_HitAccuracy(0)
	, m_AutoAim(false)
	, m_AutoHit(false)
	, m_AutoSpin(false)
	, m_CursorDance(false)
{ }


time_t OsuHandler::GetOSUTimeline(HANDLE hProc, DWORD thRdStk)
{
	int offset;
	time_t time = 0;

	ReadProcessMemory(hProc, (LPCVOID)(thRdStk + OFFSET1), &offset, sizeof(int), NULL);
	ReadProcessMemory(hProc, (LPCVOID)(offset + OFFSET2), &offset, sizeof(int), NULL);
	ReadProcessMemory(hProc, (LPCVOID)(offset + OFFSET3), &offset, sizeof(int), NULL);
	ReadProcessMemory(hProc, (LPCVOID)(offset + OFFSET4), &offset, sizeof(int), NULL);
	ReadProcessMemory(hProc, (LPCVOID)(offset + OFFSET5), &time, sizeof(int), NULL);

	if (time > TIME_BOUNDS)
		return 0;

	return time;
}


Vector2D ConvertOsuToWindowEx(Vector2D point, RECT screenScale)
{
	float w = static_cast<float>(screenScale.right);
	float h = static_cast<float>(screenScale.bottom);

	point.x = point.x / OSU_FILE_MAX_X * w;
	point.y = point.y / OSU_FILE_MAX_Y * h;

	return point;
}

Vector2D ConvertOsuToWindow(Vector2D point, RECT screenScale)
{
	float w = static_cast<float>(screenScale.right);
	float h = static_cast<float>(screenScale.bottom);
	float mult = min(w, h) * 0.85f;
	float ratio = OSU_FILE_MAX_X / OSU_FILE_MAX_Y;

	float xo = (point.x / OSU_FILE_MAX_X) - 0.5f;
	float yo = (point.y / OSU_FILE_MAX_Y) - 0.5f;

	point.x = xo * mult * ratio + w / 2.0f;
	point.y = yo * mult + h / 2.0f;

	return point;
}


void OsuHandler::StartBot(HWND hWND, HANDLE hProc, DWORD thRdStk)
{
	if (m_pMapParser->IsMapLoaded())
		return;

	m_BotActive = true;

	// getting scales
	RECT windowRect;
	RECT windowRectange;
	bool fullScreen = true;
	float xScreenResolution = GetSystemMetrics(SM_CXSCREEN);
	float yScreenResolution = GetSystemMetrics(SM_CYSCREEN);

	time_t time = GetOSUTimeline(hProc, thRdStk);
	time_t prewTime = 0;
	time_t lastHitTime = time;
	Vector2D lastHitPos = Vector2D(0, 0);
	bool keyPressed = false;

	Vector2D lastPos = Vector2D(0, 0);
	Vector2D velocity = Vector2D(0, 0);

	if (fullScreen)
	{
		windowRect = { 0, 0, xScreenResolution, yScreenResolution };
	}
	else
	{
		GetWindowRect(hWND, &windowRect);
		MapWindowPoints(hWND, NULL, (LPPOINT)&windowRect, 2);
		windowRectange = windowRect;
		windowRect.left = 0;
		windowRect.top = 0;
	}

	m_pMapParser->OnRestartBeatmap(time);

	while (m_BotActive)
	{
		// If index out of bounds
		if (m_pMapParser->GetCurrentIndex() > m_pMapParser->GetEntities().size() - 1)
			break;

		//if (fullScreen)
		//{
		//	RECT rect;
		//	GetWindowRect(hWND, &rect);
		//	GetClientRect(hWND, &windowRectange);
		//	windowRectange.left = rect.left;
		//	windowRectange.top = rect.top;
		//	//MapWindowPoints(hWND, NULL, (LPPOINT)&windowRectange, 2);
		//}

		//Vector2D curPos = AutoCO::GetCursorPos();
		//curPos.x = (curPos.x - windowRectange.left) / windowRectange.right * xScreenResolution;
		//curPos.y = (curPos.y - windowRectange.top) / windowRectange.bottom * yScreenResolution;

		static char activeKey = FIRST_HIT_BUTTON;
		time = GetOSUTimeline(hProc, thRdStk) + 20.0f;

		if (time > TIME_BOUNDS)
			time = 0;

		// Whenever game on pause bot is not running
		if (time == prewTime)
			continue;

		float dTime = (time - prewTime);

		if (prewTime > (time + 1) || dTime > 100)
		{
			prewTime = time;
			m_pMapParser->OnRestartBeatmap(time);
			lastHitTime = time;
			lastHitPos = Vector2D(0, 0);
			continue;
		}

		OsuEntity* pOsuEntity = m_pMapParser->GetCurrentEntity();

		// pressing key
		if (time >= pOsuEntity->GetStartTime() && !keyPressed)
		{
			if (m_AutoHit || m_CursorDance)
				AutoCO::KeyboardInput(activeKey, true);

			keyPressed = true;
		}

		// release key
		if (time > pOsuEntity->GetEndTime() && keyPressed)
		{
			if (m_AutoHit || m_CursorDance)
				AutoCO::KeyboardInput(activeKey, false);

			m_pMapParser->NextMapEntity();
			keyPressed = false;
			activeKey = (activeKey == FIRST_HIT_BUTTON) ? SECOND_HIT_BUTTON : FIRST_HIT_BUTTON;

			if (m_CursorDance)
			{
				float leng = velocity.Len();
				velocity.Normalize();
				m_CursorVelocity = velocity * min(1.0f, leng * 10.0f);
			}

			lastHitTime = time;
			lastHitPos = m_CursorPos;
		}

		// Mouse movement
		Vector2D objPosition = pOsuEntity->GetPosition(time);

		objPosition = ConvertOsuToWindow(objPosition, windowRect);

		if (time > pOsuEntity->GetStartTime())
		{
			m_CursorPos = objPosition;
		}
		else
		{
			float coef = max(0, min(1, (time - lastHitTime) / (float)(pOsuEntity->GetStartTime() - lastHitTime)));

			if (m_CursorDance)
				m_CursorPos = SimpleBezireCurve(lastHitPos, lastHitPos + m_CursorVelocity * (objPosition - lastHitPos).Len(), objPosition, coef);
			else
				m_CursorPos = lastHitPos + (objPosition - lastHitPos) * coef;
		}

		if (m_CursorDance)
		{
			if ((m_CursorPos - lastPos).Len() > 0.1f)
				velocity = (m_CursorPos - lastPos);
			lastPos = m_CursorPos;
		}

		// border screen, that cursor can't go out of screen
		m_CursorPos.x = max(0, min(xScreenResolution, m_CursorPos.x));
		m_CursorPos.y = max(0, min(yScreenResolution, m_CursorPos.y));

		if ((m_AutoAim || m_CursorDance))
		{
			AutoCO::SetCursorPos(m_CursorPos.x, m_CursorPos.y);
		}

		prewTime = time;
	}
}

void OsuHandler::StopBot()
{
	m_BotActive = false;
}


void OsuHandler::SetAutoHit(bool autoHit)
{
	m_AutoHit = autoHit;
}


void OsuHandler::SetAutoAim(bool autoAim)
{
	m_AutoAim = autoAim;
}


void OsuHandler::SetAutoSpin(bool autoSpin)
{
	m_AutoSpin = autoSpin;
}


void OsuHandler::SetAimAccuracy(float aimAccuracy)
{
	m_AimAccuracy = (1 - aimAccuracy / 100) * 100;
}


void OsuHandler::SetHitAccuracy(float hitAccuracy)
{
	m_HitAccuracy = (1 - hitAccuracy / 100) * 100;
}


void OsuHandler::SetCursorDanceMode(bool cursorDance)
{
	m_CursorDance = cursorDance;
}


MapParser* OsuHandler::GetMapParser()
{
	return m_pMapParser;
}

//int bufsize = GetWindowTextLength(hWND) + 1;
//LPWSTR title = new WCHAR[bufsize];
//GetWindowText(hWND, title, sizeof(title));
//if (wcscmp(title, L"osu") == 0)
//	continue;
//delete title;