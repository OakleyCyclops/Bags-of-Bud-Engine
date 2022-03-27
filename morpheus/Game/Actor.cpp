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

#include "PCH.hpp"
#pragma hdrstop

#include "Game_local.h"


/***********************************************************************

	budAnimState

***********************************************************************/

/*
=====================
budAnimState::budAnimState
=====================
*/
budAnimState::budAnimState()
{
	self			= NULL;
	animator		= NULL;
	thread			= NULL;
	idleAnim		= true;
	disabled		= true;
	channel			= ANIMCHANNEL_ALL;
	animBlendFrames = 0;
	lastAnimBlendFrames = 0;
}

/*
=====================
budAnimState::~budAnimState
=====================
*/
budAnimState::~budAnimState()
{
	delete thread;
}

/*
=====================
budAnimState::Save
=====================
*/
void budAnimState::Save( idSaveGame* savefile ) const
{

	savefile->WriteObject( self );
	
	// Save the entity owner of the animator
	savefile->WriteObject( animator->GetEntity() );
	
	savefile->WriteObject( thread );
	
	savefile->WriteString( state );
	
	savefile->WriteInt( animBlendFrames );
	savefile->WriteInt( lastAnimBlendFrames );
	savefile->WriteInt( channel );
	savefile->WriteBool( idleAnim );
	savefile->WriteBool( disabled );
}

/*
=====================
budAnimState::Restore
=====================
*/
void budAnimState::Restore( idRestoreGame* savefile )
{
	savefile->ReadObject( reinterpret_cast<idClass*&>( self ) );
	
	idEntity* animowner;
	savefile->ReadObject( reinterpret_cast<idClass*&>( animowner ) );
	if( animowner )
	{
		animator = animowner->GetAnimator();
	}
	
	savefile->ReadObject( reinterpret_cast<idClass*&>( thread ) );
	
	savefile->ReadString( state );
	
	savefile->ReadInt( animBlendFrames );
	savefile->ReadInt( lastAnimBlendFrames );
	savefile->ReadInt( channel );
	savefile->ReadBool( idleAnim );
	savefile->ReadBool( disabled );
}

/*
=====================
budAnimState::Init
=====================
*/
void budAnimState::Init( budActor* owner, budAnimator* _animator, int animchannel )
{
	assert( owner );
	assert( _animator );
	self = owner;
	animator = _animator;
	channel = animchannel;
	
	if( !thread )
	{
		thread = new idThread();
		thread->ManualDelete();
	}
	thread->EndThread();
	thread->ManualControl();
}

/*
=====================
budAnimState::Shutdown
=====================
*/
void budAnimState::Shutdown()
{
	delete thread;
	thread = NULL;
}

/*
=====================
budAnimState::SetState
=====================
*/
void budAnimState::SetState( const char* statename, int blendFrames )
{
	const function_t* func;
	
	func = self->scriptObject.GetFunction( statename );
	if( !func )
	{
		assert( 0 );
		gameLocal.Error( "Can't find function '%s' in object '%s'", statename, self->scriptObject.GetTypeName() );
	}
	
	state = statename;
	disabled = false;
	animBlendFrames = blendFrames;
	lastAnimBlendFrames = blendFrames;
	thread->CallFunction( self, func, true );
	
	animBlendFrames = blendFrames;
	lastAnimBlendFrames = blendFrames;
	disabled = false;
	idleAnim = false;
	
	if( ai_debugScript.GetInteger() == self->entityNumber )
	{
		gameLocal.Printf( "%d: %s: Animstate: %s\n", gameLocal.time, self->name.c_str(), state.c_str() );
	}
}

/*
=====================
budAnimState::StopAnim
=====================
*/
void budAnimState::StopAnim( int frames )
{
	animBlendFrames = 0;
	animator->Clear( channel, gameLocal.time, FRAME2MS( frames ) );
}

/*
=====================
budAnimState::PlayAnim
=====================
*/
void budAnimState::PlayAnim( int anim )
{
	if( anim )
	{
		animator->PlayAnim( channel, anim, gameLocal.time, FRAME2MS( animBlendFrames ) );
	}
	animBlendFrames = 0;
}

/*
=====================
budAnimState::CycleAnim
=====================
*/
void budAnimState::CycleAnim( int anim )
{
	if( anim )
	{
		animator->CycleAnim( channel, anim, gameLocal.time, FRAME2MS( animBlendFrames ) );
	}
	animBlendFrames = 0;
}

/*
=====================
budAnimState::BecomeIdle
=====================
*/
void budAnimState::BecomeIdle()
{
	idleAnim = true;
}

/*
=====================
budAnimState::Disabled
=====================
*/
bool budAnimState::Disabled() const
{
	return disabled;
}

/*
=====================
budAnimState::AnimDone
=====================
*/
bool budAnimState::AnimDone( int blendFrames ) const
{
	int animDoneTime;
	
	animDoneTime = animator->CurrentAnim( channel )->GetEndTime();
	if( animDoneTime < 0 )
	{
		// playing a cycle
		return false;
	}
	else if( animDoneTime - FRAME2MS( blendFrames ) <= gameLocal.time )
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
=====================
budAnimState::IsIdle
=====================
*/
bool budAnimState::IsIdle() const
{
	return disabled || idleAnim;
}

/*
=====================
budAnimState::GetAnimFlags
=====================
*/
animFlags_t budAnimState::GetAnimFlags() const
{
	animFlags_t flags;
	
	memset( &flags, 0, sizeof( flags ) );
	if( !disabled && !AnimDone( 0 ) )
	{
		flags = animator->GetAnimFlags( animator->CurrentAnim( channel )->AnimNum() );
	}
	
	return flags;
}

/*
=====================
budAnimState::Enable
=====================
*/
void budAnimState::Enable( int blendFrames )
{
	if( disabled )
	{
		disabled = false;
		animBlendFrames = blendFrames;
		lastAnimBlendFrames = blendFrames;
		if( state.Length() )
		{
			SetState( state.c_str(), blendFrames );
		}
	}
}

/*
=====================
budAnimState::Disable
=====================
*/
void budAnimState::Disable()
{
	disabled = true;
	idleAnim = false;
}

/*
=====================
budAnimState::UpdateState
=====================
*/
bool budAnimState::UpdateState()
{
	if( disabled )
	{
		return false;
	}
	
	if( ai_debugScript.GetInteger() == self->entityNumber )
	{
		thread->EnableDebugInfo();
	}
	else
	{
		thread->DisableDebugInfo();
	}
	
	thread->Execute();
	
	return true;
}

/***********************************************************************

	budActor

***********************************************************************/

const idEventDef AI_EnableEyeFocus( "enableEyeFocus" );
const idEventDef AI_DisableEyeFocus( "disableEyeFocus" );
const idEventDef EV_Footstep( "footstep" );
const idEventDef EV_FootstepLeft( "leftFoot" );
const idEventDef EV_FootstepRight( "rightFoot" );
const idEventDef EV_EnableWalkIK( "EnableWalkIK" );
const idEventDef EV_DisableWalkIK( "DisableWalkIK" );
const idEventDef EV_EnableLegIK( "EnableLegIK", "d" );
const idEventDef EV_DisableLegIK( "DisableLegIK", "d" );
const idEventDef AI_StopAnim( "stopAnim", "dd" );
const idEventDef AI_PlayAnim( "playAnim", "ds", 'd' );
const idEventDef AI_PlayCycle( "playCycle", "ds", 'd' );
const idEventDef AI_IdleAnim( "idleAnim", "ds", 'd' );
const idEventDef AI_SetSyncedAnimWeight( "setSyncedAnimWeight", "ddf" );
const idEventDef AI_SetBlendFrames( "setBlendFrames", "dd" );
const idEventDef AI_GetBlendFrames( "getBlendFrames", "d", 'd' );
const idEventDef AI_AnimState( "animState", "dsd" );
const idEventDef AI_GetAnimState( "getAnimState", "d", 's' );
const idEventDef AI_InAnimState( "inAnimState", "ds", 'd' );
const idEventDef AI_FinishAction( "finishAction", "s" );
const idEventDef AI_AnimDone( "animDone", "dd", 'd' );
const idEventDef AI_OverrideAnim( "overrideAnim", "d" );
const idEventDef AI_EnableAnim( "enableAnim", "dd" );
const idEventDef AI_PreventPain( "preventPain", "f" );
const idEventDef AI_DisablePain( "disablePain" );
const idEventDef AI_EnablePain( "enablePain" );
const idEventDef AI_GetPainAnim( "getPainAnim", NULL, 's' );
const idEventDef AI_SetAnimPrefix( "setAnimPrefix", "s" );
const idEventDef AI_HasAnim( "hasAnim", "ds", 'f' );
const idEventDef AI_CheckAnim( "checkAnim", "ds" );
const idEventDef AI_ChooseAnim( "chooseAnim", "ds", 's' );
const idEventDef AI_AnimLength( "animLength", "ds", 'f' );
const idEventDef AI_AnimDistance( "animDistance", "ds", 'f' );
const idEventDef AI_HasEnemies( "hasEnemies", NULL, 'd' );
const idEventDef AI_NextEnemy( "nextEnemy", "E", 'e' );
const idEventDef AI_ClosestEnemyToPoint( "closestEnemyToPoint", "v", 'e' );
const idEventDef AI_SetNextState( "setNextState", "s" );
const idEventDef AI_SetState( "setState", "s" );
const idEventDef AI_GetState( "getState", NULL, 's' );
const idEventDef AI_GetHead( "getHead", NULL, 'e' );
const idEventDef EV_SetDamageGroupScale( "setDamageGroupScale", "sf" );
const idEventDef EV_SetDamageGroupScaleAll( "setDamageGroupScaleAll", "f" );
const idEventDef EV_GetDamageGroupScale( "getDamageGroupScale", "s", 'f' );
const idEventDef EV_SetDamageCap( "setDamageCap", "f" );
const idEventDef EV_SetWaitState( "setWaitState" , "s" );
const idEventDef EV_GetWaitState( "getWaitState", NULL, 's' );

CLASS_DECLARATION( budAFEntity_Gibbable, budActor )
EVENT( AI_EnableEyeFocus,			budActor::Event_EnableEyeFocus )
EVENT( AI_DisableEyeFocus,			budActor::Event_DisableEyeFocus )
EVENT( EV_Footstep,					budActor::Event_Footstep )
EVENT( EV_FootstepLeft,				budActor::Event_Footstep )
EVENT( EV_FootstepRight,			budActor::Event_Footstep )
EVENT( EV_EnableWalkIK,				budActor::Event_EnableWalkIK )
EVENT( EV_DisableWalkIK,			budActor::Event_DisableWalkIK )
EVENT( EV_EnableLegIK,				budActor::Event_EnableLegIK )
EVENT( EV_DisableLegIK,				budActor::Event_DisableLegIK )
EVENT( AI_PreventPain,				budActor::Event_PreventPain )
EVENT( AI_DisablePain,				budActor::Event_DisablePain )
EVENT( AI_EnablePain,				budActor::Event_EnablePain )
EVENT( AI_GetPainAnim,				budActor::Event_GetPainAnim )
EVENT( AI_SetAnimPrefix,			budActor::Event_SetAnimPrefix )
EVENT( AI_StopAnim,					budActor::Event_StopAnim )
EVENT( AI_PlayAnim,					budActor::Event_PlayAnim )
EVENT( AI_PlayCycle,				budActor::Event_PlayCycle )
EVENT( AI_IdleAnim,					budActor::Event_IdleAnim )
EVENT( AI_SetSyncedAnimWeight,		budActor::Event_SetSyncedAnimWeight )
EVENT( AI_SetBlendFrames,			budActor::Event_SetBlendFrames )
EVENT( AI_GetBlendFrames,			budActor::Event_GetBlendFrames )
EVENT( AI_AnimState,				budActor::Event_AnimState )
EVENT( AI_GetAnimState,				budActor::Event_GetAnimState )
EVENT( AI_InAnimState,				budActor::Event_InAnimState )
EVENT( AI_FinishAction,				budActor::Event_FinishAction )
EVENT( AI_AnimDone,					budActor::Event_AnimDone )
EVENT( AI_OverrideAnim,				budActor::Event_OverrideAnim )
EVENT( AI_EnableAnim,				budActor::Event_EnableAnim )
EVENT( AI_HasAnim,					budActor::Event_HasAnim )
EVENT( AI_CheckAnim,				budActor::Event_CheckAnim )
EVENT( AI_ChooseAnim,				budActor::Event_ChooseAnim )
EVENT( AI_AnimLength,				budActor::Event_AnimLength )
EVENT( AI_AnimDistance,				budActor::Event_AnimDistance )
EVENT( AI_HasEnemies,				budActor::Event_HasEnemies )
EVENT( AI_NextEnemy,				budActor::Event_NextEnemy )
EVENT( AI_ClosestEnemyToPoint,		budActor::Event_ClosestEnemyToPoint )
EVENT( EV_StopSound,				budActor::Event_StopSound )
EVENT( AI_SetNextState,				budActor::Event_SetNextState )
EVENT( AI_SetState,					budActor::Event_SetState )
EVENT( AI_GetState,					budActor::Event_GetState )
EVENT( AI_GetHead,					budActor::Event_GetHead )
EVENT( EV_SetDamageGroupScale,		budActor::Event_SetDamageGroupScale )
EVENT( EV_SetDamageGroupScaleAll,	budActor::Event_SetDamageGroupScaleAll )
EVENT( EV_GetDamageGroupScale,		budActor::Event_GetDamageGroupScale )
EVENT( EV_SetDamageCap,				budActor::Event_SetDamageCap )
EVENT( EV_SetWaitState,				budActor::Event_SetWaitState )
EVENT( EV_GetWaitState,				budActor::Event_GetWaitState )
END_CLASS

/*
=====================
budActor::budActor
=====================
*/
budActor::budActor()
{
	viewAxis.Identity();
	
	scriptThread		= NULL;		// initialized by ConstructScriptObject, which is called by idEntity::Spawn
	
	use_combat_bbox		= false;
	head				= NULL;
	
	team				= 0;
	rank				= 0;
	fovDot				= 0.0f;
	eyeOffset.Zero();
	pain_debounce_time	= 0;
	pain_delay			= 0;
	pain_threshold		= 0;
	
	state				= NULL;
	idealState			= NULL;
	
	leftEyeJoint		= INVALID_JOINT;
	rightEyeJoint		= INVALID_JOINT;
	soundJoint			= INVALID_JOINT;
	
	modelOffset.Zero();
	deltaViewAngles.Zero();
	
	painTime			= 0;
	allowPain			= false;
	allowEyeFocus		= false;
	
	waitState			= "";
	
	blink_anim			= 0;
	blink_time			= 0;
	blink_min			= 0;
	blink_max			= 0;
	
	finalBoss			= false;
	damageNotByFists	= false;		// for killed by fists achievement
	
	attachments.SetGranularity( 1 );
	
	enemyNode.SetOwner( this );
	enemyList.SetOwner( this );
	
	aimAssistNode.SetOwner( this );
	aimAssistNode.AddToEnd( gameLocal.aimAssistEntities );
	
	damageCap = -1;
}

/*
=====================
budActor::~budActor
=====================
*/
budActor::~budActor()
{
	int i;
	idEntity* ent;
	
	DeconstructScriptObject();
	scriptObject.Free();
	
	StopSound( SND_CHANNEL_ANY, false );
	
	delete combatModel;
	combatModel = NULL;
	
	if( head.GetEntity() )
	{
		head.GetEntity()->ClearBody();
		head.GetEntity()->PostEventMS( &EV_Remove, 0 );
	}
	
	// remove any attached entities
	for( i = 0; i < attachments.Num(); i++ )
	{
		ent = attachments[ i ].ent.GetEntity();
		if( ent )
		{
			ent->PostEventMS( &EV_Remove, 0 );
		}
	}
	
	aimAssistNode.Remove();
	
	ShutdownThreads();
}

/*
=====================
budActor::Spawn
=====================
*/
void budActor::Spawn()
{
	idEntity*		ent;
	budStr			jointName;
	float			fovDegrees;
	copyJoints_t	copyJoint;
	
	animPrefix	= "";
	state		= NULL;
	idealState	= NULL;
	
	spawnArgs.GetInt( "rank", "0", rank );
	spawnArgs.GetInt( "team", "0", team );
	spawnArgs.GetVector( "offsetModel", "0 0 0", modelOffset );
	
	spawnArgs.GetBool( "use_combat_bbox", "0", use_combat_bbox );
	
	viewAxis = GetPhysics()->GetAxis();
	
	spawnArgs.GetFloat( "fov", "90", fovDegrees );
	SetFOV( fovDegrees );
	
	pain_debounce_time	= 0;
	
	pain_delay		= SEC2MS( spawnArgs.GetFloat( "pain_delay" ) );
	pain_threshold	= spawnArgs.GetInt( "pain_threshold" );
	
	LoadAF();
	
	walkIK.Init( this, IK_ANIM, modelOffset );
	
	// the animation used to be set to the IK_ANIM at this point, but that was fixed, resulting in
	// attachments not binding correctly, so we're stuck setting the IK_ANIM before attaching things.
	animator.ClearAllAnims( gameLocal.time, 0 );
	animator.SetFrame( ANIMCHANNEL_ALL, animator.GetAnim( IK_ANIM ), 0, 0, 0 );
	
	// spawn any attachments we might have
	const idKeyValue* kv = spawnArgs.MatchPrefix( "def_attach", NULL );
	while( kv )
	{
		idDict args;
		
		args.Set( "classname", kv->GetValue().c_str() );
		
		// make items non-touchable so the player can't take them out of the character's hands
		args.Set( "no_touch", "1" );
		
		// don't let them drop to the floor
		args.Set( "dropToFloor", "0" );
		
		gameLocal.SpawnEntityDef( args, &ent );
		if( !ent )
		{
			gameLocal.Error( "Couldn't spawn '%s' to attach to entity '%s'", kv->GetValue().c_str(), name.c_str() );
		}
		else
		{
			Attach( ent );
		}
		kv = spawnArgs.MatchPrefix( "def_attach", kv );
	}
	
	SetupDamageGroups();
	SetupHead();
	
	// clear the bind anim
	animator.ClearAllAnims( gameLocal.time, 0 );
	
	idEntity* headEnt = head.GetEntity();
	budAnimator* headAnimator;
	if( headEnt )
	{
		headAnimator = headEnt->GetAnimator();
	}
	else
	{
		headAnimator = &animator;
	}
	
	if( headEnt )
	{
		// set up the list of joints to copy to the head
		for( kv = spawnArgs.MatchPrefix( "copy_joint", NULL ); kv != NULL; kv = spawnArgs.MatchPrefix( "copy_joint", kv ) )
		{
			if( kv->GetValue() == "" )
			{
				// probably clearing out inherited key, so skip it
				continue;
			}
			
			jointName = kv->GetKey();
			if( jointName.StripLeadingOnce( "copy_joint_world " ) )
			{
				copyJoint.mod = JOINTMOD_WORLD_OVERRIDE;
			}
			else
			{
				jointName.StripLeadingOnce( "copy_joint " );
				copyJoint.mod = JOINTMOD_LOCAL_OVERRIDE;
			}
			
			copyJoint.from = animator.GetJointHandle( jointName );
			if( copyJoint.from == INVALID_JOINT )
			{
				gameLocal.Warning( "Unknown copy_joint '%s' on entity %s", jointName.c_str(), name.c_str() );
				continue;
			}
			
			jointName = kv->GetValue();
			copyJoint.to = headAnimator->GetJointHandle( jointName );
			if( copyJoint.to == INVALID_JOINT )
			{
				gameLocal.Warning( "Unknown copy_joint '%s' on head of entity %s", jointName.c_str(), name.c_str() );
				continue;
			}
			
			copyJoints.Append( copyJoint );
		}
	}
	
	// set up blinking
	blink_anim = headAnimator->GetAnim( "blink" );
	blink_time = 0;	// it's ok to blink right away
	blink_min = SEC2MS( spawnArgs.GetFloat( "blink_min", "0.5" ) );
	blink_max = SEC2MS( spawnArgs.GetFloat( "blink_max", "8" ) );
	
	// set up the head anim if necessary
	int headAnim = headAnimator->GetAnim( "def_head" );
	if( headAnim )
	{
		if( headEnt )
		{
			headAnimator->CycleAnim( ANIMCHANNEL_ALL, headAnim, gameLocal.time, 0 );
		}
		else
		{
			headAnimator->CycleAnim( ANIMCHANNEL_HEAD, headAnim, gameLocal.time, 0 );
		}
	}
	
	if( spawnArgs.GetString( "sound_bone", "", jointName ) )
	{
		soundJoint = animator.GetJointHandle( jointName );
		if( soundJoint == INVALID_JOINT )
		{
			gameLocal.Warning( "budAnimated '%s' at (%s): cannot find joint '%s' for sound playback", name.c_str(), GetPhysics()->GetOrigin().ToString( 0 ), jointName.c_str() );
		}
	}
	
	finalBoss = spawnArgs.GetBool( "finalBoss" );
	
	FinishSetup();
}

/*
================
budActor::FinishSetup
================
*/
void budActor::FinishSetup()
{
	const char*	scriptObjectName;
	
	// setup script object
	if( spawnArgs.GetString( "scriptobject", NULL, &scriptObjectName ) )
	{
		if( !scriptObject.SetType( scriptObjectName ) )
		{
			gameLocal.Error( "Script object '%s' not found on entity '%s'.", scriptObjectName, name.c_str() );
		}
		
		ConstructScriptObject();
	}
	
	SetupBody();
}

/*
================
budActor::SetupHead
================
*/
void budActor::SetupHead()
{
	budAFAttachment*		headEnt;
	budStr				jointName;
	const char*			headModel;
	jointHandle_t		joint;
	jointHandle_t		damageJoint;
	int					i;
	const idKeyValue*	sndKV;
	
	if( common->IsClient() )
	{
		return;
	}
	
	headModel = spawnArgs.GetString( "def_head", "" );
	if( headModel[ 0 ] )
	{
		jointName = spawnArgs.GetString( "head_joint" );
		joint = animator.GetJointHandle( jointName );
		if( joint == INVALID_JOINT )
		{
			gameLocal.Error( "Joint '%s' not found for 'head_joint' on '%s'", jointName.c_str(), name.c_str() );
		}
		
		// set the damage joint to be part of the head damage group
		damageJoint = joint;
		for( i = 0; i < damageGroups.Num(); i++ )
		{
			if( damageGroups[ i ] == "head" )
			{
				damageJoint = static_cast<jointHandle_t>( i );
				break;
			}
		}
		
		// copy any sounds in case we have frame commands on the head
		idDict	args;
		sndKV = spawnArgs.MatchPrefix( "snd_", NULL );
		while( sndKV )
		{
			args.Set( sndKV->GetKey(), sndKV->GetValue() );
			sndKV = spawnArgs.MatchPrefix( "snd_", sndKV );
		}
		
		// copy slowmo param to the head
		args.SetBool( "slowmo", spawnArgs.GetBool( "slowmo", "1" ) );
		
		
		headEnt = static_cast<budAFAttachment*>( gameLocal.SpawnEntityType( budAFAttachment::Type, &args ) );
		headEnt->SetName( va( "%s_head", name.c_str() ) );
		headEnt->SetBody( this, headModel, damageJoint );
		head = headEnt;
		
		budStr xSkin;
		if( spawnArgs.GetString( "skin_head_xray", "", xSkin ) )
		{
			headEnt->xraySkin = declManager->FindSkin( xSkin.c_str() );
			headEnt->UpdateModel();
		}
		
		budVec3		origin;
		budMat3		axis;
		budAttachInfo& attach = attachments.Alloc();
		attach.channel = animator.GetChannelForJoint( joint );
		animator.GetJointTransform( joint, gameLocal.time, origin, axis );
		origin = renderEntity.origin + ( origin + modelOffset ) * renderEntity.axis;
		attach.ent = headEnt;
		headEnt->SetOrigin( origin );
		headEnt->SetAxis( renderEntity.axis );
		headEnt->BindToJoint( this, joint, true );
	}
}

/*
================
budActor::CopyJointsFromBodyToHead
================
*/
void budActor::CopyJointsFromBodyToHead()
{
	idEntity*	headEnt = head.GetEntity();
	budAnimator*	headAnimator;
	int			i;
	budMat3		mat;
	budMat3		axis;
	budVec3		pos;
	
	if( !headEnt )
	{
		return;
	}
	
	headAnimator = headEnt->GetAnimator();
	
	// copy the animation from the body to the head
	for( i = 0; i < copyJoints.Num(); i++ )
	{
		if( copyJoints[ i ].mod == JOINTMOD_WORLD_OVERRIDE )
		{
			mat = headEnt->GetPhysics()->GetAxis().Transpose();
			GetJointWorldTransform( copyJoints[ i ].from, gameLocal.time, pos, axis );
			pos -= headEnt->GetPhysics()->GetOrigin();
			headAnimator->SetJointPos( copyJoints[ i ].to, copyJoints[ i ].mod, pos * mat );
			headAnimator->SetJointAxis( copyJoints[ i ].to, copyJoints[ i ].mod, axis * mat );
		}
		else
		{
			animator.GetJointLocalTransform( copyJoints[ i ].from, gameLocal.time, pos, axis );
			headAnimator->SetJointPos( copyJoints[ i ].to, copyJoints[ i ].mod, pos );
			headAnimator->SetJointAxis( copyJoints[ i ].to, copyJoints[ i ].mod, axis );
		}
	}
}

/*
================
budActor::Restart
================
*/
void budActor::Restart()
{
	assert( !head.GetEntity() );
	SetupHead();
	FinishSetup();
}

/*
================
budActor::Save

archive object for savegame file
================
*/
void budActor::Save( idSaveGame* savefile ) const
{
	budActor* ent;
	int i;
	
	savefile->WriteInt( team );
	savefile->WriteInt( rank );
	savefile->WriteMat3( viewAxis );
	
	savefile->WriteInt( enemyList.Num() );
	for( ent = enemyList.Next(); ent != NULL; ent = ent->enemyNode.Next() )
	{
		savefile->WriteObject( ent );
	}
	
	savefile->WriteFloat( fovDot );
	savefile->WriteVec3( eyeOffset );
	savefile->WriteVec3( modelOffset );
	savefile->WriteAngles( deltaViewAngles );
	
	savefile->WriteInt( pain_debounce_time );
	savefile->WriteInt( pain_delay );
	savefile->WriteInt( pain_threshold );
	
	savefile->WriteInt( damageGroups.Num() );
	for( i = 0; i < damageGroups.Num(); i++ )
	{
		savefile->WriteString( damageGroups[ i ] );
	}
	
	savefile->WriteInt( damageScale.Num() );
	for( i = 0; i < damageScale.Num(); i++ )
	{
		savefile->WriteFloat( damageScale[ i ] );
	}
	
	savefile->WriteBool( use_combat_bbox );
	head.Save( savefile );
	
	savefile->WriteInt( copyJoints.Num() );
	for( i = 0; i < copyJoints.Num(); i++ )
	{
		savefile->WriteInt( copyJoints[i].mod );
		savefile->WriteJoint( copyJoints[i].from );
		savefile->WriteJoint( copyJoints[i].to );
	}
	
	savefile->WriteJoint( leftEyeJoint );
	savefile->WriteJoint( rightEyeJoint );
	savefile->WriteJoint( soundJoint );
	
	walkIK.Save( savefile );
	
	savefile->WriteString( animPrefix );
	savefile->WriteString( painAnim );
	
	savefile->WriteInt( blink_anim );
	savefile->WriteInt( blink_time );
	savefile->WriteInt( blink_min );
	savefile->WriteInt( blink_max );
	
	// script variables
	savefile->WriteObject( scriptThread );
	
	savefile->WriteString( waitState );
	
	headAnim.Save( savefile );
	torsoAnim.Save( savefile );
	legsAnim.Save( savefile );
	
	savefile->WriteBool( allowPain );
	savefile->WriteBool( allowEyeFocus );
	
	savefile->WriteInt( painTime );
	
	savefile->WriteInt( attachments.Num() );
	for( i = 0; i < attachments.Num(); i++ )
	{
		attachments[i].ent.Save( savefile );
		savefile->WriteInt( attachments[i].channel );
	}
	
	savefile->WriteBool( finalBoss );
	
	budToken token;
	
	//FIXME: this is unneccesary
	if( state )
	{
		budLexer src( state->Name(), budStr::Length( state->Name() ), "budAI::Save" );
		
		src.ReadTokenOnLine( &token );
		src.ExpectTokenString( "::" );
		src.ReadTokenOnLine( &token );
		
		savefile->WriteString( token );
	}
	else
	{
		savefile->WriteString( "" );
	}
	
	if( idealState )
	{
		budLexer src( idealState->Name(), budStr::Length( idealState->Name() ), "budAI::Save" );
		
		src.ReadTokenOnLine( &token );
		src.ExpectTokenString( "::" );
		src.ReadTokenOnLine( &token );
		
		savefile->WriteString( token );
	}
	else
	{
		savefile->WriteString( "" );
	}
	
	savefile->WriteInt( damageCap );
	
}

/*
================
budActor::Restore

unarchives object from save game file
================
*/
void budActor::Restore( idRestoreGame* savefile )
{
	int i, num;
	budActor* ent;
	
	savefile->ReadInt( team );
	savefile->ReadInt( rank );
	savefile->ReadMat3( viewAxis );
	
	savefile->ReadInt( num );
	for( i = 0; i < num; i++ )
	{
		savefile->ReadObject( reinterpret_cast<idClass*&>( ent ) );
		assert( ent );
		if( ent )
		{
			ent->enemyNode.AddToEnd( enemyList );
		}
	}
	
	savefile->ReadFloat( fovDot );
	savefile->ReadVec3( eyeOffset );
	savefile->ReadVec3( modelOffset );
	savefile->ReadAngles( deltaViewAngles );
	
	savefile->ReadInt( pain_debounce_time );
	savefile->ReadInt( pain_delay );
	savefile->ReadInt( pain_threshold );
	
	savefile->ReadInt( num );
	damageGroups.SetGranularity( 1 );
	damageGroups.SetNum( num );
	for( i = 0; i < num; i++ )
	{
		savefile->ReadString( damageGroups[ i ] );
	}
	
	savefile->ReadInt( num );
	damageScale.SetNum( num );
	for( i = 0; i < num; i++ )
	{
		savefile->ReadFloat( damageScale[ i ] );
	}
	
	savefile->ReadBool( use_combat_bbox );
	head.Restore( savefile );
	
	savefile->ReadInt( num );
	copyJoints.SetNum( num );
	for( i = 0; i < num; i++ )
	{
		int val;
		savefile->ReadInt( val );
		copyJoints[i].mod = static_cast<jointModTransform_t>( val );
		savefile->ReadJoint( copyJoints[i].from );
		savefile->ReadJoint( copyJoints[i].to );
	}
	
	savefile->ReadJoint( leftEyeJoint );
	savefile->ReadJoint( rightEyeJoint );
	savefile->ReadJoint( soundJoint );
	
	walkIK.Restore( savefile );
	
	savefile->ReadString( animPrefix );
	savefile->ReadString( painAnim );
	
	savefile->ReadInt( blink_anim );
	savefile->ReadInt( blink_time );
	savefile->ReadInt( blink_min );
	savefile->ReadInt( blink_max );
	
	savefile->ReadObject( reinterpret_cast<idClass*&>( scriptThread ) );
	
	savefile->ReadString( waitState );
	
	headAnim.Restore( savefile );
	torsoAnim.Restore( savefile );
	legsAnim.Restore( savefile );
	
	savefile->ReadBool( allowPain );
	savefile->ReadBool( allowEyeFocus );
	
	savefile->ReadInt( painTime );
	
	savefile->ReadInt( num );
	for( i = 0; i < num; i++ )
	{
		budAttachInfo& attach = attachments.Alloc();
		attach.ent.Restore( savefile );
		savefile->ReadInt( attach.channel );
	}
	
	savefile->ReadBool( finalBoss );
	
	budStr statename;
	
	savefile->ReadString( statename );
	if( statename.Length() > 0 )
	{
		state = GetScriptFunction( statename );
	}
	
	savefile->ReadString( statename );
	if( statename.Length() > 0 )
	{
		idealState = GetScriptFunction( statename );
	}
	
	savefile->ReadInt( damageCap );
}

/*
================
budActor::Hide
================
*/
void budActor::Hide()
{
	idEntity* ent;
	idEntity* next;
	
	budAFEntity_Base::Hide();
	if( head.GetEntity() )
	{
		head.GetEntity()->Hide();
	}
	
	for( ent = GetNextTeamEntity(); ent != NULL; ent = next )
	{
		next = ent->GetNextTeamEntity();
		if( ent->GetBindMaster() == this )
		{
			ent->Hide();
			if( ent->IsType( idLight::Type ) )
			{
				static_cast<idLight*>( ent )->Off();
			}
		}
	}
	UnlinkCombat();
}

/*
================
budActor::Show
================
*/
void budActor::Show()
{
	idEntity* ent;
	idEntity* next;
	
	budAFEntity_Base::Show();
	if( head.GetEntity() )
	{
		head.GetEntity()->Show();
	}
	for( ent = GetNextTeamEntity(); ent != NULL; ent = next )
	{
		next = ent->GetNextTeamEntity();
		if( ent->GetBindMaster() == this )
		{
			ent->Show();
			if( ent->IsType( idLight::Type ) )
			{
				if( !spawnArgs.GetBool( "lights_off", "0" ) )
				{
					static_cast<idLight*>( ent )->On();
				}
				
				
			}
		}
	}
	LinkCombat();
}

/*
==============
budActor::GetDefaultSurfaceType
==============
*/
int	budActor::GetDefaultSurfaceType() const
{
	return SURFTYPE_FLESH;
}

/*
================
budActor::ProjectOverlay
================
*/
void budActor::ProjectOverlay( const budVec3& origin, const budVec3& dir, float size, const char* material )
{
	idEntity* ent;
	idEntity* next;
	
	idEntity::ProjectOverlay( origin, dir, size, material );
	
	for( ent = GetNextTeamEntity(); ent != NULL; ent = next )
	{
		next = ent->GetNextTeamEntity();
		if( ent->GetBindMaster() == this )
		{
			if( ent->fl.takedamage && ent->spawnArgs.GetBool( "bleed" ) )
			{
				ent->ProjectOverlay( origin, dir, size, material );
			}
		}
	}
}

/*
================
budActor::LoadAF
================
*/
bool budActor::LoadAF()
{
	budStr fileName;
	
	if( !spawnArgs.GetString( "ragdoll", "*unknown*", fileName ) || !fileName.Length() )
	{
		return false;
	}
	af.SetAnimator( GetAnimator() );
	return af.Load( this, fileName );
}

/*
=====================
budActor::SetupBody
=====================
*/
void budActor::SetupBody()
{
	const char* jointname;
	
	animator.ClearAllAnims( gameLocal.time, 0 );
	animator.ClearAllJoints();
	
	idEntity* headEnt = head.GetEntity();
	if( headEnt )
	{
		jointname = spawnArgs.GetString( "bone_leftEye" );
		leftEyeJoint = headEnt->GetAnimator()->GetJointHandle( jointname );
		
		jointname = spawnArgs.GetString( "bone_rightEye" );
		rightEyeJoint = headEnt->GetAnimator()->GetJointHandle( jointname );
		
		// set up the eye height.  check if it's specified in the def.
		if( !spawnArgs.GetFloat( "eye_height", "0", eyeOffset.z ) )
		{
			// if not in the def, then try to base it off the idle animation
			int anim = headEnt->GetAnimator()->GetAnim( "idle" );
			if( anim && ( leftEyeJoint != INVALID_JOINT ) )
			{
				budVec3 pos;
				budMat3 axis;
				headEnt->GetAnimator()->PlayAnim( ANIMCHANNEL_ALL, anim, gameLocal.time, 0 );
				headEnt->GetAnimator()->GetJointTransform( leftEyeJoint, gameLocal.time, pos, axis );
				headEnt->GetAnimator()->ClearAllAnims( gameLocal.time, 0 );
				headEnt->GetAnimator()->ForceUpdate();
				pos += headEnt->GetPhysics()->GetOrigin() - GetPhysics()->GetOrigin();
				eyeOffset = pos + modelOffset;
			}
			else
			{
				// just base it off the bounding box size
				eyeOffset.z = GetPhysics()->GetBounds()[ 1 ].z - 6;
			}
		}
		headAnim.Init( this, headEnt->GetAnimator(), ANIMCHANNEL_ALL );
	}
	else
	{
		jointname = spawnArgs.GetString( "bone_leftEye" );
		leftEyeJoint = animator.GetJointHandle( jointname );
		
		jointname = spawnArgs.GetString( "bone_rightEye" );
		rightEyeJoint = animator.GetJointHandle( jointname );
		
		// set up the eye height.  check if it's specified in the def.
		if( !spawnArgs.GetFloat( "eye_height", "0", eyeOffset.z ) )
		{
			// if not in the def, then try to base it off the idle animation
			int anim = animator.GetAnim( "idle" );
			if( anim && ( leftEyeJoint != INVALID_JOINT ) )
			{
				budVec3 pos;
				budMat3 axis;
				animator.PlayAnim( ANIMCHANNEL_ALL, anim, gameLocal.time, 0 );
				animator.GetJointTransform( leftEyeJoint, gameLocal.time, pos, axis );
				animator.ClearAllAnims( gameLocal.time, 0 );
				animator.ForceUpdate();
				eyeOffset = pos + modelOffset;
			}
			else
			{
				// just base it off the bounding box size
				eyeOffset.z = GetPhysics()->GetBounds()[ 1 ].z - 6;
			}
		}
		headAnim.Init( this, &animator, ANIMCHANNEL_HEAD );
	}
	
	waitState = "";
	
	torsoAnim.Init( this, &animator, ANIMCHANNEL_TORSO );
	legsAnim.Init( this, &animator, ANIMCHANNEL_LEGS );
}

/*
=====================
budActor::CheckBlink
=====================
*/
void budActor::CheckBlink()
{
	// check if it's time to blink
	if( !blink_anim || ( health <= 0 ) || !allowEyeFocus || ( blink_time > gameLocal.time ) )
	{
		return;
	}
	
	idEntity* headEnt = head.GetEntity();
	if( headEnt )
	{
		headEnt->GetAnimator()->PlayAnim( ANIMCHANNEL_EYELIDS, blink_anim, gameLocal.time, 1 );
	}
	else
	{
		animator.PlayAnim( ANIMCHANNEL_EYELIDS, blink_anim, gameLocal.time, 1 );
	}
	
	// set the next blink time
	blink_time = gameLocal.time + blink_min + gameLocal.random.RandomFloat() * ( blink_max - blink_min );
}

/*
================
budActor::GetPhysicsToVisualTransform
================
*/
bool budActor::GetPhysicsToVisualTransform( budVec3& origin, budMat3& axis )
{
	if( af.IsActive() )
	{
		af.GetPhysicsToVisualTransform( origin, axis );
		return true;
	}
	origin = modelOffset;
	axis = viewAxis;
	return true;
}

/*
================
budActor::GetPhysicsToSoundTransform
================
*/
bool budActor::GetPhysicsToSoundTransform( budVec3& origin, budMat3& axis )
{
	if( soundJoint != INVALID_JOINT )
	{
		animator.GetJointTransform( soundJoint, gameLocal.time, origin, axis );
		origin += modelOffset;
		axis = viewAxis;
	}
	else
	{
		origin = GetPhysics()->GetGravityNormal() * -eyeOffset.z;
		axis.Identity();
	}
	return true;
}

/***********************************************************************

	script state management

***********************************************************************/

/*
================
budActor::ShutdownThreads
================
*/
void budActor::ShutdownThreads()
{
	headAnim.Shutdown();
	torsoAnim.Shutdown();
	legsAnim.Shutdown();
	
	if( scriptThread )
	{
		scriptThread->EndThread();
		scriptThread->PostEventMS( &EV_Remove, 0 );
		delete scriptThread;
		scriptThread = NULL;
	}
}

/*
================
budActor::ShouldConstructScriptObjectAtSpawn

Called during idEntity::Spawn to see if it should construct the script object or not.
Overridden by subclasses that need to spawn the script object themselves.
================
*/
bool budActor::ShouldConstructScriptObjectAtSpawn() const
{
	return false;
}

/*
================
budActor::ConstructScriptObject

Called during idEntity::Spawn.  Calls the constructor on the script object.
Can be overridden by subclasses when a thread doesn't need to be allocated.
================
*/
idThread* budActor::ConstructScriptObject()
{
	const function_t* constructor;
	
	// make sure we have a scriptObject
	if( !scriptObject.HasObject() )
	{
		gameLocal.Error( "No scriptobject set on '%s'.  Check the '%s' entityDef.", name.c_str(), GetEntityDefName() );
	}
	
	if( !scriptThread )
	{
		// create script thread
		scriptThread = new idThread();
		scriptThread->ManualDelete();
		scriptThread->ManualControl();
		scriptThread->SetThreadName( name.c_str() );
	}
	else
	{
		scriptThread->EndThread();
	}
	
	// call script object's constructor
	constructor = scriptObject.GetConstructor();
	if( !constructor )
	{
		gameLocal.Error( "Missing constructor on '%s' for entity '%s'", scriptObject.GetTypeName(), name.c_str() );
	}
	
	// init the script object's data
	scriptObject.ClearObject();
	
	// just set the current function on the script.  we'll execute in the subclasses.
	scriptThread->CallFunction( this, constructor, true );
	
	return scriptThread;
}

/*
=====================
budActor::GetScriptFunction
=====================
*/
const function_t* budActor::GetScriptFunction( const char* funcname )
{
	const function_t* func;
	
	func = scriptObject.GetFunction( funcname );
	if( !func )
	{
		scriptThread->Error( "Unknown function '%s' in '%s'", funcname, scriptObject.GetTypeName() );
	}
	
	return func;
}

/*
=====================
budActor::SetState
=====================
*/
void budActor::SetState( const function_t* newState )
{
	if( newState == NULL )
	{
		gameLocal.Error( "budActor::SetState: Null state" );
		return;
	}
	
	if( ai_debugScript.GetInteger() == entityNumber )
	{
		gameLocal.Printf( "%d: %s: State: %s\n", gameLocal.time, name.c_str(), newState->Name() );
	}
	
	state = newState;
	idealState = state;
	scriptThread->CallFunction( this, state, true );
}

/*
=====================
budActor::SetState
=====================
*/
void budActor::SetState( const char* statename )
{
	const function_t* newState;
	
	newState = GetScriptFunction( statename );
	SetState( newState );
}

/*
=====================
budActor::UpdateScript
=====================
*/
void budActor::UpdateScript()
{
	int	i;
	
	if( ai_debugScript.GetInteger() == entityNumber )
	{
		scriptThread->EnableDebugInfo();
	}
	else
	{
		scriptThread->DisableDebugInfo();
	}
	
	// a series of state changes can happen in a single frame.
	// this loop limits them in case we've entered an infinite loop.
	for( i = 0; i < 20; i++ )
	{
		if( idealState != state )
		{
			SetState( idealState );
		}
		
		// don't call script until it's done waiting
		if( scriptThread->IsWaiting() )
		{
			break;
		}
		
		scriptThread->Execute();
		if( idealState == state )
		{
			break;
		}
	}
	
	if( i == 20 )
	{
		scriptThread->Warning( "budActor::UpdateScript: exited loop to prevent lockup" );
	}
}

/***********************************************************************

	vision

***********************************************************************/

/*
=====================
budActor::setFov
=====================
*/
void budActor::SetFOV( float fov )
{
	fovDot = ( float )cos( DEG2RAD( fov * 0.5f ) );
}

/*
=====================
budActor::SetEyeHeight
=====================
*/
void budActor::SetEyeHeight( float height )
{
	eyeOffset.z = height;
}

/*
=====================
budActor::EyeHeight
=====================
*/
float budActor::EyeHeight() const
{
	return eyeOffset.z;
}

/*
=====================
budActor::EyeOffset
=====================
*/
budVec3 budActor::EyeOffset() const
{
	return GetPhysics()->GetGravityNormal() * -eyeOffset.z;
}

/*
=====================
budActor::GetEyePosition
=====================
*/
budVec3 budActor::GetEyePosition() const
{
	return GetPhysics()->GetOrigin() + ( GetPhysics()->GetGravityNormal() * -eyeOffset.z );
}

/*
=====================
budActor::GetViewPos
=====================
*/
void budActor::GetViewPos( budVec3& origin, budMat3& axis ) const
{
	origin = GetEyePosition();
	axis = viewAxis;
}

/*
=====================
budActor::CheckFOV
=====================
*/
bool budActor::CheckFOV( const budVec3& pos ) const
{
	if( fovDot == 1.0f )
	{
		return true;
	}
	
	float	dot;
	budVec3	delta;
	
	delta = pos - GetEyePosition();
	
	// get our gravity normal
	const budVec3& gravityDir = GetPhysics()->GetGravityNormal();
	
	// infinite vertical vision, so project it onto our orientation plane
	delta -= gravityDir * ( gravityDir * delta );
	
	delta.Normalize();
	dot = viewAxis[ 0 ] * delta;
	
	return ( dot >= fovDot );
}

/*
=====================
budActor::CanSee
=====================
*/
bool budActor::CanSee( idEntity* ent, bool useFov ) const
{
	trace_t		tr;
	budVec3		eye;
	budVec3		toPos;
	
	if( ent->IsHidden() )
	{
		return false;
	}
	
	if( ent->IsType( budActor::Type ) )
	{
		toPos = ( ( budActor* )ent )->GetEyePosition();
	}
	else
	{
		toPos = ent->GetPhysics()->GetOrigin();
	}
	
	if( useFov && !CheckFOV( toPos ) )
	{
		return false;
	}
	
	eye = GetEyePosition();
	
	gameLocal.clip.TracePoint( tr, eye, toPos, MASK_OPAQUE, this );
	if( tr.fraction >= 1.0f || ( gameLocal.GetTraceEntity( tr ) == ent ) )
	{
		return true;
	}
	
	return false;
}

/*
=====================
budActor::PointVisible
=====================
*/
bool budActor::PointVisible( const budVec3& point ) const
{
	trace_t results;
	budVec3 start, end;
	
	start = GetEyePosition();
	end = point;
	end[2] += 1.0f;
	
	gameLocal.clip.TracePoint( results, start, end, MASK_OPAQUE, this );
	return ( results.fraction >= 1.0f );
}

/*
=====================
budActor::GetAIAimTargets

Returns positions for the AI to aim at.
=====================
*/
void budActor::GetAIAimTargets( const budVec3& lastSightPos, budVec3& headPos, budVec3& chestPos )
{
	headPos = lastSightPos + EyeOffset();
	chestPos = ( headPos + lastSightPos + GetPhysics()->GetBounds().GetCenter() ) * 0.5f;
}

/*
=====================
budActor::GetRenderView
=====================
*/
renderView_t* budActor::GetRenderView()
{
	renderView_t* rv = idEntity::GetRenderView();
	rv->viewaxis = viewAxis;
	rv->vieworg = GetEyePosition();
	return rv;
}

/***********************************************************************

	Model/Ragdoll

***********************************************************************/

/*
================
budActor::SetCombatModel
================
*/
void budActor::SetCombatModel()
{
	budAFAttachment* headEnt;
	
	if( !use_combat_bbox )
	{
		if( combatModel )
		{
			combatModel->Unlink();
			combatModel->LoadModel( modelDefHandle );
		}
		else
		{
			combatModel = new( TAG_PHYSICS_CLIP_ENTITY ) budClipModel( modelDefHandle );
		}
		
		headEnt = head.GetEntity();
		if( headEnt )
		{
			headEnt->SetCombatModel();
		}
	}
}

/*
================
budActor::GetCombatModel
================
*/
budClipModel* budActor::GetCombatModel() const
{
	return combatModel;
}

/*
================
budActor::LinkCombat
================
*/
void budActor::LinkCombat()
{
	budAFAttachment* headEnt;
	
	if( fl.hidden || use_combat_bbox )
	{
		return;
	}
	
	if( combatModel )
	{
		combatModel->Link( gameLocal.clip, this, 0, renderEntity.origin, renderEntity.axis, modelDefHandle );
	}
	headEnt = head.GetEntity();
	if( headEnt )
	{
		headEnt->LinkCombat();
	}
}

/*
================
budActor::UnlinkCombat
================
*/
void budActor::UnlinkCombat()
{
	budAFAttachment* headEnt;
	
	if( combatModel )
	{
		combatModel->Unlink();
	}
	headEnt = head.GetEntity();
	if( headEnt )
	{
		headEnt->UnlinkCombat();
	}
}

/*
================
budActor::StartRagdoll
================
*/
bool budActor::StartRagdoll()
{
	float slomoStart, slomoEnd;
	float jointFrictionDent, jointFrictionDentStart, jointFrictionDentEnd;
	float contactFrictionDent, contactFrictionDentStart, contactFrictionDentEnd;
	
	// if no AF loaded
	if( !af.IsLoaded() )
	{
		return false;
	}
	
	// if the AF is already active
	if( af.IsActive() )
	{
		return true;
	}
	
	// disable the monster bounding box
	GetPhysics()->DisableClip();
	
	// start using the AF
	af.StartFromCurrentPose( spawnArgs.GetInt( "velocityTime", "0" ) );
	
	slomoStart = MS2SEC( gameLocal.time ) + spawnArgs.GetFloat( "ragdoll_slomoStart", "-1.6" );
	slomoEnd = MS2SEC( gameLocal.time ) + spawnArgs.GetFloat( "ragdoll_slomoEnd", "0.8" );
	
	// do the first part of the death in slow motion
	af.GetPhysics()->SetTimeScaleRamp( slomoStart, slomoEnd );
	
	jointFrictionDent = spawnArgs.GetFloat( "ragdoll_jointFrictionDent", "0.1" );
	jointFrictionDentStart = MS2SEC( gameLocal.time ) + spawnArgs.GetFloat( "ragdoll_jointFrictionStart", "0.2" );
	jointFrictionDentEnd = MS2SEC( gameLocal.time ) + spawnArgs.GetFloat( "ragdoll_jointFrictionEnd", "1.2" );
	
	// set joint friction dent
	af.GetPhysics()->SetJointFrictionDent( jointFrictionDent, jointFrictionDentStart, jointFrictionDentEnd );
	
	contactFrictionDent = spawnArgs.GetFloat( "ragdoll_contactFrictionDent", "0.1" );
	contactFrictionDentStart = MS2SEC( gameLocal.time ) + spawnArgs.GetFloat( "ragdoll_contactFrictionStart", "1.0" );
	contactFrictionDentEnd = MS2SEC( gameLocal.time ) + spawnArgs.GetFloat( "ragdoll_contactFrictionEnd", "2.0" );
	
	// set contact friction dent
	af.GetPhysics()->SetContactFrictionDent( contactFrictionDent, contactFrictionDentStart, contactFrictionDentEnd );
	
	// drop any items the actor is holding
	idMoveableItem::DropItems( this, "death", NULL );
	
	// drop any articulated figures the actor is holding
	budAFEntity_Base::DropAFs( this, "death", NULL );
	
	RemoveAttachments();
	
	return true;
}

/*
================
budActor::StopRagdoll
================
*/
void budActor::StopRagdoll()
{
	if( af.IsActive() )
	{
		af.Stop();
	}
}

/*
================
budActor::UpdateAnimationControllers
================
*/
bool budActor::UpdateAnimationControllers()
{

	if( af.IsActive() )
	{
		return budAFEntity_Base::UpdateAnimationControllers();
	}
	else
	{
		animator.ClearAFPose();
	}
	
	if( walkIK.IsInitialized() )
	{
		walkIK.Evaluate();
		return true;
	}
	
	return false;
}

/*
================
budActor::RemoveAttachments
================
*/
void budActor::RemoveAttachments()
{
	int i;
	idEntity* ent;
	
	// remove any attached entities
	for( i = 0; i < attachments.Num(); i++ )
	{
		ent = attachments[ i ].ent.GetEntity();
		if( ent != NULL && ent->spawnArgs.GetBool( "remove" ) )
		{
			ent->PostEventMS( &EV_Remove, 0 );
		}
	}
}

/*
================
budActor::Attach
================
*/
void budActor::Attach( idEntity* ent )
{
	budVec3			origin;
	budMat3			axis;
	jointHandle_t	joint;
	budStr			jointName;
	budAttachInfo&	attach = attachments.Alloc();
	budAngles		angleOffset;
	budVec3			originOffset;
	
	jointName = ent->spawnArgs.GetString( "joint" );
	joint = animator.GetJointHandle( jointName );
	if( joint == INVALID_JOINT )
	{
		gameLocal.Error( "Joint '%s' not found for attaching '%s' on '%s'", jointName.c_str(), ent->GetClassname(), name.c_str() );
	}
	
	angleOffset = ent->spawnArgs.GetAngles( "angles" );
	originOffset = ent->spawnArgs.GetVector( "origin" );
	
	attach.channel = animator.GetChannelForJoint( joint );
	GetJointWorldTransform( joint, gameLocal.time, origin, axis );
	attach.ent = ent;
	
	ent->SetOrigin( origin + originOffset * renderEntity.axis );
	budMat3 rotate = angleOffset.ToMat3();
	budMat3 newAxis = rotate * axis;
	ent->SetAxis( newAxis );
	ent->BindToJoint( this, joint, true );
	ent->cinematic = cinematic;
}

/*
================
budActor::Teleport
================
*/
void budActor::Teleport( const budVec3& origin, const budAngles& angles, idEntity* destination )
{
	GetPhysics()->SetOrigin( origin + budVec3( 0, 0, CM_CLIP_EPSILON ) );
	GetPhysics()->SetLinearVelocity( vec3_origin );
	
	viewAxis = angles.ToMat3();
	
	UpdateVisuals();
	
	if( !IsHidden() )
	{
		// kill anything at the new position
		gameLocal.KillBox( this );
	}
}

/*
================
budActor::GetDeltaViewAngles
================
*/
const budAngles& budActor::GetDeltaViewAngles() const
{
	return deltaViewAngles;
}

/*
================
budActor::SetDeltaViewAngles
================
*/
void budActor::SetDeltaViewAngles( const budAngles& delta )
{
	deltaViewAngles = delta;
}

/*
================
budActor::HasEnemies
================
*/
bool budActor::HasEnemies() const
{
	budActor* ent;
	
	for( ent = enemyList.Next(); ent != NULL; ent = ent->enemyNode.Next() )
	{
		if( !ent->fl.hidden )
		{
			return true;
		}
	}
	
	return false;
}

/*
================
budActor::ClosestEnemyToPoint
================
*/
budActor* budActor::ClosestEnemyToPoint( const budVec3& pos )
{
	budActor*		ent;
	budActor*		bestEnt;
	float		bestDistSquared;
	float		distSquared;
	budVec3		delta;
	
	bestDistSquared = budMath::INFINITY;
	bestEnt = NULL;
	for( ent = enemyList.Next(); ent != NULL; ent = ent->enemyNode.Next() )
	{
		if( ent->fl.hidden )
		{
			continue;
		}
		delta = ent->GetPhysics()->GetOrigin() - pos;
		distSquared = delta.LengthSqr();
		if( distSquared < bestDistSquared )
		{
			bestEnt = ent;
			bestDistSquared = distSquared;
		}
	}
	
	return bestEnt;
}

/*
================
budActor::EnemyWithMostHealth
================
*/
budActor* budActor::EnemyWithMostHealth()
{
	budActor*		ent;
	budActor*		bestEnt;
	
	int most = -9999;
	bestEnt = NULL;
	for( ent = enemyList.Next(); ent != NULL; ent = ent->enemyNode.Next() )
	{
		if( !ent->fl.hidden && ( ent->health > most ) )
		{
			bestEnt = ent;
			most = ent->health;
		}
	}
	return bestEnt;
}

/*
================
budActor::OnLadder
================
*/
bool budActor::OnLadder() const
{
	return false;
}

/*
==============
budActor::GetAASLocation
==============
*/
void budActor::GetAASLocation( budAAS* aas, budVec3& pos, int& areaNum ) const
{
	budVec3		size;
	budBounds	bounds;
	
	GetFloorPos( 64.0f, pos );
	if( !aas )
	{
		areaNum = 0;
		return;
	}
	
	size = aas->GetSettings()->boundingBoxes[0][1];
	bounds[0] = -size;
	size.z = 32.0f;
	bounds[1] = size;
	
	areaNum = aas->PointReachableAreaNum( pos, bounds, AREA_REACHABLE_WALK );
	if( areaNum )
	{
		aas->PushPointIntoAreaNum( areaNum, pos );
	}
}

/***********************************************************************

	animation state

***********************************************************************/

/*
=====================
budActor::SetAnimState
=====================
*/
void budActor::SetAnimState( int channel, const char* statename, int blendFrames )
{
	const function_t* func;
	
	func = scriptObject.GetFunction( statename );
	if( !func )
	{
		assert( 0 );
		gameLocal.Error( "Can't find function '%s' in object '%s'", statename, scriptObject.GetTypeName() );
	}
	
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			headAnim.SetState( statename, blendFrames );
			allowEyeFocus = true;
			break;
			
		case ANIMCHANNEL_TORSO :
			torsoAnim.SetState( statename, blendFrames );
			legsAnim.Enable( blendFrames );
			allowPain = true;
			allowEyeFocus = true;
			break;
			
		case ANIMCHANNEL_LEGS :
			legsAnim.SetState( statename, blendFrames );
			torsoAnim.Enable( blendFrames );
			allowPain = true;
			allowEyeFocus = true;
			break;
			
		default:
			gameLocal.Error( "budActor::SetAnimState: Unknown anim group" );
			break;
	}
}

/*
=====================
budActor::GetAnimState
=====================
*/
const char* budActor::GetAnimState( int channel ) const
{
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			return headAnim.state;
			break;
			
		case ANIMCHANNEL_TORSO :
			return torsoAnim.state;
			break;
			
		case ANIMCHANNEL_LEGS :
			return legsAnim.state;
			break;
			
		default:
			gameLocal.Error( "budActor::GetAnimState: Unknown anim group" );
			return NULL;
			break;
	}
}

/*
=====================
budActor::InAnimState
=====================
*/
bool budActor::InAnimState( int channel, const char* statename ) const
{
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			if( headAnim.state == statename )
			{
				return true;
			}
			break;
			
		case ANIMCHANNEL_TORSO :
			if( torsoAnim.state == statename )
			{
				return true;
			}
			break;
			
		case ANIMCHANNEL_LEGS :
			if( legsAnim.state == statename )
			{
				return true;
			}
			break;
			
		default:
			gameLocal.Error( "budActor::InAnimState: Unknown anim group" );
			break;
	}
	
	return false;
}

/*
=====================
budActor::WaitState
=====================
*/
const char* budActor::WaitState() const
{
	if( waitState.Length() )
	{
		return waitState;
	}
	else
	{
		return NULL;
	}
}

/*
=====================
budActor::SetWaitState
=====================
*/
void budActor::SetWaitState( const char* _waitstate )
{
	waitState = _waitstate;
}

/*
=====================
budActor::UpdateAnimState
=====================
*/
void budActor::UpdateAnimState()
{
	headAnim.UpdateState();
	torsoAnim.UpdateState();
	legsAnim.UpdateState();
}

/*
=====================
budActor::GetAnim
=====================
*/
int budActor::GetAnim( int channel, const char* animname )
{
	int			anim;
	const char* temp;
	budAnimator* animatorPtr;
	
	if( channel == ANIMCHANNEL_HEAD )
	{
		if( !head.GetEntity() )
		{
			return 0;
		}
		animatorPtr = head.GetEntity()->GetAnimator();
	}
	else
	{
		animatorPtr = &animator;
	}
	
	if( animPrefix.Length() )
	{
		temp = va( "%s_%s", animPrefix.c_str(), animname );
		anim = animatorPtr->GetAnim( temp );
		if( anim )
		{
			return anim;
		}
	}
	
	anim = animatorPtr->GetAnim( animname );
	
	return anim;
}

/*
===============
budActor::SyncAnimChannels
===============
*/
void budActor::SyncAnimChannels( int channel, int syncToChannel, int blendFrames )
{
	budAnimator*		headAnimator;
	budAFAttachment*	headEnt;
	int				anim;
	budAnimBlend*		syncAnim;
	int				starttime;
	int				blendTime;
	int				cycle;
	
	blendTime = FRAME2MS( blendFrames );
	if( channel == ANIMCHANNEL_HEAD )
	{
		headEnt = head.GetEntity();
		if( headEnt )
		{
			headAnimator = headEnt->GetAnimator();
			syncAnim = animator.CurrentAnim( syncToChannel );
			if( syncAnim )
			{
				anim = headAnimator->GetAnim( syncAnim->AnimFullName() );
				if( !anim )
				{
					anim = headAnimator->GetAnim( syncAnim->AnimName() );
				}
				if( anim )
				{
					cycle = animator.CurrentAnim( syncToChannel )->GetCycleCount();
					starttime = animator.CurrentAnim( syncToChannel )->GetStartTime();
					headAnimator->PlayAnim( ANIMCHANNEL_ALL, anim, gameLocal.time, blendTime );
					headAnimator->CurrentAnim( ANIMCHANNEL_ALL )->SetCycleCount( cycle );
					headAnimator->CurrentAnim( ANIMCHANNEL_ALL )->SetStartTime( starttime );
				}
				else
				{
					headEnt->PlayIdleAnim( blendTime );
				}
			}
		}
	}
	else if( syncToChannel == ANIMCHANNEL_HEAD )
	{
		headEnt = head.GetEntity();
		if( headEnt )
		{
			headAnimator = headEnt->GetAnimator();
			syncAnim = headAnimator->CurrentAnim( ANIMCHANNEL_ALL );
			if( syncAnim )
			{
				anim = GetAnim( channel, syncAnim->AnimFullName() );
				if( !anim )
				{
					anim = GetAnim( channel, syncAnim->AnimName() );
				}
				if( anim )
				{
					cycle = headAnimator->CurrentAnim( ANIMCHANNEL_ALL )->GetCycleCount();
					starttime = headAnimator->CurrentAnim( ANIMCHANNEL_ALL )->GetStartTime();
					animator.PlayAnim( channel, anim, gameLocal.time, blendTime );
					animator.CurrentAnim( channel )->SetCycleCount( cycle );
					animator.CurrentAnim( channel )->SetStartTime( starttime );
				}
			}
		}
	}
	else
	{
		animator.SyncAnimChannels( channel, syncToChannel, gameLocal.time, blendTime );
	}
}

/***********************************************************************

	Damage

***********************************************************************/

/*
============
budActor::Gib
============
*/
void budActor::Gib( const budVec3& dir, const char* damageDefName )
{
	// no gibbing in multiplayer - by self damage or by moving objects
	if( common->IsMultiplayer() )
	{
		return;
	}
	// only gib once
	if( gibbed )
	{
		return;
	}
	budAFEntity_Gibbable::Gib( dir, damageDefName );
	if( head.GetEntity() )
	{
		head.GetEntity()->Hide();
	}
	StopSound( SND_CHANNEL_VOICE, false );
}


/*
============
budActor::Damage

this		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
	example: this=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback in global space
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted

inflictor, attacker, dir, and point can be NULL for environmental effects

Bleeding wounds and surface overlays are applied in the collision code that
calls Damage()
============
*/
budCVar actor_noDamage(	"actor_noDamage",			"0",		CVAR_BOOL, "actors don't take damage -- for testing" );
void budActor::Damage( idEntity* inflictor, idEntity* attacker, const budVec3& dir,
					  const char* damageDefName, const float damageScale, const int location )
{
	if( !fl.takedamage || actor_noDamage.GetBool() )
	{
		return;
	}
	
	if( !inflictor )
	{
		inflictor = gameLocal.world;
	}
	if( !attacker )
	{
		attacker = gameLocal.world;
	}
	
	if( finalBoss && budStr::FindText( GetEntityDefName(), "monster_boss_cyberdemon" ) == 0 && !inflictor->IsType( idSoulCubeMissile::Type ) )
	{
		return;
	}
	
	// for killed by fists achievement
	if( attacker->IsType( budPlayer::Type ) && budStr::Cmp( "damage_fists", damageDefName ) )
	{
		damageNotByFists = true;
	}
	
	SetTimeState ts( timeGroup );
	
	// Helltime boss is immune to all projectiles except the helltime killer
	if( finalBoss && budStr::Icmp( GetEntityDefName(), "monster_hunter_helltime" ) == 0 &&  budStr::Icmp( inflictor->GetEntityDefName(), "projectile_helltime_killer" ) )
	{
		return;
	}
	
	// Maledict is immume to the falling asteroids
	if( !budStr::Icmp( GetEntityDefName(), "monster_boss_d3xp_maledict" ) &&
			( !budStr::Icmp( damageDefName, "damage_maledict_asteroid" ) || !budStr::Icmp( damageDefName, "damage_maledict_asteroid_splash" ) ) )
	{
		return;
	}
	
	const idDict* damageDef = gameLocal.FindEntityDefDict( damageDefName );
	if( damageDef == NULL )
	{
		gameLocal.Error( "Unknown damageDef '%s'", damageDefName );
		return;
	}
	
	int	damage = damageDef->GetInt( "damage" ) * damageScale;
	damage = GetDamageForLocation( damage, location );
	
	// inform the attacker that they hit someone
	if( attacker )
	{
		attacker->DamageFeedback( this, inflictor, damage );
	}
	if( damage > 0 )
	{
		int oldHealth = health;
		health -= damage;
		
		//Check the health against any damage cap that is currently set
		if( damageCap >= 0 && health < damageCap )
		{
			health = damageCap;
		}
		
		if( health <= 0 )
		{
			if( health < -999 )
			{
				health = -999;
			}
			
			if( oldHealth > 0 )
			{
				budPlayer* player = NULL;
				if( ( attacker && attacker->IsType( budPlayer::Type ) ) )
				{
					player = static_cast< budPlayer* >( attacker );
				}
				
				if( player != NULL )
				{
					if( !damageNotByFists && player->GetExpansionType() == GAME_BASE )
					{
						player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_KILL_20_ENEMY_FISTS_HANDS );
					}
					if( player->PowerUpActive( HELLTIME ) )
					{
						player->GetAchievementManager().IncrementHellTimeKills();
					}
					if( player->PowerUpActive( BERSERK ) && player->GetExpansionType() == GAME_D3XP && !damageNotByFists )
					{
						player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_ARTIFACT_WITH_BERSERK_PUNCH_20 );
					}
					if( player->GetCurrentWeaponSlot() == player->weapon_chainsaw )
					{
						player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_KILL_20_ENEMY_WITH_CHAINSAW );
					}
					if( ( !name.Find( "monster_boss_vagary" ) || !name.Find( "vagaryaftercin" ) ) && player->GetExpansionType() == GAME_BASE )
					{
						player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_DEFEAT_VAGARY_BOSS );
					}
					if( !name.Find( "monster_boss_sabaoth" ) )
					{
						player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_DEFEAT_SABAOTH_BOSS );
					}
					if( !name.Find( "monster_boss_cyberdemon" ) )
					{
						player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_DEFEAT_CYBERDEMON_BOSS );
					}
					if( name.Icmp( "hunter_berzerk" ) == 0 )
					{
						player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_DEFEAT_BERSERK_HUNTER );
					}
					if( !name.Find( "monster_hunter_helltime" ) )
					{
						player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_DEFEAT_HELLTIME_HUNTER );
					}
					if( name.Icmp( "hunter" ) == 0 )
					{
						player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_DEFEAT_INVULNERABILITY_HUNTER );
					}
					if( inflictor && inflictor->IsType( idSoulCubeMissile::Type ) )
					{
						player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_USE_SOUL_CUBE_TO_DEFEAT_20_ENEMY );
					}
					if( inflictor && inflictor->IsType( idMoveable::Type ) )
					{
						idMoveable* moveable = static_cast< idMoveable* >( inflictor );
						// if a moveable is doing damage
						// AND it has an attacker (set when the grabber picks up a moveable )
						// AND the moveable's attacker is the attacker here (the player)
						// then the player has killed an enemy with a launched moveable from the Grabber
						if( moveable != NULL && moveable->GetAttacker() != NULL && moveable->GetAttacker()->IsType( budPlayer::Type ) && moveable->GetAttacker() == attacker && player->GetExpansionType() == GAME_D3XP && team != player->team )
						{
							player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_GRABBER_KILL_20_ENEMY );
						}
					}
					
					idProjectile* projectile = NULL;
					if( inflictor != NULL && inflictor->IsType( idProjectile::Type ) )
					{
						projectile = static_cast< idProjectile* >( inflictor );
						if( projectile != NULL )
						{
							if( projectile->GetLaunchedFromGrabber() && player->GetExpansionType() == GAME_D3XP && team != player->team )
							{
								player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_GRABBER_KILL_20_ENEMY );
							}
							if( renderEntity.hModel && budStr::Icmp( renderEntity.hModel->Name(), "models/md5/monsters/imp/imp.md5mesh" ) == 0 )
							{
								if( budStr::FindText( inflictor->GetName(), "shotgun" ) > -1 )
								{
									budStr impName;
									int	  lastKilledImpTime = player->GetAchievementManager().GetLastImpKilledTime();
									if( ( gameLocal.GetTime() - lastKilledImpTime ) <= 100 && ( impName.Icmp( name ) != 0 ) )
									{
										player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_KILL_TWO_IMPS_ONE_SHOTGUN );
									}
									else
									{
										player->GetAchievementManager().SetLastImpKilledTime( gameLocal.GetTime() );
									}
								}
							}
						}
					}
					
					if( player->health == 1 && player->team != this->team )  	// make sure it doesn't unlock if you kill a friendly dude when you have 1 heath....
					{
						player->GetAchievementManager().EventCompletesAchievement( ACHIEVEMENT_KILL_MONSTER_WITH_1_HEALTH_LEFT );
					}
				}
			}
			
			Killed( inflictor, attacker, damage, dir, location );
			if( ( health < -20 ) && spawnArgs.GetBool( "gib" ) && damageDef->GetBool( "gib" ) )
			{
				Gib( dir, damageDefName );
			}
		}
		else
		{
			Pain( inflictor, attacker, damage, dir, location );
		}
	}
	else
	{
		// don't accumulate knockback
		if( af.IsLoaded() )
		{
			// clear impacts
			af.Rest();
			
			// physics is turned off by calling af.Rest()
			BecomeActive( TH_PHYSICS );
		}
	}
}

/*
=====================
budActor::ClearPain
=====================
*/
void budActor::ClearPain()
{
	pain_debounce_time = 0;
}

/*
=====================
budActor::Pain
=====================
*/
bool budActor::Pain( idEntity* inflictor, idEntity* attacker, int damage, const budVec3& dir, int location )
{
	if( af.IsLoaded() )
	{
		// clear impacts
		af.Rest();
		
		// physics is turned off by calling af.Rest()
		BecomeActive( TH_PHYSICS );
	}
	
	if( gameLocal.time < pain_debounce_time )
	{
		return false;
	}
	
	// don't play pain sounds more than necessary
	pain_debounce_time = gameLocal.time + pain_delay;
	
	if( health > 75 )
	{
		StartSound( "snd_pain_small", SND_CHANNEL_VOICE, 0, false, NULL );
	}
	else if( health > 50 )
	{
		StartSound( "snd_pain_medium", SND_CHANNEL_VOICE, 0, false, NULL );
	}
	else if( health > 25 )
	{
		StartSound( "snd_pain_large", SND_CHANNEL_VOICE, 0, false, NULL );
	}
	else
	{
		StartSound( "snd_pain_huge", SND_CHANNEL_VOICE, 0, false, NULL );
	}
	
	if( !allowPain || ( gameLocal.time < painTime ) )
	{
		// don't play a pain anim
		return false;
	}
	
	if( pain_threshold && ( damage < pain_threshold ) )
	{
		return false;
	}
	
	// set the pain anim
	budStr damageGroup = GetDamageGroup( location );
	
	painAnim = "";
	if( animPrefix.Length() )
	{
		if( damageGroup.Length() && ( damageGroup != "legs" ) )
		{
			sprintf( painAnim, "%s_pain_%s", animPrefix.c_str(), damageGroup.c_str() );
			if( !animator.HasAnim( painAnim ) )
			{
				sprintf( painAnim, "pain_%s", damageGroup.c_str() );
				if( !animator.HasAnim( painAnim ) )
				{
					painAnim = "";
				}
			}
		}
		
		if( !painAnim.Length() )
		{
			sprintf( painAnim, "%s_pain", animPrefix.c_str() );
			if( !animator.HasAnim( painAnim ) )
			{
				painAnim = "";
			}
		}
	}
	else if( damageGroup.Length() && ( damageGroup != "legs" ) )
	{
		sprintf( painAnim, "pain_%s", damageGroup.c_str() );
		if( !animator.HasAnim( painAnim ) )
		{
			sprintf( painAnim, "pain_%s", damageGroup.c_str() );
			if( !animator.HasAnim( painAnim ) )
			{
				painAnim = "";
			}
		}
	}
	
	if( !painAnim.Length() )
	{
		painAnim = "pain";
	}
	
	if( g_debugDamage.GetBool() )
	{
		gameLocal.Printf( "Damage: joint: '%s', zone '%s', anim '%s'\n", animator.GetJointName( ( jointHandle_t )location ),
						  damageGroup.c_str(), painAnim.c_str() );
	}
	
	return true;
}

/*
=====================
budActor::SpawnGibs
=====================
*/
void budActor::SpawnGibs( const budVec3& dir, const char* damageDefName )
{
	budAFEntity_Gibbable::SpawnGibs( dir, damageDefName );
	RemoveAttachments();
}

/*
=====================
budActor::SetupDamageGroups

FIXME: only store group names once and store an index for each joint
=====================
*/
void budActor::SetupDamageGroups()
{
	int						i;
	const idKeyValue*		arg;
	budStr					groupname;
	budList<jointHandle_t>	jointList;
	int						jointnum;
	float					scale;
	
	// create damage zones
	damageGroups.SetNum( animator.NumJoints() );
	arg = spawnArgs.MatchPrefix( "damage_zone ", NULL );
	while( arg )
	{
		groupname = arg->GetKey();
		groupname.Strip( "damage_zone " );
		animator.GetJointList( arg->GetValue(), jointList );
		for( i = 0; i < jointList.Num(); i++ )
		{
			jointnum = jointList[ i ];
			damageGroups[ jointnum ] = groupname;
		}
		jointList.Clear();
		arg = spawnArgs.MatchPrefix( "damage_zone ", arg );
	}
	
	// initilize the damage zones to normal damage
	damageScale.SetNum( animator.NumJoints() );
	for( i = 0; i < damageScale.Num(); i++ )
	{
		damageScale[ i ] = 1.0f;
	}
	
	// set the percentage on damage zones
	arg = spawnArgs.MatchPrefix( "damage_scale ", NULL );
	while( arg )
	{
		scale = atof( arg->GetValue() );
		groupname = arg->GetKey();
		groupname.Strip( "damage_scale " );
		for( i = 0; i < damageScale.Num(); i++ )
		{
			if( damageGroups[ i ] == groupname )
			{
				damageScale[ i ] = scale;
			}
		}
		arg = spawnArgs.MatchPrefix( "damage_scale ", arg );
	}
}

/*
=====================
budActor::GetDamageForLocation
=====================
*/
int budActor::GetDamageForLocation( int damage, int location )
{
	if( ( location < 0 ) || ( location >= damageScale.Num() ) )
	{
		return damage;
	}
	
	return ( int )ceil( damage * damageScale[ location ] );
}

/*
=====================
budActor::GetDamageGroup
=====================
*/
const char* budActor::GetDamageGroup( int location )
{
	if( ( location < 0 ) || ( location >= damageGroups.Num() ) )
	{
		return "";
	}
	
	return damageGroups[ location ];
}


/***********************************************************************

	Events

***********************************************************************/

/*
=====================
budActor::Event_EnableEyeFocus
=====================
*/
void budActor::PlayFootStepSound()
{
	const char* sound = NULL;
	const budMaterial* material;
	
	if( !GetPhysics()->HasGroundContacts() )
	{
		return;
	}
	
	// start footstep sound based on material type
	material = GetPhysics()->GetContact( 0 ).material;
	if( material != NULL )
	{
		sound = spawnArgs.GetString( va( "snd_footstep_%s", gameLocal.sufaceTypeNames[ material->GetSurfaceType() ] ) );
	}
	if( sound != NULL && *sound == '\0' )
	{
		sound = spawnArgs.GetString( "snd_footstep" );
	}
	if( sound != NULL && *sound != '\0' )
	{
		StartSoundShader( declManager->FindSound( sound ), SND_CHANNEL_BODY, 0, false, NULL );
	}
}

/*
=====================
budActor::Event_EnableEyeFocus
=====================
*/
void budActor::Event_EnableEyeFocus()
{
	allowEyeFocus = true;
	blink_time = gameLocal.time + blink_min + gameLocal.random.RandomFloat() * ( blink_max - blink_min );
}

/*
=====================
budActor::Event_DisableEyeFocus
=====================
*/
void budActor::Event_DisableEyeFocus()
{
	allowEyeFocus = false;
	
	idEntity* headEnt = head.GetEntity();
	if( headEnt )
	{
		headEnt->GetAnimator()->Clear( ANIMCHANNEL_EYELIDS, gameLocal.time, FRAME2MS( 2 ) );
	}
	else
	{
		animator.Clear( ANIMCHANNEL_EYELIDS, gameLocal.time, FRAME2MS( 2 ) );
	}
}

/*
===============
budActor::Event_Footstep
===============
*/
void budActor::Event_Footstep()
{
	PlayFootStepSound();
}

/*
=====================
budActor::Event_EnableWalkIK
=====================
*/
void budActor::Event_EnableWalkIK()
{
	walkIK.EnableAll();
}

/*
=====================
budActor::Event_DisableWalkIK
=====================
*/
void budActor::Event_DisableWalkIK()
{
	walkIK.DisableAll();
}

/*
=====================
budActor::Event_EnableLegIK
=====================
*/
void budActor::Event_EnableLegIK( int num )
{
	walkIK.EnableLeg( num );
}

/*
=====================
budActor::Event_DisableLegIK
=====================
*/
void budActor::Event_DisableLegIK( int num )
{
	walkIK.DisableLeg( num );
}

/*
=====================
budActor::Event_PreventPain
=====================
*/
void budActor::Event_PreventPain( float duration )
{
	painTime = gameLocal.time + SEC2MS( duration );
}

/*
===============
budActor::Event_DisablePain
===============
*/
void budActor::Event_DisablePain()
{
	allowPain = false;
}

/*
===============
budActor::Event_EnablePain
===============
*/
void budActor::Event_EnablePain()
{
	allowPain = true;
}

/*
=====================
budActor::Event_GetPainAnim
=====================
*/
void budActor::Event_GetPainAnim()
{
	if( !painAnim.Length() )
	{
		idThread::ReturnString( "pain" );
	}
	else
	{
		idThread::ReturnString( painAnim );
	}
}

/*
=====================
budActor::Event_SetAnimPrefix
=====================
*/
void budActor::Event_SetAnimPrefix( const char* prefix )
{
	animPrefix = prefix;
}

/*
===============
budActor::Event_StopAnim
===============
*/
void budActor::Event_StopAnim( int channel, int frames )
{
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			headAnim.StopAnim( frames );
			break;
			
		case ANIMCHANNEL_TORSO :
			torsoAnim.StopAnim( frames );
			break;
			
		case ANIMCHANNEL_LEGS :
			legsAnim.StopAnim( frames );
			break;
			
		default:
			gameLocal.Error( "Unknown anim group" );
			break;
	}
}

/*
===============
budActor::Event_PlayAnim
===============
*/
void budActor::Event_PlayAnim( int channel, const char* animname )
{
	animFlags_t	flags;
	idEntity* headEnt;
	int	anim;
	
	anim = GetAnim( channel, animname );
	if( !anim )
	{
		if( ( channel == ANIMCHANNEL_HEAD ) && head.GetEntity() )
		{
			gameLocal.DPrintf( "missing '%s' animation on '%s' (%s)\n", animname, name.c_str(), spawnArgs.GetString( "def_head", "" ) );
		}
		else
		{
			gameLocal.DPrintf( "missing '%s' animation on '%s' (%s)\n", animname, name.c_str(), GetEntityDefName() );
		}
		idThread::ReturnInt( 0 );
		return;
	}
	
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			headEnt = head.GetEntity();
			if( headEnt )
			{
				headAnim.idleAnim = false;
				headAnim.PlayAnim( anim );
				flags = headAnim.GetAnimFlags();
				if( !flags.prevent_idle_override )
				{
					if( torsoAnim.IsIdle() )
					{
						torsoAnim.animBlendFrames = headAnim.lastAnimBlendFrames;
						SyncAnimChannels( ANIMCHANNEL_TORSO, ANIMCHANNEL_HEAD, headAnim.lastAnimBlendFrames );
						if( legsAnim.IsIdle() )
						{
							legsAnim.animBlendFrames = headAnim.lastAnimBlendFrames;
							SyncAnimChannels( ANIMCHANNEL_LEGS, ANIMCHANNEL_HEAD, headAnim.lastAnimBlendFrames );
						}
					}
				}
			}
			break;
			
		case ANIMCHANNEL_TORSO :
			torsoAnim.idleAnim = false;
			torsoAnim.PlayAnim( anim );
			flags = torsoAnim.GetAnimFlags();
			if( !flags.prevent_idle_override )
			{
				if( headAnim.IsIdle() )
				{
					headAnim.animBlendFrames = torsoAnim.lastAnimBlendFrames;
					SyncAnimChannels( ANIMCHANNEL_HEAD, ANIMCHANNEL_TORSO, torsoAnim.lastAnimBlendFrames );
				}
				if( legsAnim.IsIdle() )
				{
					legsAnim.animBlendFrames = torsoAnim.lastAnimBlendFrames;
					SyncAnimChannels( ANIMCHANNEL_LEGS, ANIMCHANNEL_TORSO, torsoAnim.lastAnimBlendFrames );
				}
			}
			break;
			
		case ANIMCHANNEL_LEGS :
			legsAnim.idleAnim = false;
			legsAnim.PlayAnim( anim );
			flags = legsAnim.GetAnimFlags();
			if( !flags.prevent_idle_override )
			{
				if( torsoAnim.IsIdle() )
				{
					torsoAnim.animBlendFrames = legsAnim.lastAnimBlendFrames;
					SyncAnimChannels( ANIMCHANNEL_TORSO, ANIMCHANNEL_LEGS, legsAnim.lastAnimBlendFrames );
					if( headAnim.IsIdle() )
					{
						headAnim.animBlendFrames = legsAnim.lastAnimBlendFrames;
						SyncAnimChannels( ANIMCHANNEL_HEAD, ANIMCHANNEL_LEGS, legsAnim.lastAnimBlendFrames );
					}
				}
			}
			break;
			
		default :
			gameLocal.Error( "Unknown anim group" );
			break;
	}
	idThread::ReturnInt( 1 );
}

/*
===============
budActor::Event_PlayCycle
===============
*/
void budActor::Event_PlayCycle( int channel, const char* animname )
{
	animFlags_t	flags;
	int			anim;
	
	anim = GetAnim( channel, animname );
	if( !anim )
	{
		if( ( channel == ANIMCHANNEL_HEAD ) && head.GetEntity() )
		{
			gameLocal.DPrintf( "missing '%s' animation on '%s' (%s)\n", animname, name.c_str(), spawnArgs.GetString( "def_head", "" ) );
		}
		else
		{
			gameLocal.DPrintf( "missing '%s' animation on '%s' (%s)\n", animname, name.c_str(), GetEntityDefName() );
		}
		idThread::ReturnInt( false );
		return;
	}
	
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			headAnim.idleAnim = false;
			headAnim.CycleAnim( anim );
			flags = headAnim.GetAnimFlags();
			if( !flags.prevent_idle_override )
			{
				if( torsoAnim.IsIdle() && legsAnim.IsIdle() )
				{
					torsoAnim.animBlendFrames = headAnim.lastAnimBlendFrames;
					SyncAnimChannels( ANIMCHANNEL_TORSO, ANIMCHANNEL_HEAD, headAnim.lastAnimBlendFrames );
					legsAnim.animBlendFrames = headAnim.lastAnimBlendFrames;
					SyncAnimChannels( ANIMCHANNEL_LEGS, ANIMCHANNEL_HEAD, headAnim.lastAnimBlendFrames );
				}
			}
			break;
			
		case ANIMCHANNEL_TORSO :
			torsoAnim.idleAnim = false;
			torsoAnim.CycleAnim( anim );
			flags = torsoAnim.GetAnimFlags();
			if( !flags.prevent_idle_override )
			{
				if( headAnim.IsIdle() )
				{
					headAnim.animBlendFrames = torsoAnim.lastAnimBlendFrames;
					SyncAnimChannels( ANIMCHANNEL_HEAD, ANIMCHANNEL_TORSO, torsoAnim.lastAnimBlendFrames );
				}
				if( legsAnim.IsIdle() )
				{
					legsAnim.animBlendFrames = torsoAnim.lastAnimBlendFrames;
					SyncAnimChannels( ANIMCHANNEL_LEGS, ANIMCHANNEL_TORSO, torsoAnim.lastAnimBlendFrames );
				}
			}
			break;
			
		case ANIMCHANNEL_LEGS :
			legsAnim.idleAnim = false;
			legsAnim.CycleAnim( anim );
			flags = legsAnim.GetAnimFlags();
			if( !flags.prevent_idle_override )
			{
				if( torsoAnim.IsIdle() )
				{
					torsoAnim.animBlendFrames = legsAnim.lastAnimBlendFrames;
					SyncAnimChannels( ANIMCHANNEL_TORSO, ANIMCHANNEL_LEGS, legsAnim.lastAnimBlendFrames );
					if( headAnim.IsIdle() )
					{
						headAnim.animBlendFrames = legsAnim.lastAnimBlendFrames;
						SyncAnimChannels( ANIMCHANNEL_HEAD, ANIMCHANNEL_LEGS, legsAnim.lastAnimBlendFrames );
					}
				}
			}
			break;
			
		default:
			gameLocal.Error( "Unknown anim group" );
	}
	
	idThread::ReturnInt( true );
}

/*
===============
budActor::Event_IdleAnim
===============
*/
void budActor::Event_IdleAnim( int channel, const char* animname )
{
	int anim;
	
	anim = GetAnim( channel, animname );
	if( !anim )
	{
		if( ( channel == ANIMCHANNEL_HEAD ) && head.GetEntity() )
		{
			gameLocal.DPrintf( "missing '%s' animation on '%s' (%s)\n", animname, name.c_str(), spawnArgs.GetString( "def_head", "" ) );
		}
		else
		{
			gameLocal.DPrintf( "missing '%s' animation on '%s' (%s)\n", animname, name.c_str(), GetEntityDefName() );
		}
		
		switch( channel )
		{
			case ANIMCHANNEL_HEAD :
				headAnim.BecomeIdle();
				break;
				
			case ANIMCHANNEL_TORSO :
				torsoAnim.BecomeIdle();
				break;
				
			case ANIMCHANNEL_LEGS :
				legsAnim.BecomeIdle();
				break;
				
			default:
				gameLocal.Error( "Unknown anim group" );
		}
		
		idThread::ReturnInt( false );
		return;
	}
	
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			headAnim.BecomeIdle();
			if( torsoAnim.GetAnimFlags().prevent_idle_override )
			{
				// don't sync to torso body if it doesn't override idle anims
				headAnim.CycleAnim( anim );
			}
			else if( torsoAnim.IsIdle() && legsAnim.IsIdle() )
			{
				// everything is idle, so play the anim on the head and copy it to the torso and legs
				headAnim.CycleAnim( anim );
				torsoAnim.animBlendFrames = headAnim.lastAnimBlendFrames;
				SyncAnimChannels( ANIMCHANNEL_TORSO, ANIMCHANNEL_HEAD, headAnim.lastAnimBlendFrames );
				legsAnim.animBlendFrames = headAnim.lastAnimBlendFrames;
				SyncAnimChannels( ANIMCHANNEL_LEGS, ANIMCHANNEL_HEAD, headAnim.lastAnimBlendFrames );
			}
			else if( torsoAnim.IsIdle() )
			{
				// sync the head and torso to the legs
				SyncAnimChannels( ANIMCHANNEL_HEAD, ANIMCHANNEL_LEGS, headAnim.animBlendFrames );
				torsoAnim.animBlendFrames = headAnim.lastAnimBlendFrames;
				SyncAnimChannels( ANIMCHANNEL_TORSO, ANIMCHANNEL_LEGS, torsoAnim.animBlendFrames );
			}
			else
			{
				// sync the head to the torso
				SyncAnimChannels( ANIMCHANNEL_HEAD, ANIMCHANNEL_TORSO, headAnim.animBlendFrames );
			}
			break;
			
		case ANIMCHANNEL_TORSO :
			torsoAnim.BecomeIdle();
			if( legsAnim.GetAnimFlags().prevent_idle_override )
			{
				// don't sync to legs if legs anim doesn't override idle anims
				torsoAnim.CycleAnim( anim );
			}
			else if( legsAnim.IsIdle() )
			{
				// play the anim in both legs and torso
				torsoAnim.CycleAnim( anim );
				legsAnim.animBlendFrames = torsoAnim.lastAnimBlendFrames;
				SyncAnimChannels( ANIMCHANNEL_LEGS, ANIMCHANNEL_TORSO, torsoAnim.lastAnimBlendFrames );
			}
			else
			{
				// sync the anim to the legs
				SyncAnimChannels( ANIMCHANNEL_TORSO, ANIMCHANNEL_LEGS, torsoAnim.animBlendFrames );
			}
			
			if( headAnim.IsIdle() )
			{
				SyncAnimChannels( ANIMCHANNEL_HEAD, ANIMCHANNEL_TORSO, torsoAnim.lastAnimBlendFrames );
			}
			break;
			
		case ANIMCHANNEL_LEGS :
			legsAnim.BecomeIdle();
			if( torsoAnim.GetAnimFlags().prevent_idle_override )
			{
				// don't sync to torso if torso anim doesn't override idle anims
				legsAnim.CycleAnim( anim );
			}
			else if( torsoAnim.IsIdle() )
			{
				// play the anim in both legs and torso
				legsAnim.CycleAnim( anim );
				torsoAnim.animBlendFrames = legsAnim.lastAnimBlendFrames;
				SyncAnimChannels( ANIMCHANNEL_TORSO, ANIMCHANNEL_LEGS, legsAnim.lastAnimBlendFrames );
				if( headAnim.IsIdle() )
				{
					SyncAnimChannels( ANIMCHANNEL_HEAD, ANIMCHANNEL_LEGS, legsAnim.lastAnimBlendFrames );
				}
			}
			else
			{
				// sync the anim to the torso
				SyncAnimChannels( ANIMCHANNEL_LEGS, ANIMCHANNEL_TORSO, legsAnim.animBlendFrames );
			}
			break;
			
		default:
			gameLocal.Error( "Unknown anim group" );
	}
	
	idThread::ReturnInt( true );
}

/*
================
budActor::Event_SetSyncedAnimWeight
================
*/
void budActor::Event_SetSyncedAnimWeight( int channel, int anim, float weight )
{
	idEntity* headEnt;
	
	headEnt = head.GetEntity();
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			if( headEnt )
			{
				animator.CurrentAnim( ANIMCHANNEL_ALL )->SetSyncedAnimWeight( anim, weight );
			}
			else
			{
				animator.CurrentAnim( ANIMCHANNEL_HEAD )->SetSyncedAnimWeight( anim, weight );
			}
			if( torsoAnim.IsIdle() )
			{
				animator.CurrentAnim( ANIMCHANNEL_TORSO )->SetSyncedAnimWeight( anim, weight );
				if( legsAnim.IsIdle() )
				{
					animator.CurrentAnim( ANIMCHANNEL_LEGS )->SetSyncedAnimWeight( anim, weight );
				}
			}
			break;
			
		case ANIMCHANNEL_TORSO :
			animator.CurrentAnim( ANIMCHANNEL_TORSO )->SetSyncedAnimWeight( anim, weight );
			if( legsAnim.IsIdle() )
			{
				animator.CurrentAnim( ANIMCHANNEL_LEGS )->SetSyncedAnimWeight( anim, weight );
			}
			if( headEnt && headAnim.IsIdle() )
			{
				animator.CurrentAnim( ANIMCHANNEL_ALL )->SetSyncedAnimWeight( anim, weight );
			}
			break;
			
		case ANIMCHANNEL_LEGS :
			animator.CurrentAnim( ANIMCHANNEL_LEGS )->SetSyncedAnimWeight( anim, weight );
			if( torsoAnim.IsIdle() )
			{
				animator.CurrentAnim( ANIMCHANNEL_TORSO )->SetSyncedAnimWeight( anim, weight );
				if( headEnt && headAnim.IsIdle() )
				{
					animator.CurrentAnim( ANIMCHANNEL_ALL )->SetSyncedAnimWeight( anim, weight );
				}
			}
			break;
			
		default:
			gameLocal.Error( "Unknown anim group" );
	}
}

/*
===============
budActor::Event_OverrideAnim
===============
*/
void budActor::Event_OverrideAnim( int channel )
{
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			headAnim.Disable();
			if( !torsoAnim.IsIdle() )
			{
				SyncAnimChannels( ANIMCHANNEL_HEAD, ANIMCHANNEL_TORSO, torsoAnim.lastAnimBlendFrames );
			}
			else
			{
				SyncAnimChannels( ANIMCHANNEL_HEAD, ANIMCHANNEL_LEGS, legsAnim.lastAnimBlendFrames );
			}
			break;
			
		case ANIMCHANNEL_TORSO :
			torsoAnim.Disable();
			SyncAnimChannels( ANIMCHANNEL_TORSO, ANIMCHANNEL_LEGS, legsAnim.lastAnimBlendFrames );
			if( headAnim.IsIdle() )
			{
				SyncAnimChannels( ANIMCHANNEL_HEAD, ANIMCHANNEL_TORSO, torsoAnim.lastAnimBlendFrames );
			}
			break;
			
		case ANIMCHANNEL_LEGS :
			legsAnim.Disable();
			SyncAnimChannels( ANIMCHANNEL_LEGS, ANIMCHANNEL_TORSO, torsoAnim.lastAnimBlendFrames );
			break;
			
		default:
			gameLocal.Error( "Unknown anim group" );
			break;
	}
}

/*
===============
budActor::Event_EnableAnim
===============
*/
void budActor::Event_EnableAnim( int channel, int blendFrames )
{
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			headAnim.Enable( blendFrames );
			break;
			
		case ANIMCHANNEL_TORSO :
			torsoAnim.Enable( blendFrames );
			break;
			
		case ANIMCHANNEL_LEGS :
			legsAnim.Enable( blendFrames );
			break;
			
		default:
			gameLocal.Error( "Unknown anim group" );
			break;
	}
}

/*
===============
budActor::Event_SetBlendFrames
===============
*/
void budActor::Event_SetBlendFrames( int channel, int blendFrames )
{
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			headAnim.animBlendFrames = blendFrames;
			headAnim.lastAnimBlendFrames = blendFrames;
			break;
			
		case ANIMCHANNEL_TORSO :
			torsoAnim.animBlendFrames = blendFrames;
			torsoAnim.lastAnimBlendFrames = blendFrames;
			break;
			
		case ANIMCHANNEL_LEGS :
			legsAnim.animBlendFrames = blendFrames;
			legsAnim.lastAnimBlendFrames = blendFrames;
			break;
			
		default:
			gameLocal.Error( "Unknown anim group" );
			break;
	}
}

/*
===============
budActor::Event_GetBlendFrames
===============
*/
void budActor::Event_GetBlendFrames( int channel )
{
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			idThread::ReturnInt( headAnim.animBlendFrames );
			break;
			
		case ANIMCHANNEL_TORSO :
			idThread::ReturnInt( torsoAnim.animBlendFrames );
			break;
			
		case ANIMCHANNEL_LEGS :
			idThread::ReturnInt( legsAnim.animBlendFrames );
			break;
			
		default:
			gameLocal.Error( "Unknown anim group" );
			break;
	}
}

/*
===============
budActor::Event_AnimState
===============
*/
void budActor::Event_AnimState( int channel, const char* statename, int blendFrames )
{
	SetAnimState( channel, statename, blendFrames );
}

/*
===============
budActor::Event_GetAnimState
===============
*/
void budActor::Event_GetAnimState( int channel )
{
	const char* state;
	
	state = GetAnimState( channel );
	idThread::ReturnString( state );
}

/*
===============
budActor::Event_InAnimState
===============
*/
void budActor::Event_InAnimState( int channel, const char* statename )
{
	bool instate;
	
	instate = InAnimState( channel, statename );
	idThread::ReturnInt( instate );
}

/*
===============
budActor::Event_FinishAction
===============
*/
void budActor::Event_FinishAction( const char* actionname )
{
	if( waitState == actionname )
	{
		SetWaitState( "" );
	}
}

/*
===============
budActor::Event_AnimDone
===============
*/
void budActor::Event_AnimDone( int channel, int blendFrames )
{
	bool result;
	
	switch( channel )
	{
		case ANIMCHANNEL_HEAD :
			result = headAnim.AnimDone( blendFrames );
			idThread::ReturnInt( result );
			break;
			
		case ANIMCHANNEL_TORSO :
			result = torsoAnim.AnimDone( blendFrames );
			idThread::ReturnInt( result );
			break;
			
		case ANIMCHANNEL_LEGS :
			result = legsAnim.AnimDone( blendFrames );
			idThread::ReturnInt( result );
			break;
			
		default:
			gameLocal.Error( "Unknown anim group" );
	}
}

/*
================
budActor::Event_HasAnim
================
*/
void budActor::Event_HasAnim( int channel, const char* animname )
{
	if( GetAnim( channel, animname ) != 0 )
	{
		idThread::ReturnFloat( 1.0f );
	}
	else
	{
		idThread::ReturnFloat( 0.0f );
	}
}

/*
================
budActor::Event_CheckAnim
================
*/
void budActor::Event_CheckAnim( int channel, const char* animname )
{
	if( !GetAnim( channel, animname ) )
	{
		if( animPrefix.Length() )
		{
			gameLocal.Error( "Can't find anim '%s_%s' for '%s'", animPrefix.c_str(), animname, name.c_str() );
		}
		else
		{
			gameLocal.Error( "Can't find anim '%s' for '%s'", animname, name.c_str() );
		}
	}
}

/*
================
budActor::Event_ChooseAnim
================
*/
void budActor::Event_ChooseAnim( int channel, const char* animname )
{
	int anim;
	
	anim = GetAnim( channel, animname );
	if( anim )
	{
		if( channel == ANIMCHANNEL_HEAD )
		{
			if( head.GetEntity() )
			{
				idThread::ReturnString( head.GetEntity()->GetAnimator()->AnimFullName( anim ) );
				return;
			}
		}
		else
		{
			idThread::ReturnString( animator.AnimFullName( anim ) );
			return;
		}
	}
	
	idThread::ReturnString( "" );
}

/*
================
budActor::Event_AnimLength
================
*/
void budActor::Event_AnimLength( int channel, const char* animname )
{
	int anim;
	
	anim = GetAnim( channel, animname );
	if( anim )
	{
		if( channel == ANIMCHANNEL_HEAD )
		{
			if( head.GetEntity() )
			{
				idThread::ReturnFloat( MS2SEC( head.GetEntity()->GetAnimator()->AnimLength( anim ) ) );
				return;
			}
		}
		else
		{
			idThread::ReturnFloat( MS2SEC( animator.AnimLength( anim ) ) );
			return;
		}
	}
	
	idThread::ReturnFloat( 0.0f );
}

/*
================
budActor::Event_AnimDistance
================
*/
void budActor::Event_AnimDistance( int channel, const char* animname )
{
	int anim;
	
	anim = GetAnim( channel, animname );
	if( anim )
	{
		if( channel == ANIMCHANNEL_HEAD )
		{
			if( head.GetEntity() )
			{
				idThread::ReturnFloat( head.GetEntity()->GetAnimator()->TotalMovementDelta( anim ).Length() );
				return;
			}
		}
		else
		{
			idThread::ReturnFloat( animator.TotalMovementDelta( anim ).Length() );
			return;
		}
	}
	
	idThread::ReturnFloat( 0.0f );
}

/*
================
budActor::Event_HasEnemies
================
*/
void budActor::Event_HasEnemies()
{
	bool hasEnemy;
	
	hasEnemy = HasEnemies();
	idThread::ReturnInt( hasEnemy );
}

/*
================
budActor::Event_NextEnemy
================
*/
void budActor::Event_NextEnemy( idEntity* ent )
{
	budActor* actor;
	
	if( !ent || ( ent == this ) )
	{
		actor = enemyList.Next();
	}
	else
	{
		if( !ent->IsType( budActor::Type ) )
		{
			gameLocal.Error( "'%s' cannot be an enemy", ent->name.c_str() );
		}
		
		actor = static_cast<budActor*>( ent );
		if( actor->enemyNode.ListHead() != &enemyList )
		{
			gameLocal.Error( "'%s' is not in '%s' enemy list", actor->name.c_str(), name.c_str() );
		}
	}
	
	for( ; actor != NULL; actor = actor->enemyNode.Next() )
	{
		if( !actor->fl.hidden )
		{
			idThread::ReturnEntity( actor );
			return;
		}
	}
	
	idThread::ReturnEntity( NULL );
}

/*
================
budActor::Event_ClosestEnemyToPoint
================
*/
void budActor::Event_ClosestEnemyToPoint( const budVec3& pos )
{
	budActor* bestEnt = ClosestEnemyToPoint( pos );
	idThread::ReturnEntity( bestEnt );
}

/*
================
budActor::Event_StopSound
================
*/
void budActor::Event_StopSound( int channel, int netSync )
{
	if( channel == SND_CHANNEL_VOICE )
	{
		idEntity* headEnt = head.GetEntity();
		if( headEnt )
		{
			headEnt->StopSound( channel, ( netSync != 0 ) );
		}
	}
	StopSound( channel, ( netSync != 0 ) );
}

/*
=====================
budActor::Event_SetNextState
=====================
*/
void budActor::Event_SetNextState( const char* name )
{
	idealState = GetScriptFunction( name );
	if( idealState == state )
	{
		state = NULL;
	}
}

/*
=====================
budActor::Event_SetState
=====================
*/
void budActor::Event_SetState( const char* name )
{
	idealState = GetScriptFunction( name );
	if( idealState == state )
	{
		state = NULL;
	}
	scriptThread->DoneProcessing();
}

/*
=====================
budActor::Event_GetState
=====================
*/
void budActor::Event_GetState()
{
	if( state )
	{
		idThread::ReturnString( state->Name() );
	}
	else
	{
		idThread::ReturnString( "" );
	}
}

/*
=====================
budActor::Event_GetHead
=====================
*/
void budActor::Event_GetHead()
{
	idThread::ReturnEntity( head.GetEntity() );
}

/*
================
budActor::Event_SetDamageGroupScale
================
*/
void budActor::Event_SetDamageGroupScale( const char* groupName, float scale )
{

	for( int i = 0; i < damageScale.Num(); i++ )
	{
		if( damageGroups[ i ] == groupName )
		{
			damageScale[ i ] = scale;
		}
	}
}

/*
================
budActor::Event_SetDamageGroupScaleAll
================
*/
void budActor::Event_SetDamageGroupScaleAll( float scale )
{

	for( int i = 0; i < damageScale.Num(); i++ )
	{
		damageScale[ i ] = scale;
	}
}

void budActor::Event_GetDamageGroupScale( const char* groupName )
{

	for( int i = 0; i < damageScale.Num(); i++ )
	{
		if( damageGroups[ i ] == groupName )
		{
			idThread::ReturnFloat( damageScale[i] );
			return;
		}
	}
	
	idThread::ReturnFloat( 0 );
}

void budActor::Event_SetDamageCap( float _damageCap )
{
	damageCap = _damageCap;
}

void budActor::Event_SetWaitState( const char* waitState )
{
	SetWaitState( waitState );
}

void budActor::Event_GetWaitState()
{
	if( WaitState() )
	{
		idThread::ReturnString( WaitState() );
	}
	else
	{
		idThread::ReturnString( "" );
	}
}
