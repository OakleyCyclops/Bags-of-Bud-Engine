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
    
        void            Init(int argc, const char* const* argv)         override;
        void            Shutdown()                                      override;
        void            Tick()                                          override;

    private:
        Console     console;
};