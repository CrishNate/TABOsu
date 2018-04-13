#include <vector>
#include "math.h"
#include <memory>
#include <unordered_map>

#ifndef OSUENTITY_H
#define OSUENTITY_H

enum OsuEntityType
{
	OET_None = 0,
	OET_HitCircle = 1,
	OET_Slider = 2,
	OET_NewCombo = 4,
	OET_Spinner = 8,
	OET_SkipCombo1 = 16,
	OET_SkipCombo2 = 32,
	OET_SkipCombo3 = 64,
};

class OsuEntity
{
public:

	OsuEntity(Vector2D point, int32_t startTime, int32_t endTime)
		: m_Point(point)
		, m_StartTime(startTime)
		, m_EndTime(endTime)
	{ }

	virtual OsuEntityType GetType() { return OsuEntityType::OET_None; }

	int32_t GetStartTime() { return m_StartTime; }
	int32_t GetEndTime() { return m_EndTime; }

protected:

	Vector2D m_Point;
	int32_t m_StartTime;
	int32_t m_EndTime;
};

#endif OSUENTITY_H