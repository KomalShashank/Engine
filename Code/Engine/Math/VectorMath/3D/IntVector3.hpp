#pragma once



class IntVector3
{
public:
	IntVector3();
	IntVector3(int initialX, int initialY, int initialZ);
	IntVector3(const IntVector3& copyableIntVector3);

	IntVector3 operator+(const IntVector3& addableIntVector3) const;
	IntVector3 operator-(const IntVector3& subtractableIntVector3) const;
	IntVector3 operator*(int multipliableIntValue) const;
	IntVector3 operator/(int dividableIntValue) const;

	void operator+=(const IntVector3& addableIntVector3);
	void operator-=(const IntVector3& subtractableIntVector3);
	void operator*=(int multipliableIntValue);
	void operator/=(int dividableIntValue);

	bool operator==(const IntVector3& comparableIntVector3) const;
	bool operator!=(const IntVector3& comparableIntVector3) const;

	IntVector3 GetNegatedIntVector3() const;

public:
	static const IntVector3 ZERO;
	static const IntVector3 ONE;
	static const IntVector3 X_AXIS;
	static const IntVector3 Y_AXIS;
	static const IntVector3 Z_AXIS;

public:
	int X;
	int Y;
	int Z;
};