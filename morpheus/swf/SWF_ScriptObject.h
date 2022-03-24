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
#ifndef __SWF_SCRIPTOBJECT_H__
#define __SWF_SCRIPTOBJECT_H__

class budSWFSpriteInstance;

/*
========================
This is the base class for script variables which are implemented in code
========================
*/
class budSWFScriptNativeVariable
{
public:
	virtual bool IsReadOnly()
	{
		return false;
	}
	virtual void Set( class budSWFScriptObject* object, const budSWFScriptVar& value ) = 0;
	virtual budSWFScriptVar Get( class budSWFScriptObject* object ) = 0;
};

#define SWF_NATIVE_VAR_DECLARE( x ) \
	class budSWFScriptNativeVar_##x : public budSWFScriptNativeVariable {			\
	public:																		\
		void Set( class budSWFScriptObject * object, const budSWFScriptVar & value );	\
		budSWFScriptVar Get( class budSWFScriptObject * object );					\
	} swfScriptVar_##x;

#define SWF_NATIVE_VAR_DECLARE_READONLY( x )									\
	class budSWFScriptNativeVar_##x : public budSWFScriptNativeVariable {			\
	public:																		\
		bool IsReadOnly() { return true; }										\
		void Set( class budSWFScriptObject * object, const budSWFScriptVar & value ) { assert( false ); } \
		budSWFScriptVar Get( class budSWFScriptObject * object );					\
	} swfScriptVar_##x;

/*
========================
This is a helper class for quickly setting up native variables which need access to a parent class
========================
*/
template< typename T >
class budSWFScriptNativeVariable_Nested : public budSWFScriptNativeVariable
{
public:
	budSWFScriptNativeVariable_Nested() : pThis( NULL ) { }
	budSWFScriptNativeVariable_Nested* Bind( T* p )
	{
		pThis = p;
		return this;
	}
	virtual void Set( class budSWFScriptObject* object, const budSWFScriptVar& value ) = 0;
	virtual budSWFScriptVar Get( class budSWFScriptObject* object ) = 0;
protected:
	T* pThis;
};

#define SWF_NATIVE_VAR_DECLARE_NESTED( x, y ) \
	class budSWFScriptNativeVar_##x : public budSWFScriptNativeVariable_Nested<y> {	\
	public:																			\
		void Set( class budSWFScriptObject * object, const budSWFScriptVar & value );	\
		budSWFScriptVar Get( class budSWFScriptObject * object );						\
	} swfScriptVar_##x;

#define SWF_NATIVE_VAR_DECLARE_NESTED_READONLY( x, y, z )							\
	class budSWFScriptNativeVar_##x : public budSWFScriptNativeVariable_Nested<y> {	\
	public:																			\
		bool IsReadOnly() { return true; }											\
		void Set( class budSWFScriptObject * object, const budSWFScriptVar & value ) { assert( false ); } \
		budSWFScriptVar Get( class budSWFScriptObject * object ) { return pThis->z; }	\
	} swfScriptVar_##x;

/*
========================
An object in an action script is a collection of variables. functions are also variables.
========================
*/
class budSWFScriptObject
{
public:
	budSWFScriptObject();
	virtual					~budSWFScriptObject();
	
	static budSWFScriptObject* 	Alloc();
	void					AddRef();
	void					Release();
	void					SetNoAutoDelete( bool b )
	{
		noAutoDelete = b;
	}
	
	void					Clear();
	
	void					MakeArray();
	
	void					SetSprite( budSWFSpriteInstance* s )
	{
		objectType = SWF_OBJECT_SPRITE;
		data.sprite = s;
	}
	budSWFSpriteInstance* 	GetSprite()
	{
		return ( objectType == SWF_OBJECT_SPRITE ) ? data.sprite : NULL;
	}
	
	void					SetText( budSWFTextInstance* t )
	{
		objectType = SWF_OBJECT_TEXT;
		data.text = t;
	}
	budSWFTextInstance* 		GetText()
	{
		return ( objectType == SWF_OBJECT_TEXT ) ? data.text : NULL;
	}
	
	// Also accessible via __proto__ property
	budSWFScriptObject* 		GetPrototype()
	{
		return prototype;
	}
	void					SetPrototype( budSWFScriptObject* _prototype )
	{
		assert( prototype == NULL );
		prototype = _prototype;
		prototype->AddRef();
	}
	budSWFScriptVar			Get( int index );
	budSWFScriptVar			Get( const char* name );
	budSWFSpriteInstance* 	GetSprite( int index );
	budSWFSpriteInstance* 	GetSprite( const char* name );
	budSWFScriptObject* 		GetObject( int index );
	budSWFScriptObject* 		GetObject( const char* name );
	budSWFTextInstance* 		GetText( int index );
	budSWFTextInstance* 		GetText( const char* name );
	void					Set( int index, const budSWFScriptVar& value );
	void					Set( const char* name, const budSWFScriptVar& value );
	void					SetNative( const char* name, budSWFScriptNativeVariable* native );
	bool					HasProperty( const char* name );
	bool					HasValidProperty( const char* name );
	budSWFScriptVar			DefaultValue( bool stringHint );
	
	// This is to implement for-in (fixme: respect DONTENUM flag)
	int						NumVariables()
	{
		return variables.Num();
	}
	const char* 			EnumVariable( int i )
	{
		return variables[i].name;
	}
	
	budSWFScriptVar			GetNestedVar( const char* arg1, const char* arg2 = NULL, const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL );
	budSWFScriptObject* 		GetNestedObj( const char* arg1, const char* arg2 = NULL, const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL );
	budSWFSpriteInstance* 	GetNestedSprite( const char* arg1, const char* arg2 = NULL, const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL );
	budSWFTextInstance* 		GetNestedText( const char* arg1, const char* arg2 = NULL, const char* arg3 = NULL, const char* arg4 = NULL, const char* arg5 = NULL, const char* arg6 = NULL );
	
	void					PrintToConsole() const;
	
private:
	int refCount;
	bool noAutoDelete;
	
	enum swfNamedVarFlags_t
	{
		SWF_VAR_FLAG_NONE = 0,
		SWF_VAR_FLAG_READONLY = BIT( 1 ),
		SWF_VAR_FLAG_DONTENUM = BIT( 2 )
	};
	struct swfNamedVar_t
	{
		swfNamedVar_t() : native( NULL ) { }
		~swfNamedVar_t();
		swfNamedVar_t& operator=( const swfNamedVar_t& other );
		
		int							index;
		int							hashNext;
		budStr						name;
		budSWFScriptVar				value;
		budSWFScriptNativeVariable* 	native;
		int							flags;
	};
	budList< swfNamedVar_t, TAG_SWF >	variables;
	
	static const int VARIABLE_HASH_BUCKETS = 16;
	int	variablesHash[VARIABLE_HASH_BUCKETS];
	
	budSWFScriptObject* 		prototype;
	
	enum swfObjectType_t
	{
		SWF_OBJECT_OBJECT,
		SWF_OBJECT_ARRAY,
		SWF_OBJECT_SPRITE,
		SWF_OBJECT_TEXT
	} objectType;
	
	union swfObjectData_t
	{
		budSWFSpriteInstance* 	sprite;			// only valid if objectType == SWF_OBJECT_SPRITE
		budSWFTextInstance* 		text;			// only valid if objectType == SWF_OBJECT_TEXT
	} data;
	
	swfNamedVar_t* 	GetVariable( int index, bool create );
	swfNamedVar_t* 	GetVariable( const char* name, bool create );
};

#endif // !__SWF_SCRIPTOBJECT_H__
