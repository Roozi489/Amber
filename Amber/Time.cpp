#include "Time.h"

#include <thread>

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

long long Time::asMicroseconds() const
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

Time seconds(float amount)
{
	return Time(static_cast<long>(amount * 1000000));
}

Time milliseconds(float amount)
{
	return Time(static_cast<long>(amount * 1000));
}

Time microseconds(long long amount)
{
	return Time(amount);
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