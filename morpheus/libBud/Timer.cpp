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

#include "libBudPCH.hpp"
#pragma hdrstop

double budTimer::base = -1.0;


/*
=================
budTimer::InitBaseClockTicks
=================
*/
void budTimer::InitBaseClockTicks() const
{
	budTimer timer;
	double ct, b;
	int i;
	
	base = 0.0;
	b = -1.0;
	for( i = 0; i < 1000; i++ )
	{
		timer.Clear();
		timer.Start();
		timer.Stop();
		ct = timer.ClockTicks();
		if( b < 0.0 || ct < b )
		{
			b = ct;
		}
	}
	base = b;
}


/*
=================
budTimerReport::budTimerReport
=================
*/
budTimerReport::budTimerReport()
{
}

/*
=================
budTimerReport::SetReportName
=================
*/
void budTimerReport::SetReportName( const char* name )
{
	reportName = ( name ) ? name : "Timer Report";
}

/*
=================
budTimerReport::~budTimerReport
=================
*/
budTimerReport::~budTimerReport()
{
	Clear();
}

/*
=================
budTimerReport::AddReport
=================
*/
int budTimerReport::AddReport( const char* name )
{
	if( name && *name )
	{
		names.Append( name );
		return timers.Append( new( TAG_LIBBUD ) budTimer() );
	}
	return -1;
}

/*
=================
budTimerReport::Clear
=================
*/
void budTimerReport::Clear()
{
	timers.DeleteContents( true );
	names.Clear();
	reportName.Clear();
}

/*
=================
budTimerReport::Reset
=================
*/
void budTimerReport::Reset()
{
	assert( timers.Num() == names.Num() );
	for( int i = 0; i < timers.Num(); i++ )
	{
		timers[i]->Clear();
	}
}

/*
=================
budTimerReport::AddTime
=================
*/
void budTimerReport::AddTime( const char* name, budTimer* time )
{
	assert( timers.Num() == names.Num() );
	int i;
	for( i = 0; i < names.Num(); i++ )
	{
		if( names[i].Icmp( name ) == 0 )
		{
			*timers[i] += *time;
			break;
		}
	}
	if( i == names.Num() )
	{
		int index = AddReport( name );
		if( index >= 0 )
		{
			timers[index]->Clear();
			*timers[index] += *time;
		}
	}
}

/*
=================
budTimerReport::PrintReport
=================
*/
void budTimerReport::PrintReport()
{
	assert( timers.Num() == names.Num() );
	libBud::common->Printf( "Timing Report for %s\n", reportName.c_str() );
	libBud::common->Printf( "-------------------------------\n" );
	float total = 0.0f;
	for( int i = 0; i < names.Num(); i++ )
	{
		libBud::common->Printf( "%s consumed %5.2f seconds\n", names[i].c_str(), timers[i]->Milliseconds() * 0.001f );
		total += timers[i]->Milliseconds();
	}
	libBud::common->Printf( "Total time for report %s was %5.2f\n\n", reportName.c_str(), total * 0.001f );
}
