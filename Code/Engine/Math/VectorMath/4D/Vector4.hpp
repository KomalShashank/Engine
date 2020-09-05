#pragma once



class Vector4
{
public:
	Vector4();
	Vector4(float initialX, float initialY, float initialZ, float initialT);
	Vector4(const Vector4& copyableVector4);

	Vector4 operator+(const Vector4& addableVector4) const;
	Vector4 operator-(const Vector4& subtractableVector4) const;
	Vector4 operator*(float multipliableValue) const;
	Vector4 operator/(float dividableValue) const;

	void operator+=(const Vector4& addableVector4);
	void operator-=(const Vector4& subtractableVector4);
	void operator*=(float multipliableValue);
	void operator/=(float dividableValue);

	bool operator==(const Vector4& comparableVector4) const;
	bool operator!=(const Vector4& comparableVector4) const;

	float GetVector4Magnitude() const;
	Vector4 GetNormalizedVector4() const;
	static float DotProduct(const Vector4& firstDottableVector4, const Vector4& secondDottableVector4);

	Vector4 GetNegatedVector4() const;

public:
	static const Vector4 ZERO;
	static const Vector4 ONE;
	static const Vector4 X_AXIS;
	static const Vector4 Y_AXIS;
	static const Vector4 Z_AXIS;
	static const Vector4 T_AXIS;

public:
	float X;
	float Y;
	float Z;
	float T;
};