#include "CorePCH.hpp"

/*
=========
Heart::Init
Everything basically starts here
=========
*/
void Heart::Init(int argc, const char** argv)
{   
    // Prints
    ConsoleShell::Print("============ Initializing Engine ============\n");
    ConsoleShell::Print("Heart::Init(int argc, const char** argv);\n");

    // The Console
    Console::Init();
    ConsoleShell::Print("Console::Init();\n");

    // Register the Heart's CVars and Cmds
    RegisterCVarsAndCmds();

    // The Shell
    ConsoleShell::Init();
    ConsoleShell::Print("ConsoleShell::Init();\n");

    // Input System
    InputSystem::Init();
    ConsoleShell::Print("InputSystem::Init();\n");

    
    // GLFW Window
    Window = glfwCreateWindow(640, 480, "Bags of Bud Engine", nullptr, nullptr);
}

/*
=========
Heart::RegisterCVarsAndCmds
=========
*/
void Heart::RegisterCVarsAndCmds()
{
    Console::Register(&hrt_allowCheats);
    Console::Register(&hrt_textMode);
    Console::Register(&hrt_globalFPSCap);
    Console::Register(&quit); 
}

/*
=========
Heart::Shutdown
=========
*/
void Heart::Shutdown()
{
    ConsoleShell::Print("============ Shutting Down ============\n");
    ConsoleShell::Print("Heart::Shutdown();\n");


    InputSystem::Shutdown();
    ConsoleShell::Print("InputSystem::Shutdown();\n");


    Console::Shutdown();
    ConsoleShell::Print("Console::Shutdown();\n");


    ConsoleShell::Shutdown();

    glfwTerminate();

    Sys_Quit();
}

/*
=========
Heart::Tick
=========
*/
void Heart::Tick()
{
    ConsoleShell::Update();
}