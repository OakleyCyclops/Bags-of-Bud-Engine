/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2014-2016 Robert Beckebans
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

#include "Common_local.hpp"

/*
================
FindUnusedFileName
================
*/
static budStr FindUnusedFileName( const char* format )
{
	budStr filename;
	
	for( int i = 0 ; i < 999 ; i++ )
	{
		filename.Format( format, i );
		int len = fileSystem->ReadFile( filename, NULL, NULL );
		if( len <= 0 )
		{
			return filename;	// file doesn't exist
		}
	}
	
	return filename;
}

extern budCVar com_smp;

// void WriteDeclCache( budDemoFile* f, int demoCategory, int demoCode, declType_t  declType )
// {
// 	f->WriteInt( demoCategory );
// 	f->WriteInt( demoCode );
	
// 	int numDecls = 0;
	
// 	for( int i = 0; i < declManager->GetNumDecls( declType ); i++ )
// 	{
// 		const budDecl* decl = declManager->DeclByIndex( declType, i, false );
// 		if( decl && decl->IsValid() )
// 			++numDecls;
// 	}
	
// 	f->WriteInt( numDecls );
// 	for( int i = 0; i < declManager->GetNumDecls( declType ); i++ )
// 	{
// 		const budDecl* decl = declManager->DeclByIndex( declType, i, false );
// 		if( decl && decl->IsValid() )
// 			f->WriteHashString( decl->GetName() );
// 	}
// }

/*
================
budCommonLocal::StartRecordingRenderDemo
================
*/
void budCommonLocal::StartRecordingRenderDemo( const char* demoName )
{
	if( writeDemo )
	{
		// allow it to act like a toggle
		StopRecordingRenderDemo();
		return;
	}
	
	if( !demoName[0] )
	{
		common->Printf( "budCommonLocal::StartRecordingRenderDemo: no name specified\n" );
		return;
	}
	
	console->Close();
	
	com_smp.SetInteger( 0 );
	
	writeDemo = new( TAG_SYSTEM ) budDemoFile;
	if( !writeDemo->OpenForWriting( demoName ) )
	{
		common->Printf( "error opening %s\n", demoName );
		delete writeDemo;
		writeDemo = NULL;
		return;
	}
	
	common->Printf( "recording to %s\n", writeDemo->GetName() );
	
	writeDemo->WriteInt( DS_VERSION );
	writeDemo->WriteInt( RENDERDEMO_VERSION );
	
	// if we are in a map already, dump the current state
}

/*
================
budCommonLocal::StopRecordingRenderDemo
================
*/
void budCommonLocal::StopRecordingRenderDemo()
{
	if( !writeDemo )
	{
		common->Printf( "budCommonLocal::StopRecordingRenderDemo: not recording\n" );
		return;
	}
	
	writeDemo->Close();
	common->Printf( "stopped recording %s.\n", writeDemo->GetName() );
	delete writeDemo;
	writeDemo = NULL;
	com_smp.SetInteger( 1 ); // motorsep 12-30-2014; turn multithreading back on
}

/*
================
budCommonLocal::StopPlayingRenderDemo

Reports timeDemo numbers and finishes any avi recording
================
*/
void budCommonLocal::StopPlayingRenderDemo()
{
	if( !readDemo )
	{
		timeDemo = TD_NO;
		return;
	}
	
	// Record the stop time before doing anything that could be time consuming
	int timeDemoStopTime = Sys_Milliseconds();
	
	EndAVICapture();
	
	readDemo->Close();
	
	// soundWorld->StopAllSounds();
	// soundSystem->SetPlayingSoundWorld( menuSoundWorld );
	
	common->Printf( "stopped playing %s.\n", readDemo->GetName() );
	delete readDemo;
	readDemo = NULL;
	
	if( timeDemo )
	{
		// report the stats
		float	demoSeconds = ( timeDemoStopTime - timeDemoStartTime ) * 0.001f;
		float	demoFPS = numDemoFrames / demoSeconds;
		budStr	message = va( "%i frames rendered in %3.1f seconds = %3.1f fps\n", numDemoFrames, demoSeconds, demoFPS );
		
		common->Printf( message );
		if( timeDemo == TD_YES_THEN_QUIT )
		{
			cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "quit\n" );
		}
		timeDemo = TD_NO;
	}
	
	com_smp.SetInteger( 1 ); // motorsep 12-30-2014; turn multithreading back on
}

/*
================
budCommonLocal::DemoShot

A demoShot is a single frame demo
================
*/
void budCommonLocal::DemoShot( const char* demoName )
{
	StartRecordingRenderDemo( demoName );
	
	// force draw one frame
	const bool captureToImage = false;
	
	StopRecordingRenderDemo();
}

/*
================
budCommonLocal::StartPlayingRenderDemo
================
*/
void budCommonLocal::StartPlayingRenderDemo( budStr demoName )
{
	if( !demoName[0] )
	{
		common->Printf( "budCommonLocal::StartPlayingRenderDemo: no name specified\n" );
		return;
	}
	
	com_smp.SetInteger( 0 );
	
	// exit any current game
	Stop();
	
	// automatically put the console away
	console->Close();
	
	readDemo = new( TAG_SYSTEM ) budDemoFile;
	demoName.DefaultFileExtension( ".demo" );
	if( !readDemo->OpenForReading( demoName ) )
	{
		common->Printf( "couldn't open %s\n", demoName.c_str() );
		delete readDemo;
		readDemo = NULL;
		Stop();
		StartMenu();
		return;
	}
	
	int opcode = -1, demoVersion = -1;
	readDemo->ReadInt( opcode );
	if( opcode != DS_VERSION )
	{
		common->Printf( "StartPlayingRenderDemo invalid demo file\n" );
		
		Stop();
		StartMenu();
		return;
	}
	
	readDemo->ReadInt( demoVersion );
	if( demoVersion != RENDERDEMO_VERSION )
	{
		common->Printf( "StartPlayingRenderDemo got version %d, expected version %d\n", demoVersion, RENDERDEMO_VERSION );
		
		Stop();
		StartMenu();
		return;
	}
	
	AdvanceRenderDemo( true );
	
	const bool captureToImage = false;
	
	numDemoFrames = 1;
	
	timeDemoStartTime = Sys_Milliseconds();
}

/*
================
budCommonLocal::TimeRenderDemo
================
*/
void budCommonLocal::TimeRenderDemo( const char* demoName, bool twice, bool quit )
{
	budStr demo = demoName;
	
	StartPlayingRenderDemo( demo );
	
	if( twice && readDemo )
	{
		while( readDemo )
		{
			const bool captureToImage = false;
			AdvanceRenderDemo( true );
		}
		
		StartPlayingRenderDemo( demo );
	}
	
	
	if( !readDemo )
	{
		return;
	}
	
	if( quit )
	{
		// this allows hardware vendors to automate some testing
		timeDemo = TD_YES_THEN_QUIT;
	}
	else
	{
		timeDemo = TD_YES;
	}
}


/*
================
budCommonLocal::BeginAVICapture
================
*/
void budCommonLocal::BeginAVICapture( const char* demoName )
{
	budStr name = demoName;
	name.ExtractFileBase( aviDemoShortName );
	aviCaptureMode = true;
	aviDemoFrameCount = 0;
}

/*
================
budCommonLocal::EndAVICapture
================
*/
void budCommonLocal::EndAVICapture()
{
	if( !aviCaptureMode )
	{
		return;
	}
	
	// write a .roqParam file so the demo can be converted to a roq file
	budFile* f = fileSystem->OpenFileWrite( va( "demos/%s/%s.roqParam",
										   aviDemoShortName.c_str(), aviDemoShortName.c_str() ) );
	f->Printf( "INPUT_DIR demos/%s\n", aviDemoShortName.c_str() );
	f->Printf( "FILENAME demos/%s/%s.RoQ\n", aviDemoShortName.c_str(), aviDemoShortName.c_str() );
	f->Printf( "\nINPUT\n" );
	f->Printf( "%s_*.tga [00000-%05i]\n", aviDemoShortName.c_str(), ( int )( aviDemoFrameCount - 1 ) );
	f->Printf( "END_INPUT\n" );
	delete f;
	
	common->Printf( "captured %i frames for %s.\n", ( int )aviDemoFrameCount, aviDemoShortName.c_str() );
	
	aviCaptureMode = false;
}


/*
================
budCommonLocal::AVIRenderDemo
================
*/
void budCommonLocal::AVIRenderDemo( const char* _demoName )
{
	budStr	demoName = _demoName;	// copy off from va() buffer
	
	StartPlayingRenderDemo( demoName );
	if( !readDemo )
	{
		return;
	}
	
	BeginAVICapture( demoName.c_str() ) ;
	
	// I don't understand why I need to do this twice, something
	// strange with the nvidia swapbuffers?
	const bool captureToImage = false;
}

/*
================
budCommonLocal::AVIGame

Start AVI recording the current game session
================
*/
void budCommonLocal::AVIGame( const char* demoName )
{
	if( aviCaptureMode )
	{
		EndAVICapture();
		return;
	}
	
	if( !mapSpawned )
	{
		common->Printf( "No map spawned.\n" );
	}
	
	if( !demoName || !demoName[0] )
	{
		budStr filename = FindUnusedFileName( "demos/game%03i.game" );
		demoName = filename.c_str();
		
		// write a one byte stub .game file just so the FindUnusedFileName works,
		fileSystem->WriteFile( demoName, demoName, 1 );
	}
	
	BeginAVICapture( demoName ) ;
}

/*
================
budCommonLocal::CompressDemoFile
================
*/
void budCommonLocal::CompressDemoFile( const char* scheme, const char* demoName )
{
	budStr	fullDemoName = "demos/";
	fullDemoName += demoName;
	fullDemoName.DefaultFileExtension( ".demo" );
	budStr compressedName = fullDemoName;
	compressedName.StripFileExtension();
	compressedName.Append( "_compressed.demo" );
	
	int savedCompression = cvarSystem->GetCVarInteger( "com_compressDemos" );
	bool savedPreload = cvarSystem->GetCVarBool( "com_preloadDemos" );
	cvarSystem->SetCVarBool( "com_preloadDemos", false );
	cvarSystem->SetCVarInteger( "com_compressDemos", atoi( scheme ) );
	
	budDemoFile demoread, demowrite;
	if( !demoread.OpenForReading( fullDemoName ) )
	{
		common->Printf( "Could not open %s for reading\n", fullDemoName.c_str() );
		return;
	}
	if( !demowrite.OpenForWriting( compressedName ) )
	{
		common->Printf( "Could not open %s for writing\n", compressedName.c_str() );
		demoread.Close();
		cvarSystem->SetCVarBool( "com_preloadDemos", savedPreload );
		cvarSystem->SetCVarInteger( "com_compressDemos", savedCompression );
		return;
	}
	common->SetRefreshOnPrint( true );
	common->Printf( "Compressing %s to %s...\n", fullDemoName.c_str(), compressedName.c_str() );
	
	static const int bufferSize = 65535;
	char buffer[bufferSize];
	int bytesRead;
	while( 0 != ( bytesRead = demoread.Read( buffer, bufferSize ) ) )
	{
		demowrite.Write( buffer, bytesRead );
		common->Printf( "." );
	}
	
	demoread.Close();
	demowrite.Close();
	
	cvarSystem->SetCVarBool( "com_preloadDemos", savedPreload );
	cvarSystem->SetCVarInteger( "com_compressDemos", savedCompression );
	
	common->Printf( "Done\n" );
	common->SetRefreshOnPrint( false );
	
}

/*
===============
budCommonLocal::AdvanceRenderDemo
===============
*/
void budCommonLocal::AdvanceRenderDemo( bool singleFrameOnly )
{
	while( true )
	{
		int	ds = DS_FINISHED;
		readDemo->ReadInt( ds );
	}
}

/*
================
Common_DemoShot_f
================
*/
CONSOLE_COMMAND( demoShot, "writes a screenshot as a demo", NULL )
{
	if( args.Argc() != 2 )
	{
		budStr filename = FindUnusedFileName( "demos/shot%03i.demo" );
		commonLocal.DemoShot( filename );
	}
	else
	{
		commonLocal.DemoShot( va( "demos/shot_%s.demo", args.Argv( 1 ) ) );
	}
}

/*
================
Common_RecordDemo_f
================
*/
CONSOLE_COMMAND( recordDemo, "records a demo", NULL )
{
	if( args.Argc() != 2 )
	{
		budStr filename = FindUnusedFileName( "demos/demo%03i.demo" );
		commonLocal.StartRecordingRenderDemo( filename );
	}
	else
	{
		commonLocal.StartRecordingRenderDemo( va( "demos/%s.demo", args.Argv( 1 ) ) );
	}
}

/*
================
Common_CompressDemo_f
================
*/
CONSOLE_COMMAND( compressDemo, "compresses a demo file", budCmdSystem::ArgCompletion_DemoName )
{
	if( args.Argc() == 2 )
	{
		commonLocal.CompressDemoFile( "2", args.Argv( 1 ) );
	}
	else if( args.Argc() == 3 )
	{
		commonLocal.CompressDemoFile( args.Argv( 2 ), args.Argv( 1 ) );
	}
	else
	{
		common->Printf( "use: CompressDemo <file> [scheme]\nscheme is the same as com_compressDemo, defaults to 2" );
	}
}

/*
================
Common_StopRecordingDemo_f
================
*/
CONSOLE_COMMAND( stopRecording, "stops demo recording", NULL )
{
	commonLocal.StopRecordingRenderDemo();
}

/*
================
Common_PlayDemo_f
================
*/
CONSOLE_COMMAND( playDemo, "plays back a demo", budCmdSystem::ArgCompletion_DemoName )
{
	if( args.Argc() >= 2 )
	{
		commonLocal.StartPlayingRenderDemo( va( "demos/%s", args.Argv( 1 ) ) );
	}
}

/*
================
Common_TimeDemo_f
================
*/
CONSOLE_COMMAND( timeDemo, "times a demo", budCmdSystem::ArgCompletion_DemoName )
{
	if( args.Argc() >= 2 )
	{
		commonLocal.TimeRenderDemo( va( "demos/%s", args.Argv( 1 ) ), ( args.Argc() > 2 ), false );
	}
}

/*
================
Common_TimeDemoQuit_f
================
*/
CONSOLE_COMMAND( timeDemoQuit, "times a demo and quits", budCmdSystem::ArgCompletion_DemoName )
{
	commonLocal.TimeRenderDemo( va( "demos/%s", args.Argv( 1 ) ), true );
}

/*
================
Common_AVIDemo_f
================
*/
CONSOLE_COMMAND( aviDemo, "writes AVIs for a demo", budCmdSystem::ArgCompletion_DemoName )
{
	commonLocal.AVIRenderDemo( va( "demos/%s", args.Argv( 1 ) ) );
}

/*
================
Common_AVIGame_f
================
*/
CONSOLE_COMMAND( aviGame, "writes AVIs for the current game", NULL )
{
	commonLocal.AVIGame( args.Argv( 1 ) );
}
