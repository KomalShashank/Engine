#pragma once

#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/VectorMath/2D/IntVector2.hpp"
#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/VectorMath/3D/IntVector3.hpp"
#include "Engine/Math/VectorMath/2D/AABB2.hpp"
#include "Engine/Math/VectorMath/3D/AABB3.hpp"



const double D_TWO_PI_VALUE = 6.283185307179586476925286766559;
const double D_PI_VALUE = 3.1415926535897932384626433832795;
const double D_HALF_PI_VALUE = 1.5707963267948966192313216916398;
const double D_QUARTER_PI_VALUE = 0.78539816339744830961566084581988;
const double D_EPSILON_VALUE = 0.00001000000000000000000000000000;

const double D_SQUARE_ROOT_OF_2 = 1.4142135623730950488016887242097;
const double D_SQUARE_ROOT_OF_3 = 1.7320508075688772935274463415059;
const double D_SQUARE_ROOT_OF_2_BY_2 = 0.70710678118654752440084436210485;
const double D_SQUARE_ROOT_OF_3_BY_2 = 0.86602540378443864676372317075294;
const double D_SQUARE_ROOT_OF_3_BY_3 = 0.57735026918962576450914878050196;
const double D_SQUARE_ROOT_OF_3_BY_6 = 0.28867513459481288225457439025098;



const float F_TWO_PI_VALUE = 6.283185307179586476925286766559f;
const float F_PI_VALUE = 3.1415926535897932384626433832795f;
const float F_HALF_PI_VALUE = 1.5707963267948966192313216916398f;
const float F_QUARTER_PI_VALUE = 0.78539816339744830961566084581988f;
const float F_EPSILON_VALUE = 0.00001000000000000000000000000000f;

const float F_SQUARE_ROOT_OF_2 = 1.4142135623730950488016887242097f;
const float F_SQUARE_ROOT_OF_3 = 1.7320508075688772935274463415059f;
const float F_SQUARE_ROOT_OF_2_BY_2 = 0.70710678118654752440084436210485f;
const float F_SQUARE_ROOT_OF_3_BY_2 = 0.86602540378443864676372317075294f;
const float F_SQUARE_ROOT_OF_3_BY_3 = 0.57735026918962576450914878050196f;
const float F_SQUARE_ROOT_OF_3_BY_6 = 0.28867513459481288225457439025098f;



float GetMinimumOfTwoFloats(float first, float second);
int GetMinimumOfTwoInts(int first, int second);
size_t GetMinimumOfTwoSize_T(size_t first, size_t second);

float GetMaximumOfTwoFloats(float first, float second);
int GetMaximumOfTwoInts(int first, int second);
size_t GetMaximumOfTwoSize_T(size_t first, size_t second);

float GetMinimumOfThreeFloats(float first, float second, float third);
int GetMinimumOfThreeInts(int first, int second, int third);

float GetMaximumOfThreeFloats(float first, float second, float third);
int GetMaximumOfThreeInts(int first, int second, int third);

float GetMinimumOfFourFloats(float first, float second, float third, float fourth);
int GetMinimumOfFourInts(int first, int second, int third, int fourth);

float GetMaximumOfFourFloats(float first, float second, float third, float fourth);
int GetMaximumOfFourInts(int first, int second, int third, int fourth);

float GetMaximumOfFloatArray(float* floatArray, size_t arraySize);
int GetMaximumOfIntArray(int* intArray, size_t arraySize);

float GetMinimumOfFloatArray(float* floatArray, size_t arraySize);
int GetMinimumOfIntArray(int* intArray, size_t arraySize);

float GetRandomFloatWithinRange(float rangeMinimum, float rangeMaximum);
double GetRandomDoubleWithinRange(double rangeMinimum, double rangeMaximum);
int GetRandomIntWithinRange(int rangeMinimum, int rangeMaximum);

Vector2 GetRandomVector2WithinRange(const Vector2& rangeMinimum, const Vector2& rangeMaximum);
IntVector2 GetRandomIntVector2WithinRange(const IntVector2& rangeMinimum, const IntVector2& rangeMaximum);

Vector3 GetRandomVector3WithinRange(const Vector3& rangeMinimum, const Vector3& rangeMaximum);
IntVector3 GetRandomIntVector3WithinRange(const IntVector3& rangeMinimum, const IntVector3& rangeMaximum);

float ConvertToRadians(float angleInDegrees);
float ConvertToDegrees(float angleInRadians);

Vector2 ConvertToPolar(const Vector2& cartesianCoordinates);
Vector2 ConvertToCartesian(const Vector2& polarCoordinates);

float CalculateEuclidianSquaredDistanceIn2D(const Vector2& startingPoint, const Vector2& endingPoint);
float CalculateEuclidianDistanceIn2D(const Vector2& startingPoint, const Vector2& endingPoint);
float CalculateManhattanDistanceIn2D(const Vector2& startingPoint, const Vector2& endingPoint);

float CalculateEuclidianSquaredDistanceIn3D(const Vector3& startingPoint, const Vector3& endingPoint);
float CalculateEuclidianDistanceIn3D(const Vector3& startingPoint, const Vector3& endingPoint);
float CalculateManhattanDistanceIn3D(const Vector3& startingPoint, const Vector3& endingPoint);

Vector2 CalculateCentroidOfTriangleIn2D(const Vector2& firstPoint, const Vector2& secondPoint, const Vector2& thirdPoint);
Vector3 CalculateCentroidOfTriangleIn3D(const Vector3& firstPoint, const Vector3& secondPoint, const Vector3& thirdPoint);

bool PointIsInsideCircle(const Vector2& foreignPoint, const Vector2& circleCenter, float circleRadius);
bool DoCirclesIntersect(const Vector2& firstCircleCenter, float firstCircleRadius, const Vector2& secondCircleCenter, float secondCircleRadius);
float CalculateShortestAngularDisplacement(float startingAngleInDegrees, float endingAngleInDegrees);

bool DoAABB2sIntersect(const AABB2& firstAABB2, const AABB2& secondAABB2);
bool DoAABB3sIntersect(const AABB3& firstAABB3, const AABB3& secondAABB3);

bool GetIntersectionPointFor2DLines(const Vector2& firstStart, const Vector2& firstEnd, const Vector2& secondStart, const Vector2& secondEnd, Vector2& intersectionPoint);
int GetPositionOfPointRelativeToLine(const Vector2& lineStartingPoint, const Vector2& lineEndingPoint, const Vector2& foreignPoint);

float RangeMap(float inputValue, float inputMinimun, float inputMaximum, float outputMinimum, float outputMaximum);
float ClampFloat(float inputValue, float clampMinimum, float clampMaximum);
int ClampInt(int inputValue, int clampMinimum, int clampMaximum);
float WrapAroundCircularRange(float inputValue, float rangeMinimum, float rangeMaximum);
float OscillateWithinRange(float inputValue, float rangeMinimum, float rangeMaximum);

float SmoothStep(float inputValue);
float FifthPowerSmoothStep(float inputValue);

float CosineOfRadians(float angleInRadians);
float SineOfRadians(float angleInRadians);
float TangentOfRadians(float angleInRadians);

float ArcCosineInRadians(float inputValue);
float ArcSineInRadians(float inputValue);
float ArcTangentInRadians(const Vector2& cartesianCoordinates);

float CosineOfDegrees(float angleInDegrees);
float SineOfDegrees(float angleInDegrees);
float TangentOfDegrees(float angleInDegrees);

float ArcCosineInDegrees(float inputValue);
float ArcSineInDegrees(float inputValue);
float ArcTangentInDegrees(const Vector2& cartesianCoordinates);

float SquareRoot(float inputValue);
float AbsoluteValue(float inputValue);
float CalculateModulus(float dividendValue, float divisorValue);

float RoundDownToFloorValue(float inputValue);
float RoundUpToCeilingValue(float inputValue);

float FastRoundDownToFloorValue(float inputValue);

int ShiftBitsLeft(int inputValue, int numberOfBits);
int ShiftBitsRight(int inputValue, int numberOfBits);

void SetBits(unsigned char& bitFlags, unsigned char bitMask);
void SetBits(unsigned short& bitFlags, unsigned short bitMask);
void ClearBits(unsigned char& bitFlags, unsigned char bitMask);
bool IsBitSet(unsigned char bitFlags, unsigned char bitMask);
bool IsBitSetAtIndex(unsigned short bitFlags, size_t bitIndex);
void SetBitAtIndex(unsigned short& bitFlags, size_t bitIndex);

size_t GetPowerOfTwo(size_t powerOfTwoNumber);

float LinearlyInterpolateIn1D(float firstValue, float secondValue, float interpolationFactor);
Vector2 LinearlyInterpolateIn2D(const Vector2& firstVector2, const Vector2& secondVector2, float interpolationFactor);
Vector3 LinearlyInterpolateIn3D(const Vector3& firstVector3, const Vector3& secondVector3, float interpolationFactor);

Vector2 SphericallyInterpolateIn2D(const Vector2& firstVector2, const Vector2& secondVector2, float interpolationFactor);
Vector3 SphericallyInterpolateIn3D(const Vector3& firstVector3, const Vector3& secondVector3, float interpolationFactor);

float CardinallyInterpolateIn2D(float firstValue, float secondValue, float negativeSlope, float positiveSlope, float blendFactor);