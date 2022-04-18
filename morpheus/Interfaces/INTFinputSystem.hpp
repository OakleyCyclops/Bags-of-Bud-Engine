#ifndef __INTF_INPUT_SYSTEM_HPP__
#define __INTF_INPUT_SYSTEM_HPP__


class INTFinputSystem
{
    public:
	    virtual void			Init() = 0;
		virtual void			Update() = 0;
	    virtual void			Shutdown() = 0;
};


#endif /* !__INTF_EVENT_SYSTEM_HPP__ */