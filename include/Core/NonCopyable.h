#pragma once

namespace Amber
{

class NonCopyable
{
protected:
	NonCopyable() {}

private:
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;

	NonCopyable(NonCopyable&&) = delete;
	NonCopyable& operator=(NonCopyable&&) = delete;
};

}