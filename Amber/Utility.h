#pragma once
#include "Vector.h"

#include <string>


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
void log(std::string message);
void clearLog();


void _assert(bool expression, const char* message, const char* file, int line, const char* function);
void _assert(bool expression, const std::string& message, const char* file, int line, const char* function);
#if _DEBUG
#define assert_amber(expression, message) _assert(expression, message, __FILE__,__LINE__,__FUNCTION__)
#else
#define assert_amber(expression, message)
#endif

template <typename T>
T clamp(T value, T min = 0, T max = 1)
{
    if (value < min)
        value = min;
    if (value > max)
        value = max;

    return value;
}

// TODO: rewrite to use variadic templates
template <typename T, typename R>
auto min(T first, R second)
{
    return first < second ? first : second;
}

template <typename T, typename R, typename S>
auto min(T first, R second, S third)
{
    return min(first, second) < third ? min(first, second) : third;
}

template <typename T, typename R>
auto max(T first, R second)
{
    return first > second ? first : second;
}

template <typename T, typename R, typename S>
auto max(T first, R second, S third)
{
    return max(first, second) > third ? max(first, second) : third;
}

namespace Impl
{
	template <typename Fn>
	struct Defer
	{
		Defer(Fn&& fn)
			: fn{ std::forward<Fn>(fn) }
		{
		}
		~Defer() { fn(); };
		Fn fn;
	};

	template <typename Fn>
	Defer<Fn> deferFn(Fn&& fn) { return Defer<Fn>(std::forward<Fn>(fn)); }
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)
#define defer(code) auto DEFER_3(_defer_) = Impl::deferFn([&](){code;});