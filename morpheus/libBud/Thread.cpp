/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#pragma hdrstop
#include "libBudPCH.hpp"

/*
================================================================================================
Contains the vartious ThreadingClass implementations.
================================================================================================
*/

/*
================================================================================================

	budSysThread

================================================================================================
*/

/*
========================
budSysThread::budSysThread
========================
*/
budSysThread::budSysThread() :
	threadHandle( 0 ),
	isWorker( false ),
	isRunning( false ),
	isTerminating( false ),
	moreWorkToDo( false ),
	signalWorkerDone( true )
{
}

/*
========================
budSysThread::~budSysThread
========================
*/
budSysThread::~budSysThread()
{
	StopThread( true );
	if( threadHandle )
	{
		Sys_DestroyThread( threadHandle );
	}
}

/*
========================
budSysThread::StartThread
========================
*/
bool budSysThread::StartThread( const char* name_, core_t core, xthreadPriority priority, int stackSize )
{
	if( isRunning )
	{
		return false;
	}
	
	name = name_;
	
	isTerminating = false;
	
	if( threadHandle )
	{
		Sys_DestroyThread( threadHandle );
	}
	
	threadHandle = Sys_CreateThread( ( xthread_t )ThreadProc, this, priority, name, core, stackSize, false );
	
	isRunning = true;
	return true;
}

/*
========================
budSysThread::StartWorkerThread
========================
*/
bool budSysThread::StartWorkerThread( const char* name_, core_t core, xthreadPriority priority, int stackSize )
{
	if( isRunning )
	{
		return false;
	}
	
	isWorker = true;
	
	bool result = StartThread( name_, core, priority, stackSize );
	
	signalWorkerDone.Wait( budSysSignal::WAIT_INFINITE );
	
	return result;
}

/*
========================
budSysThread::StopThread
========================
*/
void budSysThread::StopThread( bool wait )
{
	if( !isRunning )
	{
		return;
	}
	if( isWorker )
	{
		signalMutex.Lock();
		moreWorkToDo = true;
		signalWorkerDone.Clear();
		isTerminating = true;
		signalMoreWorkToDo.Raise();
		signalMutex.Unlock();
	}
	else
	{
		isTerminating = true;
	}
	if( wait )
	{
		WaitForThread();
	}
}

/*
========================
budSysThread::WaitForThread
========================
*/
void budSysThread::WaitForThread()
{
	if( isWorker )
	{
		signalWorkerDone.Wait( budSysSignal::WAIT_INFINITE );
	}
	else if( isRunning )
	{
		Sys_DestroyThread( threadHandle );
		threadHandle = 0;
	}
}

/*
========================
budSysThread::SignalWork
========================
*/
void budSysThread::SignalWork()
{
	if( isWorker )
	{
		signalMutex.Lock();
		moreWorkToDo = true;
		signalWorkerDone.Clear();
		signalMoreWorkToDo.Raise();
		signalMutex.Unlock();
	}
}

/*
========================
budSysThread::IsWorkDone
========================
*/
bool budSysThread::IsWorkDone()
{
	if( isWorker )
	{
		// a timeout of 0 will return immediately with true if signaled
		if( signalWorkerDone.Wait( 0 ) )
		{
			return true;
		}
	}
	return false;
}

/*
========================
budSysThread::ThreadProc
========================
*/
int budSysThread::ThreadProc( budSysThread* thread )
{
	int retVal = 0;
	
	try
	{
		if( thread->isWorker )
		{
			for( ; ; )
			{
				thread->signalMutex.Lock();
				if( thread->moreWorkToDo )
				{
					thread->moreWorkToDo = false;
					thread->signalMoreWorkToDo.Clear();
					thread->signalMutex.Unlock();
				}
				else
				{
					thread->signalWorkerDone.Raise();
					thread->signalMutex.Unlock();
					thread->signalMoreWorkToDo.Wait( budSysSignal::WAIT_INFINITE );
					continue;
				}
				
				if( thread->isTerminating )
				{
					break;
				}
				
				retVal = thread->Run();
			}
			thread->signalWorkerDone.Raise();
		}
		else
		{
			retVal = thread->Run();
		}
	}
	catch( idException& ex )
	{
		libBud::Warning( "Fatal error in thread %s: %s", thread->GetName(), ex.GetError() );
		
		// We don't handle threads terminating unexpectedly very well, so just terminate the whole process
		exit( 0 );
	}
	
	thread->isRunning = false;
	
	return retVal;
}

/*
========================
budSysThread::Run
========================
*/
int budSysThread::Run()
{
	// The Run() is not pure virtual because on destruction of a derived class
	// the virtual function pointer will be set to NULL before the budSysThread
	// destructor actually stops the thread.
	return 0;
}

/*
================================================================================================

	test

================================================================================================
*/

/*
================================================
idMyThread test class.
================================================
*/
class idMyThread : public budSysThread
{
public:
	virtual int Run()
	{
		// run threaded code here
		return 0;
	}
	// specify thread data here
};

/*
========================
TestThread
========================
*/
void TestThread()
{
	idMyThread thread;
	thread.StartThread( "myThread", CORE_ANY );
}

/*
========================
TestWorkers
========================
*/
void TestWorkers()
{
	budSysWorkerThreadGroup<idMyThread> workers( "myWorkers", 4 );
	for( ; ; )
	{
		for( int i = 0; i < workers.GetNumThreads(); i++ )
		{
			// workers.GetThread( i )-> // setup work for this thread
		}
		workers.SignalWorkAndWait();
	}
}
