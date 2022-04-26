#include "CorePCH.hpp"

static CVar con_terminalSupport("con_terminalSupport", "1", CVAR_CORE || CVAR_BOOL || CVAR_INIT, "Enables/Disables the usage of the Terminal/Command Prompt as the engine's developer console");

/*
=========
ConsoleShell::Init
Everything basically starts here
=========
*/
void ConsoleShell::Init()
{
	if (con_terminalSupport.GetBool() && isTerminal)
	{
		initscr();
		noecho();
		cbreak();
	}

    auto cmdClear = []
    {
        return;
    };
    auto cmdDump = []
    {
        return;
    };
    static CVar con_notifyTime("con_notifyTime", "3", CVAR_CORE, "time messages are displayed onscreen when console is pulled up");
    static CVar con_noPrint("con_noPrint", "1", CVAR_CORE, "print on the console but not onscreen when console is pulled up");
    static Cmd clear("clear", cmdClear, CMD_CORE, "clears the console");
    static Cmd conDump("conDump", cmdDump, CMD_CORE, "dumps the console text to a file");

	Print("Console Initialized\n");
}

void ConsoleShell::Update()
{
	if (con_terminalSupport.GetBool() && isTerminal)
	{
		refresh();
	}
}

void ConsoleShell::Shutdown()
{
	Print("Shutting down console shell");

	if (con_terminalSupport.GetBool() && isTerminal)
	{
		endwin();
	}
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
		char buffer;
		String input;
	
		buffer = getch();
		input.Append(buffer);
		input.StripTrailingWhitespace();

		if (buffer == '\n')
		{
			TerminalOutput(&input);
		}
		
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

		Print(prefix);
		
		if (console.FindCmd(input->c_str()))
		{

		}

		else if (console.FindCVar(input->c_str()))
		{

		}
		else
		{
			Print("Unknown CVar/Command '%s'\n", input->c_str());
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
void ConsoleShell::Print(const char* fmt, ...)
{
	if (con_terminalSupport.GetBool() && isTerminal)
	{
		va_list argptr;
		va_start(argptr, fmt);
		vw_printw(stdscr, fmt, argptr);
		va_end(argptr);
	}

	Update();
}