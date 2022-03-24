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

#ifndef __GAME_MOVEABLE_H__
#define __GAME_MOVEABLE_H__

/*
===============================================================================

  Entity using rigid body physics.

===============================================================================
*/

extern const idEventDef EV_BecomeNonSolid;
extern const idEventDef EV_IsAtRest;

class idMoveable : public idEntity
{
public:
	CLASS_PROTOTYPE( idMoveable );
	
	idMoveable();
	~idMoveable();
	
	void					Spawn();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	virtual void			Think();
	virtual void			ClientThink( const int curTime, const float fraction, const bool predict );
	virtual void			Hide();
	virtual void			Show();
	
	bool					AllowStep() const;
	void					EnableDamage( bool enable, float duration );
	virtual bool			Collide( const trace_t& collision, const budVec3& velocity );
	virtual void			Killed( idEntity* inflictor, idEntity* attacker, int damage, const budVec3& dir, int location );
	virtual void			WriteToSnapshot( budBitMsg& msg ) const;
	virtual void			ReadFromSnapshot( const budBitMsg& msg );
	
	void					SetAttacker( idEntity* ent );
	const idEntity* 		GetAttacker()
	{
		return attacker;
	}
	
protected:
	idPhysics_RigidBody		physicsObj;				// physics object
	budStr					brokenModel;			// model set when health drops down to or below zero
	budStr					damage;					// if > 0 apply damage to hit entities
	budStr					monsterDamage;
	idEntity*				attacker;
	budStr					fxCollide;				// fx system to start when collides with something
	int						nextCollideFxTime;		// next time it is ok to spawn collision fx
	float					minDamageVelocity;		// minimum velocity before moveable applies damage
	float					maxDamageVelocity;		// velocity at which the maximum damage is applied
	idCurve_Spline<budVec3>* initialSpline;			// initial spline path the moveable follows
	budVec3					initialSplineDir;		// initial relative direction along the spline path
	bool					explode;				// entity explodes when health drops down to or below zero
	bool					unbindOnDeath;			// unbind from master when health drops down to or below zero
	bool					allowStep;				// allow monsters to step on the object
	bool					canDamage;				// only apply damage when this is set
	int						nextDamageTime;			// next time the movable can hurt the player
	int						nextSoundTime;			// next time the moveable can make a sound
	
	const budMaterial* 		GetRenderModelMaterial() const;
	void					BecomeNonSolid();
	void					InitInitialSpline( int startTime );
	bool					FollowInitialSplinePath();
	
	void					Event_Activate( idEntity* activator );
	void					Event_BecomeNonSolid();
	void					Event_SetOwnerFromSpawnArgs();
	void					Event_IsAtRest();
	void					Event_EnableDamage( float enable );
};


/*
===============================================================================

  A barrel using rigid body physics. The barrel has special handling of
  the view model orientation to make it look like it rolls instead of slides.

===============================================================================
*/

class idBarrel : public idMoveable
{

public:
	CLASS_PROTOTYPE( idBarrel );
	idBarrel();
	
	void					Spawn();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	void					BarrelThink();
	virtual void			Think();
	virtual bool			GetPhysicsToVisualTransform( budVec3& origin, budMat3& axis );
	virtual void			ClientThink( const int curTime, const float fraction, const bool predict );
	
private:
	float					radius;					// radius of barrel
	int						barrelAxis;				// one of the coordinate axes the barrel cylinder is parallel to
	budVec3					lastOrigin;				// origin of the barrel the last think frame
	budMat3					lastAxis;				// axis of the barrel the last think frame
	float					additionalRotation;		// additional rotation of the barrel about it's axis
	budMat3					additionalAxis;			// additional rotation axis
};


/*
===============================================================================

  A barrel using rigid body physics and special handling of the view model
  orientation to make it look like it rolls instead of slides. The barrel
  can burn and explode when damaged.

===============================================================================
*/

class idExplodingBarrel : public idBarrel
{
public:
	CLASS_PROTOTYPE( idExplodingBarrel );
	
	idExplodingBarrel();
	~idExplodingBarrel();
	
	void					Spawn();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	bool					IsStable();
	void					SetStability( bool stability );
	void					StartBurning();
	void					StopBurning();
	
	virtual void			ClientThink( const int curTime, const float fraction, const bool predict );
	virtual void			Think();
	virtual void			Damage( idEntity* inflictor, idEntity* attacker, const budVec3& dir,
									const char* damageDefName, const float damageScale, const int location );
	virtual void			Killed( idEntity* inflictor, idEntity* attacker, int damage, const budVec3& dir, int location );
	
	virtual void			WriteToSnapshot( budBitMsg& msg ) const;
	virtual void			ReadFromSnapshot( const budBitMsg& msg );
	virtual bool			ClientReceiveEvent( int event, int time, const budBitMsg& msg );
	
	enum
	{
		EVENT_EXPLODE = idEntity::EVENT_MAXEVENTS,
		EVENT_MAXEVENTS
	};
	
private:
	typedef enum
	{
		NORMAL = 0,
		BURNING,
		BURNEXPIRED,
		EXPLODING
	} explode_state_t;
	explode_state_t			state;
	
	budVec3					spawnOrigin;
	budMat3					spawnAxis;
	qhandle_t				particleModelDefHandle;
	qhandle_t				lightDefHandle;
	renderEntity_t			particleRenderEntity;
	renderLight_t			light;
	int						particleTime;
	int						lightTime;
	float					time;
	bool					isStable;
	
	void					AddParticles( const char* name, bool burn );
	void					AddLight( const char* name , bool burn );
	void					ExplodingEffects();
	void					UpdateLight();
	
	void					Event_Activate( idEntity* activator );
	void					Event_Respawn();
	void					Event_Explode();
	void					Event_TriggerTargets();
};

#endif /* !__GAME_MOVEABLE_H__ */