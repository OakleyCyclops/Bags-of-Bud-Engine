/*
================================================================================================
CONFIDENTIAL AND PROPRIETARY INFORMATION/NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
Copyright 2010 id Software LLC, a ZeniMax Media company. All Rights Reserved.
================================================================================================
*/

/*
================================================================================================

Contains the windows implementation of the network session

================================================================================================
*/

#pragma hdrstop
#include "PCH.hpp"

#include "../../framework/Common_local.h"
#include "../sys_session_local.h"
#include "../sys_stats.h"
#include "../sys_savegame.h"
#include "../sys_lobby_backend_direct.h"
#include "../sys_voicechat.h"
#include "achievements.h"
//#include "win_local.h"

/*
========================
Global variables
========================
*/

extern budCVar net_port;

class idLobbyToSessionCBLocal;

/*
========================
budSessionLocalWin::budSessionLocalWin
========================
*/
class budSessionLocalWin : public budSessionLocal
{
	friend class idLobbyToSessionCBLocal;
	
public:
	budSessionLocalWin();
	virtual ~budSessionLocalWin();
	
	// budSessionLocal interface
	virtual void		Initialize();
	virtual void		Shutdown();
	
	virtual void		InitializeSoundRelatedSystems();
	virtual void		ShutdownSoundRelatedSystems();
	
	virtual void		PlatformPump();
	
	virtual void		InviteFriends();
	virtual void		InviteParty();
	virtual void		ShowPartySessions();
	
	virtual void		ShowSystemMarketplaceUI() const;
	
	virtual void					ListServers( const idCallback& callback );
	virtual void					CancelListServers();
	virtual int						NumServers() const;
	virtual const serverInfo_t* 	ServerInfo( int i ) const;
	virtual void					ConnectToServer( int i );
	virtual void					ShowServerGamerCardUI( int i );
	
	virtual void			ShowLobbyUserGamerCardUI( lobbyUserID_t lobbyUserID );
	
	virtual void			ShowOnlineSignin() {}
	virtual void			UpdateRichPresence() {}
	virtual void			CheckVoicePrivileges() {}
	
	virtual bool			ProcessInputEvent( const sysEvent_t* ev );
	
	// System UI
	virtual bool			IsSystemUIShowing() const;
	virtual void			SetSystemUIShowing( bool show );
	
	// Invites
	virtual void			HandleBootableInvite( int64 lobbyId = 0 );
	virtual void			ClearBootableInvite();
	virtual void			ClearPendingInvite();
	
	virtual bool			HasPendingBootableInvite();
	virtual void			SetDiscSwapMPInvite( void* parm );
	virtual void* 			GetDiscSwapMPInviteParms();
	
	virtual void			EnumerateDownloadableContent();
	
	virtual void 			HandleServerQueryRequest( lobbyAddress_t& remoteAddr, budBitMsg& msg, int msgType );
	virtual void 			HandleServerQueryAck( lobbyAddress_t& remoteAddr, budBitMsg& msg );
	
	// Leaderboards
	virtual void			LeaderboardUpload( lobbyUserID_t lobbyUserID, const leaderboardDefinition_t* leaderboard, const column_t* stats, const budFile_Memory* attachment = NULL );
	virtual void			LeaderboardDownload( int sessionUserIndex, const leaderboardDefinition_t* leaderboard, int startingRank, int numRows, const idLeaderboardCallback& callback );
	virtual void			LeaderboardDownloadAttachment( int sessionUserIndex, const leaderboardDefinition_t* leaderboard, int64 attachmentID );
	
	// Scoring (currently just for TrueSkill)
	virtual void			SetLobbyUserRelativeScore( lobbyUserID_t lobbyUserID, int relativeScore, int team ) {}
	
	virtual void			LeaderboardFlush();
	
	virtual idNetSessionPort& 	GetPort( bool dedicated = false );
	virtual idLobbyBackend* 	CreateLobbyBackend( const budMatchParameters& p, float skillLevel, idLobbyBackend::lobbyBackendType_t lobbyType );
	virtual idLobbyBackend* 	FindLobbyBackend( const budMatchParameters& p, int numPartyUsers, float skillLevel, idLobbyBackend::lobbyBackendType_t lobbyType );
	virtual idLobbyBackend* 	JoinFromConnectInfo( const lobbyConnectInfo_t& connectInfo , idLobbyBackend::lobbyBackendType_t lobbyType );
	virtual void				DestroyLobbyBackend( idLobbyBackend* lobbyBackend );
	virtual void				PumpLobbies();
	virtual void				JoinAfterSwap( void* joinID );
	
	virtual bool				GetLobbyAddressFromNetAddress( const netadr_t& netAddr, lobbyAddress_t& outAddr ) const;
	virtual bool				GetNetAddressFromLobbyAddress( const lobbyAddress_t& lobbyAddress, netadr_t& outNetAddr ) const;
	
public:
	void	Connect_f( const budCmdArgs& args );
	
private:
	void					EnsurePort();
	
	idLobbyBackend* 		CreateLobbyInternal( idLobbyBackend::lobbyBackendType_t lobbyType );
	
	budArray< idLobbyBackend*, 3 > lobbyBackends;
	
	idNetSessionPort		port;
	bool					canJoinLocalHost;
	
	idLobbyToSessionCBLocal*	 lobbyToSessionCB;
};

budSessionLocalWin sessionLocalWin;
budSession* session = &sessionLocalWin;

/*
========================
idLobbyToSessionCBLocal
========================
*/
class idLobbyToSessionCBLocal : public idLobbyToSessionCB
{
public:
	idLobbyToSessionCBLocal( budSessionLocalWin* sessionLocalWin_ ) : sessionLocalWin( sessionLocalWin_ ) { }
	
	virtual bool CanJoinLocalHost() const
	{
		sessionLocalWin->EnsurePort();
		return sessionLocalWin->canJoinLocalHost;
	}
	virtual class idLobbyBackend* GetLobbyBackend( idLobbyBackend::lobbyBackendType_t type ) const
	{
		return sessionLocalWin->lobbyBackends[ type ];
	}
	
private:
	budSessionLocalWin* 			sessionLocalWin;
};

idLobbyToSessionCBLocal lobbyToSessionCBLocal( &sessionLocalWin );
idLobbyToSessionCB* lobbyToSessionCB = &lobbyToSessionCBLocal;

class idVoiceChatMgrWin : public idVoiceChatMgr
{
public:
	virtual bool	GetLocalChatDataInternal( int talkerIndex, byte* data, int& dataSize )
	{
		return false;
	}
	virtual void	SubmitIncomingChatDataInternal( int talkerIndex, const byte* data, int dataSize ) { }
	virtual bool	TalkerHasData( int talkerIndex )
	{
		return false;
	}
	virtual bool	RegisterTalkerInternal( int index )
	{
		return true;
	}
	virtual void	UnregisterTalkerInternal( int index ) { }
};

/*
========================
budSessionLocalWin::budSessionLocalWin
========================
*/
budSessionLocalWin::budSessionLocalWin()
{
	signInManager		= new( TAG_SYSTEM ) idSignInManagerWin;
	saveGameManager		= new( TAG_SAVEGAMES ) idSaveGameManager();
	voiceChat			= new( TAG_SYSTEM ) idVoiceChatMgrWin();
	lobbyToSessionCB	= new( TAG_SYSTEM ) idLobbyToSessionCBLocal( this );
	
	canJoinLocalHost	= false;
	
	lobbyBackends.Zero();
}

/*
========================
budSessionLocalWin::budSessionLocalWin
========================
*/
budSessionLocalWin::~budSessionLocalWin()
{
	delete voiceChat;
	delete lobbyToSessionCB;
}

/*
========================
budSessionLocalWin::Initialize
========================
*/
void budSessionLocalWin::Initialize()
{
	budSessionLocal::Initialize();
	
	// The shipping path doesn't load title storage
	// Instead, we inject values through code which is protected through steam DRM
	titleStorageVars.Set( "MAX_PLAYERS_ALLOWED", "8" );
	titleStorageLoaded = true;
	
	// First-time check for downloadable content once game is launched
	EnumerateDownloadableContent();
	
	GetPartyLobby().Initialize( idLobby::TYPE_PARTY, sessionCallbacks );
	GetGameLobby().Initialize( idLobby::TYPE_GAME, sessionCallbacks );
	GetGameStateLobby().Initialize( idLobby::TYPE_GAME_STATE, sessionCallbacks );
	
	achievementSystem = new( TAG_SYSTEM ) budAchievementSystemWin();
	achievementSystem->Init();
}

/*
========================
budSessionLocalWin::Shutdown
========================
*/
void budSessionLocalWin::Shutdown()
{
	NET_VERBOSE_PRINT( "NET: Shutdown\n" );
	budSessionLocal::Shutdown();
	
	MoveToMainMenu();
	
	// Wait until we fully shutdown
	while( localState != STATE_IDLE && localState != STATE_PRESS_START )
	{
		Pump();
	}
	
	if( achievementSystem != NULL )
	{
		achievementSystem->Shutdown();
		delete achievementSystem;
		achievementSystem = NULL;
	}
}

/*
========================
budSessionLocalWin::InitializeSoundRelatedSystems
========================
*/
void budSessionLocalWin::InitializeSoundRelatedSystems()
{
	if( voiceChat != NULL )
	{
		voiceChat->Init( NULL );
	}
}

/*
========================
budSessionLocalWin::ShutdownSoundRelatedSystems
========================
*/
void budSessionLocalWin::ShutdownSoundRelatedSystems()
{
	if( voiceChat != NULL )
	{
		voiceChat->Shutdown();
	}
}

/*
========================
budSessionLocalWin::PlatformPump
========================
*/
void budSessionLocalWin::PlatformPump()
{
}

/*
========================
budSessionLocalWin::InviteFriends
========================
*/
void budSessionLocalWin::InviteFriends()
{
}

/*
========================
budSessionLocalWin::InviteParty
========================
*/
void budSessionLocalWin::InviteParty()
{
}

/*
========================
budSessionLocalWin::ShowPartySessions
========================
*/
void budSessionLocalWin::ShowPartySessions()
{
}

/*
========================
budSessionLocalWin::ShowSystemMarketplaceUI
========================
*/
void budSessionLocalWin::ShowSystemMarketplaceUI() const
{
}

/*
========================
budSessionLocalWin::ListServers
========================
*/
void budSessionLocalWin::ListServers( const idCallback& callback )
{
	ListServersCommon();
}

/*
========================
budSessionLocalWin::CancelListServers
========================
*/
void budSessionLocalWin::CancelListServers()
{
}

/*
========================
budSessionLocalWin::NumServers
========================
*/
int budSessionLocalWin::NumServers() const
{
	return 0;
}

/*
========================
budSessionLocalWin::ServerInfo
========================
*/
const serverInfo_t* budSessionLocalWin::ServerInfo( int i ) const
{
	return NULL;
}

/*
========================
budSessionLocalWin::ConnectToServer
========================
*/
void budSessionLocalWin::ConnectToServer( int i )
{
}

/*
========================
budSessionLocalWin::Connect_f
========================
*/
void budSessionLocalWin::Connect_f( const budCmdArgs& args )
{
	if( args.Argc() < 2 )
	{
		libBud::Printf( "Usage: Connect to IP. Use IP:Port to specify port (e.g. 10.0.0.1:1234) \n" );
		return;
	}
	
	Cancel();
	
	if( signInManager->GetMasterLocalUser() == NULL )
	{
		signInManager->RegisterLocalUser( 0 );
	}
	
	lobbyConnectInfo_t connectInfo;
	
	Sys_StringToNetAdr( args.Argv( 1 ), &connectInfo.netAddr, true );
	// DG: don't use net_port to select port to connect to
	//     the port can be specified in the command, else the default port is used
	if( connectInfo.netAddr.port == 0 )
	{
		connectInfo.netAddr.port = 27015;
	}
	// DG end
	
	ConnectAndMoveToLobby( GetPartyLobby(), connectInfo, false );
}

/*
========================
void Connect_f
========================
*/
CONSOLE_COMMAND( connect, "Connect to the specified IP", NULL )
{
	sessionLocalWin.Connect_f( args );
}

/*
========================
budSessionLocalWin::ShowServerGamerCardUI
========================
*/
void budSessionLocalWin::ShowServerGamerCardUI( int i )
{
}

/*
========================
budSessionLocalWin::ShowLobbyUserGamerCardUI(
========================
*/
void budSessionLocalWin::ShowLobbyUserGamerCardUI( lobbyUserID_t lobbyUserID )
{
}

/*
========================
budSessionLocalWin::ProcessInputEvent
========================
*/
bool budSessionLocalWin::ProcessInputEvent( const sysEvent_t* ev )
{
	if( GetSignInManager().ProcessInputEvent( ev ) )
	{
		return true;
	}
	return false;
}

/*
========================
budSessionLocalWin::IsSystemUIShowing
========================
*/
bool budSessionLocalWin::IsSystemUIShowing() const
{
	// DG: pausing here when window is out of focus like originally done on windows is hacky
	// it's done with com_pause now.
	return isSysUIShowing;
}

/*
========================
budSessionLocalWin::SetSystemUIShowing
========================
*/
void budSessionLocalWin::SetSystemUIShowing( bool show )
{
	isSysUIShowing = show;
}

/*
========================
budSessionLocalWin::HandleServerQueryRequest
========================
*/
void budSessionLocalWin::HandleServerQueryRequest( lobbyAddress_t& remoteAddr, budBitMsg& msg, int msgType )
{
	NET_VERBOSE_PRINT( "HandleServerQueryRequest from %s\n", remoteAddr.ToString() );
}

/*
========================
budSessionLocalWin::HandleServerQueryAck
========================
*/
void budSessionLocalWin::HandleServerQueryAck( lobbyAddress_t& remoteAddr, budBitMsg& msg )
{
	NET_VERBOSE_PRINT( "HandleServerQueryAck from %s\n", remoteAddr.ToString() );
	
}

/*
========================
budSessionLocalWin::ClearBootableInvite
========================
*/
void budSessionLocalWin::ClearBootableInvite()
{
}

/*
========================
budSessionLocalWin::ClearPendingInvite
========================
*/
void budSessionLocalWin::ClearPendingInvite()
{
}

/*
========================
budSessionLocalWin::HandleBootableInvite
========================
*/
void budSessionLocalWin::HandleBootableInvite( int64 lobbyId )
{
}

/*
========================
budSessionLocalWin::HasPendingBootableInvite
========================
*/
bool budSessionLocalWin::HasPendingBootableInvite()
{
	return false;
}

/*
========================
budSessionLocal::SetDiscSwapMPInvite
========================
*/
void budSessionLocalWin::SetDiscSwapMPInvite( void* parm )
{
}

/*
========================
budSessionLocal::GetDiscSwapMPInviteParms
========================
*/
void* budSessionLocalWin::GetDiscSwapMPInviteParms()
{
	return NULL;
}

/*
========================
budSessionLocalWin::EnumerateDownloadableContent
========================
*/
void budSessionLocalWin::EnumerateDownloadableContent()
{
}

/*
========================
budSessionLocalWin::LeaderboardUpload
========================
*/
void budSessionLocalWin::LeaderboardUpload( lobbyUserID_t lobbyUserID, const leaderboardDefinition_t* leaderboard, const column_t* stats, const budFile_Memory* attachment )
{
}

/*
========================
budSessionLocalWin::LeaderboardFlush
========================
*/
void budSessionLocalWin::LeaderboardFlush()
{
}

/*
========================
budSessionLocalWin::LeaderboardDownload
========================
*/
void budSessionLocalWin::LeaderboardDownload( int sessionUserIndex, const leaderboardDefinition_t* leaderboard, int startingRank, int numRows, const idLeaderboardCallback& callback )
{
}

/*
========================
budSessionLocalWin::LeaderboardDownloadAttachment
========================
*/
void budSessionLocalWin::LeaderboardDownloadAttachment( int sessionUserIndex, const leaderboardDefinition_t* leaderboard, int64 attachmentID )
{
}

/*
========================
budSessionLocalWin::EnsurePort
========================
*/
void budSessionLocalWin::EnsurePort()
{
	// Init the port using reqular sockets
	if( port.IsOpen() )
	{
		return;		// Already initialized
	}
	
	if( port.InitPort( net_port.GetInteger(), false ) )
	{
		// TODO: what about canJoinLocalHost when running two instances with different net_port values?
		canJoinLocalHost = false;
	}
	else
	{
		// Assume this is another instantiation on the same machine, and just init using any available port
		port.InitPort( PORT_ANY, false );
		canJoinLocalHost = true;
	}
}

/*
========================
budSessionLocalWin::GetPort
========================
*/
idNetSessionPort& budSessionLocalWin::GetPort( bool dedicated )
{
	EnsurePort();
	return port;
}

/*
========================
budSessionLocalWin::CreateLobbyBackend
========================
*/
idLobbyBackend* budSessionLocalWin::CreateLobbyBackend( const budMatchParameters& p, float skillLevel, idLobbyBackend::lobbyBackendType_t lobbyType )
{
	idLobbyBackend* lobbyBackend = CreateLobbyInternal( lobbyType );
	lobbyBackend->StartHosting( p, skillLevel, lobbyType );
	return lobbyBackend;
}

/*
========================
budSessionLocalWin::FindLobbyBackend
========================
*/
idLobbyBackend* budSessionLocalWin::FindLobbyBackend( const budMatchParameters& p, int numPartyUsers, float skillLevel, idLobbyBackend::lobbyBackendType_t lobbyType )
{
	idLobbyBackend* lobbyBackend = CreateLobbyInternal( lobbyType );
	lobbyBackend->StartFinding( p, numPartyUsers, skillLevel );
	return lobbyBackend;
}

/*
========================
budSessionLocalWin::JoinFromConnectInfo
========================
*/
idLobbyBackend* budSessionLocalWin::JoinFromConnectInfo( const lobbyConnectInfo_t& connectInfo, idLobbyBackend::lobbyBackendType_t lobbyType )
{
	idLobbyBackend* lobbyBackend = CreateLobbyInternal( lobbyType );
	lobbyBackend->JoinFromConnectInfo( connectInfo );
	return lobbyBackend;
}

/*
========================
budSessionLocalWin::DestroyLobbyBackend
========================
*/
void budSessionLocalWin::DestroyLobbyBackend( idLobbyBackend* lobbyBackend )
{
	assert( lobbyBackend != NULL );
	assert( lobbyBackends[lobbyBackend->GetLobbyType()] == lobbyBackend );
	
	lobbyBackends[lobbyBackend->GetLobbyType()] = NULL;
	
	lobbyBackend->Shutdown();
	delete lobbyBackend;
}

/*
========================
budSessionLocalWin::PumpLobbies
========================
*/
void budSessionLocalWin::PumpLobbies()
{
	assert( lobbyBackends[idLobbyBackend::TYPE_PARTY] == NULL || lobbyBackends[idLobbyBackend::TYPE_PARTY]->GetLobbyType() == idLobbyBackend::TYPE_PARTY );
	assert( lobbyBackends[idLobbyBackend::TYPE_GAME] == NULL || lobbyBackends[idLobbyBackend::TYPE_GAME]->GetLobbyType() == idLobbyBackend::TYPE_GAME );
	assert( lobbyBackends[idLobbyBackend::TYPE_GAME_STATE] == NULL || lobbyBackends[idLobbyBackend::TYPE_GAME_STATE]->GetLobbyType() == idLobbyBackend::TYPE_GAME_STATE );
	
	// Pump lobbyBackends
	for( int i = 0; i < lobbyBackends.Num(); i++ )
	{
		if( lobbyBackends[i] != NULL )
		{
			lobbyBackends[i]->Pump();
		}
	}
}

/*
========================
budSessionLocalWin::CreateLobbyInternal
========================
*/
idLobbyBackend* budSessionLocalWin::CreateLobbyInternal( idLobbyBackend::lobbyBackendType_t lobbyType )
{
	EnsurePort();
	idLobbyBackend* lobbyBackend = new( TAG_NETWORKING ) idLobbyBackendDirect();
	
	lobbyBackend->SetLobbyType( lobbyType );
	
	assert( lobbyBackends[lobbyType] == NULL );
	lobbyBackends[lobbyType] = lobbyBackend;
	
	return lobbyBackend;
}

/*
========================
budSessionLocalWin::JoinAfterSwap
========================
*/
void budSessionLocalWin::JoinAfterSwap( void* joinID )
{
}

/*
========================
budSessionLocalWin::GetLobbyAddressFromNetAddress
========================
*/
bool budSessionLocalWin::GetLobbyAddressFromNetAddress( const netadr_t& netAddr, lobbyAddress_t& outAddr ) const
{
	return false;
}

/*
========================
budSessionLocalWin::GetNetAddressFromLobbyAddress
========================
*/
bool budSessionLocalWin::GetNetAddressFromLobbyAddress( const lobbyAddress_t& lobbyAddress, netadr_t& outNetAddr ) const
{
	return false;
}
