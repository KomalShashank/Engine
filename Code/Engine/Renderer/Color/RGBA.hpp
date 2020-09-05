#pragma once



class RGBA
{
public:
	RGBA();
	RGBA(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);

public:
	static const RGBA BLACK;
	static const RGBA WHITE;
	static const RGBA RED;
	static const RGBA GREEN;
	static const RGBA BLUE;
	static const RGBA YELLOW;
	static const RGBA MAGENTA;
	static const RGBA CYAN;

public:
	unsigned char m_Red;
	unsigned char m_Green;
	unsigned char m_Blue;
	unsigned char m_Alpha;
};