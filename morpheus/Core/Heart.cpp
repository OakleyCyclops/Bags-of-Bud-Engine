#include "CorePCH.hpp"

/*
=========
Heart::Init
Everything basically starts here
=========
*/
void Heart::Init(int argc, const char* const* argv)
{   
    // Initialize only a few of the SDL subsystems First
    // Video will be done once the game DLL is loaded
    SDL_InitSubSystem(SDL_INIT_TIMER);

    // The Console
    console.Init();
    
    // The Shell
    consoleShell.Init();
    consoleShell.PrintF("Initializing Engine...\n");

    // Input System
    inputSystem.Init();

}

/*
=========
Heart::Shutdown
=========
*/
void Heart::Shutdown()
{
    
}

/*
=========
Heart::Tick
=========
*/
void Heart::Tick()
{
    consoleShell.Update();

}