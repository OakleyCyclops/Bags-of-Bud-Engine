/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2012-2016 Robert Beckebans
Copyright (C) 2014-2016 Kot in Action Creative Artel

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

New for tech4x:

Unlike previous SMP work, the actual GPU command drawing is done in the main thread, which avoids the
OpenGL problems with needing windows to be created by the same thread that creates the context, as well
as the issues with passing context ownership back and forth on the 360.

The game tic and the generation of the draw command list is now run in a separate thread, and overlapped
with the interpretation of the previous draw command list.

While the game tic should be nicely contained, the draw command generation winds through the user interface
code, and is potentially hazardous.  For now, the overlap will be restricted to the renderer back end,
which should also be nicely contained.

*/
#define DEFAULT_FIXED_TIC "0"
#define DEFAULT_NO_SLEEP "0"

budCVar com_deltaTimeClamp( "com_deltaTimeClamp", "50", CVAR_INTEGER, "don't process more than this time in a single frame" );

budCVar com_fixedTic( "com_fixedTic", DEFAULT_FIXED_TIC, CVAR_BOOL, "run a single game frame per render frame" );
budCVar com_noSleep( "com_noSleep", DEFAULT_NO_SLEEP, CVAR_BOOL, "don't sleep if the game is running too fast" );
budCVar com_smp( "com_smp", "1", CVAR_INTEGER | CVAR_SYSTEM | CVAR_NOCHEAT, "run the game and draw code in a separate thread" );
budCVar com_aviDemoSamples( "com_aviDemoSamples", "16", CVAR_SYSTEM, "" );
budCVar com_aviDemoWidth( "com_aviDemoWidth", "256", CVAR_SYSTEM, "" );
budCVar com_aviDemoHeight( "com_aviDemoHeight", "256", CVAR_SYSTEM, "" );
budCVar com_skipGameDraw( "com_skipGameDraw", "0", CVAR_SYSTEM | CVAR_BOOL, "" );

budCVar com_sleepGame( "com_sleepGame", "0", CVAR_SYSTEM | CVAR_INTEGER, "intentionally add a sleep in the game time" );
budCVar com_sleepDraw( "com_sleepDraw", "0", CVAR_SYSTEM | CVAR_INTEGER, "intentionally add a sleep in the draw time" );
budCVar com_sleepRender( "com_sleepRender", "0", CVAR_SYSTEM | CVAR_INTEGER, "intentionally add a sleep in the render time" );

budCVar net_drawDebugHud( "net_drawDebugHud", "0", CVAR_SYSTEM | CVAR_INTEGER, "0 = None, 1 = Hud 1, 2 = Hud 2, 3 = Snapshots" );

budCVar timescale( "timescale", "1", CVAR_SYSTEM | CVAR_FLOAT, "Number of game frames to run per render frame", 0.001f, 100.0f );

extern budCVar in_useJoystick;
extern budCVar in_joystickRumble;

/*
===============
budGameThread::Run

Run in a background thread for performance, but can also
be called directly in the foreground thread for comparison.
===============
*/
int budGameThread::Run()
{
	commonLocal.frameTiming.startGameTime = Sys_Microseconds();
	
	// debugging tool to test frame dropping behavior
	if( com_sleepGame.GetInteger() )
	{
		Sys_Sleep( com_sleepGame.GetInteger() );
	}
	
	if( numGameFrames == 0 )
	{
		// Ensure there's no stale gameReturn data from a paused game
	}
	
	if( isClient )
	{
		// run the game logic
		for( int i = 0; i < numGameFrames; i++ )
		{
			SCOPED_PROFILE_EVENT( "Client Prediction" );
		}
	}
	else
	{
		// run the game logic
		for( int i = 0; i < numGameFrames; i++ )
		{
			SCOPED_PROFILE_EVENT( "GameTic" );
		}
	}
	
	commonLocal.frameTiming.finishGameTime = Sys_Microseconds();
	
	SetThreadGameTime( ( commonLocal.frameTiming.finishGameTime - commonLocal.frameTiming.startGameTime ) / 1000 );
	
	// build render commands and geometry
	{
		SCOPED_PROFILE_EVENT( "Draw" );
	}
	
	commonLocal.frameTiming.finishDrawTime = Sys_Microseconds();
	
	SetThreadRenderTime( ( commonLocal.frameTiming.finishDrawTime - commonLocal.frameTiming.finishGameTime ) / 1000 );
	
	SetThreadTotalTime( ( commonLocal.frameTiming.finishDrawTime - commonLocal.frameTiming.startGameTime ) / 1000 );
	
	return 0;
}

extern budCVar com_forceGenericSIMD;

extern budCVar com_pause;

/*
=================
budCommonLocal::Frame
=================
*/
void budCommonLocal::Frame()
{
	try
	{
		SCOPED_PROFILE_EVENT( "Common::Frame" );
		
		// This is the only place this is incremented
		libBud::frameNumber++;
		
		// allow changing SIMD usage on the fly
		if( com_forceGenericSIMD.IsModified() )
		{
			idSIMD::InitProcessor( "doom", com_forceGenericSIMD.GetBool() );
			com_forceGenericSIMD.ClearModified();
		}
		
		// pump all the events
		Sys_GenerateEvents();
		
		// write config file if anything changed
		WriteConfiguration();
		
		eventLoop->RunEventLoop();
		
		// if the console or another gui is down, we don't need to hold the mouse cursor
		bool chatting = false;
		
		// DG: Add pause from com_pause cvar
		// RB begin
		if( com_pause.GetInteger() || console->Active() || session->IsSystemUIShowing() )
			// RB end, DG end
		{
			// RB: don't release the mouse when opening a PDA or menu
			if( console->Active() )
			{
				Sys_GrabMouseCursor( false );
			}
			usercmdGen->InhibitUsercmd( INHIBIT_SESSION, true );
			chatting = true;
		}
		else
		{
			Sys_GrabMouseCursor( true );
			usercmdGen->InhibitUsercmd( INHIBIT_SESSION, false );
		}

		const bool pauseGame = ( !mapSpawned || ( !IsMultiplayer() && ( session->IsSystemUIShowing() || com_pause.GetInteger() ) ) );
		// RB end
		
		// save the screenshot and audio from the last draw if needed
		if( aviCaptureMode )
		{
			budStr name;
			name.Format( "demos/%s/%s_%05i", aviDemoShortName.c_str(), aviDemoShortName.c_str(), aviDemoFrameCount++ );
			
			// remove any printed lines at the top before taking the screenshot
			console->ClearNotifyLines();
		}
		
		//--------------------------------------------
		// Determine how many game tics we are going to run,
		// now that the previous frame is completely finished.
		//
		// It is important that any waiting on the GPU be done
		// before this, or there will be a bad stuttering when
		// dropping frames for performance management.
		//--------------------------------------------
		
		// input:
		// thisFrameTime
		// com_noSleep
		// com_engineHz
		// com_fixedTic
		// com_deltaTimeClamp
		// IsMultiplayer
		//
		// in/out state:
		// gameFrame
		// gameTimeResidual
		// lastFrameTime
		// syncNextFrame
		//
		// Output:
		// numGameFrames
		
		// How many game frames to run
		int numGameFrames = 0;
		
		for( ;; )
		{
			const int thisFrameTime = Sys_Milliseconds();
			static int lastFrameTime = thisFrameTime;	// initialized only the first time
			const int deltaMilliseconds = thisFrameTime - lastFrameTime;
			lastFrameTime = thisFrameTime;
			
			// if there was a large gap in time since the last frame, or the frame
			// rate is very very low, limit the number of frames we will run
			const int clampedDeltaMilliseconds = Min( deltaMilliseconds, com_deltaTimeClamp.GetInteger() );
			
			gameTimeResidual += clampedDeltaMilliseconds * timescale.GetFloat();
			
			// don't run any frames when paused
			// jpcy: the game is paused when playing a demo, but playDemo should wait like the game does
			if( pauseGame && !( readDemo && !timeDemo ) )
			{
				gameFrame++;
				gameTimeResidual = 0;
				break;
			}
			
			// debug cvar to force multiple game tics
			if( com_fixedTic.GetInteger() > 0 )
			{
				numGameFrames = com_fixedTic.GetInteger();
				gameFrame += numGameFrames;
				gameTimeResidual = 0;
				break;
			}
			
			if( syncNextGameFrame )
			{
				// don't sleep at all
				syncNextGameFrame = false;
				gameFrame++;
				numGameFrames++;
				gameTimeResidual = 0;
				break;
			}
			
			for( ;; )
			{
				// How much time to wait before running the next frame,
				// based on com_engineHz
				const int frameDelay = FRAME_TO_MSEC( gameFrame + 1 ) - FRAME_TO_MSEC( gameFrame );
				if( gameTimeResidual < frameDelay )
				{
					break;
				}
				gameTimeResidual -= frameDelay;
				gameFrame++;
				numGameFrames++;
				// if there is enough residual left, we may run additional frames
			}
			
			if( numGameFrames > 0 )
			{
				// ready to actually run them
				break;
			}
			
			// if we are vsyncing, we always want to run at least one game
			// frame and never sleep, which might happen due to scheduling issues
			// if we were just looking at real time.
			if( com_noSleep.GetBool() )
			{
				numGameFrames = 1;
				gameFrame += numGameFrames;
				gameTimeResidual = 0;
				break;
			}
			
			// not enough time has passed to run a frame, as might happen if
			// we don't have vsync on, or the monitor is running at 120hz while
			// com_engineHz is 60, so sleep a bit and check again
			Sys_Sleep( 0 );
		}
		
		// jpcy: playDemo uses the game frame wait logic, but shouldn't run any game frames
		if( readDemo && !timeDemo )
			numGameFrames = 0;
			
		//--------------------------------------------
		// It would be better to push as much of this as possible
		// either before or after the renderSystem->SwapCommandBuffers(),
		// because the GPU is completely idle.
		//--------------------------------------------
		
		// Update session and syncronize to the new session state after sleeping
		session->UpdateSignInManager();
		session->Pump();
		session->ProcessSnapAckQueue();
		
		if( session->GetState() == budSession::LOADING )
		{
			// If the session reports we should be loading a map, load it!
			// ExecuteMapChange();
			mapSpawnData.savegameFile = NULL;
			mapSpawnData.persistentPlayerInfo.Clear();
			return;
		}
		else if( session->GetState() != budSession::INGAME && mapSpawned )
		{
			// If the game is running, but the session reports we are not in a game, disconnect
			// This happens when a server disconnects us or we sign out
			LeaveGame();
			return;
		}
		
		if( mapSpawned && !pauseGame )
		{
			if( IsClient() )
			{
				RunNetworkSnapshotFrame();
			}
		}
		
		ExecuteReliableMessages();
		
		// send frame and mouse events to active guis
		GuiFrameEvents();
		
		//--------------------------------------------
		// Prepare usercmds and kick off the game processing
		// in a background thread
		//--------------------------------------------
		
		// get the previous usercmd for bypassed head tracking transform
		const usercmd_t	previousCmd = usercmdGen->GetCurrentUsercmd();
		
		// build a new usercmd
		int deviceNum = session->GetSignInManager().GetMasterInputDevice();
		usercmdGen->BuildCurrentUsercmd( deviceNum );
		if( deviceNum == -1 )
		{
			for( int i = 0; i < MAX_INPUT_DEVICES; i++ )
			{
				Sys_PollJoystickInputEvents( i );
				Sys_EndJoystickInputEvents();
			}
		}
		if( pauseGame )
		{
			usercmdGen->Clear();
		}
		
		usercmd_t newCmd = usercmdGen->GetCurrentUsercmd();
		
		//----------------------------------------
		// Run the render back end, getting the GPU busy with new commands
		// ASAP to minimize the pipeline bubble.
		//----------------------------------------
		frameTiming.startRenderTime = Sys_Microseconds();
		
		frameTiming.finishRenderTime = Sys_Microseconds();
		
		// make sure the game / draw thread has completed
		// This may block if the game is taking longer than the render back end
		gameThread.WaitForThread();
		
		// Now that we have an updated game frame, we can send out new snapshots to our clients
		session->Pump(); // Pump to get updated usercmds to relay
		SendSnapshots();
		
		idLobbyBase& lobby = session->GetActivePlatformLobbyBase();
		if( lobby.HasActivePeers() )
		{
			if( net_drawDebugHud.GetInteger() == 1 )
			{
				lobby.DrawDebugNetworkHUD();
			}
			if( net_drawDebugHud.GetInteger() == 2 )
			{
				lobby.DrawDebugNetworkHUD2();
			}
			lobby.DrawDebugNetworkHUD_ServerSnapshotMetrics( net_drawDebugHud.GetInteger() == 3 );
		}
		
		// report timing information
		if( com_speeds.GetBool() )
		{
			static int lastTime = Sys_Milliseconds();
			int	nowTime = Sys_Milliseconds();
			int	com_frameMsec = nowTime - lastTime;
			lastTime = nowTime;
			Printf( "frame:%d all:%3d gfr:%3d rf:%3lld bk:%3lld\n", libBud::frameNumber, com_frameMsec, time_gameFrame, time_frontend / 1000, time_backend / 1000 );
			time_gameFrame = 0;
			time_gameDraw = 0;
		}
		
		// the FPU stack better be empty at this point or some bad code or compiler bug left values on the stack
		if( !Sys_FPU_StackIsEmpty() )
		{
			Printf( "%s", Sys_FPU_GetState() );
			FatalError( "budCommon::Frame: the FPU stack is not empty at the end of the frame\n" );
		}
		
		mainFrameTiming = frameTiming;
		
		session->GetSaveGameManager().Pump();
	}
	catch( idException& )
	{
		
		// drop back to main menu
		LeaveGame();
		
		// force the console open to show error messages
		console->Open();
		return;
	}
}

