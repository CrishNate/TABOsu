#pragma once

#include <vector>
#include "math.h"
#include <memory>
#include "ThReadStack.h"
#include <unordered_map>

class OsuEntity;

struct TimingPoint 
{
	uint32_t offset;
	float velocity;
	float msPerBeat;
};


class OsuHandler
{
public:
	OsuHandler();

	bool FindSongFile(const std::wstring& path, const std::wstring& fileName, std::wstring& resultPath);

	void StartBot(HWND hWND, HANDLE hProc, DWORD thRdStk);
	void StopBot();

	void SetAutoHit(bool autoHit);
	void SetAutoAim(bool autoAim);
	void SetAutoSpin(bool autoSpin);

	bool LoadSong(const std::wstring& path);
	void ClearSong();

	void SetOsuPath(const std::wstring& osuDirectoryPath);
	const std::wstring& GetOsuPath() const;

	bool GetTimingPointFromOffset(uint32_t offset, TimingPoint& targetPoint);

	bool ParseDifficultySettings(const std::string& str);
	bool ParseOsuEntity(const std::string& str);
	bool ParseTimingPoint(const std::string& str);

	void OnRestartBeatmap(time_t time = 0);

	time_t GetOSUTimeline(HANDLE hProc, DWORD thRdStk);

private:
	// fields
	std::vector<OsuEntity*> m_OsuEntities;
	std::vector<TimingPoint*> m_TimingPoints;

	std::wstring m_OsuPath;
	std::wstring m_CurrentBeatmap;

	uint16_t m_CurrentObject;
	float m_SliderMultiplier;
	Vector2D m_CursorPos;

	bool m_BotActive;
	bool m_AutoHit;
	bool m_AutoAim;
	bool m_AutoSpin;
};