// Copyright (c) 2007-2018 ppy Pty Ltd <contact@ppy.sh>.
// Licensed under the MIT Licence - https://raw.githubusercontent.com/ppy/osu/master/LICENCE

class CatmullApproximator
{
public:
	/// <summary>
	/// The amount of pieces to calculate for each controlpoint quadruplet.
	/// </summary>
	const int detail = 50;

	std::vector<Vector2D> controlPoints;

	CatmullApproximator(const std::vector<Vector2D>& controlPoints)
	{
		this->controlPoints = controlPoints;
	}


	/// <summary>
	/// Creates a piecewise-linear approximation of a Catmull-Rom spline.
	/// </summary>
	/// <returns>A list of vectors representing the piecewise-linear approximation.</returns>
	std::vector<Vector2D> CreateCatmull()
	{
		std::vector<Vector2D> result;

		for (int i = 0; i < controlPoints.size() - 1; i++)
		{
			Vector2D v1 = i > 0 ? controlPoints[i - 1] : controlPoints[i];
			Vector2D v2 = controlPoints[i];
			Vector2D v3 = i < controlPoints.size() - 1 ? controlPoints[i + 1] : v2 + v2 - v1;
			Vector2D v4 = i < controlPoints.size() - 2 ? controlPoints[i + 2] : v3 + v3 - v2;

			for (int c = 0; c < detail; c++)
			{
				result.push_back(findPoint(v1, v2, v3, v4, (float)c / detail));
				result.push_back(findPoint(v1, v2, v3, v4, (float)(c + 1) / detail));
			}
		}

		return result;
	}

	/// <summary>
	/// Finds a point on the spline at the position of a parameter.
	/// </summary>
	/// <param name="vec1">The first vector.</param>
	/// <param name="vec2">The second vector.</param>
	/// <param name="vec3">The third vector.</param>
	/// <param name="vec4">The fourth vector.</param>
	/// <param name="t">The parameter at which to find the point on the spline, in the range [0, 1].</param>
	/// <returns>The point on the spline at <paramref name="t"/>.</returns>
	Vector2D findPoint(Vector2D vec1, Vector2D vec2, Vector2D vec3, Vector2D vec4, float t)
	{
		float t2 = t * t;
		float t3 = t * t2;

		Vector2D result;
		result.x = 0.5f * (2.0f * vec2.x + (-vec1.x + vec3.x) * t + (2.0f * vec1.x - 5.0f * vec2.x + 4.0f * vec3.x - vec4.x) * t2 + (-vec1.x + 3.0f * vec2.x - 3.0f * vec3.x + vec4.x) * t3);
		result.y = 0.5f * (2.0f * vec2.y + (-vec1.y + vec3.y) * t + (2.0f * vec1.y - 5.0f * vec2.y + 4.0f * vec3.y - vec4.y) * t2 + (-vec1.y + 3.0f * vec2.y - 3.0f * vec3.y + vec4.y) * t3);

		return result;
	}
};