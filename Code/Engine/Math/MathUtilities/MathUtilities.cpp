#include <math.h>
#include <stdlib.h>
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



float GetMinimumOfTwoFloats(float first, float second)
{
	if (first < second)
	{
		return first;
	}
	else
	{
		return second;
	}
}



int GetMinimumOfTwoInts(int first, int second)
{
	if (first < second)
	{
		return first;
	}
	else
	{
		return second;
	}
}



size_t GetMinimumOfTwoSize_T(size_t first, size_t second)
{
	if (first < second)
	{
		return first;
	}
	else
	{
		return second;
	}
}



float GetMaximumOfTwoFloats(float first, float second)
{
	if (first > second)
	{
		return first;
	}
	else
	{
		return second;
	}
}



int GetMaximumOfTwoInts(int first, int second)
{
	if (first > second)
	{
		return first;
	}
	else
	{
		return second;
	}
}



size_t GetMaximumOfTwoSize_T(size_t first, size_t second)
{
	if (first > second)
	{
		return first;
	}
	else
	{
		return second;
	}
}



float GetMinimumOfThreeFloats(float first, float second, float third)
{
	if (first < second && first < third)
	{
		return first;
	}
	else if (second < first && second < third)
	{
		return second;
	}
	else
	{
		return third;
	}
}



int GetMinimumOfThreeInts(int first, int second, int third)
{
	if (first < second && first < third)
	{
		return first;
	}
	else if (second < first && second < third)
	{
		return second;
	}
	else
	{
		return third;
	}
}



float GetMaximumOfThreeFloats(float first, float second, float third)
{
	if (first > second && first > third)
	{
		return first;
	}
	else if (second > first && second > third)
	{
		return second;
	}
	else
	{
		return third;
	}
}



int GetMaximumOfThreeInts(int first, int second, int third)
{
	if (first > second && first > third)
	{
		return first;
	}
	else if (second > first && second > third)
	{
		return second;
	}
	else
	{
		return third;
	}
}



float GetMinimumOfFourFloats(float first, float second, float third, float fourth)
{
	if (first < second && first < third && first < fourth)
	{
		return first;
	}
	else if (second < first && second < third && second < fourth)
	{
		return second;
	}
	else if (third < first && third < second && third < fourth)
	{
		return third;
	}
	else
	{
		return fourth;
	}
}



int GetMinimumOfFourInts(int first, int second, int third, int fourth)
{
	if (first < second && first < third && first < fourth)
	{
		return first;
	}
	else if (second < first && second < third && second < fourth)
	{
		return second;
	}
	else if (third < first && third < second && third < fourth)
	{
		return third;
	}
	else
	{
		return fourth;
	}
}



float GetMaximumOfFourFloats(float first, float second, float third, float fourth)
{
	if (first > second && first > third && first > fourth)
	{
		return first;
	}
	else if (second > first && second > third && second > fourth)
	{
		return second;
	}
	else if (third > first && third > second && third > fourth)
	{
		return third;
	}
	else
	{
		return fourth;
	}
}



int GetMaximumOfFourInts(int first, int second, int third, int fourth)
{
	if (first > second && first > third && first > fourth)
	{
		return first;
	}
	else if (second > first && second > third && second > fourth)
	{
		return second;
	}
	else if (third > first && third > second && third > fourth)
	{
		return third;
	}
	else
	{
		return fourth;
	}
}



float GetMaximumOfFloatArray(float* floatArray, size_t arraySize)
{
	float maximumValue = floatArray[0];
	for (size_t arrayIndex = 0; arrayIndex < arraySize; ++arrayIndex)
	{
		if (floatArray[arrayIndex] > maximumValue)
		{
			maximumValue = floatArray[arrayIndex];
		}
	}

	return maximumValue;
}



int GetMaximumOfIntArray(int* intArray, size_t arraySize)
{
	int maximumValue = intArray[0];
	for (size_t arrayIndex = 0; arrayIndex < arraySize; ++arrayIndex)
	{
		if (intArray[arrayIndex] > maximumValue)
		{
			maximumValue = intArray[arrayIndex];
		}
	}

	return maximumValue;
}



float GetMinimumOfFloatArray(float* floatArray, size_t arraySize)
{
	float minimumValue = floatArray[0];
	for (size_t arrayIndex = 0; arrayIndex < arraySize; ++arrayIndex)
	{
		if (floatArray[arrayIndex] < minimumValue)
		{
			minimumValue = floatArray[arrayIndex];
		}
	}

	return minimumValue;
}



int GetMinimumOfIntArray(int* intArray, size_t arraySize)
{
	int minimumValue = intArray[0];
	for (size_t arrayIndex = 0; arrayIndex < arraySize; ++arrayIndex)
	{
		if (intArray[arrayIndex] < minimumValue)
		{
			minimumValue = intArray[arrayIndex];
		}
	}

	return minimumValue;
}



float GetRandomFloatWithinRange(float rangeMinimum, float rangeMaximum)
{
	float randomValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float sizeOfRange = rangeMaximum - rangeMinimum;
	float scaledRangeValue = randomValue * sizeOfRange;

	return (rangeMinimum + scaledRangeValue);
}



double GetRandomDoubleWithinRange(double rangeMinimum, double rangeMaximum)
{
	double randomValue = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
	double sizeOfRange = rangeMaximum - rangeMinimum;
	double scaledRangeValue = randomValue * sizeOfRange;

	return (rangeMinimum + scaledRangeValue);
}



int GetRandomIntWithinRange(int rangeMinimum, int rangeMaximum)
{
	int randomValue = rand() % (rangeMaximum - rangeMinimum + 1) + rangeMinimum;

	return randomValue;
}



Vector2 GetRandomVector2WithinRange(const Vector2& rangeMinimum, const Vector2& rangeMaximum)
{
	float randomXValue = GetRandomFloatWithinRange(rangeMinimum.X, rangeMaximum.X);
	float randomYValue = GetRandomFloatWithinRange(rangeMinimum.Y, rangeMaximum.Y);

	return Vector2(randomXValue, randomYValue);
}



IntVector2 GetRandomIntVector2WithinRange(const IntVector2& rangeMinimum, const IntVector2& rangeMaximum)
{
	int randomXValue = GetRandomIntWithinRange(rangeMinimum.X, rangeMaximum.X);
	int randomYValue = GetRandomIntWithinRange(rangeMinimum.Y, rangeMaximum.Y);

	return IntVector2(randomXValue, randomYValue);
}



Vector3 GetRandomVector3WithinRange(const Vector3& rangeMinimum, const Vector3& rangeMaximum)
{
	float randomXValue = GetRandomFloatWithinRange(rangeMinimum.X, rangeMaximum.X);
	float randomYValue = GetRandomFloatWithinRange(rangeMinimum.Y, rangeMaximum.Y);
	float randomZValue = GetRandomFloatWithinRange(rangeMinimum.Z, rangeMaximum.Z);

	return Vector3(randomXValue, randomYValue, randomZValue);
}



IntVector3 GetRandomIntVector3WithinRange(const IntVector3& rangeMinimum, const IntVector3& rangeMaximum)
{
	int randomXValue = GetRandomIntWithinRange(rangeMinimum.X, rangeMaximum.X);
	int randomYValue = GetRandomIntWithinRange(rangeMinimum.Y, rangeMaximum.Y);
	int randomZValue = GetRandomIntWithinRange(rangeMinimum.Z, rangeMaximum.Z);

	return IntVector3(randomXValue, randomYValue, randomZValue);
}



float ConvertToRadians(float angleInDegrees)
{
	float angleInRadians = (angleInDegrees / 180.0f) * F_PI_VALUE;

	return angleInRadians;
}



float ConvertToDegrees(float angleInRadians)
{
	float angleInDegrees = (angleInRadians / F_PI_VALUE) * 180.0f;

	return angleInDegrees;
}



Vector2 ConvertToPolar(const Vector2& cartesianCoordinates)
{
	float squaredX = cartesianCoordinates.X * cartesianCoordinates.X;
	float squaredY = cartesianCoordinates.Y * cartesianCoordinates.Y;

	float radius = SquareRoot(squaredX + squaredY);
	float theta = ArcTangentInRadians(cartesianCoordinates);

	return Vector2(radius, theta);
}



Vector2 ConvertToCartesian(const Vector2& polarCoordinates)
{
	float X = polarCoordinates.X * CosineOfRadians(polarCoordinates.Y);
	float Y = polarCoordinates.X * SineOfRadians(polarCoordinates.Y);

	return Vector2(X, Y);
}



float CalculateEuclidianSquaredDistanceIn2D(const Vector2& startingPoint, const Vector2& endingPoint)
{
	float displacementX = startingPoint.X - endingPoint.X;
	float displacementY = startingPoint.Y - endingPoint.Y;
	float euclidianSquaredDistance2D = (displacementX * displacementX) + (displacementY * displacementY);

	return euclidianSquaredDistance2D;
}



float CalculateEuclidianDistanceIn2D(const Vector2& startingPoint, const Vector2& endingPoint)
{
	float euclidianSquaredDistance2D = CalculateEuclidianSquaredDistanceIn2D(startingPoint, endingPoint);

	return SquareRoot(euclidianSquaredDistance2D);
}



float CalculateManhattanDistanceIn2D(const Vector2& startingPoint, const Vector2& endingPoint)
{
	float displacementX = startingPoint.X - endingPoint.X;
	float displacementY = startingPoint.Y - endingPoint.Y;
	float manhattanDistance2D = AbsoluteValue(displacementX) + AbsoluteValue(displacementY);

	return manhattanDistance2D;
}



float CalculateEuclidianSquaredDistanceIn3D(const Vector3& startingPoint, const Vector3& endingPoint)
{
	float displacementX = startingPoint.X - endingPoint.X;
	float displacementY = startingPoint.Y - endingPoint.Y;
	float displacementZ = startingPoint.Z - endingPoint.Z;
	float euclidianSquaredDistance3D = (displacementX * displacementX) + (displacementY * displacementY) + (displacementZ * displacementZ);

	return euclidianSquaredDistance3D;
}



float CalculateEuclidianDistanceIn3D(const Vector3& startingPoint, const Vector3& endingPoint)
{
	float euclidianSquaredDistance3D = CalculateEuclidianSquaredDistanceIn3D(startingPoint, endingPoint);

	return SquareRoot(euclidianSquaredDistance3D);
}



float CalculateManhattanDistanceIn3D(const Vector3& startingPoint, const Vector3& endingPoint)
{
	float displacementX = startingPoint.X - endingPoint.X;
	float displacementY = startingPoint.Y - endingPoint.Y;
	float displacementZ = startingPoint.Z - endingPoint.Z;
	float manhattanDistance3D = AbsoluteValue(displacementX) + AbsoluteValue(displacementY) + AbsoluteValue(displacementZ);

	return manhattanDistance3D;
}



Vector2 CalculateCentroidOfTriangleIn2D(const Vector2& firstPoint, const Vector2& secondPoint, const Vector2& thirdPoint)
{
	float centroidX = (firstPoint.X + secondPoint.X + thirdPoint.X) / 3.0f;
	float centroidY = (firstPoint.Y + secondPoint.Y + thirdPoint.Y) / 3.0f;

	return Vector2(centroidX, centroidY);
}



Vector3 CalculateCentroidOfTriangleIn3D(const Vector3& firstPoint, const Vector3& secondPoint, const Vector3& thirdPoint)
{
	float centroidX = (firstPoint.X + secondPoint.X + thirdPoint.X) / 3.0f;
	float centroidY = (firstPoint.Y + secondPoint.Y + thirdPoint.Y) / 3.0f;
	float centroidZ = (firstPoint.Z + secondPoint.Z + thirdPoint.Z) / 3.0f;

	return Vector3(centroidX, centroidY, centroidZ);
}



bool PointIsInsideCircle(const Vector2& foreignPoint, const Vector2& circleCenter, float circleRadius)
{
	float distance = CalculateEuclidianDistanceIn2D(foreignPoint, circleCenter);

	return distance <= circleRadius;
}



bool DoCirclesIntersect(const Vector2& firstCircleCenter, float firstCircleRadius, const Vector2& secondCircleCenter, float secondCircleRadius)
{
	float squaredDistanceBetweenCircles = CalculateEuclidianSquaredDistanceIn2D(firstCircleCenter, secondCircleCenter);
	float combinedRadii = firstCircleRadius + secondCircleRadius;

	return squaredDistanceBetweenCircles < (combinedRadii * combinedRadii);
}



float CalculateShortestAngularDisplacement(float startingAngleInDegrees, float endingAngleInDegrees)
{
	float angularDisplacement = endingAngleInDegrees - startingAngleInDegrees;
	while (angularDisplacement > 180.0f)
	{
		angularDisplacement -= 360.0f;
	}
	while (angularDisplacement < -180.0f)
	{
		angularDisplacement += 360.0f;
	}

	return angularDisplacement;
}



bool DoAABB2sIntersect(const AABB2& firstAABB2, const AABB2& secondAABB2)
{
	if (firstAABB2.maximums.X < secondAABB2.minimums.X)
	{
		return false;
	}
	
	if (firstAABB2.maximums.Y < secondAABB2.minimums.Y)
	{
		return false;
	}

	if (secondAABB2.maximums.X < firstAABB2.minimums.X)
	{
		return false;
	}

	if (secondAABB2.maximums.Y < firstAABB2.minimums.Y)
	{
		return false;
	}

	return true;
}



bool DoAABB3sIntersect(const AABB3& firstAABB3, const AABB3& secondAABB3)
{
	if (firstAABB3.maximums.X < secondAABB3.minimums.X)
	{
		return false;
	}

	if (firstAABB3.maximums.Y < secondAABB3.minimums.Y)
	{
		return false;
	}

	if (firstAABB3.maximums.Z < secondAABB3.minimums.Z)
	{
		return false;
	}

	if (secondAABB3.maximums.X < firstAABB3.minimums.X)
	{
		return false;
	}

	if (secondAABB3.maximums.Y < firstAABB3.minimums.Y)
	{
		return false;
	}

	if (secondAABB3.maximums.Z < firstAABB3.minimums.Z)
	{
		return false;
	}

	return true;
}



bool GetIntersectionPointFor2DLines(const Vector2& firstStart, const Vector2& firstEnd, const Vector2& secondStart, const Vector2& secondEnd, Vector2& intersectionPoint)
{
	float determinant = ((firstStart.X - firstEnd.X) * (secondStart.Y - secondEnd.Y)) - ((firstStart.Y - firstEnd.Y) * (secondStart.X - secondEnd.X));
	if (determinant == 0.0f)
	{
		return false;
	}

	float firstCoefficient = (firstStart.X * firstEnd.Y) - (firstStart.Y * firstEnd.X);
	float secondCoefficient = (secondStart.X * secondEnd.Y) - (secondStart.Y * secondEnd.X);

	float X = ((firstCoefficient * (secondStart.X - secondEnd.X)) - ((firstStart.X - firstEnd.X) * secondCoefficient)) / determinant;
	float Y = ((firstCoefficient * (secondStart.Y - secondEnd.Y)) - ((firstStart.Y - firstEnd.Y) * secondCoefficient)) / determinant;

	if (X + F_EPSILON_VALUE < GetMinimumOfTwoFloats(firstStart.X, firstEnd.X) ||
		X - F_EPSILON_VALUE > GetMaximumOfTwoFloats(firstStart.X, firstEnd.X) ||
		X + F_EPSILON_VALUE < GetMinimumOfTwoFloats(secondStart.X, secondEnd.X) ||
		X - F_EPSILON_VALUE > GetMaximumOfTwoFloats(secondStart.X, secondEnd.X))
	{
		return false;
	}

	if (Y + F_EPSILON_VALUE < GetMinimumOfTwoFloats(firstStart.Y, firstEnd.Y) ||
		Y - F_EPSILON_VALUE > GetMaximumOfTwoFloats(firstStart.Y, firstEnd.Y) ||
		Y + F_EPSILON_VALUE < GetMinimumOfTwoFloats(secondStart.Y, secondEnd.Y) ||
		Y - F_EPSILON_VALUE > GetMaximumOfTwoFloats(secondStart.Y, secondEnd.Y))
	{
		return false;
	}

	intersectionPoint.X = X;
	intersectionPoint.Y = Y;

	return true;
}



int GetPositionOfPointRelativeToLine(const Vector2& lineStartingPoint, const Vector2& lineEndingPoint, const Vector2& foreignPoint)
{
	float determinant = ((lineEndingPoint.X - lineStartingPoint.X) * (foreignPoint.Y - lineStartingPoint.Y)) - ((lineEndingPoint.Y - lineStartingPoint.Y) * (foreignPoint.X - lineStartingPoint.X));

	if (determinant > F_EPSILON_VALUE)
	{
		return 1;
	}
	else if (determinant < -F_EPSILON_VALUE)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}



float RangeMap(float inputValue, float inputMinimun, float inputMaximum, float outputMinimum, float outputMaximum)
{
	float inputDifference = inputValue - inputMinimun;
	float inputRange = inputMaximum - inputMinimun;
	float outputRange = outputMaximum - outputMinimum;

	float outputValue = outputMinimum + ((inputDifference / inputRange) * outputRange);

	return outputValue;
}



float ClampFloat(float inputValue, float clampMinimum, float clampMaximum)
{
	if (inputValue < clampMinimum)
	{
		return clampMinimum;
	}
	else if (inputValue > clampMaximum)
	{
		return clampMaximum;
	}
	else
	{
		return inputValue;
	}
}



int ClampInt(int inputValue, int clampMinimum, int clampMaximum)
{
	if (inputValue < clampMinimum)
	{
		return clampMinimum;
	}
	else if (inputValue > clampMaximum)
	{
		return clampMaximum;
	}
	else
	{
		return inputValue;
	}
}



float WrapAroundCircularRange(float inputValue, float rangeMinimum, float rangeMaximum)
{
	float wrappedValue = inputValue;
	float sizeOfRange = rangeMaximum - rangeMinimum;

	while (wrappedValue < rangeMinimum)
	{
		wrappedValue += sizeOfRange;
	}
	while (wrappedValue >= rangeMaximum)
	{
		wrappedValue -= sizeOfRange;
	}

	return wrappedValue;
}



float OscillateWithinRange(float inputValue, float rangeMinimum, float rangeMaximum)
{
	float oscillatedValue = inputValue;

	while (oscillatedValue < rangeMinimum || oscillatedValue > rangeMaximum)
	{
		if (oscillatedValue < rangeMinimum)
		{
			oscillatedValue = (rangeMinimum * 2.0f) - oscillatedValue;
		}
		if (oscillatedValue > rangeMaximum)
		{
			oscillatedValue = (rangeMaximum * 2.0f) - oscillatedValue;
		}
	}

	return oscillatedValue;
}



float SmoothStep(float inputValue)
{
	return (inputValue * inputValue * (3.0f - (2.0f * inputValue)));
}



float FifthPowerSmoothStep(float inputValue)
{
	return (inputValue * inputValue * inputValue * (inputValue * (inputValue * 6.0f - 15.0f) + 10.0f));
}



float CosineOfRadians(float angleInRadians)
{
	return cosf(angleInRadians);
}



float SineOfRadians(float angleInRadians)
{
	return sinf(angleInRadians);
}



float TangentOfRadians(float angleInRadians)
{
	return tanf(angleInRadians);
}



float ArcCosineInRadians(float inputValue)
{
	return acosf(inputValue);
}



float ArcSineInRadians(float inputValue)
{
	return asinf(inputValue);
}



float ArcTangentInRadians(const Vector2& cartesianCoordinates)
{
	return atan2f(cartesianCoordinates.Y, cartesianCoordinates.X);
}



float CosineOfDegrees(float angleInDegrees)
{
	float angleInRadians = ConvertToRadians(angleInDegrees);

	return cosf(angleInRadians);
}



float SineOfDegrees(float angleInDegrees)
{
	float angleInRadians = ConvertToRadians(angleInDegrees);

	return sinf(angleInRadians);
}



float TangentOfDegrees(float angleInDegrees)
{
	float angleInRadians = ConvertToRadians(angleInDegrees);

	return tanf(angleInRadians);
}



float ArcCosineInDegrees(float inputValue)
{
	return ConvertToDegrees(ArcCosineInRadians(inputValue));
}



float ArcSineInDegrees(float inputValue)
{
	return ConvertToDegrees(ArcSineInRadians(inputValue));
}



float ArcTangentInDegrees(const Vector2& cartesianCoordinates)
{
	return ConvertToDegrees(ArcTangentInRadians(cartesianCoordinates));
}



float SquareRoot(float inputValue)
{
	return sqrtf(inputValue);
}



float AbsoluteValue(float inputValue)
{
	return fabsf(inputValue);
}



float CalculateModulus(float dividendValue, float divisorValue)
{
	return fmodf(dividendValue, divisorValue);
}



float RoundDownToFloorValue(float inputValue)
{
	return floorf(inputValue);
}



float RoundUpToCeilingValue(float inputValue)
{
	return ceilf(inputValue);
}



float FastRoundDownToFloorValue(float inputValue)
{
	if (inputValue >= 0.0f)
	{
		return static_cast<float>(static_cast<int>(inputValue));
	}
	else
	{
		float outputValue = static_cast<float>(static_cast<int>(inputValue));

		if (outputValue == inputValue)
		{
			return outputValue;
		}
		else
		{
			return (outputValue - 1.0f);
		}
	}
}



int ShiftBitsLeft(int inputValue, int numberOfBits)
{
	return inputValue << numberOfBits;
}



int ShiftBitsRight(int inputValue, int numberOfBits)
{
	return inputValue >> numberOfBits;
}



void SetBits(unsigned char& bitFlags, unsigned char bitMask)
{
	bitFlags |= bitMask;
}



void SetBits(unsigned short& bitFlags, unsigned short bitMask)
{
	bitFlags |= bitMask;
}



void ClearBits(unsigned char& bitFlags, unsigned char bitMask)
{
	bitFlags &= ~bitMask;
}



bool IsBitSet(unsigned char bitFlags, unsigned char bitMask)
{
	return ((bitFlags & bitMask) == bitMask);
}



bool IsBitSetAtIndex(unsigned short bitFlags, size_t bitIndex)
{
	return ((bitFlags & (1 << bitIndex)) != 0);
}



void SetBitAtIndex(unsigned short& bitFlags, size_t bitIndex)
{
	bitFlags |= (1 << bitIndex);
}



size_t GetPowerOfTwo(size_t powerOfTwoNumber)
{
	double powerValue = log2(static_cast<double>(powerOfTwoNumber));

	return static_cast<size_t>(powerValue);
}



float LinearlyInterpolateIn1D(float firstValue, float secondValue, float interpolationFactor)
{
	return ((firstValue * (1.0f - interpolationFactor)) + (secondValue * interpolationFactor));
}



Vector2 LinearlyInterpolateIn2D(const Vector2& firstVector2, const Vector2& secondVector2, float interpolationFactor)
{
	return ((firstVector2 * (1.0f - interpolationFactor)) + (secondVector2 * interpolationFactor));
}



Vector3 LinearlyInterpolateIn3D(const Vector3& firstVector3, const Vector3& secondVector3, float interpolationFactor)
{
	return ((firstVector3 * (1.0f - interpolationFactor)) + (secondVector3 * interpolationFactor));
}



Vector2 SphericallyInterpolateIn2D(const Vector2& firstVector2, const Vector2& secondVector2, float interpolationFactor)
{
	float firstVector2Magnitude = firstVector2.GetVector2Magnitude();
	float secondVector2Magnitude = secondVector2.GetVector2Magnitude();
	float interpolatedMagnitude = LinearlyInterpolateIn1D(firstVector2Magnitude, secondVector2Magnitude, interpolationFactor);

	Vector2 normalizedfirstVector2 = firstVector2.GetNormalizedVector2();
	Vector2 normalizedsecondVector2 = secondVector2.GetNormalizedVector2();

	float dotProduct = Vector2::DotProduct(normalizedfirstVector2, normalizedsecondVector2);
	dotProduct = ClampFloat(dotProduct, -1.0f, 1.0f);

	Vector2 interpolatedVector2;
	float angleBetweenVectors = ArcCosineInDegrees(dotProduct);

	if (angleBetweenVectors < F_EPSILON_VALUE)
	{
		interpolatedVector2 = LinearlyInterpolateIn2D(normalizedfirstVector2, normalizedsecondVector2, interpolationFactor);
	}
	else
	{
		float coefficientDenominator = SineOfDegrees(angleBetweenVectors);

		float firstCoefficient = SineOfDegrees((1.0f - interpolationFactor) * angleBetweenVectors) / coefficientDenominator;
		float secondCoefficient = SineOfDegrees(interpolationFactor * angleBetweenVectors) / coefficientDenominator;

		interpolatedVector2 = (normalizedfirstVector2 * firstCoefficient) + (normalizedsecondVector2 * secondCoefficient);
	}

	return (interpolatedVector2 * interpolatedMagnitude);
}



Vector3 SphericallyInterpolateIn3D(const Vector3& firstVector3, const Vector3& secondVector3, float interpolationFactor)
{
	float firstVector3Magnitude = firstVector3.GetVector3Magnitude();
	float secondVector3Magnitude = secondVector3.GetVector3Magnitude();
	float interpolatedMagnitude = LinearlyInterpolateIn1D(firstVector3Magnitude, secondVector3Magnitude, interpolationFactor);

	Vector3 normalizedfirstVector3 = firstVector3.GetNormalizedVector3();
	Vector3 normalizedsecondVector3 = secondVector3.GetNormalizedVector3();
	
	float dotProduct = Vector3::DotProduct(normalizedfirstVector3, normalizedsecondVector3);
	dotProduct = ClampFloat(dotProduct, -1.0f, 1.0f);

	Vector3 interpolatedVector3;
	float angleBetweenVectors = ArcCosineInDegrees(dotProduct);

	if (angleBetweenVectors < F_EPSILON_VALUE)
	{
		interpolatedVector3 = LinearlyInterpolateIn3D(normalizedfirstVector3, normalizedsecondVector3, interpolationFactor);
	}
	else
	{
		float coefficientDenominator = SineOfDegrees(angleBetweenVectors);

		float firstCoefficient = SineOfDegrees((1.0f - interpolationFactor) * angleBetweenVectors) / coefficientDenominator;
		float secondCoefficient = SineOfDegrees(interpolationFactor * angleBetweenVectors) / coefficientDenominator;

		interpolatedVector3 = (normalizedfirstVector3 * firstCoefficient) + (normalizedsecondVector3 * secondCoefficient);
	}

	return (interpolatedVector3 * interpolatedMagnitude);
}



float CardinallyInterpolateIn2D(float firstValue, float secondValue, float negativeSlope, float positiveSlope, float blendFactor)
{
	float valueA = positiveSlope - (secondValue - firstValue);
	float valueB = -negativeSlope + (secondValue - firstValue);

	float oneMinusBlendFactor = 1.0f - blendFactor;

	float firstPart = (oneMinusBlendFactor * firstValue) + (blendFactor * secondValue);
	float secondPart = blendFactor * oneMinusBlendFactor;
	float thirdPart = (valueA * oneMinusBlendFactor) + (valueB * blendFactor);

	return (firstPart + (secondPart * thirdPart));
}