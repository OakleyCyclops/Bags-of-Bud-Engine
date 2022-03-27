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

#ifndef __GAME_AFENTITY_H__
#define __GAME_AFENTITY_H__


/*
===============================================================================

idMultiModelAF

Entity using multiple separate visual models animated with a single
articulated figure. Only used for debugging!

===============================================================================
*/
const int GIB_DELAY = 200;  // only gib this often to keep performace hits when blowing up several mobs

class idMultiModelAF : public idEntity
{
public:
	CLASS_PROTOTYPE( idMultiModelAF );
	
	void					Spawn();
	~idMultiModelAF();
	
	virtual void			Think();
	virtual void			Present();
	
protected:
	idPhysics_AF			physicsObj;
	
	void					SetModelForId( int id, const budStr& modelName );
	
private:
	budList<budRenderModel*, TAG_AF>	modelHandles;
	budList<int, TAG_AF>				modelDefHandles;
};


/*
===============================================================================

idChain

Chain hanging down from the ceiling. Only used for debugging!

===============================================================================
*/

class idChain : public idMultiModelAF
{
public:
	CLASS_PROTOTYPE( idChain );
	
	void					Spawn();
	
protected:
	void					BuildChain( const budStr& name, const budVec3& origin, float linkLength, float linkWidth, float density, int numLinks, bool bindToWorld = true );
};


/*
===============================================================================

budAFAttachment

===============================================================================
*/

class budAFAttachment : public budAnimatedEntity
{
public:
	CLASS_PROTOTYPE( budAFAttachment );
	
	budAFAttachment();
	virtual					~budAFAttachment();
	
	void					Spawn();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	void					SetBody( idEntity* bodyEnt, const char* headModel, jointHandle_t attachJoint );
	void					ClearBody();
	idEntity* 				GetBody() const;
	
	virtual void			Think();
	
	virtual void			Hide();
	virtual void			Show();
	
	void					PlayIdleAnim( int blendTime );
	
	virtual void			GetImpactInfo( idEntity* ent, int id, const budVec3& point, impactInfo_t* info );
	virtual void			ApplyImpulse( idEntity* ent, int id, const budVec3& point, const budVec3& impulse );
	virtual void			AddForce( idEntity* ent, int id, const budVec3& point, const budVec3& force );
	
	virtual	void			Damage( idEntity* inflictor, idEntity* attacker, const budVec3& dir, const char* damageDefName, const float damageScale, const int location );
	virtual void			AddDamageEffect( const trace_t& collision, const budVec3& velocity, const char* damageDefName );
	
	void					SetCombatModel();
	budClipModel* 			GetCombatModel() const;
	virtual void			LinkCombat();
	virtual void			UnlinkCombat();
	
protected:
	idEntity* 				body;
	budClipModel* 			combatModel;	// render model for hit detection of head
	int						idleAnim;
	jointHandle_t			attachJoint;
};


/*
===============================================================================

budAFEntity_Base

===============================================================================
*/

class budAFEntity_Base : public budAnimatedEntity
{
public:
	CLASS_PROTOTYPE( budAFEntity_Base );
	
	budAFEntity_Base();
	virtual					~budAFEntity_Base();
	
	void					Spawn();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	virtual void			Think();
	virtual void			AddDamageEffect( const trace_t& collision, const budVec3& velocity, const char* damageDefName );
	virtual void			GetImpactInfo( idEntity* ent, int id, const budVec3& point, impactInfo_t* info );
	virtual void			ApplyImpulse( idEntity* ent, int id, const budVec3& point, const budVec3& impulse );
	virtual void			AddForce( idEntity* ent, int id, const budVec3& point, const budVec3& force );
	virtual bool			Collide( const trace_t& collision, const budVec3& velocity );
	virtual bool			GetPhysicsToVisualTransform( budVec3& origin, budMat3& axis );
	virtual bool			UpdateAnimationControllers();
	virtual void			FreeModelDef();
	
	virtual bool			LoadAF();
	bool					IsActiveAF() const
	{
		return af.IsActive();
	}
	const char* 			GetAFName() const
	{
		return af.GetName();
	}
	idPhysics_AF* 			GetAFPhysics()
	{
		return af.GetPhysics();
	}
	
	void					SetCombatModel();
	budClipModel* 			GetCombatModel() const;
	// contents of combatModel can be set to 0 or re-enabled (mp)
	void					SetCombatContents( bool enable );
	virtual void			LinkCombat();
	virtual void			UnlinkCombat();
	
	int						BodyForClipModelId( int id ) const;
	
	void					SaveState( idDict& args ) const;
	void					LoadState( const idDict& args );
	
	void					AddBindConstraints();
	void					RemoveBindConstraints();
	
	virtual void			ShowEditingDialog();
	
	static void				DropAFs( idEntity* ent, const char* type, budList<idEntity*>* list );
	
protected:
	budAF					af;				// articulated figure
	budClipModel* 			combatModel;	// render model for hit detection
	int						combatModelContents;
	budVec3					spawnOrigin;	// spawn origin
	budMat3					spawnAxis;		// rotation axis used when spawned
	int						nextSoundTime;	// next time this can make a sound
	
	void					Event_SetConstraintPosition( const char* name, const budVec3& pos );
};

/*
===============================================================================

budAFEntity_Gibbable

===============================================================================
*/

extern const idEventDef		EV_Gib;
extern const idEventDef		EV_Gibbed;

class budAFEntity_Gibbable : public budAFEntity_Base
{
public:
	CLASS_PROTOTYPE( budAFEntity_Gibbable );
	
	budAFEntity_Gibbable();
	~budAFEntity_Gibbable();
	
	void					Spawn();
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	virtual void			Present();
	virtual	void			Damage( idEntity* inflictor, idEntity* attacker, const budVec3& dir, const char* damageDefName, const float damageScale, const int location );
	void					SetThrown( bool isThrown );
	virtual bool			Collide( const trace_t& collision, const budVec3& velocity );
	virtual void			SpawnGibs( const budVec3& dir, const char* damageDefName );
	
	bool					IsGibbed()
	{
		return gibbed;
	};
	
protected:
	budRenderModel* 			skeletonModel;
	int						skeletonModelDefHandle;
	bool					gibbed;
	
	bool					wasThrown;
	
	virtual void			Gib( const budVec3& dir, const char* damageDefName );
	void					InitSkeletonModel();
	
	void					Event_Gib( const char* damageDefName );
};

/*
===============================================================================

	budAFEntity_Generic

===============================================================================
*/

class budAFEntity_Generic : public budAFEntity_Gibbable
{
public:
	CLASS_PROTOTYPE( budAFEntity_Generic );
	
	budAFEntity_Generic();
	~budAFEntity_Generic();
	
	void					Spawn();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	virtual void			Think();
	void					KeepRunningPhysics()
	{
		keepRunningPhysics = true;
	}
	
private:
	void					Event_Activate( idEntity* activator );
	
	bool					keepRunningPhysics;
};


/*
===============================================================================

budAFEntity_WithAttachedHead

===============================================================================
*/

class budAFEntity_WithAttachedHead : public budAFEntity_Gibbable
{
public:
	CLASS_PROTOTYPE( budAFEntity_WithAttachedHead );
	
	budAFEntity_WithAttachedHead();
	~budAFEntity_WithAttachedHead();
	
	void					Spawn();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	void					SetupHead();
	
	virtual void			Think();
	
	virtual void			Hide();
	virtual void			Show();
	virtual void			ProjectOverlay( const budVec3& origin, const budVec3& dir, float size, const char* material );
	
	virtual void			LinkCombat();
	virtual void			UnlinkCombat();
	
protected:
	virtual void			Gib( const budVec3& dir, const char* damageDefName );
	
public:
	idEntityPtr<budAFAttachment>	head;
	
	void					Event_Gib( const char* damageDefName );
	void					Event_Activate( idEntity* activator );
};


/*
===============================================================================

budAFEntity_Vehicle

===============================================================================
*/

class budAFEntity_Vehicle : public budAFEntity_Base
{
public:
	CLASS_PROTOTYPE( budAFEntity_Vehicle );
	
	budAFEntity_Vehicle();
	
	void					Spawn();
	void					Use( budPlayer* player );
	
protected:
	budPlayer* 				player;
	jointHandle_t			eyesJoint;
	jointHandle_t			steeringWheelJoint;
	float					wheelRadius;
	float					steerAngle;
	float					steerSpeed;
	const budDeclParticle* 	dustSmoke;
	
	float					GetSteerAngle();
};


/*
===============================================================================

budAFEntity_VehicleSimple

===============================================================================
*/

class budAFEntity_VehicleSimple : public budAFEntity_Vehicle
{
public:
	CLASS_PROTOTYPE( budAFEntity_VehicleSimple );
	
	budAFEntity_VehicleSimple();
	~budAFEntity_VehicleSimple();
	
	void					Spawn();
	virtual void			Think();
	
protected:
	budClipModel* 			wheelModel;
	budAFConstraint_Suspension* 	suspension[4];
	jointHandle_t			wheelJoints[4];
	float					wheelAngles[4];
};


/*
===============================================================================

budAFEntity_VehicleFourWheels

===============================================================================
*/

class budAFEntity_VehicleFourWheels : public budAFEntity_Vehicle
{
public:
	CLASS_PROTOTYPE( budAFEntity_VehicleFourWheels );
	
	budAFEntity_VehicleFourWheels();
	
	void					Spawn();
	virtual void			Think();
	
protected:
	budAFBody* 				wheels[4];
	budAFConstraint_Hinge* 	steering[2];
	jointHandle_t			wheelJoints[4];
	float					wheelAngles[4];
};


/*
===============================================================================

budAFEntity_VehicleSixWheels

===============================================================================
*/

class budAFEntity_VehicleSixWheels : public budAFEntity_Vehicle
{
public:
	CLASS_PROTOTYPE( budAFEntity_VehicleSixWheels );
	
	budAFEntity_VehicleSixWheels();
	
	void					Spawn();
	virtual void			Think();
	
	float					force;
	float					velocity;
	float					steerAngle;
	
private:
	budAFBody* 				wheels[6];
	budAFConstraint_Hinge* 	steering[4];
	jointHandle_t			wheelJoints[6];
	float					wheelAngles[6];
};

/*
===============================================================================

budAFEntity_VehicleAutomated

===============================================================================
*/

class budAFEntity_VehicleAutomated : public budAFEntity_VehicleSixWheels
{
public:
	CLASS_PROTOTYPE( budAFEntity_VehicleAutomated );
	
	void					Spawn();
	void					PostSpawn();
	virtual void			Think();
	
private:

	idEntity*	waypoint;
	float		steeringSpeed;
	float		currentSteering;
	float		idealSteering;
	float		originHeight;
	
	void		Event_SetVelocity( float _velocity );
	void		Event_SetTorque( float _torque );
	void		Event_SetSteeringSpeed( float _steeringSpeed );
	void		Event_SetWayPoint( idEntity* _waypoint );
};

/*
===============================================================================

budAFEntity_SteamPipe

===============================================================================
*/

class budAFEntity_SteamPipe : public budAFEntity_Base
{
public:
	CLASS_PROTOTYPE( budAFEntity_SteamPipe );
	
	budAFEntity_SteamPipe();
	~budAFEntity_SteamPipe();
	
	void					Spawn();
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	virtual void			Think();
	
private:
	int						steamBody;
	float					steamForce;
	float					steamUpForce;
	idForce_Constant		force;
	renderEntity_t			steamRenderEntity;
	qhandle_t				steamModelDefHandle;
	
	void					InitSteamRenderEntity();
};


/*
===============================================================================

budAFEntity_ClawFourFingers

===============================================================================
*/

class budAFEntity_ClawFourFingers : public budAFEntity_Base
{
public:
	CLASS_PROTOTYPE( budAFEntity_ClawFourFingers );
	
	budAFEntity_ClawFourFingers();
	
	void					Spawn();
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
private:
	budAFConstraint_Hinge* 	fingers[4];
	
	void					Event_SetFingerAngle( float angle );
	void					Event_StopFingers();
};


/**
* idHarvestable contains all of the code required to turn an entity into a harvestable
* entity. The entity must create an instance of this class and call the appropriate
* interface methods at the correct time.
*/
class idHarvestable : public idEntity
{
public:
	CLASS_PROTOTYPE( idHarvestable );
	
	idHarvestable();
	~idHarvestable();
	
	void				Spawn();
	void				Init( idEntity* parent );
	void				Save( idSaveGame* savefile ) const;
	void				Restore( idRestoreGame* savefile );
	
	void				SetParent( idEntity* parent );
	
	void				Think();
	void				Gib();
	
protected:
	idEntityPtr<idEntity>	parentEnt;
	float					triggersize;
	budClipModel* 			trigger;
	float					giveDelay;
	float					removeDelay;
	bool					given;
	
	idEntityPtr<budPlayer>	player;
	int						startTime;
	
	bool					fxFollowPlayer;
	idEntityPtr<idEntityFx>	fx;
	budStr					fxOrient;
	
protected:
	void					BeginBurn();
	void					BeginFX();
	void					CalcTriggerBounds( float size, budBounds& bounds );
	
	bool					GetFxOrientationAxis( budMat3& mat );
	
	void					Event_SpawnHarvestTrigger();
	void					Event_Touch( idEntity* other, trace_t* trace );
} ;


/*
===============================================================================

budAFEntity_Harvest

===============================================================================
*/



class budAFEntity_Harvest : public budAFEntity_WithAttachedHead
{
public:
	CLASS_PROTOTYPE( budAFEntity_Harvest );
	
	budAFEntity_Harvest();
	~budAFEntity_Harvest();
	
	void					Spawn();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	virtual void			Think();
	
	virtual void			Gib( const budVec3& dir, const char* damageDefName );
	
protected:
	idEntityPtr<idHarvestable>	harvestEnt;
protected:
	void					Event_SpawnHarvestEntity();
	
};

#endif /* !__GAME_AFENTITY_H__ */
