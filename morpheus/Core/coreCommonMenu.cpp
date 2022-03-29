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

#include "corePCH.hpp"
#pragma hdrstop

#include "coreCommonLocal.hpp"

/*
==============
budCommonLocal::OnStartHosting
==============
*/
void budCommonLocal::OnStartHosting( budMatchParameters& parms )
{
	if( ( parms.matchFlags & MATCH_REQUIRE_PARTY_LOBBY ) == 0 )
	{
		return; // This is the party lobby or a SP match
	}
	
	// If we were searching for a random match but didn't find one, we'll need to select parameters now
	if( parms.gameMap < 0 )
	{
		if( parms.gameMode < 0 )
		{
			// Random mode means any map will do
			parms.gameMap = libBud::frameNumber % mpGameMaps.Num();
		}
		else
		{
			// Select a map which supports the chosen mode
			budList<int> supportedMaps;
			uint32 supportedMode = BIT( parms.gameMode );
			for( int i = 0; i < mpGameMaps.Num(); i++ )
			{
				if( mpGameMaps[i].supportedModes & supportedMode )
				{
					supportedMaps.Append( i );
				}
			}
			if( supportedMaps.Num() == 0 )
			{
				// We don't have any maps which support the chosen mode...
				parms.gameMap = libBud::frameNumber % mpGameMaps.Num();
				parms.gameMode = -1;
			}
			else
			{
				parms.gameMap = supportedMaps[ libBud::frameNumber % supportedMaps.Num() ];
			}
		}
	}
	if( parms.gameMode < 0 )
	{
		uint32 supportedModes = mpGameMaps[parms.gameMap].supportedModes;
		int8 supportedModeList[32] = {};
		int numSupportedModes = 0;
		for( int i = 0; i < 32; i++ )
		{
			if( supportedModes & BIT( i ) )
			{
				supportedModeList[numSupportedModes] = i;
				numSupportedModes++;
			}
		}
		parms.gameMode = supportedModeList[( libBud::frameNumber / mpGameMaps.Num() ) % numSupportedModes ];
	}
	parms.mapName = mpGameMaps[parms.gameMap].mapFile;
	parms.numSlots = session->GetTitleStorageInt( "MAX_PLAYERS_ALLOWED", 4 );
}

/*
==============
budCommonLocal::StartMainMenu
==============
*/
void budCommonLocal::StartMenu( bool playIntro )
{
	
	if( readDemo )
	{
		// if we're playing a demo, esc kills it
		UnloadMap();
	}
	
	console->Close();
	
}

/*
===============
budCommonLocal::ExitMenu
===============
*/
void budCommonLocal::ExitMenu()
{
}

/*
==============
budCommonLocal::MenuEvent

Executes any commands returned by the gui
==============
*/
bool budCommonLocal::MenuEvent( const sysEvent_t* event )
{

	if( session->GetSignInManager().ProcessInputEvent( event ) )
	{
		return true;
	}
	
	return false;
}

/*
=================
budCommonLocal::GuiFrameEvents
=================
*/
void budCommonLocal::GuiFrameEvents()
{

}
