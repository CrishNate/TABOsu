#include "math.h"
#include <vector>
#include "CircularArcApproximator.h"
#include "BezierApproximator.h"

#define lengthDivBezire 1.0f
#define lengthDivLinear 0.1f

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

class SliderPath
{
public:

	static std::vector<Vector2D> DivideLine(const std::vector<Vector2D>& points, float divs)
	{
		Vector2D startPos = points[0];
		Vector2D endPos = points[1];

		std::vector<Vector2D> result;

		float pathLength = (endPos - startPos).Len();
		uint32_t pointsCount = pathLength / divs;

		for (float i = 0; i < 1; i += 1.0f / (float)pointsCount)
		{
			Vector2D point = startPos + (endPos - startPos) * i;
			result.push_back(point);
		}

		return result;
	}

	static std::vector<Vector2D> CreatePath(const std::vector<Vector2D>& points, SliderType sliderType)
	{
		if (sliderType == SliderType::ST_Perfect)
		{
			CircularArcApproximator* caa = new CircularArcApproximator(points[0], points[1], points[2]);
			std::vector<Vector2D> points = caa->CreateArc();
			delete caa;

			return points;
		}
		else if (sliderType == SliderType::ST_Linear)
		{
			return DivideLine(points, lengthDivLinear);
		}
		else
		{
			uint16_t index = 0;
			Vector2D point = points[0];
			std::vector<Vector2D> result;

			while (true)
			{
				std::vector<Vector2D> temPoints = { points[index] };

				// separating curves
				while (true)
				{
					index++;
					if (index > points.size() - 1)
						break;

					Vector2D currentPoint = points[index];
					temPoints.push_back(currentPoint);

					if (point == currentPoint)
						break;

					point = currentPoint;
				}

				if (temPoints.size() == 2)
				{
					std::vector<Vector2D> ps = DivideLine(temPoints, lengthDivBezire);
					result.insert(result.end(), ps.begin(), ps.end());
				}
				else
				{
					// creating bezire
					BezierApproximator* bza = new BezierApproximator(temPoints);
					std::vector<Vector2D> bezire = bza->CreateBezier();
					delete bza;

					Vector2D lastPoint = bezire[0];
					std::vector<Vector2D> ps = { lastPoint };

					for (float i = 0; i < 1; i += 1.0f / 10000.0f)
					{
						uint32_t currentInx = round(i * (bezire.size() - 1));
						Vector2D currentPoint = bezire[currentInx];

						if ((currentPoint - lastPoint).Len() >= lengthDivBezire)
						{
							lastPoint = currentPoint;
							ps.push_back(currentPoint);
						}
					}

					result.insert(result.end(), ps.begin(), ps.end());
				}

				if (index > points.size() - 1)
					break;
			}

			return result;
		}
	}
};