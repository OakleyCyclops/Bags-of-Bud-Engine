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

    if (cvar->Flags & CVAR_INTEGER)
    {
        cvar->ValueString = CharMethods::IntToString(*(int*)cvar->Value, cvar->ValueString, 10);  
    }

    else if (cvar->Flags & CVAR_FLOAT)
    {
        cvar->ValueString = CharMethods::FloatToString(*(float*)cvar->Value, cvar->ValueString, 2, 10);  
    }

    else if (cvar->Flags & CVAR_BOOL)
    {
        if (*(bool*)&cvar->Value == true)
        {
            cvar->ValueString = "1";
        }

        else
        {
            cvar->ValueString = "0";
        }
    }


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

bool Console::Exec(String* input, String* arg)
{
    Cmd* cmd;
    CVar* cvar;

    if (cmd = FindCmd(input->Cstring))
    {
        cmd->FunctionPointer();
        return true;
    }

	else if (cvar = FindCVar(input->Cstring))
	{
        if (arg->Length != 0)
        {
            if (cvar->Flags & CVAR_FLOAT)
            {
                float f = strtof(arg->Cstring, nullptr);
                cvar->Value = (float*)&f;

                cvar->ValueString = (char*)arg->Cstring;
            }

            else if (cvar->Flags & CVAR_INTEGER)
            {
                int i = strtol(arg->Cstring, nullptr, 0);
                cvar->Value = (int*)&i;

                cvar->ValueString = (char*)arg->Cstring;
            }

            else if (cvar->Flags & CVAR_BOOL)
            {
                int i = strtol(arg->Cstring, nullptr, 0);
                
                if (i > 0)
                {
                    cvar->Value = (bool*)true;

                }

                else
                {
                    cvar->Value = (bool*)false;
                }

                cvar->ValueString = (char*)arg->Cstring; 
            }
        }

        else
        {
            ConsoleShell::Print(cvar->Name, " ", "is ", "\"", cvar->ValueString, "\"\n", "Enter \"help ", cvar->Name, "\"", "for more info");
        }

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