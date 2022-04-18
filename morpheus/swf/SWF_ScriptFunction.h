/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2015 Robert Beckebans

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
#ifndef __SWF_SCRIPTFUNCTION_H__
#define __SWF_SCRIPTFUNCTION_H__

/*
========================
Interface for calling functions from script
========================
*/
class budSWFScriptFunction
{
public:
	virtual ~budSWFScriptFunction() {};
	
	virtual budSWFScriptVar	Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
	{
		return budSWFScriptVar();
	}; // this should never be hit
	virtual void			AddRef() {};
	virtual void			Release() {};
	virtual budSWFScriptObject* GetPrototype()
	{
		return NULL;
	}
	virtual void			SetPrototype( budSWFScriptObject* _object ) { }
};

/*
========================
Interface for calling functions from script, implemented statically
========================
*/
class budSWFScriptFunction_Static : public budSWFScriptFunction
{
public:
	budSWFScriptFunction_Static() { }
	virtual void			AddRef() { }
	virtual void			Release() { }
};

/*
========================
Interface for calling functions from script, implemented natively on a nested class object
========================
*/
template< typename T >
class budSWFScriptFunction_Nested : public budSWFScriptFunction
{
protected:
	T* pThis;
public:
	budSWFScriptFunction_Nested() : pThis( NULL ) { }
	
	budSWFScriptFunction* 	Bind( T* _pThis )
	{
		pThis = _pThis;
		return this;
	}
	virtual void			AddRef() { }
	virtual void			Release() { }
};

/*
========================
Interface for calling functions from script, with reference counting
NOTE: The ref count starts at 0 here because it assumes you do an AddRef on the allocated
object.  The proper way is to start it at 1 and force the caller to Release, but that's
really kind of a pain in the ass.  It was made to be used like this:
object->Set( "myFunction", new budSWFScriptFunction_MyFunction() );
========================
*/
class budSWFScriptFunction_RefCounted : public budSWFScriptFunction
{
public:
	budSWFScriptFunction_RefCounted() : refCount( 0 ) { }
	void AddRef()
	{
		refCount++;
	}
	void Release()
	{
		if( --refCount <= 0 )
		{
			delete this;
		}
	}
private:
	int refCount;
};

/*
========================
Action Scripts can define a pool of constants then push values from that pool
The documentation isn't very clear on the scope of these things, but from what
I've gathered by testing, pool is per-function and copied into the function
whenever that function is declared.
========================
*/
class budSWFConstantPool
{
public:
	budSWFConstantPool();
	~budSWFConstantPool()
	{
		Clear();
	}
	
	void				Clear();
	void				Copy( const budSWFConstantPool& other );
	budSWFScriptString* Get( int n )
	{
		return pool[n];
	}
	void				Append( budSWFScriptString* s )
	{
		pool.Append( s );
	}
	
private:
	List< budSWFScriptString*, TAG_SWF > pool;
};

/*
========================
The budSWFStack class is just a helper routine for treating an List like a stack
========================
*/
class budSWFStack : public List< budSWFScriptVar >
{
public:
	budSWFScriptVar& A()
	{
		return operator[]( Num() - 1 );
	}
	budSWFScriptVar& B()
	{
		return operator[]( Num() - 2 );
	}
	budSWFScriptVar& C()
	{
		return operator[]( Num() - 3 );
	}
	budSWFScriptVar& D()
	{
		return operator[]( Num() - 4 );
	}
	void Pop( int n )
	{
		SetNum( Num() - n );
	}
};

/*
========================
budSWFScriptFunction_Script is a script function that's implemented in action script
========================
*/
class budSWFScriptFunction_Script : public budSWFScriptFunction
{
public:
	budSWFScriptFunction_Script() : refCount( 1 ), flags( 0 ), data( NULL ), length( 0 ), prototype( NULL ), defaultSprite( NULL )
	{
		registers.SetNum( 4 );
	}
	virtual		~budSWFScriptFunction_Script();
	
	static budSWFScriptFunction_Script* 	Alloc()
	{
		return new( TAG_SWF ) budSWFScriptFunction_Script;
	}
	void	AddRef()
	{
		refCount++;
	}
	void	Release()
	{
		if( --refCount == 0 )
		{
			delete this;
		}
	}
	
	// This could all be passed to Alloc (and was at one time) but in some places it's far more convenient to specify each separately
	void	SetFlags( uint16 _flags )
	{
		flags = _flags;
	}
	void	SetData( const byte* _data, uint32 _length )
	{
		data = _data;
		length = _length;
	}
	void	SetScope( List<budSWFScriptObject*>& scope );
	void	SetConstants( const budSWFConstantPool& _constants )
	{
		constants.Copy( _constants );
	}
	void	SetDefaultSprite( budSWFSpriteInstance* _sprite )
	{
		defaultSprite = _sprite;
	}
	void	AllocRegisters( int numRegs	)
	{
		registers.SetNum( numRegs );
	}
	void	AllocParameters( int numParms )
	{
		parameters.SetNum( numParms );
	}
	void	SetParameter( uint8 n, uint8 r, const char* name )
	{
		parameters[n].reg = r;
		parameters[n].name = name;
	}
	
	budSWFScriptObject* GetPrototype()
	{
		return prototype;
	}
	void	SetPrototype( budSWFScriptObject* _prototype )
	{
		_prototype->AddRef();
		assert( prototype == NULL );
		prototype = _prototype;
	}
	
	virtual budSWFScriptVar	Call( budSWFScriptObject* thisObject, const budSWFParmList& parms );
	
	// RB begin
	String CallToScript( budSWFScriptObject* thisObject, const budSWFParmList& parms, const char* filename, int characterID, int actionID );
	
private:
	budSWFScriptVar Run( budSWFScriptObject* thisObject, budSWFStack& stack, budSWFBitStream& bitstream );
	
	
	
	struct ActionBlock
	{
		ActionBlock*		parent = NULL;
		String				line;
		List<ActionBlock>	blocks;
	};
	List<ActionBlock>		actionBlocks;
	ActionBlock*			currentBlock;
	
	String		UpdateIndent( int indentLevel ) const;
	void		AddLine( const String& line );
	void		AddBlock( const String& line );
	void		QuitCurrentBlock();
	
	String		BuildActionCode( const List<ActionBlock>& blocks, int level );
	
	String		ExportToScript( budSWFScriptObject* thisObject, budSWFStack& stack, budSWFBitStream& bitstream, const char* filename, int characterID, int actionID );
	// RB end
	
private:
	int					refCount;
	
	uint16				flags;
	const  byte* 		data;
	uint32				length;
	budSWFScriptObject* prototype;
	
	budSWFSpriteInstance* defaultSprite;		// some actions have an implicit sprite they work off of (e.g. Action_GotoFrame outside of object scope)
	
	List< budSWFScriptObject*, TAG_SWF > scope;
	
	budSWFConstantPool	constants;
	List< budSWFScriptVar, TAG_SWF > registers;
	
	struct parmInfo_t
	{
		const char* name;
		uint8 reg;
	};
	List< parmInfo_t, TAG_SWF > parameters;
};

#endif // !__SWF_SCRIPTFUNCTION_H__
