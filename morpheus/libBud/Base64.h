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

#ifndef __BASE64_H__
#define __BASE64_H__

/*
===============================================================================

	base64

===============================================================================
*/

class budBase64
{
public:
	budBase64();
	budBase64( const budStr& s );
	~budBase64();
	
	void		Encode( const byte* from, int size );
	void		Encode( const budStr& src );
	int			DecodeLength() const; // minimum size in bytes of destination buffer for decoding
	int			Decode( byte* to ) const; // does not append a \0 - needs a DecodeLength() bytes buffer
	void		Decode( budStr& dest ) const; // decodes the binary content to an budStr (a bit dodgy, \0 and other non-ascii are possible in the decoded content)
	void		Decode( budFile* dest ) const;
	
	const char*	c_str() const;
	
	void 		operator=( const budStr& s );
	
private:
	byte* 		data;
	int			len;
	int			alloced;
	
	void		Init();
	void		Release();
	void		EnsureAlloced( int size );
};

BUD_INLINE budBase64::budBase64()
{
	Init();
}

BUD_INLINE budBase64::budBase64( const budStr& s )
{
	Init();
	*this = s;
}

BUD_INLINE budBase64::~budBase64()
{
	Release();
}

BUD_INLINE const char* budBase64::c_str() const
{
	return ( const char* )data;
}

BUD_INLINE void budBase64::Init()
{
	len = 0;
	alloced = 0;
	data = NULL;
}

BUD_INLINE void budBase64::Release()
{
	if( data )
	{
		delete[] data;
	}
	Init();
}

BUD_INLINE void budBase64::EnsureAlloced( int size )
{
	if( size > alloced )
	{
		Release();
	}
	data = new( TAG_libBud ) byte[size];
	alloced = size;
}

BUD_INLINE void budBase64::operator=( const budStr& s )
{
	EnsureAlloced( s.Length() + 1 ); // trailing \0 - beware, this does a Release
	strcpy( ( char* )data, s.c_str() );
	len = s.Length();
}

#endif /* !__BASE64_H__ */
