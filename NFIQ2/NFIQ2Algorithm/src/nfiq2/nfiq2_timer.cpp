#include <nfiq2_timer.hpp>

#include <chrono>

void
NFIQ2::Timer::start()
{
	this->endTime = std::chrono::steady_clock::time_point {};
	this->startTime = std::chrono::steady_clock::now();
}

double
NFIQ2::Timer::getElapsedTime()
{
	if (this->endTime < this->startTime)
		return std::numeric_limits<double>::signaling_NaN();

	return std::chrono::duration<double, std::milli>(
	    this->endTime - this->startTime)
	    .count();
}

double
NFIQ2::Timer::stop()
{
	this->endTime = std::chrono::steady_clock::now();
	return this->getElapsedTime();
}
