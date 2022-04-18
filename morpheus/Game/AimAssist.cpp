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

/*
================================================================================================
Contains the AimAssist implementation.
================================================================================================
*/

CVar aa_targetAimAssistEnable(	"aa_targetAimAssistEnable",						"0",		CVAR_BOOL | CVAR_ARCHIVE,	"Enables/Disables the entire Aim Assist system" );

CVar aa_targetAdhesionEnable(	"aa_targetAdhesionEnable",						"1",		CVAR_BOOL,	"Enables Target Adhesion" );
CVar aa_targetFrictionEnable(	"aa_targetFrictionEnable",						"1",		CVAR_BOOL,	"Enables Target Friction" );

// Selection
CVar aa_targetMaxDistance(	"aa_targetMaxDistance",							"3000",		CVAR_FLOAT, "The Maximum Distance away for a target to be considered for adhesion, friction and target lock-to" );
CVar aa_targetSelectionRadius(	"aa_targetSelectionRadius",						"128.0",	CVAR_FLOAT, "Radius used to select targets for auto aiming" );

// Adhesion
CVar aa_targetAdhesionRadius(	"aa_targetAdhesionRadius",						"96.0",		CVAR_FLOAT, "Radius used to apply adhesion amount" );
CVar aa_targetAdhesionYawSpeedMax(	"aa_targetAdhesionYawSpeedMax",					"0.6",		CVAR_FLOAT, "Max Yaw Adhesion Speed" );
CVar aa_targetAdhesionPitchSpeedMax(	"aa_targetAdhesionPitchSpeedMax",				"0.6",		CVAR_FLOAT, "Max Pitch Adhesion Speed" );
CVar aa_targetAdhesionContributionPctMax(	"aa_targetAdhesionContributionPctMax",			"0.25",		CVAR_FLOAT, "Max Adhesion Contribution Percentage - Range 0.0 - 1.0" );
CVar aa_targetAdhesionPlayerSpeedThreshold(	"aa_targetAdhesionPlayerSpeedThreshold",		"10.0",		CVAR_FLOAT, "Speed Threshold that determines how fast the player needs to move before adhesion is allowed to kick in" );

// Friction
CVar aa_targetFrictionMaxDistance(	"aa_targetFrictionMaxDistance",					"1024.0",	CVAR_FLOAT, "Minimum Distance Friction takes effect" );
CVar aa_targetFrictionOptimalDistance(	"aa_targetFrictionOptimalDistance",				"768.0",	CVAR_FLOAT, "Optimal Distance for Friction to take an effect" );
CVar aa_targetFrictionRadius(	"aa_targetFrictionRadius",						"96.0",		CVAR_FLOAT, "Friction Collision Sphere Radius" );
CVar aa_targetFrictionOptimalRadius(	"aa_targetFrictionOptimalRadius",				"192.0",	CVAR_FLOAT, "Friction Collision Sphere Radius when at Optimal Distance" );
CVar aa_targetFrictionMultiplierMin(	"aa_targetFrictionMultiplierMin",				"1.0",		CVAR_FLOAT, "Minimum Friction Scalar" );
CVar aa_targetFrictionMultiplierMax(	"aa_targetFrictionMultiplierMax",				"0.4",		CVAR_FLOAT, "Maximum Friction Scalar" );

/*
========================
budAimAssist::Init
========================
*/
void budAimAssist::Init( budPlayer* player_ )
{
	player = player_;
	angleCorrection = ang_zero;
	frictionScalar = 1.0f;
	lastTargetPos = vec3_zero;
}

/*
========================
budAimAssist::Update
========================
*/
void budAimAssist::Update()
{
	angleCorrection = ang_zero;
	
	UpdateNewAimAssist();
}

/*
========================
budAimAssist::UpdateNewAimAssist
========================
*/
void budAimAssist::UpdateNewAimAssist()
{

	angleCorrection = ang_zero;
	frictionScalar = 1.0f;
	idEntity* lastTarget = targetEntity;
	targetEntity = NULL;
	
	// is aim assisting allowed?  If not then just bail out
	if( !aa_targetAimAssistEnable.GetBool() )
	{
		return;
	}
	
	bool forceLastTarget = false;
	Vector3 targetPos;
	
	idEntity* entity = NULL;
	if( forceLastTarget )
	{
		entity = lastTarget;
		targetPos = lastTargetPos;
	}
	else
	{
		entity = FindAimAssistTarget( targetPos );
	}
	
	if( entity != NULL )
	{
	
		UpdateFriction( entity, targetPos );
		
		// by default we don't allow adhesion when we are standing still
		const float playerMovementSpeedThreshold = Square( aa_targetAdhesionPlayerSpeedThreshold.GetFloat() );
		float playerSpeed = player->GetPhysics()->GetLinearVelocity().LengthSqr();
		
		// only allow adhesion on actors (ai) or players.  Disallow adhesion on any static world entities such as explosive barrels
		if( playerSpeed > playerMovementSpeedThreshold )
		{
			UpdateAdhesion( entity, targetPos );
		}
		
		targetEntity = entity;
	}
	
	lastTargetPos = targetPos;
}

/*
========================
budAimAssist::FindAimAssistTarget
========================
*/
idEntity* budAimAssist::FindAimAssistTarget( Vector3& targetPos )
{
	if( player == NULL )
	{
		return NULL;
	}
	
	//TO DO: Make this faster
	//TO DO: Defer Traces
	idEntity* 	optimalTarget = NULL;
	float		currentBestScore = -Math::INFINITY;
	targetPos = vec3_zero;
	
	Vector3 cameraPos;
	Matrix3 cameraAxis;
	player->GetViewPos( cameraPos, cameraAxis );
	
	float maxDistanceSquared = Square( aa_targetMaxDistance.GetFloat() );
	
	Vector3 dirToTarget;
	float  distanceToTargetSquared;
	Vector3 primaryTargetPos;
	Vector3 secondaryTargetPos;
	
	for( idEntity* entity = gameLocal.aimAssistEntities.Next(); entity != NULL; entity = entity->aimAssistNode.Next() )
	{
		if( !entity->IsActive() )
		{
			continue;
		}
		
		if( entity->IsHidden() )
		{
			continue;
		}
		
		if( entity->IsType( budActor::Type ) )
		{
			budActor* actor = static_cast<budActor*>( entity );
			if( actor->team == player->team )
			{
				// In DM, LMS, and Tourney, all players are on the same team
				if( gameLocal.gameType == GAME_CTF || gameLocal.gameType == GAME_TDM || gameLocal.gameType == GAME_SP )
				{
					continue;
				}
			}
		}
		
		if( entity->IsType( budAI::Type ) )
		{
			budAI* aiEntity = static_cast<budAI*>( entity );
			if( aiEntity->ReactionTo( player ) == ATTACK_IGNORE )
			{
				continue;
			}
		}
		
		// check whether we have a valid target position for this entity - skip it if we don't
		if( !ComputeTargetPos( entity, primaryTargetPos, secondaryTargetPos ) )
		{
			continue;
		}
		
		// is it close enough to us
		dirToTarget = primaryTargetPos - cameraPos;
		distanceToTargetSquared = dirToTarget.LengthSqr();
		if( distanceToTargetSquared > maxDistanceSquared )
		{
			continue;
		}
		
		// Compute a score in the range of 0..1 for how much are looking towards the target.
		Vector3 forward = cameraAxis[ 0 ];
		forward.Normalize();
		dirToTarget.Normalize();
		float ViewDirDotTargetDir = Math::ClampFloat( -1.0f, 1.0f, forward * dirToTarget ); // compute the dot and clamp to account for floating point error
		
		// throw out anything thats outside of weapon's global FOV.
		if( ViewDirDotTargetDir < 0.0f )
		{
			continue;
		}
		
		// to be consistent we always use the primaryTargetPos to compute the score for this entity
		float computedScore = ComputeEntityAimAssistScore( primaryTargetPos, cameraPos, cameraAxis );
		
		// check if the current score beats our current best score and we have line of sight to it.
		if( computedScore > currentBestScore )
		{
		
			// determine if the current target is in our line of sight
			trace_t tr;
			gameLocal.clip.TracePoint( tr, cameraPos, primaryTargetPos, MASK_MONSTERSOLID, player );
			
			// did our trace fail?
			if( ( ( tr.fraction < 1.0f ) && ( tr.c.entityNum != entity->entityNumber ) ) || ( tr.fraction >= 1.0f ) )
			{
			
				// if the collision test failed for the primary position -- check the secondary position
				trace_t tr2;
				gameLocal.clip.TracePoint( tr2, cameraPos, secondaryTargetPos, MASK_MONSTERSOLID, player );
				
				if( ( ( tr2.fraction < 1.0f ) && ( tr2.c.entityNum != entity->entityNumber ) ) || ( tr2.fraction >= 1.0f ) )
				{
					// if the secondary position is also not visible then give up
					continue;
				}
				
				// we can see the secondary target position so we should consider this entity but use
				// the secondary position as the target position
				targetPos = secondaryTargetPos;
			}
			else
			{
				targetPos = primaryTargetPos;
			}
			
			// if we got here then this is our new best score
			optimalTarget = entity;
			currentBestScore = computedScore;
		}
	}
	
	return optimalTarget;
}

/*
========================
budAimAssist::ComputeEntityAimAssistScore
========================
*/
float budAimAssist::ComputeEntityAimAssistScore( const Vector3& targetPos, const Vector3& cameraPos, const Matrix3& cameraAxis )
{

	float score = 0.0f;
	
	Vector3 dirToTarget = targetPos - cameraPos;
	float distanceToTarget = dirToTarget.Length();
	
	// Compute a score in the range of 0..1 for how much are looking towards the target.
	Vector3 forward = cameraAxis[0];
	forward.Normalize();
	dirToTarget.Normalize();
	float ViewDirDotTargetDir = Math::ClampFloat( 0.0f, 1.0f, forward * dirToTarget ); // compute the dot and clamp to account for floating point error
	
	// the more we look at the target the higher our score
	score = ViewDirDotTargetDir;
	
	// weigh the score from the view angle higher than the distance score
	static float aimWeight = 0.8f;
	score *= aimWeight;
	// Add a score of 0..1 for how close the target is to the player
	if( distanceToTarget < aa_targetMaxDistance.GetFloat() )
	{
		float distanceScore = 1.0f - ( distanceToTarget / aa_targetMaxDistance.GetFloat() );
		float distanceWeight = 1.0f - aimWeight;
		score += ( distanceScore * distanceWeight );
	}
	
	return score * 1000.0f;
}

/*
========================
budAimAssist::UpdateAdhesion
========================
*/
void budAimAssist::UpdateAdhesion( idEntity* pTarget, const Vector3& targetPos )
{

	if( !aa_targetAdhesionEnable.GetBool() )
	{
		return;
	}
	
	if( !pTarget )
	{
		return;
	}
	
	float contributionPctMax = aa_targetAdhesionContributionPctMax.GetFloat();
	
	Vector3 cameraPos;
	Matrix3 cameraAxis;
	player->GetViewPos( cameraPos, cameraAxis );
	
	Angles cameraViewAngles = cameraAxis.ToAngles();
	cameraViewAngles.Normalize180();
	
	Vector3 cameraViewPos = cameraPos;
	Vector3 dirToTarget = targetPos - cameraViewPos;
	float distanceToTarget = dirToTarget.Length();
	
	Angles aimAngles = dirToTarget.ToAngles();
	aimAngles.Normalize180();
	
	// find the delta
	aimAngles -= cameraViewAngles;
	
	// clamp velocities to some max values
	aimAngles.yaw = Math::ClampFloat( -aa_targetAdhesionYawSpeedMax.GetFloat(), aa_targetAdhesionYawSpeedMax.GetFloat(), aimAngles.yaw );
	aimAngles.pitch = Math::ClampFloat( -aa_targetAdhesionPitchSpeedMax.GetFloat(), aa_targetAdhesionPitchSpeedMax.GetFloat(), aimAngles.pitch );
	
	Vector3 forward = cameraAxis[0];
	forward.Normalize();
	dirToTarget.Normalize();
	float ViewDirDotTargetDir = Math::ClampFloat( 0.0f, 1.0f, forward * dirToTarget ); // compute the dot and clamp to account for floating point error
	float aimLength = ViewDirDotTargetDir * distanceToTarget;
	Vector3 aimPoint = cameraPos + ( forward * aimLength );
	float delta = Math::Sqrt( Square( distanceToTarget ) - Square( aimLength ) );
	
	float contribution = Math::ClampFloat( 0.0f, contributionPctMax, 1.0f - ( delta / aa_targetAdhesionRadius.GetFloat() ) );
	angleCorrection.yaw = aimAngles.yaw * contribution;
	angleCorrection.pitch = aimAngles.pitch * contribution;
}

/*
========================
budAimAssist::ComputeFrictionRadius
========================
*/
float budAimAssist::ComputeFrictionRadius( float distanceToTarget )
{

	if( ( distanceToTarget <= Math::FLT_SMALLEST_NON_DENORMAL ) || distanceToTarget > aa_targetFrictionMaxDistance.GetFloat() )
	{
		return aa_targetFrictionRadius.GetFloat();
	}
	
	float distanceContributionScalar = ( aa_targetFrictionOptimalDistance.GetFloat() > 0.0f ) ? ( distanceToTarget / aa_targetFrictionOptimalDistance.GetFloat() ) : 0.0f;
	
	if( distanceToTarget > aa_targetFrictionOptimalDistance.GetFloat() )
	{
		const float range = Math::ClampFloat( 0.0f, aa_targetFrictionMaxDistance.GetFloat(), aa_targetFrictionMaxDistance.GetFloat() - aa_targetFrictionOptimalDistance.GetFloat() );
		if( range > Math::FLT_SMALLEST_NON_DENORMAL )
		{
			distanceContributionScalar = 1.0f - ( ( distanceToTarget - aa_targetFrictionOptimalDistance.GetFloat() ) / range );
		}
	}
	
	return Lerp( aa_targetFrictionRadius.GetFloat(), aa_targetFrictionOptimalRadius.GetFloat(), distanceContributionScalar );
}

/*
========================
budAimAssist::UpdateFriction
========================
*/
void budAimAssist::UpdateFriction( idEntity* pTarget, const Vector3& targetPos )
{

	if( !aa_targetFrictionEnable.GetBool() )
	{
		return;
	}
	
	if( pTarget == NULL )
	{
		return;
	}
	
	Vector3 cameraPos;
	Matrix3 cameraAxis;
	player->GetViewPos( cameraPos, cameraAxis );
	Vector3 dirToTarget = targetPos - cameraPos;
	float  distanceToTarget = dirToTarget.Length();
	Vector3 forward = cameraAxis[0];
	forward.Normalize();
	dirToTarget.Normalize();
	float ViewDirDotTargetDir = Math::ClampFloat( 0.0f, 1.0f, forward * dirToTarget ); // compute the dot and clamp to account for floating point error
	float aimLength = ViewDirDotTargetDir * distanceToTarget;
	Vector3 aimPoint = cameraPos + ( forward * aimLength );
	float delta = Math::Sqrt( Square( distanceToTarget ) - Square( aimLength ) );
	
	const float radius = ComputeFrictionRadius( distanceToTarget );
	if( delta < radius )
	{
		float alpha = 1.0f - ( delta / radius );
		frictionScalar = Lerp( aa_targetFrictionMultiplierMin.GetFloat(), aa_targetFrictionMultiplierMax.GetFloat(), alpha );
	}
}

/*
========================
budAimAssist::ComputeTargetPos
========================
*/
bool budAimAssist::ComputeTargetPos( idEntity* entity, Vector3& primaryTargetPos, Vector3& secondaryTargetPos )
{

	primaryTargetPos = vec3_zero;
	secondaryTargetPos = vec3_zero;
	
	if( entity == NULL )
	{
		return false;
	}
	
	// The target point on actors can now be either the head or the torso
	budActor* actor = NULL;
	if( entity->IsType( budActor::Type ) )
	{
		actor = ( budActor* ) entity;
	}
	if( actor != NULL )
	{
		// Actor AimPoint
		
		Vector3 torsoPos;
		Vector3 headPos = actor->GetEyePosition();
		
		if( actor->GetHeadEntity() != NULL )
		{
			torsoPos = actor->GetHeadEntity()->GetPhysics()->GetOrigin();
		}
		else
		{
			const float offsetScale = 0.9f;
			torsoPos = actor->GetPhysics()->GetOrigin() + ( actor->EyeOffset() * offsetScale );
		}
		
		primaryTargetPos = torsoPos;
		secondaryTargetPos = headPos;
		return true;
		
	}
	else if( entity->GetPhysics()->GetClipModel() != NULL )
	{
	
		const budBounds& box = entity->GetPhysics()->GetClipModel()->GetAbsBounds();
		primaryTargetPos = box.GetCenter();
		secondaryTargetPos = box.GetCenter();
		return true;
	}
	
	return false;
}
