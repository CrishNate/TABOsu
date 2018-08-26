// Copyright (c) 2007-2018 ppy Pty Ltd <contact@ppy.sh>.
// Licensed under the MIT Licence - https://raw.githubusercontent.com/ppy/osu/master/LICENCE
#include "math.h"
#include <vector>
#include <stack>

static uint32_t factorial(uint32_t n)
{
	return (n <= 1) ? 1 : factorial(n - 1) * n;
}

static Vector2D SimpleBezireCurve(Vector2D start, Vector2D middle, Vector2D end, float t)
{
	Vector2D p = { 0, 0 };
	std::vector<Vector2D> pts = { start, middle, end };
	int n = pts.size() - 1;

	t = max(0, min(1, t));

	for (int i = 0; i < pts.size(); i++) 
	{
		float b = (factorial(n) / (factorial(i) * factorial(n - i))) * std::pow(t, i) * std::pow(1 - t, n - i);

		p.x += pts[i].x * b;
		p.y += pts[i].y * b;
	}
	return p;
}

class BezierApproximator
{
public:
	int count;
	std::vector<Vector2D> controlPoints;
	Vector2D* subdivisionBuffer1;
	Vector2D* subdivisionBuffer2;

	const float tolerance = 0.001f;
	const float tolerance_sq = tolerance * tolerance;

	BezierApproximator(const std::vector<Vector2D>& controlPoints)
	{
		this->controlPoints = controlPoints;
		count = controlPoints.size();

		subdivisionBuffer1 = new Vector2D[count];
		subdivisionBuffer2 = new Vector2D[count * 2 - 1];
	}
	
	~BezierApproximator()
	{
		delete[] subdivisionBuffer1;
		delete[] subdivisionBuffer2;
	}

	/// <summary>
	/// Make sure the 2nd order derivative (approximated using finite elements) is within tolerable bounds.
	/// NOTE: The 2nd order derivative of a 2d curve represents its curvature, so intuitively this function
	///       checks (as the name suggests) whether our approximation is _locally_ "flat". More curvy parts
	///       need to have a denser approximation to be more "flat".
	/// </summary>
	/// <param name="controlPoints">The control points to check for flatness.</param>
	/// <returns>Whether the control points are flat enough.</returns>
	bool isFlatEnough(Vector2D* controlPoints)
	{
		for (size_t i = 1; i < count - 1; i++)
			if ((controlPoints[i - 1] - 2 * controlPoints[i] + controlPoints[i + 1]).LenSqr() > tolerance_sq * 4)
				return false;

		return true;
	}

	/// <summary>
	/// Subdivides n control points representing a bezier curve into 2 sets of n control points, each
	/// describing a bezier curve equivalent to a half of the original curve. Effectively this splits
	/// the original curve into 2 curves which result in the original curve when pieced back together.
	/// </summary>
	/// <param name="controlPoints">The control points to split.</param>
	/// <param name="l">Output: The control points corresponding to the left half of the curve.</param>
	/// <param name="r">Output: The control points corresponding to the right half of the curve.</param>
	void subdivide(Vector2D* controlPoints, Vector2D* l, Vector2D* r)
	{
		Vector2D* midpoints = subdivisionBuffer1;

		for (int i = 0; i < count; ++i)
			midpoints[i] = controlPoints[i];

		for (int i = 0; i < count; i++)
		{
			l[i] = midpoints[0];
			r[count - i - 1] = midpoints[count - i - 1];

			for (int j = 0; j < count - i - 1; j++)
				midpoints[j] = (midpoints[j] + midpoints[j + 1]) / 2;
		}
	}

	/// <summary>
	/// This uses <a href="https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm">De Casteljau's algorithm</a> to obtain an optimal
	/// piecewise-linear approximation of the bezier curve with the same amount of points as there are control points.
	/// </summary>
	/// <param name="controlPoints">The control points describing the bezier curve to be approximated.</param>
	/// <param name="output">The points representing the resulting piecewise-linear approximation.</param>
	void approximate(Vector2D* controlPoints, std::vector<Vector2D>& output)
	{
		Vector2D* l = subdivisionBuffer2;
		Vector2D* r = subdivisionBuffer1;

		subdivide(controlPoints, l, r);

		for (int i = 0; i < count - 1; ++i)
			l[count + i] = r[i + 1];

		output.push_back(controlPoints[0]);
		for (int i = 1; i < count - 1; ++i)
		{
			int index = 2 * i;
			Vector2D p = 0.25f * (l[index - 1] + 2 * l[index] + l[index + 1]);
			output.push_back(p);
		}
	}

	/// <summary>
	/// Creates a piecewise-linear approximation of a bezier curve, by adaptively repeatedly subdividing
	/// the control points until their approximation error vanishes below a given threshold.
	/// </summary>
	/// <returns>A list of vectors representing the piecewise-linear approximation.</returns>
	std::vector<Vector2D> CreateBezier()
	{
		std::vector<Vector2D> output;

		if (count == 0)
			return output;

		std::stack<Vector2D*> toFlatten;
		std::stack<Vector2D*> freeBuffers;

		// "toFlatten" contains all the curves which are not yet approximated well enough.
		// We use a stack to emulate recursion without the risk of running into a stack overflow.
		// (More specifically, we iteratively and adaptively refine our curve with a
		// <a href="https://en.wikipedia.org/wiki/Depth-first_search">Depth-first search</a>
		// over the tree resulting from the subdivisions we make.)
		Vector2D* points = new Vector2D[controlPoints.size() - 1];
		for (int i = 0; i < controlPoints.size(); i++)
			points[i] = controlPoints[i];

		toFlatten.push(points);

		Vector2D* leftChild = subdivisionBuffer2;

		while (toFlatten.size() > 0)
		{
			Vector2D* parent = toFlatten.top();
			toFlatten.pop();

			if (isFlatEnough(parent))
			{
				// If the control points we currently operate on are sufficiently "flat", we use
				// an extension to De Casteljau's algorithm to obtain a piecewise-linear approximation
				// of the bezier curve represented by our control points, consisting of the same amount
				// of points as there are control points.
				approximate(parent, output);
				freeBuffers.push(parent);
				continue;
			}

			// If we do not yet have a sufficiently "flat" (in other words, detailed) approximation we keep
			// subdividing the curve we are currently operating on.
			Vector2D* rightChild = freeBuffers.size() > 0 ? freeBuffers.top() : new Vector2D[count];
			if (freeBuffers.size() > 0) 
				freeBuffers.pop();

			subdivide(parent, leftChild, rightChild);

			// We re-use the buffer of the parent for one of the children, so that we save one allocation per iteration.
			for (int i = 0; i < count; ++i)
				parent[i] = leftChild[i];

			toFlatten.push(rightChild);
			toFlatten.push(parent);
		}

		output.push_back(controlPoints[count - 1]);
		//delete[] points;

		return output;
	}
};