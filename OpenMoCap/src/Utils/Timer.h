#ifndef TIMER_H_
#define TIMER_H_

#include <windows.h>
#include <winbase.h>

class Timer {

private:
	//! Structures to mark time.
	LARGE_INTEGER start_ticks, end_ticks, cputime, ticksPerSecond;

public:

	/*!
	 * Timer constructor.
	 */
	Timer();

	/*!
	 * Timer destructor.
	 */
	~Timer();

	/*!
	 * Starts the mark.
	 */
	void start();

	/*!
	 * Stops the mark.
	 *
	 * @return The time spent by the program to execute.
	 */
	float stop();
};

#endif /* TIMER_H_ */
