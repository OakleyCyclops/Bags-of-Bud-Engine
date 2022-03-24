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
#pragma hdrstop
#include "PCH.hpp"

budCVar swf_debugShowAddress( "swf_debugShowAddress", "0", CVAR_BOOL, "shows addresses along with object types when they are serialized" );


/*
========================
budSWFScriptObject::swfNamedVar_t::~swfNamedVar_t
========================
*/
budSWFScriptObject::swfNamedVar_t::~swfNamedVar_t()
{
}

/*
========================
budSWFScriptObject::swfNamedVar_t::operator=
========================
*/
budSWFScriptObject::swfNamedVar_t& budSWFScriptObject::swfNamedVar_t::operator=( const swfNamedVar_t& other )
{
	if( &other != this )
	{
		index = other.index;
		name = other.name;
		hashNext = other.hashNext;
		value = other.value;
		native = other.native;
		flags = other.flags;
	}
	return *this;
}

/*
========================
budSWFScriptObject::budSWFScriptObject
========================
*/
budSWFScriptObject::budSWFScriptObject() : refCount( 1 ), noAutoDelete( false ), prototype( NULL ), objectType( SWF_OBJECT_OBJECT )
{
	data.sprite = NULL;
	data.text = NULL;
	Clear();
	refCount = 1;
}

/*
========================
budSWFScriptObject::~budSWFScriptObject
========================
*/
budSWFScriptObject::~budSWFScriptObject()
{
	if( prototype != NULL )
	{
		prototype->Release();
	}
}

/*
========================
budSWFScriptObject::Alloc
========================
*/
budSWFScriptObject* 	budSWFScriptObject::Alloc()
{
	return new( TAG_SWF ) budSWFScriptObject;
}

/*
========================
budSWFScriptObject::AddRef
========================
*/
void budSWFScriptObject::AddRef()
{
	refCount++;
}

/*
========================
budSWFScriptObject::Release
========================
*/
void budSWFScriptObject::Release()
{
	if( --refCount == 0 && !noAutoDelete )
	{
		delete this;
	}
}

/*
========================
budSWFScriptObject::Clear
========================
*/
void budSWFScriptObject::Clear()
{
	variables.Clear();
	for( int i = 0; i < VARIABLE_HASH_BUCKETS; i++ )
	{
		variablesHash[i] = -1;
	}
}

/*
========================
budSWFScriptObject::HasProperty
========================
*/
bool budSWFScriptObject::HasProperty( const char* name )
{
	return ( GetVariable( name, false ) != NULL );
}

/*
========================
budSWFScriptObject::HasValidProperty
========================
*/
bool budSWFScriptObject::HasValidProperty( const char* name )
{
	budSWFScriptObject::swfNamedVar_t* const variable = GetVariable( name, false );
	if( variable == NULL )
	{
		return false;
	}
	if( variable->native != NULL )
	{
		budSWFScriptVar nv = variable->native->Get( this );
		if( nv.IsNULL() || nv.IsUndefined() )
		{
			return false;
		}
	}
	else
	{
		if( variable->value.IsNULL() || variable->value.IsUndefined() )
		{
			return false;
		}
	}
	return true;
}

/*
========================
budSWFScriptObject::Get
========================
*/
budSWFScriptVar budSWFScriptObject::Get( const char* name )
{
	swfNamedVar_t* variable = GetVariable( name, false );
	if( variable == NULL )
	{
		return budSWFScriptVar();
	}
	else
	{
		if( variable->native )
		{
			return variable->native->Get( this );
		}
		else
		{
			return variable->value;
		}
	}
}

/*
========================
budSWFScriptObject::Get
========================
*/
budSWFScriptVar budSWFScriptObject::Get( int index )
{
	swfNamedVar_t* variable = GetVariable( index, false );
	if( variable == NULL )
	{
		return budSWFScriptVar();
	}
	else
	{
		if( variable->native )
		{
			return variable->native->Get( this );
		}
		else
		{
			return variable->value;
		}
	}
}

/*
========================
budSWFScriptObject::GetSprite
========================
*/
budSWFSpriteInstance* budSWFScriptObject::GetSprite( int index )
{
	budSWFScriptVar var = Get( index );
	return var.ToSprite();
}

/*
========================
budSWFScriptObject::GetSprite
========================
*/
budSWFSpriteInstance* budSWFScriptObject::GetSprite( const char* name )
{
	budSWFScriptVar var = Get( name );
	return var.ToSprite();
}

/*
========================
budSWFScriptObject::GetObject
========================
*/
budSWFScriptObject* budSWFScriptObject::GetObject( int index )
{
	budSWFScriptVar var = Get( index );
	if( var.IsObject() )
	{
		return var.GetObject();
	}
	return NULL;
}

/*
========================
budSWFScriptObject::GetObject
========================
*/
budSWFScriptObject* budSWFScriptObject::GetObject( const char* name )
{
	budSWFScriptVar var = Get( name );
	if( var.IsObject() )
	{
		return var.GetObject();
	}
	return NULL;
}

/*
========================
budSWFScriptObject::GetText
========================
*/
budSWFTextInstance* budSWFScriptObject::GetText( int index )
{
	budSWFScriptVar var = Get( index );
	if( var.IsObject() )
	{
		return var.GetObject()->GetText();
	}
	return NULL;
}

/*
========================
budSWFScriptObject::GetText
========================
*/
budSWFTextInstance* budSWFScriptObject::GetText( const char* name )
{
	budSWFScriptVar var = Get( name );
	if( var.IsObject() )
	{
		return var.GetObject()->GetText();
	}
	return NULL;
}

/*
========================
budSWFScriptObject::Set
========================
*/
void budSWFScriptObject::Set( const char* name, const budSWFScriptVar& value )
{
	if( objectType == SWF_OBJECT_ARRAY )
	{
		if( budStr::Cmp( name, "length" ) == 0 )
		{
			int newLength = value.ToInteger();
			for( int i = 0; i < variables.Num(); i++ )
			{
				if( variables[i].index >= newLength )
				{
					variables.RemoveIndexFast( i );
					i--;
				}
			}
			// rebuild the hash table
			for( int i = 0; i < VARIABLE_HASH_BUCKETS; i++ )
			{
				variablesHash[i] = -1;
			}
			for( int i = 0; i < variables.Num(); i++ )
			{
				int hash = budStr::Hash( variables[i].name.c_str() ) & ( VARIABLE_HASH_BUCKETS - 1 );
				variables[i].hashNext = variablesHash[hash];
				variablesHash[hash] = i;
			}
		}
		else
		{
			int iName = atoi( name );
			if( iName > 0 || ( iName == 0 && budStr::Cmp( name, "0" ) == 0 ) )
			{
				swfNamedVar_t* lengthVar = GetVariable( "length", true );
				if( lengthVar->value.ToInteger() <= iName )
				{
					lengthVar->value = budSWFScriptVar( iName + 1 );
				}
			}
		}
	}
	
	swfNamedVar_t* variable = GetVariable( name, true );
	if( variable->native )
	{
		variable->native->Set( this, value );
	}
	else if( ( variable->flags & SWF_VAR_FLAG_READONLY ) == 0 )
	{
		variable->value = value;
	}
}

/*
========================
budSWFScriptObject::Set
========================
*/
void budSWFScriptObject::Set( int index, const budSWFScriptVar& value )
{
	if( index < 0 )
	{
		extern budCVar swf_debug;
		if( swf_debug.GetBool() )
		{
			libBud::Printf( "SWF: Trying to set a negative array index.\n" );
		}
		return;
	}
	if( objectType == SWF_OBJECT_ARRAY )
	{
		swfNamedVar_t* lengthVar = GetVariable( "length", true );
		if( lengthVar->value.ToInteger() <= index )
		{
			lengthVar->value = budSWFScriptVar( index + 1 );
		}
	}
	
	swfNamedVar_t* variable = GetVariable( index, true );
	if( variable->native )
	{
		variable->native->Set( this, value );
	}
	else if( ( variable->flags & SWF_VAR_FLAG_READONLY ) == 0 )
	{
		variable->value = value;
	}
}

/*
========================
budSWFScriptObject::SetNative
========================
*/
void budSWFScriptObject::SetNative( const char* name, budSWFScriptNativeVariable* native )
{
	swfNamedVar_t* variable = GetVariable( name, true );
	variable->flags = SWF_VAR_FLAG_DONTENUM;
	variable->native = native;
	if( native->IsReadOnly() )
	{
		variable->flags |= SWF_VAR_FLAG_READONLY;
	}
}

/*
========================
budSWFScriptObject::DefaultValue
========================
*/
budSWFScriptVar budSWFScriptObject::DefaultValue( bool stringHint )
{
	const char* methods[2] = { "toString", "valueOf" };
	if( !stringHint )
	{
		SwapValues( methods[0], methods[1] );
	}
	for( int i = 0; i < 2; i++ )
	{
		budSWFScriptVar method = Get( methods[i] );
		if( method.IsFunction() )
		{
			budSWFScriptVar value = method.GetFunction()->Call( this, budSWFParmList() );
			if( !value.IsObject() && !value.IsFunction() )
			{
				return value;
			}
		}
	}
	switch( objectType )
	{
		case SWF_OBJECT_OBJECT:
			if( swf_debugShowAddress.GetBool() )
			{
				return budSWFScriptVar( va( "[object:%p]", this ) );
			}
			else
			{
				return budSWFScriptVar( "[object]" );
			}
		case SWF_OBJECT_ARRAY:
			if( swf_debugShowAddress.GetBool() )
			{
				return budSWFScriptVar( va( "[array:%p]", this ) );
			}
			else
			{
				return budSWFScriptVar( "[array]" );
			}
		case SWF_OBJECT_SPRITE:
			if( data.sprite != NULL )
			{
				if( data.sprite->parent == NULL )
				{
					return budSWFScriptVar( "[_root]" );
				}
				else
				{
					return budSWFScriptVar( va( "[%s]", data.sprite->GetName() ) );
				}
			}
			else
			{
				return budSWFScriptVar( "[NULL]" );
			}
		case SWF_OBJECT_TEXT:
			if( swf_debugShowAddress.GetBool() )
			{
				return budSWFScriptVar( va( "[edittext:%p]", this ) );
			}
			else
			{
				return budSWFScriptVar( "[edittext]" );
			}
	}
	return budSWFScriptVar( "[unknown]" );
}

/*
========================
budSWFScriptObject::GetVariable
========================
*/
budSWFScriptObject::swfNamedVar_t* budSWFScriptObject::GetVariable( int index, bool create )
{
	for( int i = 0; i < variables.Num(); i++ )
	{
		if( variables[i].index == index )
		{
			return &variables[i];
		}
	}
	if( create )
	{
		swfNamedVar_t* variable = &variables.Alloc();
		variable->flags = SWF_VAR_FLAG_NONE;
		variable->index = index;
		variable->name = va( "%d", index );
		variable->native = NULL;
		int hash = budStr::Hash( variable->name ) & ( VARIABLE_HASH_BUCKETS - 1 );
		variable->hashNext = variablesHash[hash];
		variablesHash[hash] = variables.Num() - 1;
		return variable;
	}
	return NULL;
}

/*
========================
budSWFScriptObject::GetVariable
========================
*/
budSWFScriptObject::swfNamedVar_t* budSWFScriptObject::GetVariable( const char* name, bool create )
{
	int hash = budStr::Hash( name ) & ( VARIABLE_HASH_BUCKETS - 1 );
	for( int i = variablesHash[hash]; i >= 0; i = variables[i].hashNext )
	{
		if( variables[i].name == name )
		{
			return &variables[i];
		}
	}
	
	if( prototype != NULL )
	{
		swfNamedVar_t* variable = prototype->GetVariable( name, false );
		if( ( variable != NULL ) && ( variable->native || !create ) )
		{
			// If the variable is native, we want to pull it from the prototype even if we're going to set it
			return variable;
		}
	}
	
	if( create )
	{
		swfNamedVar_t* variable = &variables.Alloc();
		variable->flags = SWF_VAR_FLAG_NONE;
		variable->index = atoi( name );
		if( variable->index == 0 && budStr::Cmp( name, "0" ) != 0 )
		{
			variable->index = -1;
		}
		variable->name = name;
		variable->native = NULL;
		variable->hashNext = variablesHash[hash];
		variablesHash[hash] = variables.Num() - 1;
		return variable;
	}
	return NULL;
}

/*
========================
budSWFScriptObject::MakeArray
========================
*/
void budSWFScriptObject::MakeArray()
{
	objectType = SWF_OBJECT_ARRAY;
	swfNamedVar_t* variable = GetVariable( "length", true );
	variable->value = budSWFScriptVar( 0 );
	variable->flags = SWF_VAR_FLAG_DONTENUM;
}

/*
========================
budSWFScriptObject::GetNestedVar
========================
*/
budSWFScriptVar budSWFScriptObject::GetNestedVar( const char* arg1, const char* arg2, const char* arg3, const char* arg4, const char* arg5, const char* arg6 )
{
	const char* const args[] = { arg1, arg2, arg3, arg4, arg5, arg6 };
	const int numArgs = sizeof( args ) / sizeof( const char* );
	
	budStaticList< const char*, numArgs > vars;
	for( int i = 0; i < numArgs && args[ i ] != NULL; ++i )
	{
		vars.Append( args[ i ] );
	}
	
	budSWFScriptObject* baseObject = this;
	budSWFScriptVar retVal;
	
	for( int i = 0; i < vars.Num(); ++i )
	{
		budSWFScriptVar var = baseObject->Get( vars[ i ] );
		
		// when at the end of object path just use the latest value as result
		if( i == vars.Num() - 1 )
		{
			retVal = var;
			break;
		}
		
		// encountered variable in path that wasn't an object
		if( !var.IsObject() )
		{
			retVal = budSWFScriptVar();
			break;
		}
		
		baseObject = var.GetObject();
	}
	
	return retVal;
}

/*
========================
budSWFScriptObject::GetNestedObj
========================
*/
budSWFScriptObject* budSWFScriptObject::GetNestedObj( const char* arg1, const char* arg2, const char* arg3, const char* arg4, const char* arg5, const char* arg6 )
{
	budSWFScriptVar var = GetNestedVar( arg1, arg2, arg3, arg4, arg5, arg6 );
	
	if( !var.IsObject() )
	{
		return NULL;
	}
	
	return var.GetObject();
}

/*
========================
budSWFScriptObject::GetNestedSprite
========================
*/
budSWFSpriteInstance* budSWFScriptObject::GetNestedSprite( const char* arg1, const char* arg2, const char* arg3, const char* arg4, const char* arg5, const char* arg6 )
{
	budSWFScriptVar var = GetNestedVar( arg1, arg2, arg3, arg4, arg5, arg6 );
	return var.ToSprite();
	
}

/*
========================
budSWFScriptObject::GetNestedText
========================
*/
budSWFTextInstance* budSWFScriptObject::GetNestedText( const char* arg1, const char* arg2, const char* arg3, const char* arg4, const char* arg5, const char* arg6 )
{
	budSWFScriptVar var = GetNestedVar( arg1, arg2, arg3, arg4, arg5, arg6 );
	return var.ToText();
	
}

/*
========================
budSWFScriptObject::PrintToConsole
========================
*/
void budSWFScriptObject::PrintToConsole() const
{
	if( variables.Num() > 0 )
	{
		libBud::Printf( "%d subelements:\n", variables.Num() );
		int maxVarLength = 0;
		
		for( int i = 0; i < variables.Num(); ++i )
		{
			const budSWFScriptObject::swfNamedVar_t& nv = variables[ i ];
			const int nameLength = budStr::Length( nv.name );
			if( maxVarLength < nameLength )
			{
				maxVarLength = nameLength;
			}
		}
		
		maxVarLength += 2;	// a little extra padding
		
		const char* const fmt = va( "%%-%ds %%-10s %%-s\n", maxVarLength );
		libBud::Printf( fmt, "Name", "Type", "Value" );
		libBud::Printf( "------------------------------------------------------------\n" );
		for( int i = 0; i < variables.Num(); ++i )
		{
			const budSWFScriptObject::swfNamedVar_t& nv = variables[ i ];
			libBud::Printf( fmt, nv.name.c_str(), nv.value.TypeOf(),
						   nv.value.ToString().c_str() );
		}
	}
	else
	{
		libBud::Printf( "No subelements\n" );
	}
}
