#pragma once



class Vector3
{
public:
	Vector3();
	Vector3(float initialX, float initialY, float initialZ);
	Vector3(const Vector3& copyableVector3);

	Vector3 operator+(const Vector3& addableVector3) const;
	Vector3 operator-(const Vector3& subtractableVector3) const;
	Vector3 operator*(float multipliableValue) const;
	Vector3 operator/(float dividableValue) const;

	void operator+=(const Vector3& addableVector3);
	void operator-=(const Vector3& subtractableVector3);
	void operator*=(float multipliableValue);
	void operator/=(float dividableValue);

	bool operator==(const Vector3& comparableVector3) const;
	bool operator!=(const Vector3& comparableVector3) const;

	float GetVector3Magnitude() const;
	Vector3 GetNormalizedVector3() const;
	static float DotProduct(const Vector3& firstDottableVector3, const Vector3& secondDottableVector3);
	static Vector3 CrossProduct(const Vector3& firstCrossableVector3, const Vector3& secondCrossableVector3);

	Vector3 GetNegatedVector3() const;

public:
	static const Vector3 ZERO;
	static const Vector3 ONE;
	static const Vector3 X_AXIS;
	static const Vector3 Y_AXIS;
	static const Vector3 Z_AXIS;

public:
	float X;
	float Y;
	float Z;
};