/*
===============================================================================
	The console is strictly for development and advanced users. It should
	never be used to convey actual game information to the user, which should
	always be done through a GUI.
===============================================================================
*/

#ifndef __INTF_CONSOLE_HPP__
#define __INTF_CONSOLE_HPP__

class INTFconsole
{
	public:
	
		virtual void			Init() = 0;
		virtual void			Shutdown() = 0;

		virtual void            PrintF(const char* fmt, ...) = 0;

};

#endif /* !__INTF_CONSOLE_HPP__ */