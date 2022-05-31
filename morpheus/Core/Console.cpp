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
    ConsoleShell::Print("Shutting down console");
}

/*
==================
Console::Register(CVar* cvar)
==================
*/
void Console::Register(CVar* cvar)
{
    Node* cvarNode = new Node;
    
    cvarNode->Name = cvar->Name;
    cvarNode->Data = cvar;

    linkedList::Append(&registeredCVars, cvarNode);
}

/*
==================
Console::Register(Cmd* cmd)
==================
*/
void Console::Register(Cmd* cmd)
{
    Node* cmdNode = new Node;

    cmdNode->Name = cmd->Name;
    cmdNode->Data = cmd;

    linkedList::Append(&registeredCmds, cmdNode);
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

bool Console::Exec(String* input)
{
    if (FindCmd(input->c_str()))
    {
		Cmd* cmd = FindCmd(input->c_str());
        cmd->FunctionPointer();
        return true;
    }

	else if (FindCVar(input->c_str()))
	{
        return true;
	}
	else
	{
		return false;
	}
}


/*
==================
Console::FindCVar
==================
*/
CVar* Console::FindCVar(const char* cvarName)
{
    if (linkedList::Search(&registeredCVars, cvarName) != nullptr)
    {   
        CVar* ptr = (CVar*)linkedList::Search(&registeredCVars, cvarName)->Data;
        return ptr;
    }
    else
    {
        return nullptr;
    }
}

/*
==================
Console::FindCmd
==================
*/
Cmd* Console::FindCmd(const char* cmdName)
{
    if (linkedList::Search(&registeredCmds, cmdName) != nullptr)
    {   
        Cmd* ptr = (Cmd*)linkedList::Search(&registeredCmds, cmdName)->Data;
        return ptr;
    }
    else
    {
        return nullptr;
    }
}