#pragma once

#include "MapParser.h"
#include "ThReadStack.h"

class OsuHandler
{
public:
	OsuHandler();

	void StartBot(HWND hWND, HANDLE hProc, DWORD thRdStk);
	void StopBot();

	void SetCursorDanceMode(bool cursorDance);
	void SetAutoHit(bool autoHit);
	void SetAutoAim(bool autoAim);
	void SetAutoSpin(bool autoSpin);

	void SetAimAccuracy(float aimAccuracy);
	void SetHitAccuracy(float hitAccuracy);

	time_t GetOSUTimeline(HANDLE hProc, DWORD thRdStk);

	MapParser* GetMapParser();

private:
	
	// fields
	MapParser* m_pMapParser;
	Vector2D m_CursorPos;
	Vector2D m_CursorVelocity;

	bool m_BotActive;
	bool m_AutoHit;
	bool m_AutoAim;
	bool m_AutoSpin;
	bool m_CursorDance;

	float m_HitAccuracy;
	float m_AimAccuracy;
};