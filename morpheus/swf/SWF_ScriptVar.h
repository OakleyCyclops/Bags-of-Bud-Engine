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
#ifndef __SWF_SCRIPTVAR_H__
#define __SWF_SCRIPTVAR_H__

class budSWFScriptObject;
class budSWFScriptFunction;

/*
========================
A reference counted string
========================
*/
class budSWFScriptString : public String
{
public:
	budSWFScriptString( const String& s ) : String( s ), refCount( 1 ) { }
	
	static budSWFScriptString* Alloc( const String& s )
	{
		return new( TAG_SWF ) budSWFScriptString( s );
	}
	BUD_INLINE void	AddRef()
	{
		refCount++;
	}
	BUD_INLINE void	Release()
	{
		if( --refCount == 0 )
		{
			delete this;
		}
	}
	
private:
	int refCount;
};

/*
========================
A variable in an action script
these can be on the stack, in a script object, passed around as parameters, etc
they can contain raw data (int, float), strings, functions, or objects
========================
*/
class budSWFScriptVar
{
public:
	budSWFScriptVar() : type( SWF_VAR_UNDEF ) { }
	budSWFScriptVar( const budSWFScriptVar& other );
	budSWFScriptVar( budSWFScriptObject* o ) : type( SWF_VAR_UNDEF )
	{
		SetObject( o );
	}
	budSWFScriptVar( StringId s ) : type( SWF_VAR_UNDEF )
	{
		SetString( s );
	}
	budSWFScriptVar( const String& s ) : type( SWF_VAR_UNDEF )
	{
		SetString( s );
	}
	budSWFScriptVar( const char* s ) : type( SWF_VAR_UNDEF )
	{
		SetString( String( s ) );
	}
	budSWFScriptVar( float f ) : type( SWF_VAR_UNDEF )
	{
		SetFloat( f );
	}
	budSWFScriptVar( bool b ) : type( SWF_VAR_UNDEF )
	{
		SetBool( b );
	}
	budSWFScriptVar( int32 i ) : type( SWF_VAR_UNDEF )
	{
		SetInteger( i );
	}
	budSWFScriptVar( budSWFScriptFunction* nf ) : type( SWF_VAR_UNDEF )
	{
		SetFunction( nf );
	}
	~budSWFScriptVar();
	
	budSWFScriptVar& operator=( const budSWFScriptVar& other );
	
	// implements ECMA 262 11.9.3
	bool AbstractEquals( const budSWFScriptVar& other );
	bool StrictEquals( const budSWFScriptVar& other );
	
	void SetString( StringId s )
	{
		Free();
		type = SWF_VAR_STRINGID;
		value.i = s.GetIndex();
	}
	void SetString( const String& s )
	{
		Free();
		type = SWF_VAR_STRING;
		value.string = budSWFScriptString::Alloc( s );
	}
	void SetString( const char* s )
	{
		Free();
		type = SWF_VAR_STRING;
		value.string = budSWFScriptString::Alloc( s );
	}
	void SetString( budSWFScriptString* s )
	{
		Free();
		type = SWF_VAR_STRING;
		value.string = s;
		s->AddRef();
	}
	
	// RB begin
	void SetResult( const String& s )
	{
		Free();
		type = SWF_VAR_RESULT;
		value.string = budSWFScriptString::Alloc( s );
	}
	void SetResult( const char* s )
	{
		Free();
		type = SWF_VAR_RESULT;
		value.string = budSWFScriptString::Alloc( s );
	}
	// RB end
	
	void SetFloat( float f )
	{
		Free();
		type = SWF_VAR_FLOAT;
		value.f = f;
	}
	void SetNULL()
	{
		Free();
		type = SWF_VAR_NULL;
	}
	void SetUndefined()
	{
		Free();
		type = SWF_VAR_UNDEF;
	}
	void SetBool( bool b )
	{
		Free();
		type = SWF_VAR_BOOL;
		value.b = b;
	}
	void SetInteger( int32 i )
	{
		Free();
		type = SWF_VAR_INTEGER;
		value.i = i;
	}
	
	void SetObject( budSWFScriptObject* o );
	void SetFunction( budSWFScriptFunction* f );
	
	String	ToString() const;
	float	ToFloat() const;
	bool	ToBool() const;
	int32	ToInteger() const;
	
	budSWFScriptObject* 		GetObject()
	{
		assert( type == SWF_VAR_OBJECT );
		return value.object;
	}
	budSWFScriptObject* 		GetObject() const
	{
		assert( type == SWF_VAR_OBJECT );
		return value.object;
	}
	budSWFScriptFunction* 	GetFunction()
	{
		assert( type == SWF_VAR_FUNCTION );
		return value.function;
	}
	budSWFSpriteInstance* 	ToSprite();
	budSWFTextInstance* 		ToText();
	
	budSWFScriptVar			GetNestedVar( const char* arg1, const char* arg2 = NULL, const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL );
	budSWFScriptObject* 		GetNestedObj( const char* arg1, const char* arg2 = NULL, const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL );
	budSWFSpriteInstance* 	GetNestedSprite( const char* arg1, const char* arg2 = NULL, const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL );
	budSWFTextInstance* 		GetNestedText( const char* arg1, const char* arg2 = NULL, const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL );
	
	const char* 			TypeOf() const;
	
	// debug print of this variable to the console
	void					PrintToConsole() const;
	
	bool IsString()		const
	{
		return ( type == SWF_VAR_STRING ) || ( type == SWF_VAR_STRINGID );
	}
	bool IsNULL()		const
	{
		return ( type == SWF_VAR_NULL );
	}
	bool IsUndefined()	const
	{
		return ( type == SWF_VAR_UNDEF );
	}
	bool IsValid()		const
	{
		return ( type != SWF_VAR_UNDEF ) && ( type != SWF_VAR_NULL );
	}
	bool IsFunction()	const
	{
		return ( type == SWF_VAR_FUNCTION );
	}
	bool IsObject()		const
	{
		return ( type == SWF_VAR_OBJECT );
	}
	bool IsNumeric()	const
	{
		return ( type == SWF_VAR_FLOAT ) || ( type == SWF_VAR_INTEGER ) || ( type == SWF_VAR_BOOL );
	}
	
	bool IsResult()		const
	{
		return ( type == SWF_VAR_RESULT );
	}
	
	enum swfScriptVarType
	{
		SWF_VAR_STRINGID,
		SWF_VAR_STRING,
		SWF_VAR_FLOAT,
		SWF_VAR_NULL,
		SWF_VAR_UNDEF,
		SWF_VAR_BOOL,
		SWF_VAR_INTEGER,
		SWF_VAR_FUNCTION,
		SWF_VAR_OBJECT,
		SWF_VAR_RESULT	// RB: for P-Code to Lua
	};
	
	swfScriptVarType	GetType() const
	{
		return type;
	}
	
private:
	void Free();
	swfScriptVarType type;
	
	union swfScriptVarValue_t
	{
		float	f;
		int32	i;
		bool	b;
		budSWFScriptObject* object;
		budSWFScriptString* string;
		budSWFScriptFunction* function;
	} value;
};

#endif // !__SWF_SCRIPTVAR_H__
