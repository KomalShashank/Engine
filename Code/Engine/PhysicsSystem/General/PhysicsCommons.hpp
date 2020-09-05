#pragma once

#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"
#include "Engine/PhysicsSystem/General/MathClasses.hpp"

#define UNUSED(x) (void)(x)
#define INVALID_ID -1



const size_t MAXIMUM_POLYGON_VERTICES = 8U;
const size_t MAXIMUM_NUMBER_OF_CONTACT_POINTS = 2U;

const float LINEAR_DELTA = 0.005f;
const float ANGULAR_DELTA = 2.0f;

const float MINIMUM_LINEAR_VELOCITY_THRESHOLD = 0.01f;
const float MINIMUM_ANGULAR_VELOCITY_THRESHOLD = 2.0f;

const size_t NUMBER_OF_VELOCITY_ITERATIONS = 8U;
const size_t NUMBER_OF_POSITION_ITERATIONS = 3U;
const size_t NUMBER_OF_TIME_OF_IMPACT_POSITION_ITERATIONS = 20U;

const Vector2D DOWN_DIRECTION(0.0f, -1.0f);