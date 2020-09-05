// Code adapted from beiserloh
#include "Engine/Math/Noise/Noise.hpp"
#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/VectorMath/2D/IntVector2.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



unsigned int GetRawNoiseIn1D(int index, unsigned int randomSeed /*= 0*/)
{
	const unsigned int BIT_NOISE_1 = 0xB5297A4D;
	const unsigned int BIT_NOISE_2 = 0x68E31DA4;
	const unsigned int BIT_NOISE_3 = 0x1B56C4E9;

	unsigned int scrambledBits = static_cast<unsigned int>(index);
	scrambledBits *= BIT_NOISE_1;
	scrambledBits += randomSeed;
	scrambledBits ^= (scrambledBits >> 8);
	scrambledBits += BIT_NOISE_2;
	scrambledBits ^= (scrambledBits << 8);
	scrambledBits *= BIT_NOISE_3;
	scrambledBits ^= (scrambledBits >> 8);

	return scrambledBits;
}



unsigned int GetRawNoiseIn2D(int indexX, int indexY, unsigned int randomSeed /*= 0*/)
{
	const int PRIME_NUMBER = 198491317;
	int index = indexX + (PRIME_NUMBER * indexY);

	return GetRawNoiseIn1D(index, randomSeed);
}



unsigned int GetRawNoiseIn3D(int indexX, int indexY, int indexZ, unsigned int randomSeed /*= 0*/)
{
	const int PRIME_NUMBER_1 = 198491317;
	const int PRIME_NUMBER_2 = 6542989;
	int index = indexX + (PRIME_NUMBER_1 * indexY) + (PRIME_NUMBER_2 * indexZ);

	return GetRawNoiseIn1D(index, randomSeed);
}



unsigned int GetRawNoiseIn4D(int indexX, int indexY, int indexZ, int indexW, unsigned int randomSeed /*= 0*/)
{
	const int PRIME_NUMBER_1 = 198491317;
	const int PRIME_NUMBER_2 = 6542989;
	const int PRIME_NUMBER_3 = 357239;
	int index = indexX + (PRIME_NUMBER_1 * indexY) + (PRIME_NUMBER_2 * indexZ) + (PRIME_NUMBER_3 * indexW);

	return GetRawNoiseIn1D(index, randomSeed);
}



float GetRawNoiseIn1DFromZeroToOne(int index, unsigned int randomSeed /*= 0*/)
{
	const double MAXIMUM_UNSIGNED_INT_INVERSE = (1.0 / static_cast<double>(0xFFFFFFFF));
	float resultantNoise = static_cast<float>(MAXIMUM_UNSIGNED_INT_INVERSE * static_cast<double>(GetRawNoiseIn1D(index, randomSeed)));

	return resultantNoise;
}



float GetRawNoiseIn2DFromZeroToOne(int indexX, int indexY, unsigned int randomSeed /*= 0*/)
{
	const double MAXIMUM_UNSIGNED_INT_INVERSE = (1.0 / static_cast<double>(0xFFFFFFFF));
	float resultantNoise = static_cast<float>(MAXIMUM_UNSIGNED_INT_INVERSE * static_cast<double>(GetRawNoiseIn2D(indexX, indexY, randomSeed)));

	return resultantNoise;
}



float GetRawNoiseIn3DFromZeroToOne(int indexX, int indexY, int indexZ, unsigned int randomSeed /*= 0*/)
{
	const double MAXIMUM_UNSIGNED_INT_INVERSE = (1.0 / static_cast<double>(0xFFFFFFFF));
	float resultantNoise = static_cast<float>(MAXIMUM_UNSIGNED_INT_INVERSE * static_cast<double>(GetRawNoiseIn3D(indexX, indexY, indexZ, randomSeed)));

	return resultantNoise;
}



float GetRawNoiseIn4DFromZeroToOne(int indexX, int indexY, int indexZ, int indexW, unsigned int randomSeed /*= 0*/)
{
	const double MAXIMUM_UNSIGNED_INT_INVERSE = (1.0 / static_cast<double>(0xFFFFFFFF));
	float resultantNoise = static_cast<float>(MAXIMUM_UNSIGNED_INT_INVERSE * static_cast<double>(GetRawNoiseIn4D(indexX, indexY, indexZ, indexW, randomSeed)));

	return resultantNoise;
}



float GetRawNoiseIn1DFromMinusOneToOne(int index, unsigned int randomSeed /*= 0*/)
{
	const double MAXIMUM_INT_INVERSE = (1.0 / static_cast<double>(0x7FFFFFFF));
	float resultantNoise = static_cast<float>(MAXIMUM_INT_INVERSE * static_cast<double>(static_cast<int>(GetRawNoiseIn1D(index, randomSeed))));

	return resultantNoise;
}



float GetRawNoiseIn2DFromMinusOneToOne(int indexX, int indexY, unsigned int randomSeed /*= 0*/)
{
	const double MAXIMUM_INT_INVERSE = (1.0 / static_cast<double>(0x7FFFFFFF));
	float resultantNoise = static_cast<float>(MAXIMUM_INT_INVERSE * static_cast<double>(static_cast<int>(GetRawNoiseIn2D(indexX, indexY, randomSeed))));

	return resultantNoise;
}



float GetRawNoiseIn3DFromMinusOneToOne(int indexX, int indexY, int indexZ, unsigned int randomSeed /*= 0*/)
{
	const double MAXIMUM_INT_INVERSE = (1.0 / static_cast<double>(0x7FFFFFFF));
	float resultantNoise = static_cast<float>(MAXIMUM_INT_INVERSE * static_cast<double>(static_cast<int>(GetRawNoiseIn3D(indexX, indexY, indexZ, randomSeed))));

	return resultantNoise;
}



float GetRawNoiseIn4DFromMinusOneToOne(int indexX, int indexY, int indexZ, int indexW, unsigned int randomSeed /*= 0*/)
{
	const double MAXIMUM_INT_INVERSE = (1.0 / static_cast<double>(0x7FFFFFFF));
	float resultantNoise = static_cast<float>(MAXIMUM_INT_INVERSE * static_cast<double>(static_cast<int>(GetRawNoiseIn4D(indexX, indexY, indexZ, indexW, randomSeed))));

	return resultantNoise;
}



float ComputeFractalNoiseIn1D(float position, float noiseScale /*= 1.0f*/, unsigned int numberOfOctaves /*= 1*/, float octavePersistence /*= 0.5f*/, float octaveScale /*= 2.0f*/, bool renormalizeNoise /*= true*/, unsigned int randomSeed /*= 0*/)
{
	const float OCTAVE_OFFSET = 0.636764989593174f;

	float totalNoise = 0.0f;
	float totalAmplitude = 0.0f;
	float currentAmplitude = 1.0f;
	float currentPosition = position * (1.0f / noiseScale);

	for (unsigned int octaveIndex = 0; octaveIndex < numberOfOctaves; ++octaveIndex)
	{
		float flooredPosition = FastRoundDownToFloorValue(currentPosition);
		int westIndex = static_cast<int>(flooredPosition);
		int eastIndex = westIndex + 1;
		float westValue = GetRawNoiseIn1DFromZeroToOne(westIndex, randomSeed);
		float eastValue = GetRawNoiseIn1DFromZeroToOne(eastIndex, randomSeed);

		float distanceFromWest = currentPosition - flooredPosition;
		float eastWeight = SmoothStep(distanceFromWest);
		float westWeight = 1.0f - eastWeight;
		float noiseFromZeroToOne = (westValue * westWeight) + (eastValue * eastWeight);
		float noiseForCurrentOctave = 2.0f * (noiseFromZeroToOne - 0.5f);

		totalNoise += noiseForCurrentOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition += OCTAVE_OFFSET;
		++randomSeed;
	}

	if (renormalizeNoise && totalAmplitude > 0.0f)
	{
		totalNoise /= totalAmplitude;
		totalNoise = (totalNoise * 0.5f) + 0.5f;
		totalNoise = SmoothStep(totalNoise);
		totalNoise = (totalNoise * 2.0f) - 1.0f;
	}

	return totalNoise;
}



float ComputeFractalNoiseIn2D(Vector2 position, float noiseScale /*= 1.0f*/, unsigned int numberOfOctaves /*= 1*/, float octavePersistence /*= 0.5f*/, float octaveScale /*= 2.0f*/, bool renormalizeNoise /*= true*/, unsigned int randomSeed /*= 0*/)
{
	const float OCTAVE_OFFSET = 0.636764989593174f;

	float totalNoise = 0.0f;
	float totalAmplitude = 0.0f;
	float currentAmplitude = 1.0f;
	float inverseScale = (1.0f / noiseScale);
	Vector2 currentPosition(position.X * inverseScale, position.Y * inverseScale);

	for (unsigned int octaveIndex = 0; octaveIndex < numberOfOctaves; ++octaveIndex)
	{
		Vector2 cellMinimums(FastRoundDownToFloorValue(currentPosition.X), FastRoundDownToFloorValue(currentPosition.Y));
		int westIndex = static_cast<int>(cellMinimums.X);
		int southIndex = static_cast<int>(cellMinimums.Y);
		int eastIndex = westIndex + 1;
		int northIndex = southIndex + 1;
		float southWestValue = GetRawNoiseIn2DFromZeroToOne(westIndex, southIndex, randomSeed);
		float southEastValue = GetRawNoiseIn2DFromZeroToOne(eastIndex, southIndex, randomSeed);
		float northWestValue = GetRawNoiseIn2DFromZeroToOne(westIndex, northIndex, randomSeed);
		float northEastValue = GetRawNoiseIn2DFromZeroToOne(eastIndex, northIndex, randomSeed);

		Vector2 displacementFromMinimums = currentPosition - cellMinimums;
		float eastWeight = SmoothStep(displacementFromMinimums.X);
		float northWeight = SmoothStep(displacementFromMinimums.Y);
		float westWeight = 1.0f - eastWeight;
		float southWeight = 1.0f - northWeight;

		float southBlend = (eastWeight * southEastValue) + (westWeight * southWestValue);
		float northBlend = (eastWeight * northEastValue) + (westWeight * northWestValue);
		float totalBlend = (southWeight * southBlend) + (northWeight * northBlend);
		float noiseForCurrentOctave = 2.0f * (totalBlend - 0.5f);

		totalNoise += noiseForCurrentOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition.X += OCTAVE_OFFSET;
		currentPosition.Y += OCTAVE_OFFSET;
		++randomSeed;
	}

	if (renormalizeNoise && totalAmplitude > 0.0f)
	{
		totalNoise /= totalAmplitude;
		totalNoise = (totalNoise * 0.5f) + 0.5f;
		totalNoise = SmoothStep(totalNoise);
		totalNoise = (totalNoise * 2.0f) - 1.0f;
	}

	return totalNoise;
}



float ComputeFractalNoiseIn3D(Vector3 position, float noiseScale /*= 1.0f*/, unsigned int numberOfOctaves /*= 1*/, float octavePersistence /*= 0.5f*/, float octaveScale /*= 2.0f*/, bool renormalizeNoise /*= true*/, unsigned int randomSeed /*= 0*/)
{
	const float OCTAVE_OFFSET = 0.636764989593174f;

	float totalNoise = 0.0f;
	float totalAmplitude = 0.0f;
	float currentAmplitude = 1.0f;
	float inverseScale = (1.0f / noiseScale);
	Vector3 currentPosition(position.X * inverseScale, position.Y * inverseScale, position.Z * inverseScale);

	for (unsigned int octaveIndex = 0; octaveIndex < numberOfOctaves; ++octaveIndex)
	{
		Vector3 cellMinimums(FastRoundDownToFloorValue(currentPosition.X), FastRoundDownToFloorValue(currentPosition.Y), FastRoundDownToFloorValue(currentPosition.Z));
		int westIndex = static_cast<int>(cellMinimums.X);
		int southIndex = static_cast<int>(cellMinimums.Y);
		int belowIndex = static_cast<int>(cellMinimums.Z);
		int eastIndex = westIndex + 1;
		int northIndex = southIndex + 1;
		int aboveIndex = belowIndex + 1;

		float aboveSouthWestValue = GetRawNoiseIn3DFromZeroToOne(westIndex, southIndex, aboveIndex, randomSeed);
		float aboveSouthEastValue = GetRawNoiseIn3DFromZeroToOne(eastIndex, southIndex, aboveIndex, randomSeed);
		float aboveNorthWestValue = GetRawNoiseIn3DFromZeroToOne(westIndex, northIndex, aboveIndex, randomSeed);
		float aboveNorthEastValue = GetRawNoiseIn3DFromZeroToOne(eastIndex, northIndex, aboveIndex, randomSeed);
		float belowSouthWestValue = GetRawNoiseIn3DFromZeroToOne(westIndex, southIndex, belowIndex, randomSeed);
		float belowSouthEastValue = GetRawNoiseIn3DFromZeroToOne(eastIndex, southIndex, belowIndex, randomSeed);
		float belowNorthWestValue = GetRawNoiseIn3DFromZeroToOne(westIndex, northIndex, belowIndex, randomSeed);
		float belowNorthEastValue = GetRawNoiseIn3DFromZeroToOne(eastIndex, northIndex, belowIndex, randomSeed);

		Vector3 displacementFromMinimums = currentPosition - cellMinimums;

		float eastWeight = SmoothStep(displacementFromMinimums.X);
		float northWeight = SmoothStep(displacementFromMinimums.Y);
		float aboveWeight = SmoothStep(displacementFromMinimums.Z);
		float westWeight = 1.0f - eastWeight;
		float southWeight = 1.0f - northWeight;
		float belowWeight = 1.0f - aboveWeight;

		float belowSouthBlend = (eastWeight * belowSouthEastValue) + (westWeight * belowSouthWestValue);
		float belowNorthBlend = (eastWeight * belowNorthEastValue) + (westWeight * belowNorthWestValue);
		float aboveSouthBlend = (eastWeight * aboveSouthEastValue) + (westWeight * aboveSouthWestValue);
		float aboveNorthBlend = (eastWeight * aboveNorthEastValue) + (westWeight * aboveNorthWestValue);
		float belowBlend = (southWeight * belowSouthBlend) + (northWeight * belowNorthBlend);
		float aboveBlend = (southWeight * aboveSouthBlend) + (northWeight * aboveNorthBlend);
		float totalBlend = (belowWeight * belowBlend) + (aboveWeight * aboveBlend);
		float noiseForCurrentOctave = 2.0f * (totalBlend - 0.5f);

		totalNoise += noiseForCurrentOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition.X += OCTAVE_OFFSET;
		currentPosition.Y += OCTAVE_OFFSET;
		currentPosition.Z += OCTAVE_OFFSET;
		++randomSeed;
	}

	if (renormalizeNoise && totalAmplitude > 0.0f)
	{
		totalNoise /= totalAmplitude;
		totalNoise = (totalNoise * 0.5f) + 0.5f;
		totalNoise = SmoothStep(totalNoise);
		totalNoise = (totalNoise * 2.0f) - 1.0f;
	}

	return totalNoise;
}



float ComputeFractalNoiseIn4D(Vector4 position, float noiseScale /*= 1.0f*/, unsigned int numberOfOctaves /*= 1*/, float octavePersistence /*= 0.5f*/, float octaveScale /*= 2.0f*/, bool renormalizeNoise /*= true*/, unsigned int randomSeed /*= 0*/)
{
	const float OCTAVE_OFFSET = 0.636764989593174f;

	float totalNoise = 0.0f;
	float totalAmplitude = 0.0f;
	float currentAmplitude = 1.0f;
	float inverseScale = (1.0f / noiseScale);
	Vector4 currentPosition(position.X * inverseScale, position.Y * inverseScale, position.Z * inverseScale, position.T * inverseScale);

	for (unsigned int octaveIndex = 0; octaveIndex < numberOfOctaves; ++octaveIndex)
	{
		Vector4 cellMinimums(FastRoundDownToFloorValue(currentPosition.X), FastRoundDownToFloorValue(currentPosition.Y), FastRoundDownToFloorValue(currentPosition.Z), FastRoundDownToFloorValue(currentPosition.T));
		int westIndex = static_cast<int>(cellMinimums.X);
		int southIndex = static_cast<int>(cellMinimums.Y);
		int belowIndex = static_cast<int>(cellMinimums.Z);
		int beforeIndex = static_cast<int>(cellMinimums.T);
		int eastIndex = westIndex + 1;
		int northIndex = southIndex + 1;
		int aboveIndex = belowIndex + 1;
		int afterIndex = beforeIndex + 1;

		float beforeBelowSouthWestValue = GetRawNoiseIn4DFromZeroToOne(westIndex, southIndex, belowIndex, beforeIndex, randomSeed);
		float beforeBelowSouthEastValue = GetRawNoiseIn4DFromZeroToOne(eastIndex, southIndex, belowIndex, beforeIndex, randomSeed);
		float beforeBelowNorthWestValue = GetRawNoiseIn4DFromZeroToOne(westIndex, northIndex, belowIndex, beforeIndex, randomSeed);
		float beforeBelowNorthEastValue = GetRawNoiseIn4DFromZeroToOne(eastIndex, northIndex, belowIndex, beforeIndex, randomSeed);
		float beforeAboveSouthWestValue = GetRawNoiseIn4DFromZeroToOne(westIndex, southIndex, aboveIndex, beforeIndex, randomSeed);
		float beforeAboveSouthEastValue = GetRawNoiseIn4DFromZeroToOne(eastIndex, southIndex, aboveIndex, beforeIndex, randomSeed);
		float beforeAboveNorthWestValue = GetRawNoiseIn4DFromZeroToOne(westIndex, northIndex, aboveIndex, beforeIndex, randomSeed);
		float beforeAboveNorthEastValue = GetRawNoiseIn4DFromZeroToOne(eastIndex, northIndex, aboveIndex, beforeIndex, randomSeed);

		float afterBelowSouthWestValue = GetRawNoiseIn4DFromZeroToOne(westIndex, southIndex, belowIndex, afterIndex, randomSeed);
		float afterBelowSouthEastValue = GetRawNoiseIn4DFromZeroToOne(eastIndex, southIndex, belowIndex, afterIndex, randomSeed);
		float afterBelowNorthWestValue = GetRawNoiseIn4DFromZeroToOne(westIndex, northIndex, belowIndex, afterIndex, randomSeed);
		float afterBelowNorthEastValue = GetRawNoiseIn4DFromZeroToOne(eastIndex, northIndex, belowIndex, afterIndex, randomSeed);
		float afterAboveSouthWestValue = GetRawNoiseIn4DFromZeroToOne(westIndex, southIndex, aboveIndex, afterIndex, randomSeed);
		float afterAboveSouthEastValue = GetRawNoiseIn4DFromZeroToOne(eastIndex, southIndex, aboveIndex, afterIndex, randomSeed);
		float afterAboveNorthWestValue = GetRawNoiseIn4DFromZeroToOne(westIndex, northIndex, aboveIndex, afterIndex, randomSeed);
		float afterAboveNorthEastValue = GetRawNoiseIn4DFromZeroToOne(eastIndex, northIndex, aboveIndex, afterIndex, randomSeed);

		Vector4 displacementFromMinimums = currentPosition - cellMinimums;

		float eastWeight = SmoothStep(displacementFromMinimums.X);
		float northWeight = SmoothStep(displacementFromMinimums.Y);
		float aboveWeight = SmoothStep(displacementFromMinimums.Z);
		float afterWeight = SmoothStep(displacementFromMinimums.T);
		float westWeight = 1.0f - eastWeight;
		float southWeight = 1.0f - northWeight;
		float belowWeight = 1.0f - aboveWeight;
		float beforeWeight = 1.0f - afterWeight;

		float beforeBelowSouthBlend = (eastWeight * beforeBelowSouthEastValue) + (westWeight * beforeBelowSouthWestValue);
		float beforeBelowNorthBlend = (eastWeight * beforeBelowNorthEastValue) + (westWeight * beforeBelowNorthWestValue);
		float beforeAboveSouthBlend = (eastWeight * beforeAboveSouthEastValue) + (westWeight * beforeAboveSouthWestValue);
		float beforeAboveNorthBlend = (eastWeight * beforeAboveNorthEastValue) + (westWeight * beforeAboveNorthWestValue);
		float afterBelowSouthBlend = (eastWeight * afterBelowSouthEastValue) + (westWeight * afterBelowSouthWestValue);
		float afterBelowNorthBlend = (eastWeight * afterBelowNorthEastValue) + (westWeight * afterBelowNorthWestValue);
		float afterAboveSouthBlend = (eastWeight * afterAboveSouthEastValue) + (westWeight * afterAboveSouthWestValue);
		float afterAboveNorthBlend = (eastWeight * afterAboveNorthEastValue) + (westWeight * afterAboveNorthWestValue);
		float beforeBelowBlend = (southWeight * beforeBelowSouthBlend) + (northWeight * beforeBelowNorthBlend);
		float beforeAboveBlend = (southWeight * beforeAboveSouthBlend) + (northWeight * beforeAboveNorthBlend);
		float afterBelowBlend = (southWeight * afterBelowSouthBlend) + (northWeight * afterBelowNorthBlend);
		float afterAboveBlend = (southWeight * afterAboveSouthBlend) + (northWeight * afterAboveNorthBlend);
		float beforeBlend = (belowWeight * beforeBelowBlend) + (aboveWeight * beforeAboveBlend);
		float afterBlend = (belowWeight * afterBelowBlend) + (aboveWeight * afterAboveBlend);
		float totalBlend = (beforeWeight * beforeBlend) + (afterWeight * afterBlend);
		float noiseForCurrentOctave = 2.0f * (totalBlend - 0.5f);

		totalNoise += noiseForCurrentOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition.X += OCTAVE_OFFSET;
		currentPosition.Y += OCTAVE_OFFSET;
		currentPosition.Z += OCTAVE_OFFSET;
		currentPosition.T += OCTAVE_OFFSET;
		++randomSeed;
	}

	if (renormalizeNoise && totalAmplitude > 0.0f)
	{
		totalNoise /= totalAmplitude;
		totalNoise = (totalNoise * 0.5f) + 0.5f;
		totalNoise = SmoothStep(totalNoise);
		totalNoise = (totalNoise * 2.0f) - 1.0f;
	}

	return totalNoise;
}



float ComputePerlinNoiseIn1D(float position, float noiseScale /*= 1.0f*/, unsigned int numberOfOctaves /*= 1*/, float octavePersistence /*= 0.5f*/, float octaveScale /*= 2.0f*/, bool renormalizeNoise /*= true*/, unsigned int randomSeed /*= 0*/)
{
	const float OCTAVE_OFFSET = 0.636764989593174f;
	const float gradientVectors[2] = { -1.0f, 1.0f };

	float totalNoise = 0.0f;
	float totalAmplitude = 0.0f;
	float currentAmplitude = 1.0f;
	float currentPosition = position * (1.0f / noiseScale);

	for (unsigned int octaveIndex = 0; octaveIndex < numberOfOctaves; ++octaveIndex)
	{
		float flooredPosition = FastRoundDownToFloorValue(currentPosition);
		int westIndex = static_cast<int>(flooredPosition);
		int eastIndex = westIndex + 1;
		float westGradient = gradientVectors[GetRawNoiseIn1D(westIndex, randomSeed) & 0x00000001];
		float eastGradient = gradientVectors[GetRawNoiseIn1D(eastIndex, randomSeed) & 0x00000001];

		float distanceFromWest = currentPosition - flooredPosition;
		float distanceFromEast = distanceFromWest - 1.0f;
		float westDotProduct = westGradient * distanceFromWest;
		float eastDotProduct = eastGradient * distanceFromEast;

		float eastWeight = SmoothStep(distanceFromWest);
		float westWeight = 1.0f - eastWeight;
		float totalBlend = (westWeight * westDotProduct) + (eastWeight * eastDotProduct);
		float noiseForCurrentOctave = 2.0f * totalBlend;

		totalNoise += noiseForCurrentOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition += OCTAVE_OFFSET;
		++randomSeed;
	}

	if (renormalizeNoise && totalAmplitude > 0.0f)
	{
		totalNoise /= totalAmplitude;
		totalNoise = (totalNoise * 0.5f) + 0.5f;
		totalNoise = SmoothStep(totalNoise);
		totalNoise = (totalNoise * 2.0f) - 1.0f;
	}

	return totalNoise;
}



float ComputePerlinNoiseIn2D(Vector2 position, float noiseScale /*= 1.0f*/, unsigned int numberOfOctaves /*= 1*/, float octavePersistence /*= 0.5f*/, float octaveScale /*= 2.0f*/, bool renormalizeNoise /*= true*/, unsigned int randomSeed /*= 0*/)
{
	const float OCTAVE_OFFSET = 0.636764989593174f;
	const Vector2 gradientVectors[8] =
	{
		Vector2(+0.923879533f, +0.382683432f),
		Vector2(+0.382683432f, +0.923879533f),
		Vector2(-0.382683432f, +0.923879533f),
		Vector2(-0.923879533f, +0.382683432f),
		Vector2(-0.923879533f, -0.382683432f),
		Vector2(-0.382683432f, -0.923879533f),
		Vector2(+0.382683432f, -0.923879533f),
		Vector2(+0.923879533f, -0.382683432f)
	};

	float totalNoise = 0.0f;
	float totalAmplitude = 0.0f;
	float currentAmplitude = 1.0f;
	float inverseScale = (1.0f / noiseScale);
	Vector2 currentPosition(position.X * inverseScale, position.Y * inverseScale);

	for (unsigned int octaveIndex = 0; octaveIndex < numberOfOctaves; ++octaveIndex)
	{
		Vector2 cellMinimums(FastRoundDownToFloorValue(currentPosition.X), FastRoundDownToFloorValue(currentPosition.Y));
		Vector2 cellMaximums(cellMinimums.X + 1.0f, cellMinimums.Y + 1.0f);
		int westIndex = static_cast<int>(cellMinimums.X);
		int southIndex = static_cast<int>(cellMinimums.Y);
		int eastIndex = westIndex + 1;
		int northIndex = southIndex + 1;

		unsigned int southWestNoise = GetRawNoiseIn2D(westIndex, southIndex, randomSeed);
		unsigned int southEastNoise = GetRawNoiseIn2D(eastIndex, southIndex, randomSeed);
		unsigned int northWestNoise = GetRawNoiseIn2D(westIndex, northIndex, randomSeed);
		unsigned int northEastNoise = GetRawNoiseIn2D(eastIndex, northIndex, randomSeed);

		Vector2 southWestGradient = gradientVectors[southWestNoise & 0x00000007];
		Vector2 southEastGradient = gradientVectors[southEastNoise & 0x00000007];
		Vector2 northWestGradient = gradientVectors[northWestNoise & 0x00000007];
		Vector2 northEastGradient = gradientVectors[northEastNoise & 0x00000007];

		Vector2 distanceFromSouthWest(currentPosition.X - cellMinimums.X, currentPosition.Y - cellMinimums.Y);
		Vector2 distanceFromSouthEast(currentPosition.X - cellMaximums.X, currentPosition.Y - cellMinimums.Y);
		Vector2 distanceFromNorthWest(currentPosition.X - cellMinimums.X, currentPosition.Y - cellMaximums.Y);
		Vector2 distanceFromNorthEast(currentPosition.X - cellMaximums.X, currentPosition.Y - cellMaximums.Y);

		float southWestDotProduct = Vector2::DotProduct(southWestGradient, distanceFromSouthWest);
		float southEastDotProduct = Vector2::DotProduct(southEastGradient, distanceFromSouthEast);
		float northWestDotProduct = Vector2::DotProduct(northWestGradient, distanceFromNorthWest);
		float northEastDotProduct = Vector2::DotProduct(northEastGradient, distanceFromNorthEast);

		float eastWeight = FifthPowerSmoothStep(distanceFromSouthWest.X);
		float northWeight = FifthPowerSmoothStep(distanceFromSouthWest.Y);
		float westWeight = 1.0f - eastWeight;
		float southWeight = 1.0f - northWeight;

		float southBlend = (eastWeight * southEastDotProduct) + (westWeight * southWestDotProduct);
		float northBlend = (eastWeight * northEastDotProduct) + (westWeight * northWestDotProduct);
		float totalBlend = (southWeight * southBlend) + (northWeight * northBlend);
		float noiseForCurrentOctave = 1.5f * totalBlend;

		totalNoise += noiseForCurrentOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition.X += OCTAVE_OFFSET;
		currentPosition.Y += OCTAVE_OFFSET;
		++randomSeed;
	}

	if (renormalizeNoise && totalAmplitude > 0.0f)
	{
		totalNoise /= totalAmplitude;
		totalNoise = (totalNoise * 0.5f) + 0.5f;
		totalNoise = SmoothStep(totalNoise);
		totalNoise = (totalNoise * 2.0f) - 1.0f;
	}

	return totalNoise;
}



float ComputePerlinNoiseIn3D(Vector3 position, float noiseScale /*= 1.0f*/, unsigned int numberOfOctaves /*= 1*/, float octavePersistence /*= 0.5f*/, float octaveScale /*= 2.0f*/, bool renormalizeNoise /*= true*/, unsigned int randomSeed /*= 0*/)
{
	const float OCTAVE_OFFSET = 0.636764989593174f;
	const Vector3 gradientVectors[8] =
	{
		Vector3(+F_SQUARE_ROOT_OF_3_BY_3, +F_SQUARE_ROOT_OF_3_BY_3, +F_SQUARE_ROOT_OF_3_BY_3),
		Vector3(-F_SQUARE_ROOT_OF_3_BY_3, +F_SQUARE_ROOT_OF_3_BY_3, +F_SQUARE_ROOT_OF_3_BY_3),
		Vector3(+F_SQUARE_ROOT_OF_3_BY_3, -F_SQUARE_ROOT_OF_3_BY_3, +F_SQUARE_ROOT_OF_3_BY_3),
		Vector3(-F_SQUARE_ROOT_OF_3_BY_3, -F_SQUARE_ROOT_OF_3_BY_3, +F_SQUARE_ROOT_OF_3_BY_3),
		Vector3(+F_SQUARE_ROOT_OF_3_BY_3, +F_SQUARE_ROOT_OF_3_BY_3, -F_SQUARE_ROOT_OF_3_BY_3),
		Vector3(-F_SQUARE_ROOT_OF_3_BY_3, +F_SQUARE_ROOT_OF_3_BY_3, -F_SQUARE_ROOT_OF_3_BY_3),
		Vector3(+F_SQUARE_ROOT_OF_3_BY_3, -F_SQUARE_ROOT_OF_3_BY_3, -F_SQUARE_ROOT_OF_3_BY_3),
		Vector3(-F_SQUARE_ROOT_OF_3_BY_3, -F_SQUARE_ROOT_OF_3_BY_3, -F_SQUARE_ROOT_OF_3_BY_3)
	};

	float totalNoise = 0.0f;
	float totalAmplitude = 0.0f;
	float currentAmplitude = 1.0f;
	float inverseScale = (1.0f / noiseScale);
	Vector3 currentPosition(position.X * inverseScale, position.Y * inverseScale, position.Z * inverseScale);

	for (unsigned int octaveIndex = 0; octaveIndex < numberOfOctaves; ++octaveIndex)
	{
		Vector3 cellMinimums(FastRoundDownToFloorValue(currentPosition.X), FastRoundDownToFloorValue(currentPosition.Y), FastRoundDownToFloorValue(currentPosition.Z));
		Vector3 cellMaximums(cellMinimums.X + 1.0f, cellMinimums.Y + 1.0f, cellMinimums.Z + 1.0f);
		int westIndex = static_cast<int>(cellMinimums.X);
		int southIndex = static_cast<int>(cellMinimums.Y);
		int belowIndex = static_cast<int>(cellMinimums.Z);
		int eastIndex = westIndex + 1;
		int northIndex = southIndex + 1;
		int aboveIndex = belowIndex + 1;

		unsigned int belowSouthWestNoise = GetRawNoiseIn3D(westIndex, southIndex, belowIndex, randomSeed);
		unsigned int belowSouthEastNoise = GetRawNoiseIn3D(eastIndex, southIndex, belowIndex, randomSeed);
		unsigned int belowNorthWestNoise = GetRawNoiseIn3D(westIndex, northIndex, belowIndex, randomSeed);
		unsigned int belowNorthEastNoise = GetRawNoiseIn3D(eastIndex, northIndex, belowIndex, randomSeed);
		unsigned int aboveSouthWestNoise = GetRawNoiseIn3D(westIndex, southIndex, aboveIndex, randomSeed);
		unsigned int aboveSouthEastNoise = GetRawNoiseIn3D(eastIndex, southIndex, aboveIndex, randomSeed);
		unsigned int aboveNorthWestNoise = GetRawNoiseIn3D(westIndex, northIndex, aboveIndex, randomSeed);
		unsigned int aboveNorthEastNoise = GetRawNoiseIn3D(eastIndex, northIndex, aboveIndex, randomSeed);

		Vector3 belowSouthWestGradient = gradientVectors[belowSouthWestNoise & 0x00000007];
		Vector3 belowSouthEastGradient = gradientVectors[belowSouthEastNoise & 0x00000007];
		Vector3 belowNorthWestGradient = gradientVectors[belowNorthWestNoise & 0x00000007];
		Vector3 belowNorthEastGradient = gradientVectors[belowNorthEastNoise & 0x00000007];
		Vector3 aboveSouthWestGradient = gradientVectors[aboveSouthWestNoise & 0x00000007];
		Vector3 aboveSouthEastGradient = gradientVectors[aboveSouthEastNoise & 0x00000007];
		Vector3 aboveNorthWestGradient = gradientVectors[aboveNorthWestNoise & 0x00000007];
		Vector3 aboveNorthEastGradient = gradientVectors[aboveNorthEastNoise & 0x00000007];

		Vector3 distanceFromBelowSouthWest(currentPosition.X - cellMinimums.X, currentPosition.Y - cellMinimums.Y, currentPosition.Z - cellMinimums.Z);
		Vector3 distanceFromBelowSouthEast(currentPosition.X - cellMaximums.X, currentPosition.Y - cellMinimums.Y, currentPosition.Z - cellMinimums.Z);
		Vector3 distanceFromBelowNorthWest(currentPosition.X - cellMinimums.X, currentPosition.Y - cellMaximums.Y, currentPosition.Z - cellMinimums.Z);
		Vector3 distanceFromBelowNorthEast(currentPosition.X - cellMaximums.X, currentPosition.Y - cellMaximums.Y, currentPosition.Z - cellMinimums.Z);
		Vector3 distanceFromAboveSouthWest(currentPosition.X - cellMinimums.X, currentPosition.Y - cellMinimums.Y, currentPosition.Z - cellMaximums.Z);
		Vector3 distanceFromAboveSouthEast(currentPosition.X - cellMaximums.X, currentPosition.Y - cellMinimums.Y, currentPosition.Z - cellMaximums.Z);
		Vector3 distanceFromAboveNorthWest(currentPosition.X - cellMinimums.X, currentPosition.Y - cellMaximums.Y, currentPosition.Z - cellMaximums.Z);
		Vector3 distanceFromAboveNorthEast(currentPosition.X - cellMaximums.X, currentPosition.Y - cellMaximums.Y, currentPosition.Z - cellMaximums.Z);

		float belowSouthWestDotProduct = Vector3::DotProduct(belowSouthWestGradient, distanceFromBelowSouthWest);
		float belowSouthEastDotProduct = Vector3::DotProduct(belowSouthEastGradient, distanceFromBelowSouthEast);
		float belowNorthWestDotProduct = Vector3::DotProduct(belowNorthWestGradient, distanceFromBelowNorthWest);
		float belowNorthEastDotProduct = Vector3::DotProduct(belowNorthEastGradient, distanceFromBelowNorthEast);
		float aboveSouthWestDotProduct = Vector3::DotProduct(aboveSouthWestGradient, distanceFromAboveSouthWest);
		float aboveSouthEastDotProduct = Vector3::DotProduct(aboveSouthEastGradient, distanceFromAboveSouthEast);
		float aboveNorthWestDotProduct = Vector3::DotProduct(aboveNorthWestGradient, distanceFromAboveNorthWest);
		float aboveNorthEastDotProduct = Vector3::DotProduct(aboveNorthEastGradient, distanceFromAboveNorthEast);

		float eastWeight = FifthPowerSmoothStep(distanceFromBelowSouthWest.X);
		float northWeight = FifthPowerSmoothStep(distanceFromBelowSouthWest.Y);
		float aboveWeight = FifthPowerSmoothStep(distanceFromBelowSouthWest.Z);
		float westWeight = 1.0f - eastWeight;
		float southWeight = 1.0f - northWeight;
		float belowWeight = 1.0f - aboveWeight;

		float belowSouthBlend = (eastWeight * belowSouthEastDotProduct) + (westWeight * belowSouthWestDotProduct);
		float belowNorthBlend = (eastWeight * belowNorthEastDotProduct) + (westWeight * belowNorthWestDotProduct);
		float aboveSouthBlend = (eastWeight * aboveSouthEastDotProduct) + (westWeight * aboveSouthWestDotProduct);
		float aboveNorthBlend = (eastWeight * aboveNorthEastDotProduct) + (westWeight * aboveNorthWestDotProduct);
		float belowBlend = (southWeight * belowSouthBlend) + (northWeight * belowNorthBlend);
		float aboveBlend = (southWeight * aboveSouthBlend) + (northWeight * aboveNorthBlend);
		float totalBlend = (belowWeight * belowBlend) + (aboveWeight * aboveBlend);
		float noiseForCurrentOctave = 1.66666666f * totalBlend;

		totalNoise += noiseForCurrentOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition.X += OCTAVE_OFFSET;
		currentPosition.Y += OCTAVE_OFFSET;
		currentPosition.Z += OCTAVE_OFFSET;
		++randomSeed;
	}

	if (renormalizeNoise && totalAmplitude > 0.0f)
	{
		totalNoise /= totalAmplitude;
		totalNoise = (totalNoise * 0.5f) + 0.5f;
		totalNoise = SmoothStep(totalNoise);
		totalNoise = (totalNoise * 2.0f) - 1.0f;
	}

	return totalNoise;
}
// 
// 
// 
// float ComputePerlinNoiseIn4D(Vector4 position, float noiseScale /*= 1.0f*/, unsigned int numberOfOctaves /*= 1*/, float octavePersistence /*= 0.5f*/, float octaveScale /*= 2.0f*/, bool renormalizeNoise /*= true*/, unsigned int randomSeed /*= 0*/)
// {
// 
// }