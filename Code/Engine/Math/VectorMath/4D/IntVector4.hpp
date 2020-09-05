#pragma once



class IntVector4
{
public:
	IntVector4();
	IntVector4(int initialX, int initialY, int initialZ, int initialT);
	IntVector4(const IntVector4& copyableIntVector4);

	IntVector4 operator+(const IntVector4& addableIntVector4) const;
	IntVector4 operator-(const IntVector4& subtractableIntVector4) const;
	IntVector4 operator*(int multipliableValue) const;
	IntVector4 operator/(int dividableValue) const;

	void operator+=(const IntVector4& addableIntVector4);
	void operator-=(const IntVector4& subtractableIntVector4);
	void operator*=(int multipliableValue);
	void operator/=(int dividableValue);

	bool operator==(const IntVector4& comparableIntVector4) const;
	bool operator!=(const IntVector4& comparableIntVector4) const;

	IntVector4 GetNegatedIntVector4() const;

public:
	static const IntVector4 ZERO;
	static const IntVector4 ONE;
	static const IntVector4 X_AXIS;
	static const IntVector4 Y_AXIS;
	static const IntVector4 Z_AXIS;
	static const IntVector4 T_AXIS;

public:
	int X;
	int Y;
	int Z;
	int T;
};