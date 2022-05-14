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
		TerminalInit();
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
		TerminalShutdown();
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
	initscr();
	noecho();
	nodelay(stdscr, true);
	cbreak();
	scrollok(stdscr, true);
	keypad(stdscr, true);

	start_color();
	use_default_colors();

	init_pair(PAIR_PROMPT_SIGN, PROMPT_SIGN_COLOR);
	init_pair(PAIR_ERROR_MSG, ERROR_MSG_COLOR);

	init_pair(PAIR_FATALERROR_MSG, FATALERROR_MSG_COLOR);
	init_pair(PAIR_WARNING_MSG, WARNING_MSG_COLOR);

	init_pair(PAIR_SUCCESS_MSG, SUCCESS_MSG_COLOR);
	init_pair(PAIR_EXTRA_1, EXTRA_COLOR_1);

	init_pair(PAIR_EXTRA_2, EXTRA_COLOR_2);
	init_pair(PAIR_EXTRA_3, EXTRA_COLOR_3);
}

void ConsoleShell::TerminalShutdown()
{
	endwin();
}

void ConsoleShell::TerminalInput()
{
	if (con_terminalSupport.GetBool())
	{
		// Performing some magic so it doesn't wait for user input
		const char buffer = getch();
	
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

		Print(PAIR_PROMPT_SIGN, PROMP_SIGN_ATTR, CON_PROMPT_SIGN);
		
		if (input->Length() == 0)
		{
			Print(" \n");
			return;
		}
		else
		{
			Print(input->c_str());
			Print(" \n");

			console.Exec(input);
		}

		if (!console.Exec(input))
		{
			Error("Unknown CVar/Command '%s'\n", input->c_str());
		}
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

void ConsoleShell::Print(short pair, const char* fmt, ...)
{
	if (con_terminalSupport.GetBool() && isTerminal)
	{
		va_list argptr;
		va_start(argptr, fmt);

		attron(COLOR_PAIR(pair));
		vw_printw(stdscr, fmt, argptr);
		attroff(COLOR_PAIR(pair));

		va_end(argptr);
	}

	Update();
}

void ConsoleShell::Print(short pair, unsigned int attr, const char* fmt, ...)
{
	if (con_terminalSupport.GetBool() && isTerminal)
	{	
		va_list argptr;
		va_start(argptr, fmt);

		attron(COLOR_PAIR(pair) | attr);
		vw_printw(stdscr, fmt, argptr);
		attroff(COLOR_PAIR(pair) | attr);

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
	if (con_terminalSupport.GetBool() && isTerminal)
	{
		Print(PAIR_SUCCESS_MSG, SUCCESS_MSG_ATTR, CON_SUCCESS_MSG);

		va_list argptr;
		va_start(argptr, msg);
		vw_printw(stdscr, msg, argptr);
		va_end(argptr);
	}
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
		Print(PAIR_WARNING_MSG, WARNING_MSG_ATTR, CON_WARN_MSG);

		va_list argptr;
		va_start(argptr, msg);
		vw_printw(stdscr, msg, argptr);
		va_end(argptr);
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
		Print(PAIR_ERROR_MSG, ERROR_MSG_ATTR, CON_ERROR_MSG);

		va_list argptr;
		va_start(argptr, msg);
		vw_printw(stdscr, msg, argptr);
		va_end(argptr);
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
		Print(PAIR_FATALERROR_MSG, FATALERROR_MSG_ATTR, CON_FATALERROR_MSG);

		va_list argptr;
		va_start(argptr, msg);
		vw_printw(stdscr, msg, argptr);
		va_end(argptr);
	}
}
