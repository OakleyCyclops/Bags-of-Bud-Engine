/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2012 Robert Beckebans

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#include "CorePCH.hpp"
#include "unixPublic.hpp"
//#include "coreLocal.hpp"
//#include "local.h"

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

/*
==============
Sys_EXEPath
==============
*/
const char* Sys_EXEPath()
{

}

/*
===============
Sys_GetProcessorId
===============
*/
cpuid_t Sys_GetProcessorId()
{
	return CPUID_GENERIC;
}

/*
===============
Sys_GetProcessorString
===============
*/
const char* Sys_GetProcessorString()
{
	return "generic";
}

/*
===============
Sys_ClockTicksPerSecond
===============
*/
double Sys_ClockTicksPerSecond()
{

}

/*
========================
Sys_CPUCount

numLogicalCPUCores	- the number of logical CPU per core
numPhysicalCPUCores	- the total number of cores per package
numCPUPackages		- the total number of packages (physical processors)
========================
*/
void Sys_CPUCount( int& numLogicalCPUCores, int& numPhysicalCPUCores, int& numCPUPackages )
{

}

/*
==================
Sys_DoStartProcess
if we don't fork, this function never returns
the no-fork lets you keep the terminal when you're about to spawn an installer

if the command contains spaces, system() is used. Otherwise the more straightforward execl ( system() blows though )
==================
*/
void Sys_DoStartProcess( const char* exeName, bool dofork )
{
	bool use_system = false;
	if( strchr( exeName, ' ' ) )
	{
		use_system = true;
	}
	else
	{
		// set exec rights when it's about a single file to execute
		struct stat buf;
		if( stat( exeName, &buf ) == -1 )
		{
			printf( "stat %s failed: %s\n", exeName, strerror( errno ) );
		}
		else
		{
			if( chmod( exeName, buf.st_mode | S_IXUSR ) == -1 )
			{
				printf( "cmod +x %s failed: %s\n", exeName, strerror( errno ) );
			}
		}
	}
	if( dofork )
	{
		switch( fork() )
		{
			case -1:
				// main thread
				break;
			case 0:
				if( use_system )
				{
					printf( "system %s\n", exeName );
					system( exeName );
					_exit( 0 );
				}
				else
				{
					printf( "execl %s\n", exeName );
					execl( exeName, exeName, NULL );
					printf( "execl failed: %s\n", strerror( errno ) );
					_exit( -1 );
				}
				break;
		}
	}
	else
	{
		if( use_system )
		{
			printf( "system %s\n", exeName );
			system( exeName );
			sleep( 1 );	// on some systems I've seen that starting the new process and exiting this one should not be too close
		}
		else
		{
			printf( "execl %s\n", exeName );
			execl( exeName, exeName, NULL );
			printf( "execl failed: %s\n", strerror( errno ) );
		}
		// terminate
		_exit( 0 );
	}
}

/*
 ==================
 Sys_DoPreferences
 ==================
 */
void Sys_DoPreferences() { }

/*
========================
Sys_GetCmdLine
========================
*/
const char* Sys_GetCmdLine()
{
	// DG: don't use this, use cmdargv and cmdargc instead!
	return "TODO Sys_GetCmdLine";
}

/*
========================
Sys_ReLaunch
========================
*/
void Sys_ReLaunch()
{

}
