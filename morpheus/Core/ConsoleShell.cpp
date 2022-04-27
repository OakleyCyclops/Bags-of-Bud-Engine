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
		nodelay(stdscr, true);
		cbreak();
		scrollok(stdscr, true);
		keypad(stdscr, true);

		if (has_colors)
		{
			start_color();
			use_default_colors();
		}
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
		TerminalInput();
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
		// Performing some magic so it doesn't wait for user input
		static char buffer;
		static String input;
	
		buffer = getch();
	
		if (buffer == KEY_MOUSE)
		{
			return;
		}

		if (buffer == ERR)
		{
			// Returns and continues regular operation if the user has pressed no keys within this cycle
			return;
		}

		input.Append(buffer);
		input.StripTrailing('\n');

		mvaddstr(getcury(stdscr), 0, input.c_str());

		// Call TerminalOutput if we press enter
		if (buffer == '\n')
		{
			move(getcury(stdscr), 0);
			clrtoeol();
			TerminalOutput(&input);

			// Clear both the char and string
			memset(&buffer, 0, sizeof(buffer));
			input.Clear();
		}
		// voila
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

		init_pair(1, COLOR_WHITE, -1);

		attron(COLOR_PAIR(1));
		attron(A_DIM);
		Print(prefix);
		attroff(COLOR_PAIR(1));
		attroff(A_DIM);
		
		if (input->Length() == 0)
		{
			Print(" \n");
			return;
		}
		else
		{
			Print(input->c_str());
			Print(" \n");

			if (console.FindCmd(input->c_str()))
			{

			}

			else if (console.FindCVar(input->c_str()))
			{

			}
			else
			{
				Error("Unknown CVar/Command '%s'\n", input->c_str());
			}
		}

	}

	else
	{
		return;
	}

	
}

/*
==================
ConsoleShell::Print
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

/*
==================
ConsoleShell::Success
==================
*/
void ConsoleShell::Success(const char* msg, ...)
{

}

/*
==================
ConsoleShell::Warning
==================
*/
void ConsoleShell::Warning(const char* msg, ...)
{
	if (con_terminalSupport.GetBool() && isTerminal)
	{
		if (has_colors)
		{
			init_pair(1, COLOR_YELLOW, -1);

			attron(COLOR_PAIR(1));
			Print("WARNING: ");
			attroff(COLOR_PAIR(1));

			va_list argptr;
			va_start(argptr, msg);
			vw_printw(stdscr, msg, argptr);
			va_end(argptr);
		}
		else
		{
			Print("WARNING: ");

			va_list argptr;
			va_start(argptr, msg);
			vw_printw(stdscr, msg, argptr);
			va_end(argptr);
		}
	}
}

/*
==================
ConsoleShell::Error
==================
*/
void ConsoleShell::Error(const char* msg, ...)
{
	if (con_terminalSupport.GetBool() && isTerminal)
	{
		if (has_colors)
		{
			init_pair(2, COLOR_RED, -1);
			attron(COLOR_PAIR(2));

			Print("ERROR: ");
			attroff(COLOR_PAIR(2));

			va_list argptr;
			va_start(argptr, msg);
			vw_printw(stdscr, msg, argptr);
			va_end(argptr);
		}
		else
		{
			Print("ERROR: ");

			va_list argptr;
			va_start(argptr, msg);
			vw_printw(stdscr, msg, argptr);
			va_end(argptr);
		}
	}
}

/*
==================
ConsoleShell::FatalError
==================
*/
void ConsoleShell::FatalError(const char* msg, ...)
{
	if (con_terminalSupport.GetBool() && isTerminal)
	{
		if (has_colors)
		{
			init_pair(3, COLOR_RED, -1);
			attron(A_BOLD);
			attron(COLOR_PAIR(3));

			Print("FATAL ERROR: ");

			attroff(COLOR_PAIR(3));
			attroff(A_BOLD);

			va_list argptr;
			va_start(argptr, msg);
			vw_printw(stdscr, msg, argptr);
			va_end(argptr);
		}
		else
		{
			attron(A_BOLD);
			Print("FATAL ERROR: ");
			attroff(A_BOLD);

			va_list argptr;
			va_start(argptr, msg);
			vw_printw(stdscr, msg, argptr);
			va_end(argptr);
		}
	}
}