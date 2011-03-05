/*!
 * Abstract class for threads.
 *
 * \name AbstractThread
 * \author David Lunardi Flam
 * \version
 * \since 02/05/2009
 */
#include "AbstractThread.h"

DWORD WINAPI _threadFunc(LPVOID pvThread) {
	AbstractThread* pThread = (AbstractThread*) pvThread; //typecast

	pThread->run();

	return 0;
}

bool AbstractThread::suspend() {
	active = false;
	return ((unsigned long) -1 != SuspendThread(threadHandle));
}

bool AbstractThread::kill() {
	return (BOOL) TerminateThread(threadHandle, 1);
}

bool AbstractThread::resume() {
	active = true;
	return ((unsigned long) -1 != ResumeThread(threadHandle));
}

bool AbstractThread::wait() {
	return (WAIT_OBJECT_0 == WaitForSingleObject(threadHandle, INFINITE));
}

bool AbstractThread::isActive() {
	return active;
}

AbstractThread::~AbstractThread() {
	kill();
}

AbstractThread::AbstractThread() {

	//--- Create a new thread in suspended state!
	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) _threadFunc, (LPVOID) this, CREATE_SUSPENDED,
	(LPDWORD) &threadId);

	active = false;
}
