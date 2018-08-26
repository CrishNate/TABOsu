#include "stdafx.h"
#include "MapParser.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "HitCircle.h"
#include "Slider.h"
#include "Spinner.h"

static std::vector<std::string> SplitString(const std::string& str, char separator)
{
	std::vector<std::string> data;
	std::istringstream ss(str);
	std::string token;

	while (std::getline(ss, token, separator))
		data.push_back(token);

	return data;
}

MapParser::MapParser()
	: m_CurrentIndex(0)
	, m_SliderMultiplier(0)
{ }

bool MapParser::FindSongFile(const std::wstring& path, const std::wstring& fileName, std::wstring& resultPath)
{
	WIN32_FIND_DATA data;
	std::wstring tempPath = path + L"\\*";
	LPCWSTR lpcStr = tempPath.c_str();

	// Searthing for the file
	HANDLE search_handle = FindFirstFile(lpcStr, &data);

	if (search_handle)
	{
		do
		{
			std::wstring wfileName(data.cFileName);
			if (data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ||
				data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ||
				wcscmp(data.cFileName, L".") == 0 ||
				wcscmp(data.cFileName, L"..") == 0)
				continue;

			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				tempPath = path + L"\\" + std::wstring(data.cFileName);

				// Now searthing folder
				if (FindSongFile(tempPath, fileName, resultPath))
				{
					FindClose(search_handle);
					return true;
				}
			}
			else if (wfileName == fileName)
			{
				resultPath = path + L"\\" + wfileName;
				FindClose(search_handle);
				return true;
			}

		} while (FindNextFile(search_handle, &data));
	}

	FindClose(search_handle);
	return false;
}


bool MapParser::GetTimingPointFromOffset(uint32_t offset, TimingPoint& targetPoint) 
{
	if (m_TimingPoints.size() == 0)
		return false;

	for (size_t i = m_TimingPoints.size(); i-- > 0;) {
		if (m_TimingPoints[i]->offset <= offset) {
			targetPoint = *(m_TimingPoints[i]);
			return true;
		}
	}

	targetPoint = *(m_TimingPoints.front());
	return true;
}


bool MapParser::ParseDifficultySettings(const std::string& str)
{
	std::vector<std::string> dat = SplitString(str, ':');

	if (dat.size() < 2)
		return false;

	if (!_stricmp(dat[0].c_str(), "slidermultiplier"))
	{
		m_SliderMultiplier = std::stof(dat[1]);
	}

	return true;
}


bool MapParser::ParseOsuEntity(const std::string& str)
{
	std::vector<std::string> dat = SplitString(str, ',');
	if (dat.size() < 4)
		return false;

	OsuEntityType type = (OsuEntityType)std::stoi(dat[3]);

	int32_t startTime = std::stoll(dat[2]);
	int x = std::stoi(dat[0]);
	int y = std::stoi(dat[1]);
	Vector2D point = Vector2D(x, y);

	std::cout << type << "\n";

	if ((type & OsuEntityType::OET_HitCircle) == OsuEntityType::OET_HitCircle)
	{
		int32_t endTime = startTime + 30;
		OsuEntity* pOsuEnt = new HitCircle(point, startTime, endTime);
		m_OsuEntities.push_back(pOsuEnt);
		return true;
	}
	else if ((type & OsuEntityType::OET_Spinner) == OsuEntityType::OET_Spinner)
	{
		int32_t endTime = std::stoi(dat[5]);
		OsuEntity* pOsuEnt = new Spinner(point, startTime, endTime);
		m_OsuEntities.push_back(pOsuEnt);
		return true;
	}
	else if ((type & OsuEntityType::OET_Slider) == OsuEntityType::OET_Slider) {
		uint16_t repeat = std::stoi(dat[6]);
		uint32_t pixelLength = std::stoi(dat[7]);
		int32_t endTime;
		std::vector<Vector2D> points;
		TimingPoint objectTimingPoint;

		// getting time when slider ends
		if (GetTimingPointFromOffset(startTime, objectTimingPoint))
		{
			float pxPerBeat = m_SliderMultiplier * 100 * objectTimingPoint.velocity;
			float beats_num = (pixelLength * repeat) / pxPerBeat;
			endTime = static_cast<int>(startTime + ceil(beats_num * objectTimingPoint.msPerBeat));
		}

		// parsing points of slider
		std::vector<std::string> pointsDat = SplitString(dat[5], '|');
		SliderType sliderType = charToSliderType.at(pointsDat.at(0)[0]);
		points.push_back(point);

		for (int i = 1; i < pointsDat.size(); i++)
		{
			std::vector<std::string> scoords = SplitString(pointsDat.at(i), ':');
			Vector2D pointf = Vector2D(stoi(scoords.at(0)), stoi(scoords.at(1)));
			points.push_back(pointf);
		}

		points = SliderPath::CreatePath(points, sliderType);

		OsuEntity* pOsuEnt = new Slider(point, startTime, endTime, repeat, pixelLength, points, sliderType);
		m_OsuEntities.push_back(pOsuEnt);
		return true;
	}

	return false;
}


bool MapParser::ParseTimingPoint(const std::string& str)
{
	static float lastMsPerBeat;
	std::vector<std::string> dat = SplitString(str, ',');

	if (dat.size() < 2)
		return false;

	int32_t offset = std::stoi(dat[0]);
	float msPerBeat = std::stof(dat[1]);
	float velocity = 1.0f;

	if (msPerBeat < 0)
	{
		velocity = abs(100 / msPerBeat);

		if (lastMsPerBeat != 0) {
			msPerBeat = lastMsPerBeat;
		}
	}
	else {
		lastMsPerBeat = msPerBeat;
	}

	TimingPoint* pTimingPoint = new TimingPoint();

	pTimingPoint->offset = offset;
	pTimingPoint->velocity = velocity;
	pTimingPoint->msPerBeat = msPerBeat;

	m_TimingPoints.push_back(pTimingPoint);

	return true;
}


bool MapParser::LoadSong(const std::wstring& path)
{
	ClearSong();

	std::ifstream file(path);
	std::string currentSection;
	std::string str;

	while (getline(file, str))
	{
		if (!str.empty() && str.front() == '[' && str.back() == ']') {
			currentSection = str.substr(1, str.length() - 2);
			continue;
		}

		if (currentSection.empty())
			continue;

		if (!currentSection.compare("Difficulty"))
		{
			ParseDifficultySettings(str);
		}
		else if (!currentSection.compare("TimingPoints"))
		{
			ParseTimingPoint(str);
		}
		else if (!currentSection.compare("HitObjects"))
		{
			ParseOsuEntity(str);
		}
	}

	file.close();

	if (m_OsuEntities.size() == 0)
		return false;

	m_CurrentBeatmap = path;

	return true;
}


void MapParser::ClearSong()
{
	for (OsuEntity* pOsuEntity : m_OsuEntities)
		delete pOsuEntity;

	m_OsuEntities.clear();
	m_CurrentBeatmap.clear();
}


void MapParser::OnRestartBeatmap(time_t time)
{
	for (size_t i = 0; i < m_OsuEntities.size(); i++)
	{
		if (m_OsuEntities.at(i)->GetStartTime() > time)
		{
			m_CurrentIndex = i;
			break;
		}
	}
}


const std::wstring& MapParser::GetCurrentBeatmap()
{
	return m_CurrentBeatmap;
}


const std::vector<OsuEntity*>& MapParser::GetEntities()
{
	return m_OsuEntities;
}


uint16_t MapParser::GetCurrentIndex()
{
	return m_CurrentIndex;
}


OsuEntity* MapParser::GetCurrentEntity()
{
	return m_OsuEntities[m_CurrentIndex];
}


OsuEntity* MapParser::GetEntity(uint16_t index)
{
	return m_OsuEntities[index];
}


void MapParser::NextMapEntity()
{
	m_CurrentIndex++;
}


bool MapParser::IsMapLoaded()
{
	return m_CurrentBeatmap.empty();
}