#pragma once



class IntVector2
{
public:
	IntVector2();
	IntVector2(int initialX, int initialY);
	IntVector2(const IntVector2& copyableIntVector2);

	IntVector2 operator+(const IntVector2& addableIntVector2) const;
	IntVector2 operator-(const IntVector2& subtractableIntVector2) const;
	IntVector2 operator*(int multipliableIntValue) const;
	IntVector2 operator/(int dividableIntValue) const;

	void operator+=(const IntVector2& addableIntVector2);
	void operator-=(const IntVector2& subtractableIntVector2);
	void operator*=(int multipliableIntValue);
	void operator/=(int dividableIntValue);

	bool operator==(const IntVector2& comparableIntVector2) const;
	bool operator!=(const IntVector2& comparableIntVector2) const;

	IntVector2 GetNegatedIntVector2() const;

public:
	static const IntVector2 ZERO;
	static const IntVector2 ONE;
	static const IntVector2 X_AXIS;
	static const IntVector2 Y_AXIS;

public:
	int X;
	int Y;
};