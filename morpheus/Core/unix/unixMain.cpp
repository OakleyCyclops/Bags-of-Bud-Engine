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
// #include "coreLocal.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <pwd.h>
#include <pthread.h>
#include <dlfcn.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <dirent.h>

// RB begin
#include <sys/statvfs.h>
// RB end

#include "unixPublic.hpp"

#define					MAX_OSPATH 256
#define					COMMAND_HISTORY 64

static int				input_hide = 0;

static char				input_ret[256];

static int				history_count = 0;			// buffer fill up
static int				history_start = 0;			// current history start
static int				history_current = 0;		// goes back in history

static const char** 	cmdargv = NULL;
static int 				cmdargc = 0;

// exit - quit - error --------------------------------------------------------

static int set_exit = 0;
static char exit_spawn[1024];



/*
===============
main
===============
*/
int main(int argc, const char** argv)
{
	Heart& heart = Singleton<Heart>::GetInstance();

	cmdargc = argc;
	cmdargv = argv;

	// Let there be light...
	if (argc > 1)
	{
		heart.Init(argc - 1, &argv[1]);
	}

	else
	{
		heart.Init(0, NULL);
	}

	while (1)
	{
		heart.Tick();	
	}
}

/*
================
Posix_Exit
================
*/
void Posix_Exit( int ret )
{
	// at this point, too late to catch signals
	Posix_ClearSigs();
	
	// process spawning. it's best when it happens after everything has shut down
	if( exit_spawn[0] )
	{
		Sys_DoStartProcess( exit_spawn, false );
	}
	// in case of signal, handler tries a common->Quit
	// we use set_exit to maintain a correct exit code
	if( set_exit )
	{
		exit( set_exit );
	}
	exit( ret );
}

/*
================
Posix_SetExit
================
*/
void Posix_SetExit( int ret )
{
	set_exit = 0;
}

/*
================
Sys_Quit
================
*/
void Sys_Quit()
{
	Posix_Exit( EXIT_SUCCESS );
}

/*
===============
Sys_Shutdown
===============
*/
void Sys_Shutdown()
{
	Posix_Shutdown();
}

/*
===============
Sys_FPE_handler
===============
*/
void Sys_FPE_handler( int signum, siginfo_t* info, void* context )
{
	assert( signum == SIGFPE );
	Sys_Printf( "FPE\n" );
}

/*
===============
Sys_GetClockticks
===============
*/
double Sys_GetClockTicks()
{
// RB begin
	struct timespec now;
	
	clock_gettime( CLOCK_MONOTONIC, &now );
	
	return now.tv_sec * 1000000000LL + now.tv_nsec;
// RB end
}

/*
===============
MeasureClockTicks
===============
*/
double MeasureClockTicks()
{
	double t0, t1;
	
	t0 = Sys_GetClockTicks( );
	Sys_Sleep( 1000 );
	t1 = Sys_GetClockTicks( );
	return t1 - t0;
}

/*
================
Sys_Milliseconds
================
*/
/* base time in seconds, that's our origin
   timeval:tv_sec is an int:
   assuming this wraps every 0x7fffffff - ~68 years since the Epoch (1970) - we're safe till 2038
   using unsigned long data type to work right with Sys_XTimeToSysTime */

#ifdef CLOCK_MONOTONIC_RAW
// use RAW monotonic clock if available (=> not subject to NTP etc)
#define D3_CLOCK_TO_USE CLOCK_MONOTONIC_RAW
#else
#define D3_CLOCK_TO_USE CLOCK_MONOTONIC
#endif

// RB: changed long to int
unsigned int sys_timeBase = 0;
// RB end
/* current time in ms, using sys_timeBase as origin
   NOTE: sys_timeBase*1000 + curtime -> ms since the Epoch
     0x7fffffff ms - ~24 days
		 or is it 48 days? the specs say int, but maybe it's casted from unsigned int?
*/
int Sys_Milliseconds()
{
	// DG: use clock_gettime on all platforms
	int curtime;
	struct timespec ts;
	
	clock_gettime( D3_CLOCK_TO_USE, &ts );
	
	if( !sys_timeBase )
	{
		sys_timeBase = ts.tv_sec;
		return ts.tv_nsec / 1000000;
	}
	
	curtime = ( ts.tv_sec - sys_timeBase ) * 1000 + ts.tv_nsec / 1000000;
	
	return curtime;
}


/*
================
Sys_Microseconds
================
*/
static uint64 sys_microTimeBase = 0;

uint64 Sys_Microseconds()
{
	uint64 curtime;
	struct timespec ts;
	
	clock_gettime( D3_CLOCK_TO_USE, &ts );
	
	if( !sys_microTimeBase )
	{
		sys_microTimeBase = ts.tv_sec;
		return ts.tv_nsec / 1000;
	}
	
	curtime = ( ts.tv_sec - sys_microTimeBase ) * 1000000 + ts.tv_nsec / 1000;
	
	return curtime;
}


/*
================
Sys_Mkdir
================
*/
void Sys_Mkdir( const char* path )
{
	mkdir(path, 0777);
}

/*
================
Sys_Rmdir
================
*/
bool Sys_Rmdir( const char* path )
{
	return (rmdir( path) == 0 );
}

/*
========================
Sys_IsFileWritable
========================
*/
bool Sys_IsFileWritable( const char* path )
{
	struct stat st;
	if( stat( path, &st ) == -1 )
	{
		return true;
	}
	
	return ( st.st_mode & S_IWRITE ) != 0;
}

/*
================
Posix_Cwd
================
*/
const char* Posix_Cwd()
{
	static char cwd[MAX_OSPATH];
	
	getcwd( cwd, sizeof( cwd ) - 1 );
	cwd[MAX_OSPATH - 1] = 0;
	
	return cwd;
}

/*
=================
Sys_Init
Posix_EarlyInit/Posix_LateInit is better
=================
*/
void Sys_Init() { }

/*
=================
Posix_Shutdown
=================
*/
void Posix_Shutdown()
{

}

/*
=================
Sys_SO_Load
TODO: OSX - use the native API instead? NSModule
=================
*/
// RB: 64 bit fixes, changed int to intptr_t
intptr_t Sys_SO_Load(const char* path)
{
	void* handle = dlopen( path, RTLD_NOW );
	if( !handle )
	{
		Sys_Printf( "dlopen '%s' failed: %s\n", path, dlerror() );
	}
	
	return ( intptr_t )handle;
}
// RB end

/*
=================
Sys_SO_GetProcAddress
=================
*/
// RB: 64 bit fixes, changed int to intptr_t
void* Sys_SO_GetProcAddress( intptr_t handle, const char* sym )
{
// RB end
	const char* error;
	void* ret = dlsym( ( void* )handle, sym );
	if( ( error = dlerror() ) != NULL )
	{
		Sys_Printf( "dlsym '%s' failed: %s\n", sym, error );
	}
	return ret;
}

/*
=================
Sys_SO_Unload
=================
*/
void Sys_SO_Unload( intptr_t handle )
{
	dlclose( ( void* )handle );
}

/*
================
Sys_ShowConsole
================
*/
void Sys_ShowConsole( int visLevel, bool quitOnClose ) { }

// ---------------------------------------------------------------------------

// only relevant when specified on command line
const char* Sys_DefaultCDPath()
{
	return "";
}

ID_TIME_T Sys_FileTimeStamp( budFileHandle fp )
{
	struct stat st;
	fstat( fileno( fp ), &st );
	return st.st_mtime;
}

void Sys_Sleep( int msec )
{
	// RB begin
	if( usleep( msec * 1000 ) == -1 )
		Sys_Printf( "usleep: %s\n", strerror( errno ) );
}

// stub pretty much everywhere - heavy calling
void Sys_FlushCacheMemory( void* base, int bytes )
{
//  Sys_Printf("Sys_FlushCacheMemory stub\n");
}

/*
================
Sys_LockMemory
================
*/
bool Sys_LockMemory( void* ptr, int bytes )
{
	return true;
}

/*
================
Sys_UnlockMemory
================
*/
bool Sys_UnlockMemory( void* ptr, int bytes )
{
	return true;
}

// RB begin

/*
================
Sys_GetDriveFreeSpace
returns in megabytes
================
*/
int Sys_GetDriveFreeSpace( const char* path )
{
	int ret = 26;
	
	struct statvfs st;
	
	if( statvfs( path, &st ) == 0 )
	{
		unsigned long blocksize = st.f_bsize;
		unsigned long freeblocks = st.f_bfree;
		
		unsigned long free = blocksize * freeblocks;
		
		ret = ( double )( free ) / ( 1024.0 * 1024.0 );
	}
	
	return ret;
}

/*
========================
Sys_GetDriveFreeSpaceInBytes
========================
*/
int64 Sys_GetDriveFreeSpaceInBytes( const char* path )
{
	int64 ret = 1;
	
	struct statvfs st;
	
	if( statvfs( path, &st ) == 0 )
	{
		unsigned long blocksize = st.f_bsize;
		unsigned long freeblocks = st.f_bfree;
		
		unsigned long free = blocksize * freeblocks;
		
		ret = free;
	}
	
	return ret;
}

// RB end

/*
================
Sys_AlreadyRunning
return true if there is a copy of D3 running already
================
*/
bool Sys_AlreadyRunning()
{
	return false;
}

/*
===============
Posix_EarlyInit
===============
*/
void Posix_EarlyInit()
{
	exit_spawn[0] = '\0';
	Posix_InitSigs();
	
	// set the base time
	Sys_Milliseconds();
	
}

/*
===============
low level output
===============
*/

void Sys_DebugPrintf( const char* fmt, ... )
{
	va_list argptr;
	
	va_start( argptr, fmt );
	vprintf( fmt, argptr );
	va_end( argptr );
}

void Sys_DebugVPrintf( const char* fmt, va_list arg )
{
	vprintf( fmt, arg );
}

void Sys_Printf( const char* fmt, ... )
{
	va_list argptr;
	
	va_start( argptr, fmt );
	vprintf( fmt, argptr );
	va_end( argptr );
}

void Sys_VPrintf( const char* fmt, va_list arg )
{
	vprintf( fmt, arg );
}

/*
================
Sys_Error
================
*/
void Sys_Error( const char* error, ... )
{
	va_list argptr;
	
	Sys_Printf( "Sys_Error: " );
	va_start( argptr, error );
	Sys_DebugVPrintf( error, argptr );
	va_end( argptr );
	Sys_Printf( "\n" );
	
	Posix_Exit( EXIT_FAILURE );
}


