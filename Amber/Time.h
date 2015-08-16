#pragma once
#include <chrono>

class Time
{
public:
	Time();

	static const Time Zero;

	float asSeconds() const;
	float asMilliseconds() const;
	long long asMicroseconds() const;

	static Time now();
	static void sleep(Time time); // Sleep on current thread

private:
	friend Time seconds(float amount);
	friend Time milliseconds(float amount);
	friend Time microseconds(long long amount);

	explicit Time(long long microseconds);

	std::chrono::microseconds mMicroseconds;
};

Time seconds(float amount);
Time milliseconds(float amount);
Time microseconds(long amount);

bool operator==(Time lhs, Time rhs);
bool operator!=(Time lhs, Time rhs);

bool operator<(Time lhs, Time rhs);
bool operator>(Time lhs, Time rhs);

bool operator<=(Time lhs, Time rhs);
bool operator>=(Time lhs, Time rhs);

Time operator-(Time rhs);

Time operator+(Time lhs, Time rhs);
Time operator-(Time lhs, Time rhs);

Time& operator+=(Time& lhs, Time rhs);
Time& operator-=(Time& lhs, Time rhs);