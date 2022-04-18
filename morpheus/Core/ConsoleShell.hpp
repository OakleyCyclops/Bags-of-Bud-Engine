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
        friend class			Console;

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

        void            PrintF(const char* fmt, ...) override;
        
};

#endif /* !__CONSOLE_SHELL_HPP__ */