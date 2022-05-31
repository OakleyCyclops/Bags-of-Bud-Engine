#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

namespace Console
{
    void Init();
	void RegisterCVarsAndCmds();

    void Shutdown();
        
    void Register(CVar* cvar);
    void Register(Cmd* cmd);

    void Unregister(CVar* cvar);
    void Unregister(Cmd* cmd);
        
    bool Exec(String* input);

    CVar* FindCVar(const char* cvarName);
    Cmd*  FindCmd(const char* cmdName);

    inline static LinkedList registeredCVars;
    inline static LinkedList registeredCmds;
};

typedef enum CVarFlags
{
	CVAR_ALL				= -1,		// all flags
	CVAR_BOOL				= BIT(0),	// variable is a boolean
	CVAR_INTEGER			= BIT(1),	// variable is an integer
	CVAR_FLOAT				= BIT(2),	// variable is a float
	CVAR_CORE				= BIT(3),	// core variable
	CVAR_RENDERER			= BIT(4),	// renderer variable
	CVAR_SOUND				= BIT(5),	// sound variable
	CVAR_GUI				= BIT(6),	// gui variable
	CVAR_GAME				= BIT(7),	// game variable
	CVAR_TOOL				= BIT(8),	// tool variable
	CVAR_SERVERINFO			= BIT(10),  // sent from servers, available to menu
	CVAR_NETWORKSYNC		= BIT(11),	// cvar is synced from the server to clients
	CVAR_STATIC				= BIT(12),	// statically declared, not user created
	CVAR_CHEAT				= BIT(13),	// variable is considered a cheat
	CVAR_NOCHEAT			= BIT(14),	// variable is not considered a cheat
	CVAR_INIT				= BIT(15),	// can only be set from the command-line
	CVAR_ROM				= BIT(16),	// display only, cannot be set by user at all
	CVAR_ARCHIVE			= BIT(17),	// set to cause it to be saved to a config file
	CVAR_MODIFIED			= BIT(18)	// set when the variable is modified
};

typedef enum CmdFlags
{
	CMD_ALL				    = -1,
	CMD_CHEAT               = BIT(0),	// command is considered a cheat
	CMD_CORE                = BIT(1),	// core command
	CMD_RENDERER            = BIT(2),	// renderer command
	CMD_SOUND               = BIT(3),	// sound command
	CMD_GAME			    = BIT(4),	// game command
	CMD_TOOL				= BIT(5)	// tool command
};

struct CVar
{
	// Everything needs to be null by default otherwise we run into segfaults lol
    const char* 			Name = nullptr;					// Name
	void*             		Value = nullptr;				// Value
	const char* 			Description = nullptr;			// Description
    int                     Flags = NULL;                  	// Flags
};

struct Cmd 
{
	// Everything needs to be null by default otherwise we run into segfaults lol
    const char* 			Name = nullptr;					// Name
    funcPtr                 FunctionPointer = nullptr;		// Function pointer
	const char* 			Description = nullptr;			// Description
    int                     Flags = NULL;                  	// Flags

};

#endif /* !__CONSOLE_HPP__ */