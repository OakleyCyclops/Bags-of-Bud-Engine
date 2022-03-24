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

/*
========================
budSWF::DefineShape
========================
*/
void budSWF::DefineShape( budSWFBitStream& bitstream )
{
	uint16 characterID = bitstream.ReadU16();
	budSWFDictionaryEntry* entry = AddDictionaryEntry( characterID, SWF_DICT_SHAPE );
	if( entry == NULL )
	{
		return;
	}
	
	budSWFShapeParser swfShapeParser;
	swfShapeParser.Parse( bitstream, *entry->shape, 1 );
}

/*
========================
budSWF::DefineShape2
========================
*/
void budSWF::DefineShape2( budSWFBitStream& bitstream )
{
	uint16 characterID = bitstream.ReadU16();
	budSWFDictionaryEntry* entry = AddDictionaryEntry( characterID, SWF_DICT_SHAPE );
	if( entry == NULL )
	{
		return;
	}
	
	budSWFShapeParser swfShapeParser;
	swfShapeParser.Parse( bitstream, *entry->shape, 2 );
}

/*
========================
budSWF::DefineShape3
========================
*/
void budSWF::DefineShape3( budSWFBitStream& bitstream )
{
	uint16 characterID = bitstream.ReadU16();
	budSWFDictionaryEntry* entry = AddDictionaryEntry( characterID, SWF_DICT_SHAPE );
	if( entry == NULL )
	{
		return;
	}
	
	budSWFShapeParser swfShapeParser;
	swfShapeParser.Parse( bitstream, *entry->shape, 3 );
}

/*
========================
budSWF::DefineShape4
========================
*/
void budSWF::DefineShape4( budSWFBitStream& bitstream )
{
	uint16 characterID = bitstream.ReadU16();
	budSWFDictionaryEntry* entry = AddDictionaryEntry( characterID, SWF_DICT_SHAPE );
	if( entry == NULL )
	{
		return;
	}
	
	budSWFShapeParser swfShapeParser;
	swfShapeParser.Parse( bitstream, *entry->shape, 4 );
}


/*
========================
budSWF::DefineMorphShape
========================
*/
void budSWF::DefineMorphShape( budSWFBitStream& bitstream )
{
	uint16 characterID = bitstream.ReadU16();
	budSWFDictionaryEntry* entry = AddDictionaryEntry( characterID, SWF_DICT_MORPH );
	if( entry == NULL )
	{
		return;
	}
	
	budSWFShapeParser swfShapeParser;
	swfShapeParser.ParseMorph( bitstream, *entry->shape );
}
