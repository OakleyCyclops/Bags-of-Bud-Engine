#ifndef __CONSOLE_SHELL_HPP__
#define __CONSOLE_SHELL_HPP__

/*
======================================================================
The console is strictly for development and advanced users. It should
	never be used to convey actual game information to the user 
			which should always be done through a GUI.
======================================================================
*/

#include "Console.hpp"

namespace ConsoleShell
{

        void            Init();
        void            RegisterCVarsAndCmds();

        void            Update();
        void            Shutdown();

        void            Input();
        void            Output();

		// Terminal Input support if the engine was executed from a terminal
		void			TerminalInit();
		void			TerminalShutdown();

        void            TerminalInput();
        void            TerminalOutput(String* input);

        void            Print(const char* fmt, ...);
        void            Print(short pair, const char* fmt, ...);
        void            Print(short pair, unsigned int attr, const char* fmt, ...);

        void            Success(const char* msg, ...);
        void            Warning(const char* msg, ...);   
        void            Error(const char* msg, ...);
        void            FatalError(const char* msg, ...);

        inline static   String input;

        inline auto cmdClear = []
        {
            return;
        };

        inline auto cmdDump = []
        {
            return;
        };

        inline CVar con_terminalSupport = {"con_terminalSupport", new bool(1), "Enables/Disables the usage of the Terminal/Command Prompt as the engine's developer console", CVAR_BOOL | CVAR_CORE | CVAR_INIT};
        inline CVar con_notifyTime = {"con_notifyTime", new int(3), "time messages are displayed onscreen when console is pulled up", CVAR_INTEGER | CVAR_CORE};
        inline CVar con_noPrint = {"con_noPrint", new bool(0), "print on the console but not onscreen when console is pulled up", CVAR_BOOL | CVAR_CORE};

        inline Cmd clear = {"clear", cmdClear, "clears the console", CMD_CORE};
        inline Cmd conDump = {"conDump", cmdDump, "dumps the console text to a file", CMD_CORE};


        #ifdef USING_UNIX

            // Because conio.h isn't a thing in the GNU C library and NCurses fucking sucks

	        // Thank u :) https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux 
            char    GetChar();
            // And thank u :) https://www.flipcode.com/archives/_kbhit_for_Linux.shtml
            int     KbhHit();

        #endif 
}
/*
===============================================================
The terminal allows you to have a maximum of 8 different colors
so these macros down here are just for displaying the console
inside the terminal and what not
===============================================================
*/

/*
================
ANSI Color Codes
================
*/

// Regular

#define ANSI_BLK            "\e[0;30m"
#define ANSI_RED            "\e[0;31m"
#define ANSI_GRN            "\e[0;32m"
#define ANSI_YEL            "\e[0;33m"
#define ANSI_BLU            "\e[0;34m"
#define ANSI_MAG            "\e[0;35m"
#define ANSI_CYN            "\e[0;36m"
#define ANSI_WHT            "\e[0;37m"

// Bold

#define ANSI_BBLK           "\e[1;30m"
#define ANSI_BRED           "\e[1;31m"
#define ANSI_BGRN           "\e[1;32m"
#define ANSI_BYEL           "\e[1;33m"
#define ANSI_BBLU           "\e[1;34m"
#define ANSI_BMAG           "\e[1;35m"
#define ANSI_BCYN           "\e[1;36m"
#define ANSI_BWHT           "\e[1;37m"

// Reset

#define ANSI_RST            "\e[0m"

/*
========================
Console Message prefixes
========================
*/

#define CON_PROMPT_SIGN     "] "
#define CON_ERROR_MSG       "ERROR: "
#define CON_FATALERROR_MSG  "FATAL ERROR: "
#define CON_WARN_MSG        "WARNING: "
#define CON_SUCCESS_MSG     "SUCCESS: "

#endif /* !__CONSOLE_SHELL_HPP__ */