/*
=====================
INTFcommon.hpp
=====================
*/

#ifndef __INTF_COMMON_HPP__
#define __INTF_COMMON_HPP__

// TODO: Make this less messy
#include <stdio.h>
#include "../libBud/Macros.hpp"

class INTFcommon
{
public:
    // Initialize everything.
	// if the OS allows, pass argc/argv directly (without executable name)
	// otherwise pass the command line in a single string (without executable name)
	virtual void				Init( int argc, const char* const* argv, const char* cmdline ) = 0;

    // Shuts down everything.
	virtual void				Shutdown() = 0;
	virtual bool				IsShuttingDown() const = 0;
	virtual void				Quit() = 0;

    // Returns true if common initialization is complete.
	virtual bool				IsInitialized() const = 0;

    // Called repeatedly as the foreground thread for rendering and game logic.
	virtual void				Frame() = 0;

    	// Prints message to the console, which may cause a screen update if com_refreshOnPrint is set.
	virtual void				Printf( VERIFY_FORMAT_STRING const char* fmt, ... ) = 0;
	
	// Same as Printf, with a more usable API - Printf pipes to this.
	virtual void				VPrintf( const char* fmt, va_list arg ) = 0;
	
	// Prints message that only shows up if the "developer" cvar is set,
	// and NEVER forces a screen update, which could cause reentrancy problems.
	virtual void				DPrintf( VERIFY_FORMAT_STRING const char* fmt, ... ) = 0;
	
	// Prints WARNING %s message and adds the warning message to a queue for printing later on.
	virtual void				Warning( VERIFY_FORMAT_STRING const char* fmt, ... ) = 0;
	
	// Prints WARNING %s message in yellow that only shows up if the "developer" cvar is set.
	virtual void				DWarning( VERIFY_FORMAT_STRING const char* fmt, ... ) = 0;
	
	// Prints all queued warnings.
	virtual void				PrintWarnings() = 0;
	
	// Removes all queued warnings.
	virtual void				ClearWarnings( const char* reason ) = 0;
	
	// Issues a C++ throw. Normal errors just abort to the game loop,
	// which is appropriate for media or dynamic logic errors.
	virtual void				Error( VERIFY_FORMAT_STRING const char* fmt, ... ) ID_INSTANCE_ATTRIBUTE_PRINTF( 1, 2 ) = 0;
	
	// Fatal errors quit all the way to a system dialog box, which is appropriate for
	// static internal errors or cases where the system may be corrupted.
	virtual void                FatalError( VERIFY_FORMAT_STRING const char* fmt, ... ) ID_INSTANCE_ATTRIBUTE_PRINTF( 1, 2 ) = 0;
};




#endif /* !__INTF_COMMON_HPP__ */