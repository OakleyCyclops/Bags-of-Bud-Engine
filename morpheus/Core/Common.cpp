/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2012-2014 Robert Beckebans

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
#include "ConsoleHistory.hpp"
#include "sys_savegame.hpp"



#if defined( _DEBUG )
#define BUILD_DEBUG "-debug"
#else
#define BUILD_DEBUG ""
#endif

struct version_s
{
	version_s()
	{
		sprintf( string, "%s.%d%s %s %s %s", ENGINE_VERSION, BUILD_NUMBER, BUILD_DEBUG, BUILD_STRING, __DATE__, __TIME__ );
	}
	char	string[256];
} version;

budCVar com_version( "si_version", version.string, CVAR_SYSTEM | CVAR_ROM | CVAR_SERVERINFO, "engine version" );
budCVar com_forceGenericSIMD( "com_forceGenericSIMD", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT, "force generic platform independent SIMD" );

#ifdef ID_RETAIL
budCVar com_allowConsole( "com_allowConsole", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_INIT, "allow toggling console with the tilde key" );
#else
budCVar com_allowConsole( "com_allowConsole", "1", CVAR_BOOL | CVAR_SYSTEM | CVAR_INIT, "allow toggling console with the tilde key" );
#endif

budCVar com_developer( "developer", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT, "developer mode" );
budCVar com_speeds( "com_speeds", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT, "show engine timings" );
// DG: support "com_showFPS 2" for fps-only view like in classic doom3 => make it CVAR_INTEGER
budCVar com_showFPS( "com_showFPS", "0", CVAR_INTEGER | CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_NOCHEAT, "show frames rendered per second. 0: off 1: default bfg values, 2: only show FPS (classic view)" );
// DG end
budCVar com_showMemoryUsage( "com_showMemoryUsage", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT, "show total and per frame memory usage" );
budCVar com_updateLoadSize( "com_updateLoadSize", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT, "update the load size after loading a map" );

budCVar com_productionMode( "com_productionMode", "0", CVAR_SYSTEM | CVAR_BOOL, "0 - no special behavior, 1 - building a production build, 2 - running a production build" );

budCVar com_japaneseCensorship( "com_japaneseCensorship", "0", CVAR_NOCHEAT, "Enable Japanese censorship" );

budCVar preload_CommonAssets( "preload_CommonAssets", "1", CVAR_SYSTEM | CVAR_BOOL, "preload common assets" );

budCVar net_inviteOnly( "net_inviteOnly", "1", CVAR_BOOL | CVAR_ARCHIVE, "whether or not the private server you create allows friends to join or invite only" );

// DG: add cvar for pause
budCVar com_pause( "com_pause", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT, "set to 1 to pause game, to 0 to unpause again" );
// DG end

extern budCVar g_demoMode;

budCVar com_engineHz( "com_engineHz", "60", CVAR_FLOAT | CVAR_ARCHIVE, "Frames per second the engine runs at", 10.0f, 1024.0f );
float com_engineHz_latched = 60.0f; // Latched version of cvar, updated between map loads
int64 com_engineHz_numerator = 100LL * 1000LL;
int64 com_engineHz_denominator = 100LL * 60LL;

// RB begin
int com_editors = 0;

#if defined(_WIN32)
HWND com_hwndMsg = NULL;
#endif
// RB end

#ifdef __DOOM_DLL__
budGame* 		game = NULL;
budGameEdit* 	gameEdit = NULL;
#endif

budCommonLocal	commonLocal;
budCommon* 		common = &commonLocal;

// RB: defaulted this to 1 because we don't have a sound for the intro .bik video
budCVar com_skipIntroVideos( "com_skipIntroVideos", "1", CVAR_BOOL , "skips intro videos" );

// For doom classic
struct Globals;

/*
==================
budCommonLocal::budCommonLocal
==================
*/
budCommonLocal::budCommonLocal() :
	readSnapshotIndex( 0 ),
	writeSnapshotIndex( 0 ),
	optimalPCTBuffer( 0.5f ),
	optimalTimeBuffered( 0.0f ),
	optimalTimeBufferedWindow( 0.0f ),
	lastPacifierSessionTime( 0 ),
	lastPacifierGuiTime( 0 ),
	lastPacifierDialogState( false ),
	showShellRequested( false )
	// RB begin
#if defined(USE_DOOMCLASSIC)
	,
	currentGame( DOOM3_BFG ),
	idealCurrentGame( DOOM3_BFG ),
	doomClassicMaterial( NULL )
#endif
	// RB end
{

	snapCurrent.localTime = -1;
	snapPrevious.localTime = -1;
	snapCurrent.serverTime = -1;
	snapPrevious.serverTime = -1;
	snapTimeBuffered	= 0.0f;
	effectiveSnapRate	= 0.0f;
	totalBufferedTime	= 0;
	totalRecvTime		= 0;
	
	com_fullyInitialized = false;
	com_refreshOnPrint = false;
	com_errorEntered = ERP_NONE;
	com_shuttingDown = false;
	com_isJapaneseSKU = false;
	
	logFile = NULL;
	
	strcpy( errorMessage, "" );
	
	rd_buffer = NULL;
	rd_buffersize = 0;
	rd_flush = NULL;
	
	gameDLL = 0;
	
	nextLoadTip = 0;
	isHellMap = false;
	wipeForced = false;
	defaultLoadscreen = false;
	
	menuSoundWorld = NULL;
	
	insideExecuteMapChange = false;
	
	mapSpawnData.savegameFile = NULL;
	
	currentMapName.Clear();
	aviDemoShortName.Clear();
	
	renderWorld = NULL;
	soundWorld = NULL;
	menuSoundWorld = NULL;
	readDemo = NULL;
	writeDemo = NULL;
	
	gameFrame = 0;
	gameTimeResidual = 0;
	syncNextGameFrame = true;
	mapSpawned = false;
	aviCaptureMode = false;
	timeDemo = TD_NO;
	
	nextSnapshotSendTime = 0;
	nextUsercmdSendTime = 0;
	
	clientPrediction = 0;
	
	saveFile = NULL;
	stringsFile = NULL;
	
	ClearWipe();
}

/*
==================
budCommonLocal::Quit
==================
*/
void budCommonLocal::Quit()
{

	// don't try to shutdown if we are in a recursive error
	if( !com_errorEntered )
	{
		Shutdown();
	}
	Sys_Quit();
}


/*
============================================================================

COMMAND LINE FUNCTIONS

+ characters separate the commandLine string into multiple console
command lines.

All of these are valid:

doom +set test blah +map test
doom set test blah+map test
doom set test blah + map test

============================================================================
*/

#define		MAX_CONSOLE_LINES	32
int			com_numConsoleLines;
budCmdArgs	com_consoleLines[MAX_CONSOLE_LINES];

/*
==================
budCommonLocal::ParseCommandLine
==================
*/
void budCommonLocal::ParseCommandLine( int argc, const char* const* argv )
{
	int i, current_count;
	
	com_numConsoleLines = 0;
	current_count = 0;
	// API says no program path
	for( i = 0; i < argc; i++ )
	{
		if( budStr::Icmp( argv[ i ], "+connect_lobby" ) == 0 )
		{
			// Handle Steam bootable invites.
			
			// RB begin
#if defined(_WIN32)
			session->HandleBootableInvite( _atoi64( argv[ i + 1 ] ) );
#else
			session->HandleBootableInvite( atol( argv[ i + 1 ] ) );
#endif
			// RB end
		}
		else if( argv[ i ][ 0 ] == '+' )
		{
			com_numConsoleLines++;
			com_consoleLines[ com_numConsoleLines - 1 ].AppendArg( argv[ i ] + 1 );
		}
		else
		{
			if( !com_numConsoleLines )
			{
				com_numConsoleLines++;
			}
			com_consoleLines[ com_numConsoleLines - 1 ].AppendArg( argv[ i ] );
		}
	}
}

/*
==================
budCommonLocal::SafeMode

Check for "safe" on the command line, which will
skip loading of config file (DoomConfig.cfg)
==================
*/
bool budCommonLocal::SafeMode()
{
	int			i;
	
	for( i = 0 ; i < com_numConsoleLines ; i++ )
	{
		if( !budStr::Icmp( com_consoleLines[ i ].Argv( 0 ), "safe" )
				|| !budStr::Icmp( com_consoleLines[ i ].Argv( 0 ), "cvar_restart" ) )
		{
			com_consoleLines[ i ].Clear();
			return true;
		}
	}
	return false;
}

/*
==================
budCommonLocal::StartupVariable

Searches for command line parameters that are set commands.
If match is not NULL, only that cvar will be looked for.
That is necessary because cddir and basedir need to be set
before the filesystem is started, but all other sets should
be after execing the config and default.
==================
*/
void budCommonLocal::StartupVariable( const char* match )
{
	int i = 0;
	while(	i < com_numConsoleLines )
	{
		if( strcmp( com_consoleLines[ i ].Argv( 0 ), "set" ) != 0 )
		{
			i++;
			continue;
		}
		const char* s = com_consoleLines[ i ].Argv( 1 );
		
		if( !match || !budStr::Icmp( s, match ) )
		{
			cvarSystem->SetCVarString( s, com_consoleLines[ i ].Argv( 2 ) );
		}
		i++;
	}
}

/*
==================
budCommonLocal::AddStartupCommands

Adds command line parameters as script statements
Commands are separated by + signs

Returns true if any late commands were added, which
will keep the demoloop from immediately starting
==================
*/
void budCommonLocal::AddStartupCommands()
{
	// quote every token, so args with semicolons can work
	for( int i = 0; i < com_numConsoleLines; i++ )
	{
		if( !com_consoleLines[i].Argc() )
		{
			continue;
		}
		// directly as tokenized so nothing gets screwed
		cmdSystem->BufferCommandArgs( CMD_EXEC_APPEND, com_consoleLines[i] );
	}
}

/*
==================
budCommonLocal::WriteConfigToFile
==================
*/
void budCommonLocal::WriteConfigToFile( const char* filename )
{
	budFile* f = fileSystem->OpenFileWrite( filename );
	if( !f )
	{
		Printf( "Couldn't write %s.\n", filename );
		return;
	}
	
	idKeyInput::WriteBindings( f );
	cvarSystem->WriteFlaggedVariables( CVAR_ARCHIVE, "set", f );
	fileSystem->CloseFile( f );
}

/*
===============
budCommonLocal::WriteConfiguration

Writes key bindings and archived cvars to config file if modified
===============
*/
void budCommonLocal::WriteConfiguration()
{
	// if we are quiting without fully initializing, make sure
	// we don't write out anything
	if( !com_fullyInitialized )
	{
		return;
	}
	
	if( !( cvarSystem->GetModifiedFlags() & CVAR_ARCHIVE ) )
	{
		return;
	}
	cvarSystem->ClearModifiedFlags( CVAR_ARCHIVE );
	
	// save to the profile
	idLocalUser* user = session->GetSignInManager().GetMasterLocalUser();
	if( user != NULL )
	{
		user->SaveProfileSettings();
	}
	
#ifdef CONFIG_FILE
	// disable printing out the "Writing to:" message
	bool developer = com_developer.GetBool();
	com_developer.SetBool( false );
	
	WriteConfigToFile( CONFIG_FILE );
	
	// restore the developer cvar
	com_developer.SetBool( developer );
#endif
}

/*
===============
KeysFromBinding()
Returns the key bound to the command
===============
*/
const char* budCommonLocal::KeysFromBinding( const char* bind )
{
	return idKeyInput::KeysFromBinding( bind );
}

/*
===============
BindingFromKey()
Returns the binding bound to key
===============
*/
const char* budCommonLocal::BindingFromKey( const char* key )
{
	return idKeyInput::BindingFromKey( key );
}

/*
===============
ButtonState()
Returns the state of the button
===============
*/
int	budCommonLocal::ButtonState( int key )
{
	return usercmdGen->ButtonState( key );
}

/*
===============
ButtonState()
Returns the state of the key
===============
*/
int	budCommonLocal::KeyState( int key )
{
	return usercmdGen->KeyState( key );
}

/*
============
budCmdSystemLocal::PrintMemInfo_f

This prints out memory debugging data
============
*/
CONSOLE_COMMAND( printMemInfo, "prints memory debugging data", NULL )
{
	MemInfo_t mi;
	memset( &mi, 0, sizeof( mi ) );
	mi.filebase = commonLocal.GetCurrentMapName();
	
	common->Printf( " Used image memory: %s bytes\n", budStr::FormatNumber( mi.imageAssetsTotal ).c_str() );
	mi.assetTotals += mi.imageAssetsTotal;
	
	common->Printf( " Used model memory: %s bytes\n", budStr::FormatNumber( mi.modelAssetsTotal ).c_str() );
	mi.assetTotals += mi.modelAssetsTotal;
	
	common->Printf( " Used sound memory: %s bytes\n", budStr::FormatNumber( mi.soundAssetsTotal ).c_str() );
	mi.assetTotals += mi.soundAssetsTotal;
	
	common->Printf( " Used asset memory: %s bytes\n", budStr::FormatNumber( mi.assetTotals ).c_str() );
	
	// write overview file
	budFile* f;
	
	f = fileSystem->OpenFileAppend( "maps/printmeminfo.txt" );
	if( !f )
	{
		return;
	}
	
	f->Printf( "total(%s ) image(%s ) model(%s ) sound(%s ): %s\n", budStr::FormatNumber( mi.assetTotals ).c_str(), budStr::FormatNumber( mi.imageAssetsTotal ).c_str(),
			   budStr::FormatNumber( mi.modelAssetsTotal ).c_str(), budStr::FormatNumber( mi.soundAssetsTotal ).c_str(), mi.filebase.c_str() );
			   
	fileSystem->CloseFile( f );
}

/*
==================
Com_Error_f

Just throw a fatal error to test error shutdown procedures.
==================
*/
CONSOLE_COMMAND( error, "causes an error", NULL )
{
	if( !com_developer.GetBool() )
	{
		commonLocal.Printf( "error may only be used in developer mode\n" );
		return;
	}
	
	if( args.Argc() > 1 )
	{
		commonLocal.FatalError( "Testing fatal error" );
	}
	else
	{
		commonLocal.Error( "Testing drop error" );
	}
}

/*
==================
Com_Freeze_f

Just freeze in place for a given number of seconds to test error recovery.
==================
*/
CONSOLE_COMMAND( freeze, "freezes the game for a number of seconds", NULL )
{
	float	s;
	int		start, now;
	
	if( args.Argc() != 2 )
	{
		commonLocal.Printf( "freeze <seconds>\n" );
		return;
	}
	
	if( !com_developer.GetBool() )
	{
		commonLocal.Printf( "freeze may only be used in developer mode\n" );
		return;
	}
	
	s = atof( args.Argv( 1 ) );
	
	start = eventLoop->Milliseconds();
	
	while( 1 )
	{
		now = eventLoop->Milliseconds();
		if( ( now - start ) * 0.001f > s )
		{
			break;
		}
	}
}

/*
=================
Com_Crash_f

A way to force a bus error for development reasons
=================
*/
CONSOLE_COMMAND( crash, "causes a crash", NULL )
{
	if( !com_developer.GetBool() )
	{
		commonLocal.Printf( "crash may only be used in developer mode\n" );
		return;
	}
#ifdef __GNUC__
	__builtin_trap();
#else
	* ( int* ) 0 = 0x12345678;
#endif
}

/*
=================
Com_Quit_f
=================
*/
CONSOLE_COMMAND_SHIP( quit, "quits the game", NULL )
{
	commonLocal.Quit();
}
CONSOLE_COMMAND_SHIP( exit, "exits the game", NULL )
{
	commonLocal.Quit();
}

/*
===============
Com_WriteConfig_f

Write the config file to a specific name
===============
*/
CONSOLE_COMMAND( writeConfig, "writes a config file", NULL )
{
	budStr	filename;
	
	if( args.Argc() != 2 )
	{
		commonLocal.Printf( "Usage: writeconfig <filename>\n" );
		return;
	}
	
	filename = args.Argv( 1 );
	filename.DefaultFileExtension( ".cfg" );
	commonLocal.Printf( "Writing %s.\n", filename.c_str() );
	commonLocal.WriteConfigToFile( filename );
}

/*
========================
budCommonLocal::CheckStartupStorageRequirements
========================
*/
void budCommonLocal::CheckStartupStorageRequirements()
{
	// RB: disabled savegame and profile storage checks, because it fails sometimes without any clear reason
#if 0
	int64 availableSpace = 0;
	
	// ------------------------------------------------------------------------
	// Savegame and Profile required storage
	// ------------------------------------------------------------------------
	{
		// Make sure the save path exists in case it was deleted.
		// If the path cannot be created we can safely assume there is no
		// free space because in that case nothing can be saved anyway.
		const char* savepath = cvarSystem->GetCVarString( "fs_savepath" );
		budStr directory = savepath;
		//budStr directory = fs_savepath.GetString();
		directory += "\\";	// so it doesn't think the last part is a file and ignores in the directory creation
		fileSystem->CreateOSPath( directory );
		
		// Get the free space on the save path.
		availableSpace = Sys_GetDriveFreeSpaceInBytes( savepath );
		
		// If free space fails then get space on drive as a fall back
		// (the directory will be created later anyway)
		if( availableSpace <= 1 )
		{
			budStr savePath( savepath );
			if( savePath.Length() >= 3 )
			{
				if( savePath[ 1 ] == ':' && savePath[ 2 ] == '\\' &&
						( ( savePath[ 0 ] >= 'A' && savePath[ 0 ] <= 'Z' ) ||
						  ( savePath[ 0 ] >= 'a' && savePath[ 0 ] <= 'z' ) ) )
				{
					savePath = savePath.Left( 3 );
					availableSpace = Sys_GetDriveFreeSpaceInBytes( savePath );
				}
			}
		}
	}
	
	const int MIN_SAVE_STORAGE_PROFILE		= 1024 * 1024;
	const int MIN_SAVE_STORAGE_SAVEGAME		= MIN_SAVEGAME_SIZE_BYTES;
	
	uint64 requiredSizeBytes = MIN_SAVE_STORAGE_SAVEGAME + MIN_SAVE_STORAGE_PROFILE;
	
	libBud::Printf( "requiredSizeBytes: %lld\n", requiredSizeBytes );
	
	if( ( int64 )( requiredSizeBytes - availableSpace ) > 0 )
	{
		class budSWFScriptFunction_Continue : public budSWFScriptFunction_RefCounted
		{
		public:
			virtual ~budSWFScriptFunction_Continue() {}
			budSWFScriptVar Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
			{
				common->Dialog().ClearDialog( GDM_INSUFFICENT_STORAGE_SPACE );
				common->Quit();
				return budSWFScriptVar();
			}
		};
		
		budStaticList< budSWFScriptFunction*, 4 > callbacks;
		budStaticList< budStrId, 4 > optionText;
		callbacks.Append( new( TAG_SWF ) budSWFScriptFunction_Continue() );
		optionText.Append( budStrId( "#STR_SWF_ACCEPT" ) );
		
		// build custom space required string
		// #str_dlg_space_required ~= "There is insufficient storage available.  Please free %s and try again."
		budStr format = budStrId( "#str_dlg_startup_insufficient_storage" ).GetLocalizedString();
		budStr size;
		if( requiredSizeBytes > ( 1024 * 1024 ) )
		{
			size = va( "%.1f MB", ( float )requiredSizeBytes / ( 1024.0f * 1024.0f ) + 0.1f );	// +0.1 to avoid truncation
		}
		else
		{
			size = va( "%.1f KB", ( float )requiredSizeBytes / 1024.0f + 0.1f );
		}
		budStr msg = va( format.c_str(), size.c_str() );
		
		common->Dialog().AddDynamicDialog( GDM_INSUFFICENT_STORAGE_SPACE, callbacks, optionText, true, msg );
	}
#endif
	// RB end
	
	session->GetAchievementSystem().Start();
}

/*
===============
budCommonLocal::JapaneseCensorship
===============
*/
bool budCommonLocal::JapaneseCensorship() const
{
	return com_japaneseCensorship.GetBool() || com_isJapaneseSKU;
}

/*
===============
budCommonLocal::FilterLangList
===============
*/
void budCommonLocal::FilterLangList( budStrList* list, budStr lang )
{

	budStr temp;
	for( int i = 0; i < list->Num(); i++ )
	{
		temp = ( *list )[i];
		temp = temp.Right( temp.Length() - strlen( "strings/" ) );
		temp = temp.Left( lang.Length() );
		if( budStr::Icmp( temp, lang ) != 0 )
		{
			list->RemoveIndex( i );
			i--;
		}
	}
}

/*
===============
budCommonLocal::InitLanguageDict
===============
*/
extern budCVar sys_lang;
void budCommonLocal::InitLanguageDict()
{
	budStr fileName;
	
	//D3XP: Instead of just loading a single lang file for each language
	//we are going to load all files that begin with the language name
	//similar to the way pak files work. So you can place english001.lang
	//to add new strings to the english language dictionary
	budFileList*	langFiles;
	langFiles =  fileSystem->ListFilesTree( "strings", ".lang", true );
	
	budStrList langList = langFiles->GetList();
	
	// Loop through the list and filter
	budStrList currentLangList = langList;
	FilterLangList( &currentLangList, sys_lang.GetString() );
	
	if( currentLangList.Num() == 0 )
	{
		// reset to english and try to load again
		sys_lang.SetString( ID_LANG_ENGLISH );
		currentLangList = langList;
		FilterLangList( &currentLangList, sys_lang.GetString() );
	}
	
	budLocalization::ClearDictionary();
	for( int i = 0; i < currentLangList.Num(); i++ )
	{
		//common->Printf("%s\n", currentLangList[i].c_str());
		const byte* buffer = NULL;
		int len = fileSystem->ReadFile( currentLangList[i], ( void** )&buffer );
		if( len <= 0 )
		{
			assert( false && "couldn't read the language dict file" );
			break;
		}
		budLocalization::LoadDictionary( buffer, len, currentLangList[i] );
		fileSystem->FreeFile( ( void* )buffer );
	}
	
	fileSystem->FreeFileList( langFiles );
}

/*
=================
ReloadLanguage_f
=================
*/
CONSOLE_COMMAND( reloadLanguage, "reload language dict", NULL )
{
	commonLocal.InitLanguageDict();
}

/*
=================
Com_FinishBuild_f
=================
*/
CONSOLE_COMMAND( finishBuild, "finishes the build process", NULL )
{

}

/*
=================
budCommonLocal::InitSIMD
=================
*/
void budCommonLocal::InitSIMD()
{
	idSIMD::InitProcessor( "doom", com_forceGenericSIMD.GetBool() );
	com_forceGenericSIMD.ClearModified();
}


/*
=================
budCommonLocal::LoadGameDLL
=================
*/
void budCommonLocal::LoadGameDLL()
{
#ifdef __DOOM_DLL__
	char			dllPath[ MAX_OSPATH ];
	
	gameImport_t	gameImport;
	gameExport_t	gameExport;
	GetGameAPI_t	GetGameAPI;
	
	fileSystem->FindDLL( "game", dllPath, true );
	
	if( !dllPath[ 0 ] )
	{
		common->FatalError( "couldn't find game dynamic library" );
		return;
	}
	common->DPrintf( "Loading game DLL: '%s'\n", dllPath );
	gameDLL = sys->DLL_Load( dllPath );
	if( !gameDLL )
	{
		common->FatalError( "couldn't load game dynamic library" );
		return;
	}
	
	const char* functionName = "GetGameAPI";
	GetGameAPI = ( GetGameAPI_t ) Sys_DLL_GetProcAddress( gameDLL, functionName );
	if( !GetGameAPI )
	{
		Sys_DLL_Unload( gameDLL );
		gameDLL = NULL;
		common->FatalError( "couldn't find game DLL API" );
		return;
	}
	
	gameImport.version					= GAME_API_VERSION;
	gameImport.sys						= ::sys;
	gameImport.common					= ::common;
	gameImport.cmdSystem				= ::cmdSystem;
	gameImport.cvarSystem				= ::cvarSystem;
	gameImport.fileSystem				= ::fileSystem;
	gameImport.renderSystem				= ::renderSystem;
	gameImport.soundSystem				= ::soundSystem;
	gameImport.renderModelManager		= ::renderModelManager;
	gameImport.uiManager				= ::uiManager;
	gameImport.declManager				= ::declManager;
	gameImport.AASFileManager			= ::AASFileManager;
	gameImport.collisionModelManager	= ::collisionModelManager;
	
	gameExport							= *GetGameAPI( &gameImport );
	
	if( gameExport.version != GAME_API_VERSION )
	{
		Sys_DLL_Unload( gameDLL );
		gameDLL = NULL;
		common->FatalError( "wrong game DLL API version" );
		return;
	}
	
	game								= gameExport.game;
	gameEdit							= gameExport.gameEdit;
	
#endif

}

/*
=================
budCommonLocal::UnloadGameDLL
=================
*/
void budCommonLocal::CleanupShell()
{

}

/*
=================
budCommonLocal::UnloadGameDLL
=================
*/
void budCommonLocal::UnloadGameDLL()
{

#ifdef __DOOM_DLL__
	
	if( gameDLL )
	{
		Sys_DLL_Unload( gameDLL );
		gameDLL = NULL;
	}
	game = NULL;
	gameEdit = NULL;
	
#endif
}

/*
=================
budCommonLocal::IsInitialized
=================
*/
bool budCommonLocal::IsInitialized() const
{
	return com_fullyInitialized;
}


//======================================================================================


/*
=================
budCommonLocal::Init
=================
*/
void budCommonLocal::Init( int argc, const char* const* argv, const char* cmdline )
{
	try
	{
		// set interface pointers used by libBud
		libBud::sys			= sys;
		libBud::common		= common;
		libBud::cvarSystem	= cvarSystem;
		libBud::fileSystem	= fileSystem;
		
		// initialize libBud
		libBud::Init();
		
		// clear warning buffer
		ClearWarnings( GAME_NAME " initialization" );
		
		libBud::Printf( "Command line: %s\n", cmdline );
		//::MessageBox( NULL, cmdline, "blah", MB_OK );
		// parse command line options
		budCmdArgs args;
		if( cmdline )
		{
			// tokenize if the OS doesn't do it for us
			args.TokenizeString( cmdline, true );
			argv = args.GetArgs( &argc );
		}
		ParseCommandLine( argc, argv );
		
		// init console command system
		cmdSystem->Init();
		
		// init CVar system
		cvarSystem->Init();
		
		// register all static CVars
		budCVar::RegisterStaticVars();
		
		libBud::Printf( "QA Timing INIT: %06dms\n", Sys_Milliseconds() );
		
		// print engine version
		Printf( "%s\n", version.string );
		
		// initialize key input/binding, done early so bind command exists
		idKeyInput::Init();
		
		// init the console so we can take prints
		console->Init();
		
		// get architecture info
		Sys_Init();
		
		// initialize networking
		Sys_InitNetworking();
		
		// override cvars from command line
		StartupVariable( NULL );
		
		consoleUsed = com_allowConsole.GetBool();
		
		if( Sys_AlreadyRunning() )
		{
			Sys_Quit();
		}
		
		// initialize processor specific SIMD implementation
		InitSIMD();
		
		// initialize the file system
		fileSystem->Init();
		
		const char* defaultLang = Sys_DefaultLanguage();
		com_isJapaneseSKU = ( budStr::Icmp( defaultLang, ID_LANG_JAPANESE ) == 0 );
		
		// Allow the system to set a default lanugage
		Sys_SetLanguageFromSystem();
		
		// Pre-allocate our 20 MB save buffer here on time, instead of on-demand for each save....
		
		// saveFile.SetNameAndType( SAVEGAME_CHECKPOINT_FILENAME, SAVEGAMEFILE_BINARY );
		// saveFile.PreAllocate( MIN_SAVEGAME_SIZE_BYTES );
		
		// stringsFile.SetNameAndType( SAVEGAME_STRINGS_FILENAME, SAVEGAMEFILE_BINARY );
		// stringsFile.PreAllocate( MAX_SAVEGAME_STRING_TABLE_SIZE );
		
		fileSystem->BeginLevelLoad( "_startup", saveFile.GetDataPtr(), saveFile.GetAllocated() );
		
		// initialize the declaration manager
		// declManager->Init();
		
		// init journalling, etc
		eventLoop->Init();
		
		// init the parallel job manager
		parallelJobManager->Init();
		
		// exec the startup scripts
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "exec default.cfg\n" );
		
#ifdef CONFIG_FILE
		// skip the config file if "safe" is on the command line
		if(!SafeMode())
		{
			cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "exec " CONFIG_FILE "\n" );
		}
#endif
		
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "exec autoexec.cfg\n" );
		
		// run cfg execution
		cmdSystem->ExecuteCommandBuffer();
		
		// re-override anything from the config files with command line args
		StartupVariable( NULL );
		
		// if any archived cvars are modified after this, we will trigger a writing of the config file
		cvarSystem->ClearModifiedFlags( CVAR_ARCHIVE );
		
		// Support up to 2 digits after the decimal point
		com_engineHz_denominator = 100LL * com_engineHz.GetFloat();
		com_engineHz_latched = com_engineHz.GetFloat();
		
		const int legalMinTime = 4000;
		const bool showVideo = ( !com_skipIntroVideos.GetBool() && fileSystem->UsingResourceFiles() );
		
		int legalStartTime = Sys_Milliseconds();
		// declManager->Init2();
		
		// initialize string database so we can use it for loading messages
		InitLanguageDict();
		
		// spawn the game thread, even if we are going to run without SMP
		// one meg stack, because it can parse decls from gui surfaces (unfortunately)
		// use a lower priority so job threads can run on the same core
		gameThread.StartWorkerThread( "Game/Draw", CORE_1B, THREAD_BELOW_NORMAL, 0x100000 );
		// boost this thread's priority, so it will prevent job threads from running while
		// the render back end still has work to do
		
		// init the user command input code
		usercmdGen->Init();
		
		Sys_SetRumble( 0, 0, 0 );
		
		// startup the script debugger
		// DebuggerServerInit();
		
		// Init tool commands
		InitCommands();
		
		// load the game dll
		LoadGameDLL();
		
		
		fileSystem->UnloadResourceContainer( "_ordered" );
		
		// init the session
		session->Initialize();
		session->InitializeSoundRelatedSystems();
		
		// load the console history file
		consoleHistory.LoadHistoryFile();
		
		AddStartupCommands();
		
		StartMenu( true );
		
		while( Sys_Milliseconds() - legalStartTime < legalMinTime )
		{
			Sys_GenerateEvents();
			Sys_Sleep( 10 );
		};
		
		// print all warnings queued during initialization
		PrintWarnings();
		
		// remove any prints from the notify lines
		console->ClearNotifyLines();
		
		CheckStartupStorageRequirements();
		
		
		if( preload_CommonAssets.GetBool() && fileSystem->UsingResourceFiles() )
		{
			idPreloadManifest manifest;
			manifest.LoadManifest( "_common.preload" );
		}
		
		fileSystem->EndLevelLoad();
		
		com_fullyInitialized = true;
		
		Printf( "--- Common Initialization Complete ---\n" );
		
		libBud::Printf( "QA Timing IIS: %06dms\n", Sys_Milliseconds() );
	}
	catch( idException& )
	{
		Sys_Error( "Error during initialization" );
	}
}

/*
=================
budCommonLocal::Shutdown
=================
*/
void budCommonLocal::Shutdown()
{

	if( com_shuttingDown )
	{
		return;
	}
	com_shuttingDown = true;
	
	
	// Kill any pending saves...
	printf( "session->GetSaveGameManager().CancelToTerminate();\n" );
	session->GetSaveGameManager().CancelToTerminate();
	
	// shutdown the script debugger
	// DebuggerServerShutdown();
	
	if( aviCaptureMode )
	{
		printf( "EndAVICapture();\n" );
		EndAVICapture();
	}
	
	printf( "Stop();\n" );
	Stop();
	
	printf( "CleanupShell();\n" );
	CleanupShell();
	
	printf( "session->Shutdown();\n" );
	session->Shutdown();
	
	// shut down the event loop
	printf( "eventLoop->Shutdown();\n" );
	eventLoop->Shutdown();
	
	// shutdown the decl manager
	// printf( "declManager->Shutdown();\n" );
	// declManager->Shutdown();
	
	// unload the game dll
	printf( "UnloadGameDLL();\n" );
	UnloadGameDLL();
	
	printf( "saveFile.Clear( true );\n" );
	saveFile.Clear( true );
	printf( "stringsFile.Clear( true );\n" );
	stringsFile.Clear( true );
	
	// only shut down the log file after all output is done
	printf( "CloseLogFile();\n" );
	CloseLogFile();
	
	// shut down the file system
	printf( "fileSystem->Shutdown( false );\n" );
	fileSystem->Shutdown( false );
	
	// shut down non-portable system services
	printf( "Sys_Shutdown();\n" );
	Sys_Shutdown();
	
	// shut down the console
	printf( "console->Shutdown();\n" );
	console->Shutdown();
	
	// shut down the key system
	printf( "idKeyInput::Shutdown();\n" );
	idKeyInput::Shutdown();
	
	// shut down the cvar system
	printf( "cvarSystem->Shutdown();\n" );
	cvarSystem->Shutdown();
	
	// shut down the console command system
	printf( "cmdSystem->Shutdown();\n" );
	cmdSystem->Shutdown();
	
	// free any buffered warning messages
	printf( "ClearWarnings( GAME_NAME \" shutdown\" );\n" );
	ClearWarnings( GAME_NAME " shutdown" );
	printf( "warningCaption.Clear();\n" );
	warningCaption.Clear();
	printf( "errorList.Clear();\n" );
	errorList.Clear();
	
	// shutdown libBud
	printf( "libBud::ShutDown();\n" );
	libBud::ShutDown();
}

/*
========================
budCommonLocal::CreateMainMenu
========================
*/
void budCommonLocal::CreateMainMenu()
{

}

/*
===============
budCommonLocal::Stop

called on errors and game exits
===============
*/
void budCommonLocal::Stop( bool resetSession )
{
	ClearWipe();
	
	// clear mapSpawned and demo playing flags
	UnloadMap();
	
	insideExecuteMapChange = false;
	
	// drop all guis
	ExitMenu();
	
	if( resetSession )
	{
		session->QuitMatchToTitle();
	}
}

/*
===============
budCommonLocal::BusyWait
===============
*/
void budCommonLocal::BusyWait()
{
	Sys_GenerateEvents();
	
	const bool captureToImage = false;
	
	session->UpdateSignInManager();
	session->Pump();
}


/*
===============
budCommonLocal::InitCommands
===============
*/
void budCommonLocal::InitCommands()
{
	// compilers
	// cmdSystem->AddCommand( "dmap", Dmap_f, CMD_FL_TOOL, "compiles a map", budCmdSystem::ArgCompletion_MapName );
	// cmdSystem->AddCommand( "runAAS", RunAAS_f, CMD_FL_TOOL, "compiles an AAS file for a map", budCmdSystem::ArgCompletion_MapName );
	// cmdSystem->AddCommand( "runAASDir", RunAASDir_f, CMD_FL_TOOL, "compiles AAS files for all maps in a folder", budCmdSystem::ArgCompletion_MapName );
	// cmdSystem->AddCommand( "runReach", RunReach_f, CMD_FL_TOOL, "calculates reachability for an AAS file", budCmdSystem::ArgCompletion_MapName );
}

/*
===============
budCommonLocal::WaitForSessionState
===============
*/
bool budCommonLocal::WaitForSessionState( budSession::sessionState_t desiredState )
{
	if( session->GetState() == desiredState )
	{
		return true;
	}
	
	while( true )
	{
		BusyWait();
		
		budSession::sessionState_t sessionState = session->GetState();
		if( sessionState == desiredState )
		{
			return true;
		}
		if( sessionState != budSession::LOADING &&
				sessionState != budSession::SEARCHING &&
				sessionState != budSession::CONNECTING &&
				sessionState != budSession::BUSY &&
				sessionState != desiredState )
		{
			return false;
		}
		
		Sys_Sleep( 10 );
	}
}

/*
========================
budCommonLocal::LeaveGame
========================
*/
void budCommonLocal::LeaveGame()
{

	const bool captureToImage = false;
	
	ResetNetworkingState();
	
	
	Stop( false );
	
	CreateMainMenu();
	
	StartMenu();
	
	
}

/*
===============
budCommonLocal::ProcessEvent
===============
*/
bool budCommonLocal::ProcessEvent( const sysEvent_t* event )
{
	
	// let the pull-down console take it if desired
	if( console->ProcessEvent( event, false ) )
	{
		return true;
	}
	if( session->ProcessInputEvent( event ) )
	{
		return true;
	}
	
	// menus / etc
	if( MenuEvent( event ) )
	{
		return true;
	}
	
	// if we aren't in a game, force the console to take it
	if( !mapSpawned )
	{
		console->ProcessEvent( event, true );
		return true;
	}
	
	// in game, exec bindings for all key downs
	if( event->evType == SE_KEY && event->evValue2 == 1 )
	{
		idKeyInput::ExecKeyBinding( event->evValue );
		return true;
	}
	
	return false;
}

/*
========================
budCommonLocal::ResetPlayerInput
========================
*/
void budCommonLocal::ResetPlayerInput( int playerIndex )
{
	userCmdMgr.ResetPlayer( playerIndex );
}

/*
==================
Common_WritePrecache_f
==================
*/
CONSOLE_COMMAND( writePrecache, "writes precache commands", NULL )
{
	if( args.Argc() != 2 )
	{
		common->Printf( "USAGE: writePrecache <execFile>\n" );
		return;
	}
	budStr	str = args.Argv( 1 );
	str.DefaultFileExtension( ".cfg" );
	budFile* f = fileSystem->OpenFileWrite( str );
	// declManager->WritePrecacheCommands( f );
	
	fileSystem->CloseFile( f );
}

/*
================
Common_Disconnect_f
================
*/
CONSOLE_COMMAND_SHIP( disconnect, "disconnects from a game", NULL )
{
	session->QuitMatch();
}

/*
===============
Common_Hitch_f
===============
*/
CONSOLE_COMMAND( hitch, "hitches the game", NULL )
{
	if( args.Argc() == 2 )
	{
		Sys_Sleep( atoi( args.Argv( 1 ) ) );
	}
	else
	{
		Sys_Sleep( 100 );
	}
}

CONSOLE_COMMAND( showStringMemory, "shows memory used by strings", NULL )
{
	budStr::ShowMemoryUsage_f( args );
}
CONSOLE_COMMAND( showDictMemory, "shows memory used by dictionaries", NULL )
{
	idDict::ShowMemoryUsage_f( args );
}
CONSOLE_COMMAND( listDictKeys, "lists all keys used by dictionaries", NULL )
{
	idDict::ListKeys_f( args );
}
CONSOLE_COMMAND( listDictValues, "lists all values used by dictionaries", NULL )
{
	idDict::ListValues_f( args );
}
CONSOLE_COMMAND( testSIMD, "test SIMD code", NULL )
{
	idSIMD::Test_f( args );
}

// RB begin
CONSOLE_COMMAND( testFormattingSizes, "test printf format security", 0 )
{
	common->Printf( " sizeof( int32 ): %" PRIuSIZE " bytes\n", sizeof( int32 ) );
	common->Printf( " sizeof( int64 ): %" PRIuSIZE " bytes\n", sizeof( int64 ) );
}
// RB end
