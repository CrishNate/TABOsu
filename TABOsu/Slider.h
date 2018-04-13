#include "OsuEntity.h"

enum SliderType
{
	ST_None = 0,
	ST_Linear = 1,
	ST_Bezire = 2,
	ST_Perfect = 3,
	ST_Catmull = 4,
};

const static std::unordered_map<char, SliderType> charToSliderType = {
	{ 'L', SliderType::ST_Linear },
	{ 'P', SliderType::ST_Perfect },
	{ 'B', SliderType::ST_Bezire },
	{ 'C', SliderType::ST_Catmull }
};

class Slider
	: public OsuEntity
{
public:

	Slider(Vector2D point, int32_t startTime, int32_t endTime, uint16_t repeat, uint32_t pixelLength, const std::vector<Vector2D>& points, SliderType sliderType)
		: OsuEntity(point, startTime, endTime)
		, m_Repeat(repeat)
		, m_Points(points)
		, m_SliderType(sliderType)
		, m_PixelLength(pixelLength)
	{ }

	Vector2D GetSliderPosition(time_t time);
	Vector2D GetCursorMove(time_t tick, time_t lastTime, Vector2D lastPos);
	virtual OsuEntityType GetType() { return OsuEntityType::OET_Slider; }

private:

	uint16_t m_Repeat;
	uint32_t m_PixelLength;
	std::vector<Vector2D> m_Points;
	SliderType m_SliderType;
};