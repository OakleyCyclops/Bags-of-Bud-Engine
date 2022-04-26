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

#include "oakUtilsPCH.hpp"
#pragma hdrstop

#ifdef USE_STRING_DATA_ALLOCATOR
static DynamicBlockAlloc < char, 1 << 18, 128, TAG_STRING >	stringDataAllocator;
#endif

const char* units[2][4] =
{
	{ "B", "KB", "MB", "GB" },
	{ "B/s", "KB/s", "MB/s", "GB/s" }
};

/*
============
String::ReAllocate
============
*/
void String::ReAllocate( int amount, bool keepold )
{
	char*	newbuffer;
	int		newsize;
	int		mod;
	
	//assert( data );
	assert( amount > 0 );
	
	mod = amount % STR_ALLOC_GRAN;
	if( !mod )
	{
		newsize = amount;
	}
	else
	{
		newsize = amount + STR_ALLOC_GRAN - mod;
	}
	SetAlloced( newsize );
	
	if( keepold && data )
	{
		data[ len ] = '\0';
		strcpy( newbuffer, data );
	}
	
	if( data && data != baseBuffer )
	{
#ifdef USE_STRING_DATA_ALLOCATOR
		stringDataAllocator.Free( data );
#else
		delete [] data;
#endif
	}
	
	data = newbuffer;
}

/*
============
String::FreeData
============
*/
void String::FreeData()
{
	if( IsStatic() )
	{
		return;
	}
	
	if( data && data != baseBuffer )
	{
#ifdef USE_STRING_DATA_ALLOCATOR
		stringDataAllocator.Free( data );
#else
		delete[] data;
#endif
		data = baseBuffer;
	}
}

/*
============
String::operator=
============
*/
void String::operator=( const char* text )
{
	int l;
	int diff;
	int i;
	
	if( !text )
	{
		// safe behavior if NULL
		EnsureAlloced( 1, false );
		data[ 0 ] = '\0';
		len = 0;
		return;
	}
	
	if( text == data )
	{
		return; // copying same thing
	}
	
	// check if we're aliasing
	if( text >= data && text <= data + len )
	{
		diff = text - data;
		
		assert( strlen( text ) < ( unsigned )len );
		
		for( i = 0; text[ i ]; i++ )
		{
			data[ i ] = text[ i ];
		}
		
		data[ i ] = '\0';
		
		len -= diff;
		
		return;
	}
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = ( int )strlen( text );
	// RB end
	EnsureAlloced( l + 1, false );
	strcpy( data, text );
	len = l;
}

/*
============
String::FindChar

returns -1 if not found otherwise the index of the char
============
*/
int String::FindChar( const char* str, const char c, int start, int end )
{
	int i;
	
	if( end == -1 )
	{
		// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
		end = ( int )strlen( str ) - 1;
		// RB end
	}
	for( i = start; i <= end; i++ )
	{
		if( str[i] == c )
		{
			return i;
		}
	}
	return -1;
}

/*
============
String::FindText

returns -1 if not found otherwise the index of the text
============
*/
int String::FindText( const char* str, const char* text, bool casesensitive, int start, int end )
{
	int l, i, j;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	if( end == -1 )
	{
		end = ( int )strlen( str );
	}
	l = end - ( int )strlen( text );
	// RB end
	
	for( i = start; i <= l; i++ )
	{
		if( casesensitive )
		{
			for( j = 0; text[j]; j++ )
			{
				if( str[i + j] != text[j] )
				{
					break;
				}
			}
		}
		else
		{
			for( j = 0; text[j]; j++ )
			{
				if( ::toupper( str[i + j] ) != ::toupper( text[j] ) )
				{
					break;
				}
			}
		}
		if( !text[j] )
		{
			return i;
		}
	}
	return -1;
}

/*
============
String::Filter

Returns true if the string conforms the given filter.
Several metacharacter may be used in the filter.

*          match any string of zero or more characters
?          match any single character
[abc...]   match any of the enclosed characters; a hyphen can
           be used to specify a range (e.g. a-z, A-Z, 0-9)

============
*/
bool String::Filter( const char* filter, const char* name, bool casesensitive )
{
	String buf;
	int i, found, index;
	
	while( *filter )
	{
		if( *filter == '*' )
		{
			filter++;
			buf.Empty();
			for( i = 0; *filter; i++ )
			{
				if( *filter == '*' || *filter == '?' || ( *filter == '[' && *( filter + 1 ) != '[' ) )
				{
					break;
				}
				buf += *filter;
				if( *filter == '[' )
				{
					filter++;
				}
				filter++;
			}
			if( buf.Length() )
			{
				index = String( name ).Find( buf.c_str(), casesensitive );
				if( index == -1 )
				{
					return false;
				}
				name += index + strlen( buf );
			}
		}
		else if( *filter == '?' )
		{
			filter++;
			name++;
		}
		else if( *filter == '[' )
		{
			if( *( filter + 1 ) == '[' )
			{
				if( *name != '[' )
				{
					return false;
				}
				filter += 2;
				name++;
			}
			else
			{
				filter++;
				found = false;
				while( *filter && !found )
				{
					if( *filter == ']' && *( filter + 1 ) != ']' )
					{
						break;
					}
					if( *( filter + 1 ) == '-' && *( filter + 2 ) && ( *( filter + 2 ) != ']' || *( filter + 3 ) == ']' ) )
					{
						if( casesensitive )
						{
							if( *name >= *filter && *name <= *( filter + 2 ) )
							{
								found = true;
							}
						}
						else
						{
							if( ::toupper( *name ) >= ::toupper( *filter ) && ::toupper( *name ) <= ::toupper( *( filter + 2 ) ) )
							{
								found = true;
							}
						}
						filter += 3;
					}
					else
					{
						if( casesensitive )
						{
							if( *filter == *name )
							{
								found = true;
							}
						}
						else
						{
							if( ::toupper( *filter ) == ::toupper( *name ) )
							{
								found = true;
							}
						}
						filter++;
					}
				}
				if( !found )
				{
					return false;
				}
				while( *filter )
				{
					if( *filter == ']' && *( filter + 1 ) != ']' )
					{
						break;
					}
					filter++;
				}
				filter++;
				name++;
			}
		}
		else
		{
			if( casesensitive )
			{
				if( *filter != *name )
				{
					return false;
				}
			}
			else
			{
				if( ::toupper( *filter ) != ::toupper( *name ) )
				{
					return false;
				}
			}
			filter++;
			name++;
		}
	}
	return true;
}

/*
=============
String::StripMediaName

  makes the string lower case, replaces backslashes with forward slashes, and removes extension
=============
*/
void String::StripMediaName( const char* name, String& mediaName )
{
	char c;
	
	mediaName.Empty();
	
	for( c = *name; c; c = *( ++name ) )
	{
		// truncate at an extension
		if( c == '.' )
		{
			break;
		}
		// convert backslashes to forward slashes
		if( c == '\\' )
		{
			mediaName.Append( '/' );
		}
		else
		{
			mediaName.Append( String::ToLower( c ) );
		}
	}
}

/*
=============
String::CheckExtension
=============
*/
bool String::CheckExtension( const char* name, const char* ext )
{
	const char* s1 = name + Length( name ) - 1;
	const char* s2 = ext + Length( ext ) - 1;
	int c1, c2, d;
	
	do
	{
		c1 = *s1--;
		c2 = *s2--;
		
		d = c1 - c2;
		while( d )
		{
			if( c1 <= 'Z' && c1 >= 'A' )
			{
				d += ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			if( c2 <= 'Z' && c2 >= 'A' )
			{
				d -= ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			return false;
		}
	}
	while( s1 > name && s2 > ext );
	
	return ( s1 >= name );
}

/*
=============
String::FloatArrayToString
=============
*/
const char* String::FloatArrayToString( const float* array, const int length, const int precision )
{
	static int index = 0;
	static char str[4][16384];	// in case called by nested functions
	int i, n;
	char format[16], *s;
	
	// use an array of string so that multiple calls won't collide
	s = str[ index ];
	index = ( index + 1 ) & 3;
	
	String::snPrintf( format, sizeof( format ), "%%.%df", precision );
	n = String::snPrintf( s, sizeof( str[0] ), format, array[0] );
	if( precision > 0 )
	{
		while( n > 0 && s[n - 1] == '0' ) s[--n] = '\0';
		while( n > 0 && s[n - 1] == '.' ) s[--n] = '\0';
	}
	String::snPrintf( format, sizeof( format ), " %%.%df", precision );
	for( i = 1; i < length; i++ )
	{
		n += String::snPrintf( s + n, sizeof( str[0] ) - n, format, array[i] );
		if( precision > 0 )
		{
			while( n > 0 && s[n - 1] == '0' ) s[--n] = '\0';
			while( n > 0 && s[n - 1] == '.' ) s[--n] = '\0';
		}
	}
	return s;
}

/*
========================
String::CStyleQuote
========================
*/
const char* String::CStyleQuote( const char* str )
{
	static int index = 0;
	static char buffers[4][16384];	// in case called by nested functions
	unsigned int i;
	char* buf;
	
	buf = buffers[index];
	index = ( index + 1 ) & 3;
	
	buf[0] = '\"';
	for( i = 1; i < sizeof( buffers[0] ) - 2; i++ )
	{
		int c = *str++;
		switch( c )
		{
			case '\0':
				buf[i++] = '\"';
				buf[i] = '\0';
				return buf;
			case '\\':
				buf[i++] = '\\';
				buf[i] = '\\';
				break;
			case '\n':
				buf[i++] = '\\';
				buf[i] = 'n';
				break;
			case '\r':
				buf[i++] = '\\';
				buf[i] = 'r';
				break;
			case '\t':
				buf[i++] = '\\';
				buf[i] = 't';
				break;
			case '\v':
				buf[i++] = '\\';
				buf[i] = 'v';
				break;
			case '\b':
				buf[i++] = '\\';
				buf[i] = 'b';
				break;
			case '\f':
				buf[i++] = '\\';
				buf[i] = 'f';
				break;
			case '\a':
				buf[i++] = '\\';
				buf[i] = 'a';
				break;
			case '\'':
				buf[i++] = '\\';
				buf[i] = '\'';
				break;
			case '\"':
				buf[i++] = '\\';
				buf[i] = '\"';
				break;
			case '\?':
				buf[i++] = '\\';
				buf[i] = '\?';
				break;
			default:
				buf[i] = c;
				break;
		}
	}
	buf[i++] = '\"';
	buf[i] = '\0';
	return buf;
}

/*
========================
String::CStyleUnQuote
========================
*/
const char* String::CStyleUnQuote( const char* str )
{
	if( str[0] != '\"' )
	{
		return str;
	}
	
	static int index = 0;
	static char buffers[4][16384];	// in case called by nested functions
	unsigned int i;
	char* buf;
	
	buf = buffers[index];
	index = ( index + 1 ) & 3;
	
	str++;
	for( i = 0; i < sizeof( buffers[0] ) - 1; i++ )
	{
		int c = *str++;
		if( c == '\0' )
		{
			break;
		}
		else if( c == '\\' )
		{
			c = *str++;
			switch( c )
			{
				case '\\':
					buf[i] = '\\';
					break;
				case 'n':
					buf[i] = '\n';
					break;
				case 'r':
					buf[i] = '\r';
					break;
				case 't':
					buf[i] = '\t';
					break;
				case 'v':
					buf[i] = '\v';
					break;
				case 'b':
					buf[i] = '\b';
					break;
				case 'f':
					buf[i] = '\f';
					break;
				case 'a':
					buf[i] = '\a';
					break;
				case '\'':
					buf[i] = '\'';
					break;
				case '\"':
					buf[i] = '\"';
					break;
				case '\?':
					buf[i] = '\?';
					break;
			}
		}
		else
		{
			buf[i] = c;
		}
	}
	assert( buf[i - 1] == '\"' );
	buf[i - 1] = '\0';
	return buf;
}

/*
============
String::Last

returns -1 if not found otherwise the index of the char
============
*/
int String::Last( const char c ) const
{
	int i;
	
	for( i = Length(); i > 0; i-- )
	{
		if( data[ i - 1 ] == c )
		{
			return i - 1;
		}
	}
	
	return -1;
}

/*
========================
String::Format

perform a threadsafe sprintf to the string
========================
*/
void String::Format( const char* fmt, ... )
{
	va_list argptr;
	char text[MAX_PRINT_MSG];
	
	va_start( argptr, fmt );
	int len = String::vsnPrintf( text, sizeof( text ) - 1, fmt, argptr );
	va_end( argptr );
	text[ sizeof( text ) - 1 ] = '\0';
	
	// if( ( size_t )len >= sizeof( text ) - 1 )
	// {
	// 	libBud::common->FatalError( "Tried to set a large buffer using %s", fmt );
	// }
	*this = text;
}

/*
========================
String::FormatInt

Formats integers with commas for readability.
========================
*/
String String::FormatInt( const int num, bool isCash )
{
	String val = va( "%d", num );
	int len = val.Length();
	for( int i = 0 ; i < ( ( len - 1 ) / 3 ); i++ )
	{
		int pos = val.Length() - ( ( i + 1 ) * 3 + i );
		if( pos > 1 || val[0] != '-' )
		{
			val.Insert( ',', pos );
		}
	}
	
	if( isCash )
	{
		val.Insert( '$', val[0] == '-' ? 1 : 0 );
	}
	
	return val;
}

/*
============
String::StripLeading
============
*/
void String::StripLeading( const char c )
{
	while( data[ 0 ] == c )
	{
		memmove( &data[ 0 ], &data[ 1 ], len );
		len--;
	}
}

/*
============
String::StripLeading
============
*/
void String::StripLeading( const char* string )
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = ( int )strlen( string );
	// RB end
	if( l > 0 )
	{
		while( !Cmpn( string, l ) )
		{
			memmove( data, data + l, len - l + 1 );
			len -= l;
		}
	}
}

/*
============
String::StripLeadingOnce
============
*/
bool String::StripLeadingOnce( const char* string )
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = ( int )strlen( string );
	// RB end
	if( ( l > 0 ) && !Cmpn( string, l ) )
	{
		memmove( data, data + l, len - l + 1 );
		len -= l;
		return true;
	}
	return false;
}

/*
============
String::StripTrailing
============
*/
void String::StripTrailing( const char c )
{
	int i;
	
	for( i = Length(); i > 0 && data[ i - 1 ] == c; i-- )
	{
		data[ i - 1 ] = '\0';
		len--;
	}
}

/*
============
String::StripLeading
============
*/
void String::StripTrailing( const char* string )
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = ( int )strlen( string );
	// RB end
	if( l > 0 )
	{
		while( ( len >= l ) && !Cmpn( string, data + len - l, l ) )
		{
			len -= l;
			data[len] = '\0';
		}
	}
}

/*
============
String::StripTrailingOnce
============
*/
bool String::StripTrailingOnce( const char* string )
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = ( int )strlen( string );
	// RB end
	if( ( l > 0 ) && ( len >= l ) && !Cmpn( string, data + len - l, l ) )
	{
		len -= l;
		data[len] = '\0';
		return true;
	}
	return false;
}

/*
============
String::Replace
============
*/
bool  String::ReplaceChar( const char old, const char nw )
{
	bool replaced = false;
	for( int i = 0; i < Length(); i++ )
	{
		if( data[i] == old )
		{
			data[i] = nw;
			replaced = true;
		}
	}
	return replaced;
}

/*
============
String::Replace
============
*/
bool String::Replace( const char* old, const char* nw )
{
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	int oldLen = ( int )strlen( old );
	int newLen = ( int )strlen( nw );
	// RB end
	
	// Work out how big the new string will be
	int count = 0;
	for( int i = 0; i < Length(); i++ )
	{
		if( String::Cmpn( &data[i], old, oldLen ) == 0 )
		{
			count++;
			i += oldLen - 1;
		}
	}
	
	if( count )
	{
		String oldString( data );
		
		EnsureAlloced( len + ( ( newLen - oldLen ) * count ) + 2, false );
		
		// Replace the old data with the new data
		int j = 0;
		for( int i = 0; i < oldString.Length(); i++ )
		{
			if( String::Cmpn( &oldString[i], old, oldLen ) == 0 )
			{
				memcpy( data + j, nw, newLen );
				i += oldLen - 1;
				j += newLen;
			}
			else
			{
				data[j] = oldString[i];
				j++;
			}
		}
		data[j] = 0;
		// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
		len = ( int )strlen( data );
		// RB end
		return true;
	}
	return false;
}

/*
============
String::Mid
============
*/
const char* String::Mid( int start, int len, String& result ) const
{
	int i;
	
	result.Empty();
	
	i = Length();
	if( i == 0 || len <= 0 || start >= i )
	{
		return NULL;
	}
	
	if( start + len >= i )
	{
		len = i - start;
	}
	
	result.Append( &data[ start ], len );
	return result;
}

/*
============
String::Mid
============
*/
String String::Mid( int start, int len ) const
{
	int i;
	String result;
	
	i = Length();
	if( i == 0 || len <= 0 || start >= i )
	{
		return result;
	}
	
	if( start + len >= i )
	{
		len = i - start;
	}
	
	result.Append( &data[ start ], len );
	return result;
}

/*
============
String::StripTrailingWhitespace
============
*/
void String::StripTrailingWhitespace()
{
	int i;
	
	// cast to unsigned char to prevent stripping off high-ASCII characters
	for( i = Length(); i > 0 && ( unsigned char )( data[ i - 1 ] ) <= ' '; i-- )
	{
		data[ i - 1 ] = '\0';
		len--;
	}
}

/*
============
String::StripQuotes

Removes the quotes from the beginning and end of the string
============
*/
String& String::StripQuotes()
{
	if( data[0] != '\"' )
	{
		return *this;
	}
	
	// Remove the trailing quote first
	if( data[len - 1] == '\"' )
	{
		data[len - 1] = '\0';
		len--;
	}
	
	// Strip the leading quote now
	len--;
	memmove( &data[ 0 ], &data[ 1 ], len );
	data[len] = '\0';
	
	return *this;
}

/*
=====================================================================

  filename methods

=====================================================================
*/

/*
============
String::FileNameHash
============
*/
int String::FileNameHash() const
{
	int		i;
	int		hash; // DG: use int instead of long for 64bit compatibility
	char	letter;
	
	hash = 0;
	i = 0;
	while( data[i] != '\0' )
	{
		letter = String::ToLower( data[i] );
		if( letter == '.' )
		{
			break;				// don't include extension
		}
		if( letter == '\\' )
		{
			letter = '/';
		}
		hash += ( long )( letter ) * ( i + 119 ); // DG: use int instead of long for 64bit compatibility
		i++;
	}
	hash &= ( FILE_HASH_SIZE - 1 );
	return hash;
}

/*
============
String::BackSlashesToSlashes
============
*/
String& String::BackSlashesToSlashes()
{
	int i;
	
	for( i = 0; i < len; i++ )
	{
		if( data[ i ] == '\\' )
		{
			data[ i ] = '/';
		}
	}
	return *this;
}

/*
============
String::SlashesToBackSlashes
============
*/
String& String::SlashesToBackSlashes()
{
	int i;
	
	for( i = 0; i < len; i++ )
	{
		if( data[ i ] == '/' )
		{
			data[ i ] = '\\';
		}
	}
	return *this;
}

/*
============
String::SetFileExtension
============
*/
String& String::SetFileExtension( const char* extension )
{
	StripFileExtension();
	if( *extension != '.' )
	{
		Append( '.' );
	}
	Append( extension );
	return *this;
}

/*
============
String::StripFileExtension
============
*/
String& String::StripFileExtension()
{
	int i;
	
	for( i = len - 1; i >= 0; i-- )
	{
		if( data[i] == '.' )
		{
			data[i] = '\0';
			len = i;
			break;
		}
	}
	return *this;
}

/*
============
String::StripAbsoluteFileExtension
============
*/
String& String::StripAbsoluteFileExtension()
{
	int i;
	
	for( i = 0; i < len; i++ )
	{
		if( data[i] == '.' )
		{
			data[i] = '\0';
			len = i;
			break;
		}
	}
	
	return *this;
}

/*
==================
String::DefaultFileExtension
==================
*/
String& String::DefaultFileExtension( const char* extension )
{
	int i;
	
	// do nothing if the string already has an extension
	for( i = len - 1; i >= 0; i-- )
	{
		if( data[i] == '.' )
		{
			return *this;
		}
	}
	if( *extension != '.' )
	{
		Append( '.' );
	}
	Append( extension );
	return *this;
}

/*
==================
String::DefaultPath
==================
*/
String& String::DefaultPath( const char* basepath )
{
	if( ( ( *this )[ 0 ] == '/' ) || ( ( *this )[ 0 ] == '\\' ) )
	{
		// absolute path location
		return *this;
	}
	
	*this = basepath + *this;
	return *this;
}

/*
====================
String::AppendPath
====================
*/
void String::AppendPath( const char* text )
{
	int pos;
	int i = 0;
	
	if( text && text[i] )
	{
		pos = len;
		// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
		EnsureAlloced( len + ( int )strlen( text ) + 2 );
		// RB end
		
		if( pos )
		{
			if( data[ pos - 1 ] != '/' )
			{
				data[ pos++ ] = '/';
			}
		}
		if( text[i] == '/' )
		{
			i++;
		}
		
		for( ; text[ i ]; i++ )
		{
			if( text[ i ] == '\\' )
			{
				data[ pos++ ] = '/';
			}
			else
			{
				data[ pos++ ] = text[ i ];
			}
		}
		len = pos;
		data[ pos ] = '\0';
	}
}

/*
==================
String::StripFilename
==================
*/
String& String::StripFilename()
{
	int pos;
	
	pos = Length() - 1;
	while( ( pos > 0 ) && ( ( *this )[ pos ] != '/' ) && ( ( *this )[ pos ] != '\\' ) )
	{
		pos--;
	}
	
	if( pos < 0 )
	{
		pos = 0;
	}
	
	CapLength( pos );
	return *this;
}

/*
==================
String::StripPath
==================
*/
String& String::StripPath()
{
	int pos;
	
	pos = Length();
	while( ( pos > 0 ) && ( ( *this )[ pos - 1 ] != '/' ) && ( ( *this )[ pos - 1 ] != '\\' ) )
	{
		pos--;
	}
	
	*this = Right( Length() - pos );
	return *this;
}

/*
====================
String::ExtractFilePath
====================
*/
void String::ExtractFilePath( String& dest ) const
{
	int pos;
	
	pos = Length();
	while( ( pos > 0 ) && ( ( *this )[ pos - 1 ] != '/' ) && ( ( *this )[ pos - 1 ] != '\\' ) )
	{
		pos--;
	}
	
	Left( pos, dest );
}

/*
====================
String::ExtractFileName
====================
*/
void String::ExtractFileName( String& dest ) const
{
	int pos;

	pos = Length() - 1;
	while( ( pos > 0 ) && ( ( *this )[ pos - 1 ] != '/' ) && ( ( *this )[ pos - 1 ] != '\\' ) )
	{
		pos--;
	}
	
	Right( Length() - pos, dest );
}

/*
====================
String::ExtractFileBase
====================
*/
void String::ExtractFileBase( String& dest ) const
{
	int pos;
	int start;
	
	//
	// back up until a \ or the start
	//
	pos = Length() - 1;
	while( ( pos > 0 ) && ( ( *this )[ pos - 1 ] != '/' ) && ( ( *this )[ pos - 1 ] != '\\' ) )
	{
		pos--;
	}
	
	start = pos;
	while( ( pos < Length() ) && ( ( *this )[ pos ] != '.' ) )
	{
		pos++;
	}
	
	Mid( start, pos - start, dest );
}

/*
====================
String::ExtractFileExtension
====================
*/
void String::ExtractFileExtension( String& dest ) const
{
	int pos;
	
	//
	// back up until a . or the start
	//
	pos = Length() - 1;
	while( ( pos > 0 ) && ( ( *this )[ pos - 1 ] != '.' ) )
	{
		pos--;
	}
	
	if( !pos )
	{
		// no extension
		dest.Empty();
	}
	else
	{
		Right( Length() - pos, dest );
	}
}


/*
=====================================================================

  char * methods to replace library functions

=====================================================================
*/

/*
============
String::IsNumeric

Checks a string to see if it contains only numerical values.
============
*/
bool String::IsNumeric( const char* s )
{
	int		i;
	bool	dot;
	
	if( *s == '-' )
	{
		s++;
	}
	
	dot = false;
	for( i = 0; s[i]; i++ )
	{
		if( !isdigit( ( const unsigned char )s[i] ) )
		{
			if( ( s[ i ] == '.' ) && !dot )
			{
				dot = true;
				continue;
			}
			return false;
		}
	}
	
	return true;
}

/*
============
String::HasLower

Checks if a string has any lowercase chars
============
*/
bool String::HasLower( const char* s )
{
	if( !s )
	{
		return false;
	}
	
	while( *s )
	{
		if( CharIsLower( *s ) )
		{
			return true;
		}
		s++;
	}
	
	return false;
}

/*
============
String::HasUpper

Checks if a string has any uppercase chars
============
*/
bool String::HasUpper( const char* s )
{
	if( !s )
	{
		return false;
	}
	
	while( *s )
	{
		if( CharIsUpper( *s ) )
		{
			return true;
		}
		s++;
	}
	
	return false;
}

/*
================
String::Cmp
================
*/
int String::Cmp( const char* s1, const char* s2 )
{
	int c1, c2, d;
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		d = c1 - c2;
		if( d )
		{
			return ( INT32_SIGNBITNOTSET( d ) << 1 ) - 1;
		}
	}
	while( c1 );
	
	return 0;		// strings are equal
}

/*
================
String::Cmpn
================
*/
int String::Cmpn( const char* s1, const char* s2, int n )
{
	int c1, c2, d;
	
	assert( n >= 0 );
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		if( !n-- )
		{
			return 0;		// strings are equal until end point
		}
		
		d = c1 - c2;
		if( d )
		{
			return ( INT32_SIGNBITNOTSET( d ) << 1 ) - 1;
		}
	}
	while( c1 );
	
	return 0;		// strings are equal
}

/*
================
String::Icmp
================
*/
int String::Icmp( const char* s1, const char* s2 )
{
	int c1, c2, d;
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		d = c1 - c2;
		while( d )
		{
			if( c1 <= 'Z' && c1 >= 'A' )
			{
				d += ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			if( c2 <= 'Z' && c2 >= 'A' )
			{
				d -= ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			return ( INT32_SIGNBITNOTSET( d ) << 1 ) - 1;
		}
	}
	while( c1 );
	
	return 0;		// strings are equal
}

/*
================
String::Icmpn
================
*/
int String::Icmpn( const char* s1, const char* s2, int n )
{
	int c1, c2, d;
	
	assert( n >= 0 );
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		if( !n-- )
		{
			return 0;		// strings are equal until end point
		}
		
		d = c1 - c2;
		while( d )
		{
			if( c1 <= 'Z' && c1 >= 'A' )
			{
				d += ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			if( c2 <= 'Z' && c2 >= 'A' )
			{
				d -= ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			return ( INT32_SIGNBITNOTSET( d ) << 1 ) - 1;
		}
	}
	while( c1 );
	
	return 0;		// strings are equal
}

/*
================
String::Icmp
================
*/
int String::IcmpNoColor( const char* s1, const char* s2 )
{
	int c1, c2, d;
	
	do
	{
		while( String::IsColor( s1 ) )
		{
			s1 += 2;
		}
		while( String::IsColor( s2 ) )
		{
			s2 += 2;
		}
		c1 = *s1++;
		c2 = *s2++;
		
		d = c1 - c2;
		while( d )
		{
			if( c1 <= 'Z' && c1 >= 'A' )
			{
				d += ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			if( c2 <= 'Z' && c2 >= 'A' )
			{
				d -= ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			return ( INT32_SIGNBITNOTSET( d ) << 1 ) - 1;
		}
	}
	while( c1 );
	
	return 0;		// strings are equal
}

/*
================
String::IcmpPath
================
*/
int String::IcmpPath( const char* s1, const char* s2 )
{
	int c1, c2, d;
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		d = c1 - c2;
		while( d )
		{
			if( c1 <= 'Z' && c1 >= 'A' )
			{
				d += ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			if( c1 == '\\' )
			{
				d += ( '/' - '\\' );
				if( !d )
				{
					break;
				}
			}
			if( c2 <= 'Z' && c2 >= 'A' )
			{
				d -= ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			if( c2 == '\\' )
			{
				d -= ( '/' - '\\' );
				if( !d )
				{
					break;
				}
			}
			// make sure folders come first
			while( c1 )
			{
				if( c1 == '/' || c1 == '\\' )
				{
					break;
				}
				c1 = *s1++;
			}
			while( c2 )
			{
				if( c2 == '/' || c2 == '\\' )
				{
					break;
				}
				c2 = *s2++;
			}
			if( c1 && !c2 )
			{
				return -1;
			}
			else if( !c1 && c2 )
			{
				return 1;
			}
			// same folder depth so use the regular compare
			return ( INT32_SIGNBITNOTSET( d ) << 1 ) - 1;
		}
	}
	while( c1 );
	
	return 0;
}

/*
================
String::IcmpnPath
================
*/
int String::IcmpnPath( const char* s1, const char* s2, int n )
{
	int c1, c2, d;
	
	assert( n >= 0 );
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		if( !n-- )
		{
			return 0;		// strings are equal until end point
		}
		
		d = c1 - c2;
		while( d )
		{
			if( c1 <= 'Z' && c1 >= 'A' )
			{
				d += ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			if( c1 == '\\' )
			{
				d += ( '/' - '\\' );
				if( !d )
				{
					break;
				}
			}
			if( c2 <= 'Z' && c2 >= 'A' )
			{
				d -= ( 'a' - 'A' );
				if( !d )
				{
					break;
				}
			}
			if( c2 == '\\' )
			{
				d -= ( '/' - '\\' );
				if( !d )
				{
					break;
				}
			}
			// make sure folders come first
			while( c1 )
			{
				if( c1 == '/' || c1 == '\\' )
				{
					break;
				}
				c1 = *s1++;
			}
			while( c2 )
			{
				if( c2 == '/' || c2 == '\\' )
				{
					break;
				}
				c2 = *s2++;
			}
			if( c1 && !c2 )
			{
				return -1;
			}
			else if( !c1 && c2 )
			{
				return 1;
			}
			// same folder depth so use the regular compare
			return ( INT32_SIGNBITNOTSET( d ) << 1 ) - 1;
		}
	}
	while( c1 );
	
	return 0;
}

/*
=============
String::Copynz

Safe strncpy that ensures a trailing zero
=============
*/
void String::Copynz( char* dest, const char* src, int destsize )
{
	strncpy( dest, src, destsize - 1 );
	dest[destsize - 1] = 0;
}

/*
================
String::Append

  never goes past bounds or leaves without a terminating 0
================
*/
void String::Append( char* dest, int size, const char* src )
{
	int		l1;
	
	l1 = ( int )strlen( dest );
	String::Copynz( dest + l1, src, size - l1 );
}

/*
========================
String::IsValidUTF8
========================
*/
bool String::IsValidUTF8( const uint8* s, const int maxLen, utf8Encoding_t& encoding )
{
	struct local_t
	{
		static int GetNumEncodedUTF8Bytes( const uint8 c )
		{
			if( c < 0x80 )
			{
				return 1;
			}
			else if( ( c >> 5 ) == 0x06 )
			{
				// 2 byte encoding - the next byte must begin with
				return 2;
			}
			else if( ( c >> 4 ) == 0x0E )
			{
				// 3 byte encoding
				return 3;
			}
			else if( ( c >> 5 ) == 0x1E )
			{
				// 4 byte encoding
				return 4;
			}
			// this isnt' a valid UTF-8 precursor character
			return 0;
		}
		static bool RemainingCharsAreUTF8FollowingBytes( const uint8* s, const int curChar, const int maxLen, const int num )
		{
			if( maxLen - curChar < num )
			{
				return false;
			}
			for( int i = curChar + 1; i <= curChar + num; i++ )
			{
				if( s[ i ] == '\0' )
				{
					return false;
				}
				if( ( s[ i ] >> 6 ) != 0x02 )
				{
					return false;
				}
			}
			return true;
		}
	};
	
	// check for byte-order-marker
	encoding = UTF8_PURE_ASCII;
	utf8Encoding_t utf8Type = UTF8_ENCODED_NO_BOM;
	if( maxLen > 3 && s[ 0 ] == 0xEF && s[ 1 ] == 0xBB && s[ 2 ] == 0xBF )
	{
		utf8Type = UTF8_ENCODED_BOM;
	}
	
	for( int i = 0; s[ i ] != '\0' && i < maxLen; i++ )
	{
		int numBytes = local_t::GetNumEncodedUTF8Bytes( s[ i ] );
		if( numBytes == 1 )
		{
			continue;	// just low ASCII
		}
		else if( numBytes == 2 )
		{
			// 2 byte encoding - the next byte must begin with bit pattern 10
			if( !local_t::RemainingCharsAreUTF8FollowingBytes( s, i, maxLen, 1 ) )
			{
				return false;
			}
			// skip over UTF-8 character
			i += 1;
			encoding = utf8Type;
		}
		else if( numBytes == 3 )
		{
			// 3 byte encoding - the next 2 bytes must begin with bit pattern 10
			if( !local_t::RemainingCharsAreUTF8FollowingBytes( s, i, maxLen, 2 ) )
			{
				return false;
			}
			// skip over UTF-8 character
			i += 2;
			encoding = utf8Type;
		}
		else if( numBytes == 4 )
		{
			// 4 byte encoding - the next 3 bytes must begin with bit pattern 10
			if( !local_t::RemainingCharsAreUTF8FollowingBytes( s, i, maxLen, 3 ) )
			{
				return false;
			}
			// skip over UTF-8 character
			i += 3;
			encoding = utf8Type;
		}
		else
		{
			// this isnt' a valid UTF-8 character
			if( utf8Type == UTF8_ENCODED_BOM )
			{
				encoding = UTF8_INVALID_BOM;
			}
			else
			{
				encoding = UTF8_INVALID;
			}
			return false;
		}
	}
	return true;
}

/*
========================
String::UTF8Length
========================
*/
int String::UTF8Length( const byte* s )
{
	int mbLen = 0;
	int charLen = 0;
	while( s[ mbLen ] != '\0' )
	{
		uint32 cindex;
		cindex = s[ mbLen ];
		if( cindex < 0x80 )
		{
			mbLen++;
		}
		else
		{
			int trailing = 0;
			if( cindex >= 0xc0 )
			{
				static const byte trailingBytes[ 64 ] =
				{
					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
					2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
				};
				trailing = trailingBytes[ cindex - 0xc0 ];
			}
			mbLen += trailing + 1;
		}
		charLen++;
	}
	return charLen;
}


/*
========================
String::AppendUTF8Char
========================
*/
void String::AppendUTF8Char( uint32 c )
{
	if( c < 0x80 )
	{
		Append( ( char )c );
	}
	else if( c < 0x800 )      // 11 bits
	{
		Append( ( char )( 0xC0 | ( c >> 6 ) ) );
		Append( ( char )( 0x80 | ( c & 0x3F ) ) );
	}
	else if( c < 0x10000 )      // 16 bits
	{
		Append( ( char )( 0xE0 | ( c >> 12 ) ) );
		Append( ( char )( 0x80 | ( ( c >> 6 ) & 0x3F ) ) );
		Append( ( char )( 0x80 | ( c & 0x3F ) ) );
	}
	else if( c < 0x200000 )  	// 21 bits
	{
		Append( ( char )( 0xF0 | ( c >> 18 ) ) );
		Append( ( char )( 0x80 | ( ( c >> 12 ) & 0x3F ) ) );
		Append( ( char )( 0x80 | ( ( c >> 6 ) & 0x3F ) ) );
		Append( ( char )( 0x80 | ( c & 0x3F ) ) );
	}
	else
	{
		// UTF-8 can encode up to 6 bytes. Why don't we support that?
		// This is an invalid Unicode character
		Append( '?' );
	}
}

/*
========================
String::UTF8Char
========================
*/
uint32 String::UTF8Char( const byte* s, int& idx )
{
	if( idx >= 0 )
	{
		while( s[ idx ] != '\0' )
		{
			uint32 cindex = s[ idx ];
			if( cindex < 0x80 )
			{
				idx++;
				return cindex;
			}
			int trailing = 0;
			if( cindex >= 0xc0 )
			{
				static const byte trailingBytes[ 64 ] =
				{
					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
					2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
				};
				trailing = trailingBytes[ cindex - 0xc0 ];
			}
			static const uint32 trailingMask[ 6 ] = { 0x0000007f, 0x0000001f, 0x0000000f, 0x00000007, 0x00000003, 0x00000001 };
			cindex &= trailingMask[ trailing  ];
			while( trailing-- > 0 )
			{
				cindex <<= 6;
				cindex += s[ ++idx ] & 0x0000003f;
			}
			idx++;
			return cindex;
		}
	}
	idx++;
	return 0;	// return a null terminator if out of range
}

/*
================
String::LengthWithoutColors
================
*/
int String::LengthWithoutColors( const char* s )
{
	int len;
	const char* p;
	
	if( !s )
	{
		return 0;
	}
	
	len = 0;
	p = s;
	while( *p )
	{
		if( String::IsColor( p ) )
		{
			p += 2;
			continue;
		}
		p++;
		len++;
	}
	
	return len;
}

/*
================
String::RemoveColors
================
*/
char* String::RemoveColors( char* string )
{
	char* d;
	char* s;
	int c;
	
	s = string;
	d = string;
	while( ( c = *s ) != 0 )
	{
		if( String::IsColor( s ) )
		{
			s++;
		}
		else
		{
			*d++ = c;
		}
		s++;
	}
	*d = '\0';
	
	return string;
}

/*
================
String::snPrintf
================
*/
int String::snPrintf( char* dest, int size, const char* fmt, ... )
{
	int len;
	va_list argptr;
	char buffer[32000];	// big, but small enough to fit in PPC stack
	
	va_start( argptr, fmt );
	len = vsprintf( buffer, fmt, argptr );
	va_end( argptr );

	String::Copynz( dest, buffer, size );
	return len;
}

/*
============
String::vsnPrintf

vsnprintf portability:

C99 standard: vsnprintf returns the number of characters (excluding the trailing
'\0') which would have been written to the final string if enough space had been available
snprintf and vsnprintf do not write more than size bytes (including the trailing '\0')

win32: _vsnprintf returns the number of characters written, not including the terminating null character,
or a negative value if an output error occurs. If the number of characters to write exceeds count, then count
characters are written and -1 is returned and no trailing '\0' is added.

String::vsnPrintf: always appends a trailing '\0', returns number of characters written (not including terminal \0)
or returns -1 on failure or if the buffer would be overflowed.
============
*/
int String::vsnPrintf( char* dest, int size, const char* fmt, va_list argptr )
{
	int ret;
	
// RB begin
#ifdef _WIN32
#undef _vsnprintf
	ret = _vsnprintf( dest, size - 1, fmt, argptr );
#define _vsnprintf	use_String_vsnPrintf
#else
#undef vsnprintf
	ret = vsnprintf( dest, size, fmt, argptr );
#define vsnprintf	use_String_vsnPrintf
#endif
// RB end

	dest[size - 1] = '\0';
	if( ret < 0 || ret >= size )
	{
		return -1;
	}
	return ret;
}

/*
============
sprintf

Sets the value of the string using a printf interface.
============
*/
int sprintf( String& string, const char* fmt, ... )
{
	int l;
	va_list argptr;
	char buffer[32000];
	
	va_start( argptr, fmt );
	l = String::vsnPrintf( buffer, sizeof( buffer ) - 1, fmt, argptr );
	va_end( argptr );
	buffer[sizeof( buffer ) - 1] = '\0';
	
	string = buffer;
	return l;
}

/*
============
vsprintf

Sets the value of the string using a vprintf interface.
============
*/
int vsprintf( String& string, const char* fmt, va_list argptr )
{
	int l;
	char buffer[32000];
	
	l = String::vsnPrintf( buffer, sizeof( buffer ) - 1, fmt, argptr );
	buffer[sizeof( buffer ) - 1] = '\0';
	
	string = buffer;
	return l;
}

/*
============
va

does a varargs printf into a temp buffer
NOTE: not thread safe
============
*/
char* va( const char* fmt, ... )
{
	va_list argptr;
	static int index = 0;
	static char string[4][16384];	// in case called by nested functions
	char* buf;
	
	buf = string[index];
	index = ( index + 1 ) & 3;
	
	va_start( argptr, fmt );
	vsprintf( buf, fmt, argptr );
	va_end( argptr );
	
	return buf;
}



/*
============
String::BestUnit
============
*/
int String::BestUnit( const char* format, float value, Measure_t measure )
{
	int unit = 1;
	while( unit <= 3 && ( 1 << ( unit * 10 ) < value ) )
	{
		unit++;
	}
	unit--;
	value /= 1 << ( unit * 10 );
	sprintf( *this, format, value );
	*this += " ";
	*this += units[ measure ][ unit ];
	return unit;
}

/*
============
String::SetUnit
============
*/
void String::SetUnit( const char* format, float value, int unit, Measure_t measure )
{
	value /= 1 << ( unit * 10 );
	sprintf( *this, format, value );
	*this += " ";
	*this += units[ measure ][ unit ];
}

/*
================
String::InitMemory
================
*/
void String::InitMemory()
{
#ifdef USE_STRING_DATA_ALLOCATOR
	stringDataAllocator.Init();
#endif
}

/*
================
String::ShutdownMemory
================
*/
void String::ShutdownMemory()
{
#ifdef USE_STRING_DATA_ALLOCATOR
	stringDataAllocator.Shutdown();
#endif
}

/*
================
String::PurgeMemory
================
*/
void String::PurgeMemory()
{
#ifdef USE_STRING_DATA_ALLOCATOR
	stringDataAllocator.FreeEmptyBaseBlocks();
#endif
}

/*
================
String::FormatNumber
================
*/
struct formatList_t
{
	int			gran;
	int			count;
};

// elements of list need to decend in size
formatList_t formatList[] =
{
	{ 1000000000, 0 },
	{ 1000000, 0 },
	{ 1000, 0 }
};

int numFormatList = sizeof( formatList ) / sizeof( formatList[0] );


String String::FormatNumber( int number )
{
	String string;
	bool hit;
	
	// reset
	for( int i = 0; i < numFormatList; i++ )
	{
		formatList_t* li = formatList + i;
		li->count = 0;
	}
	
	// main loop
	do
	{
		hit = false;
		
		for( int i = 0; i < numFormatList; i++ )
		{
			formatList_t* li = formatList + i;
			
			if( number >= li->gran )
			{
				li->count++;
				number -= li->gran;
				hit = true;
				break;
			}
		}
	}
	while( hit );
	
	// print out
	bool found = false;
	
	for( int i = 0; i < numFormatList; i++ )
	{
		formatList_t* li = formatList + i;
		
		if( li->count )
		{
			if( !found )
			{
				string += va( "%i,", li->count );
			}
			else
			{
				string += va( "%3.3i,", li->count );
			}
			found = true;
		}
		else if( found )
		{
			string += va( "%3.3i,", li->count );
		}
	}
	
	if( found )
	{
		string += va( "%3.3i", number );
	}
	else
	{
		string += va( "%i", number );
	}
	
	// pad to proper size
	int count = 11 - string.Length();
	
	for( int i = 0; i < count; i++ )
	{
		string.Insert( " ", 0 );
	}
	
	return string;
}
