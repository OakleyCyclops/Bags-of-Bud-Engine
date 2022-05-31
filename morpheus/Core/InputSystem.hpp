#ifndef __INPUT_SYSTEM_HPP__
#define __INPUT_SYSTEM_HPP__

namespace InputSystem
{
    void            Init();
    void            RegisterCVarsAndCmds();

    void            Update();
    void            Shutdown();
};

struct InputEvent
{
    bool            Pressed;
    bool            canHold;
    String          Binding;     
};

#endif /* !__INPUT_SYSTEM_HPP__ */