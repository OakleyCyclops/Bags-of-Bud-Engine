#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

class ConsoleShell;

class Console final : public INTFconsole
{
    public:
    
        void Init() override;
        void Shutdown() override;
        
        void Register(CVar* cvar) override;
        void Register(Cmd* cmd) override;

        void Unregister(CVar* cvar) override;
        void Unregister(Cmd* cmd) override;

        //  Getter Functions
        CVar* FindCVar(const char* cvarName) override;
        Cmd*  FindCmd(const char* cmdName) override;

    private:
        inline static LinkedList      registeredCVars;
        inline static LinkedList      registeredCmds;

};

typedef enum
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
} cvarFlags_t;

class CVar final
{
    public:
        // Never use the default constructor.
        CVar()
        {
            assert(typeid(this) != typeid(CVar));
        }

        // Always use one of the following constructors.
	    CVar(const char* Name, char* Value, int Flags, const char* Description);

        const char*     GetName() const;
        const char*     GetValue() const;
        const char*     GetDescription() const;

        int             GetFlags() const;

        bool            GetBool() const;
        int             GetInteger() const;
        float           GetFloat() const;

        void            SetBool(bool value);
        void            SetInteger(int value);
        void            SetFloat(float value);



    protected:
    	const char* 			Name;					// Name
	    char* 			        Value;					// Value
	    const char* 			Description;			// Description
        int                     Flags;                  // Flags

        int                     IntegerValue;           // Gets it's value from atoi(string)
        float                   FloatValue;             // Gets it's value from atof(value)
};

typedef enum
{
	CMD_ALL				    = -1,
	CMD_CHEAT			    = BIT( 0 ),	// command is considered a cheat
	CMD_CORE			    = BIT( 1 ),	// core command
	CMD_RENDERER			= BIT( 2 ),	// renderer command
	CMD_SOUND			    = BIT( 3 ),	// sound command
	CMD_GAME				= BIT( 4 ),	// game command
	CMD_TOOL				= BIT( 5 )	// tool command
} cmdFlags_t;

class Cmd final
{
    public:
        // Never use the default constructor.
        Cmd()
        {
            assert(typeid(this) != typeid(Cmd));
        }

        Cmd(const char* Name, void(*FunctionPointer)(), const char* Arguments, int Flags, const char* Description);
        Cmd(const char* Name, void(*FunctionPointer)(), int Flags, const char* Description);  

        const char*     GetName() const;
        const char*     GetArguments() const;
        const char*     GetDescription() const;
        
        int             GetFlags() const;

        // Not really sure how you specify a function pointer return type
        // But i can just use void* instead i think
        funcPtr         GetFunctionPointer();

    protected:
    	const char* 			Name;					// Name
	    const char* 			Arguments;				// Arguments
	    const char* 			Description;			// Description

        int                     Flags;                  // Flags

        funcPtr FunctionPointer;                        // Function pointer

};

#endif /* !__CONSOLE_HPP__ */