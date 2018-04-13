#include "OsuEntity.h"

class HitCircle 
	: public OsuEntity
{
public:

	HitCircle(Vector2D point, int32_t startTime, int32_t endTime)
		: OsuEntity(point, startTime, endTime)
	{ }

	Vector2D GetCursorMove(time_t tick, time_t lastTime, Vector2D lastPos);
	virtual OsuEntityType GetType() { return OsuEntityType::OET_HitCircle; }

private:
};