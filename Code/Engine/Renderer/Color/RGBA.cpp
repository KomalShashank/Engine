#include "Engine/Renderer/Color/RGBA.hpp"



RGBA::RGBA()
{

}



RGBA::RGBA(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) :
m_Red(red),
m_Green(green),
m_Blue(blue),
m_Alpha(alpha)
{

}



const RGBA RGBA::BLACK(0, 0, 0, 255);
const RGBA RGBA::WHITE(255, 255, 255, 255);
const RGBA RGBA::RED(255, 0, 0, 255);
const RGBA RGBA::GREEN(0, 255, 0, 255);
const RGBA RGBA::BLUE(0, 0, 255, 255);
const RGBA RGBA::YELLOW(255, 255, 0, 255);
const RGBA RGBA::MAGENTA(255, 0, 255, 255);
const RGBA RGBA::CYAN(0, 255, 255, 255);