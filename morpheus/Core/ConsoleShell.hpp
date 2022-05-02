#ifndef __CONSOLE_SHELL_HPP__
#define __CONSOLE_SHELL_HPP__

/*
======================================================================
The console is strictly for development and advanced users. It should
	never be used to convey actual game information to the user 
			which should always be done through a GUI.
======================================================================
*/

class ConsoleShell final : public INTFconsoleShell
{
    public:

        void            Init() override;
        void            Update() override;
        void            Shutdown() override;

        void            Input() override;
        void            Output() override;

		// Terminal Input support if the engine was executed from a terminal
		void			TerminalInit() override;
		void			TerminalShutdown() override;

        void            TerminalInput() override;
        void            TerminalOutput(String* input) override;

        void            Print(const char* fmt, ...) override;
        void            Print(short pair, const char* fmt, ...) override;
        void            Print(short pair, unsigned int attr, const char* fmt, ...) override;

        void            Success(const char* msg, ...) override;
        void            Warning(const char* msg, ...) override;   
        void            Error(const char* msg, ...) override;
        void            FatalError(const char* msg, ...) override;
    private:
        inline static String input;   
};

/*
===============================================================
The terminal allows you to have a maximum of 8 different colors
so these macros down here are just for displaying the console
inside the terminal and what not
===============================================================
*/

// Pairs
#define PAIR_PROMPT_SIGN            1   // ']' <- for this thing that prefixes the edit field
#define PAIR_ERROR_MSG              2   // For messages prefixed with "ERROR: "
#define PAIR_FATALERROR_MSG         3   // For messages prefixed with "FATAL ERROR: "
#define PAIR_WARNING_MSG            4   // For messages prefixed with "WARNING: "
#define PAIR_SUCCESS_MSG            5   // For messages prefixed with "SUCCESS: "

// Extra pairs cuz why not
#define PAIR_EXTRA_1                6
#define PAIR_EXTRA_2                7
#define PAIR_EXTRA_3                8

// Pair colors and attributes
#define PROMPT_SIGN_COLOR           COLOR_WHITE, -1
#define PROMP_SIGN_ATTR             A_DIM

#define ERROR_MSG_COLOR             COLOR_RED, -1
#define ERROR_MSG_ATTR              A_NORMAL

#define FATALERROR_MSG_COLOR        COLOR_RED, -1
#define FATALERROR_MSG_ATTR         A_BOLD

#define WARNING_MSG_COLOR           COLOR_YELLOW, -1
#define WARNING_MSG_ATTR            A_NORMAL

#define SUCCESS_MSG_COLOR           COLOR_GREEN, -1
#define SUCCESS_MSG_ATTR            A_NORMAL

#define EXTRA_COLOR_1               -1, -1
#define EXTRA_ATTR_1                -1

#define EXTRA_COLOR_2               -1, -1
#define EXTRA_ATTR_2                -1

#define EXTRA_COLOR_3               -1, -1
#define EXTRA_ATTR_3                -1

// String constants
inline const char* CON_PROMPT_SIGN     =   "] ";
inline const char* CON_ERROR_MSG       =   "ERROR: ";
inline const char* CON_FATALERROR_MSG  =   "FATAL ERROR: ";
inline const char* CON_WARN_MSG        =   "WARNING: ";
inline const char* CON_SUCCESS_MSG     =   "SUCCESS: ";


#endif /* !__CONSOLE_SHELL_HPP__ */