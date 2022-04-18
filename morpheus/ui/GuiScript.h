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
#ifndef __GUISCRIPT_H
#define __GUISCRIPT_H

#include "Window.h"
#include "Winvar.h"

struct idGSWinVar
{
	idGSWinVar()
	{
		var = NULL;
		own = false;
	}
	idWinVar* var;
	bool own;
};

class budGuiScriptList;

class budGuiScript
{
	friend class budGuiScriptList;
	friend class idWindow;
	
public:
	budGuiScript();
	~budGuiScript();
	
	bool Parse( budTokenParser* src );
	void Execute( idWindow* win )
	{
		if( handler )
		{
			handler( win, &parms );
		}
	}
	void FixupParms( idWindow* win );
	size_t Size()
	{
		int sz = sizeof( *this );
		for( int i = 0; i < parms.Num(); i++ )
		{
			sz += parms[i].var->Size();
		}
		return sz;
	}
	
	void WriteToSaveGame( budFile* savefile );
	void ReadFromSaveGame( budFile* savefile );
	
protected:
	int conditionReg;
	budGuiScriptList* ifList;
	budGuiScriptList* elseList;
	List<idGSWinVar, TAG_OLD_UI> parms;
	void ( *handler )( idWindow* window, List<idGSWinVar, TAG_OLD_UI>* src );
	
};


class budGuiScriptList
{
	List<budGuiScript*, TAG_OLD_UI> list;
public:
	budGuiScriptList()
	{
		list.SetGranularity( 4 );
	};
	~budGuiScriptList()
	{
		list.DeleteContents( true );
	};
	void Execute( idWindow* win );
	void Append( budGuiScript* gs )
	{
		list.Append( gs );
	}
	size_t Size()
	{
		int sz = sizeof( *this );
		for( int i = 0; i < list.Num(); i++ )
		{
			sz += list[i]->Size();
		}
		return sz;
	}
	void FixupParms( idWindow* win );
	void ReadFromDemoFile( class budDemoFile* f ) {};
	void WriteToDemoFile( class budDemoFile* f ) {};
	
	void WriteToSaveGame( budFile* savefile );
	void ReadFromSaveGame( budFile* savefile );
};

#endif // __GUISCRIPT_H
