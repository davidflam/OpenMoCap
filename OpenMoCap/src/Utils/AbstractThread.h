#ifndef ABSTRACTTHREAD_H_
#define ABSTRACTTHREAD_H_

#include <windows.h>
#include <exception>

using namespace std;

class AbstractThread {

public:

	/*!
	 * AbstractThread destructor.
	 */
	virtual ~AbstractThread();

	//--- Thread Management
	/*!
	 * Waits for a thread to end.
	 *
	 * @return If the waiting for the thread was successful, this function returns "true", otherwise "false".
	 */
	bool wait();

	/*!
	 * Suspends the thread activity.
	 *
	 * @return If the thread activity was suspended correctly, this function returns "true", otherwise "false".
	 */
	bool suspend();

	/*!
	 * Resumes a suspended thread.
	 *
	 * @return If the thread activity was resumed correctly, this function returns "true", otherwise "false".
	 */
	bool resume();

	/*!
	 * Kills the thread.
	 *
	 * @return If the thread was killed correctly, this function returns "true", otherwise "false".
	 */
	bool kill();

	/*!
	 * Verifies the thread activity.
	 *
	 * @return If the thread is active, this function returns "true", otherwise "false".
	 */
	bool isActive();

	/*!
	 * Actual code that the thread will run.
	 * When inheriting, one must provide implementation for this method.
	 */
	virtual void run() = 0;

	/*!
	 * Starts the routine for Win32 thread API.
	 */
	friend DWORD WINAPI _threadFunc(LPVOID pvThread);

protected:
	/*!
	 * AbstractThread constructor.
	 */
	AbstractThread();

	//! Thread handle.
	HANDLE threadHandle;

	//! Activity indicator.
	bool active;

	//! Thread ID.
	DWORD threadId;
};


#endif /* ABSTRACTTHREAD_H_ */
