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
#ifndef	__STRSTATIC_H__
#define	__STRSTATIC_H__

/*
================================================
budStrStatic
================================================
*/
template< int _size_ >
class budStrStatic : public budStr
{
public:
	BUD_INLINE void operator=( const budStrStatic& text )
	{
		// we should only get here when the types, including the size, are identical
		len = text.Length();
		memcpy( data, text.data, len + 1 );
	}
	
	// all budStr operators are overloaded and the budStr default constructor is called so that the
	// static buffer can be initialized in the body of the constructor before the data is ever
	// copied.
	BUD_INLINE	budStrStatic()
	{
		buffer[ 0 ] = '\0';
		SetStaticBuffer( buffer, _size_ );
	}
	BUD_INLINE	budStrStatic( const budStrStatic& text ) :
		budStr()
	{
		buffer[ 0 ] = '\0';
		SetStaticBuffer( buffer, _size_ );
		budStr::operator=( text );
	}
	
	BUD_INLINE	budStrStatic( const budStr& text ) :
		budStr()
	{
		buffer[ 0 ] = '\0';
		SetStaticBuffer( buffer, _size_ );
		budStr::operator=( text );
	}
	
	BUD_INLINE	budStrStatic( const budStrStatic& text, int start, int end ) :
		budStr()
	{
		buffer[ 0 ] = '\0';
		SetStaticBuffer( buffer, _size_ );
		CopyRange( text.c_str(), start, end );
	}
	
	BUD_INLINE	budStrStatic( const char* text ) :
		budStr()
	{
		buffer[ 0 ] = '\0';
		SetStaticBuffer( buffer, _size_ );
		budStr::operator=( text );
	}
	
	BUD_INLINE	budStrStatic( const char* text, int start, int end ) :
		budStr()
	{
		buffer[ 0 ] = '\0';
		SetStaticBuffer( buffer, _size_ );
		CopyRange( text, start, end );
	}
	
	BUD_INLINE	explicit budStrStatic( const bool b ) :
		budStr()
	{
		buffer[ 0 ] = '\0';
		SetStaticBuffer( buffer, _size_ );
		budStr::operator=( budStr( b ) );
	}
	
	BUD_INLINE	explicit budStrStatic( const char c ) :
		budStr()
	{
		buffer[ 0 ] = '\0';
		SetStaticBuffer( buffer, _size_ );
		budStr::operator=( budStr( c ) );
	}
	
	BUD_INLINE	explicit budStrStatic( const int i ) :
		budStr()
	{
		buffer[ 0 ] = '\0';
		SetStaticBuffer( buffer, _size_ );
		budStr::operator=( budStr( i ) );
	}
	
	BUD_INLINE	explicit budStrStatic( const unsigned u ) :
		budStr()
	{
		buffer[ 0 ] = '\0';
		SetStaticBuffer( buffer, _size_ );
		budStr::operator=( budStr( u ) );
	}
	
	BUD_INLINE	explicit budStrStatic( const float f ) :
		budStr()
	{
		buffer[ 0 ] = '\0';
		SetStaticBuffer( buffer, _size_ );
		budStr::operator=( budStr( f ) );
	}
	
private:
	char		buffer[ _size_ ];
};
#endif	// __STRSTATIC_H__ 
