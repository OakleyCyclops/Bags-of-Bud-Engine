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

static CVar con_terminalSupport("con_terminalSupport", "1", CVAR_CORE || CVAR_BOOL, "Enables/Disables the usage of the Terminal/Command Prompt as the engine's developer console");

void ConsoleShell::Update()
{

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
	if (con_terminalSupport.GetBool())
	{
		char buffer[MAX_PRINT_MSG];
		String input;

		scanf("%s", buffer);
		input.Append(buffer);
		input.StripTrailingWhitespace();

		TerminalOutput(&input);
	}

	else
	{
		return;
	}
}	

void ConsoleShell::TerminalOutput(String* input)
{
	if (con_terminalSupport.GetBool())
	{
		Console& console = Singleton<Console>::GetInstance();

		// TODO: Make this actually process commands and shit
		char prefix[3] = "] ";

		PrintF(prefix);
		
		if (console.FindCmd(input->c_str()))
		{

		}

		else if (console.FindCVar(input->c_str()))
		{

		}
		else
		{
			PrintF("Unknown CVar/Command '%s'\n", input->c_str());
		}

		TerminalInput();
	}

	else
	{
		return;
	}

	
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