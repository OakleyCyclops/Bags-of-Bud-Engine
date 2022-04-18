#ifndef __INTF_CONSOLE_HPP__
#define __INTF_CONSOLE_HPP__

class CVar;
class Cmd;

class INTFconsole
{
    public:
        friend class INTFconsoleShell;

        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        virtual void Register(CVar* cvar) = 0;
        virtual void Register(Cmd* cmd) = 0;

        virtual void Unregister(CVar* cvar) = 0;
        virtual void Unregister(Cmd* cmd) = 0;

        //  Getter Functions
        virtual CVar* FindCVar(const char* cvarName) = 0;
        virtual Cmd*  FindCmd(const char* cmdName) = 0;
};

#endif /* !__INTF_CONSOLE_HPP__ */