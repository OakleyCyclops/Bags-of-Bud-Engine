/*
=====================
INTFheart.hpp
=====================
*/

#ifndef __INTF_HEART_HPP__
#define __INTF_HEART_HPP__

// TODO: Make this less messy
#include <stdio.h>

class INTFheart
{
	public:
		virtual void				Init(int argc, const char* const* argv) = 0;
		virtual void				Shutdown() = 0;

		virtual void				Tick() = 0;
};


#endif /* !__INTF_HEART_HPP__ */