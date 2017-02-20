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


void _assert(bool expression, const char* message, const char* file, int line, const char* function);
void _assert(bool expression, const std::string& message, const char* file, int line, const char* function);
#if _DEBUG
#define assert_amber(expression, message) _assert(expression, message, __FILE__,__LINE__,__FUNCTION__)
#else
#define assert_amber(expression, info)
#endif

template<typename T>
constexpr auto toUnderlyingType(T value) -> typename std::underlying_type<T>::type
{
	return static_cast<typename std::underlying_type<T>::type>(value);
}

template<typename T, typename... Ts>
constexpr std::array<std::decay_t<T>, 1 + sizeof... (Ts)> make_array(T&& t, Ts&&... ts)  noexcept(noexcept(std::is_nothrow_constructible<std::array<std::decay_t<T>, 1 + sizeof... (Ts)>, T&&, Ts&&...>::value))
{
	return{ { std::forward<T>(t), std::forward<Ts>(ts)... } };
}

template<typename T>
constexpr std::array<std::decay<T>, 0> make_array() noexcept
{
	return{};
}

}