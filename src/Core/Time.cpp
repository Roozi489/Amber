#include "Core/Time.h"

#include <thread>

namespace Amber
{

const Time Time::Zero = Time(0);

Time::Time()
	: mMicroseconds(0)
{
}

float Time::asSeconds() const
{
	return mMicroseconds.count() / 1000000.0f;
}

float Time::asMilliseconds() const
{
	return mMicroseconds.count() / 1000.0f;
}

int64_t Time::asMicroseconds() const
{
	return mMicroseconds.count();
}

Time Time::now()
{
	auto now = std::chrono::high_resolution_clock::now().time_since_epoch();

	return microseconds(std::chrono::duration_cast<std::chrono::microseconds>(now).count());
}

void Time::sleep(Time time)
{
	std::this_thread::sleep_for(std::chrono::microseconds(time.asMicroseconds()));
}

Time::Time(long long microseconds)
	: mMicroseconds(std::chrono::microseconds(microseconds))
{
}

bool operator==(Time lhs, Time rhs)
{
	return lhs.asMicroseconds() == rhs.asMicroseconds();
}

bool operator!=(Time lhs, Time rhs)
{
	return lhs.asMicroseconds() != rhs.asMicroseconds();
}

bool operator<(Time lhs, Time rhs)
{
	return lhs.asMicroseconds() < rhs.asMicroseconds();
}

bool operator>(Time lhs, Time rhs)
{
	return lhs.asMicroseconds() > rhs.asMicroseconds();
}

bool operator<=(Time lhs, Time rhs)
{
	return lhs.asMicroseconds() <= rhs.asMicroseconds();
}

bool operator>=(Time lhs, Time rhs)
{
	return lhs.asMicroseconds() >= rhs.asMicroseconds();
}

Time operator-(Time rhs)
{
	return microseconds(-rhs.asMicroseconds());
}

Time operator+(Time lhs, Time rhs)
{
	return microseconds(lhs.asMicroseconds() + rhs.asMicroseconds());
}

Time operator-(Time lhs, Time rhs)
{
	return microseconds(lhs.asMicroseconds() - rhs.asMicroseconds());
}

Time& operator+=(Time& lhs, Time rhs)
{
	return lhs = lhs + rhs;
}

Time& operator-=(Time& lhs, Time rhs)
{
	return lhs = lhs - rhs;
}

}