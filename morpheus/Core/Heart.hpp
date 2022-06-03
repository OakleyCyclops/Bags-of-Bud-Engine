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

    inline CVar hrt_allowCheats = {"hrt_allowCheats", new bool(0), "Enables cheats", CVAR_BOOL | CVAR_CORE};
    inline CVar hrt_textMode = {"hrt_textMode", new bool(0), "If true, launches the engine in \"Text Mode\" Renderer is never initialized", CVAR_BOOL | CVAR_CORE | CVAR_INIT};
    inline CVar hrt_globalFPSCap = {"hrt_globalFPSCap", new int(1000), "Global framerate cap across all threads", CVAR_INTEGER | CVAR_CORE};

    // Quit command
    inline auto cmdQuit = []
    {
        Heart::Shutdown();
    };

    inline Cmd quit = {"quit", cmdQuit, "Quits the game", CMD_CORE};
};

#endif /* !__HEART_HPP__ */