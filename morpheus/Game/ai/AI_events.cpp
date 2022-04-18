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


#include "../Game_local.h"

/***********************************************************************

	AI Events

***********************************************************************/

const idEventDef AI_FindEnemy( "findEnemy", "d", 'e' );
const idEventDef AI_FindEnemyAI( "findEnemyAI", "d", 'e' );
const idEventDef AI_FindEnemyInCombatNodes( "findEnemyInCombatNodes", NULL, 'e' );
const idEventDef AI_ClosestReachableEnemyOfEntity( "closestReachableEnemyOfEntity", "E", 'e' );
const idEventDef AI_HeardSound( "heardSound", "d", 'e' );
const idEventDef AI_SetEnemy( "setEnemy", "E" );
const idEventDef AI_ClearEnemy( "clearEnemy" );
const idEventDef AI_MuzzleFlash( "muzzleFlash", "s" );
const idEventDef AI_CreateMissile( "createMissile", "s", 'e' );
const idEventDef AI_AttackMissile( "attackMissile", "s", 'e' );
const idEventDef AI_FireMissileAtTarget( "fireMissileAtTarget", "ss", 'e' );
const idEventDef AI_LaunchMissile( "launchMissile", "vv", 'e' );
const idEventDef AI_LaunchHomingMissile( "launchHomingMissile" );
const idEventDef AI_SetHomingMissileGoal( "setHomingMissileGoal" );
const idEventDef AI_LaunchProjectile( "launchProjectile", "s" );
const idEventDef AI_AttackMelee( "attackMelee", "s", 'd' );
const idEventDef AI_DirectDamage( "directDamage", "es" );
const idEventDef AI_RadiusDamageFromJoint( "radiusDamageFromJoint", "ss" );
const idEventDef AI_BeginAttack( "attackBegin", "s" );
const idEventDef AI_EndAttack( "attackEnd" );
const idEventDef AI_MeleeAttackToJoint( "meleeAttackToJoint", "ss", 'd' );
const idEventDef AI_RandomPath( "randomPath", NULL, 'e' );
const idEventDef AI_CanBecomeSolid( "canBecomeSolid", NULL, 'f' );
const idEventDef AI_BecomeSolid( "becomeSolid" );
const idEventDef AI_BecomeRagdoll( "becomeRagdoll", NULL, 'd' );
const idEventDef AI_StopRagdoll( "stopRagdoll" );
const idEventDef AI_SetHealth( "setHealth", "f" );
const idEventDef AI_GetHealth( "getHealth", NULL, 'f' );
const idEventDef AI_AllowDamage( "allowDamage" );
const idEventDef AI_IgnoreDamage( "ignoreDamage" );
const idEventDef AI_GetCurrentYaw( "getCurrentYaw", NULL, 'f' );
const idEventDef AI_TurnTo( "turnTo", "f" );
const idEventDef AI_TurnToPos( "turnToPos", "v" );
const idEventDef AI_TurnToEntity( "turnToEntity", "E" );
const idEventDef AI_MoveStatus( "moveStatus", NULL, 'd' );
const idEventDef AI_StopMove( "stopMove" );
const idEventDef AI_MoveToCover( "moveToCover" );
const idEventDef AI_MoveToEnemy( "moveToEnemy" );
const idEventDef AI_MoveToEnemyHeight( "moveToEnemyHeight" );
const idEventDef AI_MoveOutOfRange( "moveOutOfRange", "ef" );
const idEventDef AI_MoveToAttackPosition( "moveToAttackPosition", "es" );
const idEventDef AI_Wander( "wander" );
const idEventDef AI_MoveToEntity( "moveToEntity", "e" );
const idEventDef AI_MoveToPosition( "moveToPosition", "v" );
const idEventDef AI_SlideTo( "slideTo", "vf" );
const idEventDef AI_FacingIdeal( "facingIdeal", NULL, 'd' );
const idEventDef AI_FaceEnemy( "faceEnemy" );
const idEventDef AI_FaceEntity( "faceEntity", "E" );
const idEventDef AI_GetCombatNode( "getCombatNode", NULL, 'e' );
const idEventDef AI_EnemyInCombatCone( "enemyInCombatCone", "Ed", 'd' );
const idEventDef AI_WaitMove( "waitMove" );
const idEventDef AI_GetJumpVelocity( "getJumpVelocity", "vff", 'v' );
const idEventDef AI_GetTrajectoryToPlayer( "getTrajectoryToPlayer", NULL, 'v' );
const idEventDef AI_EntityInAttackCone( "entityInAttackCone", "E", 'd' );
const idEventDef AI_CanSeeEntity( "canSee", "E", 'd' );
const idEventDef AI_SetTalkTarget( "setTalkTarget", "E" );
const idEventDef AI_GetTalkTarget( "getTalkTarget", NULL, 'e' );
const idEventDef AI_SetTalkState( "setTalkState", "d" );
const idEventDef AI_EnemyRange( "enemyRange", NULL, 'f' );
const idEventDef AI_EnemyRange2D( "enemyRange2D", NULL, 'f' );
const idEventDef AI_GetEnemy( "getEnemy", NULL, 'e' );
const idEventDef AI_GetEnemyPos( "getEnemyPos", NULL, 'v' );
const idEventDef AI_GetEnemyEyePos( "getEnemyEyePos", NULL, 'v' );
const idEventDef AI_PredictEnemyPos( "predictEnemyPos", "f", 'v' );
const idEventDef AI_CanHitEnemy( "canHitEnemy", NULL, 'd' );
const idEventDef AI_CanHitEnemyFromAnim( "canHitEnemyFromAnim", "s", 'd' );
const idEventDef AI_CanHitEnemyFromJoint( "canHitEnemyFromJoint", "s", 'd' );
const idEventDef AI_EnemyPositionValid( "enemyPositionValid", NULL, 'd' );
const idEventDef AI_ChargeAttack( "chargeAttack", "s" );
const idEventDef AI_TestChargeAttack( "testChargeAttack", NULL, 'f' );
const idEventDef AI_TestMoveToPosition( "testMoveToPosition", "v", 'd' );
const idEventDef AI_TestAnimMoveTowardEnemy( "testAnimMoveTowardEnemy", "s", 'd' );
const idEventDef AI_TestAnimMove( "testAnimMove", "s", 'd' );
const idEventDef AI_TestMeleeAttack( "testMeleeAttack", NULL, 'd' );
const idEventDef AI_TestAnimAttack( "testAnimAttack", "s", 'd' );
const idEventDef AI_Burn( "burn" );
const idEventDef AI_ClearBurn( "clearBurn" );
const idEventDef AI_PreBurn( "preBurn" );
const idEventDef AI_SetSmokeVisibility( "setSmokeVisibility", "dd" );
const idEventDef AI_NumSmokeEmitters( "numSmokeEmitters", NULL, 'd' );
const idEventDef AI_WaitAction( "waitAction", "s" );
const idEventDef AI_StopThinking( "stopThinking" );
const idEventDef AI_GetTurnDelta( "getTurnDelta", NULL, 'f' );
const idEventDef AI_GetMoveType( "getMoveType", NULL, 'd' );
const idEventDef AI_SetMoveType( "setMoveType", "d" );
const idEventDef AI_SaveMove( "saveMove" );
const idEventDef AI_RestoreMove( "restoreMove" );
const idEventDef AI_AllowMovement( "allowMovement", "f" );
const idEventDef AI_JumpFrame( "<jumpframe>" );
const idEventDef AI_EnableClip( "enableClip" );
const idEventDef AI_DisableClip( "disableClip" );
const idEventDef AI_EnableGravity( "enableGravity" );
const idEventDef AI_DisableGravity( "disableGravity" );
const idEventDef AI_EnableAFPush( "enableAFPush" );
const idEventDef AI_DisableAFPush( "disableAFPush" );
const idEventDef AI_SetFlySpeed( "setFlySpeed", "f" );
const idEventDef AI_SetFlyOffset( "setFlyOffset", "d" );
const idEventDef AI_ClearFlyOffset( "clearFlyOffset" );
const idEventDef AI_GetClosestHiddenTarget( "getClosestHiddenTarget", "s", 'e' );
const idEventDef AI_GetRandomTarget( "getRandomTarget", "s", 'e' );
const idEventDef AI_TravelDistanceToPoint( "travelDistanceToPoint", "v", 'f' );
const idEventDef AI_TravelDistanceToEntity( "travelDistanceToEntity", "e", 'f' );
const idEventDef AI_TravelDistanceBetweenPoints( "travelDistanceBetweenPoints", "vv", 'f' );
const idEventDef AI_TravelDistanceBetweenEntities( "travelDistanceBetweenEntities", "ee", 'f' );
const idEventDef AI_LookAtEntity( "lookAt", "Ef" );
const idEventDef AI_LookAtEnemy( "lookAtEnemy", "f" );
const idEventDef AI_SetJointMod( "setBoneMod", "d" );
const idEventDef AI_ThrowMoveable( "throwMoveable" );
const idEventDef AI_ThrowAF( "throwAF" );
const idEventDef AI_RealKill( "<kill>" );
const idEventDef AI_Kill( "kill" );
const idEventDef AI_WakeOnFlashlight( "wakeOnFlashlight", "d" );
const idEventDef AI_LocateEnemy( "locateEnemy" );
const idEventDef AI_KickObstacles( "kickObstacles", "Ef" );
const idEventDef AI_GetObstacle( "getObstacle", NULL, 'e' );
const idEventDef AI_PushPointIntoAAS( "pushPointIntoAAS", "v", 'v' );
const idEventDef AI_GetTurnRate( "getTurnRate", NULL, 'f' );
const idEventDef AI_SetTurnRate( "setTurnRate", "f" );
const idEventDef AI_AnimTurn( "animTurn", "f" );
const idEventDef AI_AllowHiddenMovement( "allowHiddenMovement", "d" );
const idEventDef AI_TriggerParticles( "triggerParticles", "s" );
const idEventDef AI_FindActorsInBounds( "findActorsInBounds", "vv", 'e' );
const idEventDef AI_CanReachPosition( "canReachPosition", "v", 'd' );
const idEventDef AI_CanReachEntity( "canReachEntity", "E", 'd' );
const idEventDef AI_CanReachEnemy( "canReachEnemy", NULL, 'd' );
const idEventDef AI_GetReachableEntityPosition( "getReachableEntityPosition", "e", 'v' );
const idEventDef AI_MoveToPositionDirect( "moveToPositionDirect", "v" );
const idEventDef AI_AvoidObstacles( "avoidObstacles", "d" );
const idEventDef AI_TriggerFX( "triggerFX", "ss" );
const idEventDef AI_StartEmitter( "startEmitter", "sss", 'e' );
const idEventDef AI_GetEmitter( "getEmitter", "s", 'e' );
const idEventDef AI_StopEmitter( "stopEmitter", "s" );



CLASS_DECLARATION( budActor, budAI )
EVENT( EV_Activate,							budAI::Event_Activate )
EVENT( EV_Touch,							budAI::Event_Touch )
EVENT( AI_FindEnemy,						budAI::Event_FindEnemy )
EVENT( AI_FindEnemyAI,						budAI::Event_FindEnemyAI )
EVENT( AI_FindEnemyInCombatNodes,			budAI::Event_FindEnemyInCombatNodes )
EVENT( AI_ClosestReachableEnemyOfEntity,	budAI::Event_ClosestReachableEnemyOfEntity )
EVENT( AI_HeardSound,						budAI::Event_HeardSound )
EVENT( AI_SetEnemy,							budAI::Event_SetEnemy )
EVENT( AI_ClearEnemy,						budAI::Event_ClearEnemy )
EVENT( AI_MuzzleFlash,						budAI::Event_MuzzleFlash )
EVENT( AI_CreateMissile,					budAI::Event_CreateMissile )
EVENT( AI_AttackMissile,					budAI::Event_AttackMissile )
EVENT( AI_FireMissileAtTarget,				budAI::Event_FireMissileAtTarget )
EVENT( AI_LaunchMissile,					budAI::Event_LaunchMissile )
EVENT( AI_LaunchHomingMissile,				budAI::Event_LaunchHomingMissile )
EVENT( AI_SetHomingMissileGoal,				budAI::Event_SetHomingMissileGoal )
EVENT( AI_LaunchProjectile,					budAI::Event_LaunchProjectile )
EVENT( AI_AttackMelee,						budAI::Event_AttackMelee )
EVENT( AI_DirectDamage,						budAI::Event_DirectDamage )
EVENT( AI_RadiusDamageFromJoint,			budAI::Event_RadiusDamageFromJoint )
EVENT( AI_BeginAttack,						budAI::Event_BeginAttack )
EVENT( AI_EndAttack,						budAI::Event_EndAttack )
EVENT( AI_MeleeAttackToJoint,				budAI::Event_MeleeAttackToJoint )
EVENT( AI_RandomPath,						budAI::Event_RandomPath )
EVENT( AI_CanBecomeSolid,					budAI::Event_CanBecomeSolid )
EVENT( AI_BecomeSolid,						budAI::Event_BecomeSolid )
EVENT( EV_BecomeNonSolid,					budAI::Event_BecomeNonSolid )
EVENT( AI_BecomeRagdoll,					budAI::Event_BecomeRagdoll )
EVENT( AI_StopRagdoll,						budAI::Event_StopRagdoll )
EVENT( AI_SetHealth,						budAI::Event_SetHealth )
EVENT( AI_GetHealth,						budAI::Event_GetHealth )
EVENT( AI_AllowDamage,						budAI::Event_AllowDamage )
EVENT( AI_IgnoreDamage,						budAI::Event_IgnoreDamage )
EVENT( AI_GetCurrentYaw,					budAI::Event_GetCurrentYaw )
EVENT( AI_TurnTo,							budAI::Event_TurnTo )
EVENT( AI_TurnToPos,						budAI::Event_TurnToPos )
EVENT( AI_TurnToEntity,						budAI::Event_TurnToEntity )
EVENT( AI_MoveStatus,						budAI::Event_MoveStatus )
EVENT( AI_StopMove,							budAI::Event_StopMove )
EVENT( AI_MoveToCover,						budAI::Event_MoveToCover )
EVENT( AI_MoveToEnemy,						budAI::Event_MoveToEnemy )
EVENT( AI_MoveToEnemyHeight,				budAI::Event_MoveToEnemyHeight )
EVENT( AI_MoveOutOfRange,					budAI::Event_MoveOutOfRange )
EVENT( AI_MoveToAttackPosition,				budAI::Event_MoveToAttackPosition )
EVENT( AI_Wander,							budAI::Event_Wander )
EVENT( AI_MoveToEntity,						budAI::Event_MoveToEntity )
EVENT( AI_MoveToPosition,					budAI::Event_MoveToPosition )
EVENT( AI_SlideTo,							budAI::Event_SlideTo )
EVENT( AI_FacingIdeal,						budAI::Event_FacingIdeal )
EVENT( AI_FaceEnemy,						budAI::Event_FaceEnemy )
EVENT( AI_FaceEntity,						budAI::Event_FaceEntity )
EVENT( AI_WaitAction,						budAI::Event_WaitAction )
EVENT( AI_GetCombatNode,					budAI::Event_GetCombatNode )
EVENT( AI_EnemyInCombatCone,				budAI::Event_EnemyInCombatCone )
EVENT( AI_WaitMove,							budAI::Event_WaitMove )
EVENT( AI_GetJumpVelocity,					budAI::Event_GetJumpVelocity )
EVENT( AI_GetTrajectoryToPlayer,			budAI::Event_GetTrajectoryToPlayer )
EVENT( AI_EntityInAttackCone,				budAI::Event_EntityInAttackCone )
EVENT( AI_CanSeeEntity,						budAI::Event_CanSeeEntity )
EVENT( AI_SetTalkTarget,					budAI::Event_SetTalkTarget )
EVENT( AI_GetTalkTarget,					budAI::Event_GetTalkTarget )
EVENT( AI_SetTalkState,						budAI::Event_SetTalkState )
EVENT( AI_EnemyRange,						budAI::Event_EnemyRange )
EVENT( AI_EnemyRange2D,						budAI::Event_EnemyRange2D )
EVENT( AI_GetEnemy,							budAI::Event_GetEnemy )
EVENT( AI_GetEnemyPos,						budAI::Event_GetEnemyPos )
EVENT( AI_GetEnemyEyePos,					budAI::Event_GetEnemyEyePos )
EVENT( AI_PredictEnemyPos,					budAI::Event_PredictEnemyPos )
EVENT( AI_CanHitEnemy,						budAI::Event_CanHitEnemy )
EVENT( AI_CanHitEnemyFromAnim,				budAI::Event_CanHitEnemyFromAnim )
EVENT( AI_CanHitEnemyFromJoint,				budAI::Event_CanHitEnemyFromJoint )
EVENT( AI_EnemyPositionValid,				budAI::Event_EnemyPositionValid )
EVENT( AI_ChargeAttack,						budAI::Event_ChargeAttack )
EVENT( AI_TestChargeAttack,					budAI::Event_TestChargeAttack )
EVENT( AI_TestAnimMoveTowardEnemy,			budAI::Event_TestAnimMoveTowardEnemy )
EVENT( AI_TestAnimMove,						budAI::Event_TestAnimMove )
EVENT( AI_TestMoveToPosition,				budAI::Event_TestMoveToPosition )
EVENT( AI_TestMeleeAttack,					budAI::Event_TestMeleeAttack )
EVENT( AI_TestAnimAttack,					budAI::Event_TestAnimAttack )
EVENT( AI_Burn,								budAI::Event_Burn )
EVENT( AI_PreBurn,							budAI::Event_PreBurn )
EVENT( AI_SetSmokeVisibility,				budAI::Event_SetSmokeVisibility )
EVENT( AI_NumSmokeEmitters,					budAI::Event_NumSmokeEmitters )
EVENT( AI_ClearBurn,						budAI::Event_ClearBurn )
EVENT( AI_StopThinking,						budAI::Event_StopThinking )
EVENT( AI_GetTurnDelta,						budAI::Event_GetTurnDelta )
EVENT( AI_GetMoveType,						budAI::Event_GetMoveType )
EVENT( AI_SetMoveType,						budAI::Event_SetMoveType )
EVENT( AI_SaveMove,							budAI::Event_SaveMove )
EVENT( AI_RestoreMove,						budAI::Event_RestoreMove )
EVENT( AI_AllowMovement,					budAI::Event_AllowMovement )
EVENT( AI_JumpFrame,						budAI::Event_JumpFrame )
EVENT( AI_EnableClip,						budAI::Event_EnableClip )
EVENT( AI_DisableClip,						budAI::Event_DisableClip )
EVENT( AI_EnableGravity,					budAI::Event_EnableGravity )
EVENT( AI_DisableGravity,					budAI::Event_DisableGravity )
EVENT( AI_EnableAFPush,						budAI::Event_EnableAFPush )
EVENT( AI_DisableAFPush,					budAI::Event_DisableAFPush )
EVENT( AI_SetFlySpeed,						budAI::Event_SetFlySpeed )
EVENT( AI_SetFlyOffset,						budAI::Event_SetFlyOffset )
EVENT( AI_ClearFlyOffset,					budAI::Event_ClearFlyOffset )
EVENT( AI_GetClosestHiddenTarget,			budAI::Event_GetClosestHiddenTarget )
EVENT( AI_GetRandomTarget,					budAI::Event_GetRandomTarget )
EVENT( AI_TravelDistanceToPoint,			budAI::Event_TravelDistanceToPoint )
EVENT( AI_TravelDistanceToEntity,			budAI::Event_TravelDistanceToEntity )
EVENT( AI_TravelDistanceBetweenPoints,		budAI::Event_TravelDistanceBetweenPoints )
EVENT( AI_TravelDistanceBetweenEntities,	budAI::Event_TravelDistanceBetweenEntities )
EVENT( AI_LookAtEntity,						budAI::Event_LookAtEntity )
EVENT( AI_LookAtEnemy,						budAI::Event_LookAtEnemy )
EVENT( AI_SetJointMod,						budAI::Event_SetJointMod )
EVENT( AI_ThrowMoveable,					budAI::Event_ThrowMoveable )
EVENT( AI_ThrowAF,							budAI::Event_ThrowAF )
EVENT( EV_GetAngles,						budAI::Event_GetAngles )
EVENT( EV_SetAngles,						budAI::Event_SetAngles )
EVENT( AI_RealKill,							budAI::Event_RealKill )
EVENT( AI_Kill,								budAI::Event_Kill )
EVENT( AI_WakeOnFlashlight,					budAI::Event_WakeOnFlashlight )
EVENT( AI_LocateEnemy,						budAI::Event_LocateEnemy )
EVENT( AI_KickObstacles,					budAI::Event_KickObstacles )
EVENT( AI_GetObstacle,						budAI::Event_GetObstacle )
EVENT( AI_PushPointIntoAAS,					budAI::Event_PushPointIntoAAS )
EVENT( AI_GetTurnRate,						budAI::Event_GetTurnRate )
EVENT( AI_SetTurnRate,						budAI::Event_SetTurnRate )
EVENT( AI_AnimTurn,							budAI::Event_AnimTurn )
EVENT( AI_AllowHiddenMovement,				budAI::Event_AllowHiddenMovement )
EVENT( AI_TriggerParticles,					budAI::Event_TriggerParticles )
EVENT( AI_FindActorsInBounds,				budAI::Event_FindActorsInBounds )
EVENT( AI_CanReachPosition,					budAI::Event_CanReachPosition )
EVENT( AI_CanReachEntity,					budAI::Event_CanReachEntity )
EVENT( AI_CanReachEnemy,					budAI::Event_CanReachEnemy )
EVENT( AI_GetReachableEntityPosition,		budAI::Event_GetReachableEntityPosition )
EVENT( AI_MoveToPositionDirect,				budAI::Event_MoveToPositionDirect )
EVENT( AI_AvoidObstacles, 					budAI::Event_AvoidObstacles )
EVENT( AI_TriggerFX,						budAI::Event_TriggerFX )
EVENT( AI_StartEmitter,						budAI::Event_StartEmitter )
EVENT( AI_GetEmitter,						budAI::Event_GetEmitter )
EVENT( AI_StopEmitter,						budAI::Event_StopEmitter )
END_CLASS

/*
=====================
budAI::Event_Activate
=====================
*/
void budAI::Event_Activate( idEntity* activator )
{
	Activate( activator );
}

/*
=====================
budAI::Event_Touch
=====================
*/
void budAI::Event_Touch( idEntity* other, trace_t* trace )
{
	if( !enemy.GetEntity() && !other->fl.notarget && ( ReactionTo( other ) & ATTACK_ON_ACTIVATE ) )
	{
		Activate( other );
	}
	AI_PUSHED = true;
}

/*
=====================
budAI::Event_FindEnemy
=====================
*/
void budAI::Event_FindEnemy( int useFOV )
{
	int			i;
	idEntity*	ent;
	budActor*		actor;
	
	if( gameLocal.InPlayerPVS( this ) )
	{
		for( i = 0; i < gameLocal.numClients ; i++ )
		{
			ent = gameLocal.entities[ i ];
			
			if( !ent || !ent->IsType( budActor::Type ) )
			{
				continue;
			}
			
			actor = static_cast<budActor*>( ent );
			if( ( actor->health <= 0 ) || !( ReactionTo( actor ) & ATTACK_ON_SIGHT ) )
			{
				continue;
			}
			
			if( CanSee( actor, useFOV != 0 ) )
			{
				idThread::ReturnEntity( actor );
				return;
			}
		}
	}
	
	idThread::ReturnEntity( NULL );
}

/*
=====================
budAI::Event_FindEnemyAI
=====================
*/
void budAI::Event_FindEnemyAI( int useFOV )
{
	idEntity*	ent;
	budActor*		actor;
	budActor*		bestEnemy;
	float		bestDist;
	float		dist;
	Vector3		delta;
	pvsHandle_t pvs;
	
	pvs = gameLocal.pvs.SetupCurrentPVS( GetPVSAreas(), GetNumPVSAreas() );
	
	bestDist = Math::INFINITY;
	bestEnemy = NULL;
	for( ent = gameLocal.activeEntities.Next(); ent != NULL; ent = ent->activeNode.Next() )
	{
		if( ent->fl.hidden || ent->fl.isDormant || !ent->IsType( budActor::Type ) )
		{
			continue;
		}
		
		actor = static_cast<budActor*>( ent );
		if( ( actor->health <= 0 ) || !( ReactionTo( actor ) & ATTACK_ON_SIGHT ) )
		{
			continue;
		}
		
		if( !gameLocal.pvs.InCurrentPVS( pvs, actor->GetPVSAreas(), actor->GetNumPVSAreas() ) )
		{
			continue;
		}
		
		delta = physicsObj.GetOrigin() - actor->GetPhysics()->GetOrigin();
		dist = delta.LengthSqr();
		if( ( dist < bestDist ) && CanSee( actor, useFOV != 0 ) )
		{
			bestDist = dist;
			bestEnemy = actor;
		}
	}
	
	gameLocal.pvs.FreeCurrentPVS( pvs );
	idThread::ReturnEntity( bestEnemy );
}

/*
=====================
budAI::Event_FindEnemyInCombatNodes
=====================
*/
void budAI::Event_FindEnemyInCombatNodes()
{
	int				i, j;
	idCombatNode*	node;
	idEntity*		ent;
	idEntity*		targetEnt;
	budActor*			actor;
	
	if( !gameLocal.InPlayerPVS( this ) )
	{
		// don't locate the player when we're not in his PVS
		idThread::ReturnEntity( NULL );
		return;
	}
	
	for( i = 0; i < gameLocal.numClients ; i++ )
	{
		ent = gameLocal.entities[ i ];
		
		if( !ent || !ent->IsType( budActor::Type ) )
		{
			continue;
		}
		
		actor = static_cast<budActor*>( ent );
		if( ( actor->health <= 0 ) || !( ReactionTo( actor ) & ATTACK_ON_SIGHT ) )
		{
			continue;
		}
		
		for( j = 0; j < targets.Num(); j++ )
		{
			targetEnt = targets[ j ].GetEntity();
			if( !targetEnt || !targetEnt->IsType( idCombatNode::Type ) )
			{
				continue;
			}
			
			node = static_cast<idCombatNode*>( targetEnt );
			if( !node->IsDisabled() && node->EntityInView( actor, actor->GetPhysics()->GetOrigin() ) )
			{
				idThread::ReturnEntity( actor );
				return;
			}
		}
	}
	
	idThread::ReturnEntity( NULL );
}

/*
=====================
budAI::Event_ClosestReachableEnemyOfEntity
=====================
*/
void budAI::Event_ClosestReachableEnemyOfEntity( idEntity* team_mate )
{
	budActor* actor;
	budActor* ent;
	budActor*	bestEnt;
	float	bestDistSquared;
	float	distSquared;
	Vector3	delta;
	int		areaNum;
	int		enemyAreaNum;
	aasPath_t path;
	
	if( !team_mate->IsType( budActor::Type ) )
	{
		gameLocal.Error( "Entity '%s' is not an AI character or player", team_mate->GetName() );
	}
	
	actor = static_cast<budActor*>( team_mate );
	
	const Vector3& origin = physicsObj.GetOrigin();
	areaNum = PointReachableAreaNum( origin );
	
	bestDistSquared = Math::INFINITY;
	bestEnt = NULL;
	for( ent = actor->enemyList.Next(); ent != NULL; ent = ent->enemyNode.Next() )
	{
		if( ent->fl.hidden )
		{
			continue;
		}
		delta = ent->GetPhysics()->GetOrigin() - origin;
		distSquared = delta.LengthSqr();
		if( distSquared < bestDistSquared )
		{
			const Vector3& enemyPos = ent->GetPhysics()->GetOrigin();
			enemyAreaNum = PointReachableAreaNum( enemyPos );
			if( ( areaNum != 0 ) && PathToGoal( path, areaNum, origin, enemyAreaNum, enemyPos ) )
			{
				bestEnt = ent;
				bestDistSquared = distSquared;
			}
		}
	}
	
	idThread::ReturnEntity( bestEnt );
}

/*
=====================
budAI::Event_HeardSound
=====================
*/
void budAI::Event_HeardSound( int ignore_team )
{
	// check if we heard any sounds in the last frame
	budActor*	actor = gameLocal.GetAlertEntity();
	if( actor != NULL && ( !ignore_team || ( ReactionTo( actor ) & ATTACK_ON_SIGHT ) ) && gameLocal.InPlayerPVS( this ) )
	{
		Vector3 pos = actor->GetPhysics()->GetOrigin();
		Vector3 org = physicsObj.GetOrigin();
		float dist = ( pos - org ).LengthSqr();
		if( dist < Square( AI_HEARING_RANGE ) )
		{
			idThread::ReturnEntity( actor );
			return;
		}
	}
	
	idThread::ReturnEntity( NULL );
}

/*
=====================
budAI::Event_SetEnemy
=====================
*/
void budAI::Event_SetEnemy( idEntity* ent )
{
	if( !ent )
	{
		ClearEnemy();
	}
	else if( !ent->IsType( budActor::Type ) )
	{
		gameLocal.Error( "'%s' is not an budActor (player or ai controlled character)", ent->name.c_str() );
	}
	else
	{
		SetEnemy( static_cast<budActor*>( ent ) );
	}
}

/*
=====================
budAI::Event_ClearEnemy
=====================
*/
void budAI::Event_ClearEnemy()
{
	ClearEnemy();
}

/*
=====================
budAI::Event_MuzzleFlash
=====================
*/
void budAI::Event_MuzzleFlash( const char* jointname )
{
	Vector3	muzzle;
	Matrix3	axis;
	
	GetMuzzle( jointname, muzzle, axis );
	TriggerWeaponEffects( muzzle );
}

/*
=====================
budAI::Event_CreateMissile
=====================
*/
void budAI::Event_CreateMissile( const char* jointname )
{
	Vector3 muzzle;
	Matrix3 axis;
	
	if( !projectileDef )
	{
		gameLocal.Warning( "%s (%s) doesn't have a projectile specified", name.c_str(), GetEntityDefName() );
		return idThread::ReturnEntity( NULL );
	}
	
	GetMuzzle( jointname, muzzle, axis );
	CreateProjectile( muzzle, viewAxis[ 0 ] * physicsObj.GetGravityAxis() );
	if( projectile.GetEntity() )
	{
		if( !jointname || !jointname[ 0 ] )
		{
			projectile.GetEntity()->Bind( this, true );
		}
		else
		{
			projectile.GetEntity()->BindToJoint( this, jointname, true );
		}
	}
	idThread::ReturnEntity( projectile.GetEntity() );
}

/*
=====================
budAI::Event_AttackMissile
=====================
*/
void budAI::Event_AttackMissile( const char* jointname )
{
	idProjectile* proj;
	
	proj = LaunchProjectile( jointname, enemy.GetEntity(), true );
	idThread::ReturnEntity( proj );
}

/*
=====================
budAI::Event_FireMissileAtTarget
=====================
*/
void budAI::Event_FireMissileAtTarget( const char* jointname, const char* targetname )
{
	idEntity*		aent;
	idProjectile*	proj;
	
	aent = gameLocal.FindEntity( targetname );
	if( !aent )
	{
		gameLocal.Warning( "Entity '%s' not found for 'fireMissileAtTarget'", targetname );
	}
	
	proj = LaunchProjectile( jointname, aent, false );
	idThread::ReturnEntity( proj );
}

/*
=====================
budAI::Event_LaunchMissile
=====================
*/
void budAI::Event_LaunchMissile( const Vector3& org, const Angles& ang )
{
	Vector3		start;
	trace_t		tr;
	budBounds	projBounds;
	const budClipModel* projClip;
	Matrix3		axis;
	float		distance;
	
	if( !projectileDef )
	{
		gameLocal.Warning( "%s (%s) doesn't have a projectile specified", name.c_str(), GetEntityDefName() );
		idThread::ReturnEntity( NULL );
		return;
	}
	
	axis = ang.ToMat3();
	if( !projectile.GetEntity() )
	{
		CreateProjectile( org, axis[ 0 ] );
	}
	
	// make sure the projectile starts inside the monster bounding box
	const budBounds& ownerBounds = physicsObj.GetAbsBounds();
	projClip = projectile.GetEntity()->GetPhysics()->GetClipModel();
	projBounds = projClip->GetBounds().Rotate( projClip->GetAxis() );
	
	// check if the owner bounds is bigger than the projectile bounds
	if( ( ( ownerBounds[1][0] - ownerBounds[0][0] ) > ( projBounds[1][0] - projBounds[0][0] ) ) &&
			( ( ownerBounds[1][1] - ownerBounds[0][1] ) > ( projBounds[1][1] - projBounds[0][1] ) ) &&
			( ( ownerBounds[1][2] - ownerBounds[0][2] ) > ( projBounds[1][2] - projBounds[0][2] ) ) )
	{
		if( ( ownerBounds - projBounds ).RayIntersection( org, viewAxis[ 0 ], distance ) )
		{
			start = org + distance * viewAxis[ 0 ];
		}
		else
		{
			start = ownerBounds.GetCenter();
		}
	}
	else
	{
		// projectile bounds bigger than the owner bounds, so just start it from the center
		start = ownerBounds.GetCenter();
	}
	
	gameLocal.clip.Translation( tr, start, org, projClip, projClip->GetAxis(), MASK_SHOT_RENDERMODEL, this );
	
	// launch the projectile
	idThread::ReturnEntity( projectile.GetEntity() );
	projectile.GetEntity()->Launch( tr.endpos, axis[ 0 ], Vector3_Origin );
	projectile = NULL;
	
	TriggerWeaponEffects( tr.endpos );
	
	lastAttackTime = gameLocal.time;
}


/*
=====================
budAI::Event_LaunchProjectile
=====================
*/
void budAI::Event_LaunchProjectile( const char* entityDefName )
{
	Vector3				muzzle, start, dir;
	const Dict*		projDef;
	Matrix3				axis;
	const budClipModel*	projClip;
	budBounds			projBounds;
	trace_t				tr;
	idEntity*			ent;
	const char*			clsname;
	float				distance;
	idProjectile*		proj = NULL;
	
	projDef = gameLocal.FindEntityDefDict( entityDefName );
	
	gameLocal.SpawnEntityDef( *projDef, &ent, false );
	if( ent == NULL )
	{
		clsname = projectileDef->GetString( "classname" );
		gameLocal.Error( "Could not spawn entityDef '%s'", clsname );
		return;
	}
	
	if( !ent->IsType( idProjectile::Type ) )
	{
		clsname = ent->GetClassname();
		gameLocal.Error( "'%s' is not an idProjectile", clsname );
	}
	proj = ( idProjectile* )ent;
	
	GetMuzzle( "pistol", muzzle, axis );
	proj->Create( this, muzzle, axis[0] );
	
	// make sure the projectile starts inside the monster bounding box
	const budBounds& ownerBounds = physicsObj.GetAbsBounds();
	projClip = proj->GetPhysics()->GetClipModel();
	projBounds = projClip->GetBounds().Rotate( projClip->GetAxis() );
	if( ( ownerBounds - projBounds ).RayIntersection( muzzle, viewAxis[ 0 ], distance ) )
	{
		start = muzzle + distance * viewAxis[ 0 ];
	}
	else
	{
		start = ownerBounds.GetCenter();
	}
	gameLocal.clip.Translation( tr, start, muzzle, projClip, projClip->GetAxis(), MASK_SHOT_RENDERMODEL, this );
	muzzle = tr.endpos;
	
	GetAimDir( muzzle, enemy.GetEntity(), this, dir );
	
	proj->Launch( muzzle, dir, Vector3_Origin );
	
	TriggerWeaponEffects( muzzle );
}



/*
=====================
budAI::Event_AttackMelee
=====================
*/
void budAI::Event_AttackMelee( const char* meleeDefName )
{
	bool hit;
	
	hit = AttackMelee( meleeDefName );
	idThread::ReturnInt( hit );
}

/*
=====================
budAI::Event_DirectDamage
=====================
*/
void budAI::Event_DirectDamage( idEntity* damageTarget, const char* damageDefName )
{
	DirectDamage( damageDefName, damageTarget );
}

/*
=====================
budAI::Event_RadiusDamageFromJoint
=====================
*/
void budAI::Event_RadiusDamageFromJoint( const char* jointname, const char* damageDefName )
{
	jointHandle_t joint;
	Vector3 org;
	Matrix3 axis;
	
	if( !jointname || !jointname[ 0 ] )
	{
		org = physicsObj.GetOrigin();
	}
	else
	{
		joint = animator.GetJointHandle( jointname );
		if( joint == INVALID_JOINT )
		{
			gameLocal.Error( "Unknown joint '%s' on %s", jointname, GetEntityDefName() );
		}
		GetJointWorldTransform( joint, gameLocal.time, org, axis );
	}
	
	gameLocal.RadiusDamage( org, this, this, this, this, damageDefName );
}

/*
=====================
budAI::Event_RandomPath
=====================
*/
void budAI::Event_RandomPath()
{
	idPathCorner* path;
	
	path = idPathCorner::RandomPath( this, NULL );
	idThread::ReturnEntity( path );
}

/*
=====================
budAI::Event_BeginAttack
=====================
*/
void budAI::Event_BeginAttack( const char* name )
{
	BeginAttack( name );
}

/*
=====================
budAI::Event_EndAttack
=====================
*/
void budAI::Event_EndAttack()
{
	EndAttack();
}

/*
=====================
budAI::Event_MeleeAttackToJoint
=====================
*/
void budAI::Event_MeleeAttackToJoint( const char* jointname, const char* meleeDefName )
{
	jointHandle_t	joint;
	Vector3			start;
	Vector3			end;
	Matrix3			axis;
	trace_t			trace;
	idEntity*		hitEnt;
	
	joint = animator.GetJointHandle( jointname );
	if( joint == INVALID_JOINT )
	{
		gameLocal.Error( "Unknown joint '%s' on %s", jointname, GetEntityDefName() );
	}
	animator.GetJointTransform( joint, gameLocal.time, end, axis );
	end = physicsObj.GetOrigin() + ( end + modelOffset ) * viewAxis * physicsObj.GetGravityAxis();
	start = GetEyePosition();
	
	if( ai_debugMove.GetBool() )
	{
		gameRenderWorld->DebugLine( colorYellow, start, end, 1 );
	}
	
	gameLocal.clip.TranslationEntities( trace, start, end, NULL, mat3_identity, MASK_SHOT_BOUNDINGBOX, this );
	if( trace.fraction < 1.0f )
	{
		hitEnt = gameLocal.GetTraceEntity( trace );
		if( hitEnt != NULL && hitEnt->IsType( budActor::Type ) )
		{
			DirectDamage( meleeDefName, hitEnt );
			idThread::ReturnInt( true );
			return;
		}
	}
	
	idThread::ReturnInt( false );
}

/*
=====================
budAI::Event_CanBecomeSolid
=====================
*/
void budAI::Event_CanBecomeSolid()
{
	int			i;
	int			num;
	bool		returnValue = true;
	idEntity* 	hit;
	budClipModel* cm;
	budClipModel* clipModels[ MAX_GENTITIES ];
	
	num = gameLocal.clip.ClipModelsTouchingBounds( physicsObj.GetAbsBounds(), MASK_MONSTERSOLID, clipModels, MAX_GENTITIES );
	for( i = 0; i < num; i++ )
	{
		cm = clipModels[ i ];
		
		// don't check render entities
		if( cm->IsRenderModel() )
		{
			continue;
		}
		
		hit = cm->GetEntity();
		if( ( hit == this ) || !hit->fl.takedamage )
		{
			continue;
		}
		
		// DG: add parenthesis to make precedence obvious and to appease compiler
		if( ( spawnClearMoveables && hit->IsType( idMoveable::Type ) ) || hit->IsType( idBarrel::Type ) || hit->IsType( idExplodingBarrel::Type ) )
		{
			Vector3 push;
			push = hit->GetPhysics()->GetOrigin() - GetPhysics()->GetOrigin();
			push.z = 30.f;
			push.NormalizeFast();
			if( ( Math::Fabs( push.x ) < 0.15f ) && ( Math::Fabs( push.y ) < 0.15f ) )
			{
				push.x = 10.f;
				push.y = 10.f;
				push.z = 15.f;
				push.NormalizeFast();
			}
			push *= 300.f;
			hit->GetPhysics()->SetLinearVelocity( push );
		}
		
		if( physicsObj.ClipContents( cm ) )
		{
			returnValue = false;
		}
	}
	
	idThread::ReturnFloat( returnValue );
}

/*
=====================
budAI::Event_BecomeSolid
=====================
*/
void budAI::Event_BecomeSolid()
{
	physicsObj.EnableClip();
	if( spawnArgs.GetBool( "big_monster" ) )
	{
		physicsObj.SetContents( 0 );
	}
	else if( use_combat_bbox )
	{
		physicsObj.SetContents( CONTENTS_BODY | CONTENTS_SOLID );
	}
	else
	{
		physicsObj.SetContents( CONTENTS_BODY );
	}
	physicsObj.GetClipModel()->Link( gameLocal.clip );
	fl.takedamage = !spawnArgs.GetBool( "noDamage" );
}

/*
=====================
budAI::Event_BecomeNonSolid
=====================
*/
void budAI::Event_BecomeNonSolid()
{
	fl.takedamage = false;
	physicsObj.SetContents( 0 );
	physicsObj.GetClipModel()->Unlink();
}

/*
=====================
budAI::Event_BecomeRagdoll
=====================
*/
void budAI::Event_BecomeRagdoll()
{
	bool result;
	
	result = StartRagdoll();
	idThread::ReturnInt( result );
}

/*
=====================
budAI::Event_StopRagdoll
=====================
*/
void budAI::Event_StopRagdoll()
{
	StopRagdoll();
	
	// set back the monster physics
	SetPhysics( &physicsObj );
}

/*
=====================
budAI::Event_SetHealth
=====================
*/
void budAI::Event_SetHealth( float newHealth )
{
	health = newHealth;
	fl.takedamage = true;
	if( health > 0 )
	{
		AI_DEAD = false;
	}
	else
	{
		AI_DEAD = true;
	}
}

/*
=====================
budAI::Event_GetHealth
=====================
*/
void budAI::Event_GetHealth()
{
	idThread::ReturnFloat( health );
}

/*
=====================
budAI::Event_AllowDamage
=====================
*/
void budAI::Event_AllowDamage()
{
	fl.takedamage = true;
}

/*
=====================
budAI::Event_IgnoreDamage
=====================
*/
void budAI::Event_IgnoreDamage()
{
	fl.takedamage = false;
}

/*
=====================
budAI::Event_GetCurrentYaw
=====================
*/
void budAI::Event_GetCurrentYaw()
{
	idThread::ReturnFloat( current_yaw );
}

/*
=====================
budAI::Event_TurnTo
=====================
*/
void budAI::Event_TurnTo( float angle )
{
	TurnToward( angle );
}

/*
=====================
budAI::Event_TurnToPos
=====================
*/
void budAI::Event_TurnToPos( const Vector3& pos )
{
	TurnToward( pos );
}

/*
=====================
budAI::Event_TurnToEntity
=====================
*/
void budAI::Event_TurnToEntity( idEntity* ent )
{
	if( ent )
	{
		TurnToward( ent->GetPhysics()->GetOrigin() );
	}
}

/*
=====================
budAI::Event_MoveStatus
=====================
*/
void budAI::Event_MoveStatus()
{
	idThread::ReturnInt( move.moveStatus );
}

/*
=====================
budAI::Event_StopMove
=====================
*/
void budAI::Event_StopMove()
{
	StopMove( MOVE_STATUS_DONE );
}

/*
=====================
budAI::Event_MoveToCover
=====================
*/
void budAI::Event_MoveToCover()
{
	budActor* enemyEnt = enemy.GetEntity();
	
	StopMove( MOVE_STATUS_DEST_NOT_FOUND );
	if( !enemyEnt || !MoveToCover( enemyEnt, lastVisibleEnemyPos ) )
	{
		return;
	}
}

/*
=====================
budAI::Event_MoveToEnemy
=====================
*/
void budAI::Event_MoveToEnemy()
{
	StopMove( MOVE_STATUS_DEST_NOT_FOUND );
	if( !enemy.GetEntity() || !MoveToEnemy() )
	{
		return;
	}
}

/*
=====================
budAI::Event_MoveToEnemyHeight
=====================
*/
void budAI::Event_MoveToEnemyHeight()
{
	StopMove( MOVE_STATUS_DEST_NOT_FOUND );
	MoveToEnemyHeight();
}

/*
=====================
budAI::Event_MoveOutOfRange
=====================
*/
void budAI::Event_MoveOutOfRange( idEntity* entity, float range )
{
	StopMove( MOVE_STATUS_DEST_NOT_FOUND );
	MoveOutOfRange( entity, range );
}

/*
=====================
budAI::Event_MoveToAttackPosition
=====================
*/
void budAI::Event_MoveToAttackPosition( idEntity* entity, const char* attack_anim )
{
	int anim;
	
	StopMove( MOVE_STATUS_DEST_NOT_FOUND );
	
	anim = GetAnim( ANIMCHANNEL_LEGS, attack_anim );
	if( !anim )
	{
		gameLocal.Error( "Unknown anim '%s'", attack_anim );
	}
	
	MoveToAttackPosition( entity, anim );
}

/*
=====================
budAI::Event_MoveToEntity
=====================
*/
void budAI::Event_MoveToEntity( idEntity* ent )
{
	StopMove( MOVE_STATUS_DEST_NOT_FOUND );
	if( ent )
	{
		MoveToEntity( ent );
	}
}

/*
=====================
budAI::Event_MoveToPosition
=====================
*/
void budAI::Event_MoveToPosition( const Vector3& pos )
{
	StopMove( MOVE_STATUS_DONE );
	MoveToPosition( pos );
}

/*
=====================
budAI::Event_SlideTo
=====================
*/
void budAI::Event_SlideTo( const Vector3& pos, float time )
{
	SlideToPosition( pos, time );
}
/*
=====================
budAI::Event_Wander
=====================
*/
void budAI::Event_Wander()
{
	WanderAround();
}

/*
=====================
budAI::Event_FacingIdeal
=====================
*/
void budAI::Event_FacingIdeal()
{
	bool facing = FacingIdeal();
	idThread::ReturnInt( facing );
}

/*
=====================
budAI::Event_FaceEnemy
=====================
*/
void budAI::Event_FaceEnemy()
{
	FaceEnemy();
}

/*
=====================
budAI::Event_FaceEntity
=====================
*/
void budAI::Event_FaceEntity( idEntity* ent )
{
	FaceEntity( ent );
}

/*
=====================
budAI::Event_WaitAction
=====================
*/
void budAI::Event_WaitAction( const char* waitForState )
{
	if( idThread::BeginMultiFrameEvent( this, &AI_WaitAction ) )
	{
		SetWaitState( waitForState );
	}
	
	if( !WaitState() )
	{
		idThread::EndMultiFrameEvent( this, &AI_WaitAction );
	}
}

/*
=====================
budAI::Event_GetCombatNode
=====================
*/
void budAI::Event_GetCombatNode()
{
	int				i;
	float			dist;
	idEntity*		targetEnt;
	idCombatNode*	node;
	float			bestDist;
	idCombatNode*	bestNode;
	budActor*			enemyEnt = enemy.GetEntity();
	
	if( !targets.Num() )
	{
		// no combat nodes
		idThread::ReturnEntity( NULL );
		return;
	}
	
	if( !enemyEnt || !EnemyPositionValid() )
	{
		// don't return a combat node if we don't have an enemy or
		// if we can see he's not in the last place we saw him
		
		if( team == 0 )
		{
			// find the closest attack node to the player
			bestNode = NULL;
			const Vector3& myPos = physicsObj.GetOrigin();
			const Vector3& playerPos = gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin();
			
			bestDist = ( myPos - playerPos ).LengthSqr();
			
			for( i = 0; i < targets.Num(); i++ )
			{
				targetEnt = targets[ i ].GetEntity();
				if( !targetEnt || !targetEnt->IsType( idCombatNode::Type ) )
				{
					continue;
				}
				
				node = static_cast<idCombatNode*>( targetEnt );
				if( !node->IsDisabled() )
				{
					Vector3 org = node->GetPhysics()->GetOrigin();
					dist = ( playerPos - org ).LengthSqr();
					if( dist < bestDist )
					{
						bestNode = node;
						bestDist = dist;
					}
				}
			}
			
			idThread::ReturnEntity( bestNode );
			return;
		}
		
		idThread::ReturnEntity( NULL );
		return;
	}
	
	// find the closest attack node that can see our enemy and is closer than our enemy
	bestNode = NULL;
	const Vector3& myPos = physicsObj.GetOrigin();
	bestDist = ( myPos - lastVisibleEnemyPos ).LengthSqr();
	for( i = 0; i < targets.Num(); i++ )
	{
		targetEnt = targets[ i ].GetEntity();
		if( !targetEnt || !targetEnt->IsType( idCombatNode::Type ) )
		{
			continue;
		}
		
		node = static_cast<idCombatNode*>( targetEnt );
		if( !node->IsDisabled() && node->EntityInView( enemyEnt, lastVisibleEnemyPos ) )
		{
			Vector3 org = node->GetPhysics()->GetOrigin();
			dist = ( myPos - org ).LengthSqr();
			if( dist < bestDist )
			{
				bestNode = node;
				bestDist = dist;
			}
		}
	}
	
	idThread::ReturnEntity( bestNode );
}

/*
=====================
budAI::Event_EnemyInCombatCone
=====================
*/
void budAI::Event_EnemyInCombatCone( idEntity* ent, int use_current_enemy_location )
{
	idCombatNode*	node;
	bool			result;
	budActor*			enemyEnt = enemy.GetEntity();
	
	if( !targets.Num() )
	{
		// no combat nodes
		idThread::ReturnInt( false );
		return;
	}
	
	if( !enemyEnt )
	{
		// have to have an enemy
		idThread::ReturnInt( false );
		return;
	}
	
	if( !ent || !ent->IsType( idCombatNode::Type ) )
	{
		// not a combat node
		idThread::ReturnInt( false );
		return;
	}
	
	//Allow the level designers define attack nodes that the enemy should never leave.
	//This is different that the turrent type combat nodes because they can play an animation
	if( ent->spawnArgs.GetBool( "neverLeave", "0" ) )
	{
		idThread::ReturnInt( true );
		return;
	}
	
	node = static_cast<idCombatNode*>( ent );
	if( use_current_enemy_location )
	{
		const Vector3& pos = enemyEnt->GetPhysics()->GetOrigin();
		result = node->EntityInView( enemyEnt, pos );
	}
	else
	{
		result = node->EntityInView( enemyEnt, lastVisibleEnemyPos );
	}
	
	idThread::ReturnInt( result );
}

/*
=====================
budAI::Event_WaitMove
=====================
*/
void budAI::Event_WaitMove()
{
	idThread::BeginMultiFrameEvent( this, &AI_WaitMove );
	
	if( MoveDone() )
	{
		idThread::EndMultiFrameEvent( this, &AI_WaitMove );
	}
}

/*
=====================
budAI::Event_GetJumpVelocity
=====================
*/
void budAI::Event_GetJumpVelocity( const Vector3& pos, float speed, float max_height )
{
	Vector3 start;
	Vector3 end;
	Vector3 dir;
	float dist;
	bool result;
	idEntity* enemyEnt = enemy.GetEntity();
	
	if( !enemyEnt )
	{
		idThread::ReturnVector( vec3_zero );
		return;
	}
	
	start = physicsObj.GetOrigin();
	end = pos;
	dir = end - start;
	dist = dir.Normalize();
	if( dist > 16.0f )
	{
		dist -= 16.0f;
		end -= dir * 16.0f;
	}
	
	result = PredictTrajectory( start, end, speed, physicsObj.GetGravity(), physicsObj.GetClipModel(), MASK_MONSTERSOLID, max_height, this, enemyEnt, ai_debugMove.GetBool() ? 4000 : 0, dir );
	if( result )
	{
		idThread::ReturnVector( dir * speed );
	}
	else
	{
		idThread::ReturnVector( vec3_zero );
	}
}

/*
=====================
budAI::Event_EntityInAttackCone
=====================
*/
void budAI::Event_EntityInAttackCone( idEntity* ent )
{
	float	attack_cone;
	Vector3	delta;
	float	yaw;
	float	relYaw;
	
	if( !ent )
	{
		idThread::ReturnInt( false );
		return;
	}
	
	delta = ent->GetPhysics()->GetOrigin() - GetEyePosition();
	
	// get our gravity normal
	const Vector3& gravityDir = GetPhysics()->GetGravityNormal();
	
	// infinite vertical vision, so project it onto our orientation plane
	delta -= gravityDir * ( gravityDir * delta );
	
	delta.Normalize();
	yaw = delta.ToYaw();
	
	attack_cone = spawnArgs.GetFloat( "attack_cone", "70" );
	relYaw = Math::AngleNormalize180( ideal_yaw - yaw );
	if( Math::Fabs( relYaw ) < ( attack_cone * 0.5f ) )
	{
		idThread::ReturnInt( true );
	}
	else
	{
		idThread::ReturnInt( false );
	}
}

/*
=====================
budAI::Event_CanSeeEntity
=====================
*/
void budAI::Event_CanSeeEntity( idEntity* ent )
{
	if( !ent )
	{
		idThread::ReturnInt( false );
		return;
	}
	
	bool cansee = CanSee( ent, false );
	idThread::ReturnInt( cansee );
}

/*
=====================
budAI::Event_SetTalkTarget
=====================
*/
void budAI::Event_SetTalkTarget( idEntity* target )
{
	if( target && !target->IsType( budActor::Type ) )
	{
		gameLocal.Error( "Cannot set talk target to '%s'.  Not a character or player.", target->GetName() );
	}
	talkTarget = static_cast<budActor*>( target );
	if( target )
	{
		AI_TALK = true;
	}
	else
	{
		AI_TALK = false;
	}
}

/*
=====================
budAI::Event_GetTalkTarget
=====================
*/
void budAI::Event_GetTalkTarget()
{
	idThread::ReturnEntity( talkTarget.GetEntity() );
}

/*
================
budAI::Event_SetTalkState
================
*/
void budAI::Event_SetTalkState( int state )
{
	if( ( state < 0 ) || ( state >= NUM_TALK_STATES ) )
	{
		gameLocal.Error( "Invalid talk state (%d)", state );
	}
	
	talk_state = static_cast<talkState_t>( state );
}

/*
=====================
budAI::Event_EnemyRange
=====================
*/
void budAI::Event_EnemyRange()
{
	float dist;
	budActor* enemyEnt = enemy.GetEntity();
	
	if( enemyEnt )
	{
		dist = ( enemyEnt->GetPhysics()->GetOrigin() - GetPhysics()->GetOrigin() ).Length();
	}
	else
	{
		// Just some really high number
		dist = Math::INFINITY;
	}
	
	idThread::ReturnFloat( dist );
}

/*
=====================
budAI::Event_EnemyRange2D
=====================
*/
void budAI::Event_EnemyRange2D()
{
	float dist;
	budActor* enemyEnt = enemy.GetEntity();
	
	if( enemyEnt )
	{
		dist = ( enemyEnt->GetPhysics()->GetOrigin().ToVec2() - GetPhysics()->GetOrigin().ToVec2() ).Length();
	}
	else
	{
		// Just some really high number
		dist = Math::INFINITY;
	}
	
	idThread::ReturnFloat( dist );
}

/*
=====================
budAI::Event_GetEnemy
=====================
*/
void budAI::Event_GetEnemy()
{
	idThread::ReturnEntity( enemy.GetEntity() );
}

/*
=====================
budAI::Event_GetEnemyPos
=====================
*/
void budAI::Event_GetEnemyPos()
{
	idThread::ReturnVector( lastVisibleEnemyPos );
}

/*
=====================
budAI::Event_GetEnemyEyePos
=====================
*/
void budAI::Event_GetEnemyEyePos()
{
	idThread::ReturnVector( lastVisibleEnemyPos + lastVisibleEnemyEyeOffset );
}

/*
=====================
budAI::Event_PredictEnemyPos
=====================
*/
void budAI::Event_PredictEnemyPos( float time )
{
	predictedPath_t path;
	budActor* enemyEnt = enemy.GetEntity();
	
	// if no enemy set
	if( !enemyEnt )
	{
		idThread::ReturnVector( physicsObj.GetOrigin() );
		return;
	}
	
	// predict the enemy movement
	budAI::PredictPath( enemyEnt, aas, lastVisibleEnemyPos, enemyEnt->GetPhysics()->GetLinearVelocity(), SEC2MS( time ), SEC2MS( time ), ( move.moveType == MOVETYPE_FLY ) ? SE_BLOCKED : ( SE_BLOCKED | SE_ENTER_LEDGE_AREA ), path );
	
	idThread::ReturnVector( path.endPos );
}

/*
=====================
budAI::Event_CanHitEnemy
=====================
*/
void budAI::Event_CanHitEnemy()
{
	trace_t	tr;
	idEntity* hit;
	
	budActor* enemyEnt = enemy.GetEntity();
	if( !AI_ENEMY_VISIBLE || !enemyEnt )
	{
		idThread::ReturnInt( false );
		return;
	}
	
	// don't check twice per frame
	if( gameLocal.time == lastHitCheckTime )
	{
		idThread::ReturnInt( lastHitCheckResult );
		return;
	}
	
	lastHitCheckTime = gameLocal.time;
	
	Vector3 toPos = enemyEnt->GetEyePosition();
	Vector3 eye = GetEyePosition();
	Vector3 dir;
	
	// expand the ray out as far as possible so we can detect anything behind the enemy
	dir = toPos - eye;
	dir.Normalize();
	toPos = eye + dir * MAX_WORLD_SIZE;
	gameLocal.clip.TracePoint( tr, eye, toPos, MASK_SHOT_BOUNDINGBOX, this );
	hit = gameLocal.GetTraceEntity( tr );
	if( tr.fraction >= 1.0f || ( hit == enemyEnt ) )
	{
		lastHitCheckResult = true;
	}
	else if( ( tr.fraction < 1.0f ) && ( hit->IsType( budAI::Type ) ) &&
			 ( static_cast<budAI*>( hit )->team != team ) )
	{
		lastHitCheckResult = true;
	}
	else
	{
		lastHitCheckResult = false;
	}
	
	idThread::ReturnInt( lastHitCheckResult );
}

/*
=====================
budAI::Event_CanHitEnemyFromAnim
=====================
*/
void budAI::Event_CanHitEnemyFromAnim( const char* animname )
{
	int		anim;
	Vector3	dir;
	Vector3	local_dir;
	Vector3	fromPos;
	Matrix3	axis;
	Vector3	start;
	trace_t	tr;
	float	distance;
	
	budActor* enemyEnt = enemy.GetEntity();
	if( !AI_ENEMY_VISIBLE || !enemyEnt )
	{
		idThread::ReturnInt( false );
		return;
	}
	
	anim = GetAnim( ANIMCHANNEL_LEGS, animname );
	if( !anim )
	{
		idThread::ReturnInt( false );
		return;
	}
	
	// just do a ray test if close enough
	if( enemyEnt->GetPhysics()->GetAbsBounds().IntersectsBounds( physicsObj.GetAbsBounds().Expand( 16.0f ) ) )
	{
		Event_CanHitEnemy();
		return;
	}
	
	// calculate the world transform of the launch position
	const Vector3& org = physicsObj.GetOrigin();
	dir = lastVisibleEnemyPos - org;
	physicsObj.GetGravityAxis().ProjectVector( dir, local_dir );
	local_dir.z = 0.0f;
	local_dir.ToVec2().Normalize();
	axis = local_dir.ToMat3();
	fromPos = physicsObj.GetOrigin() + missileLaunchOffset[ anim ] * axis;
	
	if( projectileClipModel == NULL )
	{
		CreateProjectileClipModel();
	}
	
	// check if the owner bounds is bigger than the projectile bounds
	const budBounds& ownerBounds = physicsObj.GetAbsBounds();
	const budBounds& projBounds = projectileClipModel->GetBounds();
	if( ( ( ownerBounds[1][0] - ownerBounds[0][0] ) > ( projBounds[1][0] - projBounds[0][0] ) ) &&
			( ( ownerBounds[1][1] - ownerBounds[0][1] ) > ( projBounds[1][1] - projBounds[0][1] ) ) &&
			( ( ownerBounds[1][2] - ownerBounds[0][2] ) > ( projBounds[1][2] - projBounds[0][2] ) ) )
	{
		if( ( ownerBounds - projBounds ).RayIntersection( org, viewAxis[ 0 ], distance ) )
		{
			start = org + distance * viewAxis[ 0 ];
		}
		else
		{
			start = ownerBounds.GetCenter();
		}
	}
	else
	{
		// projectile bounds bigger than the owner bounds, so just start it from the center
		start = ownerBounds.GetCenter();
	}
	
	gameLocal.clip.Translation( tr, start, fromPos, projectileClipModel, mat3_identity, MASK_SHOT_RENDERMODEL, this );
	fromPos = tr.endpos;
	
	if( GetAimDir( fromPos, enemy.GetEntity(), this, dir ) )
	{
		idThread::ReturnInt( true );
	}
	else
	{
		idThread::ReturnInt( false );
	}
}

/*
=====================
budAI::Event_CanHitEnemyFromJoint
=====================
*/
void budAI::Event_CanHitEnemyFromJoint( const char* jointname )
{
	trace_t	tr;
	Vector3	muzzle;
	Matrix3	axis;
	Vector3	start;
	float	distance;
	
	budActor* enemyEnt = enemy.GetEntity();
	if( !AI_ENEMY_VISIBLE || !enemyEnt )
	{
		idThread::ReturnInt( false );
		return;
	}
	
	// don't check twice per frame
	if( gameLocal.time == lastHitCheckTime )
	{
		idThread::ReturnInt( lastHitCheckResult );
		return;
	}
	
	lastHitCheckTime = gameLocal.time;
	
	const Vector3& org = physicsObj.GetOrigin();
	Vector3 toPos = enemyEnt->GetEyePosition();
	jointHandle_t joint = animator.GetJointHandle( jointname );
	if( joint == INVALID_JOINT )
	{
		gameLocal.Error( "Unknown joint '%s' on %s", jointname, GetEntityDefName() );
	}
	animator.GetJointTransform( joint, gameLocal.time, muzzle, axis );
	muzzle = org + ( muzzle + modelOffset ) * viewAxis * physicsObj.GetGravityAxis();
	
	if( projectileClipModel == NULL )
	{
		CreateProjectileClipModel();
	}
	
	// check if the owner bounds is bigger than the projectile bounds
	const budBounds& ownerBounds = physicsObj.GetAbsBounds();
	const budBounds& projBounds = projectileClipModel->GetBounds();
	if( ( ( ownerBounds[1][0] - ownerBounds[0][0] ) > ( projBounds[1][0] - projBounds[0][0] ) ) &&
			( ( ownerBounds[1][1] - ownerBounds[0][1] ) > ( projBounds[1][1] - projBounds[0][1] ) ) &&
			( ( ownerBounds[1][2] - ownerBounds[0][2] ) > ( projBounds[1][2] - projBounds[0][2] ) ) )
	{
		if( ( ownerBounds - projBounds ).RayIntersection( org, viewAxis[ 0 ], distance ) )
		{
			start = org + distance * viewAxis[ 0 ];
		}
		else
		{
			start = ownerBounds.GetCenter();
		}
	}
	else
	{
		// projectile bounds bigger than the owner bounds, so just start it from the center
		start = ownerBounds.GetCenter();
	}
	
	gameLocal.clip.Translation( tr, start, muzzle, projectileClipModel, mat3_identity, MASK_SHOT_BOUNDINGBOX, this );
	muzzle = tr.endpos;
	
	gameLocal.clip.Translation( tr, muzzle, toPos, projectileClipModel, mat3_identity, MASK_SHOT_BOUNDINGBOX, this );
	if( tr.fraction >= 1.0f || ( gameLocal.GetTraceEntity( tr ) == enemyEnt ) )
	{
		lastHitCheckResult = true;
	}
	else
	{
		lastHitCheckResult = false;
	}
	
	idThread::ReturnInt( lastHitCheckResult );
}

/*
=====================
budAI::Event_EnemyPositionValid
=====================
*/
void budAI::Event_EnemyPositionValid()
{
	bool result;
	
	result = EnemyPositionValid();
	idThread::ReturnInt( result );
}

/*
=====================
budAI::Event_ChargeAttack
=====================
*/
void budAI::Event_ChargeAttack( const char* damageDef )
{
	budActor* enemyEnt = enemy.GetEntity();
	
	StopMove( MOVE_STATUS_DEST_NOT_FOUND );
	if( enemyEnt )
	{
		Vector3 enemyOrg;
		
		if( move.moveType == MOVETYPE_FLY )
		{
			// position destination so that we're in the enemy's view
			enemyOrg = enemyEnt->GetEyePosition();
			enemyOrg -= enemyEnt->GetPhysics()->GetGravityNormal() * fly_offset;
		}
		else
		{
			enemyOrg = enemyEnt->GetPhysics()->GetOrigin();
		}
		
		BeginAttack( damageDef );
		DirectMoveToPosition( enemyOrg );
		TurnToward( enemyOrg );
	}
}

/*
=====================
budAI::Event_TestChargeAttack
=====================
*/
void budAI::Event_TestChargeAttack()
{
	trace_t trace;
	budActor* enemyEnt = enemy.GetEntity();
	predictedPath_t path;
	Vector3 end;
	
	if( !enemyEnt )
	{
		idThread::ReturnFloat( 0.0f );
		return;
	}
	
	if( move.moveType == MOVETYPE_FLY )
	{
		// position destination so that we're in the enemy's view
		end = enemyEnt->GetEyePosition();
		end -= enemyEnt->GetPhysics()->GetGravityNormal() * fly_offset;
	}
	else
	{
		end = enemyEnt->GetPhysics()->GetOrigin();
	}
	
	budAI::PredictPath( this, aas, physicsObj.GetOrigin(), end - physicsObj.GetOrigin(), 1000, 1000, ( move.moveType == MOVETYPE_FLY ) ? SE_BLOCKED : ( SE_ENTER_OBSTACLE | SE_BLOCKED | SE_ENTER_LEDGE_AREA ), path );
	
	if( ai_debugMove.GetBool() )
	{
		gameRenderWorld->DebugLine( colorGreen, physicsObj.GetOrigin(), end, 1 );
		gameRenderWorld->DebugBounds( path.endEvent == 0 ? colorYellow : colorRed, physicsObj.GetBounds(), end, 1 );
	}
	
	if( ( path.endEvent == 0 ) || ( path.blockingEntity == enemyEnt ) )
	{
		Vector3 delta = end - physicsObj.GetOrigin();
		float time = delta.LengthFast();
		idThread::ReturnFloat( time );
	}
	else
	{
		idThread::ReturnFloat( 0.0f );
	}
}

/*
=====================
budAI::Event_TestAnimMoveTowardEnemy
=====================
*/
void budAI::Event_TestAnimMoveTowardEnemy( const char* animname )
{
	int				anim;
	predictedPath_t path;
	Vector3			moveVec;
	float			yaw;
	Vector3			delta;
	budActor*			enemyEnt;
	
	enemyEnt = enemy.GetEntity();
	if( !enemyEnt )
	{
		idThread::ReturnInt( false );
		return;
	}
	
	anim = GetAnim( ANIMCHANNEL_LEGS, animname );
	if( !anim )
	{
		gameLocal.DWarning( "missing '%s' animation on '%s' (%s)", animname, name.c_str(), GetEntityDefName() );
		idThread::ReturnInt( false );
		return;
	}
	
	delta = enemyEnt->GetPhysics()->GetOrigin() - physicsObj.GetOrigin();
	yaw = delta.ToYaw();
	
	moveVec = animator.TotalMovementDelta( anim ) * Angles( 0.0f, yaw, 0.0f ).ToMat3() * physicsObj.GetGravityAxis();
	budAI::PredictPath( this, aas, physicsObj.GetOrigin(), moveVec, 1000, 1000, ( move.moveType == MOVETYPE_FLY ) ? SE_BLOCKED : ( SE_ENTER_OBSTACLE | SE_BLOCKED | SE_ENTER_LEDGE_AREA ), path );
	
	if( ai_debugMove.GetBool() )
	{
		gameRenderWorld->DebugLine( colorGreen, physicsObj.GetOrigin(), physicsObj.GetOrigin() + moveVec, 1 );
		gameRenderWorld->DebugBounds( path.endEvent == 0 ? colorYellow : colorRed, physicsObj.GetBounds(), physicsObj.GetOrigin() + moveVec, 1 );
	}
	
	idThread::ReturnInt( path.endEvent == 0 );
}

/*
=====================
budAI::Event_TestAnimMove
=====================
*/
void budAI::Event_TestAnimMove( const char* animname )
{
	int				anim;
	predictedPath_t path;
	Vector3			moveVec;
	
	anim = GetAnim( ANIMCHANNEL_LEGS, animname );
	if( !anim )
	{
		gameLocal.DWarning( "missing '%s' animation on '%s' (%s)", animname, name.c_str(), GetEntityDefName() );
		idThread::ReturnInt( false );
		return;
	}
	
	moveVec = animator.TotalMovementDelta( anim ) * Angles( 0.0f, ideal_yaw, 0.0f ).ToMat3() * physicsObj.GetGravityAxis();
	budAI::PredictPath( this, aas, physicsObj.GetOrigin(), moveVec, 1000, 1000, ( move.moveType == MOVETYPE_FLY ) ? SE_BLOCKED : ( SE_ENTER_OBSTACLE | SE_BLOCKED | SE_ENTER_LEDGE_AREA ), path );
	
	if( ai_debugMove.GetBool() )
	{
		gameRenderWorld->DebugLine( colorGreen, physicsObj.GetOrigin(), physicsObj.GetOrigin() + moveVec, 1 );
		gameRenderWorld->DebugBounds( path.endEvent == 0 ? colorYellow : colorRed, physicsObj.GetBounds(), physicsObj.GetOrigin() + moveVec, 1 );
	}
	
	idThread::ReturnInt( path.endEvent == 0 );
}

/*
=====================
budAI::Event_TestMoveToPosition
=====================
*/
void budAI::Event_TestMoveToPosition( const Vector3& position )
{
	predictedPath_t path;
	
	budAI::PredictPath( this, aas, physicsObj.GetOrigin(), position - physicsObj.GetOrigin(), 1000, 1000, ( move.moveType == MOVETYPE_FLY ) ? SE_BLOCKED : ( SE_ENTER_OBSTACLE | SE_BLOCKED | SE_ENTER_LEDGE_AREA ), path );
	
	if( ai_debugMove.GetBool() )
	{
		gameRenderWorld->DebugLine( colorGreen, physicsObj.GetOrigin(), position, 1 );
		gameRenderWorld->DebugBounds( colorYellow, physicsObj.GetBounds(), position, 1 );
		if( path.endEvent )
		{
			gameRenderWorld->DebugBounds( colorRed, physicsObj.GetBounds(), path.endPos, 1 );
		}
	}
	
	idThread::ReturnInt( path.endEvent == 0 );
}

/*
=====================
budAI::Event_TestMeleeAttack
=====================
*/
void budAI::Event_TestMeleeAttack()
{
	bool result = TestMelee();
	idThread::ReturnInt( result );
}

/*
=====================
budAI::Event_TestAnimAttack
=====================
*/
void budAI::Event_TestAnimAttack( const char* animname )
{
	int				anim;
	predictedPath_t path;
	
	anim = GetAnim( ANIMCHANNEL_LEGS, animname );
	if( !anim )
	{
		gameLocal.DWarning( "missing '%s' animation on '%s' (%s)", animname, name.c_str(), GetEntityDefName() );
		idThread::ReturnInt( false );
		return;
	}
	
	budAI::PredictPath( this, aas, physicsObj.GetOrigin(), animator.TotalMovementDelta( anim ), 1000, 1000, ( move.moveType == MOVETYPE_FLY ) ? SE_BLOCKED : ( SE_ENTER_OBSTACLE | SE_BLOCKED | SE_ENTER_LEDGE_AREA ), path );
	
	idThread::ReturnInt( path.blockingEntity && ( path.blockingEntity == enemy.GetEntity() ) );
}

/*
=====================
budAI::Event_PreBurn
=====================
*/
void budAI::Event_PreBurn()
{
	// No grabbing after the burn has started!
	noGrab = true;
	
	// for now this just turns shadows off
	renderEntity.noShadow = true;
}

/*
=====================
budAI::Event_Burn
=====================
*/
void budAI::Event_Burn()
{
	renderEntity.shaderParms[ SHADERPARM_TIME_OF_DEATH ] = gameLocal.time * 0.001f;
	SpawnParticles( "smoke_burnParticleSystem" );
	UpdateVisuals();
}

/*
=====================
budAI::Event_ClearBurn
=====================
*/
void budAI::Event_ClearBurn()
{
	renderEntity.noShadow = spawnArgs.GetBool( "noshadows" );
	renderEntity.shaderParms[ SHADERPARM_TIME_OF_DEATH ] = 0.0f;
	UpdateVisuals();
}

/*
=====================
budAI::Event_SetSmokeVisibility
=====================
*/
void budAI::Event_SetSmokeVisibility( int num, int on )
{
	int i;
	int time;
	
	if( num >= particles.Num() )
	{
		gameLocal.Warning( "Particle #%d out of range (%d particles) on entity '%s'", num, particles.Num(), name.c_str() );
		return;
	}
	
	if( on != 0 )
	{
		time = gameLocal.time;
		BecomeActive( TH_UPDATEPARTICLES );
	}
	else
	{
		time = 0;
	}
	
	if( num >= 0 )
	{
		particles[ num ].time = time;
	}
	else
	{
		for( i = 0; i < particles.Num(); i++ )
		{
			particles[ i ].time = time;
		}
	}
	
	UpdateVisuals();
}

/*
=====================
budAI::Event_NumSmokeEmitters
=====================
*/
void budAI::Event_NumSmokeEmitters()
{
	idThread::ReturnInt( particles.Num() );
}

/*
=====================
budAI::Event_StopThinking
=====================
*/
void budAI::Event_StopThinking()
{
	BecomeInactive( TH_THINK );
	idThread* thread = idThread::CurrentThread();
	if( thread )
	{
		thread->DoneProcessing();
	}
}

/*
=====================
budAI::Event_GetTurnDelta
=====================
*/
void budAI::Event_GetTurnDelta()
{
	float amount;
	
	if( turnRate )
	{
		amount = Math::AngleNormalize180( ideal_yaw - current_yaw );
		idThread::ReturnFloat( amount );
	}
	else
	{
		idThread::ReturnFloat( 0.0f );
	}
}

/*
=====================
budAI::Event_GetMoveType
=====================
*/
void budAI::Event_GetMoveType()
{
	idThread::ReturnInt( move.moveType );
}

/*
=====================
budAI::Event_SetMoveTypes
=====================
*/
void budAI::Event_SetMoveType( int moveType )
{
	if( ( moveType < 0 ) || ( moveType >= NUM_MOVETYPES ) )
	{
		gameLocal.Error( "Invalid movetype %d", moveType );
	}
	
	move.moveType = static_cast<moveType_t>( moveType );
	if( move.moveType == MOVETYPE_FLY )
	{
		travelFlags = TFL_WALK | TFL_AIR | TFL_FLY;
	}
	else
	{
		travelFlags = TFL_WALK | TFL_AIR;
	}
}

/*
=====================
budAI::Event_SaveMove
=====================
*/
void budAI::Event_SaveMove()
{
	savedMove = move;
}

/*
=====================
budAI::Event_RestoreMove
=====================
*/
void budAI::Event_RestoreMove()
{
	Vector3 goalPos;
	Vector3 dest;
	
	switch( savedMove.moveCommand )
	{
		case MOVE_NONE :
			StopMove( savedMove.moveStatus );
			break;
			
		case MOVE_FACE_ENEMY :
			FaceEnemy();
			break;
			
		case MOVE_FACE_ENTITY :
			FaceEntity( savedMove.goalEntity.GetEntity() );
			break;
			
		case MOVE_TO_ENEMY :
			MoveToEnemy();
			break;
			
		case MOVE_TO_ENEMYHEIGHT :
			MoveToEnemyHeight();
			break;
			
		case MOVE_TO_ENTITY :
			MoveToEntity( savedMove.goalEntity.GetEntity() );
			break;
			
		case MOVE_OUT_OF_RANGE :
			MoveOutOfRange( savedMove.goalEntity.GetEntity(), savedMove.range );
			break;
			
		case MOVE_TO_ATTACK_POSITION :
			MoveToAttackPosition( savedMove.goalEntity.GetEntity(), savedMove.anim );
			break;
			
		case MOVE_TO_COVER :
			MoveToCover( savedMove.goalEntity.GetEntity(), lastVisibleEnemyPos );
			break;
			
		case MOVE_TO_POSITION :
			MoveToPosition( savedMove.moveDest );
			break;
			
		case MOVE_TO_POSITION_DIRECT :
			DirectMoveToPosition( savedMove.moveDest );
			break;
			
		case MOVE_SLIDE_TO_POSITION :
			SlideToPosition( savedMove.moveDest, savedMove.duration );
			break;
			
		case MOVE_WANDER :
			WanderAround();
			break;
	}
	
	if( GetMovePos( goalPos ) )
	{
		CheckObstacleAvoidance( goalPos, dest );
	}
}

/*
=====================
budAI::Event_AllowMovement
=====================
*/
void budAI::Event_AllowMovement( float flag )
{
	allowMove = ( flag != 0.0f );
}

/*
=====================
budAI::Event_JumpFrame
=====================
*/
void budAI::Event_JumpFrame()
{
	AI_JUMP = true;
}

/*
=====================
budAI::Event_EnableClip
=====================
*/
void budAI::Event_EnableClip()
{
	physicsObj.SetClipMask( MASK_MONSTERSOLID );
	disableGravity = false;
}

/*
=====================
budAI::Event_DisableClip
=====================
*/
void budAI::Event_DisableClip()
{
	physicsObj.SetClipMask( 0 );
	disableGravity = true;
}

/*
=====================
budAI::Event_EnableGravity
=====================
*/
void budAI::Event_EnableGravity()
{
	disableGravity = false;
}

/*
=====================
budAI::Event_DisableGravity
=====================
*/
void budAI::Event_DisableGravity()
{
	disableGravity = true;
}

/*
=====================
budAI::Event_EnableAFPush
=====================
*/
void budAI::Event_EnableAFPush()
{
	af_push_moveables = true;
}

/*
=====================
budAI::Event_DisableAFPush
=====================
*/
void budAI::Event_DisableAFPush()
{
	af_push_moveables = false;
}

/*
=====================
budAI::Event_SetFlySpeed
=====================
*/
void budAI::Event_SetFlySpeed( float speed )
{
	if( move.speed == fly_speed )
	{
		move.speed = speed;
	}
	fly_speed = speed;
}

/*
================
budAI::Event_SetFlyOffset
================
*/
void budAI::Event_SetFlyOffset( int offset )
{
	fly_offset = offset;
}

/*
================
budAI::Event_ClearFlyOffset
================
*/
void budAI::Event_ClearFlyOffset()
{
	spawnArgs.GetInt( "fly_offset",	"0", fly_offset );
}

/*
=====================
budAI::Event_GetClosestHiddenTarget
=====================
*/
void budAI::Event_GetClosestHiddenTarget( const char* type )
{
	int	i;
	idEntity* ent;
	idEntity* bestEnt;
	float time;
	float bestTime;
	const Vector3& org = physicsObj.GetOrigin();
	budActor* enemyEnt = enemy.GetEntity();
	
	if( !enemyEnt )
	{
		// no enemy to hide from
		idThread::ReturnEntity( NULL );
		return;
	}
	
	if( targets.Num() == 1 )
	{
		ent = targets[ 0 ].GetEntity();
		if( ent != NULL && String::Cmp( ent->GetEntityDefName(), type ) == 0 )
		{
			if( !EntityCanSeePos( enemyEnt, lastVisibleEnemyPos, ent->GetPhysics()->GetOrigin() ) )
			{
				idThread::ReturnEntity( ent );
				return;
			}
		}
		idThread::ReturnEntity( NULL );
		return;
	}
	
	bestEnt = NULL;
	bestTime = Math::INFINITY;
	for( i = 0; i < targets.Num(); i++ )
	{
		ent = targets[ i ].GetEntity();
		if( ent != NULL && String::Cmp( ent->GetEntityDefName(), type ) == 0 )
		{
			const Vector3& destOrg = ent->GetPhysics()->GetOrigin();
			time = TravelDistance( org, destOrg );
			if( ( time >= 0.0f ) && ( time < bestTime ) )
			{
				if( !EntityCanSeePos( enemyEnt, lastVisibleEnemyPos, destOrg ) )
				{
					bestEnt = ent;
					bestTime = time;
				}
			}
		}
	}
	idThread::ReturnEntity( bestEnt );
}

/*
=====================
budAI::Event_GetRandomTarget
=====================
*/
void budAI::Event_GetRandomTarget( const char* type )
{
	int	i;
	int	num;
	int which;
	idEntity* ent;
	idEntity* ents[ MAX_GENTITIES ];
	
	num = 0;
	for( i = 0; i < targets.Num(); i++ )
	{
		ent = targets[ i ].GetEntity();
		if( ent != NULL && String::Cmp( ent->GetEntityDefName(), type ) == 0 )
		{
			ents[ num++ ] = ent;
			if( num >= MAX_GENTITIES )
			{
				break;
			}
		}
	}
	
	if( !num )
	{
		idThread::ReturnEntity( NULL );
		return;
	}
	
	which = gameLocal.random.RandomInt( num );
	idThread::ReturnEntity( ents[ which ] );
}

/*
================
budAI::Event_TravelDistanceToPoint
================
*/
void budAI::Event_TravelDistanceToPoint( const Vector3& pos )
{
	float time;
	
	time = TravelDistance( physicsObj.GetOrigin(), pos );
	idThread::ReturnFloat( time );
}

/*
================
budAI::Event_TravelDistanceToEntity
================
*/
void budAI::Event_TravelDistanceToEntity( idEntity* ent )
{
	float time;
	
	time = TravelDistance( physicsObj.GetOrigin(), ent->GetPhysics()->GetOrigin() );
	idThread::ReturnFloat( time );
}

/*
================
budAI::Event_TravelDistanceBetweenPoints
================
*/
void budAI::Event_TravelDistanceBetweenPoints( const Vector3& source, const Vector3& dest )
{
	float time;
	
	time = TravelDistance( source, dest );
	idThread::ReturnFloat( time );
}

/*
================
budAI::Event_TravelDistanceBetweenEntities
================
*/
void budAI::Event_TravelDistanceBetweenEntities( idEntity* source, idEntity* dest )
{
	float time;
	
	assert( source );
	assert( dest );
	time = TravelDistance( source->GetPhysics()->GetOrigin(), dest->GetPhysics()->GetOrigin() );
	idThread::ReturnFloat( time );
}

/*
=====================
budAI::Event_LookAtEntity
=====================
*/
void budAI::Event_LookAtEntity( idEntity* ent, float duration )
{
	if( ent == this )
	{
		ent = NULL;
	}
	
	if( ( ent != focusEntity.GetEntity() ) || ( focusTime < gameLocal.time ) )
	{
		focusEntity	= ent;
		alignHeadTime = gameLocal.time;
		forceAlignHeadTime = gameLocal.time + SEC2MS( 1 );
		blink_time = 0;
	}
	
	focusTime = gameLocal.time + SEC2MS( duration );
}

/*
=====================
budAI::Event_LookAtEnemy
=====================
*/
void budAI::Event_LookAtEnemy( float duration )
{
	budActor* enemyEnt;
	
	enemyEnt = enemy.GetEntity();
	if( ( enemyEnt != focusEntity.GetEntity() ) || ( focusTime < gameLocal.time ) )
	{
		focusEntity	= enemyEnt;
		alignHeadTime = gameLocal.time;
		forceAlignHeadTime = gameLocal.time + SEC2MS( 1 );
		blink_time = 0;
	}
	
	focusTime = gameLocal.time + SEC2MS( duration );
}

/*
===============
budAI::Event_SetJointMod
===============
*/
void budAI::Event_SetJointMod( int allow )
{
	allowJointMod = ( allow != 0 );
}

/*
================
budAI::Event_ThrowMoveable
================
*/
void budAI::Event_ThrowMoveable()
{
	idEntity* ent;
	idEntity* moveable = NULL;
	
	for( ent = GetNextTeamEntity(); ent != NULL; ent = ent->GetNextTeamEntity() )
	{
		if( ent->GetBindMaster() == this && ent->IsType( idMoveable::Type ) )
		{
			moveable = ent;
			break;
		}
	}
	if( moveable )
	{
		moveable->Unbind();
		
		// RB: 64 bit fixes, changed NULL to 0
		moveable->PostEventMS( &EV_SetOwner, 200, 0 );
		// RB end
	}
}

/*
================
budAI::Event_ThrowAF
================
*/
void budAI::Event_ThrowAF()
{
	idEntity* ent;
	idEntity* af = NULL;
	
	for( ent = GetNextTeamEntity(); ent != NULL; ent = ent->GetNextTeamEntity() )
	{
		if( ent->GetBindMaster() == this && ent->IsType( budAFEntity_Base::Type ) )
		{
			af = ent;
			break;
		}
	}
	if( af )
	{
		af->Unbind();
		
		// RB: 64 bit fixes, changed NULL to 0
		af->PostEventMS( &EV_SetOwner, 200, 0 );
		// RB end
	}
}

/*
================
budAI::Event_SetAngles
================
*/
void budAI::Event_SetAngles( Angles const& ang )
{
	current_yaw = ang.yaw;
	viewAxis = Angles( 0, current_yaw, 0 ).ToMat3();
}

/*
================
budAI::Event_GetAngles
================
*/
void budAI::Event_GetAngles()
{
	idThread::ReturnVector( Vector3( 0.0f, current_yaw, 0.0f ) );
}

/*
================
budAI::Event_GetTrajectoryToPlayer
================
*/
void budAI::Event_GetTrajectoryToPlayer()
{

	Vector3 start;
	Vector3 end;
	Vector3 dir;
	float dist;
// 	bool result;
	idEntity* enemyEnt = enemy.GetEntity();
	
	if( !enemyEnt )
	{
		idThread::ReturnVector( vec3_zero );
		return;
	}
	
	end = enemyEnt->GetPhysics()->GetOrigin();
	float speed = 400.0f;
	
	if( speed <= 0.0f )
	{
		gameLocal.Error( "Invalid speed.  speed must be > 0." );
	}
	
	start = physicsObj.GetOrigin() + Vector3( 0.0f, 0.0f, 50.0f );
	dir = end - start;
	dist = dir.Normalize();
	if( dist > 16.0f )
	{
		dist -= 16.0f;
		end -= dir * 16.0f;
	}
	
	
	
	Vector3 gravity;
	ballistics_t ballistics[2];
	Ballistics( start, end, speed, 0.0f, ballistics );
	dir.y = ballistics[0].angle;
	
	
	
	
	
//	result = PredictTrajectory( start, end, speed, physicsObj.GetGravity(), physicsObj.GetClipModel(), MASK_MONSTERSOLID, 1000.0f, this, enemyEnt, ai_debugMove.GetBool() ? 4000 : 0, dir );
//	if ( result ) {
	idThread::ReturnVector( dir * speed );
// 	} else {
// 		idThread::ReturnVector( vec3_zero );
// 	}
}

/*
================
budAI::Event_RealKill
================
*/
void budAI::Event_RealKill()
{
	health = 0;
	
	if( af.IsLoaded() )
	{
		// clear impacts
		af.Rest();
		
		// physics is turned off by calling af.Rest()
		BecomeActive( TH_PHYSICS );
	}
	
	Killed( this, this, 0, vec3_zero, INVALID_JOINT );
}

/*
================
budAI::Event_Kill
================
*/
void budAI::Event_Kill()
{
	PostEventMS( &AI_RealKill, 0 );
}

/*
================
budAI::Event_WakeOnFlashlight
================
*/
void budAI::Event_WakeOnFlashlight( int enable )
{
	wakeOnFlashlight = ( enable != 0 );
}

/*
================
budAI::Event_LocateEnemy
================
*/
void budAI::Event_LocateEnemy()
{
	budActor* enemyEnt;
	int areaNum;
	
	enemyEnt = enemy.GetEntity();
	if( !enemyEnt )
	{
		return;
	}
	
	enemyEnt->GetAASLocation( aas, lastReachableEnemyPos, areaNum );
	SetEnemyPosition();
	UpdateEnemyPosition();
}

/*
================
budAI::Event_KickObstacles
================
*/
void budAI::Event_KickObstacles( idEntity* kickEnt, float force )
{
	Vector3 dir;
	idEntity* obEnt;
	
	if( kickEnt )
	{
		obEnt = kickEnt;
	}
	else
	{
		obEnt = move.obstacle.GetEntity();
	}
	
	if( obEnt )
	{
		dir = obEnt->GetPhysics()->GetOrigin() - physicsObj.GetOrigin();
		dir.Normalize();
	}
	else
	{
		dir = viewAxis[ 0 ];
	}
	KickObstacles( dir, force, obEnt );
}

/*
================
budAI::Event_GetObstacle
================
*/
void budAI::Event_GetObstacle()
{
	idThread::ReturnEntity( move.obstacle.GetEntity() );
}

/*
================
budAI::Event_PushPointIntoAAS
================
*/
void budAI::Event_PushPointIntoAAS( const Vector3& pos )
{
	int		areaNum;
	Vector3	newPos;
	
	areaNum = PointReachableAreaNum( pos );
	if( areaNum )
	{
		newPos = pos;
		aas->PushPointIntoAreaNum( areaNum, newPos );
		idThread::ReturnVector( newPos );
	}
	else
	{
		idThread::ReturnVector( pos );
	}
}


/*
================
budAI::Event_GetTurnRate
================
*/
void budAI::Event_GetTurnRate()
{
	idThread::ReturnFloat( turnRate );
}

/*
================
budAI::Event_SetTurnRate
================
*/
void budAI::Event_SetTurnRate( float rate )
{
	turnRate = rate;
}

/*
================
budAI::Event_AnimTurn
================
*/
void budAI::Event_AnimTurn( float angles )
{
	turnVel = 0.0f;
	anim_turn_angles = angles;
	if( angles )
	{
		anim_turn_yaw = current_yaw;
		anim_turn_amount = Math::Fabs( Math::AngleNormalize180( current_yaw - ideal_yaw ) );
		if( anim_turn_amount > anim_turn_angles )
		{
			anim_turn_amount = anim_turn_angles;
		}
	}
	else
	{
		anim_turn_amount = 0.0f;
		animator.CurrentAnim( ANIMCHANNEL_LEGS )->SetSyncedAnimWeight( 0, 1.0f );
		animator.CurrentAnim( ANIMCHANNEL_LEGS )->SetSyncedAnimWeight( 1, 0.0f );
		animator.CurrentAnim( ANIMCHANNEL_TORSO )->SetSyncedAnimWeight( 0, 1.0f );
		animator.CurrentAnim( ANIMCHANNEL_TORSO )->SetSyncedAnimWeight( 1, 0.0f );
	}
}

/*
================
budAI::Event_AllowHiddenMovement
================
*/
void budAI::Event_AllowHiddenMovement( int enable )
{
	allowHiddenMovement = ( enable != 0 );
}

/*
================
budAI::Event_TriggerParticles
================
*/
void budAI::Event_TriggerParticles( const char* jointName )
{
	TriggerParticles( jointName );
}

/*
=====================
budAI::Event_FindActorsInBounds
=====================
*/
void budAI::Event_FindActorsInBounds( const Vector3& mins, const Vector3& maxs )
{
	idEntity* 	ent;
	idEntity* 	entityList[ MAX_GENTITIES ];
	int			numListedEntities;
	int			i;
	
	numListedEntities = gameLocal.clip.EntitiesTouchingBounds( budBounds( mins, maxs ), CONTENTS_BODY, entityList, MAX_GENTITIES );
	for( i = 0; i < numListedEntities; i++ )
	{
		ent = entityList[ i ];
		if( ent != this && !ent->IsHidden() && ( ent->health > 0 ) && ent->IsType( budActor::Type ) )
		{
			idThread::ReturnEntity( ent );
			return;
		}
	}
	
	idThread::ReturnEntity( NULL );
}

/*
================
budAI::Event_CanReachPosition
================
*/
void budAI::Event_CanReachPosition( const Vector3& pos )
{
	aasPath_t	path;
	int			toAreaNum;
	int			areaNum;
	
	toAreaNum = PointReachableAreaNum( pos );
	areaNum	= PointReachableAreaNum( physicsObj.GetOrigin() );
	if( !toAreaNum || !PathToGoal( path, areaNum, physicsObj.GetOrigin(), toAreaNum, pos ) )
	{
		idThread::ReturnInt( false );
	}
	else
	{
		idThread::ReturnInt( true );
	}
}

/*
================
budAI::Event_CanReachEntity
================
*/
void budAI::Event_CanReachEntity( idEntity* ent )
{
	aasPath_t	path;
	int			toAreaNum;
	int			areaNum;
	Vector3		pos;
	
	if( !ent )
	{
		idThread::ReturnInt( false );
		return;
	}
	
	if( move.moveType != MOVETYPE_FLY )
	{
		if( !ent->GetFloorPos( 64.0f, pos ) )
		{
			idThread::ReturnInt( false );
			return;
		}
		if( ent->IsType( budActor::Type ) && static_cast<budActor*>( ent )->OnLadder() )
		{
			idThread::ReturnInt( false );
			return;
		}
	}
	else
	{
		pos = ent->GetPhysics()->GetOrigin();
	}
	
	toAreaNum = PointReachableAreaNum( pos );
	if( !toAreaNum )
	{
		idThread::ReturnInt( false );
		return;
	}
	
	const Vector3& org = physicsObj.GetOrigin();
	areaNum	= PointReachableAreaNum( org );
	if( !toAreaNum || !PathToGoal( path, areaNum, org, toAreaNum, pos ) )
	{
		idThread::ReturnInt( false );
	}
	else
	{
		idThread::ReturnInt( true );
	}
}

/*
================
budAI::Event_CanReachEnemy
================
*/
void budAI::Event_CanReachEnemy()
{
	aasPath_t	path;
	int			toAreaNum;
	int			areaNum;
	Vector3		pos;
	budActor*		enemyEnt;
	
	enemyEnt = enemy.GetEntity();
	if( !enemyEnt )
	{
		idThread::ReturnInt( false );
		return;
	}
	
	if( move.moveType != MOVETYPE_FLY )
	{
		if( enemyEnt->OnLadder() )
		{
			idThread::ReturnInt( false );
			return;
		}
		enemyEnt->GetAASLocation( aas, pos, toAreaNum );
	}
	else
	{
		pos = enemyEnt->GetPhysics()->GetOrigin();
		toAreaNum = PointReachableAreaNum( pos );
	}
	
	if( !toAreaNum )
	{
		idThread::ReturnInt( false );
		return;
	}
	
	const Vector3& org = physicsObj.GetOrigin();
	areaNum	= PointReachableAreaNum( org );
	if( !PathToGoal( path, areaNum, org, toAreaNum, pos ) )
	{
		idThread::ReturnInt( false );
	}
	else
	{
		idThread::ReturnInt( true );
	}
}

/*
================
budAI::Event_GetReachableEntityPosition
================
*/
void budAI::Event_GetReachableEntityPosition( idEntity* ent )
{
	int		toAreaNum;
	Vector3	pos;
	
	if( move.moveType != MOVETYPE_FLY )
	{
		if( !ent->GetFloorPos( 64.0f, pos ) )
		{
			// NOTE: not a good way to return 'false'
			return idThread::ReturnVector( vec3_zero );
		}
		if( ent->IsType( budActor::Type ) && static_cast<budActor*>( ent )->OnLadder() )
		{
			// NOTE: not a good way to return 'false'
			return idThread::ReturnVector( vec3_zero );
		}
	}
	else
	{
		pos = ent->GetPhysics()->GetOrigin();
	}
	
	if( aas )
	{
		toAreaNum = PointReachableAreaNum( pos );
		aas->PushPointIntoAreaNum( toAreaNum, pos );
	}
	
	idThread::ReturnVector( pos );
}

/*
================
budAI::Event_MoveToPositionDirect
================
*/
void budAI::Event_MoveToPositionDirect( const Vector3& pos )
{
	StopMove( MOVE_STATUS_DONE );
	DirectMoveToPosition( pos );
}

/*
================
budAI::Event_AvoidObstacles
================
*/
void budAI::Event_AvoidObstacles( int ignore )
{
	ignore_obstacles = ( ignore == 1 ) ? false : true;
}

/*
================
budAI::Event_TriggerFX
================
*/
void budAI::Event_TriggerFX( const char* joint, const char* fx )
{
	TriggerFX( joint, fx );
}

void budAI::Event_StartEmitter( const char* name, const char* joint, const char* particle )
{
	idEntity* ent = StartEmitter( name, joint, particle );
	idThread::ReturnEntity( ent );
}

void budAI::Event_GetEmitter( const char* name )
{
	idThread::ReturnEntity( GetEmitter( name ) );
}

void budAI::Event_StopEmitter( const char* name )
{
	StopEmitter( name );
}


/*
=====================
budAI::Event_LaunchHomingMissile
=====================
*/
void budAI::Event_LaunchHomingMissile()
{
	Vector3		start;
	trace_t		tr;
	budBounds	projBounds;
	const budClipModel* projClip;
	Matrix3		axis;
	float		distance;
	
	if( !projectileDef )
	{
		gameLocal.Warning( "%s (%s) doesn't have a projectile specified", name.c_str(), GetEntityDefName() );
		idThread::ReturnEntity( NULL );
		return;
	}
	
	budActor* enemy = GetEnemy();
	if( enemy == NULL )
	{
		idThread::ReturnEntity( NULL );
		return;
	}
	
	Vector3 org = GetPhysics()->GetOrigin() + Vector3( 0.0f, 0.0f, 250.0f );
	Vector3 goal = enemy->GetPhysics()->GetOrigin();
	homingMissileGoal = goal;
	
//	axis = ( goal - org ).ToMat3();
//	axis.Identity();
	if( !projectile.GetEntity() )
	{
		idHomingProjectile* homing = ( idHomingProjectile* ) CreateProjectile( org, Vector3( 0.0f, 0.0f, 1.0f ) );
		if( homing != NULL )
		{
			homing->SetEnemy( enemy );
			homing->SetSeekPos( homingMissileGoal );
		}
	}
	
	// make sure the projectile starts inside the monster bounding box
	const budBounds& ownerBounds = physicsObj.GetAbsBounds();
	projClip = projectile.GetEntity()->GetPhysics()->GetClipModel();
	projBounds = projClip->GetBounds().Rotate( projClip->GetAxis() );
	
	// check if the owner bounds is bigger than the projectile bounds
	if( ( ( ownerBounds[1][0] - ownerBounds[0][0] ) > ( projBounds[1][0] - projBounds[0][0] ) ) &&
			( ( ownerBounds[1][1] - ownerBounds[0][1] ) > ( projBounds[1][1] - projBounds[0][1] ) ) &&
			( ( ownerBounds[1][2] - ownerBounds[0][2] ) > ( projBounds[1][2] - projBounds[0][2] ) ) )
	{
		if( ( ownerBounds - projBounds ).RayIntersection( org, viewAxis[ 0 ], distance ) )
		{
			start = org + distance * viewAxis[ 0 ];
		}
		else
		{
			start = ownerBounds.GetCenter();
		}
	}
	else
	{
		// projectile bounds bigger than the owner bounds, so just start it from the center
		start = ownerBounds.GetCenter();
	}
	
	gameLocal.clip.Translation( tr, start, org, projClip, projClip->GetAxis(), MASK_SHOT_RENDERMODEL, this );
	
	// launch the projectile
	idThread::ReturnEntity( projectile.GetEntity() );
	Vector3 dir = homingMissileGoal - org;
	Angles ang = dir.ToAngles();
	ang.pitch = -45.0f;
	projectile.GetEntity()->Launch( org, ang.ToForward(), Vector3_Origin );
	projectile = NULL;
	
	TriggerWeaponEffects( tr.endpos );
	
	lastAttackTime = gameLocal.time;
}

/*
=====================
budAI::Event_SetHomingMissileGoal
=====================
*/
void budAI::Event_SetHomingMissileGoal()
{
	budActor* enemy = GetEnemy();
	if( enemy == NULL )
	{
		idThread::ReturnEntity( NULL );
		return;
	}
	
	homingMissileGoal = enemy->GetPhysics()->GetOrigin();
}
