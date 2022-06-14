#include "CorePCH.hpp"


/*
=========
ConsoleShell::Init
Everything basically starts here
=========
*/
void ConsoleShell::Init()
{
	// Register the Heart's CVars and Cmds
    RegisterCVarsAndCmds();

	if (con_terminalSupport.Value && isTerminal)
	{
		TerminalInit();
	}
}

/*
=========
ConsoleShell::RegisterCVarsAndCmds
=========
*/
void ConsoleShell::RegisterCVarsAndCmds()
{
	Console::Register(&con_terminalSupport);
	Console::Register(&con_notifyTime);
	Console::Register(&con_noPrint);

	Console::Register(&clear);
	Console::Register(&conDump);
	Console::Register(&help);
}

/*
=========
Command functions
=========
*/
void ConsoleShell::cmdClear()
{
	return;
}

void ConsoleShell::cmdDump()
{
	return;
}

void ConsoleShell::cmdHelp()
{
	CVar* cvar;
	Cmd* cmd;

	const char* typeStr = "Type:\n";
	const char* usageStr = "Usage:\n";
	const char* descStr = "Description:\n";

	if (arg.Length != 0)
	{
		if (cmd = Console::FindCmd(arg.Cstring))
		{	
			// "Type:"
			Print("\n");

			Print(ANSI_YEL);
			Print(typeStr);

			Print("\t");

			Print("Console command\n");
				
			if (cmd->Flags & CMD_CHEAT)
			{
				Print("\t");
				Print("Cheat\n");
			}

			// "Usage:"
			Print("\n");

			Print(ANSI_YEL);
			Print(usageStr);
			Print(ANSI_RST);

			Print("\t");
				
			Print(cmd->Name);

			Print(ANSI_BLU);
			Print(" <String>\"\n");
			printf(ANSI_RST);

			// "Description:"
			Print("\n");
				
			Print(ANSI_YEL);
			Print(descStr);
			Print(ANSI_RST);

			Print("\t");
			Print("\"");
			Print(cmd->Description);
			Print("\"");

			Print("\n");
		}

		else if (cvar = Console::FindCVar(arg.Cstring))
		{	
			// "Type:"
			Print("\n");

			Print(ANSI_UYEL);
			Print(typeStr);
			Print(ANSI_RST);

			Print("\t");

			Print("Console variable\n");
				
			if (cvar->Flags & CVAR_CHEAT)
			{
				Print("\t");
				Print("Cheat\n");
			}

			// "Usage:"
			Print("\n");

			Print(ANSI_UYEL);
			Print(usageStr);
			Print(ANSI_RST);

			Print("\t");
			
			Print("\"");
			Print(cvar->Name);
			Print(" ");

			if (cvar->Flags & CVAR_INTEGER)
			{
				Print(ANSI_UBLU);
				Print("<Int>");
				Print(ANSI_RST);
			}

			else if (cvar->Flags & CVAR_FLOAT)
			{
				Print(ANSI_UBLU);
				Print("<Float>");
				Print(ANSI_RST);
			}

			else if (cvar->Flags & CVAR_BOOL)
			{
				Print(ANSI_UBLU);
				Print("<Bool>");
				Print(ANSI_RST);
			}
			Print("\"\n");	

			// "Description:"
			Print("\n");
				
			Print(ANSI_UYEL);
			Print(descStr);
			Print(ANSI_RST);


			// TODO: No clue why but for some reason
			// the description doesn't print 
			// and this function runs twice 
			// for no apparent reason

			Print("\t");
			Print("\"");
			Print(cvar->Description);
			Print("\"");

			Print("\n");
		}
	}
}

/*
=========
ConsoleShell::Update
=========
*/
void ConsoleShell::Update()
{
	if (con_terminalSupport.Value && isTerminal)
	{
		TerminalInput();
	}
}

/*
=========
ConsoleShell::Shutdown
=========
*/
void ConsoleShell::Shutdown()
{
	Print("ConsoleShell::Shutdown();\n");

	if (con_terminalSupport.Value && isTerminal)
	{
		TerminalShutdown();
	}
}

/*
=========
ConsoleShell::Input
=========
*/
void ConsoleShell::Input()
{

}

/*
=========
ConsoleShell::Output
=========
*/
void ConsoleShell::Output()
{

}

/*
=========
ConsoleShell::TerminalInit
=========
*/
void ConsoleShell::TerminalInit()
{

}

/*
=========
ConsoleShell::TerminalShutdown
=========
*/
void ConsoleShell::TerminalShutdown()
{

}

/*
=========
ConsoleShell::TerminalInput
=========
*/
void ConsoleShell::TerminalInput()
{
	if (con_terminalSupport.Value)
	{
		// FUCKING FINALLY
		if (KbhHit())
		{
			char buffer = GetChar();
			
			if (buffer != '\n' || buffer != '\t')
			{
				StringMethods::Append(&input, buffer);
				StringMethods::StripTrailing(&input, '\n');
			}

			// Call TerminalOutput if we press enter
			if (buffer == '\n')
			{
				Print(CON_PROMPT_SIGN);
				Print(input.Cstring);
				Print("\n");

				TerminalOutput(&input);

				// Clear
				StringMethods::Clear(&input); 
				StringMethods::Clear(&arg); 
			}
			// VOILA
		}

	}
}	

void ConsoleShell::TerminalOutput(String* input)
{
	int i = StringMethods::Find(input, ' ', 0, -1) + 1;
	String* strip = new String;

	// Split up the string between the Cmd/CVar and the argument
    while(i <= input->Length)
    {	
		if (input->Cstring[i] != ' ')
		{
			StringMethods::Append(&arg, input->Cstring[i]);
		}
        i++;
    }

	StringMethods::Append(strip, ' ');
	StringMethods::Append(strip, arg.Cstring);

	StringMethods::StripTrailingOnce(input, strip->Cstring);

	delete strip;

	if (!Console::Exec(input, &arg))
	{
		Error("Unknown CVar/Cmd \"%s\"\n", input->Cstring);
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
	if (con_terminalSupport.Value && isTerminal)
	{
		va_list argptr;
		va_start(argptr, fmt);

		vprintf(fmt, argptr);

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
	if (con_terminalSupport.Value && isTerminal)
	{
		printf(ANSI_GRN);
		printf(CON_SUCCESS_MSG);

		va_list argptr;
		va_start(argptr, msg);

		Print(ANSI_RST);

		vprintf(msg, argptr);
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
	if (con_terminalSupport.Value && isTerminal)
	{
		Print(ANSI_YEL);
		Print(CON_WARN_MSG);

		va_list argptr;
		va_start(argptr, msg);

		Print(ANSI_RST);

		vprintf(msg, argptr);
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
	if (con_terminalSupport.Value && isTerminal)
	{
		Print(ANSI_RED);
		Print(CON_ERROR_MSG);

		va_list argptr;
		va_start(argptr, msg);

		Print(ANSI_RST);

		vprintf(msg, argptr);
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
	if (con_terminalSupport.Value && isTerminal)
	{
		Print(ANSI_BRED);
		Print(CON_FATALERROR_MSG);

		va_list argptr;
		va_start(argptr, msg);

		Print(ANSI_RST);

		vprintf(msg, argptr);
		va_end(argptr);
	}
}

#ifdef BUDPOSIX

	// I hate this
    // Because conio.h isn't a thing in the GNU C library and NCurses fucking sucks

	// Thank u :) https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux 
	// And thank u :) https://www.flipcode.com/archives/_kbhit_for_Linux.shtml

	/*
	==================
	ConsoleShell::GetChar
	==================
	*/
    char ConsoleShell::GetChar()
	{
		char buf = 0;
		struct termios old = {0};
		fflush(stdout);

		if (tcgetattr(0, &old) < 0)
		{
			perror("tcsetattr()");
		}

    	old.c_lflag &= ~ICANON;
    	old.c_lflag &= ~ECHO;
    	old.c_cc[VMIN] = 1;
    	old.c_cc[VTIME] = 0;

    	if (tcsetattr(0, TCSANOW, &old) < 0)
		{
        	perror("tcsetattr ICANON");
		}

    	if (read(0, &buf, 1) < 0)
		{
			perror("read()");
		}

    	old.c_lflag |= ICANON;
    	old.c_lflag |= ECHO;

    	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		{
        	perror("tcsetattr ~ICANON");
		}

		return buf;
	}

	/*
	==================
	ConsoleShell::KbhHit
	==================
	*/
	int ConsoleShell::KbhHit()
	{
		static const int STDIN = 0;
		static bool initialized = false;
		
		if (!initialized) 
		{
			// Use termios to turn off line buffering
			termios term;
			tcgetattr(STDIN, &term);
			term.c_lflag &= ~ICANON;
			tcsetattr(STDIN, TCSANOW, &term);
			setbuf(stdin, NULL);
			initialized = true;
		}

    	int bytesWaiting;
    	ioctl(STDIN, FIONREAD, &bytesWaiting);
    	return bytesWaiting;
	}

#endif 