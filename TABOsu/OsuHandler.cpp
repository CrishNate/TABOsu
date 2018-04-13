#include "stdafx.h"
#include "OsuHandler.h"
#include "inputControll.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "HitCircle.h"
#include "Slider.h"
#include "Spinner.h"

char leftClickKey = 'w';
char rightClickKey = 'e';

#define OSU_FILE_MAX_X 512.0f
#define OSU_FILE_MAX_Y 384.0f

static std::vector<std::string> SplitString(const std::string& str, char separator)
{
	std::vector<std::string> data;
	std::istringstream ss(str);
	std::string token;

	while (std::getline(ss, token, separator))
		data.push_back(token);

	return data;
}


OsuHandler::OsuHandler()
	: m_OsuPath(L"")
	, m_CurrentObject(0)
	, m_CursorPos(Vector2D(0,0))
	, m_AutoAim(false)
	, m_AutoHit(false)
	, m_AutoSpin(false)
{ }


time_t OsuHandler::GetOSUTimeline(HANDLE hProc, DWORD thRdStk)
{
	int offset;
	unsigned int t = 0;

	ReadProcessMemory(hProc, (LPCVOID)(thRdStk + 0x6CC), &offset, sizeof(int), NULL);
	ReadProcessMemory(hProc, (LPCVOID)(offset + 0x90), &offset, sizeof(int), NULL);
	ReadProcessMemory(hProc, (LPCVOID)(offset + 0x20), &offset, sizeof(int), NULL);
	ReadProcessMemory(hProc, (LPCVOID)(offset + 0x134), &offset, sizeof(int), NULL);
	ReadProcessMemory(hProc, (LPCVOID)(offset + 0x90), &t, sizeof(int), NULL);
	return t;
}


bool OsuHandler::FindSongFile(const std::wstring& path, const std::wstring& fileName, std::wstring& resultPath)
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


void OsuHandler::SetOsuPath(const std::wstring& path)
{
	m_OsuPath = path;
}

const std::wstring& OsuHandler::GetOsuPath() const
{
	return m_OsuPath;
}


bool OsuHandler::GetTimingPointFromOffset(uint32_t offset, TimingPoint& targetPoint) {
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


bool OsuHandler::ParseDifficultySettings(const std::string& str)
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


bool OsuHandler::ParseOsuEntity(const std::string& str)
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
		int32_t endTime = startTime + 2;
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

		OsuEntity* pOsuEnt = new Slider(point, startTime, endTime, repeat, pixelLength, points, sliderType);
		m_OsuEntities.push_back(pOsuEnt);
		return true;
	}

	return false;
}


bool OsuHandler::ParseTimingPoint(const std::string& str)
{
	static float lastMsPerBeat;
	std::vector<std::string> dat = SplitString(str, ',');
	
	if (dat.size() < 2)
		return false;

	int32_t offset = std::stoi(dat[0]);
	float msPerBeat = std::stof(dat[1]);
	float velocity = 1.0f;

	if (msPerBeat < 0) {
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


bool OsuHandler::LoadSong(const std::wstring& path)
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


void OsuHandler::ClearSong()
{
	for (OsuEntity* pOsuEntity : m_OsuEntities)
		delete pOsuEntity;

	m_OsuEntities.clear();
	m_CurrentBeatmap.clear();
}


void OsuHandler::OnRestartBeatmap(time_t time)
{
	for (size_t i = 0; i < m_OsuEntities.size(); i++) 
	{
		if (m_OsuEntities.at(i)->GetStartTime() > time)
		{
			m_CurrentObject = i;
			break;
		}
	}
}


Vector2D ConvertOsuToWindowEx(Vector2D point, RECT screenScale)
{
	float w = static_cast<float>(screenScale.right);
	float h = static_cast<float>(screenScale.bottom);

	point.x = point.x / OSU_FILE_MAX_X * w;
	point.y = point.y / OSU_FILE_MAX_Y * h;

	return point;
}


Vector2D ConvertOsuToWindow(Vector2D point, RECT wSize)
{
	float w = static_cast<float>(wSize.right);
	float h = static_cast<float>(wSize.bottom);
	float mult = min(w, h) / 1.25f;
	float koef = OSU_FILE_MAX_X / OSU_FILE_MAX_Y;

	float xo = (point.x / OSU_FILE_MAX_X) - 0.5f;
	float yo = (point.y / OSU_FILE_MAX_Y) - 0.5f;

	point.x = xo * mult * koef + w / 2.0f + wSize.left;
	point.y = yo * mult + h / 2.0f + wSize.top;

	return point;
}

void OsuHandler::StartBot(HWND hWND, HANDLE hProc, DWORD thRdStk)
{
	if (m_CurrentBeatmap.empty())
		return;

	m_BotActive = true;

	// getting scales
	RECT windowRect;
	bool fullScreen = false;

	if (!fullScreen)
	{
		GetWindowRect(hWND, &windowRect);
		MapWindowPoints(hWND, NULL, (LPPOINT)&windowRect, 2);
		windowRect.left = 0;
		windowRect.top = 0;
		windowRect = { 0, 0, 1920, 1080 };
	}
	else
	{
		windowRect = { 0, 0, 1920, 1080 };
	}

	time_t time = GetOSUTimeline(hProc, thRdStk);
	time_t prewTime = 0;
	
	time_t lastHitTime = time;
	Vector2D lastHitPos = Vector2D(0, 0);

	bool keyPressed = false;
	OnRestartBeatmap(time);

	float x = 0;

	while (m_BotActive)
	{
		// If index out of bounds
		if (m_CurrentObject > m_OsuEntities.size() - 1)
			break;

		static char activeKey = leftClickKey;
		time = GetOSUTimeline(hProc, thRdStk);

		// Whenever game on pause bot is not running
		if (time == prewTime)
			continue;

		if (prewTime > (time + 1) 
			|| (time - prewTime) > 100)
		{
			prewTime = time;
			OnRestartBeatmap(time);
			lastHitTime = time;
			lastHitPos = Vector2D(0, 0);
			continue;
		}

		OsuEntity* pOsuEntity = m_OsuEntities[m_CurrentObject];

		// pressing key
		if (time >= (pOsuEntity->GetStartTime() - 15) && !keyPressed)
		{
			if (m_AutoHit)
				KeyboardInput(activeKey, true);

			keyPressed = true;
			prewTime = time;
			continue;
		}

		// release key
		if (time > (pOsuEntity->GetEndTime() + 15) && keyPressed)
		{
			if (m_AutoHit)
				KeyboardInput(activeKey, false);

			m_CurrentObject++;
			keyPressed = false;
			activeKey = (activeKey == leftClickKey) ? rightClickKey : leftClickKey;

			lastHitTime = time;
			lastHitPos = m_CursorPos;
		}

		// Mouse movement
		if ((pOsuEntity->GetStartTime() - time) <= 1000)
		{
			if (abs(pOsuEntity->GetStartTime() - lastHitTime) > 1000)
				lastHitTime = pOsuEntity->GetStartTime() - 1000;

			Vector2D curPos = Vector2D(0, 0);

			std::cout << pOsuEntity->GetType() << "\n";

			if (pOsuEntity->GetType() == OsuEntityType::OET_HitCircle)
			{
				curPos = static_cast<HitCircle*>(pOsuEntity)->GetCursorMove(time, lastHitTime, lastHitPos);
			}
			else if (pOsuEntity->GetType() == OsuEntityType::OET_Slider)
			{
				curPos = static_cast<Slider*>(pOsuEntity)->GetCursorMove(time, lastHitTime, lastHitPos);
			}
			else if (pOsuEntity->GetType() == OsuEntityType::OET_Spinner)
			{
				if (m_AutoSpin)
					curPos = static_cast<Spinner*>(pOsuEntity)->GetCursorMove(time, lastHitTime, lastHitPos, windowRect);
			}
			else
			{
				curPos = Vector2D(0, 0);
			}
			
			m_CursorPos = curPos;
			curPos = ConvertOsuToWindow(curPos, windowRect);

			if (m_AutoAim)
				SetCursorPosition(curPos.x, curPos.y);
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




//int bufsize = GetWindowTextLength(hWND) + 1;
//LPWSTR title = new WCHAR[bufsize];
//GetWindowText(hWND, title, sizeof(title));
//if (wcscmp(title, L"osu") == 0)
//	continue;
//delete title;