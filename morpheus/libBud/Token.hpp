/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.

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

#ifndef __TOKEN_H__
#define __TOKEN_H__

/*
===============================================================================

	budToken is a token read from a file or memory with budLexer or budParser

===============================================================================
*/

// token types
#define TT_STRING					1		// string
#define TT_LITERAL					2		// literal
#define TT_NUMBER					3		// number
#define TT_NAME						4		// name
#define TT_PUNCTUATION				5		// punctuation

// number sub types
#define TT_INTEGER					0x00001		// integer
#define TT_DECIMAL					0x00002		// decimal number
#define TT_HEX						0x00004		// hexadecimal number
#define TT_OCTAL					0x00008		// octal number
#define TT_BINARY					0x00010		// binary number
#define TT_LONG						0x00020		// long int
#define TT_UNSIGNED					0x00040		// unsigned int
#define TT_FLOAT					0x00080		// floating point number
#define TT_SINGLE_PRECISION			0x00100		// float
#define TT_DOUBLE_PRECISION			0x00200		// double
#define TT_EXTENDED_PRECISION		0x00400		// long double
#define TT_INFINITE					0x00800		// infinite 1.#INF
#define TT_INDEFINITE				0x01000		// indefinite 1.#IND
#define TT_NAN						0x02000		// NaN
#define TT_IPADDRESS				0x04000		// ip address
#define TT_IPPORT					0x08000		// ip port
#define TT_VALUESVALID				0x10000		// set if intvalue and floatvalue are valid

// string sub type is the length of the string
// literal sub type is the ASCII code
// punctuation sub type is the punctuation id
// name sub type is the length of the name

class budToken : public budStr
{

	friend class budParser;
	friend class budLexer;
	
public:
	int				type;								// token type
	int				subtype;							// token sub type
	int				line;								// line in script the token was on
	int				linesCrossed;						// number of lines crossed in white space before token
	int				flags;								// token flags, used for recursive defines
	
public:
	budToken();
	budToken( const budToken* token );
	~budToken();
	
	void			operator=( const budStr& text );
	void			operator=( const char* text );
	
	double			GetDoubleValue();				// double value of TT_NUMBER
	float			GetFloatValue();				// float value of TT_NUMBER
	unsigned long	GetUnsignedLongValue();		// unsigned long value of TT_NUMBER
	int				GetIntValue();				// int value of TT_NUMBER
	int				WhiteSpaceBeforeToken() const;// returns length of whitespace before token
	void			ClearTokenWhiteSpace();		// forget whitespace before token
	
	void			NumberValue();				// calculate values for a TT_NUMBER
	
private:
	// DG: use int instead of long for 64bit compatibility
	unsigned int	intvalue;							// integer value
	// DG end
	double			floatvalue;							// floating point value
	const char* 	whiteSpaceStart_p;					// start of white space before token, only used by budLexer
	const char* 	whiteSpaceEnd_p;					// end of white space before token, only used by budLexer
	budToken* 		next;								// next token in chain, only used by budParser
	
	void			AppendDirty( const char a );		// append character without adding trailing zero
};

BUD_INLINE budToken::budToken() : type(), subtype(), line(), linesCrossed(), flags()
{
}

BUD_INLINE budToken::budToken( const budToken* token )
{
	*this = *token;
}

BUD_INLINE budToken::~budToken()
{
}

BUD_INLINE void budToken::operator=( const char* text )
{
	*static_cast<budStr*>( this ) = text;
}

BUD_INLINE void budToken::operator=( const budStr& text )
{
	*static_cast<budStr*>( this ) = text;
}

BUD_INLINE double budToken::GetDoubleValue()
{
	if( type != TT_NUMBER )
	{
		return 0.0;
	}
	if( !( subtype & TT_VALUESVALID ) )
	{
		NumberValue();
	}
	return floatvalue;
}

BUD_INLINE float budToken::GetFloatValue()
{
	return ( float ) GetDoubleValue();
}

BUD_INLINE unsigned long	budToken::GetUnsignedLongValue()
{
	if( type != TT_NUMBER )
	{
		return 0;
	}
	if( !( subtype & TT_VALUESVALID ) )
	{
		NumberValue();
	}
	return intvalue;
}

BUD_INLINE int budToken::GetIntValue()
{
	return ( int ) GetUnsignedLongValue();
}

BUD_INLINE int budToken::WhiteSpaceBeforeToken() const
{
	return ( whiteSpaceEnd_p > whiteSpaceStart_p );
}

BUD_INLINE void budToken::AppendDirty( const char a )
{
	EnsureAlloced( len + 2, true );
	data[len++] = a;
}

#endif /* !__TOKEN_H__ */
