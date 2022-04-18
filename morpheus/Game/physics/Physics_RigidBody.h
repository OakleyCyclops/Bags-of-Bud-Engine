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

#ifndef __PHYSICS_RIGIDBODY_H__
#define __PHYSICS_RIGIDBODY_H__

/*
===================================================================================

	Rigid body physics

	Employs an impulse based dynamic simulation which is not very accurate but
	relatively fast and still reliable due to the continuous collision detection.

===================================================================================
*/

extern const float	RB_VELOCITY_MAX;
extern const int	RB_VELOCITY_TOTAL_BITS;
extern const int	RB_VELOCITY_EXPONENT_BITS;
extern const int	RB_VELOCITY_MANTISSA_BITS;

typedef struct rididBodyIState_s
{
	Vector3					position;					// position of trace model
	Matrix3					orientation;				// orientation of trace model
	Vector3					linearMomentum;				// translational momentum relative to center of mass
	Vector3					angularMomentum;			// rotational momentum relative to center of mass
	
	rididBodyIState_s() :
		position( vec3_zero ),
		orientation( mat3_identity ),
		linearMomentum( vec3_zero ),
		angularMomentum( vec3_zero )
	{
	}
} rigidBodyIState_t;

typedef struct rigidBodyPState_s
{
	int						atRest;						// set when simulation is suspended
	float					lastTimeStep;				// length of last time step
	Vector3					localOrigin;				// origin relative to master
	Matrix3					localAxis;					// axis relative to master
	Vector6					pushVelocity;				// push velocity
	Vector3					externalForce;				// external force relative to center of mass
	Vector3					externalTorque;				// external torque relative to center of mass
	rigidBodyIState_t		i;							// state used for integration
	
	rigidBodyPState_s() :
		atRest( true ),
		lastTimeStep( 0 ),
		localOrigin( vec3_zero ),
		localAxis( mat3_identity ),
		pushVelocity( vec6_zero ),
		externalForce( vec3_zero ),
		externalTorque( vec3_zero )
	{
	}
} rigidBodyPState_t;

class idPhysics_RigidBody : public idPhysics_Base
{

public:

	CLASS_PROTOTYPE( idPhysics_RigidBody );
	
	idPhysics_RigidBody();
	~idPhysics_RigidBody();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	// initialisation
	void					SetFriction( const float linear, const float angular, const float contact );
	void					SetBouncyness( const float b );
	// same as above but drop to the floor first
	void					DropToFloor();
	// no contact determination and contact friction
	void					NoContact();
	// enable/disable activation by impact
	void					EnableImpact();
	void					DisableImpact();
	
public:	// common physics interface
	void					SetClipModel( budClipModel* model, float density, int id = 0, bool freeOld = true );
	budClipModel* 			GetClipModel( int id = 0 ) const;
	int						GetNumClipModels() const;
	
	void					SetMass( float mass, int id = -1 );
	float					GetMass( int id = -1 ) const;
	
	void					SetContents( int contents, int id = -1 );
	int						GetContents( int id = -1 ) const;
	
	const budBounds& 		GetBounds( int id = -1 ) const;
	const budBounds& 		GetAbsBounds( int id = -1 ) const;
	
	bool					Evaluate( int timeStepMSec, int endTimeMSec );
	bool					Interpolate( const float fraction );
	void					ResetInterpolationState( const Vector3& origin, const Matrix3& axis );
	void					UpdateTime( int endTimeMSec );
	int						GetTime() const;
	
	void					GetImpactInfo( const int id, const Vector3& point, impactInfo_t* info ) const;
	void					ApplyImpulse( const int id, const Vector3& point, const Vector3& impulse );
	void					AddForce( const int id, const Vector3& point, const Vector3& force );
	void					Activate();
	void					PutToRest();
	bool					IsAtRest() const;
	int						GetRestStartTime() const;
	bool					IsPushable() const;
	
	void					SaveState();
	void					RestoreState();
	
	void					SetOrigin( const Vector3& newOrigin, int id = -1 );
	void					SetAxis( const Matrix3& newAxis, int id = -1 );
	
	void					Translate( const Vector3& translation, int id = -1 );
	void					Rotate( const Rotation& rotation, int id = -1 );
	
	const Vector3& 			GetOrigin( int id = 0 ) const;
	const Matrix3& 			GetAxis( int id = 0 ) const;
	
	void					SetLinearVelocity( const Vector3& newLinearVelocity, int id = 0 );
	void					SetAngularVelocity( const Vector3& newAngularVelocity, int id = 0 );
	
	const Vector3& 			GetLinearVelocity( int id = 0 ) const;
	const Vector3& 			GetAngularVelocity( int id = 0 ) const;
	
	void					ClipTranslation( trace_t& results, const Vector3& translation, const budClipModel* model ) const;
	void					ClipRotation( trace_t& results, const Rotation& rotation, const budClipModel* model ) const;
	int						ClipContents( const budClipModel* model ) const;
	
	void					DisableClip();
	void					EnableClip();
	
	void					UnlinkClip();
	void					LinkClip();
	
	bool					EvaluateContacts();
	
	void					SetPushed( int deltaTime );
	const Vector3& 			GetPushedLinearVelocity( const int id = 0 ) const;
	const Vector3& 			GetPushedAngularVelocity( const int id = 0 ) const;
	
	void					SetMaster( idEntity* master, const bool orientated );
	
	void					WriteToSnapshot( budBitMsg& msg ) const;
	void					ReadFromSnapshot( const budBitMsg& msg );
	
private:
	// state of the rigid body
	rigidBodyPState_t		current;
	rigidBodyPState_t		saved;
	
	// states for client interpolation
	rigidBodyPState_t		previous;
	rigidBodyPState_t		next;
	
	// rigid body properties
	float					linearFriction;				// translational friction
	float					angularFriction;			// rotational friction
	float					contactFriction;			// friction with contact surfaces
	float					bouncyness;					// bouncyness
	budClipModel* 			clipModel;					// clip model used for collision detection
	
	// derived properties
	float					mass;						// mass of body
	float					inverseMass;				// 1 / mass
	Vector3					centerOfMass;				// center of mass of trace model
	Matrix3					inertiaTensor;				// mass distribution
	Matrix3					inverseInertiaTensor;		// inverse inertia tensor
	
	idODE* 					integrator;					// integrator
	bool					dropToFloor;				// true if dropping to the floor and putting to rest
	bool					testSolid;					// true if testing for solid when dropping to the floor
	bool					noImpact;					// if true do not activate when another object collides
	bool					noContact;					// if true do not determine contacts and no contact friction
	
	// master
	bool					hasMaster;
	bool					isOrientated;
	
private:
	friend void				RigidBodyDerivatives( const float t, const void* clientData, const float* state, float* derivatives );
	void					Integrate( const float deltaTime, rigidBodyPState_t& next );
	bool					CheckForCollisions( const float deltaTime, rigidBodyPState_t& next, trace_t& collision );
	bool					CollisionImpulse( const trace_t& collision, Vector3& impulse );
	void					ContactFriction( float deltaTime );
	void					DropToFloorAndRest();
	bool					TestIfAtRest() const;
	void					Rest();
	void					DebugDraw();
};

#endif /* !__PHYSICS_RIGIDBODY_H__ */
