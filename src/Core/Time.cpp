#include "Core/Time.h"

#include <thread>
#include <iomanip>
#include <sstream>

namespace Amber
{

const Time Time::Zero = Time(0);

Time::Time()
	: m_microseconds(0)
{
}

float Time::asSeconds() const
{
	return m_microseconds.count() / 1000000.0f;
}

float Time::asMilliseconds() const
{
	return m_microseconds.count() / 1000.0f;
}

int64_t Time::asMicroseconds() const
{
	return m_microseconds.count();
}

std::string Time::localTimeString(const char* format)
{
	auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::stringstream ss;
	tm timeInfo;
	localtime_s(&timeInfo, &in_time_t);
	ss << std::put_time(&timeInfo, format);
	return ss.str();
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
	: m_microseconds(std::chrono::microseconds(microseconds))
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
