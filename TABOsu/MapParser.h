#pragma once

#include "math.h"
#include <vector>
#include <memory>
#include <unordered_map>

class OsuEntity;

struct TimingPoint
{
	uint32_t offset;
	float velocity;
	float msPerBeat;
};

class MapParser
{
public:
	MapParser();

	bool FindSongFile(const std::wstring& path, const std::wstring& fileName, std::wstring& resultPath);
	bool LoadSong(const std::wstring& path);
	void ClearSong();

	bool GetTimingPointFromOffset(uint32_t offset, TimingPoint& targetPoint);
	bool ParseDifficultySettings(const std::string& str);
	bool ParseOsuEntity(const std::string& str);
	bool ParseTimingPoint(const std::string& str);
	void OnRestartBeatmap(time_t time = 0);

	const std::wstring& GetCurrentBeatmap();
	const std::vector<OsuEntity*>& GetEntities();
	OsuEntity* GetEntity(uint16_t index);
	OsuEntity* GetCurrentEntity();
	uint16_t GetCurrentIndex();
	void NextMapEntity();
	bool IsMapLoaded();

private:
	// fields
	std::vector<OsuEntity*> m_OsuEntities;
	std::vector<TimingPoint*> m_TimingPoints;
	std::wstring m_CurrentBeatmap;

	uint16_t m_CurrentIndex;
	float m_SliderMultiplier;
};