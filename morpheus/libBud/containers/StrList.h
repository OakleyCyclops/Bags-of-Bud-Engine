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

#ifndef __STRLIST_H__
#define __STRLIST_H__

/*
===============================================================================

	budStrList

===============================================================================
*/

typedef budList<budStr> budStrList;
typedef budList<budStr*> budStrPtrList;
typedef budStr* budStrPtr;

///*
//================
//budListSortCompare<budStrPtr>
//
//Compares two pointers to strings. Used to sort a list of string pointers alphabetically in budList<budStr>::Sort.
//================
//*/
//template<>
//BUD_INLINE int budListSortCompare<budStrPtr, memTag_t _tag_ >( const budStrPtr *a, const budStrPtr *b ) {
//	return ( *a )->Icmp( **b );
//}

///*
//================
//budStrList::Sort
//
//Sorts the list of strings alphabetically. Creates a list of pointers to the actual strings and sorts the
//pointer list. Then copies the strings into another list using the ordered list of pointers.
//================
//*/
//template<>
//BUD_INLINE void budStrList::Sort( cmp_t *compare ) {
//	int i;
//
//	if ( !num ) {
//		return;
//	}
//
//	budList<budStr>		other;
//	budList<budStrPtr>	pointerList;
//
//	pointerList.SetNum( num );
//	for( i = 0; i < num; i++ ) {
//		pointerList[ i ] = &( *this )[ i ];
//	}
//
//	pointerList.Sort();
//
//	other.SetNum( num );
//	other.SetGranularity( granularity );
//	for( i = 0; i < other.Num(); i++ ) {
//		other[ i ] = *pointerList[ i ];
//	}
//
//	this->Swap( other );
//}

///*
//================
//budStrList::SortSubSection
//
//Sorts a subsection of the list of strings alphabetically.
//================
//*/
//template<>
//BUD_INLINE void budStrList::SortSubSection( int startIndex, int endIndex, cmp_t *compare ) {
//	int i, s;
//
//	if ( !num ) {
//		return;
//	}
//	if ( startIndex < 0 ) {
//		startIndex = 0;
//	}
//	if ( endIndex >= num ) {
//		endIndex = num - 1;
//	}
//	if ( startIndex >= endIndex ) {
//		return;
//	}
//
//	budList<budStr>		other;
//	budList<budStrPtr>	pointerList;
//
//	s = endIndex - startIndex + 1;
//	other.SetNum( s );
//	pointerList.SetNum( s );
//	for( i = 0; i < s; i++ ) {
//		other[ i ] = ( *this )[ startIndex + i ];
//		pointerList[ i ] = &other[ i ];
//	}
//
//	pointerList.Sort();
//
//	for( i = 0; i < s; i++ ) {
//		(*this)[ startIndex + i ] = *pointerList[ i ];
//	}
//}

/*
================
budStrList::Size
================
*/
template<>
BUD_INLINE size_t budStrList::Size() const
{
	size_t s;
	int i;
	
	s = sizeof( *this );
	for( i = 0; i < Num(); i++ )
	{
		s += ( *this )[ i ].Size();
	}
	
	return s;
}

/*
===============================================================================

	budStrList path sorting

===============================================================================
*/
//
///*
//================
//budListSortComparePaths
//
//Compares two pointers to strings. Used to sort a list of string pointers alphabetically in budList<budStr>::Sort.
//================
//*/
//template<class budStrPtr>
//BUD_INLINE int budListSortComparePaths( const budStrPtr *a, const budStrPtr *b ) {
//	return ( *a )->IcmpPath( **b );
//}

///*
//================
//budStrListSortPaths
//
//Sorts the list of path strings alphabetically and makes sure folders come first.
//================
//*/
//BUD_INLINE void budStrListSortPaths( budStrList &list ) {
//	int i;
//
//	if ( !list.Num() ) {
//		return;
//	}
//
//	budList<budStr>		other;
//	budList<budStrPtr>	pointerList;
//
//	pointerList.SetNum( list.Num() );
//	for( i = 0; i < list.Num(); i++ ) {
//		pointerList[ i ] = &list[ i ];
//	}
//
//	pointerList.Sort( budListSortComparePaths<budStrPtr> );
//
//	other.SetNum( list.Num() );
//	other.SetGranularity( list.GetGranularity() );
//	for( i = 0; i < other.Num(); i++ ) {
//		other[ i ] = *pointerList[ i ];
//	}
//
//	list.Swap( other );
//}

#endif /* !__STRLIST_H__ */
