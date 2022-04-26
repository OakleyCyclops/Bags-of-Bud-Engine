#ifndef __HEART_HPP__
#define __HEART_HPP__

/*
=========
coreHeart : public INTFheart

The "Heart" contains all the universal functions and commands
used throughout the engine, so things like Init(), Shutdown(), Print(), stuff like that.
=========
*/
class Heart final : public INTFheart
{
    public:
        void            Init(int argc, const char** argv)               override;
        void            Shutdown()                                      override;
        void            Tick()                                          override;

    private:
        Console& console              =       Singleton<Console>::GetInstance();
        ConsoleShell& consoleShell    =       Singleton<ConsoleShell>::GetInstance();
        InputSystem& inputSystem      =       Singleton<InputSystem>::GetInstance();
};

#endif /* !__HEART_HPP__ */