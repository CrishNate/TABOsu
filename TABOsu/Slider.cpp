#include "stdafx.h"
#include "Slider.h"
#include "CircularArcApproximator.h"
#include "BezierApproximator.h"

uint16_t factorial(uint16_t n)
{
	return (n <= 1) ? 1 : factorial(n - 1) * n;
}

Vector2D CalcBezierCurvePoint(const std::vector<Vector2D>& m_Points, float t)
{
	Vector2D p = { 0, 0 };
	int n = m_Points.size() - 1;

	t = std::fmax(0, std::fmin(1, t));

	for (int i = 0; i < m_Points.size(); i++)
	{
		float b = (factorial(n) / (factorial(i) * factorial(n - i))) * std::pow(t, i) * std::pow(1 - t, n - i);

		p.x += m_Points[i].x * b;
		p.y += m_Points[i].y * b;
	}
	return p;
}


Vector2D Slider::GetSliderPosition(time_t time)
{
	// time from slider start to slider end
	float sliderTime = static_cast<float>(m_EndTime - m_StartTime) / m_Repeat;
	float a = std::fmin(m_Repeat, static_cast<float>(time - m_StartTime) / sliderTime);
	int m_RepeatCounter = ceil(a) - 1;

	float coef = (m_RepeatCounter % 2 == 0) ? (a - m_RepeatCounter) : 1 - (a - m_RepeatCounter);
	coef = max(0, min(1, coef));

	if (m_SliderType == SliderType::ST_Bezire)
	{
		BezierApproximator* ba = new BezierApproximator(m_Points);
		std::vector<Vector2D> points = ba->CreateBezier();
		delete ba;

		return points[round(coef * (points.size() - 1))];
	}
	else if (m_SliderType == SliderType::ST_Perfect)
	{
		CircularArcApproximator* caa = new CircularArcApproximator(m_Points[0], m_Points[1], m_Points[2]);
		std::vector<Vector2D> points = caa->CreateArc();
		delete caa;

		return points[round(coef * (points.size() - 1))];
	}
	else
	{
		Vector2D startPos = m_Point;
		Vector2D endPos = m_Points.at(m_Points.size() - 1);

		return startPos + (endPos - startPos) * coef;
	}
}

Vector2D Slider::GetCursorMove(time_t tick, time_t lastTime, Vector2D lastPos)
{
	if (tick > m_StartTime)
	{
		if (m_PixelLength > 50)
		{
			Vector2D point = GetSliderPosition(tick);
			return point;
		}
	}
	else
	{
		float offset = (m_StartTime - lastTime) < 100 ? 0 : 50;
		float hitCircleTime = fmin(1000, static_cast<float>((m_StartTime - offset) - lastTime));
		float koef = static_cast<float>(tick - lastTime) / hitCircleTime;
		return lastPos + (m_Point - lastPos) * fmin(1, koef);
	}
}