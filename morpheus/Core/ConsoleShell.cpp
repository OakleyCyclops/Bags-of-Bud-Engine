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
				
			input.Append(buffer);
			input.StripTrailing('\n');

			// Call TerminalOutput if we press enter
			if (buffer == '\n')
			{
				Print(CON_PROMPT_SIGN);
				Print(input.c_str());
				Print("\n");

				TerminalOutput(&input);

				// Clear both the char and string
				input.Clear();
			}
			// VOILA
		}
		else
		{
			return;
		}

	}

	else
	{
		return;
	}
}	

void ConsoleShell::TerminalOutput(String* input)
{
	int i = input->Find(' ', 0, -1) + 1;
	
	String* arg = new String;
	String* strip = new String;

	// Split up the string between the Cmd/CVar and the argument
    while(i <= input->Length())
    {
		if (input->c_str()[i] != ' ')
		{
			arg->Append(input->c_str()[i]);
		}
        i++;
    }

	strip->Append(' ');
	strip->Append(arg->c_str());

	input->StripTrailingOnce(strip->c_str());

	delete strip;


	if (!Console::Exec(input, arg))
	{
		Error("Unknown CVar/Command '%s'\n", input->c_str());
	}

	else
	{
		Console::Exec(input, arg);
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
		printf(ANSI_RST);

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
		printf(ANSI_RST);

		va_list argptr;
		va_start(argptr, msg);
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
		printf(ANSI_YEL);
		printf(CON_WARN_MSG);
		printf(ANSI_RST);

		va_list argptr;
		va_start(argptr, msg);
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
		printf(ANSI_RED);
		printf(CON_ERROR_MSG);
		printf(ANSI_RST);

		va_list argptr;
		va_start(argptr, msg);
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
		printf(ANSI_BRED);
		printf(CON_FATALERROR_MSG);
		printf(ANSI_RST);

		va_list argptr;
		va_start(argptr, msg);
		vprintf(msg, argptr);
		va_end(argptr);
	}
}

#ifdef USING_UNIX

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

		if(tcgetattr(0, &old) < 0)
		{
			perror("tcsetattr()");
		}

    	old.c_lflag &= ~ICANON;
    	old.c_lflag &= ~ECHO;
    	old.c_cc[VMIN] = 1;
    	old.c_cc[VTIME] = 0;

    	if(tcsetattr(0, TCSANOW, &old) < 0)
		{
        	perror("tcsetattr ICANON");
		}

    	if(read(0, &buf, 1) < 0)
		{
			perror("read()");
		}

    	old.c_lflag |= ICANON;
    	old.c_lflag |= ECHO;

    	if(tcsetattr(0, TCSADRAIN, &old) < 0)
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