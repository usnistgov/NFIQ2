#ifndef TIMER_HPP
#define TIMER_HPP

#include "OS.h"
#include <time.h>
#include <iostream>

#ifdef WINDOWS
#include <windows.h>
#else
#if defined(__APPLE__)
#include <mach/clock.h>
#include <mach/mach.h>
#else
#include <sys/time.h>
#endif
#endif

namespace NFIQ
{
	class Clock 
	{
	public:
		/**
		* @brief Get the current system milli seconds time
		* @return system time
		*/
		static double getTotalMilliseconds()
		{
#ifdef WINDOWS
			LARGE_INTEGER li, freq;
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&li);
			return (static_cast<double>(li.QuadPart)/static_cast<double>(freq.QuadPart) * 1000.0);
#else
#if defined(__APPLE__)
            clock_serv_t cclock;
            mach_timespec_t ts;
            host_get_clock_service(mach_host_self(), REALTIME_CLOCK, &cclock);
            clock_get_time(cclock, &ts);
            mach_port_deallocate(mach_task_self(), cclock);
#else
			timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
			return static_cast<double>(ts.tv_sec * 1000.0 + (ts.tv_nsec / 1000000.0));
#endif
		}		
	};

	class Timer 
	{
	public:
		/**
		* @brief Constructor
		*/
		Timer()
		{
			this->startTime = 0;
			this->endTime = 0;
		}

		/**
		* @brief Sets the start time
		*/
		void startTimer()
		{
			this->endTime = 0;
			this->startTime = NFIQ::Clock::getTotalMilliseconds();
		};

		/**
		* @brief Sets the end time
		*/
		void endTimer()
		{
			this->endTime = NFIQ::Clock::getTotalMilliseconds();
		};

		/**
		* @brief Gets the elapsed time
		* @return elapsed time
		*/
		double getElapsedTime()
		{
			return (this->endTime - this->startTime);
		};

		/**
		* @brief Ends timer and gets the elapsed time
		* @return elapsed time
		*/
		double endTimerAndGetElapsedTime()
		{
			this->endTime = NFIQ::Clock::getTotalMilliseconds();
			return (this->endTime - this->startTime);
		};

	private:
		double startTime;
		double endTime;
	};
}

#endif