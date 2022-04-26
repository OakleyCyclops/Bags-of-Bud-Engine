#include "CorePCH.hpp"

static CVar hrt_textMode("hrt_textMode", "0", CVAR_CORE || CVAR_INIT || CVAR_BOOL, "If true, launches the engine in \"Text Mode\" Renderer is never initialized");
static CVar hrt_globalFPSCap("hrt_globalFPSCap", "1000", CVAR_CORE || CVAR_INTEGER, "Global framerate cap across all threads");


/*
=========
Heart::Init
Everything basically starts here
=========
*/
void Heart::Init(int argc, const char** argv)
{   
    // The Console
    console.Init();
    
    // The Shell
    consoleShell.Init();
    consoleShell.Print("Initializing Engine...\n");

    // Input System
    inputSystem.Init();

    // Quit command
    auto cmdQuit = []
    {
        Heart& heart = Singleton<Heart>::GetInstance();
        heart.Shutdown();
    };
    
    static Cmd quit("quit", cmdQuit, CMD_CORE, "quits the game");
}

/*
=========
Heart::Shutdown
=========
*/
void Heart::Shutdown()
{
    consoleShell.Print("============ Shutting Down ============\n");

    inputSystem.Shutdown();
    console.Shutdown();
    consoleShell.Shutdown();

    SDL_Quit();

    Sys_Quit();
}

/*
=========
Heart::Tick
=========
*/
void Heart::Tick()
{

}