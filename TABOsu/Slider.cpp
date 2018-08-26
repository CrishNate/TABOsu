#include "stdafx.h"
#include "Slider.h"

Vector2D Slider::GetSliderPosition(time_t time)
{
	// time from slider start to slider end
	float sliderTime = static_cast<float>(m_EndTime - m_StartTime) / m_Repeat;
	float a = std::fmin(m_Repeat, static_cast<float>(time - m_StartTime) / sliderTime);
	int repeatCounter = floor(a);
	float cf = (a - repeatCounter);

	float coef = (repeatCounter % 2 == 0) ? cf : 1 - cf;
	coef = max(0, min(1, coef));

	return m_Points[round(coef * (m_Points.size() - 1))];
}

Vector2D Slider::GetPosition(time_t tick)
{
	if (tick > m_StartTime)
	{
		return GetSliderPosition(tick);
	}
	else
	{
		return m_Point;
	}
}

uint32_t Slider::PathLengthRemain(time_t tick)
{
	if (tick < m_StartTime)
		return 0;

	float sliderTime = m_EndTime - m_StartTime;
	float time = tick - m_StartTime;
	float a = time / sliderTime;
	uint16_t totalLength = m_PixelLength * m_Repeat;

	return totalLength - totalLength * a;
}