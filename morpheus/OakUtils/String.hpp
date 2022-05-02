/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2012 Robert Beckebans

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#ifndef __STRING_HPP__
#define __STRING_HPP__


/*
===============================================================================

	Character string

===============================================================================
*/

#define ASSERT_ENUM_STRING( string, index )		( 1 / (int)!( string - index ) ) ? #string : ""

enum utf8Encoding_t
{
	UTF8_PURE_ASCII,		// no characters with values > 127
	UTF8_ENCODED_BOM,		// characters > 128 encoded with UTF8, but no byte-order-marker at the beginning
	UTF8_ENCODED_NO_BOM,	// characters > 128 encoded with UTF8, with a byte-order-marker at the beginning
	UTF8_INVALID,			// has values > 127 but isn't valid UTF8
	UTF8_INVALID_BOM		// has a byte-order-marker at the beginning, but isn't valuid UTF8 -- it's messed up
};

// these library functions should not be used for cross platform compatibility
#define strcmp			String::Cmp		// use_String_Cmp
#define strncmp			use_String_Cmpn

#if defined( StrCmpN )
#undef StrCmpN
#endif
#define StrCmpN			use_String_Cmpn

#if defined( strcmpi )
#undef strcmpi
#endif
#define strcmpi			use_String_Icmp

#if defined( StrCmpI )
#undef StrCmpI
#endif
#define StrCmpI			use_String_Icmp

#if defined( StrCmpNI )
#undef StrCmpNI
#endif
#define StrCmpNI		use_String_Icmpn

#define stricmp			String::Icmp		// use_String_Icmp

#undef strcasecmp // DG: redefining this without undefining it causes tons of compiler warnings

#define _stricmp		use_String_Icmp
#define strcasecmp		use_String_Icmp
#define strnicmp		use_String_Icmpn
#define _strnicmp		use_String_Icmpn
#define _memicmp		use_String_Icmpn

class Vector4;

#ifndef FILE_HASH_SIZE
#define FILE_HASH_SIZE		1024
#endif

// color escape character
const int C_COLOR_ESCAPE			= '^';
const int C_COLOR_DEFAULT			= '0';
const int C_COLOR_RED				= '1';
const int C_COLOR_GREEN				= '2';
const int C_COLOR_YELLOW			= '3';
const int C_COLOR_BLUE				= '4';
const int C_COLOR_CYAN				= '5';
const int C_COLOR_ORANGE			= '6';
const int C_COLOR_WHITE				= '7';
const int C_COLOR_GRAY				= '8';
const int C_COLOR_BLACK				= '9';

// color escape string
#define S_COLOR_DEFAULT				"^0"
#define S_COLOR_RED					"^1"
#define S_COLOR_GREEN				"^2"
#define S_COLOR_YELLOW				"^3"
#define S_COLOR_BLUE				"^4"
#define S_COLOR_CYAN				"^5"
#define S_COLOR_ORANGE				"^6"
#define S_COLOR_WHITE				"^7"
#define S_COLOR_GRAY				"^8"
#define S_COLOR_BLACK				"^9"

// make String a multiple of 16 bytes long
// don't make too large to keep memory requirements to a minimum
const int STR_ALLOC_BASE			= 20;
const int STR_ALLOC_GRAN			= 32;

typedef enum
{
	MEASURE_SIZE = 0,
	MEASURE_BANDWIDTH
} Measure_t;

class String
{

public:
	String();
	String( const String& text );
	String( const String& text, int start, int end );
	String( const char* text );
	String( const char* text, int start, int end );
	explicit String( const bool b );
	explicit String( const char c );
	explicit String( const int i );
	explicit String( const unsigned u );
	explicit String( const float f );
	~String();
	
	size_t				Size() const;
	const char* 		c_str() const;
	operator			const char* () const;
	operator			const char* ();
	
	char				operator[]( int index ) const;
	char& 				operator[]( int index );
	
	void				operator=( const String& text );
	void				operator=( const char* text );
	
	friend String		operator+( const String& a, const String& b );
	friend String		operator+( const String& a, const char* b );
	friend String		operator+( const char* a, const String& b );
	
	friend String		operator+( const String& a, const float b );
	friend String		operator+( const String& a, const int b );
	friend String		operator+( const String& a, const unsigned b );
	friend String		operator+( const String& a, const bool b );
	friend String		operator+( const String& a, const char b );
	
	String& 				operator+=( const String& a );
	String& 				operator+=( const char* a );
	String& 				operator+=( const float a );
	String& 				operator+=( const char a );
	String& 				operator+=( const int a );
	String& 				operator+=( const unsigned a );
	String& 				operator+=( const bool a );
	
	// case sensitive compare
	friend bool			operator==( const String& a, const String& b );
	friend bool			operator==( const String& a, const char* b );
	friend bool			operator==( const char* a, const String& b );
	
	// case sensitive compare
	friend bool			operator!=( const String& a, const String& b );
	friend bool			operator!=( const String& a, const char* b );
	friend bool			operator!=( const char* a, const String& b );
	
	// case sensitive compare
	int					Cmp( const char* text ) const;
	int					Cmpn( const char* text, int n ) const;
	int					CmpPrefix( const char* text ) const;
	
	// case insensitive compare
	int					Icmp( const char* text ) const;
	int					Icmpn( const char* text, int n ) const;
	int					IcmpPrefix( const char* text ) const;
	
	// case insensitive compare ignoring color
	int					IcmpNoColor( const char* text ) const;
	
	// compares paths and makes sure folders come first
	int					IcmpPath( const char* text ) const;
	int					IcmpnPath( const char* text, int n ) const;
	int					IcmpPrefixPath( const char* text ) const;
	
	int					Length() const;
	int					Allocated() const;
	void				Empty();
	bool				IsEmpty() const;
	void				Clear();
	void				Append( const char a );
	void				Append( const String& text );
	void				Append( const char* text );
	void				Append( const char* text, int len );
	void				Insert( const char a, int index );
	void				Insert( const char* text, int index );
	void				ToLower();
	void				ToUpper();
	bool				IsNumeric() const;
	bool				IsColor() const;
	bool				HasLower() const;
	bool				HasUpper() const;
	int					LengthWithoutColors() const;
	String& 				RemoveColors();
	void				CapLength( int );
	void				Fill( const char ch, int newlen );
	
	BUD_INLINE int			UTF8Length();
	BUD_INLINE uint32		UTF8Char( int& idx );
	static int				UTF8Length( const byte* s );
	static BUD_INLINE uint32 UTF8Char( const char* s, int& idx );
	static uint32			UTF8Char( const byte* s, int& idx );
	void					AppendUTF8Char( uint32 c );
	BUD_INLINE void			ConvertToUTF8();
	static bool				IsValidUTF8( const uint8* s, const int maxLen, utf8Encoding_t& encoding );
	static BUD_INLINE bool	IsValidUTF8( const char* s, const int maxLen, utf8Encoding_t& encoding )
	{
		return IsValidUTF8( ( const uint8* )s, maxLen, encoding );
	}
	static BUD_INLINE bool	IsValidUTF8( const uint8* s, const int maxLen );
	static BUD_INLINE bool	IsValidUTF8( const char* s, const int maxLen )
	{
		return IsValidUTF8( ( const uint8* )s, maxLen );
	}
	
	int					Find( const char c, int start = 0, int end = -1 ) const;
	int					Find( const char* text, bool casesensitive = true, int start = 0, int end = -1 ) const;
	bool				Filter( const char* filter, bool casesensitive ) const;
	int					Last( const char c ) const;						// return the index to the last occurance of 'c', returns -1 if not found
	const char* 		Left( int len, String& result ) const;			// store the leftmost 'len' characters in the result
	const char* 		Right( int len, String& result ) const;			// store the rightmost 'len' characters in the result
	const char* 		Mid( int start, int len, String& result ) const;	// store 'len' characters starting at 'start' in result
	String				Left( int len ) const;							// return the leftmost 'len' characters
	String				Right( int len ) const;							// return the rightmost 'len' characters
	String				Mid( int start, int len ) const;				// return 'len' characters starting at 'start'
	void				Format( VERIFY_FORMAT_STRING const char* fmt, ... );					// perform a threadsafe sprintf to the string
	static String		FormatInt( const int num, bool isCash = false );			// formats an integer as a value with commas
	static String		FormatCash( const int num )
	{
		return FormatInt( num, true );
	}
	void				StripLeading( const char c );					// strip char from front as many times as the char occurs
	void				StripLeading( const char* string );				// strip string from front as many times as the string occurs
	bool				StripLeadingOnce( const char* string );			// strip string from front just once if it occurs
	void				StripTrailing( const char c );					// strip char from end as many times as the char occurs
	void				StripTrailing( const char* string );			// strip string from end as many times as the string occurs
	bool				StripTrailingOnce( const char* string );		// strip string from end just once if it occurs
	void				Strip( const char c );							// strip char from front and end as many times as the char occurs
	void				Strip( const char* string );					// strip string from front and end as many times as the string occurs
	void				StripTrailingWhitespace();				// strip trailing white space characters
	String& 				StripQuotes();							// strip quotes around string
	bool				Replace( const char* old, const char* nw );
	bool				ReplaceChar( const char old, const char nw );
	BUD_INLINE void		CopyRange( const char* text, int start, int end );
	
	// file name methods
	int					FileNameHash() const;						// hash key for the filename (skips extension)
	String& 				BackSlashesToSlashes();					// convert slashes
	String& 				SlashesToBackSlashes();					// convert slashes
	String& 				SetFileExtension( const char* extension );		// set the given file extension
	String& 				StripFileExtension();						// remove any file extension
	String& 				StripAbsoluteFileExtension();				// remove any file extension looking from front (useful if there are multiple .'s)
	String& 				DefaultFileExtension( const char* extension );	// if there's no file extension use the default
	String& 				DefaultPath( const char* basepath );			// if there's no path use the default
	void				AppendPath( const char* text );					// append a partial path
	String& 				StripFilename();							// remove the filename from a path
	String& 				StripPath();								// remove the path from the filename
	void				ExtractFilePath( String& dest ) const;			// copy the file path to another string
	void				ExtractFileName( String& dest ) const;			// copy the filename to another string
	void				ExtractFileBase( String& dest ) const;			// copy the filename minus the extension to another string
	void				ExtractFileExtension( String& dest ) const;		// copy the file extension to another string
	bool				CheckExtension( const char* ext );
	
	// char * methods to replace library functions
	static int			Length( const char* s );
	static char* 		ToLower( char* s );
	static char* 		ToUpper( char* s );
	static bool			IsNumeric( const char* s );
	static bool			IsColor( const char* s );
	static bool			HasLower( const char* s );
	static bool			HasUpper( const char* s );
	static int			LengthWithoutColors( const char* s );
	static char* 		RemoveColors( char* s );
	static int			Cmp( const char* s1, const char* s2 );
	static int			Cmpn( const char* s1, const char* s2, int n );
	static int			Icmp( const char* s1, const char* s2 );
	static int			Icmpn( const char* s1, const char* s2, int n );
	static int			IcmpNoColor( const char* s1, const char* s2 );
	static int			IcmpPath( const char* s1, const char* s2 );			// compares paths and makes sure folders come first
	static int			IcmpnPath( const char* s1, const char* s2, int n );	// compares paths and makes sure folders come first
	static void			Append( char* dest, int size, const char* src );
	static void			Copynz( char* dest, const char* src, int destsize );
	static int			snPrintf( char* dest, int size, VERIFY_FORMAT_STRING const char* fmt, ... );
	static int			vsnPrintf( char* dest, int size, const char* fmt, va_list argptr );
	static int			FindChar( const char* str, const char c, int start = 0, int end = -1 );
	static int			FindText( const char* str, const char* text, bool casesensitive = true, int start = 0, int end = -1 );
	static bool			Filter( const char* filter, const char* name, bool casesensitive );
	static void			StripMediaName( const char* name, String& mediaName );
	static bool			CheckExtension( const char* name, const char* ext );
	static const char* 	FloatArrayToString( const float* array, const int length, const int precision );
	static const char* 	CStyleQuote( const char* str );
	static const char* 	CStyleUnQuote( const char* str );
	
	// hash keys
	static int			Hash( const char* string );
	static int			Hash( const char* string, int length );
	static int			IHash( const char* string );					// case insensitive
	static int			IHash( const char* string, int length );		// case insensitive
	
	// character methods
	static char			ToLower( char c );
	static char			ToUpper( char c );
	static bool			CharIsPrintable( int c );
	static bool			CharIsLower( int c );
	static bool			CharIsUpper( int c );
	static bool			CharIsAlpha( int c );
	static bool			CharIsNumeric( int c );
	static bool			CharIsNewLine( char c );
	static bool			CharIsTab( char c );
	static int			ColorIndex( int c );
	
	friend int			sprintf( String& dest, const char* fmt, ... );
	friend int			vsprintf( String& dest, const char* fmt, va_list ap );
	
	void				ReAllocate( int amount, bool keepold );				// reallocate string data buffer
	void				FreeData();									// free allocated string memory
	
	// format value in the given measurement with the best unit, returns the best unit
	int					BestUnit( const char* format, float value, Measure_t measure );
	// format value in the requested unit and measurement
	void				SetUnit( const char* format, float value, int unit, Measure_t measure );
	
	static void			InitMemory();
	static void			ShutdownMemory();
	static void			PurgeMemory();
	
	int					DynamicMemoryUsed() const;
	static String		FormatNumber( int number );
	
protected:
	int					len;
	char* 				data;
	int					allocedAndFlag;	// top bit is used to store a flag that indicates if the string data is static or not
	char				baseBuffer[ STR_ALLOC_BASE ];
	
	void				EnsureAlloced( int amount, bool keepold = true );	// ensure string data buffer is large anough
	
	// sets the data point to the specified buffer... note that this ignores makes the passed buffer empty and ignores
	// anything currently in the String's dynamic buffer.  This method is intended to be called only from a derived class's constructor.
	BUD_INLINE void		SetStaticBuffer( char* buffer, const int bufferLength );
	
private:
	// initialize string using base buffer... call ONLY FROM CONSTRUCTOR
	BUD_INLINE void		Construct();
	
	static const uint32	STATIC_BIT	= 31;
	static const uint32	STATIC_MASK	= 1u << STATIC_BIT;
	static const uint32	ALLOCED_MASK = STATIC_MASK - 1;
	
	
	BUD_INLINE int		GetAlloced() const
	{
		return allocedAndFlag & ALLOCED_MASK;
	}
	BUD_INLINE void		SetAlloced( const int a )
	{
		allocedAndFlag = ( allocedAndFlag & STATIC_MASK ) | ( a & ALLOCED_MASK );
	}
	
	BUD_INLINE bool		IsStatic() const
	{
		return ( allocedAndFlag & STATIC_MASK ) != 0;
	}
	BUD_INLINE void		SetStatic( const bool isStatic )
	{
		allocedAndFlag = ( allocedAndFlag & ALLOCED_MASK ) | ( isStatic << STATIC_BIT );
	}
	
public:
	static const int	INVALID_POSITION = -1;
};

char* 					va( VERIFY_FORMAT_STRING const char* fmt, ... ) ID_STATIC_ATTRIBUTE_PRINTF( 1, 2 );

/*
================================================================================================

	Sort routines for sorting List<String>

================================================================================================
*/

class Sort_Str : public SortQuick< String, Sort_Str >
{
public:
	int Compare( const String& a, const String& b ) const
	{
		return a.Icmp( b );
	}
};

class Sort_PathStr : public SortQuick< String, Sort_PathStr >
{
public:
	int Compare( const String& a, const String& b ) const
	{
		return a.IcmpPath( b );
	}
};

/*
========================
String::Construct
========================
*/
inline void String::Construct()
{
	SetStatic( false );
	SetAlloced( STR_ALLOC_BASE );
	data = baseBuffer;
	len = 0;
	data[ 0 ] = '\0';
}


inline void String::EnsureAlloced( int amount, bool keepold )
{
	// static string's can't reallocate
	if( IsStatic() )
	{
		return;
	}
	if( amount > GetAlloced() )
	{
		ReAllocate( amount, keepold );
	}
}

/*
========================
String::SetStaticBuffer
========================
*/
BUD_INLINE void String::SetStaticBuffer( char* buffer, const int bufferLength )
{
	// this should only be called on a freshly constructed String
	assert( data == baseBuffer );
	data = buffer;
	len = 0;
	SetAlloced( bufferLength );
	SetStatic( true );
}

BUD_INLINE String::String()
{
	Construct();
}

BUD_INLINE String::String( const String& text )
{
	Construct();
	int l;
	
	l = text.Length();
	EnsureAlloced( l + 1 );
	strcpy( data, text.data );
	len = l;
}

BUD_INLINE String::String( const String& text, int start, int end )
{
	Construct();
	int i;
	int l;
	
	if( end > text.Length() )
	{
		end = text.Length();
	}
	if( start > text.Length() )
	{
		start = text.Length();
	}
	else if( start < 0 )
	{
		start = 0;
	}
	
	l = end - start;
	if( l < 0 )
	{
		l = 0;
	}
	
	EnsureAlloced( l + 1 );
	
	for( i = 0; i < l; i++ )
	{
		data[ i ] = text[ start + i ];
	}
	
	data[ l ] = '\0';
	len = l;
}

BUD_INLINE String::String( const char* text )
{
	Construct();
	int l;
	
	if( text )
	{
		// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
		l = ( int )strlen( text );
		// RB end
		EnsureAlloced( l + 1 );
		strcpy( data, text );
		len = l;
	}
}

BUD_INLINE String::String( const char* text, int start, int end )
{
	Construct();
	int i;
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	int l = ( int )strlen( text );
	// RB end
	
	if( end > l )
	{
		end = l;
	}
	if( start > l )
	{
		start = l;
	}
	else if( start < 0 )
	{
		start = 0;
	}
	
	l = end - start;
	if( l < 0 )
	{
		l = 0;
	}
	
	EnsureAlloced( l + 1 );
	
	for( i = 0; i < l; i++ )
	{
		data[ i ] = text[ start + i ];
	}
	
	data[ l ] = '\0';
	len = l;
}

BUD_INLINE String::String( const bool b )
{
	Construct();
	EnsureAlloced( 2 );
	data[ 0 ] = b ? '1' : '0';
	data[ 1 ] = '\0';
	len = 1;
}

BUD_INLINE String::String( const char c )
{
	Construct();
	EnsureAlloced( 2 );
	data[ 0 ] = c;
	data[ 1 ] = '\0';
	len = 1;
}

BUD_INLINE String::String( const int i )
{
	Construct();
	char text[ 64 ];
	int l;
	
	l = sprintf( text, "%d", i );
	EnsureAlloced( l + 1 );
	strcpy( data, text );
	len = l;
}

BUD_INLINE String::String( const unsigned u )
{
	Construct();
	char text[ 64 ];
	int l;
	
	l = sprintf( text, "%u", u );
	EnsureAlloced( l + 1 );
	strcpy( data, text );
	len = l;
}

BUD_INLINE String::String( const float f )
{
	Construct();
	char text[ 64 ];
	int l;
	
	l = String::snPrintf( text, sizeof( text ), "%f", f );
	while( l > 0 && text[l - 1] == '0' ) text[--l] = '\0';
	while( l > 0 && text[l - 1] == '.' ) text[--l] = '\0';
	EnsureAlloced( l + 1 );
	strcpy( data, text );
	len = l;
}

BUD_INLINE String::~String()
{
	FreeData();
}

BUD_INLINE size_t String::Size() const
{
	return sizeof( *this ) + Allocated();
}

BUD_INLINE const char* String::c_str() const
{
	return data;
}

BUD_INLINE String::operator const char* ()
{
	return c_str();
}

BUD_INLINE String::operator const char* () const
{
	return c_str();
}

BUD_INLINE char String::operator[]( int index ) const
{
	assert( ( index >= 0 ) && ( index <= len ) );
	return data[ index ];
}

BUD_INLINE char& String::operator[]( int index )
{
	assert( ( index >= 0 ) && ( index <= len ) );
	return data[ index ];
}

BUD_INLINE void String::operator=( const String& text )
{
	int l;
	
	l = text.Length();
	EnsureAlloced( l + 1, false );
	memcpy( data, text.data, l );
	data[l] = '\0';
	len = l;
}

BUD_INLINE String operator+( const String& a, const String& b )
{
	String result( a );
	result.Append( b );
	return result;
}

BUD_INLINE String operator+( const String& a, const char* b )
{
	String result( a );
	result.Append( b );
	return result;
}

BUD_INLINE String operator+( const char* a, const String& b )
{
	String result( a );
	result.Append( b );
	return result;
}

BUD_INLINE String operator+( const String& a, const bool b )
{
	String result( a );
	result.Append( b ? "true" : "false" );
	return result;
}

BUD_INLINE String operator+( const String& a, const char b )
{
	String result( a );
	result.Append( b );
	return result;
}

BUD_INLINE String operator+( const String& a, const float b )
{
	char	text[ 64 ];
	String	result( a );
	
	sprintf( text, "%f", b );
	result.Append( text );
	
	return result;
}

BUD_INLINE String operator+( const String& a, const int b )
{
	char	text[ 64 ];
	String	result( a );
	
	sprintf( text, "%d", b );
	result.Append( text );
	
	return result;
}

BUD_INLINE String operator+( const String& a, const unsigned b )
{
	char	text[ 64 ];
	String	result( a );
	
	sprintf( text, "%u", b );
	result.Append( text );
	
	return result;
}

BUD_INLINE String& String::operator+=( const float a )
{
	char text[ 64 ];
	
	sprintf( text, "%f", a );
	Append( text );
	
	return *this;
}

BUD_INLINE String& String::operator+=( const int a )
{
	char text[ 64 ];
	
	sprintf( text, "%d", a );
	Append( text );
	
	return *this;
}

BUD_INLINE String& String::operator+=( const unsigned a )
{
	char text[ 64 ];
	
	sprintf( text, "%u", a );
	Append( text );
	
	return *this;
}

BUD_INLINE String& String::operator+=( const String& a )
{
	Append( a );
	return *this;
}

BUD_INLINE String& String::operator+=( const char* a )
{
	Append( a );
	return *this;
}

BUD_INLINE String& String::operator+=( const char a )
{
	Append( a );
	return *this;
}

BUD_INLINE String& String::operator+=( const bool a )
{
	Append( a ? "true" : "false" );
	return *this;
}

BUD_INLINE bool operator==( const String& a, const String& b )
{
	return ( !String::Cmp( a.data, b.data ) );
}

BUD_INLINE bool operator==( const String& a, const char* b )
{
	assert( b );
	return ( !String::Cmp( a.data, b ) );
}

BUD_INLINE bool operator==( const char* a, const String& b )
{
	assert( a );
	return ( !String::Cmp( a, b.data ) );
}

BUD_INLINE bool operator!=( const String& a, const String& b )
{
	return !( a == b );
}

BUD_INLINE bool operator!=( const String& a, const char* b )
{
	return !( a == b );
}

BUD_INLINE bool operator!=( const char* a, const String& b )
{
	return !( a == b );
}

BUD_INLINE int String::Cmp( const char* text ) const
{
	assert( text );
	return String::Cmp( data, text );
}

BUD_INLINE int String::Cmpn( const char* text, int n ) const
{
	assert( text );
	return String::Cmpn( data, text, n );
}

BUD_INLINE int String::CmpPrefix( const char* text ) const
{
	assert( text );
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	return String::Cmpn( data, text, ( int )strlen( text ) );
	// RB end
}

BUD_INLINE int String::Icmp( const char* text ) const
{
	assert( text );
	return String::Icmp( data, text );
}

BUD_INLINE int String::Icmpn( const char* text, int n ) const
{
	assert( text );
	return String::Icmpn( data, text, n );
}

BUD_INLINE int String::IcmpPrefix( const char* text ) const
{
	assert( text );
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	return String::Icmpn( data, text, ( int )strlen( text ) );
	// RB end
}

BUD_INLINE int String::IcmpNoColor( const char* text ) const
{
	assert( text );
	return String::IcmpNoColor( data, text );
}

BUD_INLINE int String::IcmpPath( const char* text ) const
{
	assert( text );
	return String::IcmpPath( data, text );
}

BUD_INLINE int String::IcmpnPath( const char* text, int n ) const
{
	assert( text );
	return String::IcmpnPath( data, text, n );
}

BUD_INLINE int String::IcmpPrefixPath( const char* text ) const
{
	assert( text );
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	return String::IcmpnPath( data, text, ( int )strlen( text ) );
	// RB end
}

BUD_INLINE int String::Length() const
{
	return len;
}

BUD_INLINE int String::Allocated() const
{
	if( data != baseBuffer )
	{
		return GetAlloced();
	}
	else
	{
		return 0;
	}
}

BUD_INLINE void String::Empty()
{
	EnsureAlloced( 1 );
	data[ 0 ] = '\0';
	len = 0;
}

BUD_INLINE bool String::IsEmpty() const
{
	return ( String::Cmp( data, "" ) == 0 );
}

BUD_INLINE void String::Clear()
{
	if( IsStatic() )
	{
		len = 0;
		data[ 0 ] = '\0';
		return;
	}
	FreeData();
	Construct();
}

inline void String::Append( const char a )
{
	EnsureAlloced( len + 2 );
	data[ len ] = a;
	len++;
	data[len] = '\0';
}

BUD_INLINE void String::Append( const String& text )
{
	int newLen;
	int i;
	
	newLen = len + text.Length();
	EnsureAlloced( newLen + 1 );
	for( i = 0; i < text.len; i++ )
	{
		data[ len + i ] = text[ i ];
	}
	len = newLen;
	data[ len ] = '\0';
}

BUD_INLINE void String::Append( const char* text )
{
	int newLen;
	int i;
	
	if( text )
	{
		// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
		newLen = len + ( int )strlen( text );
		// RB end
		EnsureAlloced( newLen + 1 );
		for( i = 0; text[ i ]; i++ )
		{
			data[ len + i ] = text[ i ];
		}
		len = newLen;
		data[ len ] = '\0';
	}
}

BUD_INLINE void String::Append( const char* text, int l )
{
	int newLen;
	int i;
	
	if( text && l )
	{
		newLen = len + l;
		EnsureAlloced( newLen + 1 );
		for( i = 0; text[ i ] && i < l; i++ )
		{
			data[ len + i ] = text[ i ];
		}
		len = newLen;
		data[ len ] = '\0';
	}
}

BUD_INLINE void String::Insert( const char a, int index )
{
	int i, l;
	
	if( index < 0 )
	{
		index = 0;
	}
	else if( index > len )
	{
		index = len;
	}
	
	l = 1;
	EnsureAlloced( len + l + 1 );
	for( i = len; i >= index; i-- )
	{
		data[i + l] = data[i];
	}
	data[index] = a;
	len++;
}

BUD_INLINE void String::Insert( const char* text, int index )
{
	int i, l;
	
	if( index < 0 )
	{
		index = 0;
	}
	else if( index > len )
	{
		index = len;
	}
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = ( int )strlen( text );
	// RB end
	EnsureAlloced( len + l + 1 );
	for( i = len; i >= index; i-- )
	{
		data[i + l] = data[i];
	}
	for( i = 0; i < l; i++ )
	{
		data[index + i] = text[i];
	}
	len += l;
}

BUD_INLINE void String::ToLower()
{
	for( int i = 0; data[i]; i++ )
	{
		if( CharIsUpper( data[i] ) )
		{
			data[i] += ( 'a' - 'A' );
		}
	}
}

BUD_INLINE void String::ToUpper()
{
	for( int i = 0; data[i]; i++ )
	{
		if( CharIsLower( data[i] ) )
		{
			data[i] -= ( 'a' - 'A' );
		}
	}
}

BUD_INLINE bool String::IsNumeric() const
{
	return String::IsNumeric( data );
}

BUD_INLINE bool String::IsColor() const
{
	return String::IsColor( data );
}

BUD_INLINE bool String::HasLower() const
{
	return String::HasLower( data );
}

BUD_INLINE bool String::HasUpper() const
{
	return String::HasUpper( data );
}

BUD_INLINE String& String::RemoveColors()
{
	String::RemoveColors( data );
	len = Length( data );
	return *this;
}

BUD_INLINE int String::LengthWithoutColors() const
{
	return String::LengthWithoutColors( data );
}

BUD_INLINE void String::CapLength( int newlen )
{
	if( len <= newlen )
	{
		return;
	}
	data[ newlen ] = 0;
	len = newlen;
}

BUD_INLINE void String::Fill( const char ch, int newlen )
{
	EnsureAlloced( newlen + 1 );
	len = newlen;
	memset( data, ch, len );
	data[ len ] = 0;
}

/*
========================
String::UTF8Length
========================
*/
BUD_INLINE int String::UTF8Length()
{
	return UTF8Length( ( byte* )data );
}

/*
========================
String::UTF8Char
========================
*/
BUD_INLINE uint32 String::UTF8Char( int& idx )
{
	return UTF8Char( ( byte* )data, idx );
}

/*
========================
String::ConvertToUTF8
========================
*/
BUD_INLINE void String::ConvertToUTF8()
{
	String temp( *this );
	Clear();
	for( int index = 0; index < temp.Length(); ++index )
	{
		AppendUTF8Char( temp[index] );
	}
}

/*
========================
String::UTF8Char
========================
*/
BUD_INLINE uint32 String::UTF8Char( const char* s, int& idx )
{
	return UTF8Char( ( byte* )s, idx );
}

/*
========================
String::IsValidUTF8
========================
*/
BUD_INLINE bool String::IsValidUTF8( const uint8* s, const int maxLen )
{
	utf8Encoding_t encoding;
	return IsValidUTF8( s, maxLen, encoding );
}

BUD_INLINE int String::Find( const char c, int start, int end ) const
{
	if( end == -1 )
	{
		end = len;
	}
	return String::FindChar( data, c, start, end );
}

BUD_INLINE int String::Find( const char* text, bool casesensitive, int start, int end ) const
{
	if( end == -1 )
	{
		end = len;
	}
	return String::FindText( data, text, casesensitive, start, end );
}

BUD_INLINE bool String::Filter( const char* filter, bool casesensitive ) const
{
	return String::Filter( filter, data, casesensitive );
}

BUD_INLINE const char* String::Left( int len, String& result ) const
{
	return Mid( 0, len, result );
}

BUD_INLINE const char* String::Right( int len, String& result ) const
{
	if( len >= Length() )
	{
		result = *this;
		return result;
	}
	return Mid( Length() - len, len, result );
}

BUD_INLINE String String::Left( int len ) const
{
	return Mid( 0, len );
}

BUD_INLINE String String::Right( int len ) const
{
	if( len >= Length() )
	{
		return *this;
	}
	return Mid( Length() - len, len );
}

BUD_INLINE void String::Strip( const char c )
{
	StripLeading( c );
	StripTrailing( c );
}

BUD_INLINE void String::Strip( const char* string )
{
	StripLeading( string );
	StripTrailing( string );
}

BUD_INLINE bool String::CheckExtension( const char* ext )
{
	return String::CheckExtension( data, ext );
}

BUD_INLINE int String::Length( const char* s )
{
	int i;
	for( i = 0; s[i]; i++ ) {}
	return i;
}

BUD_INLINE char* String::ToLower( char* s )
{
	for( int i = 0; s[i]; i++ )
	{
		if( CharIsUpper( s[i] ) )
		{
			s[i] += ( 'a' - 'A' );
		}
	}
	return s;
}

BUD_INLINE char* String::ToUpper( char* s )
{
	for( int i = 0; s[i]; i++ )
	{
		if( CharIsLower( s[i] ) )
		{
			s[i] -= ( 'a' - 'A' );
		}
	}
	return s;
}

BUD_INLINE int String::Hash( const char* string )
{
	int i, hash = 0;
	for( i = 0; *string != '\0'; i++ )
	{
		hash += ( *string++ ) * ( i + 119 );
	}
	return hash;
}

BUD_INLINE int String::Hash( const char* string, int length )
{
	int i, hash = 0;
	for( i = 0; i < length; i++ )
	{
		hash += ( *string++ ) * ( i + 119 );
	}
	return hash;
}

BUD_INLINE int String::IHash( const char* string )
{
	int i, hash = 0;
	for( i = 0; *string != '\0'; i++ )
	{
		hash += ToLower( *string++ ) * ( i + 119 );
	}
	return hash;
}

BUD_INLINE int String::IHash( const char* string, int length )
{
	int i, hash = 0;
	for( i = 0; i < length; i++ )
	{
		hash += ToLower( *string++ ) * ( i + 119 );
	}
	return hash;
}

BUD_INLINE bool String::IsColor( const char* s )
{
	return ( s[0] == C_COLOR_ESCAPE && s[1] != '\0' && s[1] != ' ' );
}

BUD_INLINE char String::ToLower( char c )
{
	if( c <= 'Z' && c >= 'A' )
	{
		return ( c + ( 'a' - 'A' ) );
	}
	return c;
}

BUD_INLINE char String::ToUpper( char c )
{
	if( c >= 'a' && c <= 'z' )
	{
		return ( c - ( 'a' - 'A' ) );
	}
	return c;
}

BUD_INLINE bool String::CharIsPrintable( int c )
{
	// test for regular ascii and western European high-ascii chars
	return ( c >= 0x20 && c <= 0x7E ) || ( c >= 0xA1 && c <= 0xFF );
}

BUD_INLINE bool String::CharIsLower( int c )
{
	// test for regular ascii and western European high-ascii chars
	return ( c >= 'a' && c <= 'z' ) || ( c >= 0xE0 && c <= 0xFF );
}

BUD_INLINE bool String::CharIsUpper( int c )
{
	// test for regular ascii and western European high-ascii chars
	return ( c <= 'Z' && c >= 'A' ) || ( c >= 0xC0 && c <= 0xDF );
}

BUD_INLINE bool String::CharIsAlpha( int c )
{
	// test for regular ascii and western European high-ascii chars
	return ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ||
			 ( c >= 0xC0 && c <= 0xFF ) );
}

BUD_INLINE bool String::CharIsNumeric( int c )
{
	return ( c <= '9' && c >= '0' );
}

BUD_INLINE bool String::CharIsNewLine( char c )
{
	return ( c == '\n' || c == '\r' || c == '\v' );
}

BUD_INLINE bool String::CharIsTab( char c )
{
	return ( c == '\t' );
}

BUD_INLINE int String::ColorIndex( int c )
{
	return ( c & 15 );
}

BUD_INLINE int String::DynamicMemoryUsed() const
{
	return ( data == baseBuffer ) ? 0 : GetAlloced();
}

/*
========================
String::CopyRange
========================
*/
BUD_INLINE void String::CopyRange( const char* text, int start, int end )
{
	int l = end - start;
	if( l < 0 )
	{
		l = 0;
	}
	
	EnsureAlloced( l + 1 );
	
	for( int i = 0; i < l; i++ )
	{
		data[ i ] = text[ start + i ];
	}
	
	data[ l ] = '\0';
	len = l;
}

#endif /* !__STR_H__ */
