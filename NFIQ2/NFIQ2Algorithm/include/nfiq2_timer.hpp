/*
 * This file is part of NIST Fingerprint Image Quality (NFIQ) 2. For more
 * information on this project, refer to:
 *   - https://nist.gov/services-resources/software/nfiq2
 *   - https://github.com/usnistgov/NFIQ2
 *
 * This work is in the public domain. For complete licensing details, refer to:
 *   - https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md
 */

#ifndef NFIQ2_TIMER_HPP_
#define NFIQ2_TIMER_HPP_

#include <chrono>

namespace NFIQ2 {

/** Used to calculate speed of internal operations. */
class Timer {
    public:
	/**
	 * @brief
	 * Start the timer.
	 *
	 * @note Resets currently running timer.
	 */
	void start();

	/**
	 * @brief
	 * Obtain the elapsed time.
	 *
	 * @return
	 * Elapsed time (milliseconds).
	 */
	double getElapsedTime();

	/**
	 * @brief
	 * Stop the timer.
	 *
	 * @return
	 * Elapsed time (milliseconds).
	 */
	double stop();

    private:
	/** Time when timer was started. */
	std::chrono::steady_clock::time_point startTime {};
	/** Time when timer was stopped. */
	std::chrono::steady_clock::time_point endTime {};
};
} // namespace NFIQ

#endif /* NFIQ2_TIMER_HPP_ */
