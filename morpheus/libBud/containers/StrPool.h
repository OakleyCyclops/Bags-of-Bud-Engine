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

#ifndef __STRPOOL_H__
#define __STRPOOL_H__

/*
===============================================================================

	budStrPool

===============================================================================
*/

class budStrPool;

class idPoolStr : public budStr
{
	friend class budStrPool;
	
public:
	idPoolStr()
	{
		numUsers = 0;
	}
	~idPoolStr()
	{
		assert( numUsers == 0 );
	}
	
	// returns total size of allocated memory
	size_t				Allocated() const
	{
		return budStr::Allocated();
	}
	// returns total size of allocated memory including size of string pool type
	size_t				Size() const
	{
		return sizeof( *this ) + Allocated();
	}
	// returns a pointer to the pool this string was allocated from
	const budStrPool* 	GetPool() const
	{
		return pool;
	}
	
private:
	budStrPool* 			pool;
	mutable int			numUsers;
};

class budStrPool
{
public:
	budStrPool()
	{
		caseSensitive = true;
	}
	
	void				SetCaseSensitive( bool caseSensitive );
	
	int					Num() const
	{
		return pool.Num();
	}
	size_t				Allocated() const;
	size_t				Size() const;
	
	const idPoolStr* 	operator[]( int index ) const
	{
		return pool[index];
	}
	
	const idPoolStr* 	AllocString( const char* string );
	void				FreeString( const idPoolStr* poolStr );
	const idPoolStr* 	CopyString( const idPoolStr* poolStr );
	void				Clear();
	
private:
	bool				caseSensitive;
	budList<idPoolStr*>	pool;
	budHashIndex			poolHash;
};

/*
================
budStrPool::SetCaseSensitive
================
*/
BUD_INLINE void budStrPool::SetCaseSensitive( bool caseSensitive )
{
	this->caseSensitive = caseSensitive;
}

/*
================
budStrPool::AllocString
================
*/
BUD_INLINE const idPoolStr* budStrPool::AllocString( const char* string )
{
	int i, hash;
	idPoolStr* poolStr;
	
	hash = poolHash.GenerateKey( string, caseSensitive );
	if( caseSensitive )
	{
		for( i = poolHash.First( hash ); i != -1; i = poolHash.Next( i ) )
		{
			if( pool[i]->Cmp( string ) == 0 )
			{
				pool[i]->numUsers++;
				return pool[i];
			}
		}
	}
	else
	{
		for( i = poolHash.First( hash ); i != -1; i = poolHash.Next( i ) )
		{
			if( pool[i]->Icmp( string ) == 0 )
			{
				pool[i]->numUsers++;
				return pool[i];
			}
		}
	}
	
	poolStr = new( TAG_libBud_STRING ) idPoolStr;
	*static_cast<budStr*>( poolStr ) = string;
	poolStr->pool = this;
	poolStr->numUsers = 1;
	poolHash.Add( hash, pool.Append( poolStr ) );
	return poolStr;
}

/*
================
budStrPool::FreeString
================
*/
BUD_INLINE void budStrPool::FreeString( const idPoolStr* poolStr )
{
	int i, hash;
	
	/*
	 * DG: numUsers can actually be 0 when shutting down the game, because then
	 * first budCommonLocal::Quit() -> budCommonLocal::Shutdown() -> libBud::Shutdown()
	 * -> idDict::Shutdown() -> idDict::globalKeys.Clear() and idDict::globalVars.Clear()
	 * is called and then, from destructors,
	 * ~budSessionLocal() => destroy idDict titleStorageVars -> ~idDict() -> idDict::Clear()
	 * -> idDict::globalVars.FreeString() and idDict::globalKeys.FreeString() (this function)
	 * is called, leading here.
	 * So just return if poolStr->numUsers < 1, instead of segfaulting/asserting below
	 * when i == -1 because nothing was found here. As there is no nice way to find out if
	 * we're shutting down (at this point) just get rid of the following assertion:
	 * assert( poolStr->numUsers >= 1 );
	 */
	if( poolStr->numUsers < 1 )
		return;
	// DG end
	
	assert( poolStr->pool == this );
	
	poolStr->numUsers--;
	if( poolStr->numUsers <= 0 )
	{
		hash = poolHash.GenerateKey( poolStr->c_str(), caseSensitive );
		if( caseSensitive )
		{
			for( i = poolHash.First( hash ); i != -1; i = poolHash.Next( i ) )
			{
				if( pool[i]->Cmp( poolStr->c_str() ) == 0 )
				{
					break;
				}
			}
		}
		else
		{
			for( i = poolHash.First( hash ); i != -1; i = poolHash.Next( i ) )
			{
				if( pool[i]->Icmp( poolStr->c_str() ) == 0 )
				{
					break;
				}
			}
		}
		assert( i != -1 );
		assert( pool[i] == poolStr );
		delete pool[i];
		pool.RemoveIndex( i );
		poolHash.RemoveIndex( hash, i );
	}
}

/*
================
budStrPool::CopyString
================
*/
BUD_INLINE const idPoolStr* budStrPool::CopyString( const idPoolStr* poolStr )
{

	assert( poolStr->numUsers >= 1 );
	
	if( poolStr->pool == this )
	{
		// the string is from this pool so just increase the user count
		poolStr->numUsers++;
		return poolStr;
	}
	else
	{
		// the string is from another pool so it needs to be re-allocated from this pool.
		return AllocString( poolStr->c_str() );
	}
}

/*
================
budStrPool::Clear
================
*/
BUD_INLINE void budStrPool::Clear()
{
	int i;
	
	for( i = 0; i < pool.Num(); i++ )
	{
		pool[i]->numUsers = 0;
	}
	pool.DeleteContents( true );
	poolHash.Free();
}

/*
================
budStrPool::Allocated
================
*/
BUD_INLINE size_t budStrPool::Allocated() const
{
	int i;
	size_t size;
	
	size = pool.Allocated() + poolHash.Allocated();
	for( i = 0; i < pool.Num(); i++ )
	{
		size += pool[i]->Allocated();
	}
	return size;
}

/*
================
budStrPool::Size
================
*/
BUD_INLINE size_t budStrPool::Size() const
{
	int i;
	size_t size;
	
	size = pool.Size() + poolHash.Size();
	for( i = 0; i < pool.Num(); i++ )
	{
		size += pool[i]->Size();
	}
	return size;
}

#endif /* !__STRPOOL_H__ */
