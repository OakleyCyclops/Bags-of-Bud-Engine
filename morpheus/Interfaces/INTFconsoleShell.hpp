/*
===============================================================================
	The console is strictly for development and advanced users. It should
	never be used to convey actual game information to the user, which should
	always be done through a GUI.
===============================================================================
*/

#ifndef __INTF_CONSOLE_SHELL_HPP__
#define __INTF_CONSOLE_SHELL_HPP__

class String;

class INTFconsoleShell
{
	public:
		
		friend class			INTFconsole;

		virtual void			Init() = 0;
		virtual void			Update() = 0;
		virtual void			Shutdown() = 0;

		virtual void			Input() = 0;
		virtual void			Output() = 0;

		// Terminal Input support if the engine was executed from a terminal
		virtual void			TerminalInit() = 0;
		virtual void			TerminalShutdown() = 0;

		virtual void			TerminalInput() = 0;
		virtual void			TerminalOutput(String* input) = 0;
		
		virtual void            Print(const char* fmt, ...) = 0;

};

#endif /* !__INTF_CONSOLE_SHELL_HPP__ */