#pragma once
#include <chrono>

#include <cstdint>

class Time
{
public:
	Time();

	static const Time Zero;

	float asSeconds() const;
	float asMilliseconds() const;
	int64_t asMicroseconds() const;

	static Time now();
	static void sleep(Time time); // Sleep on current thread

private:
	explicit Time(int64_t microseconds);

	std::chrono::microseconds mMicroseconds;

	template <typename T>
	friend Time seconds(T amount);
	template <typename T>
	friend Time milliseconds(T amount);
	template <typename T>
	friend Time microseconds(T amount);
};

template <typename T>
Time seconds(T amount) { return Time(static_cast<int64_t>(amount * 1000000)); };
template <typename T>
Time milliseconds(T amount) { return Time(static_cast<int64_t>(amount * 1000)); };
template <typename T>
Time microseconds(T amount) { return Time(static_cast<int64_t>(amount)); };

bool operator==(Time lhs, Time rhs);
bool operator!=(Time lhs, Time rhs);

bool operator<(Time lhs, Time rhs);
bool operator>(Time lhs, Time rhs);

bool operator<=(Time lhs, Time rhs);
bool operator>=(Time lhs, Time rhs);

Time operator-(Time rhs);

Time operator+(Time lhs, Time rhs);
Time operator-(Time lhs, Time rhs);

template <typename T>
Time operator*(Time lhs, T rhs) { return microseconds(lhs.asMicroseconds() * rhs); }
template <typename T>
Time operator*(T lhs, Time rhs) { return microseconds(lhs * rhs.asMicroseconds()); }

template <typename T>
Time operator/(Time lhs, T rhs) { return microseconds(lhs.asMicroseconds() / rhs); }
template <typename T>
Time operator/(T lhs, Time rhs) { return microseconds(lhs / rhs.asMicroseconds()); }

Time& operator+=(Time& lhs, Time rhs);
Time& operator-=(Time& lhs, Time rhs);