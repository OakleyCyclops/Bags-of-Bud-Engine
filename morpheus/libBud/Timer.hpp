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

#ifndef __TIMER_H__
#define __TIMER_H__

/*
===============================================================================

	Clock tick counter. Should only be used for profiling.

===============================================================================
*/

class budTimer
{
public:
	budTimer();
	budTimer( double clockTicks );
	~budTimer();
	
	budTimer			operator+( const budTimer& t ) const;
	budTimer			operator-( const budTimer& t ) const;
	budTimer& 		operator+=( const budTimer& t );
	budTimer& 		operator-=( const budTimer& t );
	
	void			Start();
	void			Stop();
	void			Clear();
	double			ClockTicks() const;
	double			Milliseconds() const;
	
private:
	static double	base;
	enum
	{
		TS_STARTED,
		TS_STOPPED
	} state;
	double			start;
	double			clockTicks;
	
	void			InitBaseClockTicks() const;
};

/*
=================
budTimer::budTimer
=================
*/
BUD_INLINE budTimer::budTimer()
{
	state = TS_STOPPED;
	clockTicks = 0.0;
}

/*
=================
budTimer::budTimer
=================
*/
BUD_INLINE budTimer::budTimer( double _clockTicks )
{
	state = TS_STOPPED;
	clockTicks = _clockTicks;
}

/*
=================
budTimer::~budTimer
=================
*/
BUD_INLINE budTimer::~budTimer()
{
}

/*
=================
budTimer::operator+
=================
*/
BUD_INLINE budTimer budTimer::operator+( const budTimer& t ) const
{
	assert( state == TS_STOPPED && t.state == TS_STOPPED );
	return budTimer( clockTicks + t.clockTicks );
}

/*
=================
budTimer::operator-
=================
*/
BUD_INLINE budTimer budTimer::operator-( const budTimer& t ) const
{
	assert( state == TS_STOPPED && t.state == TS_STOPPED );
	return budTimer( clockTicks - t.clockTicks );
}

/*
=================
budTimer::operator+=
=================
*/
BUD_INLINE budTimer& budTimer::operator+=( const budTimer& t )
{
	assert( state == TS_STOPPED && t.state == TS_STOPPED );
	clockTicks += t.clockTicks;
	return *this;
}

/*
=================
budTimer::operator-=
=================
*/
BUD_INLINE budTimer& budTimer::operator-=( const budTimer& t )
{
	assert( state == TS_STOPPED && t.state == TS_STOPPED );
	clockTicks -= t.clockTicks;
	return *this;
}

/*
=================
budTimer::Start
=================
*/
BUD_INLINE void budTimer::Start()
{
	assert( state == TS_STOPPED );
	state = TS_STARTED;
	start = libBud::sys->GetClockTicks();
}

/*
=================
budTimer::Stop
=================
*/
BUD_INLINE void budTimer::Stop()
{
	assert( state == TS_STARTED );
	clockTicks += libBud::sys->GetClockTicks() - start;
	if( base < 0.0 )
	{
		InitBaseClockTicks();
	}
	if( clockTicks > base )
	{
		clockTicks -= base;
	}
	state = TS_STOPPED;
}

/*
=================
budTimer::Clear
=================
*/
BUD_INLINE void budTimer::Clear()
{
	clockTicks = 0.0;
}

/*
=================
budTimer::ClockTicks
=================
*/
BUD_INLINE double budTimer::ClockTicks() const
{
	assert( state == TS_STOPPED );
	return clockTicks;
}

/*
=================
budTimer::Milliseconds
=================
*/
BUD_INLINE double budTimer::Milliseconds() const
{
	assert( state == TS_STOPPED );
	return clockTicks / ( libBud::sys->ClockTicksPerSecond() * 0.001 );
}


/*
===============================================================================

	Report of multiple named timers.

===============================================================================
*/

class budTimerReport
{
public:
	budTimerReport();
	~budTimerReport();
	
	void			SetReportName( const char* name );
	int				AddReport( const char* name );
	void			Clear();
	void			Reset();
	void			PrintReport();
	void			AddTime( const char* name, budTimer* time );
	
private:
	budList<budTimer*>timers;
	budStrList		names;
	budStr			reportName;
};

#endif /* !__TIMER_H__ */
