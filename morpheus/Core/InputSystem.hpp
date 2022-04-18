#ifndef __INPUT_SYSTEM_HPP__
#define __INPUT_SYSTEM_HPP__

class InputSystem final : public INTFinputSystem
{
    public:
        void            Init()                                      override;
        void            Update()                                    override;
        void            Shutdown()                                  override;
        SDL_Event event;
};

class InputEvent final
{
    public:
        InputEvent()
        {
            Pressed = false;
            canHold = false;
        }
    
    bool        Pressed;
    bool        canHold;
    String      Binding;     
};

#endif /* !__INPUT_SYSTEM_HPP__ */