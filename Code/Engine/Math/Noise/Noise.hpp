#pragma once

#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/VectorMath/4D/Vector4.hpp"



unsigned int GetRawNoiseIn1D(int index, unsigned int randomSeed = 0);
unsigned int GetRawNoiseIn2D(int indexX, int indexY, unsigned int randomSeed = 0);
unsigned int GetRawNoiseIn3D(int indexX, int indexY, int indexZ, unsigned int randomSeed = 0);
unsigned int GetRawNoiseIn4D(int indexX, int indexY, int indexZ, int indexW, unsigned int randomSeed = 0);

float GetRawNoiseIn1DFromZeroToOne(int index, unsigned int randomSeed = 0);
float GetRawNoiseIn2DFromZeroToOne(int indexX, int indexY, unsigned int randomSeed = 0);
float GetRawNoiseIn3DFromZeroToOne(int indexX, int indexY, int indexZ, unsigned int randomSeed = 0);
float GetRawNoiseIn4DFromZeroToOne(int indexX, int indexY, int indexZ, int indexW, unsigned int randomSeed = 0);

float GetRawNoiseIn1DFromMinusOneToOne(int index, unsigned int randomSeed = 0);
float GetRawNoiseIn2DFromMinusOneToOne(int indexX, int indexY, unsigned int randomSeed = 0);
float GetRawNoiseIn3DFromMinusOneToOne(int indexX, int indexY, int indexZ, unsigned int randomSeed = 0);
float GetRawNoiseIn4DFromMinusOneToOne(int indexX, int indexY, int indexZ, int indexW, unsigned int randomSeed = 0);

float ComputeFractalNoiseIn1D(float position, float noiseScale = 1.0f, unsigned int numberOfOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.0f, bool renormalizeNoise = true, unsigned int randomSeed = 0);
float ComputeFractalNoiseIn2D(Vector2 position, float noiseScale = 1.0f, unsigned int numberOfOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.0f, bool renormalizeNoise = true, unsigned int randomSeed = 0);
float ComputeFractalNoiseIn3D(Vector3 position, float noiseScale = 1.0f, unsigned int numberOfOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.0f, bool renormalizeNoise = true, unsigned int randomSeed = 0);
float ComputeFractalNoiseIn4D(Vector4 position, float noiseScale = 1.0f, unsigned int numberOfOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.0f, bool renormalizeNoise = true, unsigned int randomSeed = 0);

float ComputePerlinNoiseIn1D(float position, float noiseScale = 1.0f, unsigned int numberOfOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.0f, bool renormalizeNoise = true, unsigned int randomSeed = 0);
float ComputePerlinNoiseIn2D(Vector2 position, float noiseScale = 1.0f, unsigned int numberOfOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.0f, bool renormalizeNoise = true, unsigned int randomSeed = 0);
float ComputePerlinNoiseIn3D(Vector3 position, float noiseScale = 1.0f, unsigned int numberOfOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.0f, bool renormalizeNoise = true, unsigned int randomSeed = 0);
// float ComputePerlinNoiseIn4D(Vector4 position, float noiseScale = 1.0f, unsigned int numberOfOctaves = 1, float octavePersistence = 0.5f, float octaveScale = 2.0f, bool renormalizeNoise = true, unsigned int randomSeed = 0);