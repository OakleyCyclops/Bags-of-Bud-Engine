#ifndef __STRING_HPP__
#define __STRING_HPP__

// Forward declaration
class Vector4;

enum utf8Encoding_t
{
	UTF8_PURE_ASCII,		// no characters with values > 127
	UTF8_ENCODED_BOM,		// characters > 128 encoded with UTF8, but no byte-order-marker at the beginning
	UTF8_ENCODED_NO_BOM,	// characters > 128 encoded with UTF8, with a byte-order-marker at the beginning
	UTF8_INVALID,			// has values > 127 but isn't valid UTF8
	UTF8_INVALID_BOM		// has a byte-order-marker at the beginning, but isn't valuid UTF8 -- it's messed up
};

// Units
inline const char* units[2][4] =
{
	{ "B", "KB", "MB", "GB" },
	{ "B/s", "KB/s", "MB/s", "GB/s" }
};

// Color escape character
const int C_COLOR_ESCAPE			= 	'^';
const int C_COLOR_DEFAULT			= 	'0';
const int C_COLOR_RED				= 	'1';
const int C_COLOR_GREEN				= 	'2';
const int C_COLOR_YELLOW			= 	'3';
const int C_COLOR_BLUE				= 	'4';
const int C_COLOR_CYAN				= 	'5';
const int C_COLOR_ORANGE			= 	'6';
const int C_COLOR_WHITE				= 	'7';
const int C_COLOR_GRAY				= 	'8';
const int C_COLOR_BLACK				= 	'9';

// Color escape string
#define S_COLOR_DEFAULT				  	"^0";
#define S_COLOR_RED						"^1";
#define S_COLOR_GREEN			 		"^2";
#define S_COLOR_YELLOW			 		"^3";
#define S_COLOR_BLUE			 		"^4";
#define S_COLOR_CYAN			 		"^5";
#define S_COLOR_ORANGE			 		"^6";
#define S_COLOR_WHITE			 		"^7";
#define S_COLOR_GRAY			 		"^8";
#define S_COLOR_BLACK			 		"^9";

// make String a multiple of 16 bytes long
// don't make too large to keep memory requirements to a minimum
const int STR_ALLOC_BASE			= 20;
const int STR_ALLOC_GRAN			= 32;

/*
====================
	String class
====================
*/
class String final
{
    public:
        String();

        String(const String& text);
        String(const String& text, int start, int end);
        
        String(const char* text);
        String(const char* text, int start, int end);

        explicit String( const bool b );
        explicit String( const char c );
        explicit String( const int i );
        explicit String( const unsigned u );
        explicit String( const float f );

        ~String();

        size_t				Size() const;

        const char* 		c_str() const;

        int					Length() const;
        int					LengthWithoutColors() const;

	    int					Allocated() const;

	    void				Empty();
	    void				Clear();
        void				CapLength(int newLength);
	    void				Fill(const char ch, int newLength);

	    void				Append(const char a);
	    void				Append(const String& text);
	    void				Append(const char* text);
	    void				Append(const char* text, int length);

	    void				Insert(const char a, int index);
	    void				Insert(const char* text, int index);

	    void				ToLower();
	    void				ToUpper();

        bool				IsEmpty() const;
	    bool				IsNumeric() const;
	    bool				IsColor() const;
	    bool				HasLower() const;
	    bool				HasUpper() const;

        // case sensitive compare
	    int					Cmp(const char* text) const;
	    int					Cmpn(const char* text, int n) const;
	    int					CmpPrefix(const char* text) const;
	
	    // case insensitive compare
	    int					Icmp(const char* text) const;
	    int					Icmpn(const char* text, int n ) const;
	    int					IcmpPrefix(const char* text) const;

		// compares paths and makes sure folders come first
		int					IcmpPath( const char* text ) const;
		int					IcmpnPath( const char* text, int n ) const;
		int					IcmpPrefixPath( const char* text ) const;
	
	    // case insensitive compare ignoring color
	    int					IcmpNoColor(const char* text) const;

        // UTF-8 Stuff
        int                 UTF8Length();
        int                 UTF8Length(const byte* s);

		int					Find(const char c, int start = 0, int end = -1) const;
		int					Find(const char* text, bool casesensitive = true, int start = 0, int end = -1) const;

	    uint32              UTF8Char(int& idx);
	    uint32              UTF8Char(const char* s, int& idx);
	    uint32              UTF8Char(const byte* s, int& idx);

	    void                AppendUTF8Char(uint32 c);
	    void                ConvertToUTF8();

	    bool                IsValidUTF8(const uint8* s, const int maxLen, utf8Encoding_t& encoding);
	    bool                IsValidUTF8(const char* s, const int maxLen, utf8Encoding_t& encoding);

	    String& 			RemoveColors();
		
		bool				StripTrailingOnce(const char* string);		// strip string from end just once if it occurs
		bool				StripLeadingOnce(const char* string);		// strip string from front just once if it occurs


		void				StripLeading(const char c);					// strip char from front as many times as the char occurs
		void				StripLeading(const char* string);			// strip string from front as many times as the string occurs
		void				StripTrailing(const char c);				// strip char from end as many times as the char occurs
		void				StripTrailing(const char* string);			// strip string from end as many times as the string occurs
		void				Strip(const char c);						// strip char from front and end as many times as the char occurs
		void				Strip(const char* string);					// strip string from front and end as many times as the string occurs
		void				StripTrailingWhitespace();					// strip trailing white space characters

		String& 			StripQuotes();								// strip quotes around string

		// hash keys
		static int			Hash(const char* string);
		static int			Hash(const char* string, int length);

		static int			IHash(const char* string);					// case insensitive
		static int			IHash(const char* string, int length);		// case insensitive
		
		// character methods
		static char			ToLower(char c);
		static char			ToUpper(char c);
		static int			ColorIndex(int c);
		static Vector4& 	ColorForIndex(int i);
		
		friend int			sprintf(String& dest, const char* fmt, ...);
		friend int			vsprintf(String& dest, const char* fmt, va_list ap);
		
		void				ReAllocate( int amount, bool keepold );				// reallocate string data buffer
		void				FreeData();											// free allocated string memory

	    operator			const char* () const;
	    operator			const char* ();
	
	    char				operator[](int index) const;
	    char& 				operator[](int index);
	
	    void				operator=(const String& text);
	    void				operator=(const char* text);
	
	    friend String		operator+(const String& a, const String& b);
	    friend String		operator+(const String& a, const char* b);
	    friend String		operator+(const char* a, const String& b);
	
	    friend String		operator+(const String& a, const float b);
	    friend String		operator+(const String& a, const int b);
	    friend String		operator+(const String& a, const unsigned b);
	    friend String		operator+(const String& a, const bool b);
	    friend String		operator+(const String& a, const char b);

        String& 			operator+=(const String& a);
	    String& 			operator+=(const char* a);
	    String& 			operator+=(const float a);
	    String& 			operator+=(const char a);
	    String& 			operator+=(const int a);
	    String& 			operator+=(const unsigned a);
	    String& 			operator+=(const bool a);

        // Case-sensitive compare
	    friend bool			operator==(const String& a, const String& b);
	    friend bool			operator==(const String& a, const char* b);
	    friend bool			operator==(const char* a, const String& b);
	
	    friend bool			operator!=(const String& a, const String& b);
	    friend bool			operator!=(const String& a, const char* b);
	    friend bool			operator!=(const char* a, const String& b);

	private:
		// Initialize string using base buffer
		// ONLY CALL FROM CONSTRUCTOR
		void				Construct();
	
		// sets the data point to the specified buffer... note that this ignores makes the passed buffer empty and ignores
		// anything currently in the budStr's dynamic buffer.  This method is intended to be called only from a derived class's constructor.
		void				SetStaticBuffer(char* buffer, const int bufferLength);

		bool				IsStatic() const;
		void				SetStatic(const bool isStatic);

		int					GetAlloced() const;
		void				SetAlloced(const int a);

		// ensure string data buffer is large anough
		void				EnsureAlloced( int amount, bool keepold = true );

		static const uint32	STATIC_BIT	= 31;
		static const uint32	STATIC_MASK	= 1u << STATIC_BIT;
		static const uint32	ALLOCED_MASK = STATIC_MASK - 1;

	protected:
		int					length;
		char*				data;
		int					allocedAndFlag;
		char				baseBuffer[STR_ALLOC_BASE];
};

/*
==========================================================
	Struct of char* methods to replace library methods 
==========================================================
*/
inline struct CharMethods
{
	static int			Length(const char* s);
	static int			LengthWithoutColors(const char* s);
	static int			Cmp(const char* s1, const char* s2);
	static int			Cmpn(const char* s1, const char* s2, int n);
	static int			Icmp(const char* s1, const char* s2);
	static int			Icmpn(const char* s1, const char* s2, int n);
	static int			IcmpNoColor(const char* s1, const char* s2);
	static int			IcmpPath(const char* s1, const char* s2);			// compares paths and makes sure folders come first
	static int			IcmpnPath(const char* s1, const char* s2, int n);	// compares paths and makes sure folders come first
	static int			snPrintf(char* dest, int size, const char* fmt, ...);
	static int			vsnPrintf(char* dest, int size, const char* fmt, va_list argptr);
	static int			FindChar(const char* str, const char c, int start = 0, int end = -1);
	static int			FindText(const char* str, const char* text, bool casesensitive = true, int start = 0, int end = -1);
	static int			ColorIndex(int c);

	static void			Append(char* dest, int size, const char* src);
	static void			Copynz(char* dest, const char* src, int destsize);
	static void			StripMediaName(const char* name, String& mediaName);

	static bool			CharIsPrintable(int c);
	static bool			CharIsLower(int c);
	static bool			CharIsUpper(int c);
	static bool			CharIsAlpha(int c);
	static bool			CharIsNumeric(int c);
	static bool			CharIsNewLine(char c);
	static bool			CharIsTab(char c);
	static bool			IsNumeric(const char* s);
	static bool			IsColor(const char* s);
	static bool			HasLower(const char* s);
	static bool			HasUpper(const char* s);
	static bool			Filter(const char* filter, const char* name, bool casesensitive);
	static bool			CheckExtension(const char* name, const char* ext);

	static char* 		ToLower(char* s);
	static char* 		ToUpper(char* s);
	static char* 		RemoveColors(char* string);

	static char			ToLower(char c);
	static char			ToUpper(char c);

	static const char* 	FloatArrayToString(const float* array, const int length, const int precision);
	static const char* 	CStyleQuote(const char* str);
	static const char* 	CStyleUnQuote(const char* str);
} CharMethods;

#endif /* !__STRING_HPP__ */