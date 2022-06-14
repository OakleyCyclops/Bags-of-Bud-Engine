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
	String struct
====================
*/
struct String
{
	String();
    String(const String& text);
    String(const String& text, int start, int end);
    String(const char* text);
    String(const char* text, int start, int end);

    explicit String(const bool b);
    explicit String(const char c);
    explicit String(const int i);
    explicit String(const unsigned u);
    explicit String(const float f);

    ~String();

	void				Construct();	// Initialize string using base buffer
	void				FreeData();		// Free allocated string memory

	operator			const char* () const;
	operator			const char* ();

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

	friend bool			operator==(const String& a, const String& b);
	friend bool			operator==(const String& a, const char* b);
	friend bool			operator==(const char* a, const String& b);
	
	friend bool			operator!=(const String& a, const String& b);
	friend bool			operator!=(const String& a, const char* b);
	friend bool			operator!=(const char* a, const String& b);

	char				operator[](int index) const;
	char& 				operator[](int index);
	
	void				operator=(const String& text);
	void				operator=(const char* text);

	int					Length;
	char*				Cstring;
	int					AllocedAndFlag;
	char				BaseBuffer[STR_ALLOC_BASE];
	
	static const uint32	STATIC_BIT	= 31;
	static const uint32	STATIC_MASK	= 1u << STATIC_BIT;
	static const uint32	ALLOCED_MASK = STATIC_MASK - 1;
};

namespace StringMethods
{
    size_t 		Size(String* str);

    int			LengthWithoutColors(String* str);
	int			Allocated(String* str);

	void		Empty(String* str);
	void		Clear(String* str);
    void		CapLength(String* str, int newLength);
	void		Fill(String* str, const char ch, int newLength);

	void		Append(String* str, const char a);
	void		Append(String* str, const String& text);
	void		Append(String* str, const char* text);
	void		Append(String* str, const char* text, int length);

	String& 	BackSlashesToForwardSlashes(String& str);
	String&		ForwardSlashesToBackSlashes(String& str);

	void		Insert(String* str, const char a, int index);
	void		Insert(String* str, const char* text, int index);

	void		ToLower(String* str);
	void		ToUpper(String* str);

    bool		IsEmpty(String* str);
	bool		IsNumeric(String* str);
	bool		IsColor(String* str);
	bool		HasLower(String* str);
	bool		HasUpper(String* str);

    // case sensitive compare
	int			Cmp(String* str, const char* text);
	int			Cmpn(String* str, const char* text, int n);
	int			CmpPrefix(String* str, const char* text);
	
	// case insensitive compare
	int			Icmp(String* str, const char* text);
	int			Icmpn(String* str, const char* text, int n );
	int			IcmpPrefix(String* str, const char* text);

	// compares paths and makes sure folders come first
	int			IcmpPath(String* str, const char* text );
	int			IcmpnPath(String* str, const char* text, int n );
	int			IcmpPrefixPath(String* str, const char* text );
	
	// case insensitive compare ignoring color
	int			IcmpNoColor(String* str, const char* text);

    // UTF-8 Stuff
    int			UTF8Length(String* str);
    int			UTF8Length(String* str, const byte* s);

	int			Find(String* str, const char c, int start = 0, int end = -1);
	int			Find(String* str, const char* text, bool casesensitive = true, int start = 0, int end = -1);

	uint32		UTF8Char(String* str, int& idx);
	uint32		UTF8Char(String* str, const char* s, int& idx);
	uint32		UTF8Char(String* str, const byte* s, int& idx);

	void		AppendUTF8Char(String* str, uint32 c);
	void		ConvertToUTF8(String* str);

	bool		IsValidUTF8(String* str, const uint8* s, const int maxLen, utf8Encoding_t& encoding);
	bool		IsValidUTF8(String* str, const char* s, const int maxLen, utf8Encoding_t& encoding);

	String& 	RemoveColors(String* str);
		
	bool		StripTrailingOnce(String* str, const char* string);		// strip string from end just once if it occurs
	bool		StripLeadingOnce(String* str, const char* string);		// strip string from front just once if it occurs


	void		StripLeading(String* str, const char c);					// strip char from front as many times as the char occurs
	void		StripLeading(String* str, const char* string);			// strip string from front as many times as the string occurs
	void		StripTrailing(String* str, const char c);				// strip char from end as many times as the char occurs
	void		StripTrailing(String* str, const char* string);			// strip string from end as many times as the string occurs
	void		Strip(String* str, const char c);						// strip char from front and end as many times as the char occurs
	void		Strip(String* str, const char* string);					// strip string from front and end as many times as the string occurs
	void		StripTrailingWhitespace(String* str);					// strip trailing white space characters

	String& 	StripQuotes(String* str);								// strip quotes around string

	// hash keys
	int			Hash(String* str, const char* string);
	int			Hash(String* str, const char* string, int length);

	int			IHash(String* str, const char* string);					// case insensitive
	int			IHash(String* str, const char* string, int length);		// case insensitive
		
	// character methods
	char		ToLower(String* str, char c);
	char		ToUpper(String* str, char c);
	int			ColorIndex(String* str, int c);
	Vector4& 	ColorForIndex(String* str, int i);
		
	int			sprintf(String* src, String* dest, const char* fmt, ...);
	int			vsprintf(String* src, String* dest, const char* fmt, va_list ap);
		
	void		ReAllocate(String* str, int amount, bool keepold);				// reallocate string data buffer
	
	// sets the data point to the specified buffer... note that this ignores makes the passed buffer empty and ignores
	// anything currently in the budStr's dynamic buffer.  This method is intended to be called only from a derived class's constructor.
	void		SetStaticBuffer(char* buffer, const int bufferLength);

	bool		IsStatic(String* str);
	void		SetStatic(String* str, const bool isStatic);

	int			GetAlloced(String* str);
	void		SetAlloced(String* str, const int a);

	// ensure string data buffer is large anough
	void		EnsureAlloced(String* str, int amount, bool keepold = true);
};

/*
==========================================================
	Char* methods to replace library methods 
==========================================================
*/
namespace CharMethods
{
	int				Length(const char* s);
	int				LengthWithoutColors(const char* s);
	int				Cmp(const char* s1, const char* s2);
	int				Cmpn(const char* s1, const char* s2, int n);
	int				Icmp(const char* s1, const char* s2);
	int				Icmpn(const char* s1, const char* s2, int n);
	int				IcmpNoColor(const char* s1, const char* s2);
	int				IcmpPath(const char* s1, const char* s2);			// compares paths and makes sure folders come first
	int				IcmpnPath(const char* s1, const char* s2, int n);	// compares paths and makes sure folders come first
	int				snPrintf(char* dest, int size, const char* fmt, ...);
	int				vsnPrintf(char* dest, int size, const char* fmt, va_list argptr);
	int				FindChar(const char* str, const char c, int start = 0, int end = -1);
	int				FindText(const char* str, const char* text, bool casesensitive = true, int start = 0, int end = -1);
	int				ColorIndex(int c);

	void			Append(char* dest, int size, const char* src);
	void			Copynz(char* dest, const char* src, int destsize);
	void			StripMediaName(const char* name, String& mediaName);

	bool			CharIsPrintable(int c);
	bool			CharIsLower(int c);
	bool			CharIsUpper(int c);
	bool			CharIsAlpha(int c);
	bool			CharIsNumeric(int c);
	bool			CharIsNewLine(char c);
	bool			CharIsTab(char c);
	bool			IsNumeric(const char* s);
	bool			IsColor(const char* s);
	bool			HasLower(const char* s);
	bool			HasUpper(const char* s);
	bool			Filter(const char* filter, const char* name, bool casesensitive);
	bool			CheckExtension(const char* name, const char* ext);

	char*			ToLower(char* s);
	char*			ToUpper(char* s);
	char*			RemoveColors(char* string);


	char*			FloatToString(float f, char* s, unsigned int afterPoint, unsigned int base);
	char*			DoubleToString(double d, char* s, unsigned int afterPoint, unsigned int base);
	char*			LongToString(long l, char* s, unsigned int base);
	char*			IntToString(int i, char* s, unsigned int base);


	char			ToLower(char c);
	char			ToUpper(char c);

	const char* 	FloatArrayToString(const float* array, const int length, const int precision);
	const char* 	CStyleQuote(const char* str);
	const char* 	CStyleUnQuote(const char* str);
}

#endif /* !__STRING_HPP__ */