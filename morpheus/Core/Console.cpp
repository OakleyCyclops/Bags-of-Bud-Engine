#include "CorePCH.hpp"
#include <unistd.h>

/*
=========
Console::Init
Everything basically starts here
=========
*/
void Console::Init()
{

}

void Console::Shutdown()
{

}

/*
==================
Console::PrintF
Both client and server can use this, and it will output to the appropriate place.
A raw string should NEVER be passed as fmt, because of "%f" type crashers.
==================
*/
void Console::PrintF(const char* fmt, ...)
{
	va_list argptr;
	va_start( argptr, fmt );
	vprintf(fmt, argptr);
	va_end( argptr );
}