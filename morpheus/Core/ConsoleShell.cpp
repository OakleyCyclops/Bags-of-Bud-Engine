#include "CorePCH.hpp"
#include <unistd.h>


/*
=========
ConsoleShell::Init
Everything basically starts here
=========
*/
void ConsoleShell::Init()
{

}

void ConsoleShell::Update()
{
	TerminalInput();
}

void ConsoleShell::Shutdown()
{

}

void ConsoleShell::Input()
{

}

void ConsoleShell::Output()
{

}

void ConsoleShell::TerminalInit()
{

}

void ConsoleShell::TerminalShutdown()
{
	
}

void ConsoleShell::TerminalInput()
{
	char buffer[MAX_PRINT_MSG];
	String input;

	scanf("%s", buffer);
	input.Append(buffer);
	input.StripTrailingWhitespace();

	TerminalOutput(&input);
}	

void ConsoleShell::TerminalOutput(String* input)
{
	// TODO: Make this actually process commands and shit
	char prefix[3] = "] ";

	PrintF(prefix);
	PrintF("Unknown CVar/Command '%s'\n", input->c_str());
}

/*
==================
ConsoleShell::PrintF
Both client and server can use this, and it will output to the appropriate place.
A raw string should NEVER be passed as fmt, because of "%f" type crashers.
==================
*/
void ConsoleShell::PrintF(const char* fmt, ...)
{
	va_list argptr;
	va_start( argptr, fmt );
	vprintf(fmt, argptr);
	va_end( argptr );
}