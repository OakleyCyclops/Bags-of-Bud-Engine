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

#ifndef __PHYSICS_H__
#define __PHYSICS_H__

/*
===============================================================================

	Physics abstract class

	A physics object is a tool to manipulate the position and orientation of
	an entity. The physics object is a container for budClipModels used for
	collision detection. The physics deals with moving these collision models
	through the world according to the laws of physics or other rules.

	The mass of a clip model is the volume of the clip model times the density.
	An arbitrary mass can however be set for specific clip models or the
	whole physics object. The contents of a clip model is a set of bit flags
	that define the contents. The clip mask defines the contents a clip model
	collides with.

	The linear velocity of a physics object is a vector that defines the
	translation of the center of mass in units per second. The angular velocity
	of a physics object is a vector that passes through the center of mass. The
	direction of this vector defines the axis of rotation and the magnitude
	defines the rate of rotation about the axis in radians per second.
	The gravity is the change in velocity per second due to gravitational force.

	Entities update their visual position and orientation from the physics
	using GetOrigin() and GetAxis(). Direct origin and axis changes of
	entities should go through the physics. In other words the physics origin
	and axis are updated first and the entity updates it's visual position
	from the physics.

===============================================================================
*/

#define CONTACT_EPSILON			0.25f				// maximum contact seperation distance

class idEntity;

typedef struct impactInfo_s
{
	float						invMass;			// inverse mass
	budMat3						invInertiaTensor;	// inverse inertia tensor
	budVec3						position;			// impact position relative to center of mass
	budVec3						velocity;			// velocity at the impact position
} impactInfo_t;


class idPhysics : public idClass
{

public:
	ABSTRACT_PROTOTYPE( idPhysics );
	
	virtual						~idPhysics();
	static int					SnapTimeToPhysicsFrame( int t );
	
	// Must not be virtual
	void						Save( idSaveGame* savefile ) const;
	void						Restore( idRestoreGame* savefile );
	
public:	// common physics interface
	// set pointer to entity using physics
	virtual void				SetSelf( idEntity* e ) = 0;
	// clip models
	virtual void				SetClipModel( budClipModel* model, float density, int id = 0, bool freeOld = true ) = 0;
	virtual void				SetClipBox( const budBounds& bounds, float density );
	virtual budClipModel* 		GetClipModel( int id = 0 ) const = 0;
	virtual int					GetNumClipModels() const = 0;
	// get/set the mass of a specific clip model or the whole physics object
	virtual void				SetMass( float mass, int id = -1 ) = 0;
	virtual float				GetMass( int id = -1 ) const = 0;
	// get/set the contents of a specific clip model or the whole physics object
	virtual void				SetContents( int contents, int id = -1 ) = 0;
	virtual int					GetContents( int id = -1 ) const = 0;
	// get/set the contents a specific clip model or the whole physics object collides with
	virtual void				SetClipMask( int mask, int id = -1 ) = 0;
	virtual int					GetClipMask( int id = -1 ) const = 0;
	// get the bounds of a specific clip model or the whole physics object
	virtual const budBounds& 	GetBounds( int id = -1 ) const = 0;
	virtual const budBounds& 	GetAbsBounds( int id = -1 ) const = 0;
	// evaluate the physics with the given time step, returns true if the object moved
	virtual bool				Evaluate( int timeStepMSec, int endTimeMSec ) = 0;
	// Interpolate between the two known snapshots with the given fraction, used for MP clients.
	// returns true if the object moved.
	virtual bool				Interpolate( const float fraction ) = 0;
	// resets the prev and next states to the parameters.
	virtual void				ResetInterpolationState( const budVec3& origin, const budMat3& axis ) = 0;
	// update the time without moving
	virtual void				UpdateTime( int endTimeMSec ) = 0;
	// get the last physics update time
	virtual int					GetTime() const = 0;
	// collision interaction between different physics objects
	virtual void				GetImpactInfo( const int id, const budVec3& point, impactInfo_t* info ) const = 0;
	virtual void				ApplyImpulse( const int id, const budVec3& point, const budVec3& impulse ) = 0;
	virtual void				AddForce( const int id, const budVec3& point, const budVec3& force ) = 0;
	virtual void				Activate() = 0;
	virtual void				PutToRest() = 0;
	virtual bool				IsAtRest() const = 0;
	virtual int					GetRestStartTime() const = 0;
	virtual bool				IsPushable() const = 0;
	// save and restore the physics state
	virtual void				SaveState() = 0;
	virtual void				RestoreState() = 0;
	// set the position and orientation in master space or world space if no master set
	virtual void				SetOrigin( const budVec3& newOrigin, int id = -1 ) = 0;
	virtual void				SetAxis( const budMat3& newAxis, int id = -1 ) = 0;
	// translate or rotate the physics object in world space
	virtual void				Translate( const budVec3& translation, int id = -1 ) = 0;
	virtual void				Rotate( const budRotation& rotation, int id = -1 ) = 0;
	// get the position and orientation in world space
	virtual const budVec3& 		GetOrigin( int id = 0 ) const = 0;
	virtual const budMat3& 		GetAxis( int id = 0 ) const = 0;
	// set linear and angular velocity
	virtual void				SetLinearVelocity( const budVec3& newLinearVelocity, int id = 0 ) = 0;
	virtual void				SetAngularVelocity( const budVec3& newAngularVelocity, int id = 0 ) = 0;
	// get linear and angular velocity
	virtual const budVec3& 		GetLinearVelocity( int id = 0 ) const = 0;
	virtual const budVec3& 		GetAngularVelocity( int id = 0 ) const = 0;
	// gravity
	virtual void				SetGravity( const budVec3& newGravity ) = 0;
	virtual const budVec3& 		GetGravity() const = 0;
	virtual const budVec3& 		GetGravityNormal() const = 0;
	// get first collision when translating or rotating this physics object
	virtual void				ClipTranslation( trace_t& results, const budVec3& translation, const budClipModel* model ) const = 0;
	virtual void				ClipRotation( trace_t& results, const budRotation& rotation, const budClipModel* model ) const = 0;
	virtual int					ClipContents( const budClipModel* model ) const = 0;
	// disable/enable the clip models contained by this physics object
	virtual void				DisableClip() = 0;
	virtual void				EnableClip() = 0;
	// link/unlink the clip models contained by this physics object
	virtual void				UnlinkClip() = 0;
	virtual void				LinkClip() = 0;
	// contacts
	virtual bool				EvaluateContacts() = 0;
	virtual int					GetNumContacts() const = 0;
	virtual const contactInfo_t& GetContact( int num ) const = 0;
	virtual void				ClearContacts() = 0;
	virtual void				AddContactEntity( idEntity* e ) = 0;
	virtual void 				RemoveContactEntity( idEntity* e ) = 0;
	// ground contacts
	virtual bool				HasGroundContacts() const = 0;
	virtual bool				IsGroundEntity( int entityNum ) const = 0;
	virtual bool				IsGroundClipModel( int entityNum, int id ) const = 0;
	// set the master entity for objects bound to a master
	virtual void				SetMaster( idEntity* master, const bool orientated = true ) = 0;
	// set pushed state
	virtual void				SetPushed( int deltaTime ) = 0;
	virtual const budVec3& 		GetPushedLinearVelocity( const int id = 0 ) const = 0;
	virtual const budVec3& 		GetPushedAngularVelocity( const int id = 0 ) const = 0;
	// get blocking info, returns NULL if the object is not blocked
	virtual const trace_t* 		GetBlockingInfo() const = 0;
	virtual idEntity* 			GetBlockingEntity() const = 0;
	// movement end times in msec for reached events at the end of predefined motion
	virtual int					GetLinearEndTime() const = 0;
	virtual int					GetAngularEndTime() const = 0;
	// networking
	virtual void				WriteToSnapshot( budBitMsg& msg ) const = 0;
	virtual void				ReadFromSnapshot( const budBitMsg& msg ) = 0;
};

#endif /* !__PHYSICS_H__ */