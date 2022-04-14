
#include "libBudPCH.hpp"

// thank u admer very cool :)

template <typename Class>

class Singleton final
{
    public:
        static Class& GetInstance()
        {
            static Class instance;
            return instance;
        }

        static const Class& GetInstanceConst()
        {
            return GetInstance();
        }
};