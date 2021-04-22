#ifndef NFIQ2_TIMER_HPP_
#define NFIQ2_TIMER_HPP_

#include <chrono>

namespace NFIQ {
/** Used to calculate speed of internal operations. */
class Timer {
    public:
	/**
	 * @brief Start the timer.
	 * @note Resets currently running timer.
	 */
	void start();

	/**
	 * @brief Gets the elapsed time.
	 * @return Elapsed time (milliseconds).
	 */
	double getElapsedTime();

	/**
	 * @brief End timer, returning the elapsed time.
	 * @return Elapsed time (milliseconds).
	 */
	double endTimerAndGetElapsedTime();

    private:
	/** Time when timer was started. */
	std::chrono::steady_clock::time_point startTime {};
	/** Time when timer was stopped. */
	std::chrono::steady_clock::time_point endTime {};
};
} // namespace NFIQ

#endif
