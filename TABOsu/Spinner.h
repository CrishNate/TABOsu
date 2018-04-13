#include "OsuEntity.h"

class Spinner
	: public OsuEntity
{
public:

	Spinner(Vector2D point, int32_t startTime, int32_t endTime)
		: OsuEntity(point, startTime, endTime)
	{ }

	Vector2D GetCursorMove(time_t tick, time_t lastTime, Vector2D lastPos, RECT winTrans);
	virtual OsuEntityType GetType() { return OsuEntityType::OET_Spinner; }

private:

};