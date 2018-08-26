#include "OsuEntity.h"
#include "SliderPath.h"

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

	Vector2D GetPosition(time_t tick);
	uint32_t PathLengthRemain(time_t tick);
	virtual OsuEntityType GetType() { return OsuEntityType::OET_Slider; }

private:

	Vector2D GetSliderPosition(time_t time);

	uint16_t m_Repeat;
	uint32_t m_PixelLength;
	std::vector<Vector2D> m_Points;
	SliderType m_SliderType;
};