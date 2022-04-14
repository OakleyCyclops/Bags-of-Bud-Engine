/*
======================================================================
The console is strictly for development and advanced users. It should
	never be used to convey actual game information to the user 
			which should always be done through a GUI.
======================================================================
*/
class Console final : public INTFconsole
{
    public:
        void            Init()                                      override;
        void            Shutdown()                                  override;
        void            PrintF(const char* fmt, ...)                override;
};