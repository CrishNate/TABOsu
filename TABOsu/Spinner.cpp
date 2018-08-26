#include "stdafx.h"
#include "Spinner.h"

Vector2D Spinner::GetPosition(time_t tick)
{
	float dTime = (tick - m_LastTime);
	m_LastTime = tick;

	float radius = 50.0f + cos(m_Rotation) * sin(1.1 * m_Rotation) * 20.0f;
	Vector2D point = m_Point + Vector2D(cos(m_Rotation), sin(m_Rotation)) * radius;
	m_Rotation += dTime * Random(1.0f, 1.1f) / 20.0f;

	return point;
}