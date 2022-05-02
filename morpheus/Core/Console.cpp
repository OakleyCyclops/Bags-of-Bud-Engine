#include "CorePCH.hpp"

/*
==================
Console::Init
==================
*/
void Console::Init()
{

}

/*
==================
Console::Shutdown
==================
*/
void Console::Shutdown()
{
    ConsoleShell& consoleShell = Singleton<ConsoleShell>::GetInstance();

    consoleShell.Print("Shutting down console");
}

/*
==================
Console::Register(CVar* cvar)
==================
*/
void Console::Register(CVar* cvar)
{
    Node* cvarNode = new Node(cvar->GetName(), cvar);

    this->registeredCVars.Append(cvarNode);
}

/*
==================
Console::Register(Cmd* cmd)
==================
*/
void Console::Register(Cmd* cmd)
{
    Node* cmdNode = new Node(cmd->GetName(), cmd);

    this->registeredCmds.Append(cmdNode);  
}

/*
==================
Console::Unregister(CVar* cvar)
==================
*/
void Console::Unregister(CVar* cvar)
{

}

/*
==================
Console::Unregister(Cmd* cmd)
==================
*/
void Console::Unregister(Cmd* cmd)
{
    
}

/*
==================
Console::FindCVar(const char* cvarName)
==================
*/
CVar* Console::FindCVar(const char* cvarName)
{
    if (registeredCVars.Search(cvarName) != nullptr)
    {   
        CVar* ptr = (CVar*)registeredCVars.Search(cvarName)->GetData();
        return ptr;
    }
    else
    {
        return nullptr;
    }
}

/*
==================
Console::FindCmd(const char* cmdName)
==================
*/
Cmd* Console::FindCmd(const char* cmdName)
{
    if (registeredCmds.Search(cmdName) != nullptr)
    {
        Cmd* ptr = (Cmd*)registeredCmds.Search(cmdName)->GetData();
        return ptr;
    }
    else
    {
        return nullptr;
    }
}

/*
===========
CVar::CVar
===========
*/
CVar::CVar(const char* Name, char* Value, int Flags, const char* Description)
{
    Console& console = Singleton<Console>::GetInstance();

    this->Name = Name;
    this->Value = Value;
    this->Flags = Flags | CVAR_STATIC;
    this->Description = Description;

    console.Register(this);
}

/*
===========
Cmd::Cmd
===========
*/
Cmd::Cmd(const char* Name, funcPtr FunctionPointer, const char* Arguments, int Flags, const char* Description)
{
    Console& console = Singleton<Console>::GetInstance();

    this->Name = Name;
    this->FunctionPointer = FunctionPointer;
    this->Arguments = Arguments;
    this->Flags = Flags;
    this->Description = Description;
    
    console.Register(this);
}

/*
===========
Cmd::Cmd
===========
*/
Cmd::Cmd(const char* Name, funcPtr FunctionPointer, int Flags, const char* Description)
{
    Console& console = Singleton<Console>::GetInstance();

    this->Name = Name;
    this->FunctionPointer = FunctionPointer;
    this->Flags = Flags;
    this->Description = Description;

    console.Register(this);
}

/*
===========
Cvar::GetName
===========
*/
const char* CVar::GetName() const
{
    return this->Name;
}

/*
===========
Cvar::GetBool
===========
*/
bool CVar::GetBool() const
{
    if (atoi(this->Value))
    {
        return true;
    }

    else
    {
        return false;
    }
}

/*
===========
Cvar::GetInteger
===========
*/
int CVar::GetInteger() const
{
    if (isdigit(atoi(this->Value)))
    {
        return atoi(this->Value);
    }

    else
    {
        return 0;
    }
}

/*
===========
Cvar::GetFloat
===========
*/
float CVar::GetFloat() const
{
    if (atof(this->Value))
    {
        return atof(this->Value);
    }

    else
    {
        return 0.0f;
    }
}

/*
===========
CVar::SetBool(const bool value)
===========
*/
void CVar::SetBool(const bool value)
{
    if (value)
    {
        char yes = '1';
        this->Value = &yes;
        this->IntegerValue = 1;
    }

    else if (!value)
    {
        char no = '0';
        this->Value = &no;
        this->IntegerValue = 0;
    }
}

/*
===========
CVar::SetInteger(int value)
===========
*/
void CVar::SetInteger(int value)
{
    sprintf(this->Value, "%i", value);
}

/*
===========
CVar::SetFloat(const float value)
===========
*/
void CVar::SetFloat(const float value)
{
    sprintf(this->Value, "%f", value);
}

/*
===========
Cvar::GetName
===========
*/
const char* Cmd::GetName() const
{
    return this->Name;
}

/*
===========
Cmd::GetFunctionPointer
===========
*/
funcPtr Cmd::GetFunctionPointer()
{
    return this->FunctionPointer;
}