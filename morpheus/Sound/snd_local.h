/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2013-2016 Robert Beckebans
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

#ifndef __SND_LOCAL_H__
#define __SND_LOCAL_H__

#include "WaveFile.h"

// Maximum number of voices we can have allocated
#define MAX_HARDWARE_VOICES 48

// A single voice can play multiple channels (up to 5.1, but most commonly stereo)
// This is the maximum number of channels which can play simultaneously
// This is limited primarily by seeking on the optical drive, secondarily by memory consumption, and tertiarily by CPU time spent mixing
#define MAX_HARDWARE_CHANNELS 64

// We may need up to 3 buffers for each hardware voice if they are all long sounds
#define MAX_SOUND_BUFFERS ( MAX_HARDWARE_VOICES * 3 )

// Maximum number of channels in a sound sample
#define MAX_CHANNELS_PER_VOICE	8

/*
========================
MsecToSamples
SamplesToMsec
========================
*/
BUD_INLINE_EXTERN uint32 MsecToSamples( uint32 msec, uint32 sampleRate )
{
	return ( msec * ( sampleRate / 100 ) ) / 10;
}
BUD_INLINE_EXTERN uint32 SamplesToMsec( uint32 samples, uint32 sampleRate )
{
	return sampleRate < 100 ? 0 : ( samples * 10 ) / ( sampleRate / 100 );
}

/*
========================
DBtoLinear
LinearToDB
========================
*/
BUD_INLINE_EXTERN float DBtoLinear( float db )
{
	return budMath::Pow( 2.0f, db * ( 1.0f / 6.0f ) );
}
BUD_INLINE_EXTERN float LinearToDB( float linear )
{
	return ( linear > 0.0f ) ? ( budMath::Log( linear ) * ( 6.0f / 0.693147181f ) ) : -999.0f;
}

// demo sound commands
typedef enum
{
	SCMD_STATE,				// followed by a load game state
	SCMD_PLACE_LISTENER,
	SCMD_ALLOC_EMITTER,
	SCMD_FREE,
	SCMD_UPDATE,
	SCMD_START,
	SCMD_MODIFY,
	SCMD_STOP,
	SCMD_FADE,
	SCMD_CACHESOUNDSHADER,
} soundDemoCommand_t;

#include "SoundVoice.h"

//#define AL_ALEXT_PROTOTYPES

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include "OpenAL/AL_SoundSample.h"
#include "OpenAL/AL_SoundVoice.h"
#include "OpenAL/AL_SoundHardware.h"

BUD_INLINE_EXTERN ALenum CheckALErrors_( const char* filename, int line )
{
	ALenum err = alGetError();
	if( err != AL_NO_ERROR )
	{
		libBud::Printf( "OpenAL Error: %s (0x%x), @ %s %d\n", alGetString( err ), err, filename, line );
	}
	return err;
}
#define CheckALErrors() CheckALErrors_(__FILE__, __LINE__)

BUD_INLINE_EXTERN ALCenum CheckALCErrors_( ALCdevice* device, const char* filename, int linenum )
{
	ALCenum err = alcGetError( device );
	if( err != ALC_NO_ERROR )
	{
		libBud::Printf( "ALC Error: %s (0x%x), @ %s %d\n", alcGetString( device, err ), err, filename, linenum );
	}
	return err;
}
#define CheckALCErrors(x) CheckALCErrors_((x), __FILE__, __LINE__)



//------------------------
// Listener data
//------------------------
struct listener_t
{
	budMat3	axis;		// orientation of the listener
	budVec3	pos;		// position in meters
	int		id;			// the entity number, used to detect when a sound is local
	int		area;		// area number the listener is in
};

class idSoundFade
{
public:
	int		fadeStartTime;
	int		fadeEndTime;
	float	fadeStartVolume;
	float	fadeEndVolume;
	
	
public:
	idSoundFade()
	{
		Clear();
	}
	
	void	Clear();
	void	SetVolume( float to );
	void	Fade( float to, int length, int soundTime );
	
	float	GetVolume( int soundTime ) const;
};

/*
================================================
idSoundChannel
================================================
*/
class idSoundChannel
{
public:
	bool	CanMute() const;
	
	void	Mute();
	bool	CheckForCompletion( int currentTime );
	
	void	UpdateVolume( int currentTime );
	void	UpdateHardware( float volumeAdd, int currentTime );
	
	// returns true if this channel is marked as looping
	bool	IsLooping() const;
	
	class idSoundEmitterLocal* 	emitter;
	
	int						startTime;
	int						endTime;
	int						logicalChannel;
	bool					allowSlow;
	
	soundShaderParms_t		parms;				// combines shader parms and per-channel overrides
	const idSoundShader* 	soundShader;
	idSoundSample* 			leadinSample;
	idSoundSample* 			loopingSample;
	idSoundFade				volumeFade;
	
	float					volumeDB;			// last volume at which this channel will play (calculated in UpdateVolume)
	float					currentAmplitude;	// current amplitude on the hardware voice
	
	// hardwareVoice will be freed and NULL'd when a sound is out of range,
	// and reallocated when it comes back in range
	idSoundVoice* 			hardwareVoice;
	
	// only allocated by the soundWorld block allocator
	idSoundChannel();
	~idSoundChannel();
};

// Maximum number of SoundChannels for a single SoundEmitter.
// This is probably excessive...
const int MAX_CHANNELS_PER_EMITTER = 16;

/*
===================================================================================

budSoundWorldLocal

===================================================================================
*/

class budSoundWorldLocal : public budSoundWorld
{
public:
	budSoundWorldLocal();
	virtual					~budSoundWorldLocal();
	
	//------------------------
	// Functions from budSoundWorld, implemented in SoundWorld.cpp
	//------------------------
	
	// Called at map start
	virtual void			ClearAllSoundEmitters();
	
	// stop all playing sounds
	virtual void			StopAllSounds();
	
	// get a new emitter that can play sounds in this world
	virtual idSoundEmitter* AllocSoundEmitter();
	
	// for load games
	virtual idSoundEmitter* EmitterForIndex( int index );
	
	// query data from all emitters in the world
	virtual float			CurrentShakeAmplitude();
	
	// where is the camera
	virtual void			PlaceListener( const budVec3& origin, const budMat3& axis, const int listenerId );
	
	virtual void			WriteSoundShaderLoad( const idSoundShader* snd );
	
	// fade all sounds in the world with a given shader soundClass
	// to is in Db, over is in seconds
	virtual void			FadeSoundClasses( const int soundClass, const float to, const float over );
	
	// dumps the current state and begins archiving commands
	virtual void			StartWritingDemo( budDemoFile* demo );
	virtual void			StopWritingDemo();
	
	// read a sound command from a demo file
	virtual void			ProcessDemoCommand( budDemoFile* readDemo );
	
	// menu sounds
	virtual int				PlayShaderDirectly( const char* name, int channel = -1 );
	
	virtual void			Skip( int time );
	
	virtual void			Pause();
	virtual void			UnPause();
	virtual bool			IsPaused()
	{
		return isPaused;
	}
	
	virtual int				GetSoundTime();
	
	// avidump
	virtual void			AVIOpen( const char* path, const char* name );
	virtual void			AVIClose();
	
	// SaveGame Support
	virtual void			WriteToSaveGame( budFile* savefile );
	virtual void			ReadFromSaveGame( budFile* savefile );
	
	virtual void			SetSlowmoSpeed( float speed );
	virtual void			SetEnviroSuit( bool active );
	
	//=======================================
	
	//------------------------
	// Random stuff that's not exposed outside the sound system
	//------------------------
	void			Update();
	void			OnReloadSound( const budDecl* decl );
	
	idSoundChannel* 	AllocSoundChannel();
	void				FreeSoundChannel( idSoundChannel* );
	
public:
	// even though all these variables are public, nobody outside the sound system includes SoundWorld_local.h
	// so this is equivalent to making it private and friending all the other classes in the sound system
	
	idSoundFade			volumeFade;						// master volume knob for the entire world
	idSoundFade			soundClassFade[SOUND_MAX_CLASSES];
	
	budRenderWorld* 		renderWorld;	// for debug visualization and light amplitude sampling
	budDemoFile* 		writeDemo;		// if not NULL, archive commands here
	
	float				currentCushionDB;	// channels at or below this level will be faded to 0
	float				shakeAmp;			// last calculated shake amplitude
	
	listener_t			listener;
	budList<idSoundEmitterLocal*, TAG_AUDIO>	emitters;
	
	idSoundEmitter* 	localSound;			// for PlayShaderDirectly()
	
	idBlockAlloc<idSoundEmitterLocal, 16>	emitterAllocator;
	idBlockAlloc<idSoundChannel, 16>		channelAllocator;
	
	idSoundFade				pauseFade;
	int						pausedTime;
	int						accumulatedPauseTime;
	bool					isPaused;
	
	float					slowmoSpeed;
	bool					enviroSuitActive;
	
public:
	struct soundPortalTrace_t
	{
		int		portalArea;
		const soundPortalTrace_t* prevStack;
	};
	
	void			ResolveOrigin( const int stackDepth, const soundPortalTrace_t* prevStack, const int soundArea, const float dist, const budVec3& soundOrigin, idSoundEmitterLocal* def );
};


/*
================================================
idSoundEmitterLocal
================================================
*/
class idSoundEmitterLocal : public idSoundEmitter
{
public:
	virtual void	Free( bool immediate );
	
	virtual void	Reset();
	
	virtual void	UpdateEmitter( const budVec3& origin, int listenerId, const soundShaderParms_t* parms );
	
	virtual int		StartSound( const idSoundShader* shader, const s_channelType channel, float diversity = 0, int shaderFlags = 0, bool allowSlow = true );
	
	virtual void	ModifySound( const s_channelType channel, const soundShaderParms_t* parms );
	virtual void	StopSound( const s_channelType channel );
	
	virtual void	FadeSound( const s_channelType channel, float to, float over );
	
	virtual bool	CurrentlyPlaying( const s_channelType channel = SCHANNEL_ANY ) const;
	
	virtual	float	CurrentAmplitude();
	
	virtual	int		Index() const;
	
	//----------------------------------------------
	
	void			Init( int i, budSoundWorldLocal* sw );
	
	// Returns true if the emitter should be freed.
	bool			CheckForCompletion( int currentTime );
	
	void			OverrideParms( const soundShaderParms_t* base, const soundShaderParms_t* over, soundShaderParms_t* out );
	
	void			Update( int currentTime );
	void			OnReloadSound( const budDecl* decl );
	
	//----------------------------------------------
	
	budSoundWorldLocal* 		soundWorld;						// the world that holds this emitter
	
	int			index;							// in world emitter list
	bool		canFree;						// if true, this emitter can be canFree (once channels.Num() == 0)
	
	// a single soundEmitter can have many channels playing from the same point
	budStaticList<idSoundChannel*, MAX_CHANNELS_PER_EMITTER> channels;
	
	//----- set by UpdateEmitter -----
	budVec3				origin;
	soundShaderParms_t	parms;
	int					emitterId;						// sounds will be full volume when emitterId == listenerId
	
	//----- set by Update -----
	int			lastValidPortalArea;
	float		directDistance;
	float		spatializedDistance;
	budVec3		spatializedOrigin;
	
	// sound emitters are only allocated by the soundWorld block allocator
	idSoundEmitterLocal();
	virtual			~idSoundEmitterLocal();
};


/*
===================================================================================

idSoundSystemLocal

===================================================================================
*/
class idSoundSystemLocal : public idSoundSystem
{
public:
	// all non-hardware initialization
	virtual void			Init();
	
	// shutdown routine
	virtual	void			Shutdown();
	
	virtual budSoundWorld* 	AllocSoundWorld( budRenderWorld* rw );
	virtual void			FreeSoundWorld( budSoundWorld* sw );
	
	// specifying NULL will cause silence to be played
	virtual void			SetPlayingSoundWorld( budSoundWorld* soundWorld );
	
	// some tools, like the sound dialog, may be used in both the game and the editor
	// This can return NULL, so check!
	virtual budSoundWorld* 	GetPlayingSoundWorld();
	
	// sends the current playing sound world information to the sound hardware
	virtual void			Render();
	
	// Mutes the SSG_MUSIC group
	virtual void			MuteBackgroundMusic( bool mute )
	{
		musicMuted = mute;
	}
	
	// sets the final output volume to 0
	// This should only be used when the app is deactivated
	// Since otherwise there will be problems with different subsystems muting and unmuting at different times
	virtual void			SetMute( bool mute )
	{
		muted = mute;
	}
	virtual bool			IsMuted()
	{
		return muted;
	}
	
	virtual void			OnReloadSound( const budDecl* sound );
	
	virtual void			StopAllSounds();
	
	virtual void			InitStreamBuffers();
	virtual void			FreeStreamBuffers();
	
	virtual void* 			GetIXAudio2() const; // FIXME: stupid name; get rid of this? not sure if it's really needed..
	
	// RB begin
	virtual void*			GetOpenALDevice() const;
	// RB end
	
	// for the sound level meter window
	virtual cinData_t		ImageForTime( const int milliseconds, const bool waveform );
	
	// Free all sounds loaded during the last map load
	virtual	void			BeginLevelLoad();
	
	// We might want to defer the loading of new sounds to this point
	virtual	void			EndLevelLoad();
	
	// prints memory info
	virtual void			PrintMemInfo( MemInfo_t* mi );
	
	//-------------------------
	
	// Before a sound is reloaded, any active voices using it must
	// be stopped.  Returns true if any were playing, and should be
	// restarted after the sound is reloaded.
	void					StopVoicesWithSample( const idSoundSample* const sample );
	
	void					Restart();
	void					SetNeedsRestart()
	{
		needsRestart = true;
	}
	
	int						SoundTime() const;
	
	// may return NULL if there are no more voices left
	idSoundVoice* 			AllocateVoice( const idSoundSample* leadinSample, const idSoundSample* loopingSample );
	void					FreeVoice( idSoundVoice* );
	
	idSoundSample* 			LoadSample( const char* name );
	
	virtual void			Preload( idPreloadManifest& preload );
	
	struct bufferContext_t
	{
		bufferContext_t() :
			voice( NULL ),
			sample( NULL ),
			bufferNumber( 0 )
		{ }
		idSoundVoice_OpenAL* 	voice;
		idSoundSample_OpenAL*	sample;
		
		int bufferNumber;
	};
	
	// Get a stream buffer from the free pool, returns NULL if none are available
	bufferContext_t* 			ObtainStreamBufferContext();
	void						ReleaseStreamBufferContext( bufferContext_t* p );
	
	budSysMutex					streamBufferMutex;
	budStaticList< bufferContext_t*, MAX_SOUND_BUFFERS > freeStreamBufferContexts;
	budStaticList< bufferContext_t*, MAX_SOUND_BUFFERS > activeStreamBufferContexts;
	budStaticList< bufferContext_t, MAX_SOUND_BUFFERS > bufferContexts;
	
	budSoundWorldLocal* 			currentSoundWorld;
	budStaticList<budSoundWorldLocal*, 32>	soundWorlds;
	
	budList<idSoundSample*, TAG_AUDIO>		samples;
	budHashIndex					sampleHash;
	
	idSoundHardware				hardware;
	
	idRandom2					random;
	
	int							soundTime;
	bool						muted;
	bool						musicMuted;
	bool						needsRestart;
	
	bool						insideLevelLoad;
	
	//-------------------------
	
	idSoundSystemLocal() :
		currentSoundWorld( NULL ),
		soundTime( 0 ),
		muted( false ),
		musicMuted( false ),
		needsRestart( false )
	{}
};

extern	idSoundSystemLocal	soundSystemLocal;

#endif /* !__SND_LOCAL_H__ */
