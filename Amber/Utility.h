#pragma once
#include "Vector.h"

#include <string>
#include <vector>

const float pi = 3.141592653589793238462643383f;
const float pi_2 = 1.570796326794896619231321691f;

struct FaceIndexVertTexNorm
{
    unsigned int v0, t0, n0;
    unsigned int v1, t1, n1;
    unsigned int v2, t2, n2;
};



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

enum class BitmapFormat
{
    GRAY8,
    RGB8,
    RGBA8
};

struct Bitmap
{
    int width;
    int height;
    BitmapFormat format;
    unsigned char* buffer;
};

float toDegrees(float valueInRadians);
float toRadians(float valueInDegrees);

bool getLowestPosRoot(float a, float b, float c, float maxR, float* root);

// TODO: actual test
void testVectorsMatricesRays();

std::string loadFileToString(const std::string& fileName);

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems);
std::vector<std::string> split(const std::string& s, char delim);

std::string toLower(const std::string& text);

bool compareStringsCaseInsensitive(const std::string& a, const std::string& b);

void _checkGlError(const char* file, int line, const char* function);
#define checkGlError() _checkGlError(__FILE__,__LINE__,__FUNCTION__)
void ignoreGLError();

void log(std::string message);
void clearLog();

struct SDL_Surface; // Forward declare
SDL_Surface* loadSDL_SurfaceFromFile(const char* filename);

void _criticalError(const char* message, const char* file, int line, const char* function);
void _criticalError(const std::string& message, const char* file, int line, const char* function);
#define criticalError(msg) _criticalError(msg, __FILE__,__LINE__,__FUNCTION__)

void messageBox(const char* message);
void messageBox(const std::string& message);
void messageBox(const char* title, const char* message);
void messageBox(const std::string& title, const std::string& message);

void _assert(bool expression, const char* message, const char* file, int line, const char* function);
void _assert(bool expression, const std::string& message, const char* file, int line, const char* function);
#if _DEBUG
#define assert_amber(expression, message) _assert(expression, message, __FILE__,__LINE__,__FUNCTION__)
#else
#define assert_amber(expression, message)
#endif

void quit();

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

// ScopeGuard
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