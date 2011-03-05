/*!
 * Class that contains functions to mark time.
 *
 * \name Timer
 * \author David Lunardi Flam
 * \version
 * \since 06/23/2008
 */
#include "Timer.h"

Timer::Timer() {

}

Timer::~Timer() {

}

void Timer::start() {

	QueryPerformanceCounter(&start_ticks);
	QueryPerformanceFrequency(&ticksPerSecond);
}

float Timer::stop() {

	QueryPerformanceCounter(&end_ticks);
	cputime.QuadPart = end_ticks.QuadPart - start_ticks.QuadPart;

	//---Resets Counter
	QueryPerformanceCounter(&start_ticks);

	return (float) cputime.QuadPart / (float) ticksPerSecond.QuadPart;
}
