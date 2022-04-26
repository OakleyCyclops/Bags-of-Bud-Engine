#include "CorePCH.hpp"

void Console::Init()
{

}

void Console::Shutdown()
{
    ConsoleShell& consoleShell = Singleton<ConsoleShell>::GetInstance();

    consoleShell.Print("Shutting down console");
}

void Console::Register(CVar* cvar)
{
    Node* cvarNode = new Node(cvar->GetName(), cvar);

    this->registeredCVars.Append(cvarNode);
}

void Console::Register(Cmd* cmd)
{
    Node* cmdNode = new Node(cmd->GetName(), cmd);

    this->registeredCmds.Append(cmdNode);  
}

void Console::Unregister(CVar* cvar)
{

}

void Console::Unregister(Cmd* cmd)
{
    
}

void* Console::FindCVar(const char* cvarName)
{
    if (registeredCVars.Search(cvarName) != nullptr)
    {   
        void* ptr = registeredCVars.Search(cvarName)->GetData();
        return ptr;
    }
    else
    {
        return nullptr;
    }
}

void* Console::FindCmd(const char* cmdName)
{
    if (registeredCmds.Search(cmdName) != nullptr)
    {
        void* ptr = registeredCmds.Search(cmdName)->GetData();
        return ptr;
    }
    else
    {
        return nullptr;
    }
}

CVar::CVar(const char* Name, char* Value, int Flags, const char* Description)
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

void CVar::SetInteger(int value)
{
    sprintf(this->Value, "%i", value);
}

void CVar::SetFloat(const float value)
{
    sprintf(this->Value, "%f", value);
}

const char* Cmd::GetName() const
{
    return this->Name;
}