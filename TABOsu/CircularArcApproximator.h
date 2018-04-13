#include "math.h"
#include <vector>

#define epsilon 0.0001f

class CircularArcApproximator
{
public:
	Vector2D a;
	Vector2D b;
	Vector2D c;

	int amountPoints;
	const float tolerance = 0.1f;

	CircularArcApproximator(Vector2D a, Vector2D b, Vector2D c)
	{
		this->a = a;
		this->b = b;
		this->c = c;
	}

	/// <summary>
	/// Creates a piecewise-linear approximation of a circular arc curve.
	/// </summary>
	/// <returns>A list of vectors representing the piecewise-linear approximation.</returns>
	std::vector<Vector2D> CreateArc()
	{
		float aSq = (b - c).LenSqr();
		float bSq = (a - c).LenSqr();
		float cSq = (a - b).LenSqr();

		// If we have a degenerate triangle where a side-length is almost zero, then give up and fall
		// back to a more numerically stable method.
		if (abs(aSq) < epsilon || abs(bSq) < epsilon || abs(cSq) < epsilon)
			return std::vector<Vector2D>();

		float s = aSq * (bSq + cSq - aSq);
		float t = bSq * (aSq + cSq - bSq);
		float u = cSq * (aSq + bSq - cSq);

		float sum = s + t + u;

		// If we have a degenerate triangle with an almost-zero size, then give up and fall
		// back to a more numerically stable method.
		if (abs(sum) < epsilon)
			return std::vector<Vector2D>();

		Vector2D centre = (s * a + t * b + u * c) / sum;
		Vector2D dA = a - centre;
		Vector2D dC = c - centre;

		float r = dA.Len();

		double thetaStart = atan2(dA.y, dA.x);
		double thetaEnd = atan2(dC.y, dC.x);

		while (thetaEnd < thetaStart)
			thetaEnd += 2 * PI;

		double dir = 1;
		double thetaRange = thetaEnd - thetaStart;

		// Decide in which direction to draw the circle, depending on which side of
		// AC B lies.
		Vector2D orthoAtoC = c - a;
		orthoAtoC = Vector2D(orthoAtoC.y, -orthoAtoC.x);
		if (DotProduct(orthoAtoC, b - a) < 0)
		{
			dir = -dir;
			thetaRange = 2 * PI - thetaRange;
		}

		// We select the amount of points for the approximation by requiring the discrete curvature
		// to be smaller than the provided tolerance. The exact angle required to meet the tolerance
		// is: 2 * Math.Acos(1 - TOLERANCE / r)
		// The special case is required for extremely short sliders where the radius is smaller than
		// the tolerance. This is a pathological rather than a realistic case.
		amountPoints = 2 * r <= tolerance ? 2 : fmax(2, (int)ceilf(thetaRange / (2 * acos(1 - tolerance / r))));

		std::vector<Vector2D> output;

		for (int i = 0; i < amountPoints; ++i)
		{
			double fract = (double)i / (amountPoints - 1);
			double theta = thetaStart + dir * fract * thetaRange;
			Vector2D o = Vector2D((float)cos(theta), (float)sin(theta)) * r;
			output.push_back(centre + o);
		}

		return output;
	}
};