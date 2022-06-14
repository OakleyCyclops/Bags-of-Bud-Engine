#ifndef __GAME_HPP__
#define __GAME_HPP__

namespace Game
{
    void Init(int argc, const char** argv);
    void RegisterCVarsAndCmds();

    void Shutdown();
    void Tick();

    inline GLFWwindow* Window;
};

#endif /* !__GAME_HPP__ */