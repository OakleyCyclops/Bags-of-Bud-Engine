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

#include "PCH.hpp"
#pragma hdrstop

#define PUNCTABLE

//longer punctuations first
punctuation_t default_punctuations[] =
{
	//binary operators
	{">>=", P_RSHIFT_ASSIGN},
	{"<<=", P_LSHIFT_ASSIGN},
	//
	{"...", P_PARMS},
	//define merge operator
	{"##", P_PRECOMPMERGE},				// pre-compiler
	//logic operators
	{"&&", P_LOGIC_AND},					// pre-compiler
	{"||", P_LOGIC_OR},					// pre-compiler
	{">=", P_LOGIC_GEQ},					// pre-compiler
	{"<=", P_LOGIC_LEQ},					// pre-compiler
	{"==", P_LOGIC_EQ},					// pre-compiler
	{"!=", P_LOGIC_UNEQ},				// pre-compiler
	//arithmatic operators
	{"*=", P_MUL_ASSIGN},
	{"/=", P_DIV_ASSIGN},
	{"%=", P_MOD_ASSIGN},
	{"+=", P_ADD_ASSIGN},
	{"-=", P_SUB_ASSIGN},
	{"++", P_INC},
	{"--", P_DEC},
	//binary operators
	{"&=", P_BIN_AND_ASSIGN},
	{"|=", P_BIN_OR_ASSIGN},
	{"^=", P_BIN_XOR_ASSIGN},
	{">>", P_RSHIFT},					// pre-compiler
	{"<<", P_LSHIFT},					// pre-compiler
	//reference operators
	{"->", P_POINTERREF},
	//C++
	{"::", P_CPP1},
	{".*", P_CPP2},
	//arithmatic operators
	{"*", P_MUL},						// pre-compiler
	{"/", P_DIV},						// pre-compiler
	{"%", P_MOD},						// pre-compiler
	{"+", P_ADD},						// pre-compiler
	{"-", P_SUB},						// pre-compiler
	{"=", P_ASSIGN},
	//binary operators
	{"&", P_BIN_AND},					// pre-compiler
	{"|", P_BIN_OR},						// pre-compiler
	{"^", P_BIN_XOR},					// pre-compiler
	{"~", P_BIN_NOT},					// pre-compiler
	//logic operators
	{"!", P_LOGIC_NOT},					// pre-compiler
	{">", P_LOGIC_GREATER},				// pre-compiler
	{"<", P_LOGIC_LESS},					// pre-compiler
	//reference operator
	{".", P_REF},
	//seperators
	{",", P_COMMA},						// pre-compiler
	{";", P_SEMICOLON},
	//label indication
	{":", P_COLON},						// pre-compiler
	//if statement
	{"?", P_QUESTIONMARK},				// pre-compiler
	//embracements
	{"(", P_PARENTHESESOPEN},			// pre-compiler
	{")", P_PARENTHESESCLOSE},			// pre-compiler
	{"{", P_BRACEOPEN},					// pre-compiler
	{"}", P_BRACECLOSE},					// pre-compiler
	{"[", P_SQBRACKETOPEN},
	{"]", P_SQBRACKETCLOSE},
	//
	{"\\", P_BACKSLASH},
	//precompiler operator
	{"#", P_PRECOMP},					// pre-compiler
	{"$", P_DOLLAR},
	{NULL, 0}
};

int default_punctuationtable[256];
int default_nextpunctuation[sizeof( default_punctuations ) / sizeof( punctuation_t )];
int default_setup;

char budLexer::baseFolder[ 256 ];

/*
================
budLexer::CreatePunctuationTable
================
*/
void budLexer::CreatePunctuationTable( const punctuation_t* punctuations )
{
	int i, n, lastp;
	const punctuation_t* p, *newp;
	
	//get memory for the table
	if( punctuations == default_punctuations )
	{
		budLexer::punctuationtable = default_punctuationtable;
		budLexer::nextpunctuation = default_nextpunctuation;
		if( default_setup )
		{
			return;
		}
		default_setup = true;
		i = sizeof( default_punctuations ) / sizeof( punctuation_t );
	}
	else
	{
		if( !budLexer::punctuationtable || budLexer::punctuationtable == default_punctuationtable )
		{
			budLexer::punctuationtable = ( int* ) Mem_Alloc( 256 * sizeof( int ), TAG_libBud_LEXER );
		}
		if( budLexer::nextpunctuation && budLexer::nextpunctuation != default_nextpunctuation )
		{
			Mem_Free( budLexer::nextpunctuation );
		}
		for( i = 0; punctuations[i].p; i++ )
		{
		}
		budLexer::nextpunctuation = ( int* ) Mem_Alloc( i * sizeof( int ), TAG_libBud_LEXER );
	}
	memset( budLexer::punctuationtable, 0xFF, 256 * sizeof( int ) );
	memset( budLexer::nextpunctuation, 0xFF, i * sizeof( int ) );
	//add the punctuations in the list to the punctuation table
	for( i = 0; punctuations[i].p; i++ )
	{
		newp = &punctuations[i];
		lastp = -1;
		//sort the punctuations in this table entry on length (longer punctuations first)
		for( n = budLexer::punctuationtable[( unsigned int ) newp->p[0]]; n >= 0; n = budLexer::nextpunctuation[n] )
		{
			p = &punctuations[n];
			if( strlen( p->p ) < strlen( newp->p ) )
			{
				budLexer::nextpunctuation[i] = n;
				if( lastp >= 0 )
				{
					budLexer::nextpunctuation[lastp] = i;
				}
				else
				{
					budLexer::punctuationtable[( unsigned int ) newp->p[0]] = i;
				}
				break;
			}
			lastp = n;
		}
		if( n < 0 )
		{
			budLexer::nextpunctuation[i] = -1;
			if( lastp >= 0 )
			{
				budLexer::nextpunctuation[lastp] = i;
			}
			else
			{
				budLexer::punctuationtable[( unsigned int ) newp->p[0]] = i;
			}
		}
	}
}

/*
================
budLexer::GetPunctuationFromId
================
*/
const char* budLexer::GetPunctuationFromId( int id )
{
	int i;
	
	for( i = 0; budLexer::punctuations[i].p; i++ )
	{
		if( budLexer::punctuations[i].n == id )
		{
			return budLexer::punctuations[i].p;
		}
	}
	return "unknown punctuation";
}

/*
================
budLexer::GetPunctuationId
================
*/
int budLexer::GetPunctuationId( const char* p )
{
	int i;
	
	for( i = 0; budLexer::punctuations[i].p; i++ )
	{
		if( !strcmp( budLexer::punctuations[i].p, p ) )
		{
			return budLexer::punctuations[i].n;
		}
	}
	return 0;
}

/*
================
budLexer::Error
================
*/
void budLexer::Error( const char* str, ... )
{
	char text[MAX_STRING_CHARS];
	va_list ap;
	
	hadError = true;
	
	if( budLexer::flags & LEXFL_NOERRORS )
	{
		return;
	}
	
	va_start( ap, str );
	vsprintf( text, str, ap );
	va_end( ap );
	
	if( budLexer::flags & LEXFL_NOFATALERRORS )
	{
		libBud::common->Warning( "file %s, line %d: %s", budLexer::filename.c_str(), budLexer::line, text );
	}
	else
	{
		libBud::common->Error( "file %s, line %d: %s", budLexer::filename.c_str(), budLexer::line, text );
	}
}

/*
================
budLexer::Warning
================
*/
void budLexer::Warning( const char* str, ... )
{
	char text[MAX_STRING_CHARS];
	va_list ap;
	
	if( budLexer::flags & LEXFL_NOWARNINGS )
	{
		return;
	}
	
	va_start( ap, str );
	vsprintf( text, str, ap );
	va_end( ap );
	libBud::common->Warning( "file %s, line %d: %s", budLexer::filename.c_str(), budLexer::line, text );
}

/*
================
budLexer::SetPunctuations
================
*/
void budLexer::SetPunctuations( const punctuation_t* p )
{
#ifdef PUNCTABLE
	if( p )
	{
		budLexer::CreatePunctuationTable( p );
	}
	else
	{
		budLexer::CreatePunctuationTable( default_punctuations );
	}
#endif //PUNCTABLE
	if( p )
	{
		budLexer::punctuations = p;
	}
	else
	{
		budLexer::punctuations = default_punctuations;
	}
}

/*
================
budLexer::ReadWhiteSpace

Reads spaces, tabs, C-like comments etc.
When a newline character is found the scripts line counter is increased.
================
*/
int budLexer::ReadWhiteSpace()
{
	while( 1 )
	{
		// skip white space
		while( *budLexer::script_p <= ' ' )
		{
			if( !*budLexer::script_p )
			{
				return 0;
			}
			if( *budLexer::script_p == '\n' )
			{
				budLexer::line++;
			}
			budLexer::script_p++;
		}
		// skip comments
		if( *budLexer::script_p == '/' )
		{
			// comments //
			if( *( budLexer::script_p + 1 ) == '/' )
			{
				budLexer::script_p++;
				do
				{
					budLexer::script_p++;
					if( !*budLexer::script_p )
					{
						return 0;
					}
				}
				while( *budLexer::script_p != '\n' );
				budLexer::line++;
				budLexer::script_p++;
				if( !*budLexer::script_p )
				{
					return 0;
				}
				continue;
			}
			// comments /* */
			else if( *( budLexer::script_p + 1 ) == '*' )
			{
				budLexer::script_p++;
				while( 1 )
				{
					budLexer::script_p++;
					if( !*budLexer::script_p )
					{
						return 0;
					}
					if( *budLexer::script_p == '\n' )
					{
						budLexer::line++;
					}
					else if( *budLexer::script_p == '/' )
					{
						if( *( budLexer::script_p - 1 ) == '*' )
						{
							break;
						}
						if( *( budLexer::script_p + 1 ) == '*' )
						{
							budLexer::Warning( "nested comment" );
						}
					}
				}
				budLexer::script_p++;
				if( !*budLexer::script_p )
				{
					return 0;
				}
				budLexer::script_p++;
				if( !*budLexer::script_p )
				{
					return 0;
				}
				continue;
			}
		}
		break;
	}
	return 1;
}

/*
========================
budLexer::SkipWhiteSpace

Reads spaces, tabs, C-like comments etc. When a newline character is found, the scripts line
counter is increased. Returns false if there is no token left to be read.
========================
*/
bool budLexer::SkipWhiteSpace( bool currentLine )
{
	while( 1 )
	{
		assert( script_p <= end_p );
		if( script_p == end_p )
		{
			return false;
		}
		// skip white space
		while( *script_p <= ' ' )
		{
			if( script_p == end_p )
			{
				return false;
			}
			if( !*script_p )
			{
				return false;
			}
			if( *script_p == '\n' )
			{
				line++;
				if( currentLine )
				{
					script_p++;
					return true;
				}
			}
			script_p++;
		}
		// skip comments
		if( *script_p == '/' )
		{
			// comments //
			if( *( script_p + 1 ) == '/' )
			{
				script_p++;
				do
				{
					script_p++;
					if( !*script_p )
					{
						return false;
					}
				}
				while( *script_p != '\n' );
				line++;
				script_p++;
				if( currentLine )
				{
					return true;
				}
				if( !*script_p )
				{
					return false;
				}
				continue;
			}
			// comments /* */
			else if( *( script_p + 1 ) == '*' )
			{
				script_p++;
				while( 1 )
				{
					script_p++;
					if( !*script_p )
					{
						return false;
					}
					if( *script_p == '\n' )
					{
						line++;
					}
					else if( *script_p == '/' )
					{
						if( *( script_p - 1 ) == '*' )
						{
							break;
						}
						if( *( script_p + 1 ) == '*' )
						{
							Warning( "nested comment" );
						}
					}
				}
				script_p++;
				if( !*script_p )
				{
					return false;
				}
				continue;
			}
		}
		break;
	}
	return true;
}

/*
================
budLexer::ReadEscapeCharacter
================
*/
int budLexer::ReadEscapeCharacter( char* ch )
{
	int c, val, i;
	
	// step over the leading '\\'
	budLexer::script_p++;
	// determine the escape character
	switch( *budLexer::script_p )
	{
		case '\\':
			c = '\\';
			break;
		case 'n':
			c = '\n';
			break;
		case 'r':
			c = '\r';
			break;
		case 't':
			c = '\t';
			break;
		case 'v':
			c = '\v';
			break;
		case 'b':
			c = '\b';
			break;
		case 'f':
			c = '\f';
			break;
		case 'a':
			c = '\a';
			break;
		case '\'':
			c = '\'';
			break;
		case '\"':
			c = '\"';
			break;
		case '\?':
			c = '\?';
			break;
		case 'x':
		{
			budLexer::script_p++;
			for( i = 0, val = 0; ; i++, budLexer::script_p++ )
			{
				c = *budLexer::script_p;
				if( c >= '0' && c <= '9' )
					c = c - '0';
				else if( c >= 'A' && c <= 'Z' )
					c = c - 'A' + 10;
				else if( c >= 'a' && c <= 'z' )
					c = c - 'a' + 10;
				else
					break;
				val = ( val << 4 ) + c;
			}
			budLexer::script_p--;
			if( val > 0xFF )
			{
				budLexer::Warning( "too large value in escape character" );
				val = 0xFF;
			}
			c = val;
			break;
		}
		default: //NOTE: decimal ASCII code, NOT octal
		{
			if( *budLexer::script_p < '0' || *budLexer::script_p > '9' )
			{
				budLexer::Error( "unknown escape char" );
			}
			for( i = 0, val = 0; ; i++, budLexer::script_p++ )
			{
				c = *budLexer::script_p;
				if( c >= '0' && c <= '9' )
					c = c - '0';
				else
					break;
				val = val * 10 + c;
			}
			budLexer::script_p--;
			if( val > 0xFF )
			{
				budLexer::Warning( "too large value in escape character" );
				val = 0xFF;
			}
			c = val;
			break;
		}
	}
	// step over the escape character or the last digit of the number
	budLexer::script_p++;
	// store the escape character
	*ch = c;
	// succesfully read escape character
	return 1;
}

/*
================
budLexer::ReadString

Escape characters are interpretted.
Reads two strings with only a white space between them as one string.
================
*/
int budLexer::ReadString( budToken* token, int quote )
{
	int tmpline;
	const char* tmpscript_p;
	char ch;
	
	if( quote == '\"' )
	{
		token->type = TT_STRING;
	}
	else
	{
		token->type = TT_LITERAL;
	}
	
	// leading quote
	budLexer::script_p++;
	
	while( 1 )
	{
		// if there is an escape character and escape characters are allowed
		if( *budLexer::script_p == '\\' && !( budLexer::flags & LEXFL_NOSTRINGESCAPECHARS ) )
		{
			if( !budLexer::ReadEscapeCharacter( &ch ) )
			{
				return 0;
			}
			token->AppendDirty( ch );
		}
		// if a trailing quote
		else if( *budLexer::script_p == quote )
		{
			// step over the quote
			budLexer::script_p++;
			// if consecutive strings should not be concatenated
			if( ( budLexer::flags & LEXFL_NOSTRINGCONCAT ) &&
					( !( budLexer::flags & LEXFL_ALLOWBACKSLASHSTRINGCONCAT ) || ( quote != '\"' ) ) )
			{
				break;
			}
			
			tmpscript_p = budLexer::script_p;
			tmpline = budLexer::line;
			// read white space between possible two consecutive strings
			if( !budLexer::ReadWhiteSpace() )
			{
				budLexer::script_p = tmpscript_p;
				budLexer::line = tmpline;
				break;
			}
			
			if( budLexer::flags & LEXFL_NOSTRINGCONCAT )
			{
				if( *budLexer::script_p != '\\' )
				{
					budLexer::script_p = tmpscript_p;
					budLexer::line = tmpline;
					break;
				}
				// step over the '\\'
				budLexer::script_p++;
				if( !budLexer::ReadWhiteSpace() || ( *budLexer::script_p != quote ) )
				{
					budLexer::Error( "expecting string after '\' terminated line" );
					return 0;
				}
			}
			
			// if there's no leading qoute
			if( *budLexer::script_p != quote )
			{
				budLexer::script_p = tmpscript_p;
				budLexer::line = tmpline;
				break;
			}
			// step over the new leading quote
			budLexer::script_p++;
		}
		else
		{
			if( *budLexer::script_p == '\0' )
			{
				budLexer::Error( "missing trailing quote" );
				return 0;
			}
			if( *budLexer::script_p == '\n' )
			{
				budLexer::Error( "newline inside string" );
				return 0;
			}
			token->AppendDirty( *budLexer::script_p++ );
		}
	}
	token->data[token->len] = '\0';
	
	if( token->type == TT_LITERAL )
	{
		if( !( budLexer::flags & LEXFL_ALLOWMULTICHARLITERALS ) )
		{
			if( token->Length() != 1 )
			{
				budLexer::Warning( "literal is not one character long" );
			}
		}
		token->subtype = ( *token )[0];
	}
	else
	{
		// the sub type is the length of the string
		token->subtype = token->Length();
	}
	return 1;
}

/*
================
budLexer::ReadName
================
*/
int budLexer::ReadName( budToken* token )
{
	char c;
	
	token->type = TT_NAME;
	do
	{
		token->AppendDirty( *budLexer::script_p++ );
		c = *budLexer::script_p;
	}
	while( ( c >= 'a' && c <= 'z' ) ||
			( c >= 'A' && c <= 'Z' ) ||
			( c >= '0' && c <= '9' ) ||
			c == '_' ||
			// if treating all tokens as strings, don't parse '-' as a separate token
			( ( budLexer::flags & LEXFL_ONLYSTRINGS ) && ( c == '-' ) ) ||
			// if special path name characters are allowed
			( ( budLexer::flags & LEXFL_ALLOWPATHNAMES ) && ( c == '/' || c == '\\' || c == ':' || c == '.' ) ) );
	token->data[token->len] = '\0';
	//the sub type is the length of the name
	token->subtype = token->Length();
	return 1;
}

/*
================
budLexer::CheckString
================
*/
BUD_INLINE int budLexer::CheckString( const char* str ) const
{
	int i;
	
	for( i = 0; str[i]; i++ )
	{
		if( budLexer::script_p[i] != str[i] )
		{
			return false;
		}
	}
	return true;
}

/*
================
budLexer::ReadNumber
================
*/
int budLexer::ReadNumber( budToken* token )
{
	int i;
	int dot;
	char c, c2;
	
	token->type = TT_NUMBER;
	token->subtype = 0;
	token->intvalue = 0;
	token->floatvalue = 0;
	
	c = *budLexer::script_p;
	c2 = *( budLexer::script_p + 1 );
	
	if( c == '0' && c2 != '.' )
	{
		// check for a hexadecimal number
		if( c2 == 'x' || c2 == 'X' )
		{
			token->AppendDirty( *budLexer::script_p++ );
			token->AppendDirty( *budLexer::script_p++ );
			c = *budLexer::script_p;
			while( ( c >= '0' && c <= '9' ) ||
					( c >= 'a' && c <= 'f' ) ||
					( c >= 'A' && c <= 'F' ) )
			{
				token->AppendDirty( c );
				c = *( ++budLexer::script_p );
			}
			token->subtype = TT_HEX | TT_INTEGER;
		}
		// check for a binary number
		else if( c2 == 'b' || c2 == 'B' )
		{
			token->AppendDirty( *budLexer::script_p++ );
			token->AppendDirty( *budLexer::script_p++ );
			c = *budLexer::script_p;
			while( c == '0' || c == '1' )
			{
				token->AppendDirty( c );
				c = *( ++budLexer::script_p );
			}
			token->subtype = TT_BINARY | TT_INTEGER;
		}
		// its an octal number
		else
		{
			token->AppendDirty( *budLexer::script_p++ );
			c = *budLexer::script_p;
			while( c >= '0' && c <= '7' )
			{
				token->AppendDirty( c );
				c = *( ++budLexer::script_p );
			}
			token->subtype = TT_OCTAL | TT_INTEGER;
		}
	}
	else
	{
		// decimal integer or floating point number or ip address
		dot = 0;
		while( 1 )
		{
			if( c >= '0' && c <= '9' )
			{
			}
			else if( c == '.' )
			{
				dot++;
			}
			else
			{
				break;
			}
			token->AppendDirty( c );
			c = *( ++budLexer::script_p );
		}
		if( c == 'e' && dot == 0 )
		{
			//We have scientific notation without a decimal point
			dot++;
		}
		// if a floating point number
		if( dot == 1 )
		{
			token->subtype = TT_DECIMAL | TT_FLOAT;
			// check for floating point exponent
			if( c == 'e' )
			{
				//Append the e so that GetFloatValue code works
				token->AppendDirty( c );
				c = *( ++budLexer::script_p );
				if( c == '-' )
				{
					token->AppendDirty( c );
					c = *( ++budLexer::script_p );
				}
				else if( c == '+' )
				{
					token->AppendDirty( c );
					c = *( ++budLexer::script_p );
				}
				while( c >= '0' && c <= '9' )
				{
					token->AppendDirty( c );
					c = *( ++budLexer::script_p );
				}
			}
			// check for floating point exception infinite 1.#INF or indefinite 1.#IND or NaN
			else if( c == '#' )
			{
				c2 = 4;
				if( CheckString( "INF" ) )
				{
					token->subtype |= TT_INFINITE;
				}
				else if( CheckString( "IND" ) )
				{
					token->subtype |= TT_INDEFINITE;
				}
				else if( CheckString( "NAN" ) )
				{
					token->subtype |= TT_NAN;
				}
				else if( CheckString( "QNAN" ) )
				{
					token->subtype |= TT_NAN;
					c2++;
				}
				else if( CheckString( "SNAN" ) )
				{
					token->subtype |= TT_NAN;
					c2++;
				}
				for( i = 0; i < c2; i++ )
				{
					token->AppendDirty( c );
					c = *( ++budLexer::script_p );
				}
				while( c >= '0' && c <= '9' )
				{
					token->AppendDirty( c );
					c = *( ++budLexer::script_p );
				}
				if( !( budLexer::flags & LEXFL_ALLOWFLOATEXCEPTIONS ) )
				{
					token->AppendDirty( 0 );	// zero terminate for c_str
					budLexer::Error( "parsed %s", token->c_str() );
				}
			}
		}
		else if( dot > 1 )
		{
			if( !( budLexer::flags & LEXFL_ALLOWIPADDRESSES ) )
			{
				budLexer::Error( "more than one dot in number" );
				return 0;
			}
			if( dot != 3 )
			{
				budLexer::Error( "ip address should have three dots" );
				return 0;
			}
			token->subtype = TT_IPADDRESS;
		}
		else
		{
			token->subtype = TT_DECIMAL | TT_INTEGER;
		}
	}
	
	if( token->subtype & TT_FLOAT )
	{
		if( c > ' ' )
		{
			// single-precision: float
			if( c == 'f' || c == 'F' )
			{
				token->subtype |= TT_SINGLE_PRECISION;
				budLexer::script_p++;
			}
			// extended-precision: long double
			else if( c == 'l' || c == 'L' )
			{
				token->subtype |= TT_EXTENDED_PRECISION;
				budLexer::script_p++;
			}
			// default is double-precision: double
			else
			{
				token->subtype |= TT_DOUBLE_PRECISION;
			}
		}
		else
		{
			token->subtype |= TT_DOUBLE_PRECISION;
		}
	}
	else if( token->subtype & TT_INTEGER )
	{
		if( c > ' ' )
		{
			// default: signed long
			for( i = 0; i < 2; i++ )
			{
				// long integer
				if( c == 'l' || c == 'L' )
				{
					token->subtype |= TT_LONG;
				}
				// unsigned integer
				else if( c == 'u' || c == 'U' )
				{
					token->subtype |= TT_UNSIGNED;
				}
				else
				{
					break;
				}
				c = *( ++budLexer::script_p );
			}
		}
	}
	else if( token->subtype & TT_IPADDRESS )
	{
		if( c == ':' )
		{
			token->AppendDirty( c );
			c = *( ++budLexer::script_p );
			while( c >= '0' && c <= '9' )
			{
				token->AppendDirty( c );
				c = *( ++budLexer::script_p );
			}
			token->subtype |= TT_IPPORT;
		}
	}
	token->data[token->len] = '\0';
	return 1;
}

/*
================
budLexer::ReadPunctuation
================
*/
int budLexer::ReadPunctuation( budToken* token )
{
	int l, n, i;
	const char* p;
	const punctuation_t* punc;
	
#ifdef PUNCTABLE
	for( n = budLexer::punctuationtable[( unsigned int ) * ( budLexer::script_p )]; n >= 0; n = budLexer::nextpunctuation[n] )
	{
		punc = &( budLexer::punctuations[n] );
#else
	int i;
		
	for( i = 0; budLexer::punctuations[i].p; i++ )
	{
		punc = &budLexer::punctuations[i];
#endif
		p = punc->p;
		// check for this punctuation in the script
		for( l = 0; p[l] && budLexer::script_p[l]; l++ )
		{
			if( budLexer::script_p[l] != p[l] )
			{
				break;
			}
		}
		if( !p[l] )
		{
			//
			token->EnsureAlloced( l + 1, false );
			for( i = 0; i <= l; i++ )
			{
				token->data[i] = p[i];
			}
			token->len = l;
			//
			budLexer::script_p += l;
			token->type = TT_PUNCTUATION;
			// sub type is the punctuation id
			token->subtype = punc->n;
			return 1;
		}
	}
	return 0;
}

/*
================
budLexer::ReadToken
================
*/
int budLexer::ReadToken( budToken* token )
{
	int c;
	
	if( !loaded )
	{
		libBud::common->Error( "budLexer::ReadToken: no file loaded" );
		return 0;
	}
	
	if( script_p == NULL )
	{
		return 0;
	}
	
	// if there is a token available (from unreadToken)
	if( tokenavailable )
	{
		tokenavailable = 0;
		*token = budLexer::token;
		return 1;
	}
	// save script pointer
	lastScript_p = script_p;
	// save line counter
	lastline = line;
	// clear the token stuff
	token->data[0] = '\0';
	token->len = 0;
	// start of the white space
	whiteSpaceStart_p = script_p;
	token->whiteSpaceStart_p = script_p;
	// read white space before token
	if( !ReadWhiteSpace() )
	{
		return 0;
	}
	// end of the white space
	budLexer::whiteSpaceEnd_p = script_p;
	token->whiteSpaceEnd_p = script_p;
	// line the token is on
	token->line = line;
	// number of lines crossed before token
	token->linesCrossed = line - lastline;
	// clear token flags
	token->flags = 0;
	
	c = *budLexer::script_p;
	
	// if we're keeping everything as whitespace deliminated strings
	if( budLexer::flags & LEXFL_ONLYSTRINGS )
	{
		// if there is a leading quote
		if( c == '\"' || c == '\'' )
		{
			if( !budLexer::ReadString( token, c ) )
			{
				return 0;
			}
		}
		else if( !budLexer::ReadName( token ) )
		{
			return 0;
		}
	}
	// if there is a number
	else if( ( c >= '0' && c <= '9' ) ||
			 ( c == '.' && ( *( budLexer::script_p + 1 ) >= '0' && *( budLexer::script_p + 1 ) <= '9' ) ) )
	{
		if( !budLexer::ReadNumber( token ) )
		{
			return 0;
		}
		// if names are allowed to start with a number
		if( budLexer::flags & LEXFL_ALLOWNUMBERNAMES )
		{
			c = *budLexer::script_p;
			if( ( c >= 'a' && c <= 'z' ) ||	( c >= 'A' && c <= 'Z' ) || c == '_' )
			{
				if( !budLexer::ReadName( token ) )
				{
					return 0;
				}
			}
		}
	}
	// if there is a leading quote
	else if( c == '\"' || c == '\'' )
	{
		if( !budLexer::ReadString( token, c ) )
		{
			return 0;
		}
	}
	// if there is a name
	else if( ( c >= 'a' && c <= 'z' ) ||	( c >= 'A' && c <= 'Z' ) || c == '_' )
	{
		if( !budLexer::ReadName( token ) )
		{
			return 0;
		}
	}
	// names may also start with a slash when pathnames are allowed
	else if( ( budLexer::flags & LEXFL_ALLOWPATHNAMES ) && ( ( c == '/' || c == '\\' ) || c == '.' ) )
	{
		if( !budLexer::ReadName( token ) )
		{
			return 0;
		}
	}
	// check for punctuations
	else if( !budLexer::ReadPunctuation( token ) )
	{
		budLexer::Error( "unknown punctuation %c", c );
		return 0;
	}
	// succesfully read a token
	return 1;
}

/*
================
budLexer::ExpectTokenString
================
*/
int budLexer::ExpectTokenString( const char* string )
{
	budToken token;
	
	if( !budLexer::ReadToken( &token ) )
	{
		budLexer::Error( "couldn't find expected '%s'", string );
		return 0;
	}
	if( token != string )
	{
		budLexer::Error( "expected '%s' but found '%s'", string, token.c_str() );
		return 0;
	}
	return 1;
}

/*
================
budLexer::ExpectTokenType
================
*/
int budLexer::ExpectTokenType( int type, int subtype, budToken* token )
{
	budStr str;
	
	if( !budLexer::ReadToken( token ) )
	{
		budLexer::Error( "couldn't read expected token" );
		return 0;
	}
	
	if( token->type != type )
	{
		switch( type )
		{
			case TT_STRING:
				str = "string";
				break;
			case TT_LITERAL:
				str = "literal";
				break;
			case TT_NUMBER:
				str = "number";
				break;
			case TT_NAME:
				str = "name";
				break;
			case TT_PUNCTUATION:
				str = "punctuation";
				break;
			default:
				str = "unknown type";
				break;
		}
		budLexer::Error( "expected a %s but found '%s'", str.c_str(), token->c_str() );
		return 0;
	}
	if( token->type == TT_NUMBER )
	{
		if( ( token->subtype & subtype ) != subtype )
		{
			str.Clear();
			if( subtype & TT_DECIMAL ) str = "decimal ";
			if( subtype & TT_HEX ) str = "hex ";
			if( subtype & TT_OCTAL ) str = "octal ";
			if( subtype & TT_BINARY ) str = "binary ";
			if( subtype & TT_UNSIGNED ) str += "unsigned ";
			if( subtype & TT_LONG ) str += "long ";
			if( subtype & TT_FLOAT ) str += "float ";
			if( subtype & TT_INTEGER ) str += "integer ";
			str.StripTrailing( ' ' );
			budLexer::Error( "expected %s but found '%s'", str.c_str(), token->c_str() );
			return 0;
		}
	}
	else if( token->type == TT_PUNCTUATION )
	{
		if( subtype < 0 )
		{
			budLexer::Error( "BUG: wrong punctuation subtype" );
			return 0;
		}
		if( token->subtype != subtype )
		{
			budLexer::Error( "expected '%s' but found '%s'", GetPunctuationFromId( subtype ), token->c_str() );
			return 0;
		}
	}
	return 1;
}

/*
================
budLexer::ExpectAnyToken
================
*/
int budLexer::ExpectAnyToken( budToken* token )
{
	if( !budLexer::ReadToken( token ) )
	{
		budLexer::Error( "couldn't read expected token" );
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
================
budLexer::CheckTokenString
================
*/
int budLexer::CheckTokenString( const char* string )
{
	budToken tok;
	
	if( !ReadToken( &tok ) )
	{
		return 0;
	}
	// if the given string is available
	if( tok == string )
	{
		return 1;
	}
	// unread token
	script_p = lastScript_p;
	line = lastline;
	return 0;
}

/*
================
budLexer::CheckTokenType
================
*/
int budLexer::CheckTokenType( int type, int subtype, budToken* token )
{
	budToken tok;
	
	if( !ReadToken( &tok ) )
	{
		return 0;
	}
	// if the type matches
	if( tok.type == type && ( tok.subtype & subtype ) == subtype )
	{
		*token = tok;
		return 1;
	}
	// unread token
	script_p = lastScript_p;
	line = lastline;
	return 0;
}

/*
================
budLexer::PeekTokenString
================
*/
int budLexer::PeekTokenString( const char* string )
{
	budToken tok;
	
	if( !ReadToken( &tok ) )
	{
		return 0;
	}
	
	// unread token
	script_p = lastScript_p;
	line = lastline;
	
	// if the given string is available
	if( tok == string )
	{
		return 1;
	}
	return 0;
}

/*
================
budLexer::PeekTokenType
================
*/
int budLexer::PeekTokenType( int type, int subtype, budToken* token )
{
	budToken tok;
	
	if( !ReadToken( &tok ) )
	{
		return 0;
	}
	
	// unread token
	script_p = lastScript_p;
	line = lastline;
	
	// if the type matches
	if( tok.type == type && ( tok.subtype & subtype ) == subtype )
	{
		*token = tok;
		return 1;
	}
	return 0;
}

/*
================
budLexer::SkipUntilString
================
*/
int budLexer::SkipUntilString( const char* string )
{
	budToken token;
	
	while( budLexer::ReadToken( &token ) )
	{
		if( token == string )
		{
			return 1;
		}
	}
	return 0;
}

/*
================
budLexer::SkipRestOfLine
================
*/
int budLexer::SkipRestOfLine()
{
	budToken token;
	
	while( budLexer::ReadToken( &token ) )
	{
		if( token.linesCrossed )
		{
			budLexer::script_p = lastScript_p;
			budLexer::line = lastline;
			return 1;
		}
	}
	return 0;
}

/*
=================
budLexer::SkipBracedSection

Skips until a matching close brace is found.
Internal brace depths are properly skipped.
=================
*/
int budLexer::SkipBracedSection( bool parseFirstBrace )
{
	budToken token;
	int depth;
	
	depth = parseFirstBrace ? 0 : 1;
	do
	{
		if( !ReadToken( &token ) )
		{
			return false;
		}
		if( token.type == TT_PUNCTUATION )
		{
			if( token == "{" )
			{
				depth++;
			}
			else if( token == "}" )
			{
				depth--;
			}
		}
	}
	while( depth );
	return true;
}

/*
================
budLexer::UnreadToken
================
*/
void budLexer::UnreadToken( const budToken* token )
{
	if( budLexer::tokenavailable )
	{
		libBud::common->FatalError( "budLexer::unreadToken, unread token twice\n" );
	}
	budLexer::token = *token;
	budLexer::tokenavailable = 1;
}

/*
================
budLexer::ReadTokenOnLine
================
*/
int budLexer::ReadTokenOnLine( budToken* token )
{
	budToken tok;
	
	if( !budLexer::ReadToken( &tok ) )
	{
		budLexer::script_p = lastScript_p;
		budLexer::line = lastline;
		return false;
	}
	// if no lines were crossed before this token
	if( !tok.linesCrossed )
	{
		*token = tok;
		return true;
	}
	// restore our position
	budLexer::script_p = lastScript_p;
	budLexer::line = lastline;
	token->Clear();
	return false;
}

/*
================
budLexer::ReadRestOfLine
================
*/
const char*	budLexer::ReadRestOfLine( budStr& out )
{
	while( 1 )
	{
	
		if( *budLexer::script_p == '\n' )
		{
			budLexer::line++;
			break;
		}
		
		if( !*budLexer::script_p )
		{
			break;
		}
		
		if( *budLexer::script_p <= ' ' )
		{
			out += " ";
		}
		else
		{
			out += *budLexer::script_p;
		}
		budLexer::script_p++;
		
	}
	
	out.Strip( ' ' );
	return out.c_str();
}

/*
================
budLexer::ParseInt
================
*/
int budLexer::ParseInt()
{
	budToken token;
	
	if( !budLexer::ReadToken( &token ) )
	{
		budLexer::Error( "couldn't read expected integer" );
		return 0;
	}
	if( token.type == TT_PUNCTUATION && token == "-" )
	{
		budLexer::ExpectTokenType( TT_NUMBER, TT_INTEGER, &token );
		return -( ( signed int ) token.GetIntValue() );
	}
	else if( token.type != TT_NUMBER || token.subtype == TT_FLOAT )
	{
		budLexer::Error( "expected integer value, found '%s'", token.c_str() );
	}
	return token.GetIntValue();
}

/*
================
budLexer::ParseBool
================
*/
bool budLexer::ParseBool()
{
	budToken token;
	
	if( !budLexer::ExpectTokenType( TT_NUMBER, 0, &token ) )
	{
		budLexer::Error( "couldn't read expected boolean" );
		return false;
	}
	return ( token.GetIntValue() != 0 );
}

/*
================
budLexer::ParseFloat
================
*/
float budLexer::ParseFloat( bool* errorFlag )
{
	budToken token;
	
	if( errorFlag )
	{
		*errorFlag = false;
	}
	
	if( !budLexer::ReadToken( &token ) )
	{
		if( errorFlag )
		{
			budLexer::Warning( "couldn't read expected floating point number" );
			*errorFlag = true;
		}
		else
		{
			budLexer::Error( "couldn't read expected floating point number" );
		}
		return 0;
	}
	if( token.type == TT_PUNCTUATION && token == "-" )
	{
		budLexer::ExpectTokenType( TT_NUMBER, 0, &token );
		return -token.GetFloatValue();
	}
	else if( token.type != TT_NUMBER )
	{
		if( errorFlag )
		{
			budLexer::Warning( "expected float value, found '%s'", token.c_str() );
			*errorFlag = true;
		}
		else
		{
			budLexer::Error( "expected float value, found '%s'", token.c_str() );
		}
	}
	return token.GetFloatValue();
}

/*
================
budLexer::Parse1DMatrix
================
*/
int budLexer::Parse1DMatrix( int x, float* m )
{
	int i;
	
	if( !budLexer::ExpectTokenString( "(" ) )
	{
		return false;
	}
	
	for( i = 0; i < x; i++ )
	{
		m[i] = budLexer::ParseFloat();
	}
	
	if( !budLexer::ExpectTokenString( ")" ) )
	{
		return false;
	}
	return true;
}

// RB begin
int budLexer::Parse1DMatrixJSON( int x, float* m )
{
	int i;
	
	if( !budLexer::ExpectTokenString( "[" ) )
	{
		return false;
	}
	
	for( i = 0; i < x; i++ )
	{
		m[i] = budLexer::ParseFloat();
		
		if( i < ( x - 1 ) && !budLexer::ExpectTokenString( "," ) )
		{
			return false;
		}
	}
	
	if( !budLexer::ExpectTokenString( "]" ) )
	{
		return false;
	}
	return true;
}
// RB end

/*
================
budLexer::Parse2DMatrix
================
*/
int budLexer::Parse2DMatrix( int y, int x, float* m )
{
	int i;
	
	if( !budLexer::ExpectTokenString( "(" ) )
	{
		return false;
	}
	
	for( i = 0; i < y; i++ )
	{
		if( !budLexer::Parse1DMatrix( x, m + i * x ) )
		{
			return false;
		}
	}
	
	if( !budLexer::ExpectTokenString( ")" ) )
	{
		return false;
	}
	return true;
}

/*
================
budLexer::Parse3DMatrix
================
*/
int budLexer::Parse3DMatrix( int z, int y, int x, float* m )
{
	int i;
	
	if( !budLexer::ExpectTokenString( "(" ) )
	{
		return false;
	}
	
	for( i = 0 ; i < z; i++ )
	{
		if( !budLexer::Parse2DMatrix( y, x, m + i * x * y ) )
		{
			return false;
		}
	}
	
	if( !budLexer::ExpectTokenString( ")" ) )
	{
		return false;
	}
	return true;
}

/*
=================
budParser::ParseBracedSection

The next token should be an open brace.
Parses until a matching close brace is found.
Maintains exact characters between braces.

  FIXME: this should use ReadToken and replace the token white space with correct indents and newlines
=================
*/
const char* budLexer::ParseBracedSectionExact( budStr& out, int tabs )
{
	int		depth;
	bool	doTabs;
	bool	skipWhite;
	
	out.Empty();
	
	if( !budLexer::ExpectTokenString( "{" ) )
	{
		return out.c_str( );
	}
	
	out = "{";
	depth = 1;
	skipWhite = false;
	doTabs = tabs >= 0;
	
	while( depth && *budLexer::script_p )
	{
		char c = *( budLexer::script_p++ );
		
		switch( c )
		{
			case '\t':
			case ' ':
			{
				if( skipWhite )
				{
					continue;
				}
				break;
			}
			case '\n':
			{
				if( doTabs )
				{
					skipWhite = true;
					out += c;
					continue;
				}
				break;
			}
			case '{':
			{
				depth++;
				tabs++;
				break;
			}
			case '}':
			{
				depth--;
				tabs--;
				break;
			}
		}
		
		if( skipWhite )
		{
			int i = tabs;
			if( c == '{' )
			{
				i--;
			}
			skipWhite = false;
			for( ; i > 0; i-- )
			{
				out += '\t';
			}
		}
		out += c;
	}
	return out.c_str();
}

/*
=================
budLexer::ParseBracedSection

The next token should be an open brace.
Parses until a matching close brace is found.
Internal brace depths are properly skipped.
=================
*/
const char* budLexer::ParseBracedSection( budStr& out )
{
	budToken token;
	int i, depth;
	
	out.Empty();
	if( !budLexer::ExpectTokenString( "{" ) )
	{
		return out.c_str();
	}
	out = "{";
	depth = 1;
	do
	{
		if( !budLexer::ReadToken( &token ) )
		{
			Error( "missing closing brace" );
			return out.c_str();
		}
		
		// if the token is on a new line
		for( i = 0; i < token.linesCrossed; i++ )
		{
			out += "\r\n";
		}
		
		if( token.type == TT_PUNCTUATION )
		{
			if( token[0] == '{' )
			{
				depth++;
			}
			else if( token[0] == '}' )
			{
				depth--;
			}
		}
		
		if( token.type == TT_STRING )
		{
			out += "\"" + token + "\"";
		}
		else
		{
			out += token;
		}
		out += " ";
	}
	while( depth );
	
	return out.c_str();
}

/*
=================
budLexer::ParseRestOfLine

  parse the rest of the line
=================
*/
const char* budLexer::ParseRestOfLine( budStr& out )
{
	budToken token;
	
	out.Empty();
	while( budLexer::ReadToken( &token ) )
	{
		if( token.linesCrossed )
		{
			budLexer::script_p = lastScript_p;
			budLexer::line = lastline;
			break;
		}
		if( out.Length() )
		{
			out += " ";
		}
		out += token;
	}
	return out.c_str();
}

/*
========================
budLexer::ParseCompleteLine

Returns a string up to the \n, but doesn't eat any whitespace at the beginning of the next line.
========================
*/
const char* budLexer::ParseCompleteLine( budStr& out )
{
	budToken token;
	const char*	start;
	
	start = script_p;
	
	while( 1 )
	{
		// end of buffer
		if( *script_p == 0 )
		{
			break;
		}
		if( *script_p == '\n' )
		{
			line++;
			script_p++;
			break;
		}
		script_p++;
	}
	
	out.Empty();
	out.Append( start, script_p - start );
	
	return out.c_str();
}

/*
================
budLexer::GetLastWhiteSpace
================
*/
int budLexer::GetLastWhiteSpace( budStr& whiteSpace ) const
{
	whiteSpace.Clear();
	for( const char* p = whiteSpaceStart_p; p < whiteSpaceEnd_p; p++ )
	{
		whiteSpace.Append( *p );
	}
	return whiteSpace.Length();
}

/*
================
budLexer::GetLastWhiteSpaceStart
================
*/
int budLexer::GetLastWhiteSpaceStart() const
{
	return whiteSpaceStart_p - buffer;
}

/*
================
budLexer::GetLastWhiteSpaceEnd
================
*/
int budLexer::GetLastWhiteSpaceEnd() const
{
	return whiteSpaceEnd_p - buffer;
}

/*
================
budLexer::Reset
================
*/
void budLexer::Reset()
{
	// pointer in script buffer
	budLexer::script_p = budLexer::buffer;
	// pointer in script buffer before reading token
	budLexer::lastScript_p = budLexer::buffer;
	// begin of white space
	budLexer::whiteSpaceStart_p = NULL;
	// end of white space
	budLexer::whiteSpaceEnd_p = NULL;
	// set if there's a token available in budLexer::token
	budLexer::tokenavailable = 0;
	
	budLexer::line = 1;
	budLexer::lastline = 1;
	// clear the saved token
	budLexer::token = "";
}

/*
================
budLexer::EndOfFile
================
*/
bool budLexer::EndOfFile()
{
	return budLexer::script_p >= budLexer::end_p;
}

/*
================
budLexer::NumLinesCrossed
================
*/
int budLexer::NumLinesCrossed()
{
	return budLexer::line - budLexer::lastline;
}

/*
================
budLexer::LoadFile
================
*/
int budLexer::LoadFile( const char* filename, bool OSPath )
{
	budFile* fp;
	budStr pathname;
	int length;
	char* buf;
	
	if( budLexer::loaded )
	{
		libBud::common->Error( "budLexer::LoadFile: another script already loaded" );
		return false;
	}
	
	if( !OSPath && ( baseFolder[0] != '\0' ) )
	{
		pathname = va( "%s/%s", baseFolder, filename );
	}
	else
	{
		pathname = filename;
	}
	if( OSPath )
	{
		fp = libBud::fileSystem->OpenExplicitFileRead( pathname );
	}
	else
	{
		fp = libBud::fileSystem->OpenFileRead( pathname );
	}
	if( !fp )
	{
		return false;
	}
	length = fp->Length();
	buf = ( char* ) Mem_Alloc( length + 1, TAG_libBud_LEXER );
	buf[length] = '\0';
	fp->Read( buf, length );
	budLexer::fileTime = fp->Timestamp();
	budLexer::filename = fp->GetFullPath();
	libBud::fileSystem->CloseFile( fp );
	
	budLexer::buffer = buf;
	budLexer::length = length;
	// pointer in script buffer
	budLexer::script_p = budLexer::buffer;
	// pointer in script buffer before reading token
	budLexer::lastScript_p = budLexer::buffer;
	// pointer to end of script buffer
	budLexer::end_p = &( budLexer::buffer[length] );
	
	budLexer::tokenavailable = 0;
	budLexer::line = 1;
	budLexer::lastline = 1;
	budLexer::allocated = true;
	budLexer::loaded = true;
	
	return true;
}

/*
================
budLexer::LoadMemory
================
*/
int budLexer::LoadMemory( const char* ptr, int length, const char* name, int startLine )
{
	if( budLexer::loaded )
	{
		libBud::common->Error( "budLexer::LoadMemory: another script already loaded" );
		return false;
	}
	budLexer::filename = name;
	budLexer::buffer = ptr;
	budLexer::fileTime = 0;
	budLexer::length = length;
	// pointer in script buffer
	budLexer::script_p = budLexer::buffer;
	// pointer in script buffer before reading token
	budLexer::lastScript_p = budLexer::buffer;
	// pointer to end of script buffer
	budLexer::end_p = &( budLexer::buffer[length] );
	
	budLexer::tokenavailable = 0;
	budLexer::line = startLine;
	budLexer::lastline = startLine;
	budLexer::allocated = false;
	budLexer::loaded = true;
	
	return true;
}

/*
================
budLexer::FreeSource
================
*/
void budLexer::FreeSource()
{
#ifdef PUNCTABLE
	if( budLexer::punctuationtable && budLexer::punctuationtable != default_punctuationtable )
	{
		Mem_Free( ( void* ) budLexer::punctuationtable );
		budLexer::punctuationtable = NULL;
	}
	if( budLexer::nextpunctuation && budLexer::nextpunctuation != default_nextpunctuation )
	{
		Mem_Free( ( void* ) budLexer::nextpunctuation );
		budLexer::nextpunctuation = NULL;
	}
#endif //PUNCTABLE
	if( budLexer::allocated )
	{
		Mem_Free( ( void* ) budLexer::buffer );
		budLexer::buffer = NULL;
		budLexer::allocated = false;
	}
	budLexer::tokenavailable = 0;
	budLexer::token = "";
	budLexer::loaded = false;
}

/*
================
budLexer::budLexer
================
*/
budLexer::budLexer()
{
	budLexer::loaded = false;
	budLexer::filename = "";
	budLexer::flags = 0;
	budLexer::SetPunctuations( NULL );
	budLexer::allocated = false;
	budLexer::fileTime = 0;
	budLexer::length = 0;
	budLexer::line = 0;
	budLexer::lastline = 0;
	budLexer::tokenavailable = 0;
	budLexer::token = "";
	budLexer::next = NULL;
	budLexer::hadError = false;
}

/*
================
budLexer::budLexer
================
*/
budLexer::budLexer( int flags )
{
	budLexer::loaded = false;
	budLexer::filename = "";
	budLexer::flags = flags;
	budLexer::SetPunctuations( NULL );
	budLexer::allocated = false;
	budLexer::fileTime = 0;
	budLexer::length = 0;
	budLexer::line = 0;
	budLexer::lastline = 0;
	budLexer::tokenavailable = 0;
	budLexer::token = "";
	budLexer::next = NULL;
	budLexer::hadError = false;
}

/*
================
budLexer::budLexer
================
*/
budLexer::budLexer( const char* filename, int flags, bool OSPath )
{
	budLexer::loaded = false;
	budLexer::flags = flags;
	budLexer::SetPunctuations( NULL );
	budLexer::allocated = false;
	budLexer::token = "";
	budLexer::next = NULL;
	budLexer::hadError = false;
	budLexer::LoadFile( filename, OSPath );
}

/*
================
budLexer::budLexer
================
*/
budLexer::budLexer( const char* ptr, int length, const char* name, int flags )
{
	budLexer::loaded = false;
	budLexer::flags = flags;
	budLexer::SetPunctuations( NULL );
	budLexer::allocated = false;
	budLexer::token = "";
	budLexer::next = NULL;
	budLexer::hadError = false;
	budLexer::LoadMemory( ptr, length, name );
}

/*
================
budLexer::~budLexer
================
*/
budLexer::~budLexer()
{
	budLexer::FreeSource();
}

/*
================
budLexer::SetBaseFolder
================
*/
void budLexer::SetBaseFolder( const char* path )
{
	budStr::Copynz( baseFolder, path, sizeof( baseFolder ) );
}

/*
================
budLexer::HadError
================
*/
bool budLexer::HadError() const
{
	return hadError;
}

