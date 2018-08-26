#include "OsuEntity.h"

class Spinner
	: public OsuEntity
{
public:

	Spinner(Vector2D point, int32_t startTime, int32_t endTime)
		: OsuEntity(point, startTime, endTime)
		, m_LastTime(0)
		, m_Rotation(0)
	{ }

	Vector2D GetPosition(time_t tick);
	virtual OsuEntityType GetType() { return OsuEntityType::OET_Spinner; }

private:
	time_t m_LastTime;
	float m_Rotation;
};