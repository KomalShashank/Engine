#pragma once



class Vector2
{
public:
	Vector2();
	Vector2(float initialX, float initialY);
	Vector2(const Vector2& copyableVector2);

	Vector2 operator+(const Vector2& addableVector2) const;
	Vector2 operator-(const Vector2& subtractableVector2) const;
	Vector2 operator*(float multipliableValue) const;
	Vector2 operator/(float dividableValue) const;

	void operator+=(const Vector2& addableVector2);
	void operator-=(const Vector2& subtractableVector2);
	void operator*=(float multipliableValue);
	void operator/=(float dividableValue);

	bool operator==(const Vector2& comparableVector2) const;
	bool operator!=(const Vector2& comparableVector2) const;

	float GetVector2Magnitude() const;
	Vector2 GetNormalizedVector2() const;
	static float DotProduct(const Vector2& firstDottableVector2, const Vector2& secondDottableVector2);

	Vector2 GetNegatedVector2() const;

public:
	static const Vector2 ZERO;
	static const Vector2 ONE;
	static const Vector2 X_AXIS;
	static const Vector2 Y_AXIS;

public:
	float X;
	float Y;
};