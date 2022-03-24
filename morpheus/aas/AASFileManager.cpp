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


#include "AASFile.h"
#include "AASFile_local.h"

/*
===============================================================================

	AAS File Manager

===============================================================================
*/

class budAASFileManagerLocal : public budAASFileManager
{
public:
	virtual						~budAASFileManagerLocal() {}
	
	virtual budAASFile* 			LoadAAS( const char* fileName, unsigned int mapFileCRC );
	virtual void				FreeAAS( budAASFile* file );
};

budAASFileManagerLocal			AASFileManagerLocal;
budAASFileManager* 				AASFileManager = &AASFileManagerLocal;


/*
================
budAASFileManagerLocal::LoadAAS
================
*/
budAASFile* budAASFileManagerLocal::LoadAAS( const char* fileName, unsigned int mapFileCRC )
{
	budAASFileLocal* file = new( TAG_AAS ) budAASFileLocal();
	if( !file->Load( fileName, mapFileCRC ) )
	{
		delete file;
		return NULL;
	}
	return file;
}

/*
================
budAASFileManagerLocal::FreeAAS
================
*/
void budAASFileManagerLocal::FreeAAS( budAASFile* file )
{
	delete file;
}
