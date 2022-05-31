#ifndef __HEART_HPP__
#define __HEART_HPP__

/*
=========
Heart

The "Heart" contains all the universal functions and commands
used throughout the engine, so things like Init(), Shutdown(), Print(), stuff like that.
=========
*/
namespace Heart
{
    void Init(int argc, const char** argv);
    void RegisterCVarsAndCmds();

    void Shutdown();
    void Tick();

    inline GLFWwindow* Window;

    inline CVar hrt_textMode = {"hrt_textMode", (bool*)0, "If true, launches the engine in \"Text Mode\" Renderer is never initialized", CVAR_CORE || CVAR_INIT || CVAR_BOOL};
    inline CVar hrt_globalFPSCap = {"hrt_globalFPSCap", (int*)1000, "Global framerate cap across all threads", CVAR_CORE || CVAR_INTEGER};

    // Quit command
    inline auto cmdQuit = []
    {
        Heart::Shutdown();
    };

    inline Cmd quit = {"quit", cmdQuit, "Quits the game", CMD_CORE};
};

#endif /* !__HEART_HPP__ */