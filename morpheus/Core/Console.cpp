#include "CorePCH.hpp"

void Console::Init()
{

    auto cmdClear = [](void)
    {

    };
    auto cmdDump = [](void)
    {

    };
    
    static CVar con_notifyTime("con_notifyTime", "3", CVAR_CORE, "time messages are displayed onscreen when console is pulled up");
    static CVar con_noPrint("con_noPrint", "1", CVAR_CORE, "print on the console but not onscreen when console is pulled up");
    static Cmd clear("clear", cmdClear, CMD_CORE, "clears the console");
    static Cmd conDump("conDump", cmdDump, CMD_CORE, "dumps the console text to a file");
}

void Console::Shutdown()
{

}

void Console::Register(CVar* cvar)
{
    static Node cvarNode(cvar->GetName(), cvar);

    this->registeredCVars.Append(&cvarNode);
}

void Console::Register(Cmd* cmd)
{
    static Node cmdNode(cmd->GetName(), cmd);

    this->registeredCmds.Append(&cmdNode);  
}

void Console::Unregister(CVar* cvar)
{

}

void Console::Unregister(Cmd* cmd)
{
    
}

CVar* Console::FindCVar(const char* cvarName)
{

}

Cmd* Console::FindCmd(const char* cmdName)
{

}

CVar::CVar(const char* Name, const char* Value, int Flags, const char* Description)
{
    Console& console = Singleton<Console>::GetInstance();

    this->Name = Name;
    this->Value = Value;
    this->Flags = Flags | CVAR_STATIC;
    this->Description = Description;

    console.Register(this);
}

Cmd::Cmd(const char* Name, void(*FunctionPointer)(), const char* Arguments, int Flags, const char* Description)
{
    Console& console = Singleton<Console>::GetInstance();

    this->Name = Name;
    this->FunctionPointer = FunctionPointer;
    this->Arguments = Arguments;
    this->Flags = Flags;
    this->Description = Description;
    
    console.Register(this);
}

Cmd::Cmd(const char* Name, void(*FunctionPointer)(), int Flags, const char* Description)
{
    Console& console = Singleton<Console>::GetInstance();

    this->Name = Name;
    this->FunctionPointer = FunctionPointer;
    this->Flags = Flags;
    this->Description = Description;

    console.Register(this);
}

const char* CVar::GetName() const
{
    return this->Name;
}

const char* Cmd::GetName() const
{
    return this->Name;
}