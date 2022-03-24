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

#ifndef __REGEXP_H__
#define __REGEXP_H__

class budTokenParser;
class idWindow;
class idWinVar;

class idRegister
{
public:
	idRegister();
	idRegister( const char* p, int t );
	
	enum REGTYPE { VEC4 = 0, FLOAT, BOOL, INT, STRING, VEC2, VEC3, RECTANGLE, NUMTYPES } ;
	static int REGCOUNT[NUMTYPES];
	
	bool				enabled;
	short				type;
	budStr				name;
	int					regCount;
	unsigned short		regs[4];
	idWinVar* 			var;
	
	void				SetToRegs( float* registers );
	void				GetFromRegs( float* registers );
	void				CopyRegs( idRegister* src );
	void				Enable( bool b )
	{
		enabled = b;
	}
	void				ReadFromDemoFile( budDemoFile* f );
	void				WriteToDemoFile( budDemoFile* f );
	void				WriteToSaveGame( budFile* savefile );
	void				ReadFromSaveGame( budFile* savefile );
};

BUD_INLINE idRegister::idRegister()
{
}

BUD_INLINE idRegister::idRegister( const char* p, int t )
{
	name = p;
	type = t;
	assert( t >= 0 && t < NUMTYPES );
	regCount = REGCOUNT[t];
	enabled = ( type == STRING ) ? false : true;
	var = NULL;
};

BUD_INLINE void idRegister::CopyRegs( idRegister* src )
{
	regs[0] = src->regs[0];
	regs[1] = src->regs[1];
	regs[2] = src->regs[2];
	regs[3] = src->regs[3];
}

class idRegisterList
{
public:

	idRegisterList();
	~idRegisterList();
	
	void				AddReg( const char* name, int type, budTokenParser* src, idWindow* win, idWinVar* var );
	void				AddReg( const char* name, int type, budVec4 data, idWindow* win, idWinVar* var );
	
	idRegister* 		FindReg( const char* name );
	void				SetToRegs( float* registers );
	void				GetFromRegs( float* registers );
	void				Reset();
	void				ReadFromDemoFile( budDemoFile* f );
	void				WriteToDemoFile( budDemoFile* f );
	void				WriteToSaveGame( budFile* savefile );
	void				ReadFromSaveGame( budFile* savefile );
	
private:
	budList<idRegister*>	regs;
	budHashIndex			regHash;
};

BUD_INLINE idRegisterList::idRegisterList()
{
	regs.SetGranularity( 4 );
	regHash.SetGranularity( 4 );
	regHash.Clear( 32, 4 );
}

BUD_INLINE idRegisterList::~idRegisterList()
{
}

#endif /* !__REGEXP_H__ */
