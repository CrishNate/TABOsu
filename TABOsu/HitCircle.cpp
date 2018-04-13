#include "stdafx.h"
#include "HitCircle.h"

Vector2D HitCircle::GetCursorMove(time_t tick, time_t lastTime, Vector2D lastPos)
{
	float offset = (m_StartTime - lastTime) < 100 ? 0 : 50;
	float hitCircleTime = fmin(1000, static_cast<float>((m_StartTime - offset) - lastTime));
	float koef = static_cast<float>(tick - lastTime) / hitCircleTime;

	return lastPos + (m_Point - lastPos) * fmin(1, koef);
}