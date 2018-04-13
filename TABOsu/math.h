// Impulse Engine Math : IEMath
#include <cmath> // abs, sqrt
#include <cassert> // assert
#include <algorithm> // max, min

#ifndef MASSLIB_H
#define MASSLIB_H

#define PI 3.141592741f
#define EPSILON 0.00000000001f

struct Vector2D
{
	union
	{
		float m[1][1];
		float v[2];

		struct
		{
			float x;
			float y;
		};
	};


	Vector2D()
	{ }


	Vector2D(float x_, float y_)
		: x(x_)
		, y(y_)
	{ }


	void Set(float x_, float y_)
	{
		x = x_;
		y = y_;
	}


	Vector2D operator-(void) const
	{
		return Vector2D(-x, -y);
	}


	Vector2D operator*(float s) const
	{
		return Vector2D(x * s, y * s);
	}


	Vector2D operator/(float s) const
	{
		return Vector2D(x / s, y / s);
	}


	void operator*=(float s)
	{
		x *= s;
		y *= s;
	}


	bool operator==(const Vector2D& rhs)
	{
		return x == rhs.x && y == rhs.y;
	}


	bool operator!=(const Vector2D& rhs)
	{
		return !(x == rhs.x && y == rhs.y);
	}


	Vector2D operator+(const Vector2D& rhs) const
	{
		return Vector2D(x + rhs.x, y + rhs.y);
	}


	Vector2D operator+(float s) const
	{
		return Vector2D(x + s, y + s);
	}


	void operator+=(const Vector2D& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}


	Vector2D operator-(const Vector2D& rhs) const
	{
		return Vector2D(x - rhs.x, y - rhs.y);
	}


	void operator-=(const Vector2D& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
	}


	float LenSqr(void) const
	{
		return x * x + y * y;
	}


	float Distance(const Vector2D& rhs) const
	{
		return std::sqrt(std::pow(rhs.x - x, 2) + std::pow(rhs.y - y, 2));
	}


	float Len(void) const
	{
		return std::sqrt(x * x + y * y);
	}


	void Rotate(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);

		float xp = x * c - y * s;
		float yp = x * s + y * c;

		x = xp;
		y = yp;
	}

	Vector2D GetRotated(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);

		float xp = x * c - y * s;
		float yp = x * s + y * c;

		return Vector2D(xp, yp);
	}

	void Normalize(void)
	{
		float len = Len();

		if (len > EPSILON)
		{
			float invLen = 1.0f / len;
			x *= invLen;
			y *= invLen;
		}
	}
};

inline Vector2D operator*(float s, const Vector2D& v)
{
	return Vector2D(s * v.x, s * v.y);
}

struct Mat2
{
	union
	{
		struct
		{
			float m00, m01;
			float m10, m11;
		};

		float m[2][2];
		float v[4];
	};

	Mat2() {}
	Mat2(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);

		m00 = c; m01 = -s;
		m10 = s; m11 = c;
	}

	Mat2(float a, float b, float c, float d)
		: m00(a), m01(b)
		, m10(c), m11(d)
	{
	}

	void Set(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);

		m00 = c; m01 = -s;
		m10 = s; m11 = c;
	}

	Mat2 Abs(void) const
	{
		return Mat2(std::abs(m00), std::abs(m01), std::abs(m10), std::abs(m11));
	}

	Vector2D AxisX(void) const
	{
		return Vector2D(m00, m10);
	}

	Vector2D AxisY(void) const
	{
		return Vector2D(m01, m11);
	}

	Mat2 Transpose(void) const
	{
		return Mat2(m00, m10, m01, m11);
	}

	const Vector2D operator*(const Vector2D& rhs) const
	{
		return Vector2D(m00 * rhs.x + m01 * rhs.y, m10 * rhs.x + m11 * rhs.y);
	}

	const Mat2 operator*(const Mat2& rhs) const
	{
		// [00 01]  [00 01]
		// [10 11]  [10 11]

		return Mat2(
			m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0],
			m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1],
			m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0],
			m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1]
			);
	}
};

inline Vector2D Min(const Vector2D& a, const Vector2D& b)
{
	return Vector2D(std::fmin(a.x, b.x), std::fmin(a.y, b.y));
}

inline Vector2D Max(const Vector2D& a, const Vector2D& b)
{
	return Vector2D(std::fmax(a.x, b.x), std::fmax(a.y, b.y));
}

inline float DotProduct(const Vector2D& a, const Vector2D& b)
{
	return a.x * b.x + a.y * b.y;
}

inline float DistSqr(const Vector2D& a, const Vector2D& b)
{
	Vector2D c = a - b;
	return DotProduct(c, c);
}

inline float Dist(const Vector2D& a, const Vector2D& b)
{
	Vector2D c = a - b;
	return std::sqrt(DotProduct(c, c));
}

inline Vector2D Cross(const Vector2D& v, float a)
{
	return Vector2D(a * v.y, -a * v.x);
}

inline Vector2D Cross(float a, const Vector2D& v)
{
	return Vector2D(-a * v.y, a * v.x);
}

inline float Cross(const Vector2D& a, const Vector2D& b)
{
	return a.x * b.y - a.y * b.x;
}

// Comparison with tolerance of EPSILON
inline bool Equal(float a, float b)
{
	// <= instead of < for NaN comparison safety
	return std::abs(a - b) <= EPSILON;
}

inline float Sqr(float a)
{
	return a * a;
}

inline float Clamp(float min, float max, float a)
{
	if (a < min) return min;
	if (a > max) return max;
	return a;
}

inline int Round(float a)
{
	return (int)(a + 0.5f);
}


inline float ToDegrees(float a)
{
	return a * 180 / PI;
}


inline float ToRadians(float a)
{
	return a * PI / 180;
}

inline float Random(float l, float h)
{
	float a = (float)rand();
	a /= RAND_MAX;
	a = (h - l) * a + l;
	return a;
}

inline bool BiasGreaterThan(float a, float b)
{
	const float k_biasRelative = 0.95f;
	const float k_biasAbsolute = 0.01f;
	return a >= b * k_biasRelative + a * k_biasAbsolute;
}

const float gravityScale = 50.0f;
const Vector2D gravity(0, 10.0f * gravityScale);
const float dt = 1.0f / 60.0f;

#endif /* MASSLIB_H */