#include "stdafx.h"
#include "Spinner.h"

Vector2D Spinner::GetCursorMove(time_t tick, time_t lastTime, Vector2D lastPos, RECT winTrans)
{
	if (tick > m_StartTime)
	{
		float radius = 50.0f;
		Vector2D point = m_Point + Vector2D(cos(tick / 20.0f), sin(tick / 20.0f)) * radius;
	}
	else
	{
		float offset = (m_StartTime - lastTime) < 100 ? 0 : 50;
		float hitCircleTime = fmin(1000, static_cast<float>((m_StartTime - offset) - lastTime));
		float koef = static_cast<float>(tick - lastTime) / hitCircleTime;

		return lastPos + (m_Point - lastPos) * fmin(1, koef);
	}
}