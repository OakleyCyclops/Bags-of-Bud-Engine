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
#include "corePCH.hpp"
#include "coreSessionLocal.hpp"
#include "coreVOIP.hpp"
#include "coreServerSearch.hpp"


budCVar ui_skinIndex( "ui_skinIndex", "0", CVAR_ARCHIVE, "Selected skin index" );
budCVar ui_autoSwitch( "ui_autoSwitch", "1", CVAR_ARCHIVE | CVAR_BOOL, "auto switch weapon" );
budCVar ui_autoReload( "ui_autoReload", "1", CVAR_ARCHIVE | CVAR_BOOL, "auto reload weapon" );

budCVar net_maxSearchResults( "net_maxSearchResults", "25", CVAR_INTEGER, "Max results that are allowed to be returned in a search request" );
budCVar net_maxSearchResultsToTry( "net_maxSearchResultsToTry", "5", CVAR_INTEGER, "Max results to try before giving up." );		// At 15 second timeouts per, 1 min 15 worth of connecting attempt time

budCVar net_LobbyCoalesceTimeInSeconds( "net_LobbyCoalesceTimeInSeconds", "30", CVAR_INTEGER, "Time in seconds when a lobby will try to coalesce with another lobby when there is only one user." );
budCVar net_LobbyRandomCoalesceTimeInSeconds( "net_LobbyRandomCoalesceTimeInSeconds", "3", CVAR_INTEGER, "Random time to add to net_LobbyCoalesceTimeInSeconds" );

budCVar net_useGameStateLobby( "net_useGameStateLobby", "0", CVAR_BOOL, "" );
//budCVar net_useGameStateLobby( "net_useGameStateLobby", "1", CVAR_BOOL, "" );

#if !defined( ID_RETAIL ) || defined( ID_RETAIL_INTERNAL )
budCVar net_ignoreTitleStorage( "net_ignoreTitleStorage", "0", CVAR_BOOL, "Ignore title storage" );
#endif

budCVar net_maxLoadResourcesTimeInSeconds( "net_maxLoadResourcesTimeInSeconds", "0", CVAR_INTEGER, "How long, in seconds, clients have to load resources. Used for loose asset builds." );
budCVar net_migrateHost( "net_migrateHost", "-1", CVAR_INTEGER, "Become host of session (0 = party, 1 = game) for testing purposes" );
extern budCVar net_debugBaseStates;

budCVar net_testPartyMemberConnectFail( "net_testPartyMemberConnectFail", "-1", CVAR_INTEGER, "Force this party member index to fail to connect to games." );

//FIXME: this could use a better name.
budCVar net_offlineTransitionThreshold( "net_offlineTransitionThreshold", "1000", CVAR_INTEGER, "Time, in milliseconds, to wait before kicking back to the main menu when a profile losses backend connection during an online game" );

budCVar net_port( "net_port", "27015", CVAR_INTEGER | CVAR_NOCHEAT, "host port number" ); // Port to host when using dedicated servers, port to broadcast on when looking for a dedicated server to connect to
budCVar net_headlessServer( "net_headlessServer", "0", CVAR_BOOL, "toggle to automatically host a game and allow peer[0] to control menus" );

const char* budSessionLocal::stateToString[ NUM_STATES ] =
{
	ASSERT_ENUM_STRING( STATE_PRESS_START, 0 ),
	ASSERT_ENUM_STRING( STATE_IDLE, 1 ),
	ASSERT_ENUM_STRING( STATE_PARTY_LOBBY_HOST, 2 ),
	ASSERT_ENUM_STRING( STATE_PARTY_LOBBY_PEER, 3 ),
	ASSERT_ENUM_STRING( STATE_GAME_LOBBY_HOST, 4 ),
	ASSERT_ENUM_STRING( STATE_GAME_LOBBY_PEER, 5 ),
	ASSERT_ENUM_STRING( STATE_GAME_STATE_LOBBY_HOST, 6 ),
	ASSERT_ENUM_STRING( STATE_GAME_STATE_LOBBY_PEER, 7 ),
	ASSERT_ENUM_STRING( STATE_CREATE_AND_MOVE_TO_PARTY_LOBBY, 8 ),
	ASSERT_ENUM_STRING( STATE_CREATE_AND_MOVE_TO_GAME_LOBBY, 9 ),
	ASSERT_ENUM_STRING( STATE_CREATE_AND_MOVE_TO_GAME_STATE_LOBBY, 10 ),
	ASSERT_ENUM_STRING( STATE_FIND_OR_CREATE_MATCH, 11 ),
	ASSERT_ENUM_STRING( STATE_CONNECT_AND_MOVE_TO_PARTY, 12 ),
	ASSERT_ENUM_STRING( STATE_CONNECT_AND_MOVE_TO_GAME, 13 ),
	ASSERT_ENUM_STRING( STATE_CONNECT_AND_MOVE_TO_GAME_STATE, 14 ),
	ASSERT_ENUM_STRING( STATE_BUSY, 15 ),
	ASSERT_ENUM_STRING( STATE_LOADING, 16 ),
	ASSERT_ENUM_STRING( STATE_INGAME, 17 ),
};

struct netVersion_s
{
	netVersion_s()
	{
		sprintf( string, "%s.%d", ENGINE_VERSION, BUILD_NUMBER );
	}
	char	string[256];
} netVersion;

/*
========================
NetGetVersionChecksum
========================
*/
// RB: 64 bit fixes, changed long to int
unsigned int NetGetVersionChecksum()
{
#if 0
	return budStr( com_version.GetString() ).Hash();
#else
	unsigned int ret = 0;
	
	CRC32_InitChecksum( ret );
	CRC32_UpdateChecksum( ret, netVersion.string, budStr::Length( netVersion.string ) );
	CRC32_FinishChecksum( ret );
	
	NET_VERBOSE_PRINT( "NetGetVersionChecksum - string   : %s\n", netVersion.string );
	NET_VERBOSE_PRINT( "NetGetVersionChecksum - checksum : %u\n", ret );
	return ret;
#endif
}
// RB end

/*
========================
budSessionLocal::budSessionLocal
========================
*/
budSessionLocal::budSessionLocal() :
	processorSaveFiles( new( TAG_SAVEGAMES ) idSaveGameProcessorSaveFiles ),
	processorLoadFiles( new( TAG_SAVEGAMES ) idSaveGameProcessorLoadFiles ),
	processorDelete(	new( TAG_SAVEGAMES ) idSaveGameProcessorDelete ),
	processorEnumerate( new( TAG_SAVEGAMES ) idSaveGameProcessorEnumerateGames )
{
	InitBaseState();
}

/*
========================
budSessionLocal::budSessionLocal
========================
*/
budSessionLocal::~budSessionLocal()
{
	delete processorSaveFiles;
	delete processorLoadFiles;
	delete processorDelete;
	delete processorEnumerate;
	delete sessionCallbacks;
}


/*
========================
budSessionLocal::InitBaseState
========================
*/
void budSessionLocal::InitBaseState()
{

	//assert( mem.IsGlobalHeap() );
	
	localState						= STATE_PRESS_START;
	sessionOptions					= 0;
	currentID						= 0;
	
	sessionCallbacks				= new( TAG_NETWORKING ) budSessionLocalCallbacks( this );
	
	connectType						= CONNECT_NONE;
	connectTime						= 0;
	
	upstreamDropRate				= 0.0f;
	upstreamDropRateTime			= 0;
	upstreamQueueRate				= 0.0f;
	upstreamQueueRateTime			= 0;
	queuedBytes						= 0;
	
	lastVoiceSendtime				= 0;
	hasShownVoiceRestrictionDialog	= false;
	
	isSysUIShowing					= false;
	
	pendingInviteDevice				= 0;
	pendingInviteMode				= PENDING_INVITE_NONE;
	
	downloadedContent.Clear();
	marketplaceHasNewContent		= false;
	
	offlineTransitionTimerStart		= 0;
	showMigratingInfoStartTime		= 0;
	nextGameCoalesceTime			= 0;
	gameLobbyWasCoalesced			= false;
	numFullSnapsReceived			= 0;
	
	flushedStats					= false;
	
	titleStorageLoaded				= false;
	
	droppedByHost					= false;
	loadingID						= 0;
	
	storedPeer						= -1;
	storedMsgType					= -1;
	
	inviteInfoRequested				= false;
	
	enumerationHandle				= 0;
	
	waitingOnGameStateMembersToLeaveTime	= 0;
	waitingOnGameStateMembersToJoinTime		= 0;
}

/*
========================
budSessionLocal::FinishDisconnect
========================
*/
void budSessionLocal::FinishDisconnect()
{
	GetPort().Close();
	while( sendQueue.Peek() != NULL )
	{
		sendQueue.RemoveFirst();
	}
	while( recvQueue.Peek() != NULL )
	{
		recvQueue.RemoveFirst();
	}
}

//====================================================================================

budCVar net_connectTimeoutInSeconds( "net_connectTimeoutInSeconds", "15", CVAR_INTEGER, "timeout (in seconds) while connecting" );

/*
========================
budSessionLocal::CreatePartyLobby
========================
*/
void budSessionLocal::CreatePartyLobby( const budMatchParameters& parms_ )
{
	NET_VERBOSE_PRINT( "NET: CreatePartyLobby\n" );
	
	// Shutdown any possible party lobby
	GetPartyLobby().Shutdown();
	GetPartyLobby().ResetAllMigrationState();
	
	// Shutdown any possible game lobby
	GetGameLobby().Shutdown();
	GetGameStateLobby().Shutdown();
	
	// Start hosting a new party lobby
	GetPartyLobby().StartHosting( parms_ );
	
	connectType = CONNECT_NONE;
	connectTime = Sys_Milliseconds();
	
	// Wait for it to complete
	SetState( STATE_CREATE_AND_MOVE_TO_PARTY_LOBBY );
}

/*
========================
budSessionLocal::CreateMatch
========================
*/
void budSessionLocal::CreateMatch( const budMatchParameters& p )
{
	NET_VERBOSE_PRINT( "NET: CreateMatch\n" );
	
	if( ( p.matchFlags & MATCH_PARTY_INVITE_PLACEHOLDER ) && !GetPartyLobby().IsLobbyActive() )
	{
		NET_VERBOSE_PRINT( "NET: CreateMatch MATCH_PARTY_INVITE_PLACEHOLDER\n" );
		CreatePartyLobby( p );
		connectType = CONNECT_NONE;
		return;
	}
	
	// Shutdown any possible game lobby
	GetGameLobby().Shutdown();
	GetGameStateLobby().Shutdown();
	GetGameLobby().ResetAllMigrationState();
	
	// Start hosting a new game lobby
	GetGameLobby().StartHosting( p );
	
	connectType = CONNECT_NONE;
	connectTime = Sys_Milliseconds();
	
	// Wait for it to complete
	SetState( STATE_CREATE_AND_MOVE_TO_GAME_LOBBY );
}

/*
========================
budSessionLocal::CreateGameStateLobby
========================
*/
void budSessionLocal::CreateGameStateLobby( const budMatchParameters& p )
{
	NET_VERBOSE_PRINT( "NET: CreateGameStateLobby\n" );
	
	// Shutdown any possible game state lobby
	GetGameStateLobby().Shutdown();
	GetGameStateLobby().ResetAllMigrationState();
	
	// Start hosting a new game lobby
	GetGameStateLobby().StartHosting( p );
	
	connectType = CONNECT_NONE;
	connectTime = Sys_Milliseconds();
	
	waitingOnGameStateMembersToLeaveTime	= 0;		// Make sure to reset
	waitingOnGameStateMembersToJoinTime		= 0;
	
	// Wait for it to complete
	SetState( STATE_CREATE_AND_MOVE_TO_GAME_STATE_LOBBY );
}

/*
========================
budSessionLocal::FindOrCreateMatch
========================
*/
void budSessionLocal::FindOrCreateMatch( const budMatchParameters& p )
{
	NET_VERBOSE_PRINT( "NET: FindOrCreateMatch\n" );
	
	if( ( p.matchFlags & MATCH_PARTY_INVITE_PLACEHOLDER ) && !GetPartyLobby().IsLobbyActive() )
	{
		NET_VERBOSE_PRINT( "NET: FindOrCreateMatch MATCH_PARTY_INVITE_PLACEHOLDER\n" );
		CreatePartyLobby( p );
		connectType = CONNECT_FIND_OR_CREATE;
		return;
	}
	
	// Shutdown any possible game lobby
	GetGameLobby().Shutdown();
	GetGameStateLobby().Shutdown();
	GetGameLobby().ResetAllMigrationState();
	
	// Start searching for a game
	GetGameLobby().StartFinding( p );
	
	connectType				= CONNECT_FIND_OR_CREATE;
	connectTime				= Sys_Milliseconds();
	gameLobbyWasCoalesced	= false;
	numFullSnapsReceived	= 0;
	
	// Wait for searching to complete
	SetState( STATE_FIND_OR_CREATE_MATCH );
}

/*
========================
budSessionLocal::StartLoading
========================
*/
void budSessionLocal::StartLoading()
{
	NET_VERBOSE_PRINT( "NET: StartLoading\n" );
	
	if( MatchTypeIsOnline( GetActingGameStateLobby().parms.matchFlags ) )
	{
		if( !GetActingGameStateLobby().IsHost() )
		{
			libBud::Warning( "Ignoring call to StartLoading because we are not the host.  state is %s", stateToString[ localState ] );
			return;
		}
		
		for( int p = 0; p < GetActingGameStateLobby().peers.Num(); p++ )
		{
			if( GetActingGameStateLobby().peers[p].IsConnected() )
			{
				GetActingGameStateLobby().QueueReliableMessage( p, idLobby::RELIABLE_START_LOADING );
				GetActingGameStateLobby().peers[p].startResourceLoadTime = Sys_Milliseconds();
			}
		}
	}
	
	VerifySnapshotInitialState();
	
	SetState( STATE_LOADING );
}

/*
========================
budSessionLocal::StartMatch
========================
*/
void budSessionLocal::StartMatch()
{
	NET_VERBOSE_PRINT( "NET: StartMatch\n" );
	
	if( net_headlessServer.GetBool() )
	{
		StartLoading();		// This is so we can force start matches on headless servers to test performance using bots
		return;
	}
	
	if( localState != STATE_GAME_LOBBY_HOST )
	{
		libBud::Warning( "budSessionLocal::StartMatch called when not hosting game lobby" );
		return;
	}
	
	assert( !GetGameStateLobby().IsLobbyActive() );
	
	// make absolutely sure we only call StartMatch once per migrate
	GetGameLobby().migrationInfo.persistUntilGameEndsData.hasRelaunchedMigratedGame = true;
	
	// Clear snap ack queue between games
	GetGameLobby().snapDeltaAckQueue.Clear();
	
	extern budCVar net_bw_challenge_enable;
	if( session->GetTitleStorageBool( "net_bw_challenge_enable", net_bw_challenge_enable.GetBool() ) && GetGameLobby().HasActivePeers() )
	{
		GetGameLobby().bandwidthChallengeFinished = false;
		StartOrContinueBandwidthChallenge( false );
	}
	
	if( GetGameLobby().BandwidthTestStarted() )
	{
		// Put session in busy state
		NET_VERBOSE_PRINT( "NET: StartMatch -> Start Bandwidth Challenge\n" );
		SetState( STATE_BUSY );
	}
	else
	{
		// Start loading
		StartLoading();
	}
}

/*
========================
budSessionLocal::GetBackState
========================
*/
budSessionLocal::sessionState_t budSessionLocal::GetBackState()
{
	sessionState_t currentState = GetState();
	
	const bool isInGameLobby		= currentState == GAME_LOBBY;
	const bool isInPartyLobby		= currentState == PARTY_LOBBY;
	const bool isInGame				= currentState == INGAME || currentState == LOADING;		// Counting loading as ingame as far as what back state to go to
	
	if( isInGame )
	{
		return GAME_LOBBY;		// If in the game, go back to game lobby
	}
	
	if( !isInPartyLobby && isInGameLobby && ShouldHavePartyLobby() )
	{
		return PARTY_LOBBY;		// If in the game lobby, and we should have a party lobby, and we are the host, go back to party lobby
	}
	
	if( currentState != IDLE )
	{
		return IDLE;			// From here, go to idle if we aren't there yet
	}
	
	return PRESS_START;			// Otherwise, go back to press start
}

/*
========================
budSessionLocal::Cancel
========================
*/
void budSessionLocal::Cancel()
{
	NET_VERBOSE_PRINT( "NET: Cancel\n" );
	
	if( localState == STATE_PRESS_START )
	{
		return;		// We're as far back as we can go
	}
	
	ClearVoiceGroups(); // this is here as a catch-all
	
	// See what state we need to go to
	switch( GetBackState() )
	{
		case GAME_LOBBY:
			EndMatch();		// End current match to go to game lobby
			break;
			
		case PARTY_LOBBY:
			if( GetPartyLobby().IsHost() )
			{
				if( sessionOptions & OPTION_LEAVE_WITH_PARTY )
				{
					// NOTE - This will send a message on the team lobby channel,
					// so it won't be affected by the fact that we're shutting down the game lobby
					GetPartyLobby().NotifyPartyOfLeavingGameLobby();
				}
				else
				{
					// Host wants to be alone, disconnect all peers from the party
					GetPartyLobby().DisconnectAllPeers();
				}
				
				// End the game lobby, and go back to party lobby as host
				GetGameLobby().Shutdown();
				GetGameStateLobby().Shutdown();
				SetState( STATE_PARTY_LOBBY_HOST );
				
				// Always remove this flag.  SendGoodbye uses this to determine if we should send a "leave with party"
				// and we don't want this flag hanging around, and causing false positives when it's called in the future.
				// Make them set this each time.
				sessionOptions &= ~OPTION_LEAVE_WITH_PARTY;
			}
			else
			{
				// If we aren't the host of a party and we want to go back to one, we need to create a party now
				CreatePartyLobby( GetPartyLobby().parms );
			}
			break;
			
		case IDLE:
			// Go back to main menu
			GetGameLobby().Shutdown();
			GetGameStateLobby().Shutdown();
			GetPartyLobby().Shutdown();
			SetState( STATE_IDLE );
			break;
			
		case PRESS_START:
			// Go back to press start/main
			GetGameLobby().Shutdown();
			GetGameStateLobby().Shutdown();
			GetPartyLobby().Shutdown();
			SetState( STATE_PRESS_START );
			break;
	}
	
	// Validate the current lobby immediately
	ValidateLobbies();
}

/*
========================
budSessionLocal::MoveToPressStart
========================
*/
void budSessionLocal::MoveToPressStart()
{
	if( localState != STATE_PRESS_START )
	{
		assert( signInManager != NULL );
		signInManager->RemoveAllLocalUsers();
		hasShownVoiceRestrictionDialog = false;
		MoveToMainMenu();
		session->FinishDisconnect();
		SetState( STATE_PRESS_START );
	}
}

/*
========================
budSessionLocal::ShouldShowMigratingDialog
========================
*/
bool budSessionLocal::ShouldShowMigratingDialog() const
{
	const idLobby* activeLobby = GetActivePlatformLobby();
	
	if( activeLobby == NULL )
	{
		return false;
	}
	
	return activeLobby->ShouldShowMigratingDialog();
}

/*
========================
budSessionLocal::IsCurrentLobbyMigrating
========================
*/
bool budSessionLocal::IsCurrentLobbyMigrating() const
{
	const idLobby* activeLobby = GetActivePlatformLobby();
	
	if( activeLobby == NULL )
	{
		return false;
	}
	
	return activeLobby->IsMigrating();
}

/*
========================
budSessionLocal::IsLosingConnectionToHost
========================
*/
bool budSessionLocal::IsLosingConnectionToHost() const
{
	return GetActingGameStateLobby().IsLosingConnectionToHost();
}

/*
========================
budSessionLocal::WasMigrationGame
returns true if we are hosting a migrated game and we had valid migration data
========================
*/
bool budSessionLocal::WasMigrationGame() const
{
	return GetGameLobby().IsMigratedStatsGame();
}

/*
========================
budSessionLocal::ShouldRelaunchMigrationGame
returns true if we are hosting a migrated game and we had valid migration data
========================
*/
bool budSessionLocal::ShouldRelaunchMigrationGame() const
{
	return GetGameLobby().ShouldRelaunchMigrationGame() && !IsCurrentLobbyMigrating();
}

/*
========================
budSessionLocal::GetMigrationGameData
========================
*/
bool budSessionLocal::GetMigrationGameData( budBitMsg& msg, bool reading )
{
	return GetGameLobby().GetMigrationGameData( msg, reading );
}

/*
========================
budSessionLocal::GetMigrationGameDataUser
========================
*/
bool budSessionLocal::GetMigrationGameDataUser( lobbyUserID_t lobbyUserID, budBitMsg& msg, bool reading )
{
	if( GetGameStateLobby().IsHost() )
	{
		return false;
	}
	
	return GetGameLobby().GetMigrationGameDataUser( lobbyUserID, msg, reading );
}


/*
========================
budSessionLocal::GetMatchParamUpdate
========================
*/
bool budSessionLocal::GetMatchParamUpdate( int& peer, int& msg )
{
	if( storedPeer != -1 && storedMsgType != -1 )
	{
		peer = storedPeer;
		msg = storedMsgType;
		storedPeer = -1;
		storedMsgType = -1;
		return true;
	}
	return false;
}



/*
========================
budSessionLocal::UpdatePartyParms

Updates the party parameters when in a party lobby OR a game lobby in order to keep them always in sync.
========================
*/
void budSessionLocal::UpdatePartyParms( const budMatchParameters& p )
{
	if( ( GetState() != PARTY_LOBBY && GetState() != GAME_LOBBY ) || !GetPartyLobby().IsHost() )
	{
		return;
	}
	
	// NET_VERBOSE_PRINT( "NET: UpdatePartyParms\n" );
	
	GetPartyLobby().UpdateMatchParms( p );
}

/*
========================
budSessionLocal::UpdateMatchParms
========================
*/
void budSessionLocal::UpdateMatchParms( const budMatchParameters& p )
{
	if( GetState() != GAME_LOBBY || !GetGameLobby().IsHost() )
	{
		return;
	}
	
	NET_VERBOSE_PRINT( "NET: UpdateMatchParms\n" );
	
	GetGameLobby().UpdateMatchParms( p );
}

/*
========================
budSessionLocal::StartSessions
========================
*/
void budSessionLocal::StartSessions()
{
	if( GetPartyLobby().lobbyBackend != NULL )
	{
		GetPartyLobby().lobbyBackend->StartSession();
	}
	
	if( GetGameLobby().lobbyBackend != NULL )
	{
		GetGameLobby().lobbyBackend->StartSession();
	}
	
	SetLobbiesAreJoinable( false );
}

/*
========================
budSessionLocal::EndSessions
========================
*/
void budSessionLocal::EndSessions()
{
	if( GetPartyLobby().lobbyBackend != NULL )
	{
		GetPartyLobby().lobbyBackend->EndSession();
	}
	
	if( GetGameLobby().lobbyBackend != NULL )
	{
		GetGameLobby().lobbyBackend->EndSession();
	}
	
	SetLobbiesAreJoinable( true );
}

/*
========================
budSessionLocal::SetLobbiesAreJoinable
========================
*/
void budSessionLocal::SetLobbiesAreJoinable( bool joinable )
{
	// NOTE - We don't manipulate the joinable state when we are supporting join in progress
	// Lobbies will naturally be non searchable when there are no free slots
	if( GetPartyLobby().lobbyBackend != NULL && !MatchTypeIsJoinInProgress( GetPartyLobby().parms.matchFlags ) )
	{
		NET_VERBOSE_PRINT( "Party lobbyBackend SetIsJoinable: %d\n", joinable );
		GetPartyLobby().lobbyBackend->SetIsJoinable( joinable );
	}
	
	if( GetGameLobby().lobbyBackend != NULL && !MatchTypeIsJoinInProgress( GetGameLobby().parms.matchFlags ) )
	{
		GetGameLobby().lobbyBackend->SetIsJoinable( joinable );
		NET_VERBOSE_PRINT( "Game lobbyBackend SetIsJoinable: %d\n", joinable );
		
	}
}

/*
========================
budSessionLocal::MoveToMainMenu
========================
*/
void budSessionLocal::MoveToMainMenu()
{
	GetPartyLobby().Shutdown();
	GetGameLobby().Shutdown();
	GetGameStateLobby().Shutdown();
	SetState( STATE_IDLE );
}

/*
========================
budSessionLocal::HandleVoiceRestrictionDialog
========================
*/
void budSessionLocal::HandleVoiceRestrictionDialog()
{
	// don't bother complaining about voice restrictions when in a splitscreen lobby
	if( MatchTypeIsLocal( GetActivePlatformLobby()->parms.matchFlags ) )
	{
		return;
	}
	
	// Pop a dialog up the first time we are in a lobby and have voice chat restrictions due to account privileges
	if( voiceChat != NULL && voiceChat->IsRestrictedByPrivleges() && !hasShownVoiceRestrictionDialog )
	{
		hasShownVoiceRestrictionDialog = true;
	}
}

/*
========================
budSessionLocal::WaitOnLobbyCreate

Called from State_Create_And_Move_To_Party_Lobby and State_Create_And_Move_To_Game_Lobby and State_Create_And_Move_To_Game_State_Lobby.
This function will create the lobby, then wait for it to either succeed or fail.
========================
*/
bool budSessionLocal::WaitOnLobbyCreate( idLobby& lobby )
{

	assert( localState == STATE_CREATE_AND_MOVE_TO_PARTY_LOBBY || localState == STATE_CREATE_AND_MOVE_TO_GAME_LOBBY || localState == STATE_CREATE_AND_MOVE_TO_GAME_STATE_LOBBY );
	assert( connectType == CONNECT_FIND_OR_CREATE || connectType == CONNECT_NONE );
	
	if( lobby.GetState() == idLobby::STATE_FAILED )
	{
		NET_VERBOSE_PRINT( "NET: budSessionLocal::WaitOnLobbyCreate lobby.GetState() == idLobby::STATE_FAILED (%s)\n", lobby.GetLobbyName() );
		// If we failed to create a lobby, assume connection to backend service was lost
		MoveToMainMenu();
		return false;
	}
	
	if( DetectDisconnectFromService( true ) )
	{
		return false;
	}
	
	if( lobby.GetState() != idLobby::STATE_IDLE )
	{
		return false;		// Valid but busy
	}
	
	NET_VERBOSE_PRINT( "NET: budSessionLocal::WaitOnLobbyCreate SUCCESS (%s)\n", lobby.GetLobbyName() );
	
	return true;
}

/*
========================
budSessionLocal::DetectDisconnectFromService
Called from CreateMatch/CreatePartyLobby/FindOrCreateMatch state machines
========================
*/
bool budSessionLocal::DetectDisconnectFromService( bool cancelAndShowMsg )
{
	const int DETECT_SERVICE_DISCONNECT_TIMEOUT_IN_SECONDS = session->GetTitleStorageInt( "DETECT_SERVICE_DISCONNECT_TIMEOUT_IN_SECONDS", 30 );
	
	// If we are taking too long, cancel the connection
	if( DETECT_SERVICE_DISCONNECT_TIMEOUT_IN_SECONDS > 0 )
	{
		if( Sys_Milliseconds() - connectTime > 1000 * DETECT_SERVICE_DISCONNECT_TIMEOUT_IN_SECONDS )
		{
			NET_VERBOSE_PRINT( "NET: budSessionLocal::DetectDisconnectFromService timed out\n" );
			if( cancelAndShowMsg )
			{
				MoveToMainMenu();
			}
			
			return true;
		}
	}
	
	return false;
}

/*
========================
budSessionLocal::HandleConnectionFailed
Called anytime a connection fails, and does the right thing.
========================
*/
void budSessionLocal::HandleConnectionFailed( idLobby& lobby, bool wasFull )
{
	assert( localState == STATE_CONNECT_AND_MOVE_TO_PARTY || localState == STATE_CONNECT_AND_MOVE_TO_GAME || localState == STATE_CONNECT_AND_MOVE_TO_GAME_STATE );
	assert( connectType == CONNECT_FIND_OR_CREATE || connectType == CONNECT_DIRECT );
	bool canPlayOnline = true;
	
	// Check for online status (this is only a problem on the PS3 at the moment. The 360 LIVE system handles this for us
	if( GetSignInManager().GetMasterLocalUser() != NULL )
	{
		canPlayOnline = GetSignInManager().GetMasterLocalUser()->CanPlayOnline();
	}
	
	if( connectType == CONNECT_FIND_OR_CREATE )
	{
		// Clear the "Lobby was Full" dialog in case it's up
		// We only want to see this msg when doing a direct connect (CONNECT_DIRECT)
		
		assert( localState == STATE_CONNECT_AND_MOVE_TO_GAME || localState == STATE_CONNECT_AND_MOVE_TO_GAME_STATE );
		assert( lobby.lobbyType == idLobby::TYPE_GAME );
		if( !lobby.ConnectToNextSearchResult() )
		{
			CreateMatch( GetGameLobby().parms );		// Assume any time we are connecting to a game lobby, it is from a FindOrCreateMatch call, so create a match
		}
	}
	else if( connectType == CONNECT_DIRECT )
	{
		if( localState == STATE_CONNECT_AND_MOVE_TO_GAME && GetPartyLobby().IsPeer() )
		{
			int flags = GetPartyLobby().parms.matchFlags;
			
			if( MatchTypeIsOnline( flags ) && ( flags & MATCH_REQUIRE_PARTY_LOBBY ) && ( ( flags & MATCH_PARTY_INVITE_PLACEHOLDER ) == 0 ) )
			{
				// We get here when our party host told us to connect to a game, but the game didn't exist.
				// Just drop back to the party lobby and wait for further orders.
				SetState( STATE_PARTY_LOBBY_PEER );
				return;
			}
		}
		
		MoveToMainMenu();
	}
	else
	{
		// Shouldn't be possible, but just in case
		MoveToMainMenu();
	}
}

/*
========================
budSessionLocal::HandleConnectAndMoveToLobby
Called from State_Connect_And_Move_To_Party/State_Connect_And_Move_To_Game
========================
*/
bool budSessionLocal::HandleConnectAndMoveToLobby( idLobby& lobby )
{
	assert( localState == STATE_CONNECT_AND_MOVE_TO_PARTY || localState == STATE_CONNECT_AND_MOVE_TO_GAME || localState == STATE_CONNECT_AND_MOVE_TO_GAME_STATE );
	assert( connectType == CONNECT_FIND_OR_CREATE || connectType == CONNECT_DIRECT );
	
	if( lobby.GetState() == idLobby::STATE_FAILED )
	{
		// If we get here, we were trying to connect to a lobby (from state State_Connect_And_Move_To_Party/State_Connect_And_Move_To_Game)
		HandleConnectionFailed( lobby, false );
		return true;
	}
	
	if( lobby.GetState() != idLobby::STATE_IDLE )
	{
		return HandlePackets();	// Valid but busy
	}
	
	assert( !GetPartyLobby().waitForPartyOk );
	
	//
	// Past this point, we've connected to the lobby
	//
	
	// If we are connecting to a game lobby, see if we need to keep waiting as either a host or peer while we're confirming all party members made it
	if( lobby.lobbyType == idLobby::TYPE_GAME )
	{
		if( GetPartyLobby().IsHost() )
		{
			// As a host, wait until all party members make it
			assert( !GetGameLobby().waitForPartyOk );
			
			const int timeoutMs = session->GetTitleStorageInt( "net_connectTimeoutInSeconds", net_connectTimeoutInSeconds.GetInteger() ) * 1000;
			
			if( timeoutMs != 0 && Sys_Milliseconds() - lobby.helloStartTime > timeoutMs )
			{
				// Took too long, move to next result, or create a game instead
				HandleConnectionFailed( lobby, false );
				return true;
			}
			
			int numUsersIn = 0;
			
			for( int i = 0; i < GetPartyLobby().GetNumLobbyUsers(); i++ )
			{
			
				if( net_testPartyMemberConnectFail.GetInteger() == i )
				{
					continue;
				}
				
				bool foundUser = false;
				
				lobbyUser_t* partyUser = GetPartyLobby().GetLobbyUser( i );
				
				for( int j = 0; j < GetGameLobby().GetNumLobbyUsers(); j++ )
				{
					lobbyUser_t* gameUser = GetGameLobby().GetLobbyUser( j );
					
					if( GetGameLobby().IsSessionUserLocal( gameUser ) || gameUser->address.Compare( partyUser->address, true ) )
					{
						numUsersIn++;
						foundUser = true;
						break;
					}
				}
				
				assert( !GetPartyLobby().IsSessionUserIndexLocal( i ) || foundUser );
			}
			
			if( numUsersIn != GetPartyLobby().GetNumLobbyUsers() )
			{
				return HandlePackets();		// All users not in, keep waiting until all user make it, or we time out
			}
			
			NET_VERBOSE_PRINT( "NET: All party members made it into the game lobby.\n" );
			
			// Let all the party members know everyone made it, and it's ok to stay at this server
			for( int i = 0; i < GetPartyLobby().peers.Num(); i++ )
			{
				if( GetPartyLobby().peers[ i ].IsConnected() )
				{
					GetPartyLobby().QueueReliableMessage( i, idLobby::RELIABLE_PARTY_CONNECT_OK );
				}
			}
		}
		else
		{
			if( !verify( lobby.host != -1 ) )
			{
				MoveToMainMenu();
				connectType = CONNECT_NONE;
				return false;
			}
			
			// As a peer, wait for server to tell us everyone made it
			if( GetGameLobby().waitForPartyOk )
			{
				const int timeoutMs = session->GetTitleStorageInt( "net_connectTimeoutInSeconds", net_connectTimeoutInSeconds.GetInteger() ) * 1000;
				
				if( timeoutMs != 0 && Sys_Milliseconds() - lobby.helloStartTime > timeoutMs )
				{
					GetGameLobby().waitForPartyOk = false;		// Just connect to this game lobby if we haven't heard from the party host for the entire timeout duration
				}
			}
			
			if( GetGameLobby().waitForPartyOk )
			{
				return HandlePackets();			// Waiting on party host to tell us everyone made it
			}
		}
	}
	
	// Success
	switch( lobby.lobbyType )
	{
		case idLobby::TYPE_PARTY:
			SetState( STATE_PARTY_LOBBY_PEER );
			break;
		case idLobby::TYPE_GAME:
			SetState( STATE_GAME_LOBBY_PEER );
			break;
		case idLobby::TYPE_GAME_STATE:
			waitingOnGameStateMembersToJoinTime = Sys_Milliseconds();
			// As a host of the game lobby, it's our duty to notify our members to also join this game state lobby
			GetGameLobby().SendMembersToLobby( GetGameStateLobby(), false );
			SetState( STATE_GAME_STATE_LOBBY_PEER );
			break;
	}
	
	connectType = CONNECT_NONE;
	
	return false;
}

/*
========================
budSessionLocal::State_Create_And_Move_To_Party_Lobby
========================
*/
bool budSessionLocal::State_Create_And_Move_To_Party_Lobby()
{
	if( WaitOnLobbyCreate( GetPartyLobby() ) )
	{
	
		if( GetPartyLobby().parms.matchFlags & MATCH_PARTY_INVITE_PLACEHOLDER )
		{
			// If this party lobby was for a placeholder, continue on with either finding or creating a game lobby
			if( connectType == CONNECT_FIND_OR_CREATE )
			{
				FindOrCreateMatch( GetPartyLobby().parms );
				return true;
			}
			else if( connectType == CONNECT_NONE )
			{
				CreateMatch( GetPartyLobby().parms );
				return true;
			}
		}
		
		// Success
		SetState( STATE_PARTY_LOBBY_HOST );
		
		return true;
	}
	
	return HandlePackets();		// Valid but busy
}

/*
========================
budSessionLocal::State_Create_And_Move_To_Game_Lobby
========================
*/
bool budSessionLocal::State_Create_And_Move_To_Game_Lobby()
{

	if( WaitOnLobbyCreate( GetGameLobby() ) )
	{
		// Success
		SetState( STATE_GAME_LOBBY_HOST );
		
		// Now that we've created our game lobby, send our own party users to it
		// NOTE - We pass in false to wait on party members since we are the host, and we know they can connect to us
		GetPartyLobby().SendMembersToLobby( GetGameLobby(), false );
		return true;
	}
	
	return false;
}

/*
========================
budSessionLocal::State_Create_And_Move_To_Game_State_Lobby
========================
*/
bool budSessionLocal::State_Create_And_Move_To_Game_State_Lobby()
{

	if( WaitOnLobbyCreate( GetGameStateLobby() ) )
	{
		// Success
		SetState( STATE_GAME_STATE_LOBBY_HOST );
		
		// Now that we've created our game state lobby, send our own game users to it
		// NOTE - We pass in false to wait on party members since we are the host, and we know they can connect to us
		GetGameLobby().SendMembersToLobby( GetGameStateLobby(), false );
		
		// If we are the host of a game lobby, we know we are not using dedicated servers, so we want to start the match immediately
		// as soon as we detect all users have connected.
		if( GetGameLobby().IsHost() )
		{
			waitingOnGameStateMembersToJoinTime = Sys_Milliseconds();
		}
		
		return true;
	}
	
	return false;
}

/*
========================
budSessionLocal::State_Find_Or_Create_Match
========================
*/
bool budSessionLocal::State_Find_Or_Create_Match()
{
	assert( connectType == CONNECT_FIND_OR_CREATE );
	
	if( GetGameLobby().GetState() == idLobby::STATE_FAILED )
	{
		// Failed to find any games.  Create one instead (we're assuming this always gets called from FindOrCreateMatch
		CreateMatch( GetGameLobby().parms );
		return true;
	}
	
	if( DetectDisconnectFromService( true ) )
	{
		return false;
	}
	
	if( GetGameLobby().GetState() != idLobby::STATE_IDLE )
	{
		return HandlePackets();		// Valid but busy
	}
	
	// Done searching, connect to the first search result
	if( !GetGameLobby().ConnectToNextSearchResult() )
	{
		// Failed to find any games.  Create one instead (we're assuming this always gets called from FindOrCreateMatch
		CreateMatch( GetGameLobby().parms );
		return true;
	}
	
	SetState( STATE_CONNECT_AND_MOVE_TO_GAME );
	
	return true;
}

/*
========================
budSessionLocal::State_Connect_And_Move_To_Party
========================
*/
bool budSessionLocal::State_Connect_And_Move_To_Party()
{
	return HandleConnectAndMoveToLobby( GetPartyLobby() );
}

/*
========================
budSessionLocal::State_Connect_And_Move_To_Game
========================
*/
bool budSessionLocal::State_Connect_And_Move_To_Game()
{
	return HandleConnectAndMoveToLobby( GetGameLobby() );
}

/*
========================
budSessionLocal::State_Connect_And_Move_To_Game_State
========================
*/
bool budSessionLocal::State_Connect_And_Move_To_Game_State()
{
	return HandleConnectAndMoveToLobby( GetGameStateLobby() );
}

/*
========================
budSessionLocal::State_InGame
========================
*/
bool budSessionLocal::State_InGame()
{
	return HandlePackets();
}

/*
========================
budSessionLocal::State_Loading
========================
*/
bool budSessionLocal::State_Loading()
{

	HandlePackets();
	
	if( !GetActingGameStateLobby().loaded )
	{
		return false;
	}
	
	SetVoiceGroupsToTeams();
	
	if( GetActingGameStateLobby().IsHost() )
	{
		bool everyoneLoaded = true;
		for( int p = 0; p < GetActingGameStateLobby().peers.Num(); p++ )
		{
			idLobby::peer_t& peer = GetActingGameStateLobby().peers[p];
			
			if( !peer.IsConnected() )
			{
				continue;		// We don't care about peers that aren't connected as a game session
			}
			
			if( !peer.loaded )
			{
				everyoneLoaded = false;
				continue;		// Don't waste time sending resources to a peer who hasn't loaded the map yet
			}
			
			if( GetActingGameStateLobby().SendResources( p ) )
			{
				everyoneLoaded = false;
				
				// if client is taking a LONG time to load up - give them the boot: they're just holding up the lunch line. Useful for loose assets playtesting.
				int time = Sys_Milliseconds();
				int maxLoadTime = net_maxLoadResourcesTimeInSeconds.GetInteger();
				if( maxLoadTime > 0 && peer.startResourceLoadTime + SEC2MS( maxLoadTime ) < time )
				{
					NET_VERBOSERESOURCE_PRINT( "NET: dropping client %i - %s because they took too long to load resources.\n Check 'net_maxLoadResourcesTimeInSeconds' to adjust the time allowed.\n", p, GetPeerName( p ) );
					GetActingGameStateLobby().DisconnectPeerFromSession( p );
					continue;
				}
			}
		}
		if( !everyoneLoaded )
		{
			return false;
		}
	}
	else
	{
		// not sure how we got there, but we won't be receiving anything that could get us out of this state anymore
		// possible step towards fixing the join stalling/disconnect problems
		if( GetActingGameStateLobby().peers.Num() == 0 )
		{
			NET_VERBOSE_PRINT( "NET: no peers in budSessionLocal::State_Loading - giving up\n" );
			MoveToMainMenu();
		}
		// need at least a peer with a real connection
		bool haveOneGoodPeer = false;
		for( int p = 0; p < GetActingGameStateLobby().peers.Num(); p++ )
		{
			if( GetActingGameStateLobby().peers[p].IsConnected() )
			{
				haveOneGoodPeer = true;
				break;
			}
		}
		if( !haveOneGoodPeer )
		{
			NET_VERBOSE_PRINT( "NET: no good peers in budSessionLocal::State_Loading - giving up\n" );
			MoveToMainMenu();
		}
		
		return false;
	}
	
	GetActingGameStateLobby().ResetBandwidthStats();
	
	// if we got here then we're the host and everyone indicated map load finished
	NET_VERBOSE_PRINT( "NET: (loading) Starting Game\n" );
	SetState( STATE_INGAME );		// NOTE - Only the host is in-game at this point, all peers will start becoming in-game when they receive their first full snap
	return true;
}

/*
========================
budSessionLocal::State_Busy
========================
*/
bool budSessionLocal::State_Busy()
{
	idLobby* activeLobby = GetActivePlatformLobby();
	if( activeLobby == NULL )
	{
		libBud::Warning( "No active session lobby when budSessionLocal::State_Busy called" );
		return false;
	}
	
	if( activeLobby->bandwidthChallengeFinished )
	{
		// Start loading
		NET_VERBOSE_PRINT( "NET: Bandwidth test finished - Start loading\n" );
		StartLoading();
	}
	
	return HandlePackets();
}

/*
========================
budSessionLocal::VerifySnapshotInitialState
========================
*/
void budSessionLocal::VerifySnapshotInitialState()
{
	// Verify that snapshot state is reset
	for( int p = 0; p < GetActingGameStateLobby().peers.Num(); p++ )
	{
		if( !GetActingGameStateLobby().peers[p].IsConnected() )
		{
			assert( GetActingGameStateLobby().peers[p].snapProc == NULL );
			continue;
		}
		
		assert( GetActingGameStateLobby().peers[p].snapProc != NULL );
		
		if( !verify( GetActingGameStateLobby().peers[p].needToSubmitPendingSnap == false ) )
		{
			libBud::Error( "Invalid needToSubmitPendingSnap state\n" );
		}
		if( !verify( GetActingGameStateLobby().peers[p].snapProc->HasPendingSnap() == false ) )
		{
			libBud::Error( "Invalid HasPendingSnap state\n" );
		}
		if( !verify( GetActingGameStateLobby().peers[p].snapProc->GetSnapSequence() == budSnapShotProcessor::INITIAL_SNAP_SEQUENCE ) )
		{
			libBud::Error( "Invalid INITIAL_SNAP_SEQUENCE state %d for peer %d \n", GetActingGameStateLobby().peers[p].snapProc->GetSnapSequence(), p );
		}
		if( !verify( GetActingGameStateLobby().peers[p].snapProc->GetBaseSequence() == -1 ) )
		{
			libBud::Error( "Invalid GetBaseSequence state\n" );
		}
	}
}

/*
========================
budSessionLocal::State_Party_Lobby_Host
========================
*/
bool budSessionLocal::State_Party_Lobby_Host()
{
	HandleVoiceRestrictionDialog();
	return HandlePackets();
}

/*
========================
budSessionLocal::State_Game_Lobby_Host
========================
*/
bool budSessionLocal::State_Game_Lobby_Host()
{
	HandleVoiceRestrictionDialog();
	return HandlePackets();
}

/*
========================
budSessionLocal::State_Game_State_Lobby_Host
========================
*/
bool budSessionLocal::State_Game_State_Lobby_Host()
{
	HandleVoiceRestrictionDialog();
	
	if( waitingOnGameStateMembersToLeaveTime != 0 )
	{
	
		const int MAX_LEAVE_WAIT_TIME_IN_SECONDS = 5;
		
		const bool forceDisconnectMembers = ( Sys_Milliseconds() - waitingOnGameStateMembersToLeaveTime ) > MAX_LEAVE_WAIT_TIME_IN_SECONDS * 1000;
		
		// Check to see if all peers have finally left
		if( GetGameStateLobby().GetNumConnectedPeers() == 0 || forceDisconnectMembers )
		{
		
			//
			// All peers left, we can stop waiting
			//
			
			waitingOnGameStateMembersToLeaveTime = 0;
			
			assert( !GetGameLobby().IsPeer() );
			
			if( GetGameLobby().IsHost() )
			{
				// If we aren't a dedicated game state host, then drop back to the game lobby as host
				GetGameStateLobby().Shutdown();
				SetState( STATE_GAME_LOBBY_HOST );
			}
			else
			{
				// A dedicated game state host will remain in State_Game_State_Lobby_Host mode while waiting for another set of users to join
				// DEDICATED_SERVER_FIXME: Notify master server we can server another game now
				GetGameStateLobby().DisconnectAllPeers();
			}
		}
	}
	else
	{
		// When all the players from the game lobby are in the game state lobby, StartLoading
		if( GetGameLobby().IsHost() )
		{
			if( GetGameStateLobby().GetNumLobbyUsers() == GetGameLobby().GetNumLobbyUsers() )
			{
				waitingOnGameStateMembersToJoinTime = 0;
				StartLoading();
			}
		}
		else
		{
			// The dedicated server host relies on the game host to say when all users are in
			if( GetGameStateLobby().startLoadingFromHost )
			{
				GetGameStateLobby().startLoadingFromHost = false;
				StartLoading();
			}
		}
	}
	
	return HandlePackets();
}

/*
========================
budSessionLocal::State_Party_Lobby_Peer
========================
*/
bool budSessionLocal::State_Party_Lobby_Peer()
{
	HandleVoiceRestrictionDialog();
	return HandlePackets();
}

/*
========================
budSessionLocal::State_Game_Lobby_Peer
========================
*/
bool budSessionLocal::State_Game_Lobby_Peer()
{
	HandleVoiceRestrictionDialog();
	bool saving = false;
	budPlayerProfile* profile = GetProfileFromMasterLocalUser();
	if( profile != NULL && ( profile->GetState() == budPlayerProfile::SAVING || profile->GetRequestedState() == budPlayerProfile::SAVE_REQUESTED ) )
	{
		saving = true;
	}
	
	if( GetActingGameStateLobby().startLoadingFromHost && !saving )
	{
		
		VerifySnapshotInitialState();
		
		// Set loading flag back to false
		GetActingGameStateLobby().startLoadingFromHost = false;
		
		// Set state to loading
		SetState( STATE_LOADING );
		
		loadingID++;
		
		return true;
	}
	
	return HandlePackets();
}

/*
========================
budSessionLocal::State_Game_State_Lobby_Peer
========================
*/
bool budSessionLocal::State_Game_State_Lobby_Peer()
{
	// We are in charge of telling the dedicated host that all our members are in
	if( GetGameLobby().IsHost() && waitingOnGameStateMembersToJoinTime != 0 )
	{
		int foundMembers = 0;
		
		for( int i = 0; i < GetGameLobby().GetNumLobbyUsers(); i++ )
		{
			if( GetGameStateLobby().GetLobbyUserByID( GetGameLobby().GetLobbyUser( i )->lobbyUserID, true ) != NULL )
			{
				foundMembers++;
			}
		}
		
		// Give all of our game members 10 seconds to join, otherwise start without them
		const int MAX_JOIN_WAIT_TIME_IN_SECONDS = 10;
		
		const bool forceStart = ( Sys_Milliseconds() - waitingOnGameStateMembersToJoinTime ) > MAX_JOIN_WAIT_TIME_IN_SECONDS * 1000;
		
		if( foundMembers == GetGameLobby().GetNumLobbyUsers() || forceStart )
		{
			byte buffer[ idPacketProcessor::MAX_PACKET_SIZE ];
			
			budBitMsg msg( buffer, sizeof( buffer ) );
			
			// Write match parameters to the game state host, and tell him to start
			GetGameLobby().parms.Write( msg );
			
			// Tell the game state lobby host we are ready
			GetGameStateLobby().QueueReliableMessage( GetGameStateLobby().host, idLobby::RELIABLE_START_MATCH_GAME_LOBBY_HOST, msg.GetReadData(), msg.GetSize() );
			
			waitingOnGameStateMembersToJoinTime = 0;
		}
	}
	
	return State_Game_Lobby_Peer();
}

/*
========================
budSessionLocal::~budSession
========================
*/
budSession::~budSession()
{
	delete signInManager;
	signInManager = NULL;
	delete saveGameManager;
	saveGameManager = NULL;
	delete dedicatedServerSearch;
	dedicatedServerSearch = NULL;
}

budCVar net_verbose( "net_verbose", "0", CVAR_BOOL | CVAR_NOCHEAT, "Print a bunch of message about the network session" );
budCVar net_verboseResource( "net_verboseResource", "0", CVAR_BOOL, "Prints a bunch of message about network resources" );
budCVar net_verboseReliable( "net_verboseReliable", "0", CVAR_BOOL, "Prints the more spammy messages about reliable network msgs" );
budCVar si_splitscreen( "si_splitscreen", "0", CVAR_INTEGER, "force splitscreen" );

budCVar net_forceLatency( "net_forceLatency", "0", CVAR_INTEGER, "Simulate network latency (milliseconds round trip time - applied equally on the receive and on the send)" );
budCVar net_forceDrop( "net_forceDrop", "0", CVAR_INTEGER, "Percentage chance of simulated network packet loss" );
budCVar net_forceUpstream( "net_forceUpstream", "0", CVAR_FLOAT, "Force a maximum upstream in kB/s (256kbps <-> 32kB/s)" ); // I would much rather deal in kbps but most of the code is written in bytes ..
budCVar net_forceUpstreamQueue( "net_forceUpstreamQueue", "64", CVAR_INTEGER, "How much data is queued when enforcing upstream (in kB)" );
budCVar net_verboseSimulatedTraffic( "net_verboseSimulatedTraffic", "0", CVAR_BOOL, "Print some stats about simulated traffic (net_force* cvars)" );

/*
========================
budSessionLocal::Initialize
========================
*/
void budSessionLocal::Initialize()
{
}

/*
========================
budSessionLocal::Shutdown
========================
*/
void budSessionLocal::Shutdown()
{
}

/*
========================
budSession interface semi-common between platforms (#ifdef's in sys_session_local.cpp)
========================
*/

budCVar com_deviceZeroOverride( "com_deviceZeroOverride", "-1", CVAR_INTEGER, "change input routing for device 0 to poll a different device" );
budCVar mp_bot_input_override( "mp_bot_input_override", "-1", CVAR_INTEGER, "Override local input routing for bot control" );

/*
========================
budSessionLocal::GetInputRouting
This function sets up inputRouting to be a mapping from inputDevice index to session user index.
========================
*/
int budSessionLocal::GetInputRouting( int inputRouting[ MAX_INPUT_DEVICES ] )
{

	int numLocalUsers = 0;
	for( int i = 0; i < MAX_INPUT_DEVICES; i++ )
	{
		inputRouting[i] = -1;
	}
	
	for( int i = 0; i < GetActingGameStateLobby().GetNumLobbyUsers(); i++ )
	{
		if( GetActingGameStateLobby().IsSessionUserIndexLocal( i ) )
		{
		
			// Find the local user that this session user maps to
			const idLocalUser* localUser = GetActingGameStateLobby().GetLocalUserFromLobbyUserIndex( i );
			
			if( localUser != NULL )
			{
				int localDevice = localUser->GetInputDevice();
				if( localDevice == 0 && com_deviceZeroOverride.GetInteger() > 0 )
				{
					localDevice = com_deviceZeroOverride.GetInteger();
				}
				assert( localDevice < MAX_INPUT_DEVICES );
				// Route the input device that this local user is mapped to
				assert( inputRouting[localDevice] == -1 );	// Make sure to only initialize each entry once
				inputRouting[localDevice] = i;
				
				if( mp_bot_input_override.GetInteger() >= 0 )
				{
					inputRouting[localDevice] = mp_bot_input_override.GetInteger();
				}
				
				numLocalUsers++;
			}
		}
	}
	
	// For testing swapping controllers
	if( si_splitscreen.GetInteger() == 2 && numLocalUsers == 2 )
	{
		SwapValues( inputRouting[0], inputRouting[1] );
	}
	
	return numLocalUsers;
}

/*
========================
budSessionLocal::EndMatch
EndMatch is meant for the host to cleanly end a match and return to the lobby page
========================
*/
void budSessionLocal::EndMatch( bool premature /*=false*/ )
{
	if( verify( GetActingGameStateLobby().IsHost() ) )
	{
		// Host quits back to game lobby, and will notify peers internally to do the same
		EndMatchInternal( premature );
	}
}

/*
========================
budSessionLocal::EndMatch
this is for when the game is over before we go back to lobby. Need this incase the host leaves during this time
========================
*/
void budSessionLocal::MatchFinished( )
{
	if( verify( GetActingGameStateLobby().IsHost() ) )
	{
		// host is putting up end game stats make sure other peers know and clear migration data
		MatchFinishedInternal();
	}
}

/*
========================
budSessionLocal::QuitMatch
QuitMatch is considered a premature ending of a match, and does the right thing depending on whether the host or peer is quitting
========================
*/
void budSessionLocal::QuitMatch()
{
	if( GetActingGameStateLobby().IsHost() && !MatchTypeIsRanked( GetActingGameStateLobby().parms.matchFlags ) )
	{
		EndMatch( true );		// When host quits private match, takes members back to game lobby
	}
	else
	{
		// Quitting a public match (or not being a host) before it ends takes you to an empty party lobby
		CreatePartyLobby( GetActingGameStateLobby().parms );
	}
}

/*
========================
budSessionLocal::QuitMatchToTitle
QuitMatchToTitle will forcefully quit the match and return to the title screen.
========================
*/
void budSessionLocal::QuitMatchToTitle()
{
	MoveToMainMenu();
}

/*
========================
budSessionLocal::ClearMigrationState
========================
*/
void budSessionLocal::ClearMigrationState()
{
	// We are ending the match without migration, so clear that state
	GetPartyLobby().ResetAllMigrationState();
	GetGameLobby().ResetAllMigrationState();
}

/*
========================
budSessionLocal::EndMatchInternal
========================
*/
void budSessionLocal::EndMatchInternal( bool premature/*=false*/ )
{
	assert( GetGameStateLobby().IsLobbyActive() == net_useGameStateLobby.GetBool() );
	
	ClearVoiceGroups();
	
	for( int p = 0; p < GetActingGameStateLobby().peers.Num(); p++ )
	{
		// If we are the host, increment the session ID.  The client will use a rolling check to catch it
		if( GetActingGameStateLobby().IsHost() )
		{
			if( GetActingGameStateLobby().peers[p].IsConnected() )
			{
				if( GetActingGameStateLobby().peers[p].packetProc != NULL )
				{
					GetActingGameStateLobby().peers[p].packetProc->VerifyEmptyReliableQueue( idLobby::RELIABLE_GAME_DATA, idLobby::RELIABLE_DUMMY_MSG );
				}
				GetActingGameStateLobby().peers[p].sessionID = GetActingGameStateLobby().IncrementSessionID( GetActingGameStateLobby().peers[p].sessionID );
			}
		}
		GetActingGameStateLobby().peers[p].ResetMatchData();
	}
	
	GetActingGameStateLobby().snapDeltaAckQueue.Clear();
	
	GetActingGameStateLobby().loaded	= false;
	
	gameLobbyWasCoalesced	= false;		// Reset this back to false.  We use this so the lobby code doesn't randomly choose a map when we coalesce
	numFullSnapsReceived	= 0;
	
	ClearMigrationState();
	
	if( GetActingGameStateLobby().IsLobbyActive() && ( GetActingGameStateLobby().GetMatchParms().matchFlags & MATCH_REQUIRE_PARTY_LOBBY ) )
	{
		// All peers need to remove disconnected users to stay in sync
		GetActingGameStateLobby().CompactDisconnectedUsers();
		
		// Go back to the game lobby
		if( GetActingGameStateLobby().IsHost() )
		{
			// We want the game state host to go back to STATE_GAME_STATE_LOBBY_HOST, so he can wait on all his game state peers to leave
			SetState( GetGameStateLobby().IsHost() ? STATE_GAME_STATE_LOBBY_HOST : STATE_GAME_LOBBY_HOST );		// We want the dedicated host to go back to STATE_GAME_STATE_LOBBY_HOST
		}
		else
		{
			SetState( STATE_GAME_LOBBY_PEER );
		}
	}
	else
	{
		SetState( STATE_IDLE );
	}
	
	if( GetActingGameStateLobby().IsHost() )
	{
		// Send a reliable msg to all peers to also "EndMatch"
		for( int p = 0; p < GetActingGameStateLobby().peers.Num(); p++ )
		{
			GetActingGameStateLobby().QueueReliableMessage( p, premature ? idLobby::RELIABLE_ENDMATCH_PREMATURE : idLobby::RELIABLE_ENDMATCH );
		}
	}
	else if( premature )
	{
		// Notify client that host left early and thats why we are back in the lobby
		const bool stats = MatchTypeHasStats( GetActingGameStateLobby().GetMatchParms().matchFlags ) && ( GetFlushedStats() == false );
	}
	
	if( GetGameStateLobby().IsLobbyActive() )
	{
		if( GetGameStateLobby().IsHost() )
		{
			// As a game state host, keep the lobby around, so we can make sure we know when everyone leaves (which means they got the reliable msg to EndMatch)
			waitingOnGameStateMembersToLeaveTime = Sys_Milliseconds();
		}
		else if( GetGameStateLobby().IsPeer() )
		{
			// Game state lobby peers should disconnect now
			GetGameStateLobby().Shutdown();
		}
	}
}

/*
========================
budSessionLocal::MatchFinishedInternal
========================
*/
void budSessionLocal::MatchFinishedInternal()
{
	ClearMigrationState();
	
	if( GetActingGameStateLobby().IsHost() )
	{
		// Send a reliable msg to all peers to also "EndMatch"
		for( int p = 0; p < GetActingGameStateLobby().peers.Num(); p++ )
		{
			GetActingGameStateLobby().QueueReliableMessage( p, idLobby::RELIABLE_MATCHFINISHED );
		}
	}
}

/*
========================
budSessionLocal::EndMatchForMigration
========================
*/
void budSessionLocal::EndMatchForMigration()
{
	ClearVoiceGroups();
}

/*
========================
budSessionLocal::ShouldHavePartyLobby
========================
*/
bool budSessionLocal::ShouldHavePartyLobby()
{
	if( GetActivePlatformLobby() == NULL )
	{
		return false;
	}
	
	budMatchParameters& parms = GetActivePlatformLobby()->parms;
	
	int flags = parms.matchFlags;
	
	// Don't we always have a party lobby if we're online? At least in Doom 3?
	return MatchTypeIsOnline( flags ) && ( ( flags & MATCH_PARTY_INVITE_PLACEHOLDER ) == 0 );
}

/*
========================
budSessionLocal::ValidateLobbies
Determines if any of the session instances need to become the host
========================
*/
void budSessionLocal::ValidateLobbies()
{
	if( localState == STATE_PRESS_START || localState == STATE_IDLE )
	{
		// At press start or main menu, don't do anything
		return;
	}
	
	if( GetActivePlatformLobby() == NULL )
	{
		// If we're in between lobbies, don't do anything yet (the state transitioning code will handle error cases)
		return;
	}
	
	// Validate lobbies that should be alive and active
	if( ShouldHavePartyLobby() && GetState() >= budSession::PARTY_LOBBY )
	{
		ValidateLobby( GetPartyLobby() );
	}
	if( GetState() >= budSession::GAME_LOBBY && !net_headlessServer.GetBool() )
	{
		ValidateLobby( GetGameLobby() );
	}
}

/*
========================
budSessionLocal::ValidateLobby
========================
*/
void budSessionLocal::ValidateLobby( idLobby& lobby )
{
	if( lobby.lobbyBackend == NULL || lobby.lobbyBackend->GetState() == idLobbyBackend::STATE_FAILED || lobby.GetState() == idLobby::STATE_FAILED )
	{
		NET_VERBOSE_PRINT( "NET: ValidateLobby: FAILED (lobbyType = %i, state = %s)\n", lobby.lobbyType, stateToString[ localState ] );
		if( lobby.failedReason == idLobby::FAILED_MIGRATION_CONNECT_FAILED || lobby.failedReason == idLobby::FAILED_CONNECT_FAILED )
		{
			MoveToMainMenu();
		}
		else
		{
			// If the lobbyBackend goes bad under our feet for no known reason, assume we lost connection to the back end service
			MoveToMainMenu();
		}
	}
}

/*
========================
budSessionLocal::Pump
========================
*/
void budSessionLocal::Pump()
{
	SCOPED_PROFILE_EVENT( "Session::Pump" );
	
	static int lastPumpTime = -1;
	
	const int time					= Sys_Milliseconds();
	const int elapsedPumpSeconds	= ( time - lastPumpTime ) / 1000;
	
	if( lastPumpTime != -1 && elapsedPumpSeconds > 2 )
	{
		libBud::Warning( "budSessionLocal::Pump was not called for %i seconds", elapsedPumpSeconds );
	}
	
	lastPumpTime = time;
	
	if( net_migrateHost.GetInteger() >= 0 )
	{
		if( net_migrateHost.GetInteger() <= 2 )
		{
			if( net_migrateHost.GetInteger() == 0 )
			{
				GetPartyLobby().PickNewHost( true, true );
			}
			else
			{
				GetGameLobby().PickNewHost( true, true );
			}
		}
		else
		{
			GetPartyLobby().PickNewHost( true, true );
			GetGameLobby().PickNewHost( true, true );
		}
		net_migrateHost.SetInteger( -1 );
	}
	
	PlatformPump();
	
	// Send any voice packets if it's time
	SendVoiceAudio();
	
	bool shouldContinue = true;
	
	while( shouldContinue )
	{
		// Each iteration, validate the session instances
		ValidateLobbies();
		
		// Pump state
		shouldContinue = HandleState();
		
		// Pump lobbies
		PumpLobbies();
	}
	
	if( GetPartyLobby().lobbyBackend != NULL )
	{
		// Make sure game properties aren't set on the lobbyBackend if we aren't in a game lobby.
		// This is so we show up properly in search results in Play with Friends option
		GetPartyLobby().lobbyBackend->SetInGame( GetGameLobby().IsLobbyActive() );
		
		// Temp location
		UpdateMasterUserHeadsetState();
	}
	
	// Do some last minute checks, make sure everything about the current state and lobbyBackend state is valid, otherwise, take action
	ValidateLobbies();
	
	GetActingGameStateLobby().UpdateSnaps();
	
	idLobby* activeLobby = GetActivePlatformLobby();
	
	// Pump pings for the active lobby
	if( activeLobby != NULL )
	{
		activeLobby->PumpPings();
	}
	
	// Pump packet processing for all lobbies
	GetPartyLobby().PumpPackets();
	GetGameLobby().PumpPackets();
	GetGameStateLobby().PumpPackets();
	
	int currentTime = Sys_Milliseconds();
	
	const int SHOW_MIGRATING_INFO_IN_SECONDS = 3;	// Show for at least this long once we start showing it
	
	if( ShouldShowMigratingDialog() )
	{
		showMigratingInfoStartTime = currentTime;
	}
	
	// Update possible pending invite
	UpdatePendingInvite();
	
	// Check to see if we should coalesce the lobby
	if( nextGameCoalesceTime != 0 )
	{
	
		if( GetGameLobby().IsLobbyActive() &&
				GetGameLobby().IsHost() &&
				GetState() == budSession::GAME_LOBBY &&
				GetPartyLobby().GetNumLobbyUsers() <= 1 &&
				GetGameLobby().GetNumLobbyUsers() == 1 &&
				MatchTypeIsRanked( GetGameLobby().parms.matchFlags ) &&
				Sys_Milliseconds() > nextGameCoalesceTime )
		{
		
			// If the player doesn't care about the mode or map,
			// make sure the search is broadened.
			budMatchParameters newGameParms = GetGameLobby().parms;
			newGameParms.gameMap = GAME_MAP_RANDOM;
			
			// Assume that if the party lobby's mode is random,
			// the player chose "Quick Match" and doesn't care about the mode.
			// If the player chose "Find Match" and a specific mode,
			// the party lobby mode will be set to non-random.
			if( GetPartyLobby().parms.gameMode == GAME_MODE_RANDOM )
			{
				newGameParms.gameMode = GAME_MODE_RANDOM;
			}
			
			FindOrCreateMatch( newGameParms );
			
			gameLobbyWasCoalesced	= true;		// Remember that this round was coalesced.  We so this so main menu doesn't randomize the map, which looks odd
			nextGameCoalesceTime	= 0;
		}
	}
}

/*
========================
budSessionLocal::ProcessSnapAckQueue
========================
*/
void budSessionLocal::ProcessSnapAckQueue()
{
	if( GetActingGameStateLobby().IsLobbyActive() )
	{
		GetActingGameStateLobby().ProcessSnapAckQueue();
	}
}

/*
========================
budSessionLocal::UpdatePendingInvite
========================
*/
void budSessionLocal::UpdatePendingInvite()
{
	if( pendingInviteMode == PENDING_INVITE_NONE )
	{
		return;		// No pending invite
	}
	
	idLocalUser* masterLocalUser = signInManager->GetMasterLocalUser();
	
	if( masterLocalUser == NULL && signInManager->IsDeviceBeingRegistered( pendingInviteDevice ) )
	{
		libBud::Printf( "masterLocalUser == NULL\n" );
		return;		// Waiting on master to sign in to continue with invite
	}
	
	const bool wasFromInvite = pendingInviteMode == PENDING_INVITE_WAITING;	// Remember if this was a real invite, or a self invitation (matters when lobby is invite only)
	
	// At this point, the invitee should be ready
	pendingInviteMode = PENDING_INVITE_NONE;
	
	if( masterLocalUser == NULL || masterLocalUser->GetInputDevice() != pendingInviteDevice || !masterLocalUser->IsOnline() )
	{
		libBud::Printf( "ignoring invite - master local user is not setup properly\n" );
		return; // If there is no master, if the invitee is not online, or different than the current master, then ignore invite
	}
	
	// Everything looks good, let's join the party
	ConnectAndMoveToLobby( GetPartyLobby(), pendingInviteConnectInfo, wasFromInvite );
}

/*
========================
budSessionLocal::HandleState
========================
*/
bool budSessionLocal::HandleState()
{
	// Handle individual lobby states
	GetPartyLobby().Pump();
	GetGameLobby().Pump();
	GetGameStateLobby().Pump();
	
	// Let IsHost be authoritative on the qualification of peer/host state types
	if( GetPartyLobby().IsHost() && localState == STATE_PARTY_LOBBY_PEER )
	{
		SetState( STATE_PARTY_LOBBY_HOST );
	}
	else if( GetPartyLobby().IsPeer() && localState == STATE_PARTY_LOBBY_HOST )
	{
		SetState( STATE_PARTY_LOBBY_PEER );
	}
	
	// Let IsHost be authoritative on the qualification of peer/host state types
	if( GetGameLobby().IsHost() && localState == STATE_GAME_LOBBY_PEER )
	{
		SetState( STATE_GAME_LOBBY_HOST );
	}
	else if( GetGameLobby().IsPeer() && localState == STATE_GAME_LOBBY_HOST )
	{
		SetState( STATE_GAME_LOBBY_PEER );
	}
	
	switch( localState )
	{
		case STATE_PRESS_START:
			return false;
		case STATE_IDLE:
			HandlePackets();
			return false;		// Call handle packets, since packets from old sessions could still be in flight, which need to be emptied
		case STATE_PARTY_LOBBY_HOST:
			return State_Party_Lobby_Host();
		case STATE_PARTY_LOBBY_PEER:
			return State_Party_Lobby_Peer();
		case STATE_GAME_LOBBY_HOST:
			return State_Game_Lobby_Host();
		case STATE_GAME_LOBBY_PEER:
			return State_Game_Lobby_Peer();
		case STATE_GAME_STATE_LOBBY_HOST:
			return State_Game_State_Lobby_Host();
		case STATE_GAME_STATE_LOBBY_PEER:
			return State_Game_State_Lobby_Peer();
		case STATE_LOADING:
			return State_Loading();
		case STATE_INGAME:
			return State_InGame();
		case STATE_CREATE_AND_MOVE_TO_PARTY_LOBBY:
			return State_Create_And_Move_To_Party_Lobby();
		case STATE_CREATE_AND_MOVE_TO_GAME_LOBBY:
			return State_Create_And_Move_To_Game_Lobby();
		case STATE_CREATE_AND_MOVE_TO_GAME_STATE_LOBBY:
			return State_Create_And_Move_To_Game_State_Lobby();
		case STATE_FIND_OR_CREATE_MATCH:
			return State_Find_Or_Create_Match();
		case STATE_CONNECT_AND_MOVE_TO_PARTY:
			return State_Connect_And_Move_To_Party();
		case STATE_CONNECT_AND_MOVE_TO_GAME:
			return State_Connect_And_Move_To_Game();
		case STATE_CONNECT_AND_MOVE_TO_GAME_STATE:
			return State_Connect_And_Move_To_Game_State();
		case STATE_BUSY:
			return State_Busy();
		default:
			libBud::Error( "HandleState:  Unknown state in budSessionLocal" );
	}
}

/*
========================
budSessionLocal::GetState
========================
*/
budSessionLocal::sessionState_t budSessionLocal::GetState() const
{
	// Convert our internal state to one of the external states
	switch( localState )
	{
		case STATE_PRESS_START:
			return PRESS_START;
		case STATE_IDLE:
			return IDLE;
		case STATE_PARTY_LOBBY_HOST:
			return PARTY_LOBBY;
		case STATE_PARTY_LOBBY_PEER:
			return PARTY_LOBBY;
		case STATE_GAME_LOBBY_HOST:
			return GAME_LOBBY;
		case STATE_GAME_LOBBY_PEER:
			return GAME_LOBBY;
		case STATE_GAME_STATE_LOBBY_HOST:
			return GAME_LOBBY;
		case STATE_GAME_STATE_LOBBY_PEER:
			return GAME_LOBBY;
		case STATE_LOADING:
			return LOADING;
		case STATE_INGAME:
			return INGAME;
		case STATE_CREATE_AND_MOVE_TO_PARTY_LOBBY:
			return CONNECTING;
		case STATE_CREATE_AND_MOVE_TO_GAME_LOBBY:
			return CONNECTING;
		case STATE_CREATE_AND_MOVE_TO_GAME_STATE_LOBBY:
			return CONNECTING;
		case STATE_FIND_OR_CREATE_MATCH:
			return SEARCHING;
		case STATE_CONNECT_AND_MOVE_TO_PARTY:
			return CONNECTING;
		case STATE_CONNECT_AND_MOVE_TO_GAME:
			return CONNECTING;
		case STATE_CONNECT_AND_MOVE_TO_GAME_STATE:
			return CONNECTING;
		case STATE_BUSY:
			return BUSY;
		default:
		{
			libBud::Error( "GetState: Unknown state in budSessionLocal" );
		}
	};
}

const char* budSessionLocal::GetStateString() const
{
	static const char* stateToString[] =
	{
		ASSERT_ENUM_STRING( STATE_PRESS_START, 0 ),
		ASSERT_ENUM_STRING( STATE_IDLE, 1 ),
		ASSERT_ENUM_STRING( STATE_PARTY_LOBBY_HOST, 2 ),
		ASSERT_ENUM_STRING( STATE_PARTY_LOBBY_PEER, 3 ),
		ASSERT_ENUM_STRING( STATE_GAME_LOBBY_HOST, 4 ),
		ASSERT_ENUM_STRING( STATE_GAME_LOBBY_PEER, 5 ),
		ASSERT_ENUM_STRING( STATE_GAME_STATE_LOBBY_HOST, 6 ),
		ASSERT_ENUM_STRING( STATE_GAME_STATE_LOBBY_PEER, 7 ),
		ASSERT_ENUM_STRING( STATE_CREATE_AND_MOVE_TO_PARTY_LOBBY, 8 ),
		ASSERT_ENUM_STRING( STATE_CREATE_AND_MOVE_TO_GAME_LOBBY, 9 ),
		ASSERT_ENUM_STRING( STATE_CREATE_AND_MOVE_TO_GAME_STATE_LOBBY, 10 ),
		ASSERT_ENUM_STRING( STATE_FIND_OR_CREATE_MATCH, 11 ),
		ASSERT_ENUM_STRING( STATE_CONNECT_AND_MOVE_TO_PARTY, 12 ),
		ASSERT_ENUM_STRING( STATE_CONNECT_AND_MOVE_TO_GAME, 13 ),
		ASSERT_ENUM_STRING( STATE_CONNECT_AND_MOVE_TO_GAME_STATE, 14 ),
		ASSERT_ENUM_STRING( STATE_BUSY, 15 ),
		ASSERT_ENUM_STRING( STATE_LOADING, 16 ),
		ASSERT_ENUM_STRING( STATE_INGAME, 17 )
	};
	return stateToString[ localState ];
}

// budSession interface

/*
========================
budSessionLocal::LoadingFinished

Only called by budCommonLocal::FinalizeMapChange
========================
*/
void budSessionLocal::LoadingFinished()
{
	NET_VERBOSE_PRINT( "NET: Loading Finished\n" );
	
	assert( GetState() == budSession::LOADING );
	
	GetActingGameStateLobby().loaded = true;
	
	if( MatchTypeIsLocal( GetActingGameStateLobby().parms.matchFlags ) )
	{
		SetState( STATE_INGAME );
	}
	else if( !GetActingGameStateLobby().IsHost() )  	// Tell game host we're done loading
	{
		byte buffer[ idPacketProcessor::MAX_PACKET_SIZE ];
		budBitMsg msg( buffer, sizeof( buffer ) );
		GetActingGameStateLobby().QueueReliableMessage( GetActingGameStateLobby().host, idLobby::RELIABLE_LOADING_DONE, msg.GetReadData(), msg.GetSize() );
	}
	else
	{
		SetState( STATE_INGAME );
	}
	
	SetFlushedStats( false );
}

/*
========================
budSessionLocal::SendUsercmds
========================
*/
void budSessionLocal::SendUsercmds( budBitMsg& msg )
{
	if( localState != STATE_INGAME )
	{
		return;
	}
	
	if( GetActingGameStateLobby().IsPeer() )
	{
		idLobby::peer_t& hostPeer = GetActingGameStateLobby().peers[GetActingGameStateLobby().host];
		
		// Don't send user cmds if we have unsent packet fragments
		//  (This can happen if we have packets to send, but SendAnotherFragment got throttled)
		if( hostPeer.packetProc->HasMoreFragments() )
		{
			libBud::Warning( "NET: Client called SendUsercmds while HasMoreFragments(). Skipping userCmds for this frame." );
			return;
		}
		
		int sequence = hostPeer.snapProc->GetLastAppendedSequence();
		
		// Add incoming BPS for QoS
		float incomingBPS = hostPeer.receivedBps;
		if( hostPeer.receivedBpsIndex != sequence )
		{
			incomingBPS = budMath::ClampFloat( 0.0f, static_cast<float>( idLobby::BANDWIDTH_REPORTING_MAX ), hostPeer.packetProc->GetIncomingRateBytes() );
			hostPeer.receivedBpsIndex = sequence;
			hostPeer.receivedBps = incomingBPS;
		}
		uint16 incomingBPS_quantized = budMath::Ftoi( incomingBPS * ( ( BIT( idLobby::BANDWIDTH_REPORTING_BITS ) - 1 )  / idLobby::BANDWIDTH_REPORTING_MAX ) );
		
		byte buffer[idPacketProcessor::MAX_FINAL_PACKET_SIZE];
		lzwCompressionData_t lzwData;
		idLZWCompressor lzwCompressor( &lzwData );
		lzwCompressor.Start( buffer, sizeof( buffer ) );
		lzwCompressor.WriteAgnostic( sequence );
		lzwCompressor.WriteAgnostic( incomingBPS_quantized );
		lzwCompressor.Write( msg.GetReadData(), msg.GetSize() );
		lzwCompressor.End();
		
		GetActingGameStateLobby().ProcessOutgoingMsg( GetActingGameStateLobby().host, buffer, lzwCompressor.Length(), false, 0 );
		
		if( net_debugBaseStates.GetBool() && sequence < 50 )
		{
			libBud::Printf( "NET: Acking snap %d \n", sequence );
		}
	}
}

/*
========================
budSessionLocal::SendSnapshot
========================
*/
void budSessionLocal::SendSnapshot( budSnapShot& ss )
{
	for( int p = 0; p < GetActingGameStateLobby().peers.Num(); p++ )
	{
		idLobby::peer_t& peer = GetActingGameStateLobby().peers[p];
		
		if( !peer.IsConnected() )
		{
			continue;
		}
		
		if( !peer.loaded )
		{
			continue;
		}
		
		if( peer.pauseSnapshots )
		{
			continue;
		}
		
		GetActingGameStateLobby().SendSnapshotToPeer( ss, p );
	}
}

/*
========================
budSessionLocal::UpdateSignInManager
========================
*/
void budSessionLocal::UpdateSignInManager()
{
	if( !HasSignInManager() )
	{
		return;
	}
	
	if( net_headlessServer.GetBool() )
	{
		return;
	}
	
	// FIXME: We need to ask the menu system for this info.  Just making a best guess for now
	// (assume we are allowed to join the party as a splitscreen user if we are in the party lobby)
	bool allowJoinParty	= ( localState == STATE_PARTY_LOBBY_HOST || localState == STATE_PARTY_LOBBY_PEER ) && GetPartyLobby().state == idLobby::STATE_IDLE;
	bool allowJoinGame	= ( localState == STATE_GAME_LOBBY_HOST || localState == STATE_GAME_LOBBY_PEER ) && GetGameLobby().state == idLobby::STATE_IDLE;
	
	bool eitherLobbyRunning	= GetActivePlatformLobby() != NULL && ( GetPartyLobby().IsLobbyActive() || GetGameLobby().IsLobbyActive() );
	bool onlineMatch		= eitherLobbyRunning && MatchTypeIsOnline( GetActivePlatformLobby()->parms.matchFlags );
	
	//=================================================================================
	// Get the number of desired signed in local users depending on what mode we're in.
	//=================================================================================
	int minDesiredUsers = 0;
	int maxDesiredUsers = Max( 1, signInManager->GetNumLocalUsers() );
	
	if( si_splitscreen.GetInteger() != 0 )
	{
		// For debugging, force 2 splitscreen players
		minDesiredUsers = 2;
		maxDesiredUsers = 2;
		allowJoinGame = true;
	}
	else if( onlineMatch || ( eitherLobbyRunning == false ) )
	{
		// If this an online game, then only 1 user can join locally.
		// Also, if no sessions are active, remove any extra players.
		maxDesiredUsers = 1;
	}
	else if( allowJoinParty || allowJoinGame )
	{
		// If we are in the party lobby, allow 2 splitscreen users to join
		maxDesiredUsers = 2;
	}
	
	// Set the number of desired users
	signInManager->SetDesiredLocalUsers( minDesiredUsers, maxDesiredUsers );
	
	//=================================================================================
	// Update signin manager
	//=================================================================================
	
	// Update signin mgr.  This manager tracks signed in local users, which the session then uses
	// to determine who should be in the lobby.
	signInManager->Pump();
	
	// Get the master local user
	idLocalUser* masterUser = signInManager->GetMasterLocalUser();
	
	if( onlineMatch && masterUser != NULL && !masterUser->CanPlayOnline() && !masterUser->HasOwnerChanged() )
	{
		if( localState > STATE_IDLE )
		{
			// User is still valid, just no longer online
			if( offlineTransitionTimerStart == 0 )
			{
				offlineTransitionTimerStart = Sys_Milliseconds();
			}
			
			if( ( Sys_Milliseconds() - offlineTransitionTimerStart ) > net_offlineTransitionThreshold.GetInteger() )
			{
				MoveToMainMenu();
			}
		}
		return;		// Bail out so signInManager->ValidateLocalUsers below doesn't prematurely remove the master user before we can detect loss of connection
	}
	else
	{
		offlineTransitionTimerStart = 0;
	}
	
	// Remove local users (from the signin manager) who aren't allowed to be online if this is an online match.
	// Remove local user (from the signin manager) who are not properly signed into a profile.
	signInManager->ValidateLocalUsers( onlineMatch );
	
	//=================================================================================
	// Check to see if we need to go to "Press Start"
	//=================================================================================
	
	// Get the master local user (again, after ValidateOnlineLocalUsers, to make sure he is still valid)
	masterUser = signInManager->GetMasterLocalUser();
	
	if( masterUser == NULL )
	{
		// If we don't have a master user at all, then we need to be at "Press Start"
		return;
	}
	else if( localState == STATE_PRESS_START )
	{
	
	
		// If we have a master user, and we are at press start, move to the menu area
		SetState( STATE_IDLE );
		
	}
	
	// See if the master user either isn't persistent (but needs to be), OR, if the owner changed
	// RequirePersistentMaster is poorly named, this really means RequireSignedInMaster
	if( masterUser->HasOwnerChanged() || ( RequirePersistentMaster() && !masterUser->IsProfileReady() ) )
	{
		return;
	}
	
	//=================================================================================
	// Sync lobby users with the signed in users
	// The initial list of session users are normally determined at connect or create time.
	// These functions allow splitscreen users to join in, or check to see if existing
	// users (including the master) need to be removed.
	//=================================================================================
	GetPartyLobby().SyncLobbyUsersWithLocalUsers( allowJoinParty, onlineMatch );
	GetGameLobby().SyncLobbyUsersWithLocalUsers( allowJoinGame, onlineMatch );
	GetGameStateLobby().SyncLobbyUsersWithLocalUsers( allowJoinGame, onlineMatch );
}

/*
========================
budSessionLocal::GetProfileFromMasterLocalUser
========================
*/
budPlayerProfile* budSessionLocal::GetProfileFromMasterLocalUser()
{
	budPlayerProfile* profile = NULL;
	idLocalUser* masterUser = signInManager->GetMasterLocalUser();
	
	if( masterUser != NULL )
	{
		profile = masterUser->GetProfile();
	}
	
	if( profile == NULL )
	{
		// Whoops
		profile = signInManager->GetDefaultProfile();
		//libBud::Warning( "Returning fake profile until the code is fixed to handle NULL profiles." );
	}
	
	return profile;
}

/*
========================
budSessionLocal::GetPeerName
========================
*/
const char* budSessionLocal::GetPeerName( int peerNum )
{
	return GetActingGameStateLobby().GetPeerName( peerNum );
}


/*
========================
budSessionLocal::SetState
========================
*/
void budSessionLocal::SetState( state_t newState )
{

	assert( newState < NUM_STATES );
	assert( localState < NUM_STATES );
	verify_array_size( stateToString, NUM_STATES );
	
	if( newState == localState )
	{
		NET_VERBOSE_PRINT( "NET: SetState: State SAME %s\n", stateToString[ newState ] );
		return;
	}
	
	// Set the current state
	NET_VERBOSE_PRINT( "NET: SetState: State changing from %s to %s\n", stateToString[ localState ], stateToString[ newState ] );
	
	if( localState < STATE_LOADING && newState >= STATE_LOADING )
	{
		// Tell lobby instances that the match has started
		StartSessions();
		// Clear certain dialog boxes we don't want to see in-game
	}
	else if( localState >= STATE_LOADING && newState < STATE_LOADING )
	{
		// Tell lobby instances that the match has ended
		if( !WasMigrationGame() )    // Don't end the session if we are going right back into the game
		{
			EndSessions();
		}
	}
	
	if( newState == STATE_GAME_LOBBY_HOST || newState == STATE_GAME_LOBBY_PEER )
	{
		ComputeNextGameCoalesceTime();
	}
	
	localState = newState;
}

/*
========================
budSessionLocal::HandlePackets
========================
*/
bool budSessionLocal::HandlePackets()
{
	SCOPED_PROFILE_EVENT( "Session::HandlePackets" );
	
	byte				packetBuffer[ idPacketProcessor::MAX_FINAL_PACKET_SIZE ];
	lobbyAddress_t		remoteAddress;
	int					recvSize = 0;
	bool				fromDedicated = false;
	
	while( ReadRawPacket( remoteAddress, packetBuffer, recvSize, fromDedicated, sizeof( packetBuffer ) ) && recvSize > 0 )
	{
	
		// fragMsg will hold the raw packet
		budBitMsg fragMsg;
		fragMsg.InitRead( packetBuffer, recvSize );
		
		// Peek at the session ID
		idPacketProcessor::sessionId_t sessionID = idPacketProcessor::GetSessionID( fragMsg );
		
		// libBud::Printf( "NET: HandlePackets - session %d, size %d \n", sessionID, recvSize );
		
		// Make sure it's valid
		if( sessionID == idPacketProcessor::SESSION_ID_INVALID )
		{
			libBud::Printf( "NET: Invalid sessionID %s.\n", remoteAddress.ToString() );
			continue;
		}
		
		//
		// Distribute the packet to the proper lobby
		//
		
		const int maskedType = sessionID & idPacketProcessor::LOBBY_TYPE_MASK;
		
		if( !verify( maskedType > 0 ) )
		{
			continue;
		}
		
		idLobby::lobbyType_t lobbyType = ( idLobby::lobbyType_t )( maskedType - 1 );
		
		switch( lobbyType )
		{
			case idLobby::TYPE_PARTY:
				GetPartyLobby().HandlePacket( remoteAddress, fragMsg, sessionID );
				break;
			case idLobby::TYPE_GAME:
				GetGameLobby().HandlePacket( remoteAddress, fragMsg, sessionID );
				break;
			case idLobby::TYPE_GAME_STATE:
				GetGameStateLobby().HandlePacket( remoteAddress, fragMsg, sessionID );
				break;
			default:
				assert( 0 );
		}
	}
	
	return false;
}

/*
========================
budSessionLocal::GetActivePlatformLobby
========================
*/
idLobby* budSessionLocal::GetActivePlatformLobby()
{
	sessionState_t state = GetState();
	
	if( ( state == GAME_LOBBY ) || ( state == BUSY ) || ( state == INGAME ) || ( state == LOADING ) )
	{
		return &GetGameLobby();
	}
	else if( state == PARTY_LOBBY )
	{
		return &GetPartyLobby();
	}
	
	return NULL;
}

/*
========================
budSessionLocal::GetActivePlatformLobby
========================
*/
const idLobby* budSessionLocal::GetActivePlatformLobby() const
{
	sessionState_t state = GetState();
	
	if( ( state == GAME_LOBBY ) || ( state == BUSY ) || ( state == INGAME ) || ( state == LOADING ) )
	{
		return &GetGameLobby();
	}
	else if( state == PARTY_LOBBY )
	{
		return &GetPartyLobby();
	}
	
	return NULL;
}

/*
========================
budSessionLocal::GetActingGameStateLobby
========================
*/
idLobby& budSessionLocal::GetActingGameStateLobby()
{
	if( net_useGameStateLobby.GetBool() )
	{
		return GetGameStateLobby();
	}
	
	return GetGameLobby();
}

/*
========================
budSessionLocal::GetActingGameStateLobby
========================
*/
const idLobby& budSessionLocal::GetActingGameStateLobby() const
{
	if( net_useGameStateLobby.GetBool() )
	{
		return GetGameStateLobby();
	}
	
	return GetGameLobby();
}

/*
========================
budSessionLocal::GetLobbyFromType
========================
*/
idLobby* budSessionLocal::GetLobbyFromType( idLobby::lobbyType_t lobbyType )
{
	switch( lobbyType )
	{
		case idLobby::TYPE_PARTY:
			return &GetPartyLobby();
		case idLobby::TYPE_GAME:
			return &GetGameLobby();
		case idLobby::TYPE_GAME_STATE:
			return &GetGameStateLobby();
	}
	
	return NULL;
}

/*
========================
budSessionLocal::GetActivePlatformLobbyBase
This returns the base version for the budSession version
========================
*/
idLobbyBase& budSessionLocal::GetActivePlatformLobbyBase()
{
	idLobby* activeLobby = GetActivePlatformLobby();
	
	if( activeLobby != NULL )
	{
		return *activeLobby;
	}
	
	return stubLobby;		// So we can return at least something
}

/*
========================
budSessionLocal::GetLobbyFromLobbyUserID
========================
*/
idLobbyBase& budSessionLocal::GetLobbyFromLobbyUserID( lobbyUserID_t lobbyUserID )
{
	if( !lobbyUserID.IsValid() )
	{
		return stubLobby;	// So we can return at least something
	}
	
	idLobby* lobby = GetLobbyFromType( ( idLobby::lobbyType_t )lobbyUserID.GetLobbyType() );
	
	if( lobby != NULL )
	{
		return *lobby;
	}
	
	return stubLobby;		// So we can return at least something
}

/*
========================
budSessionLocal::TickSendQueue
========================
*/
void budSessionLocal::TickSendQueue()
{
	assert( !sendQueue.IsEmpty() );
	int now = Sys_Milliseconds();
	idQueuePacket* packet = sendQueue.Peek();
	while( packet != NULL )
	{
		if( now < packet->time )
		{
			break;
		}
		
		GetPort( packet->dedicated ).SendRawPacket( packet->address, packet->data, packet->size );
		
		if( net_forceUpstream.GetFloat() != 0.0f && net_forceUpstreamQueue.GetFloat() != 0.0f )
		{
			// FIXME: no can do both
			assert( net_forceLatency.GetInteger() == 0 );
			// compute / update an added traffic due to the queuing
			// we can't piggyback on upstreamDropRate because of the way it's computed and clamped to zero
			int time = Sys_Milliseconds();
			if( time > upstreamQueueRateTime )
			{
				upstreamQueueRate -= upstreamQueueRate * ( float )( time - upstreamQueueRateTime ) / 1000.0f;
				if( upstreamQueueRate < 0.0f )
				{
					upstreamQueueRate = 0.0f;
				}
				upstreamQueueRateTime = time;
			}
			// update queued bytes
			queuedBytes -= packet->size;
			if( net_verboseSimulatedTraffic.GetBool() )
			{
				libBud::Printf( "send queued packet size %d to %s\n", packet->size, packet->address.ToString() );
			}
		}
		
		sendQueue.RemoveFirst(); // we have it already, just push it off the queue before freeing
		packetAllocator.Free( packet );
		packet = sendQueue.Peek();
	}
}

/*
========================
budSessionLocal::QueuePacket
========================
*/
void budSessionLocal::QueuePacket( idQueue< idQueuePacket, &idQueuePacket::queueNode >& queue, int time, const lobbyAddress_t& to, const void* data, int size, bool dedicated )
{
	//mem.PushHeap();
	
	idQueuePacket* packet = packetAllocator.Alloc();
	
	packet->address		= to;
	packet->size		= size;
	packet->dedicated	= dedicated;
	packet->time		= time;
	
	memcpy( packet->data, data, size );
	
	queue.Add( packet );
	
	//mem.PopHeap();
}

/*
========================
budSessionLocal::ReadRawPacketFromQueue
========================
*/
bool budSessionLocal::ReadRawPacketFromQueue( int time, lobbyAddress_t& from, void* data, int& size, bool& outDedicated, int maxSize )
{
	idQueuePacket* packet = recvQueue.Peek();
	
	if( packet == NULL || time < packet->time )
	{
		return false;		// Either there are no packets, or no packet is ready
	}
	
	//libBud::Printf( "NET: Packet recvd: %d ms\n", now );
	
	from = packet->address;
	size = packet->size;
	assert( size <= maxSize );
	outDedicated = packet->dedicated;
	memcpy( data, packet->data, packet->size );
	recvQueue.RemoveFirst(); // we have it already, just push it off the queue before freeing
	packetAllocator.Free( packet );
	
	return true;
}

/*
========================
budSessionLocal::SendRawPacket
========================
*/
void budSessionLocal::SendRawPacket( const lobbyAddress_t& to, const void* data, int size, bool dedicated )
{
	const int now = Sys_Milliseconds();
	
	if( net_forceUpstream.GetFloat() != 0 )
	{
	
		// the total bandwidth rate at which the networking systems are trying to push data through
		float totalOutgoingRate = ( float )GetActingGameStateLobby().GetTotalOutgoingRate(); // B/s
		
		// update the rate at which we have been taking data out by dropping it
		int time = Sys_Milliseconds();
		if( time > upstreamDropRateTime )
		{
			upstreamDropRate -= upstreamDropRate * ( float )( time - upstreamDropRateTime ) / 1000.0f;
			if( upstreamDropRate < 0.0f )
			{
				upstreamDropRate = 0.0f;
			}
			upstreamDropRateTime = time;
		}
		
		if( ( float )( totalOutgoingRate - upstreamDropRate + upstreamQueueRate ) > net_forceUpstream.GetFloat() * 1024.0f )  // net_forceUpstream is in kB/s, everything else in B/s
		{
			if( net_forceUpstreamQueue.GetFloat() == 0.0f )
			{
				// just drop the packet - not representative, but simple
				if( net_verboseSimulatedTraffic.GetBool() )
				{
					libBud::Printf( "drop %d bytes to %s\n", size, to.ToString() );
				}
				// increase the instant drop rate with the data we just dropped
				upstreamDropRate += size;
				return;
			}
			
			// simulate a network device with a send queue
			// do we have room in the queue?
			assert( net_forceUpstreamQueue.GetFloat() > 0.0f );
			if( ( float )( queuedBytes + size ) > net_forceUpstreamQueue.GetFloat() * 1024.0f )  // net_forceUpstreamQueue is in kB/s
			{
				// too much queued, this is still a drop
				// FIXME: factorize
				// just drop the packet - not representative, but simple
				if( net_verboseSimulatedTraffic.GetBool() )
				{
					libBud::Printf( "full queue: drop %d bytes to %s\n", size, to.ToString() );
				}
				// increase the instant drop rate with the data we just dropped
				upstreamDropRate += size;
				return;
			}
			// there is room to buffer up in the queue
			queuedBytes += size;
			// with queuedBytes and the current upstream, when should this packet be sent?
			int queuedPacketSendDelay = 1000.0f * ( ( float )queuedBytes / ( net_forceUpstream.GetFloat() * 1024.0f ) ); // in ms
			// queue for sending
			if( net_verboseSimulatedTraffic.GetBool() )
			{
				libBud::Printf( "queuing packet: %d bytes delayed %d ms\n", size, queuedPacketSendDelay );
			}
			
			QueuePacket( sendQueue, now + queuedPacketSendDelay, to, data, size, dedicated );
			
			// will abuse the forced latency code below to take care of the sending
			// FIXME: right now, can't have both on
			assert( net_forceLatency.GetInteger() == 0 );
		}
	}
	
	// short path
	// NOTE: network queuing: will go to tick the queue whenever sendQueue isn't empty, regardless of latency
	if( net_forceLatency.GetInteger() == 0 && sendQueue.IsEmpty() )
	{
		GetPort( dedicated ).SendRawPacket( to, data, size );
		return;
	}
	
	if( net_forceUpstream.GetFloat() != 0.0f && net_forceUpstreamQueue.GetFloat() != 0.0f )
	{
		// FIXME: not doing both just yet
		assert( net_forceLatency.GetInteger() == 0 );
		TickSendQueue();
		return; // we done (at least for queue only path)
	}
	
	// queue up
	assert( size != 0 && size <= idPacketProcessor::MAX_FINAL_PACKET_SIZE );
	
	QueuePacket( sendQueue, now + net_forceLatency.GetInteger() / 2, to, data, size, dedicated );
	
	TickSendQueue();
}

/*
========================
budSessionLocal::ReadRawPacket
========================
*/
bool budSessionLocal::ReadRawPacket( lobbyAddress_t& from, void* data, int& size, bool& outDedicated, int maxSize )
{
	SCOPED_PROFILE_EVENT( "Session::ReadRawPacket" );
	
	assert( maxSize <= idPacketProcessor::MAX_FINAL_PACKET_SIZE );
	
	if( !sendQueue.IsEmpty() )
	{
		TickSendQueue();
	}
	
	const int now = Sys_Milliseconds();
	
	// Make sure we give both ports equal time
	static bool currentDedicated = false;
	currentDedicated = !currentDedicated;
	
	for( int i = 0; i < 2; i++ )
	{
		// BRIAN_FIXME: Dedicated servers fuck up running 2 instances on the same machine
		// outDedicated = ( i == 0 ) ? currentDedicated : !currentDedicated;
		outDedicated = false;
		
		if( GetPort( outDedicated ).ReadRawPacket( from, data, size, maxSize ) )
		{
			if( net_forceLatency.GetInteger() == 0 && recvQueue.IsEmpty() )
			{
				// If we aren't forcing latency, and queue is empty, return result immediately
				return true;
			}
			
			// the cvar is meant to be a round trip latency so we're applying half on the send and half on the recv
			const int time = ( net_forceLatency.GetInteger() == 0 ) ? 0 : now + net_forceLatency.GetInteger() / 2;
			
			// Otherwise, queue result
			QueuePacket( recvQueue, time, from, data, size, outDedicated );
		}
	}
	
	// Return any queued results
	return ReadRawPacketFromQueue( now, from, data, size, outDedicated, maxSize );
}

/*
========================
budSessionLocal::ConnectAndMoveToLobby
========================
*/
void budSessionLocal::ConnectAndMoveToLobby( idLobby& lobby, const lobbyConnectInfo_t& connectInfo, bool fromInvite )
{

	// Since we are connecting directly to a lobby, make sure no search results are left over from previous FindOrCreateMatch results
	// If we don't do this, we might think we should attempt to connect to an old search result, and we don't want to in this case
	lobby.searchResults.Clear();
	
	// Attempt to connect to the lobby
	lobby.ConnectTo( connectInfo, fromInvite );
	
	connectType = CONNECT_DIRECT;
	
	// Wait for connection
	switch( lobby.lobbyType )
	{
		case idLobby::TYPE_PARTY:
			SetState( STATE_CONNECT_AND_MOVE_TO_PARTY );
			break;
		case idLobby::TYPE_GAME:
			SetState( STATE_CONNECT_AND_MOVE_TO_GAME );
			break;
		case idLobby::TYPE_GAME_STATE:
			SetState( STATE_CONNECT_AND_MOVE_TO_GAME_STATE );
			break;
	}
}

/*
========================
budSessionLocal::GoodbyeFromHost
========================
*/
void budSessionLocal::GoodbyeFromHost( idLobby& lobby, int peerNum, const lobbyAddress_t& remoteAddress, int msgType )
{
	if( !verify( localState > STATE_IDLE ) )
	{
		libBud::Printf( "NET: Got disconnected from host %s on session %s when we were not in a lobby or game.\n", remoteAddress.ToString(), lobby.GetLobbyName() );
		MoveToMainMenu();
		return;		// Ignore if we are not past the main menu
	}
	
	// Goodbye from host.  See if we were connecting vs connected
	if( ( localState == STATE_CONNECT_AND_MOVE_TO_PARTY || localState == STATE_CONNECT_AND_MOVE_TO_GAME ) && lobby.peers[peerNum].GetConnectionState() == idLobby::CONNECTION_CONNECTING )
	{
		// We were denied a connection attempt
		libBud::Printf( "NET: Denied connection attempt from host %s on session %s. MsgType %i.\n", remoteAddress.ToString(), lobby.GetLobbyName(), msgType );
		// This will try to move to the next connection if one exists, otherwise will create a match
		HandleConnectionFailed( lobby, msgType == idLobby::OOB_GOODBYE_FULL );
	}
	else
	{
		// We were disconnected from a server we were previously connected to
		libBud::Printf( "NET: Disconnected from host %s on session %s. MsgType %i.\n", remoteAddress.ToString(), lobby.GetLobbyName(), msgType );
		
		const bool leaveGameWithParty = ( msgType == idLobby::OOB_GOODBYE_W_PARTY );
		
		if( leaveGameWithParty && lobby.lobbyType == idLobby::TYPE_GAME && lobby.IsPeer() && GetState() == budSession::GAME_LOBBY && GetPartyLobby().host >= 0 &&
				lobby.peers[peerNum].address.Compare( GetPartyLobby().peers[GetPartyLobby().host].address, true ) )
		{
			// If a host is telling us goodbye from a game lobby, and the game host is the same as our party host,
			// and we aren't in a game, and the host wants us to leave with him, then do so now
			GetGameLobby().Shutdown();
			GetGameStateLobby().Shutdown();
			SetState( STATE_PARTY_LOBBY_PEER );
		}
		else
		{
			// Host left, so pick a new host (possibly even us) for this lobby
			lobby.PickNewHost();
		}
	}
}

/*
========================
budSessionLocal::WriteLeaderboardToMsg
========================
*/
void budSessionLocal::WriteLeaderboardToMsg( budBitMsg& msg, const leaderboardDefinition_t* leaderboard, const column_t* stats )
{
	assert( Sys_FindLeaderboardDef( leaderboard->id ) == leaderboard );
	
	msg.WriteLong( leaderboard->id );
	
	for( int i = 0; i < leaderboard->numColumns; ++i )
	{
		uint64 value = stats[i].value;
		
		//libBud::Printf( "value = %i\n", (int32)value );
		
		for( int j = 0; j < leaderboard->columnDefs[i].bits; j++ )
		{
			msg.WriteBits( value & 1, 1 );
			value >>= 1;
		}
		//msg.WriteData( &stats[i].value, sizeof( stats[i].value ) );
	}
}

/*
========================
budSessionLocal::ReadLeaderboardFromMsg
========================
*/
const leaderboardDefinition_t* budSessionLocal::ReadLeaderboardFromMsg( budBitMsg& msg, column_t* stats )
{
	int id = msg.ReadLong();
	
	const leaderboardDefinition_t* leaderboard = Sys_FindLeaderboardDef( id );
	
	if( leaderboard == NULL )
	{
		libBud::Printf( "NET: Invalid leaderboard id: %i\n", id );
		return NULL;
	}
	
	for( int i = 0; i < leaderboard->numColumns; ++i )
	{
		uint64 value = 0;
		
		for( int j = 0; j < leaderboard->columnDefs[i].bits; j++ )
		{
			value |= ( uint64 )( msg.ReadBits( 1 ) & 1 ) << j;
		}
		
		stats[i].value = value;
		
		//libBud::Printf( "value = %i\n", (int32)value );
		//msg.ReadData( &stats[i].value, sizeof( stats[i].value ) );
	}
	
	return leaderboard;
}

/*
========================
budSessionLocal::SendLeaderboardStatsToPlayer
========================
*/
void budSessionLocal::SendLeaderboardStatsToPlayer( lobbyUserID_t lobbyUserID, const leaderboardDefinition_t* leaderboard, const column_t* stats )
{

	const int sessionUserIndex = GetActingGameStateLobby().GetLobbyUserIndexByID( lobbyUserID );
	
	if( GetActingGameStateLobby().IsLobbyUserDisconnected( sessionUserIndex ) )
	{
		libBud::Warning( "Tried to tell disconnected user to report stats" );
		return;
	}
	
	const int peerIndex = GetActingGameStateLobby().PeerIndexFromLobbyUser( lobbyUserID );
	
	if( peerIndex == -1 )
	{
		libBud::Warning( "Tried to tell invalid peer index to report stats" );
		return;
	}
	
	if( !verify( GetActingGameStateLobby().IsHost() ) ||
			!verify( peerIndex < GetActingGameStateLobby().peers.Num() ) ||
			!verify( GetActingGameStateLobby().peers[ peerIndex ].IsConnected() ) )
	{
		libBud::Warning( "Tried to tell invalid peer to report stats" );
		return;
	}
	
	NET_VERBOSE_PRINT( "Telling sessionUserIndex %i (peer %i) to report stats\n", sessionUserIndex, peerIndex );
	
	lobbyUser_t* gameUser = GetActingGameStateLobby().GetLobbyUser( sessionUserIndex );
	
	if( !verify( gameUser != NULL ) )
	{
		return;
	}
	
	byte buffer[ idPacketProcessor::MAX_PACKET_SIZE ];
	budBitMsg msg( buffer, sizeof( buffer ) );
	
	// Use the user ID
	gameUser->lobbyUserID.WriteToMsg( msg );
	
	WriteLeaderboardToMsg( msg, leaderboard, stats );
	
	GetActingGameStateLobby().QueueReliableMessage( peerIndex, idLobby::RELIABLE_POST_STATS, msg.GetReadData(), msg.GetSize() );
}

/*
========================
budSessionLocal::RecvLeaderboardStatsForPlayer
========================
*/
void budSessionLocal::RecvLeaderboardStatsForPlayer( budBitMsg& msg )
{
	column_t stats[ MAX_LEADERBOARD_COLUMNS ];
	
	lobbyUserID_t lobbyUserID;
	lobbyUserID.ReadFromMsg( msg );
	
	const leaderboardDefinition_t* leaderboard = ReadLeaderboardFromMsg( msg, stats );
	
	if( leaderboard == NULL )
	{
		libBud::Printf( "RecvLeaderboardStatsForPlayer: Invalid lb.\n" );
		return;
	}
	
	LeaderboardUpload( lobbyUserID, leaderboard, stats );
}

/*
========================
budSessionLocal::RequirePersistentMaster
========================
*/
bool budSessionLocal::RequirePersistentMaster()
{
	return signInManager->RequirePersistentMaster();
}

/*
========================
CheckAndUpdateValue
========================
*/
template<typename T>
bool CheckAndUpdateValue( T& value, const T& newValue )
{
	if( value == newValue )
	{
		return false;
	}
	value = newValue;
	return true;
}

/*
========================
lobbyUser_t::UpdateClientMutableData
========================
*/
bool lobbyUser_t::UpdateClientMutableData( const idLocalUser* localUser )
{
	bool updated = false;
	const budPlayerProfile* profile = localUser->GetProfile();
	if( profile != NULL )
	{
		updated |= CheckAndUpdateValue( level, profile->GetLevel() );
	}
	updated |= CheckAndUpdateValue( selectedSkin, ui_skinIndex.GetInteger() );
	updated |= CheckAndUpdateValue( weaponAutoSwitch, ui_autoSwitch.GetBool() );
	updated |= CheckAndUpdateValue( weaponAutoReload, ui_autoReload.GetBool() );
	return updated;
}

/*
========================
budSessionLocal::ComputeNextGameCoalesceTime
========================
*/
void budSessionLocal::ComputeNextGameCoalesceTime()
{
	const int coalesceTimeInSeconds			= session->GetTitleStorageInt( "net_LobbyCoalesceTimeInSeconds", net_LobbyCoalesceTimeInSeconds.GetInteger() );
	const int randomCoalesceTimeInSeconds	= session->GetTitleStorageInt( "net_LobbyRandomCoalesceTimeInSeconds", net_LobbyRandomCoalesceTimeInSeconds.GetInteger() );
	
	if( coalesceTimeInSeconds != 0 )
	{
		static idRandom2 random( Sys_Milliseconds() );
		
		nextGameCoalesceTime = Sys_Milliseconds() + ( coalesceTimeInSeconds + random.RandomInt( randomCoalesceTimeInSeconds ) ) * 1000;
	}
	else
	{
		nextGameCoalesceTime = 0;
	}
}

/*
========================
lobbyUser_t::Net_BandwidthChallenge
========================
*/
CONSOLE_COMMAND( Net_BandwidthChallenge, "Test network bandwidth", 0 )
{
	session->StartOrContinueBandwidthChallenge( true );
}

/*
========================
lobbyUser_t::Net_ThrottlePeer
========================
*/
CONSOLE_COMMAND( Net_ThrottlePeer, "Test network bandwidth", 0 )
{

	int peerNum = -1;
	int snapRate = 0;
	
	if( args.Argc() >= 3 )
	{
		peerNum = atoi( args.Argv( 1 ) );
		snapRate = atoi( args.Argv( 2 ) );
	}
	
	// Note DebugSetPeerSnaprate will handle peerNum=-1 by printing out list of peers
	session->DebugSetPeerSnaprate( peerNum, snapRate );
}


// FIXME: Move to sys_stats.cpp
budStaticList< leaderboardDefinition_t*, MAX_LEADERBOARDS > registeredLeaderboards;

/*
========================
Sys_FindLeaderboardDef
========================
*/
const leaderboardDefinition_t* Sys_FindLeaderboardDef( int id )
{
	for( int i = 0; i < registeredLeaderboards.Num() ; i++ )
	{
		if( registeredLeaderboards[i] && registeredLeaderboards[i]->id == id )
		{
			return registeredLeaderboards[i];
		}
	}
	
	return NULL;
}

/*
========================
Sys_CreateLeaderboardDef
========================
*/
leaderboardDefinition_t* Sys_CreateLeaderboardDef( int id_, int numColumns_, const columnDef_t* columnDefs_,
		rankOrder_t rankOrder_, bool supportsAttachments_, bool checkAgainstCurrent_ )
{

	leaderboardDefinition_t* newDef = new( TAG_NETWORKING ) leaderboardDefinition_t(	id_, numColumns_, columnDefs_, rankOrder_, supportsAttachments_, checkAgainstCurrent_ );
	
	// try and reuse a free spot
	int leaderboardHandle = registeredLeaderboards.FindNull();
	
	if( leaderboardHandle == -1 )
	{
		leaderboardHandle = registeredLeaderboards.Append( NULL );
	}
	
	registeredLeaderboards[ leaderboardHandle ]  = newDef;
	
	return newDef;
}

/*
========================
Sys_CreateLeaderboardDef
========================
*/
void Sys_DestroyLeaderboardDefs()
{

	// delete and clear all the contents of the registeredLeaderboards static list.
	registeredLeaderboards.DeleteContents( true );
}

/*
========================
budSessionLocal::StartOrContinueBandwidthChallenge
This will start a bandwidth test if one is not active
returns true if a test has completed
========================
*/
bool budSessionLocal::StartOrContinueBandwidthChallenge( bool forceStart )
{
	idLobby* activeLobby = GetActivePlatformLobby();
	if( activeLobby == NULL )
	{
		libBud::Warning( "No active session lobby when budSessionLocal::StartBandwidthChallenge called" );
		return true;
	}
	
	if( !forceStart && activeLobby->bandwidthChallengeFinished )
	{
		activeLobby->bandwidthChallengeFinished = false;
		return true;
	}
	
	if( !activeLobby->BandwidthTestStarted() )
	{
		activeLobby->BeginBandwidthTest();
	}
	
	return false;
}

/*
========================
budSessionLocal::DebugSetPeerSnaprate
This is debug function for manually setting peer's snaprate in game
========================
*/
void budSessionLocal::DebugSetPeerSnaprate( int peerIndex, int snapRateMS )
{
	idLobby* activeLobby = GetActivePlatformLobby();
	if( activeLobby == NULL )
	{
		libBud::Warning( "No active session lobby when budSessionLocal::StartBandwidthChallenge called" );
		return;
	}
	
	if( peerIndex < 0 || peerIndex > activeLobby->peers.Num() )
	{
		libBud::Printf( "Invalid peer %d\n", peerIndex );
		for( int i = 0; i < activeLobby->peers.Num(); i++ )
		{
			libBud::Printf( "Peer[%d] %s\n", i, activeLobby->GetPeerName( i ) );
		}
		return;
	}
	
	activeLobby->peers[peerIndex].throttledSnapRate = snapRateMS * 1000;
	activeLobby->peers[peerIndex].receivedThrottle = 0;
	libBud::Printf( "Set peer %s new snapRate: %d\n", activeLobby->GetPeerName( peerIndex ), activeLobby->peers[peerIndex].throttledSnapRate );
}

/*
========================
budSessionLocal::DebugSetPeerSnaprate
This is debug function for manually setting peer's snaprate in game
========================
*/
float budSessionLocal::GetIncomingByteRate()
{
	idLobby* activeLobby = GetActivePlatformLobby();
	if( activeLobby == NULL )
	{
		libBud::Warning( "No active session lobby when budSessionLocal::GetIncomingByteRate called" );
		return 0;
	}
	
	float total = 0;
	for( int p = 0; p < activeLobby->peers.Num(); p++ )
	{
		if( activeLobby->peers[p].IsConnected() )
		{
			total += activeLobby->peers[p].packetProc->GetIncomingRateBytes();
		}
	}
	
	return total;
}

/*
========================
budSessionLocal::OnLocalUserSignin
========================
*/
void budSessionLocal::OnLocalUserSignin( idLocalUser* user )
{
	// Do stuff before calling OnMasterLocalUserSignin()
	
	// We may not have a profile yet, need to call user's version...
	user->LoadProfileSettings();
	
	// for all consoles except the PS3 we enumerate right away because they don't
	// take such a long time as the PS3. PS3 enumeration is done in the
	// background and kicked off when the profile callback is triggered
	if( user == GetSignInManager().GetMasterLocalUser() )
	{
		OnMasterLocalUserSignin();
	}
}

/*
========================
budSessionLocal::OnLocalUserSignout
========================
*/
void budSessionLocal::OnLocalUserSignout( idLocalUser* user )
{
	// Do stuff before calling OnMasterLocalUserSignout()
	
	if( GetSignInManager().GetMasterLocalUser() == NULL )
	{
		OnMasterLocalUserSignout();
	}
}

/*
========================
budSessionLocal::OnMasterLocalUserSignout
========================
*/
void budSessionLocal::OnMasterLocalUserSignout()
{
	CancelSaveGameWithHandle( enumerationHandle );
	enumerationHandle = 0;
	GetSaveGameManager().GetEnumeratedSavegamesNonConst().Clear();
}

/*
========================
budSessionLocal::OnMasterLocalUserSignin
========================
*/
void budSessionLocal::OnMasterLocalUserSignin()
{
	enumerationHandle = EnumerateSaveGamesAsync();
}

/*
========================
budSessionLocal::OnLocalUserProfileLoaded
========================
*/
void budSessionLocal::OnLocalUserProfileLoaded( idLocalUser* user )
{
	user->RequestSyncAchievements();
}

/*
========================
budSessionLocal::SetVoiceGroupsToTeams
========================
*/
void budSessionLocal::SetVoiceGroupsToTeams()
{
	// move voice chat to team
	int myTeam = 0;
	for( int i = 0; i < GetGameLobby().GetNumLobbyUsers(); ++i )
	{
		const lobbyUser_t* gameUser = GetGameLobby().GetLobbyUser( i );
		
		if( !verify( gameUser != NULL ) )
		{
			continue;
		}
		
		if( gameUser->IsDisconnected() )
		{
			continue;
		}
		
		int userTeam = gameUser->teamNumber;
		
		voiceChat->SetTalkerGroup( gameUser, GetGameLobby().lobbyType, userTeam );
		
		if( GetGameLobby().IsSessionUserIndexLocal( i ) )
		{
			myTeam = userTeam;
		}
	}
	
	SetActiveChatGroup( myTeam );
}

/*
========================
budSessionLocal::ClearVoiceGroups
========================
*/
void budSessionLocal::ClearVoiceGroups()
{
	for( int i = 0; i < GetGameLobby().GetNumLobbyUsers(); ++i )
	{
		const lobbyUser_t* gameUser = GetGameLobby().GetLobbyUser( i );
		
		if( !verify( gameUser != NULL ) )
		{
			continue;
		}
		
		if( gameUser->IsDisconnected() )
		{
			continue;
		}
		
		voiceChat->SetTalkerGroup( gameUser, GetGameLobby().lobbyType, 0 );
	}
	
	SetActiveChatGroup( 0 );
}

/*
========================
budSessionLocal::SendVoiceAudio
========================
*/
void budSessionLocal::SendVoiceAudio()
{
	if( voiceChat == NULL )
	{
		return;
	}
	
	idLobby* activeLobby = GetActivePlatformLobby();
	
	int activeSessionIndex = ( activeLobby != NULL ) ? activeLobby->lobbyType : -1;
	
	voiceChat->SetActiveLobby( activeSessionIndex );
	voiceChat->Pump();
	
	if( activeLobby == NULL )
	{
		return;
	}
	
	int time = Sys_Milliseconds();
	
	const int VOICE_THROTTLE_TIME_IN_MS	= session->GetTitleStorageInt( "VOICE_THROTTLE_TIME_IN_MS", 33 ) ;		// Don't allow faster than 30hz send rate
	
	if( time - lastVoiceSendtime < VOICE_THROTTLE_TIME_IN_MS )
	{
		return;
	}
	
	lastVoiceSendtime = time;
	
	budStaticList< int, MAX_PLAYERS > localTalkers;
	
	voiceChat->GetActiveLocalTalkers( localTalkers );
	
	for( int i = 0; i < localTalkers.Num(); i++ )
	{
	
		// NOTE - For 360, we don't need more than XHV_MAX_VOICECHAT_PACKETS * XHV_VOICECHAT_MODE_PACKET_SIZE bytes
		const int MAX_VDP_DATA_SIZE = 1000;
		
		byte buffer[MAX_VDP_DATA_SIZE];
		
		const int titleStorageDataSize = session->GetTitleStorageInt( "MAX_VDP_DATA_SIZE", 1000 );
		const int dataSizeAvailable = Min< int >( titleStorageDataSize, sizeof( buffer ) );
		
		// in-out parameter
		int dataSize = dataSizeAvailable;
		if( !voiceChat->GetLocalChatData( localTalkers[i], buffer, dataSize ) )
		{
			continue;
		}
		assert( dataSize <= sizeof( buffer ) );
		
		budStaticList< const lobbyAddress_t*, MAX_PLAYERS > recipients;
		
		voiceChat->GetRecipientsForTalker( localTalkers[i], recipients );
		
		for( int j = 0; j < recipients.Num(); j++ )
		{
			activeLobby->SendConnectionLess( *recipients[j], idLobby::OOB_VOICE_AUDIO, buffer, dataSize );
		}
	}
}

/*
========================
budSessionLocal::HandleOobVoiceAudio
========================
*/
void budSessionLocal::HandleOobVoiceAudio( const lobbyAddress_t& from, const budBitMsg& msg )
{

	idLobby* activeLobby = GetActivePlatformLobby();
	
	if( activeLobby == NULL )
	{
		return;
	}
	
	voiceChat->SetActiveLobby( activeLobby->lobbyType );
	
	voiceChat->SubmitIncomingChatData( msg.GetReadData() + msg.GetReadCount(), msg.GetRemainingData() );
}

/*
========================
budSessionLocal::SetActiveChatGroup
========================
*/
void budSessionLocal::SetActiveChatGroup( int groupIndex )
{
	voiceChat->SetActiveChatGroup( groupIndex );
}

/*
========================
budSessionLocal::GetLobbyUserVoiceState
========================
*/
voiceState_t budSessionLocal::GetLobbyUserVoiceState( lobbyUserID_t lobbyUserID )
{
	idLobby* activeLobby = GetActivePlatformLobby();
	
	if( activeLobby == NULL )
	{
		return VOICECHAT_STATE_NOT_TALKING;
	}
	
	const lobbyUser_t* user = activeLobby->GetLobbyUserByID( lobbyUserID );
	
	if( !verify( user != NULL ) )
	{
		return VOICECHAT_STATE_NOT_TALKING;
	}
	
	return voiceChat->GetVoiceState( user );
}

/*
========================
budSessionLocal::GetDisplayStateFromVoiceState
========================
*/
voiceStateDisplay_t budSessionLocal::GetDisplayStateFromVoiceState( voiceState_t voiceState ) const
{
	if( ( GetState() == GAME_LOBBY && MatchTypeIsLocal( GetGameLobby().GetMatchParms().matchFlags ) )
			|| ( GetState() == PARTY_LOBBY && MatchTypeIsLocal( GetPartyLobby().GetMatchParms().matchFlags ) ) )
	{
		return VOICECHAT_DISPLAY_NONE;	// never show voice stuff in splitscreen
	}
	
	switch( voiceState )
	{
		case VOICECHAT_STATE_MUTED_REMOTE:
		case VOICECHAT_STATE_MUTED_LOCAL:
		case VOICECHAT_STATE_MUTED_ALL:
			return VOICECHAT_DISPLAY_MUTED;
		case VOICECHAT_STATE_NOT_TALKING:
			return VOICECHAT_DISPLAY_NOTTALKING;
		case VOICECHAT_STATE_TALKING:
			return VOICECHAT_DISPLAY_TALKING;
		case VOICECHAT_STATE_TALKING_GLOBAL:
			return VOICECHAT_DISPLAY_TALKING_GLOBAL;
		case VOICECHAT_STATE_NO_MIC:
		default:
			return VOICECHAT_DISPLAY_NOTTALKING;
	}
}

/*
========================
budSessionLocal::ToggleLobbyUserVoiceMute
========================
*/
void budSessionLocal::ToggleLobbyUserVoiceMute( lobbyUserID_t lobbyUserID )
{
	idLobby* activeLobby = GetActivePlatformLobby();
	
	if( activeLobby == NULL )
	{
		return;
	}
	
	// Get the master local user
	idLocalUser* masterUser = signInManager->GetMasterLocalUser();
	
	if( masterUser == NULL )
	{
		return;
	}
	
	const lobbyUser_t* srcUser = activeLobby->GetLobbyUser( activeLobby->GetLobbyUserIndexByLocalUserHandle( masterUser->GetLocalUserHandle() ) );
	
	if( srcUser == NULL )
	{
		return;
	}
	
	const lobbyUser_t* targetUser = activeLobby->GetLobbyUserByID( lobbyUserID );
	
	if( !verify( targetUser != NULL ) )
	{
		return;
	}
	
	if( srcUser == targetUser )
	{
		return;		// Can't toggle yourself
	}
	
	voiceChat->ToggleMuteLocal( srcUser, targetUser );
}

/*
========================
budSessionLocal::UpdateMasterUserHeadsetState
========================
*/
void budSessionLocal::UpdateMasterUserHeadsetState()
{
	if( GetState() != PARTY_LOBBY && GetState() != GAME_LOBBY && GetState() != INGAME )
	{
		return;
	}
	
	lobbyUser_t* user = GetActivePlatformLobby()->GetSessionUserFromLocalUser( signInManager->GetMasterLocalUser() );
	
	// TODO: Is this possible?
	if( user == NULL )
	{
		return;
	}
	
	int talkerIndex = voiceChat->FindTalkerByUserId( user->lobbyUserID, GetActivePlatformLobby()->lobbyType );
	bool voiceChanged = voiceChat->HasHeadsetStateChanged( talkerIndex );
	
	if( voiceChanged )
	{
		byte buffer[ idPacketProcessor::MAX_MSG_SIZE ];
		budBitMsg msg( buffer, sizeof( buffer ) );
		msg.WriteLong( 1 );
		user->lobbyUserID.WriteToMsg( msg );
		msg.WriteBool( voiceChat->GetHeadsetState( talkerIndex ) );
		
		libBud::Printf( "Sending voicestate %d for user %d %s\n", voiceChat->GetHeadsetState( talkerIndex ), talkerIndex, user->gamertag );
		
		if( GetActivePlatformLobby()->IsHost() )
		{
			for( int p = 0; p < GetActivePlatformLobby()->peers.Num(); p++ )
			{
				if( GetActivePlatformLobby()->peers[p].IsConnected() )
				{
					GetActivePlatformLobby()->QueueReliableMessage( p, idLobby::RELIABLE_HEADSET_STATE, msg.GetReadData(), msg.GetSize() );
				}
			}
			
		}
		else
		{
			GetActivePlatformLobby()->QueueReliableMessage( GetActivePlatformLobby()->host, idLobby::RELIABLE_HEADSET_STATE, msg.GetReadData(), msg.GetSize() );
		}
	}
	
}

/*
========================
budSessionLocal::GetNumContentPackages
========================
*/
int	budSessionLocal::GetNumContentPackages() const
{
	return downloadedContent.Num();
}

/*
========================
budSessionLocal::GetContentPackageID
========================
*/
int budSessionLocal::GetContentPackageID( int contentIndex ) const
{
	assert( contentIndex < MAX_CONTENT_PACKAGES );
	
	if( downloadedContent[ contentIndex ].isMounted )
	{
		return downloadedContent[ contentIndex ].dlcID;
	}
	
	return 0;
}

/*
========================
budSessionLocal::GetContentPackagePath
========================
*/
const char* budSessionLocal::GetContentPackagePath( int contentIndex ) const
{
	assert( contentIndex < MAX_CONTENT_PACKAGES );
	
	if( downloadedContent[ contentIndex ].isMounted )
	{
		return downloadedContent[ contentIndex ].rootPath.c_str();
	}
	
	return NULL;
}

/*
========================
budSessionLocal::GetContentPackageIndexForID
========================
*/
int budSessionLocal::GetContentPackageIndexForID( int contentID ) const
{
	int contentIndex = -1;
	
	for( int i = 0; i < downloadedContent.Num(); i++ )
	{
		if( downloadedContent[i].dlcID == contentID )
		{
			contentIndex = i;
			break;
		}
	}
	
	return contentIndex;
}

/*
========================
budSessionLocal::SetLobbyUserRelativeScore
========================
*/
void budSessionLocal::SetLobbyUserRelativeScore( lobbyUserID_t lobbyUserID, int relativeScore, int team )
{
	// All platforms but 360 stub this out
}

/*
========================
budSessionLocal::ReadTitleStorage
========================
*/
void budSessionLocal::ReadTitleStorage( void* buffer, int bufferLen )
{
	// https://ps3.scedev.net/projects/ps3_sdk_docs/docs/ps3-en,NP_Lookup-Reference,sceNpLookupTitleSmallStorageAsync/1
	// If the file is not on the server, this will be handled as though a file of 0 bytes were on the server.
	// This means that 0 will be set to contentLength and 0 (for normal termination) will return for the return value.
	// This situation can occur with problems in actual operation, so the application must be designed not to hang up even in such situations
	//bufferLen = 0;
	
	libBud::Printf( "ReadTitleStorage: %i bytes\n", bufferLen );
	
#if !defined( ID_RETAIL ) || defined( ID_RETAIL_INTERNAL )
	if( net_ignoreTitleStorage.GetBool() )   //&& libBud::GetProduction() < PROD_PRODUCTION ) {
	{
		libBud::Printf( "ReadTitleStorage: *********************** IGNORING ********************\n" );
		return;
	}
#endif
	
	//budScopedGlobalHeap	everythingHereGoesInTheGlobalHeap;
	
	budParser parser( LEXFL_NOERRORS | LEXFL_NOFATALERRORS | LEXFL_NOSTRINGCONCAT );
	parser.LoadMemory( ( const char* )buffer, bufferLen, "default.tss" );
	
	bool valid = true;
	
	while( true )
	{
		budToken token;
		
		if( !parser.ReadToken( &token ) )
		{
			break;
		}
		
		if( token.Icmp( "netvars" ) == 0 )
		{
			if( !titleStorageVars.Parse( parser ) )
			{
				valid = false;
				break;
			}
		}
		else
		{
			valid = false;
			break;
		}
	}
	
	if( valid )
	{
		titleStorageLoaded = true;
		libBud::Printf( "ReadTitleStorage: SUCCESS\n" );
		titleStorageVars.Print();
	}
	else
	{
		titleStorageLoaded = false;
		libBud::Printf( "ReadTitleStorage: FAILED\n" );
		titleStorageVars.Clear();
	}
}

/*
========================
budSessionLocal::ReadDLCInfo
========================
*/
bool budSessionLocal::ReadDLCInfo( idDict& dlcInfo, void* buffer, int bufferLen )
{
	budParser parser( LEXFL_NOERRORS | LEXFL_NOFATALERRORS | LEXFL_NOSTRINGCONCAT );
	parser.LoadMemory( ( const char* )buffer, bufferLen, "info.txt" );
	
	bool valid = true;
	
	while( true )
	{
		budToken token;
		
		if( !parser.ReadToken( &token ) )
		{
			break;
		}
		
		if( token.Icmp( "dlcInfo" ) == 0 )
		{
			if( !dlcInfo.Parse( parser ) )
			{
				valid = false;
				break;
			}
		}
		else
		{
			valid = false;
			break;
		}
	}
	
	return valid;
}

/*
========================
budSessionLocal::IsPlatformPartyInLobby
========================
*/
bool budSessionLocal::IsPlatformPartyInLobby()
{
	idLocalUser* user = session->GetSignInManager().GetMasterLocalUser();
	idLobby* lobby = GetActivePlatformLobby();
	
	if( user == NULL || lobby == NULL )
	{
		return false;
	}
	
	if( user->GetPartyCount() > MAX_PLAYERS || user->GetPartyCount() < 2 )
	{
		return false;
	}
	
	// TODO: Implement PC
	return false;
}

/*
========================
budSessionLocal::PrePickNewHost
This is called when we have determined that we need to pick a new host.
Call PickNewHostInternal to continue on with the host picking process.
========================
*/
void budSessionLocal::PrePickNewHost( idLobby& lobby, bool forceMe, bool inviteOldHost )
{
	NET_VERBOSE_PRINT( "budSessionLocal::PrePickNewHost: (%s)\n", lobby.GetLobbyName() );
	
	if( GetActivePlatformLobby() == NULL )
	{
		NET_VERBOSE_PRINT( "budSessionLocal::PrePickNewHost: GetActivePlatformLobby() == NULL (%s)\n", lobby.GetLobbyName() );
		return;
	}
	
	// Check to see if we can migrate AT ALL
	// This is checking for coop, we should make this a specific option (MATCH_ALLOW_MIGRATION)
	if( GetPartyLobby().parms.matchFlags & MATCH_PARTY_INVITE_PLACEHOLDER )
	{
		NET_VERBOSE_PRINT( "budSessionLocal::PrePickNewHost: MATCH_PARTY_INVITE_PLACEHOLDER (%s)\n", lobby.GetLobbyName() );
		
		// Can't migrate, shut both lobbies down, and create a new match using the original parms
		GetGameStateLobby().Shutdown();
		GetGameLobby().Shutdown();
		GetPartyLobby().Shutdown();
		
		// Throw up the appropriate dialog message so the player knows what happeend
		if( localState >= budSessionLocal::STATE_LOADING )
		{
			NET_VERBOSE_PRINT( "budSessionLocal::PrePickNewHost: localState >= budSessionLocal::STATE_LOADING (%s)\n", lobby.GetLobbyName() );
		}
		else
		{
			NET_VERBOSE_PRINT( "budSessionLocal::PrePickNewHost: localState < budSessionLocal::STATE_LOADING (%s)\n", lobby.GetLobbyName() );
		}
		
		CreateMatch( GetActivePlatformLobby()->parms );
		
		return;
	}
	
	// Check to see if the match is searchable
	if( GetState() >= budSession::GAME_LOBBY && MatchTypeIsSearchable( GetGameLobby().parms.matchFlags ) )
	{
		NET_VERBOSE_PRINT( "budSessionLocal::PrePickNewHost: MatchTypeIsSearchable (%s)\n", lobby.GetLobbyName() );
		// Searchable games migrate lobbies independently, and don't need to stay in sync
		lobby.PickNewHostInternal( forceMe, inviteOldHost );
		return;
	}
	
	//
	// Beyond this point, game lobbies must be sync'd with party lobbies as far as host status
	// So to enforce that, we pull you out of the game lobby if you are in one when migration occurs
	//
	
	// Check to see if we should go back to a party lobby
	if( GetBackState() >= budSessionLocal::PARTY_LOBBY || GetState() == budSession::PARTY_LOBBY )
	{
		NET_VERBOSE_PRINT( "budSessionLocal::PrePickNewHost: GetBackState() >= budSessionLocal::PARTY_LOBBY || GetState() == budSession::PARTY_LOBBY (%s)\n", lobby.GetLobbyName() );
		// Force the party lobby to start picking a new host if we lost the game lobby host
		GetPartyLobby().PickNewHostInternal( forceMe, inviteOldHost );
		
		// End the game lobby, and go back to party lobby
		GetGameStateLobby().Shutdown();
		GetGameLobby().Shutdown();
		SetState( GetPartyLobby().IsHost() ? budSessionLocal::STATE_PARTY_LOBBY_HOST : budSessionLocal::STATE_PARTY_LOBBY_PEER );
	}
	else
	{
		NET_VERBOSE_PRINT( "budSessionLocal::PrePickNewHost: GetBackState() < budSessionLocal::PARTY_LOBBY && GetState() != budSession::PARTY_LOBBY (%s)\n", lobby.GetLobbyName() );
		
		// Go back to main menu
		GetGameLobby().Shutdown();
		GetGameStateLobby().Shutdown();
		GetPartyLobby().Shutdown();
		SetState( budSessionLocal::STATE_IDLE );
	}
}
/*
========================
budSessionLocal::PreMigrateInvite
This is called just before we get invited to a migrated session
If we return false, the invite will be ignored
========================
*/
bool budSessionLocal::PreMigrateInvite( idLobby& lobby )
{
	if( GetActivePlatformLobby() == NULL )
	{
		return false;
	}
	
	// Check to see if we can migrate AT ALL
	// This is checking for coop, we should make this a specific option (MATCH_ALLOW_MIGRATION)
	if( !verify( ( GetPartyLobby().parms.matchFlags & MATCH_PARTY_INVITE_PLACEHOLDER ) == 0 ) )
	{
		return false;	// Shouldn't get invites for coop (we should make this a specific option (MATCH_ALLOW_MIGRATION))
	}
	
	// Check to see if the match is searchable
	if( MatchTypeIsSearchable( GetGameLobby().parms.matchFlags ) )
	{
		// Searchable games migrate lobbies independently, and don't need to stay in sync
		return true;
	}
	
	//
	// Beyond this point, game lobbies must be sync'd with party lobbies as far as host status
	// So to enforce that, we pull you out of the game lobby if you are in one when migration occurs
	//
	
	if( lobby.lobbyType != idLobby::TYPE_PARTY )
	{
		return false;		// We shouldn't be getting invites from non party lobbies when in a non searchable game
	}
	
	// Non placeholder Party lobbies can always migrate
	if( GetBackState() >= budSessionLocal::PARTY_LOBBY )
	{
		// Non searchable games go back to the party lobby
		GetGameLobby().Shutdown();
		SetState( GetPartyLobby().IsHost() ? budSessionLocal::STATE_PARTY_LOBBY_HOST : budSessionLocal::STATE_PARTY_LOBBY_PEER );
	}
	
	return true;	// Non placeholder Party lobby invites joinable
}

/*
================================================================================================
lobbyAddress_t
================================================================================================
*/

/*
========================
lobbyAddress_t::lobbyAddress_t
========================
*/
lobbyAddress_t::lobbyAddress_t()
{
	memset( &netAddr, 0, sizeof( netAddr ) );
	netAddr.type = NA_BAD;
}

/*
========================
lobbyAddress_t::InitFromIPandPort
========================
*/
void lobbyAddress_t::InitFromIPandPort( const char* ip, int port )
{
	Sys_StringToNetAdr( ip, &netAddr, true );
	if( !netAddr.port )
	{
		netAddr.port = port;
	}
}


/*
========================
lobbyAddress_t::InitFromNetadr
========================
*/
void lobbyAddress_t::InitFromNetadr( const netadr_t& netadr )
{
	assert( netadr.type != NA_BAD );
	netAddr = netadr;
}

/*
========================
lobbyAddress_t::ToString
========================
*/
const char* lobbyAddress_t::ToString() const
{
	return Sys_NetAdrToString( netAddr );
}

/*
========================
lobbyAddress_t::UsingRelay
========================
*/
bool lobbyAddress_t::UsingRelay() const
{
	return false;
}

/*
========================
lobbyAddress_t::Compare
========================
*/
bool lobbyAddress_t::Compare( const lobbyAddress_t& addr, bool ignoreSessionCheck ) const
{
	return Sys_CompareNetAdrBase( netAddr, addr.netAddr );
}

/*
========================
lobbyAddress_t::WriteToMsg
========================
*/
void lobbyAddress_t::WriteToMsg( budBitMsg& msg ) const
{
	msg.WriteData( &netAddr, sizeof( netAddr ) );
}

/*
========================
lobbyAddress_t::ReadFromMsg
========================
*/
void lobbyAddress_t::ReadFromMsg( budBitMsg& msg )
{
	msg.ReadData( &netAddr, sizeof( netAddr ) );
}

/*
================================================================================================
idNetSessionPort
================================================================================================
*/

/*
========================
idNetSessionPort::idNetSessionPort
========================
*/
idNetSessionPort::idNetSessionPort() :
	forcePacketDropCurr( 0.0f ),
	forcePacketDropPrev( 0.0f )
{
}

/*
========================
idNetSessionPort::InitPort
========================
*/
bool idNetSessionPort::InitPort( int portNumber, bool useBackend )
{
	return UDP.InitForPort( portNumber );
}

/*
========================
idNetSessionPort::ReadRawPacket
========================
*/
bool idNetSessionPort::ReadRawPacket( lobbyAddress_t& from, void* data, int& size, int maxSize )
{
	bool result = UDP.GetPacket( from.netAddr, data, size, maxSize );
	
	static idRandom2 random( Sys_Milliseconds() );
	if( net_forceDrop.GetInteger() != 0 )
	{
		forcePacketDropCurr = random.RandomInt( 100 );
		if( net_forceDrop.GetInteger() >= forcePacketDropCurr )
		{
			return false;
		}
	}
	
	return result;
}

/*
========================
idNetSessionPort::SendRawPacket
========================
*/
void idNetSessionPort::SendRawPacket( const lobbyAddress_t& to, const void* data, int size )
{
	static idRandom2 random( Sys_Milliseconds() );
	if( net_forceDrop.GetInteger() != 0 && net_forceDrop.GetInteger() >= random.RandomInt( 100 ) )
	{
		return;
	}
	assert( size <= idPacketProcessor::MAX_FINAL_PACKET_SIZE );
	
	UDP.SendPacket( to.netAddr, data, size );
}

/*
========================
idNetSessionPort::IsOpen
========================
*/
bool idNetSessionPort::IsOpen()
{
	return UDP.IsOpen();
}

/*
========================
idNetSessionPort::Close
========================
*/
void idNetSessionPort::Close()
{
	UDP.Close();
}

/*
================================================================================================
Commands
================================================================================================
*/

//====================================================================================

CONSOLE_COMMAND( voicechat_mute, "TEMP", 0 )
{
	if( args.Argc() != 2 )
	{
		libBud::Printf( "Usage: voicechat_mute <user index>\n" );
		return;
	}
	
	int i = atoi( args.Argv( 1 ) );
	session->ToggleLobbyUserVoiceMute( session->GetActivePlatformLobbyBase().GetLobbyUserIdByOrdinal( i ) );
}

/*
========================
force_disconnect_all
========================
*/
CONSOLE_COMMAND( force_disconnect_all, "force disconnect on all users", 0 )
{
	session->GetSignInManager().RemoveAllLocalUsers();
}

/*
========================
void Net_DebugOutputSignedInUsers_f
========================
*/
void Net_DebugOutputSignedInUsers_f( const budCmdArgs& args )
{
	session->GetSignInManager().DebugOutputLocalUserInfo();
}

budCommandLink Net_DebugOutputSignedInUsers( "net_debugOutputSignedInUsers", Net_DebugOutputSignedInUsers_f, "Outputs all the local users and other debugging information from the sign in manager" );

/*
========================
void Net_RemoveUserFromLobby_f
========================
*/
void Net_RemoveUserFromLobby_f( const budCmdArgs& args )
{
	if( args.Argc() > 1 )
	{
		int localUserNum = atoi( args.Argv( 1 ) );
		if( localUserNum < session->GetSignInManager().GetNumLocalUsers() )
		{
			session->GetSignInManager().RemoveLocalUserByIndex( localUserNum );
		}
		else
		{
			libBud::Printf( "This user is not in the lobby\n" );
		}
	}
	else
	{
		libBud::Printf( "Usage: net_RemoveUserFromLobby <localUserNum>\n" );
	}
}

budCommandLink Net_RemoveUserFromLobby( "net_removeUserFromLobby", Net_RemoveUserFromLobby_f, "Removes the given user from the lobby" );

/*
========================
Net_dropClient
========================
*/
CONSOLE_COMMAND( Net_DropClient, "Drop a client", 0 )
{
	if( args.Argc() < 3 )
	{
		libBud::Printf( "usage: Net_DropClient <clientnum> [<session>] 0/default: drop from game, 1: drop from party, otherwise drop from both\n" );
		return;
	}
	int lobbyType = 0;
	if( args.Argc() > 2 )
	{
		lobbyType = atoi( args.Argv( 2 ) );
	}
	session->DropClient( atoi( args.Argv( 1 ) ), lobbyType );
}

/*
========================
budSessionLocal::DropClient
========================
*/
void budSessionLocal::DropClient( int peerNum, int session )
{
	if( session == 1 || session >= 2 )
	{
		GetPartyLobby().DisconnectPeerFromSession( peerNum );
	}
	if( session == 0 || session >= 2 )
	{
		GetGameLobby().DisconnectPeerFromSession( peerNum );
	}
}

/*
========================
budSessionLocal::ListServersCommon
========================
*/
void budSessionLocal::ListServersCommon()
{
	netadr_t broadcast;
	memset( &broadcast, 0, sizeof( broadcast ) );
	broadcast.type = NA_BROADCAST;
	broadcast.port = net_port.GetInteger();
	
	lobbyAddress_t address;
	address.InitFromNetadr( broadcast );
	
	byte buffer[ idPacketProcessor::MAX_PACKET_SIZE - 2 ];
	budBitMsg msg( buffer, sizeof( buffer ) );
	
	// Add the current version info to the query
	const unsigned int localChecksum = NetGetVersionChecksum(); // DG: use int instead of long for 64bit compatibility
	
	NET_VERBOSE_PRINT( "ListServers: Hash checksum: %u, broadcasting to: %s\n", localChecksum, address.ToString() );
	
	msg.WriteLong( localChecksum );
	
	GetPort();
	// Send the query as a broadcast
	GetPartyLobby().SendConnectionLess( address, idLobby::OOB_MATCH_QUERY, msg.GetReadData(), msg.GetSize() );
}

/*
========================
budSessionLocal::HandleDedicatedServerQueryRequest
========================
*/
void budSessionLocal::HandleDedicatedServerQueryRequest( lobbyAddress_t& remoteAddr, budBitMsg& msg, int msgType )
{
	NET_VERBOSE_PRINT( "HandleDedicatedServerQueryRequest from %s\n", remoteAddr.ToString() );
	
	bool canJoin = true;
	
	// DG: use int instead of long for 64bit compatibility
	const unsigned int localChecksum = NetGetVersionChecksum();
	const unsigned int remoteChecksum = msg.ReadLong();
	// DG end
	
	if( remoteChecksum != localChecksum )
	{
		NET_VERBOSE_PRINT( "HandleServerQueryRequest: Invalid version from %s\n", remoteAddr.ToString() );
		canJoin = false;
	}
	
	// Make sure we are the host of this party session
	if( !GetPartyLobby().IsHost() )
	{
		NET_VERBOSE_PRINT( "HandleServerQueryRequest: Not host of party\n" );
		canJoin = false;
	}
	
	// Make sure there is a session active
	if( GetActivePlatformLobby() == NULL )
	{
		canJoin = false;
	}
	
	// Make sure we have enough free slots
	if( GetPartyLobby().NumFreeSlots() == 0 || GetGameLobby().NumFreeSlots() == 0 )
	{
		NET_VERBOSE_PRINT( "No free slots\n" );
		canJoin = false;
	}
	
	if( MatchTypeInviteOnly( GetPartyLobby().parms.matchFlags ) )
	{
		canJoin = false;
	}
	
	// Buffer to hold reply msg
	byte buffer[ idPacketProcessor::MAX_PACKET_SIZE - 2 ];
	budBitMsg retmsg( buffer, sizeof( buffer ) );
	
	idLocalUser* masterUser = GetSignInManager().GetMasterLocalUser();
	
	if( masterUser == NULL && !net_headlessServer.GetBool() )
	{
		canJoin = false;
	}
	
	// Send the info about this game session to the caller
	retmsg.WriteBool( canJoin );
	
	if( canJoin )
	{
		serverInfo_t serverInfo;
		serverInfo.joinable = ( session->GetState() >= budSession::LOADING );
		
		if( !net_headlessServer.GetBool() )
		{
			serverInfo.serverName = masterUser->GetGamerTag();
		}
		
		if( GetGameLobby().IsLobbyActive() )
		{
			serverInfo.gameMap = GetGameLobby().parms.gameMap;
			serverInfo.gameMode = GetGameLobby().parms.gameMode;
		}
		else
		{
			serverInfo.gameMode = -1;
		}
		
		serverInfo.numPlayers = GetActivePlatformLobby()->GetNumLobbyUsers();
		serverInfo.maxPlayers = GetActivePlatformLobby()->parms.numSlots;
		serverInfo.Write( retmsg );
		
		for( int i = 0; i < GetActivePlatformLobby()->GetNumLobbyUsers(); i++ )
		{
			retmsg.WriteString( GetActivePlatformLobby()->GetLobbyUserName( GetActivePlatformLobby()->GetLobbyUserIdByOrdinal( i ) ) );
		}
	}
	
	// Send it
	GetPartyLobby().SendConnectionLess( remoteAddr, idLobby::OOB_MATCH_QUERY_ACK, retmsg.GetReadData(), retmsg.GetSize() );
}

/*
========================
budSessionLocal::HandleDedicatedServerQueryAck
========================
*/
void budSessionLocal::HandleDedicatedServerQueryAck( lobbyAddress_t& remoteAddr, budBitMsg& msg )
{
	NET_VERBOSE_PRINT( "HandleDedicatedServerQueryAck from %s\n", remoteAddr.ToString() );
	dedicatedServerSearch->HandleQueryAck( remoteAddr, msg );
}

/*
========================
budSessionLocal::ServerPlayerList
========================
*/
const budList< budStr >* budSessionLocal::ServerPlayerList( int i )
{
	return NULL;
}

/*
========================
lobbyUserID_t::Serialize
========================
*/
void lobbyUserID_t::Serialize( budSerializer& ser )
{
	localUserHandle.Serialize( ser );
	ser.Serialize( lobbyType );
}
