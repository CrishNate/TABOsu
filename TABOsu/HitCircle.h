#include "OsuEntity.h"

class HitCircle 
	: public OsuEntity
{
public:

	HitCircle(Vector2D point, int32_t startTime, int32_t endTime)
		: OsuEntity(point, startTime, endTime)
	{ }

	Vector2D GetPosition(time_t tick);
	virtual OsuEntityType GetType() { return OsuEntityType::OET_HitCircle; }

private:
};