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

#ifndef __PHYSICS_STATIC_H__
#define __PHYSICS_STATIC_H__

/*
===============================================================================

	Physics for a non moving object using at most one collision model.

===============================================================================
*/

class budBitMsg;

typedef struct staticPState_s
{
	Vector3					origin;
	Matrix3					axis;
	Vector3					localOrigin;
	Matrix3					localAxis;
} staticPState_t;

// Storing the state used for interpolation with quaternions
// means I don't have to do a bunch of conversions between
// Matrix3s and Quats every frame.
struct staticInterpolatePState_t
{
	Vector3					origin;
	Quat					axis;
	Vector3					localOrigin;
	Quat					localAxis;
};

/*
================
ReadStaticInterpolatePStateFromSnapshot
================
*/
staticInterpolatePState_t ReadStaticInterpolatePStateFromSnapshot( const budBitMsg& msg );
staticPState_s	ConvertInterpolateStateToPState( const staticInterpolatePState_t& interpolateState );
staticInterpolatePState_t ConvertPStateToInterpolateState( const staticPState_t& state );

class idPhysics_Static : public idPhysics
{

public:
	CLASS_PROTOTYPE( idPhysics_Static );
	
	idPhysics_Static();
	~idPhysics_Static();
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
public:	// common physics interface
	void					SetSelf( idEntity* e );
	
	void					SetClipModel( budClipModel* model, float density, int id = 0, bool freeOld = true );
	budClipModel* 			GetClipModel( int id = 0 ) const;
	int						GetNumClipModels() const;
	
	void					SetMass( float mass, int id = -1 );
	float					GetMass( int id = -1 ) const;
	
	void					SetContents( int contents, int id = -1 );
	int						GetContents( int id = -1 ) const;
	
	void					SetClipMask( int mask, int id = -1 );
	int						GetClipMask( int id = -1 ) const;
	
	const budBounds& 		GetBounds( int id = -1 ) const;
	const budBounds& 		GetAbsBounds( int id = -1 ) const;
	
	bool					Evaluate( int timeStepMSec, int endTimeMSec );
	bool					Interpolate( const float fraction );
	void					ResetInterpolationState( const Vector3& origin, const Matrix3& axis ) {}
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
	
	void					SetGravity( const Vector3& newGravity );
	const Vector3& 			GetGravity() const;
	const Vector3& 			GetGravityNormal() const;
	
	void					ClipTranslation( trace_t& results, const Vector3& translation, const budClipModel* model ) const;
	void					ClipRotation( trace_t& results, const Rotation& rotation, const budClipModel* model ) const;
	int						ClipContents( const budClipModel* model ) const;
	
	void					DisableClip();
	void					EnableClip();
	
	void					UnlinkClip();
	void					LinkClip();
	
	bool					EvaluateContacts();
	int						GetNumContacts() const;
	const contactInfo_t& 	GetContact( int num ) const;
	void					ClearContacts();
	void					AddContactEntity( idEntity* e );
	void					RemoveContactEntity( idEntity* e );
	
	bool					HasGroundContacts() const;
	bool					IsGroundEntity( int entityNum ) const;
	bool					IsGroundClipModel( int entityNum, int id ) const;
	
	void					SetPushed( int deltaTime );
	const Vector3& 			GetPushedLinearVelocity( const int id = 0 ) const;
	const Vector3& 			GetPushedAngularVelocity( const int id = 0 ) const;
	
	void					SetMaster( idEntity* master, const bool orientated = true );
	
	const trace_t* 			GetBlockingInfo() const;
	idEntity* 				GetBlockingEntity() const;
	
	int						GetLinearEndTime() const;
	int						GetAngularEndTime() const;
	
	void					WriteToSnapshot( budBitMsg& msg ) const;
	void					ReadFromSnapshot( const budBitMsg& msg );
	
protected:
	idEntity* 				self;					// entity using this physics object
	staticPState_t			current;				// physics state
	budClipModel* 			clipModel;				// collision model
	
	// Used for client-side interpolation
	staticInterpolatePState_t	previous;
	staticInterpolatePState_t	next;
	
	// master
	bool					hasMaster;
	bool					isOrientated;
};

staticPState_t InterpolateStaticPState( const staticInterpolatePState_t& previous,
										const staticInterpolatePState_t& next,
										float fraction );

#endif /* !__PHYSICS_STATIC_H__ */
