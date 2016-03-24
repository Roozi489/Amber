#pragma once
#include "Math/Vector.h"

#include <string>

namespace Amber
{

// TODO: Physics
enum class CollisionHit
{
	Face,
	Edge,
	Point
};

struct CollisionResult
{
	bool collisionOccured;
	Vector3f collisionPoint;
	Vector3f collisionNormal;
	float collisionDistance;
	float collisionTime;

	CollisionHit collisionHit;
};

// TODO: Logger
void log(std::string&& message);
void log(const std::string& message);
void clearLog();


void _assert(bool expression, const char* message, const char* file, int line, const char* function);
void _assert(bool expression, const std::string& message, const char* file, int line, const char* function);
#if _DEBUG
#define assert_amber(expression, message) _assert(expression, message, __FILE__,__LINE__,__FUNCTION__)
#else
#define assert_amber(expression, message)
#endif

}