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

CLASS_DECLARATION( idPhysics_Base, idPhysics_AF )
END_CLASS

const float ERROR_REDUCTION					= 0.5f;
const float ERROR_REDUCTION_MAX				= 256.0f;
const float LIMIT_ERROR_REDUCTION			= 0.3f;
const float LCP_EPSILON						= 1e-7f;
const float LIMIT_LCP_EPSILON				= 1e-4f;
const float CONTACT_LCP_EPSILON				= 1e-6f;
const float CENTER_OF_MASS_EPSILON			= 1e-4f;
const float NO_MOVE_TIME					= 1.0f;
const float NO_MOVE_TRANSLATION_TOLERANCE	= 10.0f;
const float NO_MOVE_ROTATION_TOLERANCE		= 10.0f;
const float MIN_MOVE_TIME					= -1.0f;
const float MAX_MOVE_TIME					= -1.0f;
const float IMPULSE_THRESHOLD				= 500.0f;
const float SUSPEND_LINEAR_VELOCITY			= 10.0f;
const float SUSPEND_ANGULAR_VELOCITY		= 15.0f;
const float SUSPEND_LINEAR_ACCELERATION		= 20.0f;
const float SUSPEND_ANGULAR_ACCELERATION	= 30.0f;
const budVec6 vec6_lcp_epsilon				= budVec6( LCP_EPSILON, LCP_EPSILON, LCP_EPSILON,
		LCP_EPSILON, LCP_EPSILON, LCP_EPSILON );

#define AF_TIMINGS

#ifdef AF_TIMINGS
static int lastTimerReset = 0;
static int numArticulatedFigures = 0;
static budTimer timer_total, timer_pc, timer_ac, timer_collision, timer_lcp;
#endif



//===============================================================
//
//	budAFConstraint
//
//===============================================================

/*
================
budAFConstraint::budAFConstraint
================
*/
budAFConstraint::budAFConstraint()
{
	type				= CONSTRAINT_INVALID;
	name				= "noname";
	body1				= NULL;
	body2				= NULL;
	physics				= NULL;
	
	lo.Zero( 6 );
	lo.SubVec6( 0 )		= -vec6_infinity;
	hi.Zero( 6 );
	hi.SubVec6( 0 )		= vec6_infinity;
	e.SetSize( 6 );
	e.SubVec6( 0 )		= vec6_lcp_epsilon;
	
	boxConstraint		= NULL;
	boxIndex[0]			= -1;
	boxIndex[1]			= -1;
	boxIndex[2]			= -1;
	boxIndex[3]			= -1;
	boxIndex[4]			= -1;
	boxIndex[5]			= -1;
	
	firstIndex			= 0;
	
	memset( &fl, 0, sizeof( fl ) );
}

/*
================
budAFConstraint::~budAFConstraint
================
*/
budAFConstraint::~budAFConstraint()
{
}

/*
================
budAFConstraint::SetBody1
================
*/
void budAFConstraint::SetBody1( budAFBody* body )
{
	if( body1 != body )
	{
		body1 = body;
		if( physics )
		{
			physics->SetChanged();
		}
	}
}

/*
================
budAFConstraint::SetBody2
================
*/
void budAFConstraint::SetBody2( budAFBody* body )
{
	if( body2 != body )
	{
		body2 = body;
		if( physics )
		{
			physics->SetChanged();
		}
	}
}

/*
================
budAFConstraint::GetMultiplier
================
*/
const budVecX& budAFConstraint::GetMultiplier()
{
	return lm;
}

/*
================
budAFConstraint::Evaluate
================
*/
void budAFConstraint::Evaluate( float invTimeStep )
{
	assert( 0 );
}

/*
================
budAFConstraint::ApplyFriction
================
*/
void budAFConstraint::ApplyFriction( float invTimeStep )
{
}

/*
================
budAFConstraint::GetForce
================
*/
void budAFConstraint::GetForce( budAFBody* body, budVec6& force )
{
	budVecX v;
	
	v.SetData( 6, VECX_ALLOCA( 6 ) );
	if( body == body1 )
	{
		J1.TransposeMultiply( v, lm );
	}
	else if( body == body2 )
	{
		J2.TransposeMultiply( v, lm );
	}
	else
	{
		v.Zero();
	}
	force[0] = v[0];
	force[1] = v[1];
	force[2] = v[2];
	force[3] = v[3];
	force[4] = v[4];
	force[5] = v[5];
}

/*
================
budAFConstraint::Translate
================
*/
void budAFConstraint::Translate( const budVec3& translation )
{
	assert( 0 );
}

/*
================
budAFConstraint::Rotate
================
*/
void budAFConstraint::Rotate( const budRotation& rotation )
{
	assert( 0 );
}

/*
================
budAFConstraint::GetCenter
================
*/
void budAFConstraint::GetCenter( budVec3& center )
{
	center.Zero();
}

/*
================
budAFConstraint::DebugDraw
================
*/
void budAFConstraint::DebugDraw()
{
}

/*
================
budAFConstraint::InitSize
================
*/
void budAFConstraint::InitSize( int size )
{
	J1.Zero( size, 6 );
	J2.Zero( size, 6 );
	c1.Zero( size );
	c2.Zero( size );
	s.Zero( size );
	lm.Zero( size );
}

/*
================
budAFConstraint::Save
================
*/
void budAFConstraint::Save( idSaveGame* saveFile ) const
{
	saveFile->WriteInt( type );
}

/*
================
budAFConstraint::Restore
================
*/
void budAFConstraint::Restore( idRestoreGame* saveFile )
{
	constraintType_t t;
	saveFile->ReadInt( ( int& )t );
	assert( t == type );
}


//===============================================================
//
//	budAFConstraint_Fixed
//
//===============================================================

/*
================
budAFConstraint_Fixed::budAFConstraint_Fixed
================
*/
budAFConstraint_Fixed::budAFConstraint_Fixed( const budStr& name, budAFBody* body1, budAFBody* body2 )
{
	assert( body1 );
	type = CONSTRAINT_FIXED;
	this->name = name;
	this->body1 = body1;
	this->body2 = body2;
	InitSize( 6 );
	fl.allowPrimary = true;
	fl.noCollision = true;
	
	InitOffset();
}

/*
================
budAFConstraint_Fixed::InitOffset
================
*/
void budAFConstraint_Fixed::InitOffset()
{
	if( body2 )
	{
		offset = ( body1->GetWorldOrigin() - body2->GetWorldOrigin() ) * body2->GetWorldAxis().Transpose();
		relAxis = body1->GetWorldAxis() * body2->GetWorldAxis().Transpose();
	}
	else
	{
		offset = body1->GetWorldOrigin();
		relAxis = body1->GetWorldAxis();
	}
}

/*
================
budAFConstraint_Fixed::SetBody1
================
*/
void budAFConstraint_Fixed::SetBody1( budAFBody* body )
{
	if( body1 != body )
	{
		body1 = body;
		InitOffset();
		if( physics )
		{
			physics->SetChanged();
		}
	}
}

/*
================
budAFConstraint_Fixed::SetBody2
================
*/
void budAFConstraint_Fixed::SetBody2( budAFBody* body )
{
	if( body2 != body )
	{
		body2 = body;
		InitOffset();
		if( physics )
		{
			physics->SetChanged();
		}
	}
}

/*
================
budAFConstraint_Fixed::Evaluate
================
*/
void budAFConstraint_Fixed::Evaluate( float invTimeStep )
{
	budVec3 ofs, a2;
	budMat3 ax;
	budRotation r;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	if( master )
	{
		a2 = offset * master->GetWorldAxis();
		ofs = a2 + master->GetWorldOrigin();
		ax = relAxis * master->GetWorldAxis();
	}
	else
	{
		a2.Zero();
		ofs = offset;
		ax = relAxis;
	}
	
	J1.Set(	mat3_identity, mat3_zero,
			mat3_zero, mat3_identity );
			
	if( body2 )
	{
		J2.Set(	-mat3_identity, SkewSymmetric( a2 ),
				mat3_zero, -mat3_identity );
	}
	else
	{
		J2.Zero( 6, 6 );
	}
	
	c1.SubVec3( 0 ) = -( invTimeStep * ERROR_REDUCTION ) * ( ofs - body1->GetWorldOrigin() );
	r = ( body1->GetWorldAxis().Transpose() * ax ).ToRotation();
	c1.SubVec3( 1 ) = -( invTimeStep * ERROR_REDUCTION ) * ( r.GetVec() * -( float ) DEG2RAD( r.GetAngle() ) );
	
	c1.Clamp( -ERROR_REDUCTION_MAX, ERROR_REDUCTION_MAX );
}

/*
================
budAFConstraint_Fixed::ApplyFriction
================
*/
void budAFConstraint_Fixed::ApplyFriction( float invTimeStep )
{
	// no friction
}

/*
================
budAFConstraint_Fixed::Translate
================
*/
void budAFConstraint_Fixed::Translate( const budVec3& translation )
{
	if( !body2 )
	{
		offset += translation;
	}
}

/*
================
budAFConstraint_Fixed::Rotate
================
*/
void budAFConstraint_Fixed::Rotate( const budRotation& rotation )
{
	if( !body2 )
	{
		offset *= rotation;
		relAxis *= rotation.ToMat3();
	}
}

/*
================
budAFConstraint_Fixed::GetCenter
================
*/
void budAFConstraint_Fixed::GetCenter( budVec3& center )
{
	center = body1->GetWorldOrigin();
}

/*
================
budAFConstraint_Fixed::DebugDraw
================
*/
void budAFConstraint_Fixed::DebugDraw()
{
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	if( master )
	{
		gameRenderWorld->DebugLine( colorRed, body1->GetWorldOrigin(), master->GetWorldOrigin() );
	}
	else
	{
		gameRenderWorld->DebugLine( colorRed, body1->GetWorldOrigin(), vec3_origin );
	}
}

/*
================
budAFConstraint_Fixed::Save
================
*/
void budAFConstraint_Fixed::Save( idSaveGame* saveFile ) const
{
	budAFConstraint::Save( saveFile );
	saveFile->WriteVec3( offset );
	saveFile->WriteMat3( relAxis );
}

/*
================
budAFConstraint_Fixed::Restore
================
*/
void budAFConstraint_Fixed::Restore( idRestoreGame* saveFile )
{
	budAFConstraint::Restore( saveFile );
	saveFile->ReadVec3( offset );
	saveFile->ReadMat3( relAxis );
}


//===============================================================
//
//	budAFConstraint_BallAndSocketJoint
//
//===============================================================

/*
================
budAFConstraint_BallAndSocketJoint::budAFConstraint_BallAndSocketJoint
================
*/
budAFConstraint_BallAndSocketJoint::budAFConstraint_BallAndSocketJoint( const budStr& name, budAFBody* body1, budAFBody* body2 )
{
	assert( body1 );
	type = CONSTRAINT_BALLANDSOCKETJOINT;
	this->name = name;
	this->body1 = body1;
	this->body2 = body2;
	InitSize( 3 );
	coneLimit = NULL;
	pyramidLimit = NULL;
	friction = 0.0f;
	fc = NULL;
	fl.allowPrimary = true;
	fl.noCollision = true;
}

/*
================
budAFConstraint_BallAndSocketJoint::~budAFConstraint_BallAndSocketJoint
================
*/
budAFConstraint_BallAndSocketJoint::~budAFConstraint_BallAndSocketJoint()
{
	if( coneLimit )
	{
		delete coneLimit;
	}
	if( pyramidLimit )
	{
		delete pyramidLimit;
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::SetAnchor
================
*/
void budAFConstraint_BallAndSocketJoint::SetAnchor( const budVec3& worldPosition )
{

	// get anchor relative to center of mass of body1
	anchor1 = ( worldPosition - body1->GetWorldOrigin() ) * body1->GetWorldAxis().Transpose();
	if( body2 )
	{
		// get anchor relative to center of mass of body2
		anchor2 = ( worldPosition - body2->GetWorldOrigin() ) * body2->GetWorldAxis().Transpose();
	}
	else
	{
		anchor2 = worldPosition;
	}
	
	if( coneLimit )
	{
		coneLimit->SetAnchor( anchor2 );
	}
	if( pyramidLimit )
	{
		pyramidLimit->SetAnchor( anchor2 );
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::GetAnchor
================
*/
budVec3 budAFConstraint_BallAndSocketJoint::GetAnchor() const
{
	if( body2 )
	{
		return body2->GetWorldOrigin() + body2->GetWorldAxis() * anchor2;
	}
	return anchor2;
}

/*
================
budAFConstraint_BallAndSocketJoint::SetNoLimit
================
*/
void budAFConstraint_BallAndSocketJoint::SetNoLimit()
{
	if( coneLimit )
	{
		delete coneLimit;
		coneLimit = NULL;
	}
	if( pyramidLimit )
	{
		delete pyramidLimit;
		pyramidLimit = NULL;
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::SetConeLimit
================
*/
void budAFConstraint_BallAndSocketJoint::SetConeLimit( const budVec3& coneAxis, const float coneAngle, const budVec3& body1Axis )
{
	if( pyramidLimit )
	{
		delete pyramidLimit;
		pyramidLimit = NULL;
	}
	if( !coneLimit )
	{
		coneLimit = new( TAG_PHYSICS_AF ) budAFConstraint_ConeLimit;
		coneLimit->SetPhysics( physics );
	}
	if( body2 )
	{
		coneLimit->Setup( body1, body2, anchor2, coneAxis * body2->GetWorldAxis().Transpose(), coneAngle, body1Axis * body1->GetWorldAxis().Transpose() );
	}
	else
	{
		coneLimit->Setup( body1, body2, anchor2, coneAxis, coneAngle, body1Axis * body1->GetWorldAxis().Transpose() );
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::SetPyramidLimit
================
*/
void budAFConstraint_BallAndSocketJoint::SetPyramidLimit( const budVec3& pyrambudAxis, const budVec3& baseAxis,
		const float angle1, const float angle2, const budVec3& body1Axis )
{
	if( coneLimit )
	{
		delete coneLimit;
		coneLimit = NULL;
	}
	if( !pyramidLimit )
	{
		pyramidLimit = new( TAG_PHYSICS_AF ) budAFConstraint_PyramidLimit;
		pyramidLimit->SetPhysics( physics );
	}
	if( body2 )
	{
		pyramidLimit->Setup( body1, body2, anchor2, pyrambudAxis * body2->GetWorldAxis().Transpose(),
							 baseAxis * body2->GetWorldAxis().Transpose(), angle1, angle2,
							 body1Axis * body1->GetWorldAxis().Transpose() );
	}
	else
	{
		pyramidLimit->Setup( body1, body2, anchor2, pyrambudAxis, baseAxis, angle1, angle2,
							 body1Axis * body1->GetWorldAxis().Transpose() );
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::SetLimitEpsilon
================
*/
void budAFConstraint_BallAndSocketJoint::SetLimitEpsilon( const float e )
{
	if( coneLimit )
	{
		coneLimit->SetEpsilon( e );
	}
	if( pyramidLimit )
	{
		pyramidLimit->SetEpsilon( e );
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::GetFriction
================
*/
float budAFConstraint_BallAndSocketJoint::GetFriction() const
{
	if( af_forceFriction.GetFloat() > 0.0f )
	{
		return af_forceFriction.GetFloat();
	}
	return friction * physics->GetJointFrictionScale();
}

/*
================
budAFConstraint_BallAndSocketJoint::Evaluate
================
*/
void budAFConstraint_BallAndSocketJoint::Evaluate( float invTimeStep )
{
	budVec3 a1, a2;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	a1 = anchor1 * body1->GetWorldAxis();
	
	if( master )
	{
		a2 = anchor2 * master->GetWorldAxis();
		c1.SubVec3( 0 ) = -( invTimeStep * ERROR_REDUCTION ) * ( a2 + master->GetWorldOrigin() - ( a1 + body1->GetWorldOrigin() ) );
	}
	else
	{
		c1.SubVec3( 0 ) = -( invTimeStep * ERROR_REDUCTION ) * ( anchor2 - ( a1 + body1->GetWorldOrigin() ) );
	}
	
	c1.Clamp( -ERROR_REDUCTION_MAX, ERROR_REDUCTION_MAX );
	
	J1.Set( mat3_identity, -SkewSymmetric( a1 ) );
	
	if( body2 )
	{
		J2.Set( -mat3_identity, SkewSymmetric( a2 ) );
	}
	else
	{
		J2.Zero( 3, 6 );
	}
	
	if( coneLimit )
	{
		coneLimit->Add( physics, invTimeStep );
	}
	else if( pyramidLimit )
	{
		pyramidLimit->Add( physics, invTimeStep );
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::ApplyFriction
================
*/
void budAFConstraint_BallAndSocketJoint::ApplyFriction( float invTimeStep )
{
	budVec3 angular;
	float invMass, currentFriction;
	
	currentFriction = GetFriction();
	
	if( currentFriction <= 0.0f )
	{
		return;
	}
	
	if( af_useImpulseFriction.GetBool() || af_useJointImpulseFriction.GetBool() )
	{
	
		angular = body1->GetAngularVelocity();
		invMass = body1->GetInverseMass();
		if( body2 )
		{
			angular -= body2->GetAngularVelocity();
			invMass += body2->GetInverseMass();
		}
		
		angular *= currentFriction / invMass;
		
		body1->SetAngularVelocity( body1->GetAngularVelocity() - angular * body1->GetInverseMass() );
		if( body2 )
		{
			body2->SetAngularVelocity( body2->GetAngularVelocity() + angular * body2->GetInverseMass() );
		}
	}
	else
	{
		if( !fc )
		{
			fc = new( TAG_PHYSICS_AF ) budAFConstraint_BallAndSocketJointFriction;
			fc->Setup( this );
		}
		
		fc->Add( physics, invTimeStep );
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::GetForce
================
*/
void budAFConstraint_BallAndSocketJoint::GetForce( budAFBody* body, budVec6& force )
{
	budAFConstraint::GetForce( body, force );
	// FIXME: add limit force
}

/*
================
budAFConstraint_BallAndSocketJoint::Translate
================
*/
void budAFConstraint_BallAndSocketJoint::Translate( const budVec3& translation )
{
	if( !body2 )
	{
		anchor2 += translation;
	}
	if( coneLimit )
	{
		coneLimit->Translate( translation );
	}
	else if( pyramidLimit )
	{
		pyramidLimit->Translate( translation );
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::Rotate
================
*/
void budAFConstraint_BallAndSocketJoint::Rotate( const budRotation& rotation )
{
	if( !body2 )
	{
		anchor2 *= rotation;
	}
	if( coneLimit )
	{
		coneLimit->Rotate( rotation );
	}
	else if( pyramidLimit )
	{
		pyramidLimit->Rotate( rotation );
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::GetCenter
================
*/
void budAFConstraint_BallAndSocketJoint::GetCenter( budVec3& center )
{
	center = body1->GetWorldOrigin() + anchor1 * body1->GetWorldAxis();
}

/*
================
budAFConstraint_BallAndSocketJoint::DebugDraw
================
*/
void budAFConstraint_BallAndSocketJoint::DebugDraw()
{
	budVec3 a1 = body1->GetWorldOrigin() + anchor1 * body1->GetWorldAxis();
	gameRenderWorld->DebugLine( colorBlue, a1 - budVec3( 5, 0, 0 ), a1 + budVec3( 5, 0, 0 ) );
	gameRenderWorld->DebugLine( colorBlue, a1 - budVec3( 0, 5, 0 ), a1 + budVec3( 0, 5, 0 ) );
	gameRenderWorld->DebugLine( colorBlue, a1 - budVec3( 0, 0, 5 ), a1 + budVec3( 0, 0, 5 ) );
	
	if( af_showLimits.GetBool() )
	{
		if( coneLimit )
		{
			coneLimit->DebugDraw();
		}
		if( pyramidLimit )
		{
			pyramidLimit->DebugDraw();
		}
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::Save
================
*/
void budAFConstraint_BallAndSocketJoint::Save( idSaveGame* saveFile ) const
{
	budAFConstraint::Save( saveFile );
	saveFile->WriteVec3( anchor1 );
	saveFile->WriteVec3( anchor2 );
	saveFile->WriteFloat( friction );
	if( coneLimit )
	{
		coneLimit->Save( saveFile );
	}
	if( pyramidLimit )
	{
		pyramidLimit->Save( saveFile );
	}
}

/*
================
budAFConstraint_BallAndSocketJoint::Restore
================
*/
void budAFConstraint_BallAndSocketJoint::Restore( idRestoreGame* saveFile )
{
	budAFConstraint::Restore( saveFile );
	saveFile->ReadVec3( anchor1 );
	saveFile->ReadVec3( anchor2 );
	saveFile->ReadFloat( friction );
	if( coneLimit )
	{
		coneLimit->Restore( saveFile );
	}
	if( pyramidLimit )
	{
		pyramidLimit->Restore( saveFile );
	}
}


//===============================================================
//
//	budAFConstraint_BallAndSocketJointFriction
//
//===============================================================

/*
================
budAFConstraint_BallAndSocketJointFriction::budAFConstraint_BallAndSocketJointFriction
================
*/
budAFConstraint_BallAndSocketJointFriction::budAFConstraint_BallAndSocketJointFriction()
{
	type = CONSTRAINT_FRICTION;
	name = "ballAndSocketJointFriction";
	InitSize( 3 );
	joint = NULL;
	fl.allowPrimary = false;
	fl.frameConstraint = true;
}

/*
================
budAFConstraint_BallAndSocketJointFriction::Setup
================
*/
void budAFConstraint_BallAndSocketJointFriction::Setup( budAFConstraint_BallAndSocketJoint* bsj )
{
	this->joint = bsj;
	body1 = bsj->GetBody1();
	body2 = bsj->GetBody2();
}

/*
================
budAFConstraint_BallAndSocketJointFriction::Evaluate
================
*/
void budAFConstraint_BallAndSocketJointFriction::Evaluate( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_BallAndSocketJointFriction::ApplyFriction
================
*/
void budAFConstraint_BallAndSocketJointFriction::ApplyFriction( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_BallAndSocketJointFriction::Add
================
*/
bool budAFConstraint_BallAndSocketJointFriction::Add( idPhysics_AF* phys, float invTimeStep )
{
	float f;
	
	physics = phys;
	
	f = joint->GetFriction() * joint->GetMultiplier().Length();
	if( f == 0.0f )
	{
		return false;
	}
	
	lo[0] = lo[1] = lo[2] = -f;
	hi[0] = hi[1] = hi[2] = f;
	
	J1.Zero( 3, 6 );
	J1[0][3] = J1[1][4] = J1[2][5] = 1.0f;
	
	if( body2 )
	{
	
		J2.Zero( 3, 6 );
		J2[0][3] = J2[1][4] = J2[2][5] = 1.0f;
	}
	
	physics->AddFrameConstraint( this );
	
	return true;
}

/*
================
budAFConstraint_BallAndSocketJointFriction::Translate
================
*/
void budAFConstraint_BallAndSocketJointFriction::Translate( const budVec3& translation )
{
}

/*
================
budAFConstraint_BallAndSocketJointFriction::Rotate
================
*/
void budAFConstraint_BallAndSocketJointFriction::Rotate( const budRotation& rotation )
{
}


//===============================================================
//
//	budAFConstraint_UniversalJoint
//
//===============================================================

/*
================
budAFConstraint_UniversalJoint::budAFConstraint_UniversalJoint
================
*/
budAFConstraint_UniversalJoint::budAFConstraint_UniversalJoint( const budStr& name, budAFBody* body1, budAFBody* body2 )
{
	assert( body1 );
	type = CONSTRAINT_UNIVERSALJOINT;
	this->name = name;
	this->body1 = body1;
	this->body2 = body2;
	InitSize( 4 );
	coneLimit = NULL;
	pyramidLimit = NULL;
	friction = 0.0f;
	fc = NULL;
	fl.allowPrimary = true;
	fl.noCollision = true;
}

/*
================
budAFConstraint_UniversalJoint::~budAFConstraint_UniversalJoint
================
*/
budAFConstraint_UniversalJoint::~budAFConstraint_UniversalJoint()
{
	if( coneLimit )
	{
		delete coneLimit;
	}
	if( pyramidLimit )
	{
		delete pyramidLimit;
	}
	if( fc )
	{
		delete fc;
	}
}

/*
================
budAFConstraint_UniversalJoint::SetAnchor
================
*/
void budAFConstraint_UniversalJoint::SetAnchor( const budVec3& worldPosition )
{

	// get anchor relative to center of mass of body1
	anchor1 = ( worldPosition - body1->GetWorldOrigin() ) * body1->GetWorldAxis().Transpose();
	if( body2 )
	{
		// get anchor relative to center of mass of body2
		anchor2 = ( worldPosition - body2->GetWorldOrigin() ) * body2->GetWorldAxis().Transpose();
	}
	else
	{
		anchor2 = worldPosition;
	}
	
	if( coneLimit )
	{
		coneLimit->SetAnchor( anchor2 );
	}
	if( pyramidLimit )
	{
		pyramidLimit->SetAnchor( anchor2 );
	}
}

/*
================
budAFConstraint_UniversalJoint::GetAnchor
================
*/
budVec3 budAFConstraint_UniversalJoint::GetAnchor() const
{
	if( body2 )
	{
		return body2->GetWorldOrigin() + body2->GetWorldAxis() * anchor2;
	}
	return anchor2;
}

/*
================
budAFConstraint_UniversalJoint::SetShafts
================
*/
void budAFConstraint_UniversalJoint::SetShafts( const budVec3& cardanShaft1, const budVec3& cardanShaft2 )
{
	budVec3 cardanAxis;
	float l;
	
	shaft1 = cardanShaft1;
	l = shaft1.Normalize();
	assert( l != 0.0f );
	shaft2 = cardanShaft2;
	l = shaft2.Normalize();
	assert( l != 0.0f );
	
	// the cardan axis is a vector orthogonal to both cardan shafts
	cardanAxis = shaft1.Cross( shaft2 );
	if( cardanAxis.Normalize() == 0.0f )
	{
		budVec3 vecY;
		shaft1.OrthogonalBasis( cardanAxis, vecY );
		cardanAxis.Normalize();
	}
	
	shaft1 *= body1->GetWorldAxis().Transpose();
	axis1 = cardanAxis * body1->GetWorldAxis().Transpose();
	if( body2 )
	{
		shaft2 *= body2->GetWorldAxis().Transpose();
		axis2 = cardanAxis * body2->GetWorldAxis().Transpose();
	}
	else
	{
		axis2 = cardanAxis;
	}
	
	if( coneLimit )
	{
		coneLimit->SetBody1Axis( shaft1 );
	}
	if( pyramidLimit )
	{
		pyramidLimit->SetBody1Axis( shaft1 );
	}
}

/*
================
budAFConstraint_UniversalJoint::SetNoLimit
================
*/
void budAFConstraint_UniversalJoint::SetNoLimit()
{
	if( coneLimit )
	{
		delete coneLimit;
		coneLimit = NULL;
	}
	if( pyramidLimit )
	{
		delete pyramidLimit;
		pyramidLimit = NULL;
	}
}

/*
================
budAFConstraint_UniversalJoint::SetConeLimit
================
*/
void budAFConstraint_UniversalJoint::SetConeLimit( const budVec3& coneAxis, const float coneAngle )
{
	if( pyramidLimit )
	{
		delete pyramidLimit;
		pyramidLimit = NULL;
	}
	if( !coneLimit )
	{
		coneLimit = new( TAG_PHYSICS_AF ) budAFConstraint_ConeLimit;
		coneLimit->SetPhysics( physics );
	}
	if( body2 )
	{
		coneLimit->Setup( body1, body2, anchor2, coneAxis * body2->GetWorldAxis().Transpose(), coneAngle, shaft1 );
	}
	else
	{
		coneLimit->Setup( body1, body2, anchor2, coneAxis, coneAngle, shaft1 );
	}
}

/*
================
budAFConstraint_UniversalJoint::SetPyramidLimit
================
*/
void budAFConstraint_UniversalJoint::SetPyramidLimit( const budVec3& pyrambudAxis, const budVec3& baseAxis,
		const float angle1, const float angle2 )
{
	if( coneLimit )
	{
		delete coneLimit;
		coneLimit = NULL;
	}
	if( !pyramidLimit )
	{
		pyramidLimit = new( TAG_PHYSICS_AF ) budAFConstraint_PyramidLimit;
		pyramidLimit->SetPhysics( physics );
	}
	if( body2 )
	{
		pyramidLimit->Setup( body1, body2, anchor2, pyrambudAxis * body2->GetWorldAxis().Transpose(),
							 baseAxis * body2->GetWorldAxis().Transpose(), angle1, angle2, shaft1 );
	}
	else
	{
		pyramidLimit->Setup( body1, body2, anchor2, pyrambudAxis, baseAxis, angle1, angle2, shaft1 );
	}
}

/*
================
budAFConstraint_UniversalJoint::SetLimitEpsilon
================
*/
void budAFConstraint_UniversalJoint::SetLimitEpsilon( const float e )
{
	if( coneLimit )
	{
		coneLimit->SetEpsilon( e );
	}
	if( pyramidLimit )
	{
		pyramidLimit->SetEpsilon( e );
	}
}

/*
================
budAFConstraint_UniversalJoint::GetFriction
================
*/
float budAFConstraint_UniversalJoint::GetFriction() const
{
	if( af_forceFriction.GetFloat() > 0.0f )
	{
		return af_forceFriction.GetFloat();
	}
	return friction * physics->GetJointFrictionScale();
}

/*
================
budAFConstraint_UniversalJoint::Evaluate

  NOTE: this joint is homokinetic
================
*/
void budAFConstraint_UniversalJoint::Evaluate( float invTimeStep )
{
	budVec3 a1, a2, s1, s2, d1, d2, v;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	a1 = anchor1 * body1->GetWorldAxis();
	s1 = shaft1 * body1->GetWorldAxis();
	d1 = s1.Cross( axis1 * body1->GetWorldAxis() );
	
	if( master )
	{
		a2 = anchor2 * master->GetWorldAxis();
		s2 = shaft2 * master->GetWorldAxis();
		d2 = axis2 * master->GetWorldAxis();
		c1.SubVec3( 0 ) = -( invTimeStep * ERROR_REDUCTION ) * ( a2 + master->GetWorldOrigin() - ( a1 + body1->GetWorldOrigin() ) );
	}
	else
	{
		a2 = anchor2;
		s2 = shaft2;
		d2 = axis2;
		c1.SubVec3( 0 ) = -( invTimeStep * ERROR_REDUCTION ) * ( a2 - ( a1 + body1->GetWorldOrigin() ) );
	}
	
	J1.Set(	mat3_identity,	-SkewSymmetric( a1 ),
			mat3_zero,		budMat3( s1[0], s1[1], s1[2],
									0.0f, 0.0f, 0.0f,
									0.0f, 0.0f, 0.0f ) );
	J1.SetSize( 4, 6 );
	
	if( body2 )
	{
		J2.Set(	-mat3_identity,	SkewSymmetric( a2 ),
				mat3_zero,		budMat3( s2[0], s2[1], s2[2],
										0.0f, 0.0f, 0.0f,
										0.0f, 0.0f, 0.0f ) );
		J2.SetSize( 4, 6 );
	}
	else
	{
		J2.Zero( 4, 6 );
	}
	
	v = s1.Cross( s2 );
	if( v.Normalize() != 0.0f )
	{
		budMat3 m1, m2;
		
		m1[0] = s1;
		m1[1] = v;
		m1[2] = v.Cross( m1[0] );
		
		m2[0] = -s2;
		m2[1] = v;
		m2[2] = v.Cross( m2[0] );
		
		d2 *= m2.Transpose() * m1;
	}
	
	c1[3] = -( invTimeStep * ERROR_REDUCTION ) * ( d1 * d2 );
	
	c1.Clamp( -ERROR_REDUCTION_MAX, ERROR_REDUCTION_MAX );
	
	if( coneLimit )
	{
		coneLimit->Add( physics, invTimeStep );
	}
	else if( pyramidLimit )
	{
		pyramidLimit->Add( physics, invTimeStep );
	}
}

/*
================
budAFConstraint_UniversalJoint::ApplyFriction
================
*/
void budAFConstraint_UniversalJoint::ApplyFriction( float invTimeStep )
{
	budVec3 angular;
	float invMass, currentFriction;
	
	currentFriction = GetFriction();
	
	if( currentFriction <= 0.0f )
	{
		return;
	}
	
	if( af_useImpulseFriction.GetBool() || af_useJointImpulseFriction.GetBool() )
	{
	
		angular = body1->GetAngularVelocity();
		invMass = body1->GetInverseMass();
		if( body2 )
		{
			angular -= body2->GetAngularVelocity();
			invMass += body2->GetInverseMass();
		}
		
		angular *= currentFriction / invMass;
		
		body1->SetAngularVelocity( body1->GetAngularVelocity() - angular * body1->GetInverseMass() );
		if( body2 )
		{
			body2->SetAngularVelocity( body2->GetAngularVelocity() + angular * body2->GetInverseMass() );
		}
	}
	else
	{
		if( !fc )
		{
			fc = new( TAG_PHYSICS_AF ) budAFConstraint_UniversalJointFriction;
			fc->Setup( this );
		}
		
		fc->Add( physics, invTimeStep );
	}
}

/*
================
budAFConstraint_UniversalJoint::GetForce
================
*/
void budAFConstraint_UniversalJoint::GetForce( budAFBody* body, budVec6& force )
{
	budAFConstraint::GetForce( body, force );
	// FIXME: add limit force
}

/*
================
budAFConstraint_UniversalJoint::Translate
================
*/
void budAFConstraint_UniversalJoint::Translate( const budVec3& translation )
{
	if( !body2 )
	{
		anchor2 += translation;
	}
	if( coneLimit )
	{
		coneLimit->Translate( translation );
	}
	else if( pyramidLimit )
	{
		pyramidLimit->Translate( translation );
	}
}

/*
================
budAFConstraint_UniversalJoint::Rotate
================
*/
void budAFConstraint_UniversalJoint::Rotate( const budRotation& rotation )
{
	if( !body2 )
	{
		anchor2 *= rotation;
		shaft2 *= rotation.ToMat3();
		axis2 *= rotation.ToMat3();
	}
	if( coneLimit )
	{
		coneLimit->Rotate( rotation );
	}
	else if( pyramidLimit )
	{
		pyramidLimit->Rotate( rotation );
	}
}

/*
================
budAFConstraint_UniversalJoint::GetCenter
================
*/
void budAFConstraint_UniversalJoint::GetCenter( budVec3& center )
{
	center = body1->GetWorldOrigin() + anchor1 * body1->GetWorldAxis();
}

/*
================
budAFConstraint_UniversalJoint::DebugDraw
================
*/
void budAFConstraint_UniversalJoint::DebugDraw()
{
	budVec3 a1, a2, s1, s2, d1, d2, v;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	a1 = body1->GetWorldOrigin() + anchor1 * body1->GetWorldAxis();
	s1 = shaft1 * body1->GetWorldAxis();
	d1 = axis1 * body1->GetWorldAxis();
	
	if( master )
	{
		a2 = master->GetWorldOrigin() + anchor2 * master->GetWorldAxis();
		s2 = shaft2 * master->GetWorldAxis();
		d2 = axis2 * master->GetWorldAxis();
	}
	else
	{
		a2 = anchor2;
		s2 = shaft2;
		d2 = axis2;
	}
	
	v = s1.Cross( s2 );
	if( v.Normalize() != 0.0f )
	{
		budMat3 m1, m2;
		
		m1[0] = s1;
		m1[1] = v;
		m1[2] = v.Cross( m1[0] );
		
		m2[0] = -s2;
		m2[1] = v;
		m2[2] = v.Cross( m2[0] );
		
		d2 *= m2.Transpose() * m1;
	}
	
	gameRenderWorld->DebugArrow( colorCyan, a1, a1 + s1 * 5.0f, 1.0f );
	gameRenderWorld->DebugArrow( colorBlue, a2, a2 + s2 * 5.0f, 1.0f );
	gameRenderWorld->DebugLine( colorGreen, a1, a1 + d1 * 5.0f );
	gameRenderWorld->DebugLine( colorGreen, a2, a2 + d2 * 5.0f );
	
	if( af_showLimits.GetBool() )
	{
		if( coneLimit )
		{
			coneLimit->DebugDraw();
		}
		if( pyramidLimit )
		{
			pyramidLimit->DebugDraw();
		}
	}
}

/*
================
budAFConstraint_UniversalJoint::Save
================
*/
void budAFConstraint_UniversalJoint::Save( idSaveGame* saveFile ) const
{
	budAFConstraint::Save( saveFile );
	saveFile->WriteVec3( anchor1 );
	saveFile->WriteVec3( anchor2 );
	saveFile->WriteVec3( shaft1 );
	saveFile->WriteVec3( shaft2 );
	saveFile->WriteVec3( axis1 );
	saveFile->WriteVec3( axis2 );
	saveFile->WriteFloat( friction );
	if( coneLimit )
	{
		coneLimit->Save( saveFile );
	}
	if( pyramidLimit )
	{
		pyramidLimit->Save( saveFile );
	}
}

/*
================
budAFConstraint_UniversalJoint::Restore
================
*/
void budAFConstraint_UniversalJoint::Restore( idRestoreGame* saveFile )
{
	budAFConstraint::Restore( saveFile );
	saveFile->ReadVec3( anchor1 );
	saveFile->ReadVec3( anchor2 );
	saveFile->ReadVec3( shaft1 );
	saveFile->ReadVec3( shaft2 );
	saveFile->ReadVec3( axis1 );
	saveFile->ReadVec3( axis2 );
	saveFile->ReadFloat( friction );
	if( coneLimit )
	{
		coneLimit->Restore( saveFile );
	}
	if( pyramidLimit )
	{
		pyramidLimit->Restore( saveFile );
	}
}


//===============================================================
//
//	budAFConstraint_UniversalJointFriction
//
//===============================================================

/*
================
budAFConstraint_UniversalJointFriction::budAFConstraint_UniversalJointFriction
================
*/
budAFConstraint_UniversalJointFriction::budAFConstraint_UniversalJointFriction()
{
	type = CONSTRAINT_FRICTION;
	name = "universalJointFriction";
	InitSize( 2 );
	joint = NULL;
	fl.allowPrimary = false;
	fl.frameConstraint = true;
}

/*
================
budAFConstraint_UniversalJointFriction::Setup
================
*/
void budAFConstraint_UniversalJointFriction::Setup( budAFConstraint_UniversalJoint* uj )
{
	this->joint = uj;
	body1 = uj->GetBody1();
	body2 = uj->GetBody2();
}

/*
================
budAFConstraint_UniversalJointFriction::Evaluate
================
*/
void budAFConstraint_UniversalJointFriction::Evaluate( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_UniversalJointFriction::ApplyFriction
================
*/
void budAFConstraint_UniversalJointFriction::ApplyFriction( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_UniversalJointFriction::Add
================
*/
bool budAFConstraint_UniversalJointFriction::Add( idPhysics_AF* phys, float invTimeStep )
{
	budVec3 s1, s2, dir1, dir2;
	float f;
	
	physics = phys;
	
	f = joint->GetFriction() * joint->GetMultiplier().Length();
	if( f == 0.0f )
	{
		return false;
	}
	
	lo[0] = lo[1] = -f;
	hi[0] = hi[1] = f;
	
	joint->GetShafts( s1, s2 );
	
	s1 *= body1->GetWorldAxis();
	s1.NormalVectors( dir1, dir2 );
	
	J1.SetSize( 2, 6 );
	J1.SubVec6( 0 ).SubVec3( 0 ).Zero();
	J1.SubVec6( 0 ).SubVec3( 1 ) = dir1;
	J1.SubVec6( 1 ).SubVec3( 0 ).Zero();
	J1.SubVec6( 1 ).SubVec3( 1 ) = dir2;
	
	if( body2 )
	{
	
		J2.SetSize( 2, 6 );
		J2.SubVec6( 0 ).SubVec3( 0 ).Zero();
		J2.SubVec6( 0 ).SubVec3( 1 ) = -dir1;
		J2.SubVec6( 1 ).SubVec3( 0 ).Zero();
		J2.SubVec6( 1 ).SubVec3( 1 ) = -dir2;
	}
	
	physics->AddFrameConstraint( this );
	
	return true;
}

/*
================
budAFConstraint_UniversalJointFriction::Translate
================
*/
void budAFConstraint_UniversalJointFriction::Translate( const budVec3& translation )
{
}

/*
================
budAFConstraint_UniversalJointFriction::Rotate
================
*/
void budAFConstraint_UniversalJointFriction::Rotate( const budRotation& rotation )
{
}


//===============================================================
//
//	budAFConstraint_CylindricalJoint
//
//===============================================================

/*
================
budAFConstraint_CylindricalJoint::budAFConstraint_CylindricalJoint
================
*/
budAFConstraint_CylindricalJoint::budAFConstraint_CylindricalJoint( const budStr& name, budAFBody* body1, budAFBody* body2 )
{
	assert( 0 );	// FIXME: implement
}

/*
================
budAFConstraint_CylindricalJoint::Evaluate
================
*/
void budAFConstraint_CylindricalJoint::Evaluate( float invTimeStep )
{
	assert( 0 );	// FIXME: implement
}

/*
================
budAFConstraint_CylindricalJoint::ApplyFriction
================
*/
void budAFConstraint_CylindricalJoint::ApplyFriction( float invTimeStep )
{
	assert( 0 );	// FIXME: implement
}

/*
================
budAFConstraint_CylindricalJoint::Translate
================
*/
void budAFConstraint_CylindricalJoint::Translate( const budVec3& translation )
{
	assert( 0 );	// FIXME: implement
}

/*
================
budAFConstraint_CylindricalJoint::Rotate
================
*/
void budAFConstraint_CylindricalJoint::Rotate( const budRotation& rotation )
{
	assert( 0 );	// FIXME: implement
}

/*
================
budAFConstraint_CylindricalJoint::DebugDraw
================
*/
void budAFConstraint_CylindricalJoint::DebugDraw()
{
	assert( 0 );	// FIXME: implement
}


//===============================================================
//
//	budAFConstraint_Hinge
//
//===============================================================

/*
================
budAFConstraint_Hinge::budAFConstraint_Hinge
================
*/
budAFConstraint_Hinge::budAFConstraint_Hinge( const budStr& name, budAFBody* body1, budAFBody* body2 )
{
	assert( body1 );
	type = CONSTRAINT_HINGE;
	this->name = name;
	this->body1 = body1;
	this->body2 = body2;
	InitSize( 5 );
	coneLimit = NULL;
	steering = NULL;
	friction = 0.0f;
	fc = NULL;
	fl.allowPrimary = true;
	fl.noCollision = true;
	initialAxis = body1->GetWorldAxis();
	if( body2 )
	{
		initialAxis *= body2->GetWorldAxis().Transpose();
	}
}

/*
================
budAFConstraint_Hinge::~budAFConstraint_Hinge
================
*/
budAFConstraint_Hinge::~budAFConstraint_Hinge()
{
	if( coneLimit )
	{
		delete coneLimit;
	}
	if( fc )
	{
		delete fc;
	}
	if( steering )
	{
		delete steering;
	}
}

/*
================
budAFConstraint_Hinge::SetAnchor
================
*/
void budAFConstraint_Hinge::SetAnchor( const budVec3& worldPosition )
{
	// get anchor relative to center of mass of body1
	anchor1 = ( worldPosition - body1->GetWorldOrigin() ) * body1->GetWorldAxis().Transpose();
	if( body2 )
	{
		// get anchor relative to center of mass of body2
		anchor2 = ( worldPosition - body2->GetWorldOrigin() ) * body2->GetWorldAxis().Transpose();
	}
	else
	{
		anchor2 = worldPosition;
	}
	
	if( coneLimit )
	{
		coneLimit->SetAnchor( anchor2 );
	}
}

/*
================
budAFConstraint_Hinge::GetAnchor
================
*/
budVec3 budAFConstraint_Hinge::GetAnchor() const
{
	if( body2 )
	{
		return body2->GetWorldOrigin() + body2->GetWorldAxis() * anchor2;
	}
	return anchor2;
}

/*
================
budAFConstraint_Hinge::SetAxis
================
*/
void budAFConstraint_Hinge::SetAxis( const budVec3& axis )
{
	budVec3 normAxis;
	
	normAxis = axis;
	normAxis.Normalize();
	
	// get axis relative to body1
	axis1 = normAxis * body1->GetWorldAxis().Transpose();
	if( body2 )
	{
		// get axis relative to body2
		axis2 = normAxis * body2->GetWorldAxis().Transpose();
	}
	else
	{
		axis2 = normAxis;
	}
}

/*
================
budAFConstraint_Hinge::GetAxis
================
*/
budVec3 budAFConstraint_Hinge::GetAxis() const
{
	if( body2 )
	{
		return axis2 * body2->GetWorldAxis();
	}
	return axis2;
}

/*
================
budAFConstraint_Hinge::SetNoLimit
================
*/
void budAFConstraint_Hinge::SetNoLimit()
{
	if( coneLimit )
	{
		delete coneLimit;
		coneLimit = NULL;
	}
}

/*
================
budAFConstraint_Hinge::SetLimit
================
*/
void budAFConstraint_Hinge::SetLimit( const budVec3& axis, const float angle, const budVec3& body1Axis )
{
	if( !coneLimit )
	{
		coneLimit = new( TAG_PHYSICS_AF ) budAFConstraint_ConeLimit;
		coneLimit->SetPhysics( physics );
	}
	if( body2 )
	{
		coneLimit->Setup( body1, body2, anchor2, axis * body2->GetWorldAxis().Transpose(), angle, body1Axis * body1->GetWorldAxis().Transpose() );
	}
	else
	{
		coneLimit->Setup( body1, body2, anchor2, axis, angle, body1Axis * body1->GetWorldAxis().Transpose() );
	}
}

/*
================
budAFConstraint_Hinge::SetLimitEpsilon
================
*/
void budAFConstraint_Hinge::SetLimitEpsilon( const float e )
{
	if( coneLimit )
	{
		coneLimit->SetEpsilon( e );
	}
}

/*
================
budAFConstraint_Hinge::GetFriction
================
*/
float budAFConstraint_Hinge::GetFriction() const
{
	if( af_forceFriction.GetFloat() > 0.0f )
	{
		return af_forceFriction.GetFloat();
	}
	return friction * physics->GetJointFrictionScale();
}

/*
================
budAFConstraint_Hinge::GetAngle
================
*/
float budAFConstraint_Hinge::GetAngle() const
{
	budMat3 axis;
	budRotation rotation;
	float angle;
	
	axis = body1->GetWorldAxis() * body2->GetWorldAxis().Transpose() * initialAxis.Transpose();
	rotation = axis.ToRotation();
	angle = rotation.GetAngle();
	if( rotation.GetVec() * axis1 < 0.0f )
	{
		return -angle;
	}
	return angle;
}

/*
================
budAFConstraint_Hinge::SetSteerAngle
================
*/
void budAFConstraint_Hinge::SetSteerAngle( const float degrees )
{
	if( coneLimit )
	{
		delete coneLimit;
		coneLimit = NULL;
	}
	if( !steering )
	{
		steering = new( TAG_PHYSICS_AF ) budAFConstraint_HingeSteering();
		steering->Setup( this );
	}
	steering->SetSteerAngle( degrees );
}

/*
================
budAFConstraint_Hinge::SetSteerSpeed
================
*/
void budAFConstraint_Hinge::SetSteerSpeed( const float speed )
{
	if( steering )
	{
		steering->SetSteerSpeed( speed );
	}
}

/*
================
budAFConstraint_Hinge::Evaluate
================
*/
void budAFConstraint_Hinge::Evaluate( float invTimeStep )
{
	budVec3 a1, a2;
	budVec3 x1, x2, cross;
	budVec3 vecX, vecY;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	x1 = axis1 * body1->GetWorldAxis();		// axis in body1 space
	x1.OrthogonalBasis( vecX, vecY );				// basis for axis in body1 space
	
	a1 = anchor1 * body1->GetWorldAxis();	// anchor in body1 space
	
	if( master )
	{
		a2 = anchor2 * master->GetWorldAxis();	// anchor in master space
		x2 = axis2 * master->GetWorldAxis();
		c1.SubVec3( 0 ) = -( invTimeStep * ERROR_REDUCTION ) * ( a2 + master->GetWorldOrigin() - ( a1 + body1->GetWorldOrigin() ) );
	}
	else
	{
		a2 = anchor2;
		x2 = axis2;
		c1.SubVec3( 0 ) = -( invTimeStep * ERROR_REDUCTION ) * ( a2 - ( a1 + body1->GetWorldOrigin() ) );
	}
	
	J1.Set(	mat3_identity,	-SkewSymmetric( a1 ),
			mat3_zero,		budMat3(	vecX[0], vecX[1], vecX[2],
									vecY[0], vecY[1], vecY[2],
									0.0f, 0.0f, 0.0f ) );
	J1.SetSize( 5, 6 );
	
	if( body2 )
	{
		J2.Set(	-mat3_identity,	SkewSymmetric( a2 ),
				mat3_zero,		budMat3(	-vecX[0], -vecX[1], -vecX[2],
										-vecY[0], -vecY[1], -vecY[2],
										0.0f, 0.0f, 0.0f ) );
		J2.SetSize( 5, 6 );
	}
	else
	{
		J2.Zero( 5, 6 );
	}
	
	cross = x1.Cross( x2 );
	
	c1[3] = -( invTimeStep * ERROR_REDUCTION ) * ( cross * vecX );
	c1[4] = -( invTimeStep * ERROR_REDUCTION ) * ( cross * vecY );
	
	c1.Clamp( -ERROR_REDUCTION_MAX, ERROR_REDUCTION_MAX );
	
	if( steering )
	{
		steering->Add( physics, invTimeStep );
	}
	else if( coneLimit )
	{
		coneLimit->Add( physics, invTimeStep );
	}
}

/*
================
budAFConstraint_Hinge::ApplyFriction
================
*/
void budAFConstraint_Hinge::ApplyFriction( float invTimeStep )
{
	budVec3 angular;
	float invMass, currentFriction;
	
	currentFriction = GetFriction();
	
	if( currentFriction <= 0.0f )
	{
		return;
	}
	
	if( af_useImpulseFriction.GetBool() || af_useJointImpulseFriction.GetBool() )
	{
	
		angular = body1->GetAngularVelocity();
		invMass = body1->GetInverseMass();
		if( body2 )
		{
			angular -= body2->GetAngularVelocity();
			invMass += body2->GetInverseMass();
		}
		
		angular *= currentFriction / invMass;
		
		body1->SetAngularVelocity( body1->GetAngularVelocity() - angular * body1->GetInverseMass() );
		if( body2 )
		{
			body2->SetAngularVelocity( body2->GetAngularVelocity() + angular * body2->GetInverseMass() );
		}
	}
	else
	{
		if( !fc )
		{
			fc = new( TAG_PHYSICS_AF ) budAFConstraint_HingeFriction;
			fc->Setup( this );
		}
		
		fc->Add( physics, invTimeStep );
	}
}

/*
================
budAFConstraint_Hinge::GetForce
================
*/
void budAFConstraint_Hinge::GetForce( budAFBody* body, budVec6& force )
{
	budAFConstraint::GetForce( body, force );
	// FIXME: add limit force
}

/*
================
budAFConstraint_Hinge::Translate
================
*/
void budAFConstraint_Hinge::Translate( const budVec3& translation )
{
	if( !body2 )
	{
		anchor2 += translation;
	}
	if( coneLimit )
	{
		coneLimit->Translate( translation );
	}
}

/*
================
budAFConstraint_Hinge::Rotate
================
*/
void budAFConstraint_Hinge::Rotate( const budRotation& rotation )
{
	if( !body2 )
	{
		anchor2 *= rotation;
		axis2 *= rotation.ToMat3();
	}
	if( coneLimit )
	{
		coneLimit->Rotate( rotation );
	}
}

/*
================
budAFConstraint_Hinge::GetCenter
================
*/
void budAFConstraint_Hinge::GetCenter( budVec3& center )
{
	center = body1->GetWorldOrigin() + anchor1 * body1->GetWorldAxis();
}

/*
================
budAFConstraint_Hinge::DebugDraw
================
*/
void budAFConstraint_Hinge::DebugDraw()
{
	budVec3 vecX, vecY;
	budVec3 a1 = body1->GetWorldOrigin() + anchor1 * body1->GetWorldAxis();
	budVec3 x1 = axis1 * body1->GetWorldAxis();
	x1.OrthogonalBasis( vecX, vecY );
	
	gameRenderWorld->DebugArrow( colorBlue, a1 - 4.0f * x1, a1 + 4.0f * x1, 1 );
	gameRenderWorld->DebugLine( colorBlue, a1 - 2.0f * vecX, a1 + 2.0f * vecX );
	gameRenderWorld->DebugLine( colorBlue, a1 - 2.0f * vecY, a1 + 2.0f * vecY );
	
	if( af_showLimits.GetBool() )
	{
		if( coneLimit )
		{
			coneLimit->DebugDraw();
		}
	}
}

/*
================
budAFConstraint_Hinge::Save
================
*/
void budAFConstraint_Hinge::Save( idSaveGame* saveFile ) const
{
	budAFConstraint::Save( saveFile );
	saveFile->WriteVec3( anchor1 );
	saveFile->WriteVec3( anchor2 );
	saveFile->WriteVec3( axis1 );
	saveFile->WriteVec3( axis2 );
	saveFile->WriteMat3( initialAxis );
	saveFile->WriteFloat( friction );
	if( coneLimit )
	{
		saveFile->WriteBool( true );
		coneLimit->Save( saveFile );
	}
	else
	{
		saveFile->WriteBool( false );
	}
	if( steering )
	{
		saveFile->WriteBool( true );
		steering->Save( saveFile );
	}
	else
	{
		saveFile->WriteBool( false );
	}
	if( fc )
	{
		saveFile->WriteBool( true );
		fc->Save( saveFile );
	}
	else
	{
		saveFile->WriteBool( false );
	}
}

/*
================
budAFConstraint_Hinge::Restore
================
*/
void budAFConstraint_Hinge::Restore( idRestoreGame* saveFile )
{
	bool b;
	budAFConstraint::Restore( saveFile );
	saveFile->ReadVec3( anchor1 );
	saveFile->ReadVec3( anchor2 );
	saveFile->ReadVec3( axis1 );
	saveFile->ReadVec3( axis2 );
	saveFile->ReadMat3( initialAxis );
	saveFile->ReadFloat( friction );
	
	saveFile->ReadBool( b );
	if( b )
	{
		if( !coneLimit )
		{
			coneLimit = new( TAG_PHYSICS_AF ) budAFConstraint_ConeLimit;
		}
		coneLimit->SetPhysics( physics );
		coneLimit->Restore( saveFile );
	}
	saveFile->ReadBool( b );
	if( b )
	{
		if( !steering )
		{
			steering = new( TAG_PHYSICS_AF ) budAFConstraint_HingeSteering;
		}
		steering->Setup( this );
		steering->Restore( saveFile );
	}
	saveFile->ReadBool( b );
	if( b )
	{
		if( !fc )
		{
			fc = new( TAG_PHYSICS_AF ) budAFConstraint_HingeFriction;
		}
		fc->Setup( this );
		fc->Restore( saveFile );
	}
}


//===============================================================
//
//	budAFConstraint_HingeFriction
//
//===============================================================

/*
================
budAFConstraint_HingeFriction::budAFConstraint_HingeFriction
================
*/
budAFConstraint_HingeFriction::budAFConstraint_HingeFriction()
{
	type = CONSTRAINT_FRICTION;
	name = "hingeFriction";
	InitSize( 1 );
	hinge = NULL;
	fl.allowPrimary = false;
	fl.frameConstraint = true;
}

/*
================
budAFConstraint_HingeFriction::Setup
================
*/
void budAFConstraint_HingeFriction::Setup( budAFConstraint_Hinge* h )
{
	this->hinge = h;
	body1 = h->GetBody1();
	body2 = h->GetBody2();
}

/*
================
budAFConstraint_HingeFriction::Evaluate
================
*/
void budAFConstraint_HingeFriction::Evaluate( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_HingeFriction::ApplyFriction
================
*/
void budAFConstraint_HingeFriction::ApplyFriction( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_HingeFriction::Add
================
*/
bool budAFConstraint_HingeFriction::Add( idPhysics_AF* phys, float invTimeStep )
{
	budVec3 a1, a2;
	float f;
	
	physics = phys;
	
	f = hinge->GetFriction() * hinge->GetMultiplier().Length();
	if( f == 0.0f )
	{
		return false;
	}
	
	lo[0] = -f;
	hi[0] = f;
	
	hinge->GetAxis( a1, a2 );
	
	a1 *= body1->GetWorldAxis();
	
	J1.SetSize( 1, 6 );
	J1.SubVec6( 0 ).SubVec3( 0 ).Zero();
	J1.SubVec6( 0 ).SubVec3( 1 ) = a1;
	
	if( body2 )
	{
		a2 *= body2->GetWorldAxis();
		
		J2.SetSize( 1, 6 );
		J2.SubVec6( 0 ).SubVec3( 0 ).Zero();
		J2.SubVec6( 0 ).SubVec3( 1 ) = -a2;
	}
	
	physics->AddFrameConstraint( this );
	
	return true;
}

/*
================
budAFConstraint_HingeFriction::Translate
================
*/
void budAFConstraint_HingeFriction::Translate( const budVec3& translation )
{
}

/*
================
budAFConstraint_HingeFriction::Rotate
================
*/
void budAFConstraint_HingeFriction::Rotate( const budRotation& rotation )
{
}


//===============================================================
//
//	budAFConstraint_HingeSteering
//
//===============================================================

/*
================
budAFConstraint_HingeSteering::budAFConstraint_HingeSteering
================
*/
budAFConstraint_HingeSteering::budAFConstraint_HingeSteering()
{
	type = CONSTRAINT_HINGESTEERING;
	name = "hingeFriction";
	InitSize( 1 );
	hinge = NULL;
	fl.allowPrimary = false;
	fl.frameConstraint = true;
	steerSpeed = 0.0f;
	epsilon = LCP_EPSILON;
}

/*
================
budAFConstraint_HingeSteering::Save
================
*/
void budAFConstraint_HingeSteering::Save( idSaveGame* saveFile ) const
{
	saveFile->WriteFloat( steerAngle );
	saveFile->WriteFloat( steerSpeed );
	saveFile->WriteFloat( epsilon );
}

/*
================
budAFConstraint_HingeSteering::Restore
================
*/
void budAFConstraint_HingeSteering::Restore( idRestoreGame* saveFile )
{
	saveFile->ReadFloat( steerAngle );
	saveFile->ReadFloat( steerSpeed );
	saveFile->ReadFloat( epsilon );
}

/*
================
budAFConstraint_HingeSteering::Setup
================
*/
void budAFConstraint_HingeSteering::Setup( budAFConstraint_Hinge* h )
{
	this->hinge = h;
	body1 = h->GetBody1();
	body2 = h->GetBody2();
}

/*
================
budAFConstraint_HingeSteering::Evaluate
================
*/
void budAFConstraint_HingeSteering::Evaluate( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_HingeSteering::ApplyFriction
================
*/
void budAFConstraint_HingeSteering::ApplyFriction( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_HingeSteering::Add
================
*/
bool budAFConstraint_HingeSteering::Add( idPhysics_AF* phys, float invTimeStep )
{
	float angle, speed;
	budVec3 a1, a2;
	
	physics = phys;
	
	hinge->GetAxis( a1, a2 );
	angle = hinge->GetAngle();
	
	a1 *= body1->GetWorldAxis();
	
	J1.SetSize( 1, 6 );
	J1.SubVec6( 0 ).SubVec3( 0 ).Zero();
	J1.SubVec6( 0 ).SubVec3( 1 ) = a1;
	
	if( body2 )
	{
		a2 *= body2->GetWorldAxis();
		
		J2.SetSize( 1, 6 );
		J2.SubVec6( 0 ).SubVec3( 0 ).Zero();
		J2.SubVec6( 0 ).SubVec3( 1 ) = -a2;
	}
	
	speed = steerAngle - angle;
	if( steerSpeed != 0.0f )
	{
		if( speed > steerSpeed )
		{
			speed = steerSpeed;
		}
		else if( speed < -steerSpeed )
		{
			speed = -steerSpeed;
		}
	}
	
	c1[0] = DEG2RAD( speed ) * invTimeStep;
	
	physics->AddFrameConstraint( this );
	
	return true;
}

/*
================
budAFConstraint_HingeSteering::Translate
================
*/
void budAFConstraint_HingeSteering::Translate( const budVec3& translation )
{
}

/*
================
budAFConstraint_HingeSteering::Rotate
================
*/
void budAFConstraint_HingeSteering::Rotate( const budRotation& rotation )
{
}


//===============================================================
//
//	budAFConstraint_Slider
//
//===============================================================

/*
================
budAFConstraint_Slider::budAFConstraint_Slider
================
*/
budAFConstraint_Slider::budAFConstraint_Slider( const budStr& name, budAFBody* body1, budAFBody* body2 )
{
	assert( body1 );
	type = CONSTRAINT_SLIDER;
	this->name = name;
	this->body1 = body1;
	this->body2 = body2;
	InitSize( 5 );
	fl.allowPrimary = true;
	fl.noCollision = true;
	
	if( body2 )
	{
		offset = ( body1->GetWorldOrigin() - body2->GetWorldOrigin() ) * body1->GetWorldAxis().Transpose();
		relAxis = body1->GetWorldAxis() * body2->GetWorldAxis().Transpose();
	}
	else
	{
		offset = body1->GetWorldOrigin();
		relAxis = body1->GetWorldAxis();
	}
}

/*
================
budAFConstraint_Slider::SetAxis
================
*/
void budAFConstraint_Slider::SetAxis( const budVec3& ax )
{
	budVec3 normAxis;
	
	// get normalized axis relative to body1
	normAxis = ax;
	normAxis.Normalize();
	if( body2 )
	{
		axis = normAxis * body2->GetWorldAxis().Transpose();
	}
	else
	{
		axis = normAxis;
	}
}

/*
================
budAFConstraint_Slider::Evaluate
================
*/
void budAFConstraint_Slider::Evaluate( float invTimeStep )
{
	budVec3 vecX, vecY, ofs;
	budRotation r;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	if( master )
	{
		( axis * master->GetWorldAxis() ).OrthogonalBasis( vecX, vecY );
		ofs = master->GetWorldOrigin() + master->GetWorldAxis() * offset - body1->GetWorldOrigin();
		r = ( body1->GetWorldAxis().Transpose() * ( relAxis * master->GetWorldAxis() ) ).ToRotation();
	}
	else
	{
		axis.OrthogonalBasis( vecX, vecY );
		ofs = offset - body1->GetWorldOrigin();
		r = ( body1->GetWorldAxis().Transpose() * relAxis ).ToRotation();
	}
	
	J1.Set(	mat3_zero, mat3_identity,
			budMat3( vecX, vecY, vec3_origin ), mat3_zero );
	J1.SetSize( 5, 6 );
	
	if( body2 )
	{
	
		J2.Set(	mat3_zero, -mat3_identity,
				budMat3( -vecX, -vecY, vec3_origin ), mat3_zero );
		J2.SetSize( 5, 6 );
	}
	else
	{
		J2.Zero( 5, 6 );
	}
	
	c1.SubVec3( 0 ) = -( invTimeStep * ERROR_REDUCTION ) * ( r.GetVec() * - ( float ) DEG2RAD( r.GetAngle() ) );
	
	c1[3] = -( invTimeStep * ERROR_REDUCTION ) * ( vecX * ofs );
	c1[4] = -( invTimeStep * ERROR_REDUCTION ) * ( vecY * ofs );
	
	c1.Clamp( -ERROR_REDUCTION_MAX, ERROR_REDUCTION_MAX );
}

/*
================
budAFConstraint_Slider::ApplyFriction
================
*/
void budAFConstraint_Slider::ApplyFriction( float invTimeStep )
{
	// no friction
}

/*
================
budAFConstraint_Slider::Translate
================
*/
void budAFConstraint_Slider::Translate( const budVec3& translation )
{
	if( !body2 )
	{
		offset += translation;
	}
}

/*
================
budAFConstraint_Slider::Rotate
================
*/
void budAFConstraint_Slider::Rotate( const budRotation& rotation )
{
	if( !body2 )
	{
		offset *= rotation;
	}
}

/*
================
budAFConstraint_Slider::GetCenter
================
*/
void budAFConstraint_Slider::GetCenter( budVec3& center )
{
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	if( master )
	{
		center = master->GetWorldOrigin() + master->GetWorldAxis() * offset - body1->GetWorldOrigin();
	}
	else
	{
		center = offset - body1->GetWorldOrigin();
	}
}

/*
================
budAFConstraint_Slider::DebugDraw
================
*/
void budAFConstraint_Slider::DebugDraw()
{
	budVec3 ofs;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	if( master )
	{
		ofs = master->GetWorldOrigin() + master->GetWorldAxis() * offset - body1->GetWorldOrigin();
	}
	else
	{
		ofs = offset - body1->GetWorldOrigin();
	}
	gameRenderWorld->DebugLine( colorGreen, ofs, ofs + axis * body1->GetWorldAxis() );
}

/*
================
budAFConstraint_Slider::Save
================
*/
void budAFConstraint_Slider::Save( idSaveGame* saveFile ) const
{
	budAFConstraint::Save( saveFile );
	saveFile->WriteVec3( axis );
	saveFile->WriteVec3( offset );
	saveFile->WriteMat3( relAxis );
}

/*
================
budAFConstraint_Slider::Restore
================
*/
void budAFConstraint_Slider::Restore( idRestoreGame* saveFile )
{
	budAFConstraint::Restore( saveFile );
	saveFile->ReadVec3( axis );
	saveFile->ReadVec3( offset );
	saveFile->ReadMat3( relAxis );
}


//===============================================================
//
//	budAFConstraint_Line
//
//===============================================================

/*
================
budAFConstraint_Line::budAFConstraint_Line
================
*/
budAFConstraint_Line::budAFConstraint_Line( const budStr& name, budAFBody* body1, budAFBody* body2 )
{
	assert( 0 );	// FIXME: implement
}

/*
================
budAFConstraint_Line::Evaluate
================
*/
void budAFConstraint_Line::Evaluate( float invTimeStep )
{
	assert( 0 );	// FIXME: implement
}

/*
================
budAFConstraint_Line::ApplyFriction
================
*/
void budAFConstraint_Line::ApplyFriction( float invTimeStep )
{
	assert( 0 );	// FIXME: implement
}

/*
================
budAFConstraint_Line::Translate
================
*/
void budAFConstraint_Line::Translate( const budVec3& translation )
{
	assert( 0 );	// FIXME: implement
}

/*
================
budAFConstraint_Line::Rotate
================
*/
void budAFConstraint_Line::Rotate( const budRotation& rotation )
{
	assert( 0 );	// FIXME: implement
}

/*
================
budAFConstraint_Line::DebugDraw
================
*/
void budAFConstraint_Line::DebugDraw()
{
	assert( 0 );	// FIXME: implement
}


//===============================================================
//
//	budAFConstraint_Plane
//
//===============================================================

/*
================
budAFConstraint_Plane::budAFConstraint_Plane
================
*/
budAFConstraint_Plane::budAFConstraint_Plane( const budStr& name, budAFBody* body1, budAFBody* body2 )
{
	assert( body1 );
	type = CONSTRAINT_PLANE;
	this->name = name;
	this->body1 = body1;
	this->body2 = body2;
	InitSize( 1 );
	fl.allowPrimary = true;
	fl.noCollision = true;
}

/*
================
budAFConstraint_Plane::SetPlane
================
*/
void budAFConstraint_Plane::SetPlane( const budVec3& normal, const budVec3& anchor )
{
	// get anchor relative to center of mass of body1
	anchor1 = ( anchor - body1->GetWorldOrigin() ) * body1->GetWorldAxis().Transpose();
	if( body2 )
	{
		// get anchor relative to center of mass of body2
		anchor2 = ( anchor - body2->GetWorldOrigin() ) * body2->GetWorldAxis().Transpose();
		planeNormal = normal * body2->GetWorldAxis().Transpose();
	}
	else
	{
		anchor2 = anchor;
		planeNormal = normal;
	}
}

/*
================
budAFConstraint_Plane::Evaluate
================
*/
void budAFConstraint_Plane::Evaluate( float invTimeStep )
{
	budVec3 a1, a2, normal, p;
	budVec6 v;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	a1 = body1->GetWorldOrigin() + anchor1 * body1->GetWorldAxis();
	if( master )
	{
		a2 = master->GetWorldOrigin() + anchor2 * master->GetWorldAxis();
		normal = planeNormal * master->GetWorldAxis();
	}
	else
	{
		a2 = anchor2;
		normal = planeNormal;
	}
	
	p = a1 - body1->GetWorldOrigin();
	v.SubVec3( 0 ) = normal;
	v.SubVec3( 1 ) = p.Cross( normal );
	J1.Set( 1, 6, v.ToFloatPtr() );
	
	if( body2 )
	{
		p = a1 - body2->GetWorldOrigin();
		v.SubVec3( 0 ) = -normal;
		v.SubVec3( 1 ) = p.Cross( -normal );
		J2.Set( 1, 6, v.ToFloatPtr() );
	}
	
	c1[0] = -( invTimeStep * ERROR_REDUCTION ) * ( a1 * normal - a2 * normal );
	
	c1.Clamp( -ERROR_REDUCTION_MAX, ERROR_REDUCTION_MAX );
}

/*
================
budAFConstraint_Plane::ApplyFriction
================
*/
void budAFConstraint_Plane::ApplyFriction( float invTimeStep )
{
	// no friction
}

/*
================
budAFConstraint_Plane::Translate
================
*/
void budAFConstraint_Plane::Translate( const budVec3& translation )
{
	if( !body2 )
	{
		anchor2 += translation;
	}
}

/*
================
budAFConstraint_Plane::Rotate
================
*/
void budAFConstraint_Plane::Rotate( const budRotation& rotation )
{
	if( !body2 )
	{
		anchor2 *= rotation;
		planeNormal *= rotation.ToMat3();
	}
}

/*
================
budAFConstraint_Plane::DebugDraw
================
*/
void budAFConstraint_Plane::DebugDraw()
{
	budVec3 a1, normal, right, up;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	a1 = body1->GetWorldOrigin() + anchor1 * body1->GetWorldAxis();
	if( master )
	{
		normal = planeNormal * master->GetWorldAxis();
	}
	else
	{
		normal = planeNormal;
	}
	normal.NormalVectors( right, up );
	normal *= 4.0f;
	right *= 4.0f;
	up *= 4.0f;
	
	gameRenderWorld->DebugLine( colorCyan, a1 - right, a1 + right );
	gameRenderWorld->DebugLine( colorCyan, a1 - up, a1 + up );
	gameRenderWorld->DebugArrow( colorCyan, a1, a1 + normal, 1 );
}

/*
================
budAFConstraint_Plane::Save
================
*/
void budAFConstraint_Plane::Save( idSaveGame* saveFile ) const
{
	budAFConstraint::Save( saveFile );
	saveFile->WriteVec3( anchor1 );
	saveFile->WriteVec3( anchor2 );
	saveFile->WriteVec3( planeNormal );
}

/*
================
budAFConstraint_Plane::Restore
================
*/
void budAFConstraint_Plane::Restore( idRestoreGame* saveFile )
{
	budAFConstraint::Restore( saveFile );
	saveFile->ReadVec3( anchor1 );
	saveFile->ReadVec3( anchor2 );
	saveFile->ReadVec3( planeNormal );
}


//===============================================================
//
//	budAFConstraint_Spring
//
//===============================================================

/*
================
budAFConstraint_Spring::budAFConstraint_Spring
================
*/
budAFConstraint_Spring::budAFConstraint_Spring( const budStr& name, budAFBody* body1, budAFBody* body2 )
{
	assert( body1 );
	type = CONSTRAINT_SPRING;
	this->name = name;
	this->body1 = body1;
	this->body2 = body2;
	InitSize( 1 );
	fl.allowPrimary = false;
	kstretch = kcompress = damping = 1.0f;
	minLength = maxLength = restLength = 0.0f;
}

/*
================
budAFConstraint_Spring::SetAnchor
================
*/
void budAFConstraint_Spring::SetAnchor( const budVec3& worldAnchor1, const budVec3& worldAnchor2 )
{
	// get anchor relative to center of mass of body1
	anchor1 = ( worldAnchor1 - body1->GetWorldOrigin() ) * body1->GetWorldAxis().Transpose();
	if( body2 )
	{
		// get anchor relative to center of mass of body2
		anchor2 = ( worldAnchor2 - body2->GetWorldOrigin() ) * body2->GetWorldAxis().Transpose();
	}
	else
	{
		anchor2 = worldAnchor2;
	}
}

/*
================
budAFConstraint_Spring::SetSpring
================
*/
void budAFConstraint_Spring::SetSpring( const float stretch, const float compress, const float damping, const float restLength )
{
	assert( stretch >= 0.0f && compress >= 0.0f && restLength >= 0.0f );
	this->kstretch = stretch;
	this->kcompress = compress;
	this->damping = damping;
	this->restLength = restLength;
}

/*
================
budAFConstraint_Spring::SetLimit
================
*/
void budAFConstraint_Spring::SetLimit( const float minLength, const float maxLength )
{
	assert( minLength >= 0.0f && maxLength >= 0.0f && maxLength >= minLength );
	this->minLength = minLength;
	this->maxLength = maxLength;
}

/*
================
budAFConstraint_Spring::Evaluate
================
*/
void budAFConstraint_Spring::Evaluate( float invTimeStep )
{
	budVec3 a1, a2, velocity1, velocity2, force;
	budVec6 v1, v2;
	float d, dampingForce, length, error;
	bool limit;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	a1 = body1->GetWorldOrigin() + anchor1 * body1->GetWorldAxis();
	velocity1 = body1->GetPointVelocity( a1 );
	
	if( master )
	{
		a2 = master->GetWorldOrigin() + anchor2 * master->GetWorldAxis();
		velocity2 = master->GetPointVelocity( a2 );
	}
	else
	{
		a2 = anchor2;
		velocity2.Zero();
	}
	
	force = a2 - a1;
	d = force * force;
	if( d != 0.0f )
	{
		dampingForce = damping * budMath::Fabs( ( velocity2 - velocity1 ) * force ) / d;
	}
	else
	{
		dampingForce = 0.0f;
	}
	length = force.Normalize();
	
	if( length > restLength )
	{
		if( kstretch > 0.0f )
		{
			budVec3 springForce = force * ( Square( length - restLength ) * kstretch - dampingForce );
			body1->AddForce( a1, springForce );
			if( master )
			{
				master->AddForce( a2, -springForce );
			}
		}
	}
	else
	{
		if( kcompress > 0.0f )
		{
			budVec3 springForce = force * -( Square( restLength - length ) * kcompress - dampingForce );
			body1->AddForce( a1, springForce );
			if( master )
			{
				master->AddForce( a2, -springForce );
			}
		}
	}
	
	// check for spring limits
	if( length < minLength )
	{
		force = -force;
		error = minLength - length;
		limit = true;
	}
	else if( maxLength > 0.0f && length > maxLength )
	{
		error = length - maxLength;
		limit = true;
	}
	else
	{
		error = 0.0f;
		limit = false;
	}
	
	if( limit )
	{
		a1 -= body1->GetWorldOrigin();
		v1.SubVec3( 0 ) = force;
		v1.SubVec3( 1 ) = a1.Cross( force );
		J1.Set( 1, 6, v1.ToFloatPtr() );
		if( body2 )
		{
			a2 -= body2->GetWorldOrigin();
			v2.SubVec3( 0 ) = -force;
			v2.SubVec3( 1 ) = a2.Cross( -force );
			J2.Set( 1, 6, v2.ToFloatPtr() );
		}
		c1[0] = -( invTimeStep * ERROR_REDUCTION ) * error;
		lo[0] = 0.0f;
	}
	else
	{
		J1.Zero( 0, 0 );
		J2.Zero( 0, 0 );
	}
	
	c1.Clamp( -ERROR_REDUCTION_MAX, ERROR_REDUCTION_MAX );
}

/*
================
budAFConstraint_Spring::ApplyFriction
================
*/
void budAFConstraint_Spring::ApplyFriction( float invTimeStep )
{
	// no friction
}

/*
================
budAFConstraint_Spring::Translate
================
*/
void budAFConstraint_Spring::Translate( const budVec3& translation )
{
	if( !body2 )
	{
		anchor2 += translation;
	}
}

/*
================
budAFConstraint_Spring::Rotate
================
*/
void budAFConstraint_Spring::Rotate( const budRotation& rotation )
{
	if( !body2 )
	{
		anchor2 *= rotation;
	}
}

/*
================
budAFConstraint_Spring::GetCenter
================
*/
void budAFConstraint_Spring::GetCenter( budVec3& center )
{
	budAFBody* master;
	budVec3 a1, a2;
	
	master = body2 ? body2 : physics->GetMasterBody();
	a1 = body1->GetWorldOrigin() + anchor1 * body1->GetWorldAxis();
	if( master )
	{
		a2 = master->GetWorldOrigin() + anchor2 * master->GetWorldAxis();
	}
	else
	{
		a2 = anchor2;
	}
	center = ( a1 + a2 ) * 0.5f;
}

/*
================
budAFConstraint_Spring::DebugDraw
================
*/
void budAFConstraint_Spring::DebugDraw()
{
	budAFBody* master;
	float length;
	budVec3 a1, a2, dir, mid, p;
	
	master = body2 ? body2 : physics->GetMasterBody();
	a1 = body1->GetWorldOrigin() + anchor1 * body1->GetWorldAxis();
	if( master )
	{
		a2 = master->GetWorldOrigin() + anchor2 * master->GetWorldAxis();
	}
	else
	{
		a2 = anchor2;
	}
	dir = a2 - a1;
	mid = a1 + 0.5f * dir;
	length = dir.Normalize();
	
	// draw spring
	gameRenderWorld->DebugLine( colorGreen, a1, a2 );
	
	// draw rest length
	p = restLength * 0.5f * dir;
	gameRenderWorld->DebugCircle( colorWhite, mid + p, dir, 1.0f, 10 );
	gameRenderWorld->DebugCircle( colorWhite, mid - p, dir, 1.0f, 10 );
	if( restLength > length )
	{
		gameRenderWorld->DebugLine( colorWhite, a2, mid + p );
		gameRenderWorld->DebugLine( colorWhite, a1, mid - p );
	}
	
	if( minLength > 0.0f )
	{
		// draw min length
		gameRenderWorld->DebugCircle( colorBlue, mid + minLength * 0.5f * dir, dir, 2.0f, 10 );
		gameRenderWorld->DebugCircle( colorBlue, mid - minLength * 0.5f * dir, dir, 2.0f, 10 );
	}
	
	if( maxLength > 0.0f )
	{
		// draw max length
		gameRenderWorld->DebugCircle( colorRed, mid + maxLength * 0.5f * dir, dir, 2.0f, 10 );
		gameRenderWorld->DebugCircle( colorRed, mid - maxLength * 0.5f * dir, dir, 2.0f, 10 );
	}
}

/*
================
budAFConstraint_Spring::Save
================
*/
void budAFConstraint_Spring::Save( idSaveGame* saveFile ) const
{
	budAFConstraint::Save( saveFile );
	saveFile->WriteVec3( anchor1 );
	saveFile->WriteVec3( anchor2 );
	saveFile->WriteFloat( kstretch );
	saveFile->WriteFloat( kcompress );
	saveFile->WriteFloat( damping );
	saveFile->WriteFloat( restLength );
	saveFile->WriteFloat( minLength );
	saveFile->WriteFloat( maxLength );
}

/*
================
budAFConstraint_Spring::Restore
================
*/
void budAFConstraint_Spring::Restore( idRestoreGame* saveFile )
{
	budAFConstraint::Restore( saveFile );
	saveFile->ReadVec3( anchor1 );
	saveFile->ReadVec3( anchor2 );
	saveFile->ReadFloat( kstretch );
	saveFile->ReadFloat( kcompress );
	saveFile->ReadFloat( damping );
	saveFile->ReadFloat( restLength );
	saveFile->ReadFloat( minLength );
	saveFile->ReadFloat( maxLength );
}


//===============================================================
//
//	budAFConstraint_Contact
//
//===============================================================

/*
================
budAFConstraint_Contact::budAFConstraint_Contact
================
*/
budAFConstraint_Contact::budAFConstraint_Contact()
{
	name = "contact";
	type = CONSTRAINT_CONTACT;
	InitSize( 1 );
	fc = NULL;
	fl.allowPrimary = false;
	fl.frameConstraint = true;
}

/*
================
budAFConstraint_Contact::~budAFConstraint_Contact
================
*/
budAFConstraint_Contact::~budAFConstraint_Contact()
{
	if( fc )
	{
		delete fc;
	}
}

/*
================
budAFConstraint_Contact::Setup
================
*/
void budAFConstraint_Contact::Setup( budAFBody* b1, budAFBody* b2, contactInfo_t& c )
{
	budVec3 p;
	budVec6 v;
	float vel;
	float minBounceVelocity = 2.0f;
	
	assert( b1 );
	
	body1 = b1;
	body2 = b2;
	contact = c;
	
	p = c.point - body1->GetWorldOrigin();
	v.SubVec3( 0 ) = c.normal;
	v.SubVec3( 1 ) = p.Cross( c.normal );
	J1.Set( 1, 6, v.ToFloatPtr() );
	vel = v.SubVec3( 0 ) * body1->GetLinearVelocity() + v.SubVec3( 1 ) * body1->GetAngularVelocity();
	
	if( body2 )
	{
		p = c.point - body2->GetWorldOrigin();
		v.SubVec3( 0 ) = -c.normal;
		v.SubVec3( 1 ) = p.Cross( -c.normal );
		J2.Set( 1, 6, v.ToFloatPtr() );
		vel += v.SubVec3( 0 ) * body2->GetLinearVelocity() + v.SubVec3( 1 ) * body2->GetAngularVelocity();
		c2[0] = 0.0f;
	}
	
	if( body1->GetBouncyness() > 0.0f && -vel > minBounceVelocity )
	{
		c1[0] = body1->GetBouncyness() * vel;
	}
	else
	{
		c1[0] = 0.0f;
	}
	
	e[0] = CONTACT_LCP_EPSILON;
	lo[0] = 0.0f;
	hi[0] = budMath::INFINITY;
	boxConstraint = NULL;
	boxIndex[0] = -1;
}

/*
================
budAFConstraint_Contact::Evaluate
================
*/
void budAFConstraint_Contact::Evaluate( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_Contact::ApplyFriction
================
*/
void budAFConstraint_Contact::ApplyFriction( float invTimeStep )
{
	budVec3 r, velocity, normal, dir1, dir2;
	float friction, magnitude, forceNumerator, forceDenominator;
	budVecX impulse, dv;
	
	if( !body1 )
	{
		return;
	}
	friction = body1->GetContactFriction();
	if( body2 && body2->GetContactFriction() < friction )
	{
		friction = body2->GetContactFriction();
	}
	
	friction *= physics->GetContactFrictionScale();
	
	if( friction <= 0.0f )
	{
		return;
	}
	
	// separate friction per contact is silly but it's fast and often looks close enough
	if( af_useImpulseFriction.GetBool() )
	{
	
		impulse.SetData( 6, VECX_ALLOCA( 6 ) );
		dv.SetData( 6, VECX_ALLOCA( 6 ) );
		
		// calculate velocity in the contact plane
		r = contact.point - body1->GetWorldOrigin();
		velocity = body1->GetLinearVelocity() + body1->GetAngularVelocity().Cross( r );
		velocity -= contact.normal * velocity * contact.normal;
		
		// get normalized direction of friction and magnitude of velocity
		normal = -velocity;
		magnitude = normal.Normalize();
		
		forceNumerator = friction * magnitude;
		forceDenominator = body1->GetInverseMass() + ( ( body1->GetInverseWorldInertia() * r.Cross( normal ) ).Cross( r ) * normal );
		impulse.SubVec3( 0 ) = ( forceNumerator / forceDenominator ) * normal;
		impulse.SubVec3( 1 ) = r.Cross( impulse.SubVec3( 0 ) );
		body1->InverseWorldSpatialInertiaMultiply( dv, impulse.ToFloatPtr() );
		
		// modify velocity with friction force
		body1->SetLinearVelocity( body1->GetLinearVelocity() + dv.SubVec3( 0 ) );
		body1->SetAngularVelocity( body1->GetAngularVelocity() + dv.SubVec3( 1 ) );
	}
	else
	{
	
		if( !fc )
		{
			fc = new( TAG_PHYSICS_AF ) budAFConstraint_ContactFriction;
		}
		// call setup each frame because contact constraints are re-used for different bodies
		fc->Setup( this );
		fc->Add( physics, invTimeStep );
	}
}

/*
================
budAFConstraint_Contact::Translate
================
*/
void budAFConstraint_Contact::Translate( const budVec3& translation )
{
	assert( 0 );	// contact should never be translated
}

/*
================
budAFConstraint_Contact::Rotate
================
*/
void budAFConstraint_Contact::Rotate( const budRotation& rotation )
{
	assert( 0 );	// contact should never be rotated
}

/*
================
budAFConstraint_Contact::GetCenter
================
*/
void budAFConstraint_Contact::GetCenter( budVec3& center )
{
	center = contact.point;
}

/*
================
budAFConstraint_Contact::DebugDraw
================
*/
void budAFConstraint_Contact::DebugDraw()
{
	budVec3 x, y;
	contact.normal.NormalVectors( x, y );
	gameRenderWorld->DebugLine( colorWhite, contact.point, contact.point + 6.0f * contact.normal );
	gameRenderWorld->DebugLine( colorWhite, contact.point - 2.0f * x, contact.point + 2.0f * x );
	gameRenderWorld->DebugLine( colorWhite, contact.point - 2.0f * y, contact.point + 2.0f * y );
}


//===============================================================
//
//	budAFConstraint_ContactFriction
//
//===============================================================

/*
================
budAFConstraint_ContactFriction::budAFConstraint_ContactFriction
================
*/
budAFConstraint_ContactFriction::budAFConstraint_ContactFriction()
{
	type = CONSTRAINT_FRICTION;
	name = "contactFriction";
	InitSize( 2 );
	cc = NULL;
	fl.allowPrimary = false;
	fl.frameConstraint = true;
}

/*
================
budAFConstraint_ContactFriction::Setup
================
*/
void budAFConstraint_ContactFriction::Setup( budAFConstraint_Contact* cc )
{
	this->cc = cc;
	body1 = cc->GetBody1();
	body2 = cc->GetBody2();
}

/*
================
budAFConstraint_ContactFriction::Evaluate
================
*/
void budAFConstraint_ContactFriction::Evaluate( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_ContactFriction::ApplyFriction
================
*/
void budAFConstraint_ContactFriction::ApplyFriction( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_ContactFriction::Add
================
*/
bool budAFConstraint_ContactFriction::Add( idPhysics_AF* phys, float invTimeStep )
{
	budVec3 r, dir1, dir2;
	float friction;
	int newRow;
	
	physics = phys;
	
	friction = body1->GetContactFriction() * physics->GetContactFrictionScale();
	
	// if the body only has friction in one direction
	if( body1->GetFrictionDirection( dir1 ) )
	{
		// project the friction direction into the contact plane
		dir1 -= dir1 * cc->GetContact().normal * dir1;
		dir1.Normalize();
		
		r = cc->GetContact().point - body1->GetWorldOrigin();
		
		J1.SetSize( 1, 6 );
		J1.SubVec6( 0 ).SubVec3( 0 ) = dir1;
		J1.SubVec6( 0 ).SubVec3( 1 ) = r.Cross( dir1 );
		c1.SetSize( 1 );
		c1[0] = 0.0f;
		
		if( body2 )
		{
			r = cc->GetContact().point - body2->GetWorldOrigin();
			
			J2.SetSize( 1, 6 );
			J2.SubVec6( 0 ).SubVec3( 0 ) = -dir1;
			J2.SubVec6( 0 ).SubVec3( 1 ) = r.Cross( -dir1 );
			c2.SetSize( 1 );
			c2[0] = 0.0f;
		}
		
		lo[0] = -friction;
		hi[0] = friction;
		boxConstraint = cc;
		boxIndex[0] = 0;
	}
	else
	{
		// get two friction directions orthogonal to contact normal
		cc->GetContact().normal.NormalVectors( dir1, dir2 );
		
		r = cc->GetContact().point - body1->GetWorldOrigin();
		
		J1.SetSize( 2, 6 );
		J1.SubVec6( 0 ).SubVec3( 0 ) = dir1;
		J1.SubVec6( 0 ).SubVec3( 1 ) = r.Cross( dir1 );
		J1.SubVec6( 1 ).SubVec3( 0 ) = dir2;
		J1.SubVec6( 1 ).SubVec3( 1 ) = r.Cross( dir2 );
		c1.SetSize( 2 );
		c1[0] = c1[1] = 0.0f;
		
		if( body2 )
		{
			r = cc->GetContact().point - body2->GetWorldOrigin();
			
			J2.SetSize( 2, 6 );
			J2.SubVec6( 0 ).SubVec3( 0 ) = -dir1;
			J2.SubVec6( 0 ).SubVec3( 1 ) = r.Cross( -dir1 );
			J2.SubVec6( 1 ).SubVec3( 0 ) = -dir2;
			J2.SubVec6( 1 ).SubVec3( 1 ) = r.Cross( -dir2 );
			c2.SetSize( 2 );
			c2[0] = c2[1] = 0.0f;
			
			if( body2->GetContactFriction() < friction )
			{
				friction = body2->GetContactFriction();
			}
		}
		
		lo[0] = -friction;
		hi[0] = friction;
		boxConstraint = cc;
		boxIndex[0] = 0;
		lo[1] = -friction;
		hi[1] = friction;
		boxIndex[1] = 0;
	}
	
	if( body1->GetContactMotorDirection( dir1 ) && body1->GetContactMotorForce() > 0.0f )
	{
		// project the motor force direction into the contact plane
		dir1 -= dir1 * cc->GetContact().normal * dir1;
		dir1.Normalize();
		
		r = cc->GetContact().point - body1->GetWorldOrigin();
		
		newRow = J1.GetNumRows();
		J1.ChangeSize( newRow + 1, J1.GetNumColumns() );
		J1.SubVec6( newRow ).SubVec3( 0 ) = -dir1;
		J1.SubVec6( newRow ).SubVec3( 1 ) = r.Cross( -dir1 );
		c1.ChangeSize( newRow + 1 );
		c1[newRow] = body1->GetContactMotorVelocity();
		
		if( body2 )
		{
			r = cc->GetContact().point - body2->GetWorldOrigin();
			
			J2.ChangeSize( newRow + 1, J2.GetNumColumns() );
			J2.SubVec6( newRow ).SubVec3( 0 ) = -dir1;
			J2.SubVec6( newRow ).SubVec3( 1 ) = r.Cross( -dir1 );
			c2.ChangeSize( newRow + 1 );
			c2[newRow] = 0.0f;
		}
		
		lo[newRow] = -body1->GetContactMotorForce();
		hi[newRow] = body1->GetContactMotorForce();
		boxIndex[newRow] = -1;
	}
	
	physics->AddFrameConstraint( this );
	
	return true;
}

/*
================
budAFConstraint_ContactFriction::Translate
================
*/
void budAFConstraint_ContactFriction::Translate( const budVec3& translation )
{
}

/*
================
budAFConstraint_ContactFriction::Rotate
================
*/
void budAFConstraint_ContactFriction::Rotate( const budRotation& rotation )
{
}

/*
================
budAFConstraint_ContactFriction::DebugDraw
================
*/
void budAFConstraint_ContactFriction::DebugDraw()
{
}


//===============================================================
//
//	budAFConstraint_ConeLimit
//
//===============================================================

/*
================
budAFConstraint_ConeLimit::budAFConstraint_ConeLimit
================
*/
budAFConstraint_ConeLimit::budAFConstraint_ConeLimit()
{
	type = CONSTRAINT_CONELIMIT;
	name = "coneLimit";
	InitSize( 1 );
	fl.allowPrimary = false;
	fl.frameConstraint = true;
}

/*
================
budAFConstraint_ConeLimit::Setup

  the coneAnchor is the top of the cone in body2 space
  the coneAxis is the axis of the cone in body2 space
  the coneAngle is the angle the cone hull makes at the top
  the body1Axis is the axis in body1 space that should stay within the cone
================
*/
void budAFConstraint_ConeLimit::Setup( budAFBody* b1, budAFBody* b2, const budVec3& coneAnchor, const budVec3& coneAxis, const float coneAngle, const budVec3& body1Axis )
{
	this->body1 = b1;
	this->body2 = b2;
	this->coneAxis = coneAxis;
	this->coneAxis.Normalize();
	this->coneAnchor = coneAnchor;
	this->body1Axis = body1Axis;
	this->body1Axis.Normalize();
	this->cosAngle = ( float ) cos( DEG2RAD( coneAngle * 0.5f ) );
	this->sinHalfAngle = ( float ) sin( DEG2RAD( coneAngle * 0.25f ) );
	this->cosHalfAngle = ( float ) cos( DEG2RAD( coneAngle * 0.25f ) );
}

/*
================
budAFConstraint_ConeLimit::SetAnchor
================
*/
void budAFConstraint_ConeLimit::SetAnchor( const budVec3& coneAnchor )
{
	this->coneAnchor = coneAnchor;
}

/*
================
budAFConstraint_ConeLimit::SetBody1Axis
================
*/
void budAFConstraint_ConeLimit::SetBody1Axis( const budVec3& body1Axis )
{
	this->body1Axis = body1Axis;
}

/*
================
budAFConstraint_ConeLimit::Evaluate
================
*/
void budAFConstraint_ConeLimit::Evaluate( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_ConeLimit::ApplyFriction
================
*/
void budAFConstraint_ConeLimit::ApplyFriction( float invTimeStep )
{
}

/*
================
budAFConstraint_ConeLimit::Add
================
*/
bool budAFConstraint_ConeLimit::Add( idPhysics_AF* phys, float invTimeStep )
{
	float a;
	budVec6 J1row, J2row;
	budVec3 ax, anchor, body1ax, normal, coneVector, p1, p2;
	idQuat q;
	budAFBody* master;
	
	if( af_skipLimits.GetBool() )
	{
		lm.Zero();	// constraint exerts no force
		return false;
	}
	
	physics = phys;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	if( master )
	{
		ax = coneAxis * master->GetWorldAxis();
		anchor = master->GetWorldOrigin() + coneAnchor * master->GetWorldAxis();
	}
	else
	{
		ax = coneAxis;
		anchor = coneAnchor;
	}
	
	body1ax = body1Axis * body1->GetWorldAxis();
	
	a = ax * body1ax;
	
	// if the body1 axis is inside the cone
	if( a > cosAngle )
	{
		lm.Zero();	// constraint exerts no force
		return false;
	}
	
	// calculate the inward cone normal for the position the body1 axis went outside the cone
	normal = body1ax.Cross( ax );
	normal.Normalize();
	q.x = normal.x * sinHalfAngle;
	q.y = normal.y * sinHalfAngle;
	q.z = normal.z * sinHalfAngle;
	q.w = cosHalfAngle;
	coneVector = ax * q.ToMat3();
	normal = coneVector.Cross( ax ).Cross( coneVector );
	normal.Normalize();
	
	p1 = anchor + 32.0f * coneVector - body1->GetWorldOrigin();
	
	J1row.SubVec3( 0 ) = normal;
	J1row.SubVec3( 1 ) = p1.Cross( normal );
	J1.Set( 1, 6, J1row.ToFloatPtr() );
	
	c1[0] = ( invTimeStep * LIMIT_ERROR_REDUCTION ) * ( normal * ( 32.0f * body1ax ) );
	
	if( body2 )
	{
	
		p2 = anchor + 32.0f * coneVector - master->GetWorldOrigin();
		
		J2row.SubVec3( 0 ) = -normal;
		J2row.SubVec3( 1 ) = p2.Cross( -normal );
		J2.Set( 1, 6, J2row.ToFloatPtr() );
		
		c2[0] = 0.0f;
	}
	
	lo[0] = 0.0f;
	e[0] = LIMIT_LCP_EPSILON;
	
	physics->AddFrameConstraint( this );
	
	return true;
}

/*
================
budAFConstraint_ConeLimit::Translate
================
*/
void budAFConstraint_ConeLimit::Translate( const budVec3& translation )
{
	if( !body2 )
	{
		coneAnchor += translation;
	}
}

/*
================
budAFConstraint_ConeLimit::Rotate
================
*/
void budAFConstraint_ConeLimit::Rotate( const budRotation& rotation )
{
	if( !body2 )
	{
		coneAnchor *= rotation;
		coneAxis *= rotation.ToMat3();
	}
}

/*
================
budAFConstraint_ConeLimit::DebugDraw
================
*/
void budAFConstraint_ConeLimit::DebugDraw()
{
	budVec3 ax, anchor, x, y, z, start, end;
	float sinAngle, a, size = 10.0f;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	if( master )
	{
		ax = coneAxis * master->GetWorldAxis();
		anchor = master->GetWorldOrigin() + coneAnchor * master->GetWorldAxis();
	}
	else
	{
		ax = coneAxis;
		anchor = coneAnchor;
	}
	
	// draw body1 axis
	gameRenderWorld->DebugLine( colorGreen, anchor, anchor + size * ( body1Axis * body1->GetWorldAxis() ) );
	
	// draw cone
	ax.NormalVectors( x, y );
	sinAngle = budMath::Sqrt( 1.0f - cosAngle * cosAngle );
	x *= size * sinAngle;
	y *= size * sinAngle;
	z = anchor + ax * size * cosAngle;
	start = x + z;
	for( a = 0.0f; a < 360.0f; a += 45.0f )
	{
		end = x * ( float ) cos( DEG2RAD( a + 45.0f ) ) + y * ( float ) sin( DEG2RAD( a + 45.0f ) ) + z;
		gameRenderWorld->DebugLine( colorMagenta, anchor, start );
		gameRenderWorld->DebugLine( colorMagenta, start, end );
		start = end;
	}
}

/*
================
budAFConstraint_ConeLimit::Save
================
*/
void budAFConstraint_ConeLimit::Save( idSaveGame* saveFile ) const
{
	budAFConstraint::Save( saveFile );
	saveFile->WriteVec3( coneAnchor );
	saveFile->WriteVec3( coneAxis );
	saveFile->WriteVec3( body1Axis );
	saveFile->WriteFloat( cosAngle );
	saveFile->WriteFloat( sinHalfAngle );
	saveFile->WriteFloat( cosHalfAngle );
	saveFile->WriteFloat( epsilon );
}

/*
================
budAFConstraint_ConeLimit::Restore
================
*/
void budAFConstraint_ConeLimit::Restore( idRestoreGame* saveFile )
{
	budAFConstraint::Restore( saveFile );
	saveFile->ReadVec3( coneAnchor );
	saveFile->ReadVec3( coneAxis );
	saveFile->ReadVec3( body1Axis );
	saveFile->ReadFloat( cosAngle );
	saveFile->ReadFloat( sinHalfAngle );
	saveFile->ReadFloat( cosHalfAngle );
	saveFile->ReadFloat( epsilon );
}


//===============================================================
//
//	budAFConstraint_PyramidLimit
//
//===============================================================

/*
================
budAFConstraint_PyramidLimit::budAFConstraint_PyramidLimit
================
*/
budAFConstraint_PyramidLimit::budAFConstraint_PyramidLimit()
{
	type = CONSTRAINT_PYRAMIDLIMIT;
	name = "pyramidLimit";
	InitSize( 1 );
	fl.allowPrimary = false;
	fl.frameConstraint = true;
}

/*
================
budAFConstraint_PyramidLimit::Setup
================
*/
void budAFConstraint_PyramidLimit::Setup( budAFBody* b1, budAFBody* b2, const budVec3& pyrambudAnchor,
		const budVec3& pyrambudAxis, const budVec3& baseAxis,
		const float pyrambudAngle1, const float pyrambudAngle2, const budVec3& body1Axis )
{
	body1 = b1;
	body2 = b2;
	// setup the base and make sure the basis is orthonormal
	pyramidBasis[2] = pyrambudAxis;
	pyramidBasis[2].Normalize();
	pyramidBasis[0] = baseAxis;
	pyramidBasis[0] -= pyramidBasis[2] * baseAxis * pyramidBasis[2];
	pyramidBasis[0].Normalize();
	pyramidBasis[1] = pyramidBasis[0].Cross( pyramidBasis[2] );
	// pyramid top
	this->pyrambudAnchor = pyrambudAnchor;
	// angles
	cosAngle[0] = ( float ) cos( DEG2RAD( pyrambudAngle1 * 0.5f ) );
	cosAngle[1] = ( float ) cos( DEG2RAD( pyrambudAngle2 * 0.5f ) );
	sinHalfAngle[0] = ( float ) sin( DEG2RAD( pyrambudAngle1 * 0.25f ) );
	sinHalfAngle[1] = ( float ) sin( DEG2RAD( pyrambudAngle2 * 0.25f ) );
	cosHalfAngle[0] = ( float ) cos( DEG2RAD( pyrambudAngle1 * 0.25f ) );
	cosHalfAngle[1] = ( float ) cos( DEG2RAD( pyrambudAngle2 * 0.25f ) );
	
	this->body1Axis = body1Axis;
}

/*
================
budAFConstraint_PyramidLimit::SetAnchor
================
*/
void budAFConstraint_PyramidLimit::SetAnchor( const budVec3& pyrambudAnchor )
{
	this->pyrambudAnchor = pyrambudAnchor;
}

/*
================
budAFConstraint_PyramidLimit::SetBody1Axis
================
*/
void budAFConstraint_PyramidLimit::SetBody1Axis( const budVec3& body1Axis )
{
	this->body1Axis = body1Axis;
}

/*
================
budAFConstraint_PyramidLimit::Evaluate
================
*/
void budAFConstraint_PyramidLimit::Evaluate( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_PyramidLimit::ApplyFriction
================
*/
void budAFConstraint_PyramidLimit::ApplyFriction( float invTimeStep )
{
}

/*
================
budAFConstraint_PyramidLimit::Add
================
*/
bool budAFConstraint_PyramidLimit::Add( idPhysics_AF* phys, float invTimeStep )
{
	int i;
	float a[2];
	budVec6 J1row, J2row;
	budMat3 worldBase;
	budVec3 anchor, body1ax, ax[2], v, normal, pyrambudVector, p1, p2;
	idQuat q;
	budAFBody* master;
	
	if( af_skipLimits.GetBool() )
	{
		lm.Zero();	// constraint exerts no force
		return false;
	}
	
	physics = phys;
	master = body2 ? body2 : physics->GetMasterBody();
	
	if( master )
	{
		worldBase[0] = pyramidBasis[0] * master->GetWorldAxis();
		worldBase[1] = pyramidBasis[1] * master->GetWorldAxis();
		worldBase[2] = pyramidBasis[2] * master->GetWorldAxis();
		anchor = master->GetWorldOrigin() + pyrambudAnchor * master->GetWorldAxis();
	}
	else
	{
		worldBase = pyramidBasis;
		anchor = pyrambudAnchor;
	}
	
	body1ax = body1Axis * body1->GetWorldAxis();
	
	for( i = 0; i < 2; i++ )
	{
		ax[i] = body1ax - worldBase[!i] * body1ax * worldBase[!i];
		ax[i].Normalize();
		a[i] = worldBase[2] * ax[i];
	}
	
	// if the body1 axis is inside the pyramid
	if( a[0] > cosAngle[0] && a[1] > cosAngle[1] )
	{
		lm.Zero();	// constraint exerts no force
		return false;
	}
	
	// calculate the inward pyramid normal for the position the body1 axis went outside the pyramid
	pyrambudVector = worldBase[2];
	for( i = 0; i < 2; i++ )
	{
		if( a[i] <= cosAngle[i] )
		{
			v = ax[i].Cross( worldBase[2] );
			v.Normalize();
			q.x = v.x * sinHalfAngle[i];
			q.y = v.y * sinHalfAngle[i];
			q.z = v.z * sinHalfAngle[i];
			q.w = cosHalfAngle[i];
			pyrambudVector *= q.ToMat3();
		}
	}
	normal = pyrambudVector.Cross( worldBase[2] ).Cross( pyrambudVector );
	normal.Normalize();
	
	p1 = anchor + 32.0f * pyrambudVector - body1->GetWorldOrigin();
	
	J1row.SubVec3( 0 ) = normal;
	J1row.SubVec3( 1 ) = p1.Cross( normal );
	J1.Set( 1, 6, J1row.ToFloatPtr() );
	
	c1[0] = ( invTimeStep * LIMIT_ERROR_REDUCTION ) * ( normal * ( 32.0f * body1ax ) );
	
	if( body2 )
	{
	
		p2 = anchor + 32.0f * pyrambudVector - master->GetWorldOrigin();
		
		J2row.SubVec3( 0 ) = -normal;
		J2row.SubVec3( 1 ) = p2.Cross( -normal );
		J2.Set( 1, 6, J2row.ToFloatPtr() );
		
		c2[0] = 0.0f;
	}
	
	lo[0] = 0.0f;
	e[0] = LIMIT_LCP_EPSILON;
	
	physics->AddFrameConstraint( this );
	
	return true;
}

/*
================
budAFConstraint_PyramidLimit::Translate
================
*/
void budAFConstraint_PyramidLimit::Translate( const budVec3& translation )
{
	if( !body2 )
	{
		pyrambudAnchor += translation;
	}
}

/*
================
budAFConstraint_PyramidLimit::Rotate
================
*/
void budAFConstraint_PyramidLimit::Rotate( const budRotation& rotation )
{
	if( !body2 )
	{
		pyrambudAnchor *= rotation;
		pyramidBasis[0] *= rotation.ToMat3();
		pyramidBasis[1] *= rotation.ToMat3();
		pyramidBasis[2] *= rotation.ToMat3();
	}
}

/*
================
budAFConstraint_PyramidLimit::DebugDraw
================
*/
void budAFConstraint_PyramidLimit::DebugDraw()
{
	int i;
	float size = 10.0f;
	budVec3 anchor, dir, p[4];
	budMat3 worldBase, m[2];
	idQuat q;
	budAFBody* master;
	
	master = body2 ? body2 : physics->GetMasterBody();
	
	if( master )
	{
		worldBase[0] = pyramidBasis[0] * master->GetWorldAxis();
		worldBase[1] = pyramidBasis[1] * master->GetWorldAxis();
		worldBase[2] = pyramidBasis[2] * master->GetWorldAxis();
		anchor = master->GetWorldOrigin() + pyrambudAnchor * master->GetWorldAxis();
	}
	else
	{
		worldBase = pyramidBasis;
		anchor = pyrambudAnchor;
	}
	
	// draw body1 axis
	gameRenderWorld->DebugLine( colorGreen, anchor, anchor + size * ( body1Axis * body1->GetWorldAxis() ) );
	
	// draw the pyramid
	for( i = 0; i < 2; i++ )
	{
		q.x = worldBase[!i].x * sinHalfAngle[i];
		q.y = worldBase[!i].y * sinHalfAngle[i];
		q.z = worldBase[!i].z * sinHalfAngle[i];
		q.w = cosHalfAngle[i];
		m[i] = q.ToMat3();
	}
	
	dir = worldBase[2] * size;
	p[0] = anchor + m[0] * ( m[1] * dir );
	p[1] = anchor + m[0] * ( m[1].Transpose() * dir );
	p[2] = anchor + m[0].Transpose() * ( m[1].Transpose() * dir );
	p[3] = anchor + m[0].Transpose() * ( m[1] * dir );
	
	for( i = 0; i < 4; i++ )
	{
		gameRenderWorld->DebugLine( colorMagenta, anchor, p[i] );
		gameRenderWorld->DebugLine( colorMagenta, p[i], p[( i + 1 ) & 3] );
	}
}

/*
================
budAFConstraint_PyramidLimit::Save
================
*/
void budAFConstraint_PyramidLimit::Save( idSaveGame* saveFile ) const
{
	budAFConstraint::Save( saveFile );
	saveFile->WriteVec3( pyrambudAnchor );
	saveFile->WriteMat3( pyramidBasis );
	saveFile->WriteVec3( body1Axis );
	saveFile->WriteFloat( cosAngle[0] );
	saveFile->WriteFloat( cosAngle[1] );
	saveFile->WriteFloat( sinHalfAngle[0] );
	saveFile->WriteFloat( sinHalfAngle[1] );
	saveFile->WriteFloat( cosHalfAngle[0] );
	saveFile->WriteFloat( cosHalfAngle[1] );
	saveFile->WriteFloat( epsilon );
}

/*
================
budAFConstraint_PyramidLimit::Restore
================
*/
void budAFConstraint_PyramidLimit::Restore( idRestoreGame* saveFile )
{
	budAFConstraint::Restore( saveFile );
	saveFile->ReadVec3( pyrambudAnchor );
	saveFile->ReadMat3( pyramidBasis );
	saveFile->ReadVec3( body1Axis );
	saveFile->ReadFloat( cosAngle[0] );
	saveFile->ReadFloat( cosAngle[1] );
	saveFile->ReadFloat( sinHalfAngle[0] );
	saveFile->ReadFloat( sinHalfAngle[1] );
	saveFile->ReadFloat( cosHalfAngle[0] );
	saveFile->ReadFloat( cosHalfAngle[1] );
	saveFile->ReadFloat( epsilon );
}


//===============================================================
//
//	budAFConstraint_Suspension
//
//===============================================================

/*
================
budAFConstraint_Suspension::budAFConstraint_Suspension
================
*/
budAFConstraint_Suspension::budAFConstraint_Suspension()
{
	type = CONSTRAINT_SUSPENSION;
	name = "suspension";
	InitSize( 3 );
	fl.allowPrimary = false;
	fl.frameConstraint = true;
	
	localOrigin.Zero();
	localAxis.Identity();
	suspensionUp = 0.0f;
	suspensionDown = 0.0f;
	suspensionKCompress = 0.0f;
	suspensionDamping = 0.0f;
	steerAngle = 0.0f;
	friction = 2.0f;
	motorEnabled = false;
	motorForce = 0.0f;
	motorVelocity = 0.0f;
	wheelModel = NULL;
	memset( &trace, 0, sizeof( trace ) );
	epsilon = LCP_EPSILON;
}

/*
================
budAFConstraint_Suspension::Setup
================
*/
void budAFConstraint_Suspension::Setup( const char* name, budAFBody* body, const budVec3& origin, const budMat3& axis, budClipModel* clipModel )
{
	this->name = name;
	body1 = body;
	body2 = NULL;
	localOrigin = ( origin - body->GetWorldOrigin() ) * body->GetWorldAxis().Transpose();
	localAxis = axis * body->GetWorldAxis().Transpose();
	wheelModel = clipModel;
}

/*
================
budAFConstraint_Suspension::SetSuspension
================
*/
void budAFConstraint_Suspension::SetSuspension( const float up, const float down, const float k, const float d, const float f )
{
	suspensionUp = up;
	suspensionDown = down;
	suspensionKCompress = k;
	suspensionDamping = d;
	friction = f;
}

/*
================
budAFConstraint_Suspension::GetWheelOrigin
================
*/
const budVec3 budAFConstraint_Suspension::GetWheelOrigin() const
{
	return body1->GetWorldOrigin() + wheelOffset * body1->GetWorldAxis();
}

/*
================
budAFConstraint_Suspension::Evaluate
================
*/
void budAFConstraint_Suspension::Evaluate( float invTimeStep )
{
	float velocity, suspensionLength, springLength, compression, dampingForce, springForce;
	budVec3 origin, start, end, vel1, vel2, springDir, r, frictionDir, motorDir;
	budMat3 axis;
	budRotation rotation;
	
	axis = localAxis * body1->GetWorldAxis();
	origin = body1->GetWorldOrigin() + localOrigin * body1->GetWorldAxis();
	start = origin + suspensionUp * axis[2];
	end = origin - suspensionDown * axis[2];
	
	rotation.SetVec( axis[2] );
	rotation.SetAngle( steerAngle );
	
	axis *= rotation.ToMat3();
	
	gameLocal.clip.Translation( trace, start, end, wheelModel, axis, MASK_SOLID, NULL );
	
	wheelOffset = ( trace.endpos - body1->GetWorldOrigin() ) * body1->GetWorldAxis().Transpose();
	
	if( trace.fraction >= 1.0f )
	{
		J1.SetSize( 0, 6 );
		if( body2 )
		{
			J2.SetSize( 0, 6 );
		}
		return;
	}
	
	// calculate and add spring force
	vel1 = body1->GetPointVelocity( start );
	if( body2 )
	{
		vel2 = body2->GetPointVelocity( trace.c.point );
	}
	else
	{
		vel2.Zero();
	}
	
	suspensionLength = suspensionUp + suspensionDown;
	springDir = trace.endpos - start;
	springLength = trace.fraction * suspensionLength;
	dampingForce = suspensionDamping * budMath::Fabs( ( vel2 - vel1 ) * springDir ) / ( 1.0f + springLength * springLength );
	compression = suspensionLength - springLength;
	springForce = compression * compression * suspensionKCompress - dampingForce;
	
	r = trace.c.point - body1->GetWorldOrigin();
	J1.SetSize( 2, 6 );
	J1.SubVec6( 0 ).SubVec3( 0 ) = trace.c.normal;
	J1.SubVec6( 0 ).SubVec3( 1 ) = r.Cross( trace.c.normal );
	c1.SetSize( 2 );
	c1[0] = 0.0f;
	velocity = J1.SubVec6( 0 ).SubVec3( 0 ) * body1->GetLinearVelocity() + J1.SubVec6( 0 ).SubVec3( 1 ) * body1->GetAngularVelocity();
	
	if( body2 )
	{
		r = trace.c.point - body2->GetWorldOrigin();
		J2.SetSize( 2, 6 );
		J2.SubVec6( 0 ).SubVec3( 0 ) = -trace.c.normal;
		J2.SubVec6( 0 ).SubVec3( 1 ) = r.Cross( -trace.c.normal );
		c2.SetSize( 2 );
		c2[0] = 0.0f;
		velocity += J2.SubVec6( 0 ).SubVec3( 0 ) * body2->GetLinearVelocity() + J2.SubVec6( 0 ).SubVec3( 1 ) * body2->GetAngularVelocity();
	}
	
	c1[0] = -compression;		// + 0.5f * -velocity;
	
	e[0] = 1e-4f;
	lo[0] = 0.0f;
	hi[0] = springForce;
	boxConstraint = NULL;
	boxIndex[0] = -1;
	
	// project the friction direction into the contact plane
	frictionDir = axis[1] - axis[1] * trace.c.normal * axis[1];
	frictionDir.Normalize();
	
	r = trace.c.point - body1->GetWorldOrigin();
	
	J1.SubVec6( 1 ).SubVec3( 0 ) = frictionDir;
	J1.SubVec6( 1 ).SubVec3( 1 ) = r.Cross( frictionDir );
	c1[1] = 0.0f;
	
	if( body2 )
	{
		r = trace.c.point - body2->GetWorldOrigin();
		
		J2.SubVec6( 1 ).SubVec3( 0 ) = -frictionDir;
		J2.SubVec6( 1 ).SubVec3( 1 ) = r.Cross( -frictionDir );
		c2[1] = 0.0f;
	}
	
	lo[1] = -friction * physics->GetContactFrictionScale();
	hi[1] = friction * physics->GetContactFrictionScale();
	
	boxConstraint = this;
	boxIndex[1] = 0;
	
	
	if( motorEnabled )
	{
		// project the motor force direction into the contact plane
		motorDir = axis[0] - axis[0] * trace.c.normal * axis[0];
		motorDir.Normalize();
		
		r = trace.c.point - body1->GetWorldOrigin();
		
		J1.ChangeSize( 3, J1.GetNumColumns() );
		J1.SubVec6( 2 ).SubVec3( 0 ) = -motorDir;
		J1.SubVec6( 2 ).SubVec3( 1 ) = r.Cross( -motorDir );
		c1.ChangeSize( 3 );
		c1[2] = motorVelocity;
		
		if( body2 )
		{
			r = trace.c.point - body2->GetWorldOrigin();
			
			J2.ChangeSize( 3, J2.GetNumColumns() );
			J2.SubVec6( 2 ).SubVec3( 0 ) = -motorDir;
			J2.SubVec6( 2 ).SubVec3( 1 ) = r.Cross( -motorDir );
			c2.ChangeSize( 3 );
			c2[2] = 0.0f;
		}
		
		lo[2] = -motorForce;
		hi[2] = motorForce;
		boxIndex[2] = -1;
	}
}

/*
================
budAFConstraint_Suspension::ApplyFriction
================
*/
void budAFConstraint_Suspension::ApplyFriction( float invTimeStep )
{
	// do nothing
}

/*
================
budAFConstraint_Suspension::Translate
================
*/
void budAFConstraint_Suspension::Translate( const budVec3& translation )
{
}

/*
================
budAFConstraint_Suspension::Rotate
================
*/
void budAFConstraint_Suspension::Rotate( const budRotation& rotation )
{
}

/*
================
budAFConstraint_Suspension::DebugDraw
================
*/
void budAFConstraint_Suspension::DebugDraw()
{
	budVec3 origin;
	budMat3 axis;
	budRotation rotation;
	
	axis = localAxis * body1->GetWorldAxis();
	
	rotation.SetVec( axis[2] );
	rotation.SetAngle( steerAngle );
	
	axis *= rotation.ToMat3();
	
	if( trace.fraction < 1.0f )
	{
		origin = trace.c.point;
		
		gameRenderWorld->DebugLine( colorWhite, origin, origin + 6.0f * axis[2] );
		gameRenderWorld->DebugLine( colorWhite, origin - 4.0f * axis[0], origin + 4.0f * axis[0] );
		gameRenderWorld->DebugLine( colorWhite, origin - 2.0f * axis[1], origin + 2.0f * axis[1] );
	}
}


//===============================================================
//
//	budAFBody
//
//===============================================================

/*
================
budAFBody::budAFBody
================
*/
budAFBody::budAFBody()
{
	Init();
}

/*
================
budAFBody::budAFBody
================
*/
budAFBody::budAFBody( const budStr& name, budClipModel* clipModel, float density )
{

	assert( clipModel );
	assert( clipModel->IsTraceModel() );
	
	Init();
	
	this->name = name;
	this->clipModel = NULL;
	
	SetClipModel( clipModel );
	SetDensity( density );
	
	current->worldOrigin = clipModel->GetOrigin();
	current->worldAxis = clipModel->GetAxis();
	*next = *current;
	
}

/*
================
budAFBody::~budAFBody
================
*/
budAFBody::~budAFBody()
{
	delete clipModel;
}

/*
================
budAFBody::Init
================
*/
void budAFBody::Init()
{
	name						= "noname";
	parent						= NULL;
	clipModel					= NULL;
	primaryConstraint			= NULL;
	tree						= NULL;
	
	linearFriction				= -1.0f;
	angularFriction				= -1.0f;
	contactFriction				= -1.0f;
	bouncyness					= -1.0f;
	clipMask					= 0;
	
	frictionDir					= vec3_zero;
	contactMotorDir				= vec3_zero;
	contactMotorVelocity		= 0.0f;
	contactMotorForce			= 0.0f;
	
	mass						= 1.0f;
	invMass						= 1.0f;
	centerOfMass				= vec3_zero;
	inertiaTensor				= mat3_identity;
	inverseInertiaTensor		= mat3_identity;
	
	current						= &state[0];
	next						= &state[1];
	current->worldOrigin		= vec3_zero;
	current->worldAxis			= mat3_identity;
	current->spatialVelocity	= vec6_zero;
	current->externalForce		= vec6_zero;
	*next						= *current;
	saved						= *current;
	atRestOrigin				= vec3_zero;
	atRestAxis					= mat3_identity;
	
	s.Zero( 6 );
	totalForce.Zero( 6 );
	auxForce.Zero( 6 );
	acceleration.Zero( 6 );
	
	response					= NULL;
	responseIndex				= NULL;
	numResponses				= 0;
	maxAuxiliaryIndex			= 0;
	maxSubTreeAuxiliaryIndex	= 0;
	
	memset( &fl, 0, sizeof( fl ) );
	
	fl.selfCollision			= true;
	fl.isZero					= true;
}

/*
================
budAFBody::SetClipModel
================
*/
void budAFBody::SetClipModel( budClipModel* clipModel )
{
	if( this->clipModel && this->clipModel != clipModel )
	{
		delete this->clipModel;
	}
	this->clipModel = clipModel;
}

/*
================
budAFBody::SetFriction
================
*/
void budAFBody::SetFriction( float linear, float angular, float contact )
{
	if( linear < 0.0f || linear > 1.0f ||
			angular < 0.0f || angular > 1.0f ||
			contact < 0.0f )
	{
		gameLocal.Warning( "budAFBody::SetFriction: friction out of range, linear = %.1f, angular = %.1f, contact = %.1f", linear, angular, contact );
		return;
	}
	linearFriction = linear;
	angularFriction = angular;
	contactFriction = contact;
}

/*
================
budAFBody::SetBouncyness
================
*/
void budAFBody::SetBouncyness( float bounce )
{
	if( bounce < 0.0f || bounce > 1.0f )
	{
		gameLocal.Warning( "budAFBody::SetBouncyness: bouncyness out of range, bounce = %.1f", bounce );
		return;
	}
	bouncyness = bounce;
}

/*
================
budAFBody::SetDensity
================
*/
void budAFBody::SetDensity( float density, const budMat3& inertiaScale )
{

	// get the body mass properties
	clipModel->GetMassProperties( density, mass, centerOfMass, inertiaTensor );
	
	// make sure we have a valid mass
	if( mass <= 0.0f || IEEE_FLT_IS_NAN( mass ) )
	{
		gameLocal.Warning( "budAFBody::SetDensity: invalid mass for body '%s'", name.c_str() );
		mass = 1.0f;
		centerOfMass.Zero();
		inertiaTensor.Identity();
	}
	
	// make sure the center of mass is at the body origin
	if( !centerOfMass.Compare( vec3_origin, CENTER_OF_MASS_EPSILON ) )
	{
		gameLocal.Warning( "budAFBody::SetDentity: center of mass not at origin for body '%s'", name.c_str() );
	}
	centerOfMass.Zero();
	
	// calculate the inverse mass and inverse inertia tensor
	invMass = 1.0f / mass;
	if( inertiaScale != mat3_identity )
	{
		inertiaTensor *= inertiaScale;
	}
	if( inertiaTensor.IsDiagonal( 1e-3f ) )
	{
		inertiaTensor[0][1] = inertiaTensor[0][2] = 0.0f;
		inertiaTensor[1][0] = inertiaTensor[1][2] = 0.0f;
		inertiaTensor[2][0] = inertiaTensor[2][1] = 0.0f;
		inverseInertiaTensor.Identity();
		inverseInertiaTensor[0][0] = 1.0f / inertiaTensor[0][0];
		inverseInertiaTensor[1][1] = 1.0f / inertiaTensor[1][1];
		inverseInertiaTensor[2][2] = 1.0f / inertiaTensor[2][2];
	}
	else
	{
		inverseInertiaTensor = inertiaTensor.Inverse();
	}
}

/*
================
budAFBody::SetFrictionDirection
================
*/
void budAFBody::SetFrictionDirection( const budVec3& dir )
{
	frictionDir = dir * current->worldAxis.Transpose();
	fl.useFrictionDir = true;
}

/*
================
budAFBody::GetFrictionDirection
================
*/
bool budAFBody::GetFrictionDirection( budVec3& dir ) const
{
	if( fl.useFrictionDir )
	{
		dir = frictionDir * current->worldAxis;
		return true;
	}
	return false;
}

/*
================
budAFBody::SetContactMotorDirection
================
*/
void budAFBody::SetContactMotorDirection( const budVec3& dir )
{
	contactMotorDir = dir * current->worldAxis.Transpose();
	fl.useContactMotorDir = true;
}

/*
================
budAFBody::GetContactMotorDirection
================
*/
bool budAFBody::GetContactMotorDirection( budVec3& dir ) const
{
	if( fl.useContactMotorDir )
	{
		dir = contactMotorDir * current->worldAxis;
		return true;
	}
	return false;
}

/*
================
budAFBody::GetPointVelocity
================
*/
budVec3 budAFBody::GetPointVelocity( const budVec3& point ) const
{
	budVec3 r = point - current->worldOrigin;
	return current->spatialVelocity.SubVec3( 0 ) + current->spatialVelocity.SubVec3( 1 ).Cross( r );
}

/*
================
budAFBody::AddForce
================
*/
void budAFBody::AddForce( const budVec3& point, const budVec3& force )
{
	current->externalForce.SubVec3( 0 ) += force;
	current->externalForce.SubVec3( 1 ) += ( point - current->worldOrigin ).Cross( force );
}

/*
================
budAFBody::InverseWorldSpatialInertiaMultiply

  dst = this->inverseWorldSpatialInertia * v;
================
*/
BUD_INLINE void budAFBody::InverseWorldSpatialInertiaMultiply( budVecX& dst, const float* v ) const
{
	const float* mPtr = inverseWorldSpatialInertia.ToFloatPtr();
	const float* vPtr = v;
	float* dstPtr = dst.ToFloatPtr();
	
	if( fl.spatialInertiaSparse )
	{
		dstPtr[0] = mPtr[0 * 6 + 0] * vPtr[0];
		dstPtr[1] = mPtr[1 * 6 + 1] * vPtr[1];
		dstPtr[2] = mPtr[2 * 6 + 2] * vPtr[2];
		dstPtr[3] = mPtr[3 * 6 + 3] * vPtr[3] + mPtr[3 * 6 + 4] * vPtr[4] + mPtr[3 * 6 + 5] * vPtr[5];
		dstPtr[4] = mPtr[4 * 6 + 3] * vPtr[3] + mPtr[4 * 6 + 4] * vPtr[4] + mPtr[4 * 6 + 5] * vPtr[5];
		dstPtr[5] = mPtr[5 * 6 + 3] * vPtr[3] + mPtr[5 * 6 + 4] * vPtr[4] + mPtr[5 * 6 + 5] * vPtr[5];
	}
	else
	{
		gameLocal.Warning( "spatial inertia is not sparse for body %s", name.c_str() );
	}
}

/*
================
budAFBody::Save
================
*/
void budAFBody::Save( idSaveGame* saveFile )
{
	saveFile->WriteFloat( linearFriction );
	saveFile->WriteFloat( angularFriction );
	saveFile->WriteFloat( contactFriction );
	saveFile->WriteFloat( bouncyness );
	saveFile->WriteInt( clipMask );
	saveFile->WriteVec3( frictionDir );
	saveFile->WriteVec3( contactMotorDir );
	saveFile->WriteFloat( contactMotorVelocity );
	saveFile->WriteFloat( contactMotorForce );
	
	saveFile->WriteFloat( mass );
	saveFile->WriteFloat( invMass );
	saveFile->WriteVec3( centerOfMass );
	saveFile->WriteMat3( inertiaTensor );
	saveFile->WriteMat3( inverseInertiaTensor );
	
	saveFile->WriteVec3( current->worldOrigin );
	saveFile->WriteMat3( current->worldAxis );
	saveFile->WriteVec6( current->spatialVelocity );
	saveFile->WriteVec6( current->externalForce );
	saveFile->WriteVec3( atRestOrigin );
	saveFile->WriteMat3( atRestAxis );
}

/*
================
budAFBody::Restore
================
*/
void budAFBody::Restore( idRestoreGame* saveFile )
{
	saveFile->ReadFloat( linearFriction );
	saveFile->ReadFloat( angularFriction );
	saveFile->ReadFloat( contactFriction );
	saveFile->ReadFloat( bouncyness );
	saveFile->ReadInt( clipMask );
	saveFile->ReadVec3( frictionDir );
	saveFile->ReadVec3( contactMotorDir );
	saveFile->ReadFloat( contactMotorVelocity );
	saveFile->ReadFloat( contactMotorForce );
	
	saveFile->ReadFloat( mass );
	saveFile->ReadFloat( invMass );
	saveFile->ReadVec3( centerOfMass );
	saveFile->ReadMat3( inertiaTensor );
	saveFile->ReadMat3( inverseInertiaTensor );
	
	saveFile->ReadVec3( current->worldOrigin );
	saveFile->ReadMat3( current->worldAxis );
	saveFile->ReadVec6( current->spatialVelocity );
	saveFile->ReadVec6( current->externalForce );
	saveFile->ReadVec3( atRestOrigin );
	saveFile->ReadMat3( atRestAxis );
}



//===============================================================
//                                                        M
//  budAFTree                                             MrE
//                                                        E
//===============================================================

/*
================
budAFTree::Factor

  factor matrix for the primary constraints in the tree
================
*/
void budAFTree::Factor() const
{
	int i, j;
	budAFBody* body;
	budAFConstraint* child = NULL;
	budMatX childI;
	
	childI.SetData( 6, 6, MATX_ALLOCA( 6 * 6 ) );
	
	// from the leaves up towards the root
	for( i = sortedBodies.Num() - 1; i >= 0; i-- )
	{
		body = sortedBodies[i];
		
		if( body->children.Num() )
		{
		
			for( j = 0; j < body->children.Num(); j++ )
			{
			
				child = body->children[j]->primaryConstraint;
				
				// child->I = - child->body1->J.Transpose() * child->body1->I * child->body1->J;
				childI.SetSize( child->J1.GetNumRows(), child->J1.GetNumRows() );
				child->body1->J.TransposeMultiply( child->body1->I ).Multiply( childI, child->body1->J );
				childI.Negate();
				
				child->invI = childI;
				if( !child->invI.InverseFastSelf() )
				{
					gameLocal.Warning( "budAFTree::Factor: couldn't invert %dx%d matrix for constraint '%s'",
									   child->invI.GetNumRows(), child->invI.GetNumColumns(), child->GetName().c_str() );
				}
				child->J = child->invI * child->J;
				
				body->I -= child->J.TransposeMultiply( childI ) * child->J;
			}
			
			body->invI = body->I;
			if( !body->invI.InverseFastSelf() && child != NULL )
			{
				gameLocal.Warning( "budAFTree::Factor: couldn't invert %dx%d matrix for body %s",
								   child->invI.GetNumRows(), child->invI.GetNumColumns(), body->GetName().c_str() );
			}
			if( body->primaryConstraint )
			{
				body->J = body->invI * body->J;
			}
		}
		else if( body->primaryConstraint )
		{
			body->J = body->inverseWorldSpatialInertia * body->J;
		}
	}
}

/*
================
budAFTree::Solve

  solve for primary constraints in the tree
================
*/
void budAFTree::Solve( int auxiliaryIndex ) const
{
	int i, j;
	budAFBody* body, *child;
	budAFConstraint* primaryConstraint;
	
	// from the leaves up towards the root
	for( i = sortedBodies.Num() - 1; i >= 0; i-- )
	{
		body = sortedBodies[i];
		
		for( j = 0; j < body->children.Num(); j++ )
		{
			child = body->children[j];
			primaryConstraint = child->primaryConstraint;
			
			if( !child->fl.isZero )
			{
				child->J.TransposeMultiplySub( primaryConstraint->s, child->s );
				primaryConstraint->fl.isZero = false;
			}
			if( !primaryConstraint->fl.isZero )
			{
				primaryConstraint->J.TransposeMultiplySub( body->s, primaryConstraint->s );
				body->fl.isZero = false;
			}
		}
	}
	
	bool useSymmetry = af_useSymmetry.GetBool();
	
	// from the root down towards the leaves
	for( i = 0; i < sortedBodies.Num(); i++ )
	{
		body = sortedBodies[i];
		primaryConstraint = body->primaryConstraint;
		
		if( primaryConstraint )
		{
		
			if( useSymmetry && body->parent->maxSubTreeAuxiliaryIndex < auxiliaryIndex )
			{
				continue;
			}
			
			if( !primaryConstraint->fl.isZero )
			{
				primaryConstraint->s = primaryConstraint->invI * primaryConstraint->s;
			}
			primaryConstraint->J.MultiplySub( primaryConstraint->s, primaryConstraint->body2->s );
			
			primaryConstraint->lm = primaryConstraint->s;
			
			if( useSymmetry && body->maxSubTreeAuxiliaryIndex < auxiliaryIndex )
			{
				continue;
			}
			
			if( body->children.Num() )
			{
				if( !body->fl.isZero )
				{
					body->s = body->invI * body->s;
				}
				body->J.MultiplySub( body->s, primaryConstraint->s );
			}
		}
		else if( body->children.Num() )
		{
			body->s = body->invI * body->s;
		}
	}
}

/*
================
budAFTree::Response

  calculate body forces in the tree in response to a constraint force
================
*/
void budAFTree::Response( const budAFConstraint* constraint, int row, int auxiliaryIndex ) const
{
	int i, j;
	budAFBody* body;
	budAFConstraint* child, *primaryConstraint;
	budVecX v;
	
	// if a single body don't waste time because there aren't any primary constraints
	if( sortedBodies.Num() == 1 )
	{
		body = constraint->body1;
		if( body->tree == this )
		{
			body->GetResponseForce( body->numResponses ) = constraint->J1.SubVec6( row );
			body->responseIndex[body->numResponses++] = auxiliaryIndex;
		}
		else
		{
			body = constraint->body2;
			body->GetResponseForce( body->numResponses ) = constraint->J2.SubVec6( row );
			body->responseIndex[body->numResponses++] = auxiliaryIndex;
		}
		return;
	}
	
	v.SetData( 6, VECX_ALLOCA( 6 ) );
	
	// initialize right hand side to zero
	for( i = 0; i < sortedBodies.Num(); i++ )
	{
		body = sortedBodies[i];
		primaryConstraint = body->primaryConstraint;
		if( primaryConstraint )
		{
			primaryConstraint->s.Zero();
			primaryConstraint->fl.isZero = true;
		}
		body->s.Zero();
		body->fl.isZero = true;
		body->GetResponseForce( body->numResponses ).Zero();
	}
	
	// set right hand side for first constrained body
	body = constraint->body1;
	if( body->tree == this )
	{
		body->InverseWorldSpatialInertiaMultiply( v, constraint->J1[row] );
		primaryConstraint = body->primaryConstraint;
		if( primaryConstraint )
		{
			primaryConstraint->J1.Multiply( primaryConstraint->s, v );
			primaryConstraint->fl.isZero = false;
		}
		for( i = 0; i < body->children.Num(); i++ )
		{
			child = body->children[i]->primaryConstraint;
			child->J2.Multiply( child->s, v );
			child->fl.isZero = false;
		}
		body->GetResponseForce( body->numResponses ) = constraint->J1.SubVec6( row );
	}
	
	// set right hand side for second constrained body
	body = constraint->body2;
	if( body && body->tree == this )
	{
		body->InverseWorldSpatialInertiaMultiply( v, constraint->J2[row] );
		primaryConstraint = body->primaryConstraint;
		if( primaryConstraint )
		{
			primaryConstraint->J1.MultiplyAdd( primaryConstraint->s, v );
			primaryConstraint->fl.isZero = false;
		}
		for( i = 0; i < body->children.Num(); i++ )
		{
			child = body->children[i]->primaryConstraint;
			child->J2.MultiplyAdd( child->s, v );
			child->fl.isZero = false;
		}
		body->GetResponseForce( body->numResponses ) = constraint->J2.SubVec6( row );
	}
	
	
	// solve for primary constraints
	Solve( auxiliaryIndex );
	
	bool useSymmetry = af_useSymmetry.GetBool();
	
	// store body forces in response to the constraint force
	budVecX force;
	for( i = 0; i < sortedBodies.Num(); i++ )
	{
		body = sortedBodies[i];
		
		if( useSymmetry && body->maxAuxiliaryIndex < auxiliaryIndex )
		{
			continue;
		}
		
		force.SetData( 6, body->response + body->numResponses * 8 );
		
		// add forces of all primary constraints acting on this body
		primaryConstraint = body->primaryConstraint;
		if( primaryConstraint )
		{
			primaryConstraint->J1.TransposeMultiplyAdd( force, primaryConstraint->lm );
		}
		for( j = 0; j < body->children.Num(); j++ )
		{
			child = body->children[j]->primaryConstraint;
			child->J2.TransposeMultiplyAdd( force, child->lm );
		}
		
		body->responseIndex[body->numResponses++] = auxiliaryIndex;
	}
}

/*
================
budAFTree::CalculateForces

  calculate forces on the bodies in the tree
================
*/
void budAFTree::CalculateForces( float timeStep ) const
{
	int i, j;
	float invStep;
	budAFBody* body;
	budAFConstraint* child, *c, *primaryConstraint;
	
	// forces on bodies
	for( i = 0; i < sortedBodies.Num(); i++ )
	{
		body = sortedBodies[i];
		
		body->totalForce.SubVec6( 0 ) = body->current->externalForce + body->auxForce.SubVec6( 0 );
	}
	
	// if a single body don't waste time because there aren't any primary constraints
	if( sortedBodies.Num() == 1 )
	{
		return;
	}
	
	invStep = 1.0f / timeStep;
	
	// initialize right hand side
	for( i = 0; i < sortedBodies.Num(); i++ )
	{
		body = sortedBodies[i];
		
		body->InverseWorldSpatialInertiaMultiply( body->acceleration, body->totalForce.ToFloatPtr() );
		body->acceleration.SubVec6( 0 ) += body->current->spatialVelocity * invStep;
		primaryConstraint = body->primaryConstraint;
		if( primaryConstraint )
		{
			// b = ( J * acc + c )
			c = primaryConstraint;
			c->s = c->J1 * c->body1->acceleration + c->J2 * c->body2->acceleration + invStep * ( c->c1 + c->c2 );
			c->fl.isZero = false;
		}
		body->s.Zero();
		body->fl.isZero = true;
	}
	
	// solve for primary constraints
	Solve();
	
	// calculate forces on bodies after applying primary constraints
	for( i = 0; i < sortedBodies.Num(); i++ )
	{
		body = sortedBodies[i];
		
		// add forces of all primary constraints acting on this body
		primaryConstraint = body->primaryConstraint;
		if( primaryConstraint )
		{
			primaryConstraint->J1.TransposeMultiplyAdd( body->totalForce, primaryConstraint->lm );
		}
		for( j = 0; j < body->children.Num(); j++ )
		{
			child = body->children[j]->primaryConstraint;
			child->J2.TransposeMultiplyAdd( body->totalForce, child->lm );
		}
	}
}

/*
================
budAFTree::SetMaxSubTreeAuxiliaryIndex
================
*/
void budAFTree::SetMaxSubTreeAuxiliaryIndex()
{
	int i, j;
	budAFBody* body, *child;
	
	// from the leaves up towards the root
	for( i = sortedBodies.Num() - 1; i >= 0; i-- )
	{
		body = sortedBodies[i];
		
		body->maxSubTreeAuxiliaryIndex = body->maxAuxiliaryIndex;
		for( j = 0; j < body->children.Num(); j++ )
		{
			child = body->children[j];
			if( child->maxSubTreeAuxiliaryIndex > body->maxSubTreeAuxiliaryIndex )
			{
				body->maxSubTreeAuxiliaryIndex = child->maxSubTreeAuxiliaryIndex;
			}
		}
	}
}

/*
================
budAFTree::SortBodies_r
================
*/
void budAFTree::SortBodies_r( budList<budAFBody*>& sortedList, budAFBody* body )
{
	int i;
	
	for( i = 0; i < body->children.Num(); i++ )
	{
		sortedList.Append( body->children[i] );
	}
	for( i = 0; i < body->children.Num(); i++ )
	{
		SortBodies_r( sortedList, body->children[i] );
	}
}

/*
================
budAFTree::SortBodies

  sort body list to make sure parents come first
================
*/
void budAFTree::SortBodies()
{
	int i;
	budAFBody* body;
	
	// find the root
	for( i = 0; i < sortedBodies.Num(); i++ )
	{
		if( !sortedBodies[i]->parent )
		{
			break;
		}
	}
	
	if( i >= sortedBodies.Num() )
	{
		gameLocal.Error( "Articulated figure tree has no root." );
	}
	
	body = sortedBodies[i];
	sortedBodies.Clear();
	sortedBodies.Append( body );
	SortBodies_r( sortedBodies, body );
}

/*
================
budAFTree::DebugDraw
================
*/
void budAFTree::DebugDraw( const budVec4& color ) const
{
	int i;
	budAFBody* body;
	
	for( i = 1; i < sortedBodies.Num(); i++ )
	{
		body = sortedBodies[i];
		gameRenderWorld->DebugArrow( color, body->parent->current->worldOrigin, body->current->worldOrigin, 1 );
	}
}


//===============================================================
//                                                        M
//  idPhysics_AF                                         MrE
//                                                        E
//===============================================================

/*
================
idPhysics_AF::EvaluateConstraints
================
*/
void idPhysics_AF::EvaluateConstraints( float timeStep )
{
	int i;
	float invTimeStep;
	budAFBody* body;
	budAFConstraint* c;
	
	invTimeStep = 1.0f / timeStep;
	
	// setup the constraint equations for the current position and orientation of the bodies
	for( i = 0; i < primaryConstraints.Num(); i++ )
	{
		c = primaryConstraints[i];
		c->Evaluate( invTimeStep );
		c->J = c->J2;
	}
	for( i = 0; i < auxiliaryConstraints.Num(); i++ )
	{
		auxiliaryConstraints[i]->Evaluate( invTimeStep );
	}
	
	// add contact constraints to the list with frame constraints
	for( i = 0; i < contactConstraints.Num(); i++ )
	{
		AddFrameConstraint( contactConstraints[i] );
	}
	
	// setup body primary constraint matrix
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		if( body->primaryConstraint )
		{
			body->J = body->primaryConstraint->J1.Transpose();
		}
	}
}

/*
================
idPhysics_AF::EvaluateBodies
================
*/
void idPhysics_AF::EvaluateBodies( float timeStep )
{
	int i;
	budAFBody* body;
	budMat3 axis;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		// we transpose the axis before using it because budMat3 is column-major
		axis = body->current->worldAxis.Transpose();
		
		// if the center of mass is at the body point of reference
		if( body->centerOfMass.Compare( vec3_origin, CENTER_OF_MASS_EPSILON ) )
		{
		
			// spatial inertia in world space
			body->I.Set( body->mass * mat3_identity, mat3_zero,
						 mat3_zero, axis * body->inertiaTensor * axis.Transpose() );
						 
			// inverse spatial inertia in world space
			body->inverseWorldSpatialInertia.Set( body->invMass * mat3_identity, mat3_zero,
												  mat3_zero, axis * body->inverseInertiaTensor * axis.Transpose() );
												  
			body->fl.spatialInertiaSparse = true;
		}
		else
		{
			budMat3 massMoment = body->mass * SkewSymmetric( body->centerOfMass );
			
			// spatial inertia in world space
			body->I.Set( body->mass * mat3_identity, massMoment,
						 massMoment.Transpose(), axis * body->inertiaTensor * axis.Transpose() );
						 
			// inverse spatial inertia in world space
			body->inverseWorldSpatialInertia = body->I.InverseFast();
			
			body->fl.spatialInertiaSparse = false;
		}
		
		// initialize auxiliary constraint force to zero
		body->auxForce.Zero();
	}
}

/*
================
idPhysics_AF::AddFrameConstraints
================
*/
void idPhysics_AF::AddFrameConstraints()
{
	int i;
	
	// add frame constraints to auxiliary constraints
	for( i = 0; i < frameConstraints.Num(); i++ )
	{
		auxiliaryConstraints.Append( frameConstraints[i] );
	}
}

/*
================
idPhysics_AF::RemoveFrameConstraints
================
*/
void idPhysics_AF::RemoveFrameConstraints()
{
	// remove all the frame constraints from the auxiliary constraints
	auxiliaryConstraints.SetNum( auxiliaryConstraints.Num() - frameConstraints.Num() );
	frameConstraints.SetNum( 0 );
}

/*
================
idPhysics_AF::ApplyFriction
================
*/
void idPhysics_AF::ApplyFriction( float timeStep, float endTimeMSec )
{
	int i;
	float invTimeStep;
	
	if( af_skipFriction.GetBool() )
	{
		return;
	}
	
	if( jointFrictionDentStart < MS2SEC( endTimeMSec ) && jointFrictionDentEnd > MS2SEC( endTimeMSec ) )
	{
		float halfTime = ( jointFrictionDentEnd - jointFrictionDentStart ) * 0.5f;
		if( jointFrictionDentStart + halfTime > MS2SEC( endTimeMSec ) )
		{
			jointFrictionDentScale = 1.0f - ( 1.0f - jointFrictionDent ) * ( MS2SEC( endTimeMSec ) - jointFrictionDentStart ) / halfTime;
		}
		else
		{
			jointFrictionDentScale = jointFrictionDent + ( 1.0f - jointFrictionDent ) * ( MS2SEC( endTimeMSec ) - jointFrictionDentStart - halfTime ) / halfTime;
		}
	}
	else
	{
		jointFrictionDentScale = 0.0f;
	}
	
	if( contactFrictionDentStart < MS2SEC( endTimeMSec ) && contactFrictionDentEnd > MS2SEC( endTimeMSec ) )
	{
		float halfTime = ( contactFrictionDentEnd - contactFrictionDentStart ) * 0.5f;
		if( contactFrictionDentStart + halfTime > MS2SEC( endTimeMSec ) )
		{
			contactFrictionDentScale = 1.0f - ( 1.0f - contactFrictionDent ) * ( MS2SEC( endTimeMSec ) - contactFrictionDentStart ) / halfTime;
		}
		else
		{
			contactFrictionDentScale = contactFrictionDent + ( 1.0f - contactFrictionDent ) * ( MS2SEC( endTimeMSec ) - contactFrictionDentStart - halfTime ) / halfTime;
		}
	}
	else
	{
		contactFrictionDentScale = 0.0f;
	}
	
	invTimeStep = 1.0f / timeStep;
	
	for( i = 0; i < primaryConstraints.Num(); i++ )
	{
		primaryConstraints[i]->ApplyFriction( invTimeStep );
	}
	for( i = 0; i < auxiliaryConstraints.Num(); i++ )
	{
		auxiliaryConstraints[i]->ApplyFriction( invTimeStep );
	}
	for( i = 0; i < frameConstraints.Num(); i++ )
	{
		frameConstraints[i]->ApplyFriction( invTimeStep );
	}
}

/*
================
idPhysics_AF::PrimaryFactor
================
*/
void idPhysics_AF::PrimaryFactor()
{
	int i;
	
	for( i = 0; i < trees.Num(); i++ )
	{
		trees[i]->Factor();
	}
}

/*
================
idPhysics_AF::PrimaryForces
================
*/
void idPhysics_AF::PrimaryForces( float timeStep )
{
	int i;
	
	for( i = 0; i < trees.Num(); i++ )
	{
		trees[i]->CalculateForces( timeStep );
	}
}

/*
================
idPhysics_AF::AuxiliaryForces
================
*/
void idPhysics_AF::AuxiliaryForces( float timeStep )
{
	int i, j, k, l, n, m, s, numAuxConstraints, *index, *boxIndex;
	float* ptr, *j1, *j2, *dstPtr, *forcePtr;
	float invStep, u;
	budAFBody* body;
	budAFConstraint* constraint;
	budVecX tmp;
	budMatX jmk;
	budVecX rhs, w, lm, lo, hi;
	
	// get the number of one dimensional auxiliary constraints
	for( numAuxConstraints = 0, i = 0; i < auxiliaryConstraints.Num(); i++ )
	{
		numAuxConstraints += auxiliaryConstraints[i]->J1.GetNumRows();
	}
	
	if( numAuxConstraints == 0 )
	{
		return;
	}
	
	// allocate memory to store the body response to auxiliary constraint forces
	forcePtr = ( float* ) _alloca16( bodies.Num() * numAuxConstraints * 8 * sizeof( float ) );
	index = ( int* ) _alloca16( bodies.Num() * numAuxConstraints * sizeof( int ) );
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		body->response = forcePtr;
		body->responseIndex = index;
		body->numResponses = 0;
		body->maxAuxiliaryIndex = 0;
		forcePtr += numAuxConstraints * 8;
		index += numAuxConstraints;
	}
	
	// set on each body the largest index of an auxiliary constraint constraining the body
	if( af_useSymmetry.GetBool() )
	{
		for( k = 0, i = 0; i < auxiliaryConstraints.Num(); i++ )
		{
			constraint = auxiliaryConstraints[i];
			for( j = 0; j < constraint->J1.GetNumRows(); j++, k++ )
			{
				if( k > constraint->body1->maxAuxiliaryIndex )
				{
					constraint->body1->maxAuxiliaryIndex = k;
				}
				if( constraint->body2 && k > constraint->body2->maxAuxiliaryIndex )
				{
					constraint->body2->maxAuxiliaryIndex = k;
				}
			}
		}
		for( i = 0; i < trees.Num(); i++ )
		{
			trees[i]->SetMaxSubTreeAuxiliaryIndex();
		}
	}
	
	// calculate forces of primary constraints in response to the auxiliary constraint forces
	for( k = 0, i = 0; i < auxiliaryConstraints.Num(); i++ )
	{
		constraint = auxiliaryConstraints[i];
		
		for( j = 0; j < constraint->J1.GetNumRows(); j++, k++ )
		{
		
			// calculate body forces in the tree in response to the constraint force
			constraint->body1->tree->Response( constraint, j, k );
			// if there is a second body which is part of a different tree
			if( constraint->body2 && constraint->body2->tree != constraint->body1->tree )
			{
				// calculate body forces in the second tree in response to the constraint force
				constraint->body2->tree->Response( constraint, j, k );
			}
		}
	}
	
	// NOTE: the rows are 16 byte padded
	jmk.SetData( numAuxConstraints, ( ( numAuxConstraints + 3 ) & ~3 ), MATX_ALLOCA( numAuxConstraints * ( ( numAuxConstraints + 3 ) & ~3 ) ) );
	tmp.SetData( 6, VECX_ALLOCA( 6 ) );
	
	// create constraint matrix for auxiliary constraints using a mass matrix adjusted for the primary constraints
	for( k = 0, i = 0; i < auxiliaryConstraints.Num(); i++ )
	{
		constraint = auxiliaryConstraints[i];
		
		for( j = 0; j < constraint->J1.GetNumRows(); j++, k++ )
		{
			constraint->body1->InverseWorldSpatialInertiaMultiply( tmp, constraint->J1[j] );
			j1 = tmp.ToFloatPtr();
			ptr = constraint->body1->response;
			index = constraint->body1->responseIndex;
			dstPtr = jmk[k];
			s = af_useSymmetry.GetBool() ? k + 1 : numAuxConstraints;
			for( l = n = 0, m = index[n]; n < constraint->body1->numResponses && m < s; n++, m = index[n] )
			{
				while( l < m )
				{
					dstPtr[l++] = 0.0f;
				}
				dstPtr[l++] = j1[0] * ptr[0] + j1[1] * ptr[1] + j1[2] * ptr[2] +
							  j1[3] * ptr[3] + j1[4] * ptr[4] + j1[5] * ptr[5];
				ptr += 8;
			}
			
			while( l < s )
			{
				dstPtr[l++] = 0.0f;
			}
			
			if( constraint->body2 )
			{
				constraint->body2->InverseWorldSpatialInertiaMultiply( tmp, constraint->J2[j] );
				j2 = tmp.ToFloatPtr();
				ptr = constraint->body2->response;
				index = constraint->body2->responseIndex;
				for( n = 0, m = index[n]; n < constraint->body2->numResponses && m < s; n++, m = index[n] )
				{
					dstPtr[m] += j2[0] * ptr[0] + j2[1] * ptr[1] + j2[2] * ptr[2] +
								 j2[3] * ptr[3] + j2[4] * ptr[4] + j2[5] * ptr[5];
					ptr += 8;
				}
			}
		}
	}
	
	if( af_useSymmetry.GetBool() )
	{
		n = jmk.GetNumColumns();
		for( i = 0; i < numAuxConstraints; i++ )
		{
			ptr = jmk.ToFloatPtr() + ( i + 1 ) * n + i;
			dstPtr = jmk.ToFloatPtr() + i * n + i + 1;
			for( j = i + 1; j < numAuxConstraints; j++ )
			{
				*dstPtr++ = *ptr;
				ptr += n;
			}
		}
	}
	
	invStep = 1.0f / timeStep;
	
	// calculate body acceleration
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		body->InverseWorldSpatialInertiaMultiply( body->acceleration, body->totalForce.ToFloatPtr() );
		body->acceleration.SubVec6( 0 ) += body->current->spatialVelocity * invStep;
	}
	
	rhs.SetData( numAuxConstraints, VECX_ALLOCA( numAuxConstraints ) );
	lo.SetData( numAuxConstraints, VECX_ALLOCA( numAuxConstraints ) );
	hi.SetData( numAuxConstraints, VECX_ALLOCA( numAuxConstraints ) );
	lm.SetData( numAuxConstraints, VECX_ALLOCA( numAuxConstraints ) );
	boxIndex = ( int* ) _alloca16( numAuxConstraints * sizeof( int ) );
	
	// set first index for special box constrained variables
	for( k = 0, i = 0; i < auxiliaryConstraints.Num(); i++ )
	{
		auxiliaryConstraints[i]->firstIndex = k;
		k += auxiliaryConstraints[i]->J1.GetNumRows();
	}
	
	// initialize right hand side and low and high bounds for auxiliary constraints
	for( k = 0, i = 0; i < auxiliaryConstraints.Num(); i++ )
	{
		constraint = auxiliaryConstraints[i];
		n = k;
		
		for( j = 0; j < constraint->J1.GetNumRows(); j++, k++ )
		{
		
			j1 = constraint->J1[j];
			ptr = constraint->body1->acceleration.ToFloatPtr();
			rhs[k] = j1[0] * ptr[0] + j1[1] * ptr[1] + j1[2] * ptr[2] + j1[3] * ptr[3] + j1[4] * ptr[4] + j1[5] * ptr[5];
			rhs[k] += constraint->c1[j] * invStep;
			
			if( constraint->body2 )
			{
				j2 = constraint->J2[j];
				ptr = constraint->body2->acceleration.ToFloatPtr();
				rhs[k] += j2[0] * ptr[0] + j2[1] * ptr[1] + j2[2] * ptr[2] + j2[3] * ptr[3] + j2[4] * ptr[4] + j2[5] * ptr[5];
				rhs[k] += constraint->c2[j] * invStep;
			}
			
			rhs[k] = -rhs[k];
			lo[k] = constraint->lo[j];
			hi[k] = constraint->hi[j];
			
			if( constraint->boxIndex[j] >= 0 )
			{
				if( constraint->boxConstraint->fl.isPrimary )
				{
					gameLocal.Error( "cannot reference primary constraints for the box index" );
				}
				boxIndex[k] = constraint->boxConstraint->firstIndex + constraint->boxIndex[j];
			}
			else
			{
				boxIndex[k] = -1;
			}
			jmk[k][k] += constraint->e[j] * invStep;
		}
	}
	
#ifdef AF_TIMINGS
	timer_lcp.Start();
#endif
	
	// calculate lagrange multipliers for auxiliary constraints
	if( !lcp->Solve( jmk, lm, rhs, lo, hi, boxIndex ) )
	{
		return;		// bad monkey!
	}
	
#ifdef AF_TIMINGS
	timer_lcp.Stop();
#endif
	
	// calculate auxiliary constraint forces
	for( k = 0, i = 0; i < auxiliaryConstraints.Num(); i++ )
	{
		constraint = auxiliaryConstraints[i];
		
		for( j = 0; j < constraint->J1.GetNumRows(); j++, k++ )
		{
			constraint->lm[j] = u = lm[k];
			
			j1 = constraint->J1[j];
			ptr = constraint->body1->auxForce.ToFloatPtr();
			ptr[0] += j1[0] * u;
			ptr[1] += j1[1] * u;
			ptr[2] += j1[2] * u;
			ptr[3] += j1[3] * u;
			ptr[4] += j1[4] * u;
			ptr[5] += j1[5] * u;
			
			if( constraint->body2 )
			{
				j2 = constraint->J2[j];
				ptr = constraint->body2->auxForce.ToFloatPtr();
				ptr[0] += j2[0] * u;
				ptr[1] += j2[1] * u;
				ptr[2] += j2[2] * u;
				ptr[3] += j2[3] * u;
				ptr[4] += j2[4] * u;
				ptr[5] += j2[5] * u;
			}
		}
	}
	
	// recalculate primary constraint forces in response to auxiliary constraint forces
	PrimaryForces( timeStep );
	
	// clear pointers pointing to stack space so tools don't get confused
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		body->response = NULL;
		body->responseIndex = NULL;
	}
}

/*
================
idPhysics_AF::VerifyContactConstraints
================
*/
void idPhysics_AF::VerifyContactConstraints()
{
#if 0
	int i;
	float impulseNumerator, impulseDenominator;
	budVec3 r, velocity, normalVelocity, normal, impulse;
	budAFBody* body;
	
	for( i = 0; i < contactConstraints.Num(); i++ )
	{
		body = contactConstraints[i]->body1;
		const contactInfo_t& contact = contactConstraints[i]->GetContact();
		
		r = contact.point - body->GetCenterOfMass();
		
		// calculate velocity at contact point
		velocity = body->GetLinearVelocity() + body->GetAngularVelocity().Cross( r );
		
		// velocity along normal vector
		normalVelocity = ( velocity * contact.normal ) * contact.normal;
		
		// if moving towards the surface at the contact point
		if( normalVelocity * contact.normal < 0.0f )
		{
			// calculate impulse
			normal = -normalVelocity;
			impulseNumerator = normal.Normalize();
			impulseDenominator = body->GetInverseMass() + ( ( body->GetInverseWorldInertia() * r.Cross( normal ) ).Cross( r ) * normal );
			impulse = ( impulseNumerator / impulseDenominator ) * normal * 1.0001f;
			
			// apply impulse
			body->SetLinearVelocity( body->GetLinearVelocity() + impulse );
			body->SetAngularVelocity( body->GetAngularVelocity() + r.Cross( impulse ) );
		}
	}
#else
	int i;
	budAFBody* body;
	budVec3 normal;
	
	for( i = 0; i < contactConstraints.Num(); i++ )
	{
		body = contactConstraints[i]->body1;
		normal = contactConstraints[i]->GetContact().normal;
		if( normal * body->next->spatialVelocity.SubVec3( 0 ) <= 0.0f )
		{
			body->next->spatialVelocity.SubVec3( 0 ) -= 1.0001f * ( normal * body->next->spatialVelocity.SubVec3( 0 ) ) * normal;
		}
		body = contactConstraints[i]->body2;
		if( !body )
		{
			continue;
		}
		normal = -normal;
		if( normal * body->next->spatialVelocity.SubVec3( 0 ) <= 0.0f )
		{
			body->next->spatialVelocity.SubVec3( 0 ) -= 1.0001f * ( normal * body->next->spatialVelocity.SubVec3( 0 ) ) * normal;
		}
	}
#endif
}

/*
================
idPhysics_AF::Evolve
================
*/
void idPhysics_AF::Evolve( float timeStep )
{
	int i;
	float angle;
	budVec3 vec;
	budAFBody* body;
	budVec6 force;
	budRotation rotation;
	float vSqr, maxLinearVelocity, maxAngularVelocity;
	
	maxLinearVelocity = af_maxLinearVelocity.GetFloat() / timeStep;
	maxAngularVelocity = af_maxAngularVelocity.GetFloat() / timeStep;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		// calculate the spatial velocity for the next physics state
		body->InverseWorldSpatialInertiaMultiply( body->acceleration, body->totalForce.ToFloatPtr() );
		body->next->spatialVelocity = body->current->spatialVelocity + timeStep * body->acceleration.SubVec6( 0 );
		
		if( maxLinearVelocity > 0.0f )
		{
			// cap the linear velocity
			vSqr = body->next->spatialVelocity.SubVec3( 0 ).LengthSqr();
			if( vSqr > Square( maxLinearVelocity ) )
			{
				body->next->spatialVelocity.SubVec3( 0 ) *= budMath::InvSqrt( vSqr ) * maxLinearVelocity;
			}
		}
		
		if( maxAngularVelocity > 0.0f )
		{
			// cap the angular velocity
			vSqr = body->next->spatialVelocity.SubVec3( 1 ).LengthSqr();
			if( vSqr > Square( maxAngularVelocity ) )
			{
				body->next->spatialVelocity.SubVec3( 1 ) *= budMath::InvSqrt( vSqr ) * maxAngularVelocity;
			}
		}
	}
	
	// make absolutely sure all contact constraints are satisfied
	VerifyContactConstraints();
	
	// calculate the position of the bodies for the next physics state
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		// translate world origin
		body->next->worldOrigin = body->current->worldOrigin + timeStep * body->next->spatialVelocity.SubVec3( 0 );
		
		// convert angular velocity to a rotation matrix
		vec = body->next->spatialVelocity.SubVec3( 1 );
		angle = -timeStep * ( float ) RAD2DEG( vec.Normalize() );
		rotation = budRotation( vec3_origin, vec, angle );
		rotation.Normalize180();
		
		// rotate world axis
		body->next->worldAxis = body->current->worldAxis * rotation.ToMat3();
		body->next->worldAxis.OrthoNormalizeSelf();
		
		// linear and angular friction
		body->next->spatialVelocity.SubVec3( 0 ) -= body->linearFriction * body->next->spatialVelocity.SubVec3( 0 );
		body->next->spatialVelocity.SubVec3( 1 ) -= body->angularFriction * body->next->spatialVelocity.SubVec3( 1 );
	}
}

/*
================
idPhysics_AF::CollisionImpulse

  apply impulse to the colliding bodies
  the current state of the body should be set to the moment of impact
  this is silly as it doesn't take the AF structure into account
================
*/
bool idPhysics_AF::CollisionImpulse( float timeStep, budAFBody* body, trace_t& collision )
{
	budVec3 r, velocity, impulse;
	budMat3 inverseWorldInertiaTensor;
	float impulseNumerator, impulseDenominator;
	impactInfo_t info;
	idEntity* ent;
	
	ent = gameLocal.entities[collision.c.entityNum];
	if( ent == self )
	{
		return false;
	}
	
	// get info from other entity involved
	ent->GetImpactInfo( self, collision.c.id, collision.c.point, &info );
	// collision point relative to the body center of mass
	r = collision.c.point - ( body->current->worldOrigin + body->centerOfMass * body->current->worldAxis );
	// the velocity at the collision point
	velocity = body->current->spatialVelocity.SubVec3( 0 ) + body->current->spatialVelocity.SubVec3( 1 ).Cross( r );
	// subtract velocity of other entity
	velocity -= info.velocity;
	// never stick
	if( velocity * collision.c.normal > 0.0f )
	{
		velocity = collision.c.normal;
	}
	inverseWorldInertiaTensor = body->current->worldAxis.Transpose() * body->inverseInertiaTensor * body->current->worldAxis;
	impulseNumerator = -( 1.0f + body->bouncyness ) * ( velocity * collision.c.normal );
	impulseDenominator = body->invMass + ( ( inverseWorldInertiaTensor * r.Cross( collision.c.normal ) ).Cross( r ) * collision.c.normal );
	if( info.invMass )
	{
		impulseDenominator += info.invMass + ( ( info.invInertiaTensor * info.position.Cross( collision.c.normal ) ).Cross( info.position ) * collision.c.normal );
	}
	impulse = ( impulseNumerator / impulseDenominator ) * collision.c.normal;
	
	// apply impact to other entity
	ent->ApplyImpulse( self, collision.c.id, collision.c.point, -impulse );
	
	// callback to self to let the entity know about the impact
	return self->Collide( collision, velocity );
}

/*
================
idPhysics_AF::ApplyCollisions
================
*/
bool idPhysics_AF::ApplyCollisions( float timeStep )
{
	int i;
	
	for( i = 0; i < collisions.Num(); i++ )
	{
		if( CollisionImpulse( timeStep, collisions[i].body, collisions[i].trace ) )
		{
			return true;
		}
	}
	return false;
}

/*
================
idPhysics_AF::SetupCollisionForBody
================
*/
idEntity* idPhysics_AF::SetupCollisionForBody( budAFBody* body ) const
{
	int i;
	budAFBody* b;
	idEntity* passEntity;
	
	passEntity = NULL;
	
	if( !selfCollision || !body->fl.selfCollision || af_skipSelfCollision.GetBool() )
	{
	
		// disable all bodies
		for( i = 0; i < bodies.Num(); i++ )
		{
			bodies[i]->clipModel->Disable();
		}
		
		// don't collide with world collision model if attached to the world
		for( i = 0; i < body->constraints.Num(); i++ )
		{
			if( !body->constraints[i]->fl.noCollision )
			{
				continue;
			}
			// if this constraint attaches the body to the world
			if( body->constraints[i]->body2 == NULL )
			{
				// don't collide with the world collision model
				passEntity = gameLocal.world;
			}
		}
		
	}
	else
	{
	
		// enable all bodies that have self collision
		for( i = 0; i < bodies.Num(); i++ )
		{
			if( bodies[i]->fl.selfCollision )
			{
				bodies[i]->clipModel->Enable();
			}
			else
			{
				bodies[i]->clipModel->Disable();
			}
		}
		
		// don't let the body collide with itself
		body->clipModel->Disable();
		
		// disable any bodies attached with constraints
		for( i = 0; i < body->constraints.Num(); i++ )
		{
			if( !body->constraints[i]->fl.noCollision )
			{
				continue;
			}
			// if this constraint attaches the body to the world
			if( body->constraints[i]->body2 == NULL )
			{
				// don't collide with the world collision model
				passEntity = gameLocal.world;
			}
			else
			{
				if( body->constraints[i]->body1 == body )
				{
					b = body->constraints[i]->body2;
				}
				else if( body->constraints[i]->body2 == body )
				{
					b = body->constraints[i]->body1;
				}
				else
				{
					continue;
				}
				// don't collide with this body
				b->clipModel->Disable();
			}
		}
	}
	
	return passEntity;
}

/*
================
idPhysics_AF::CheckForCollisions

  check for collisions between the current and next state
  if there is a collision the next state is set to the state at the moment of impact
  assumes all bodies are linked for collision detection and relinks all bodies after moving them
================
*/
void idPhysics_AF::CheckForCollisions( float timeStep )
{
//	#define TEST_COLLISION_DETECTION
	int i, index;
	budAFBody* body;
	budMat3 axis;
	budRotation rotation;
	trace_t collision;
	idEntity* passEntity;
	
	// clear list with collisions
	collisions.SetNum( 0 );
	
	if( !enableCollision )
	{
		return;
	}
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		if( body->clipMask != 0 )
		{
		
			passEntity = SetupCollisionForBody( body );
			
#ifdef TEST_COLLISION_DETECTION
			bool startsolid = false;
			if( gameLocal.clip.Contents( body->current->worldOrigin, body->clipModel,
										 body->current->worldAxis, body->clipMask, passEntity ) )
			{
				startsolid = true;
			}
#endif
			
			TransposeMultiply( body->current->worldAxis, body->next->worldAxis, axis );
			rotation = axis.ToRotation();
			rotation.SetOrigin( body->current->worldOrigin );
			
			// if there was a collision
			if( gameLocal.clip.Motion( collision, body->current->worldOrigin, body->next->worldOrigin, rotation,
									   body->clipModel, body->current->worldAxis, body->clipMask, passEntity ) )
			{
			
				// set the next state to the state at the moment of impact
				body->next->worldOrigin = collision.endpos;
				body->next->worldAxis = collision.endAxis;
				
				// add collision to the list
				index = collisions.Num();
				collisions.SetNum( index + 1 );
				collisions[index].trace = collision;
				collisions[index].body = body;
			}
			
#ifdef TEST_COLLISION_DETECTION
			if( gameLocal.clip.Contents( body->next->worldOrigin, body->clipModel,
										 body->next->worldAxis, body->clipMask, passEntity ) )
			{
				if( !startsolid )
				{
					int bah = 1;
				}
			}
#endif
		}
		
		body->clipModel->Link( gameLocal.clip, self, body->clipModel->GetId(), body->next->worldOrigin, body->next->worldAxis );
	}
}

/*
================
idPhysics_AF::EvaluateContacts
================
*/
bool idPhysics_AF::EvaluateContacts()
{
	int i, j, k, numContacts, numBodyContacts;
	budAFBody* body;
	contactInfo_t contactInfo[10];
	idEntity* passEntity;
	budVecX dir( 6, VECX_ALLOCA( 6 ) );
	
	// evaluate bodies
	EvaluateBodies( current.lastTimeStep );
	
	// remove all existing contacts
	ClearContacts();
	
	contactBodies.SetNum( 0 );
	
	if( !enableCollision )
	{
		return false;
	}
	
	// find all the contacts
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		if( body->clipMask == 0 )
		{
			continue;
		}
		
		passEntity = SetupCollisionForBody( body );
		
		body->InverseWorldSpatialInertiaMultiply( dir, body->current->externalForce.ToFloatPtr() );
		dir.SubVec6( 0 ) = body->current->spatialVelocity + current.lastTimeStep * dir.SubVec6( 0 );
		dir.SubVec3( 0 ).Normalize();
		dir.SubVec3( 1 ).Normalize();
		
		numContacts = gameLocal.clip.Contacts( contactInfo, 10, body->current->worldOrigin, dir.SubVec6( 0 ), 2.0f, //CONTACT_EPSILON,
											   body->clipModel, body->current->worldAxis, body->clipMask, passEntity );
											   
#if 1
		// merge nearby contacts between the same bodies
		// and assure there are at most three planar contacts between any pair of bodies
		for( j = 0; j < numContacts; j++ )
		{
		
			numBodyContacts = 0;
			for( k = 0; k < contacts.Num(); k++ )
			{
				if( contacts[k].entityNum == contactInfo[j].entityNum )
				{
					if( ( contacts[k].id == i && contactInfo[j].id == contactBodies[k] ) ||
							( contactBodies[k] == i && contacts[k].id == contactInfo[j].id ) )
					{
					
						if( ( contacts[k].point - contactInfo[j].point ).LengthSqr() < Square( 2.0f ) )
						{
							break;
						}
						if( budMath::Fabs( contacts[k].normal * contactInfo[j].normal ) > 0.9f )
						{
							numBodyContacts++;
						}
					}
				}
			}
			
			if( k >= contacts.Num() && numBodyContacts < 3 )
			{
				contacts.Append( contactInfo[j] );
				contactBodies.Append( i );
			}
		}
		
#else
		
		for( j = 0; j < numContacts; j++ )
		{
			contacts.Append( contactInfo[j] );
			contactBodies.Append( i );
		}
#endif
		
	}
	
	AddContactEntitiesForContacts();
	
	return ( contacts.Num() != 0 );
}

/*
================
idPhysics_AF::SetupContactConstraints
================
*/
void idPhysics_AF::SetupContactConstraints()
{
	int i;
	
	// make sure enough contact constraints are allocated
	contactConstraints.AssureSizeAlloc( contacts.Num(), budListNewElement<budAFConstraint_Contact> );
	contactConstraints.SetNum( contacts.Num() );
	
	// setup contact constraints
	for( i = 0; i < contacts.Num(); i++ )
	{
		// add contact constraint
		contactConstraints[i]->physics = this;
		if( contacts[i].entityNum == self->entityNumber )
		{
			contactConstraints[i]->Setup( bodies[contactBodies[i]], bodies[ contacts[i].id ], contacts[i] );
		}
		else
		{
			contactConstraints[i]->Setup( bodies[contactBodies[i]], NULL, contacts[i] );
		}
	}
}

/*
================
idPhysics_AF::ApplyContactForces
================
*/
void idPhysics_AF::ApplyContactForces()
{
#if 0
	int i;
	idEntity* ent;
	budVec3 force;
	
	for( i = 0; i < contactConstraints.Num(); i++ )
	{
		if( contactConstraints[i]->body2 != NULL )
		{
			continue;
		}
		const contactInfo_t& contact = contactConstraints[i]->GetContact();
		ent = gameLocal.entities[contact.entityNum];
		if( !ent )
		{
			continue;
		}
		force.Zero();
		ent->AddForce( self, contact.id, contact.point, force );
	}
#endif
}

/*
================
idPhysics_AF::ClearExternalForce
================
*/
void idPhysics_AF::ClearExternalForce()
{
	int i;
	budAFBody* body;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		// clear external force
		body->current->externalForce.Zero();
		body->next->externalForce.Zero();
	}
}

/*
================
idPhysics_AF::AddGravity
================
*/
void idPhysics_AF::AddGravity()
{
	int i;
	budAFBody* body;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		// add gravitational force
		body->current->externalForce.SubVec3( 0 ) += body->mass * gravityVector;
	}
}

/*
================
idPhysics_AF::SwapStates
================
*/
void idPhysics_AF::SwapStates()
{
	int i;
	budAFBody* body;
	AFBodyPState_t* swap;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
	
		body = bodies[i];
		
		// swap the current and next state for next simulation step
		swap = body->current;
		body->current = body->next;
		body->next = swap;
	}
}

/*
================
idPhysics_AF::UpdateClipModels
================
*/
void idPhysics_AF::UpdateClipModels()
{
	int i;
	budAFBody* body;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		body->clipModel->Link( gameLocal.clip, self, body->clipModel->GetId(), body->current->worldOrigin, body->current->worldAxis );
	}
}

/*
================
idPhysics_AF::SetSuspendSpeed
================
*/
void idPhysics_AF::SetSuspendSpeed( const budVec2& velocity, const budVec2& acceleration )
{
	this->suspendVelocity = velocity;
	this->suspendAcceleration = acceleration;
}

/*
================
idPhysics_AF::SetSuspendTime
================
*/
void idPhysics_AF::SetSuspendTime( const float minTime, const float maxTime )
{
	this->minMoveTime = minTime;
	this->maxMoveTime = maxTime;
}

/*
================
idPhysics_AF::SetSuspendTolerance
================
*/
void idPhysics_AF::SetSuspendTolerance( const float noMoveTime, const float noMoveTranslation, const float noMoveRotation )
{
	this->noMoveTime = noMoveTime;
	this->noMoveTranslation = noMoveTranslation;
	this->noMoveRotation = noMoveRotation;
}

/*
================
idPhysics_AF::SetTimeScaleRamp
================
*/
void idPhysics_AF::SetTimeScaleRamp( const float start, const float end )
{
	timeScaleRampStart = start;
	timeScaleRampEnd = end;
}

/*
================
idPhysics_AF::SetJointFrictionDent
================
*/
void idPhysics_AF::SetJointFrictionDent( const float dent, const float start, const float end )
{
	jointFrictionDent = dent;
	jointFrictionDentStart = start;
	jointFrictionDentEnd = end;
}

/*
================
idPhysics_AF::GetJointFrictionScale
================
*/
float idPhysics_AF::GetJointFrictionScale() const
{
	if( jointFrictionDentScale > 0.0f )
	{
		return jointFrictionDentScale;
	}
	else if( jointFrictionScale > 0.0f )
	{
		return jointFrictionScale;
	}
	else if( af_jointFrictionScale.GetFloat() > 0.0f )
	{
		return af_jointFrictionScale.GetFloat();
	}
	return 1.0f;
}

/*
================
idPhysics_AF::SetContactFrictionDent
================
*/
void idPhysics_AF::SetContactFrictionDent( const float dent, const float start, const float end )
{
	contactFrictionDent = dent;
	contactFrictionDentStart = start;
	contactFrictionDentEnd = end;
}

/*
================
idPhysics_AF::GetContactFrictionScale
================
*/
float idPhysics_AF::GetContactFrictionScale() const
{
	if( contactFrictionDentScale > 0.0f )
	{
		return contactFrictionDentScale;
	}
	else if( contactFrictionScale > 0.0f )
	{
		return contactFrictionScale;
	}
	else if( af_contactFrictionScale.GetFloat() > 0.0f )
	{
		return af_contactFrictionScale.GetFloat();
	}
	return 1.0f;
}

/*
================
idPhysics_AF::TestIfAtRest
================
*/
bool idPhysics_AF::TestIfAtRest( float timeStep )
{
	int i;
	float translationSqr, maxTranslationSqr, rotation, maxRotation;
	budAFBody* body;
	
	if( current.atRest >= 0 )
	{
		return true;
	}
	
	current.activateTime += timeStep;
	
	// if the simulation should never be suspended before a certaint amount of time passed
	if( minMoveTime > 0.0f && current.activateTime < minMoveTime )
	{
		return false;
	}
	
	// if the simulation should always be suspended after a certain amount time passed
	if( maxMoveTime > 0.0f && current.activateTime > maxMoveTime )
	{
		return true;
	}
	
	// test if all bodies hardly moved over a period of time
	if( current.noMoveTime == 0.0f )
	{
		for( i = 0; i < bodies.Num(); i++ )
		{
			body = bodies[i];
			body->atRestOrigin = body->current->worldOrigin;
			body->atRestAxis = body->current->worldAxis;
		}
		current.noMoveTime += timeStep;
	}
	else if( current.noMoveTime > noMoveTime )
	{
		current.noMoveTime = 0.0f;
		maxTranslationSqr = 0.0f;
		maxRotation = 0.0f;
		for( i = 0; i < bodies.Num(); i++ )
		{
			body = bodies[i];
			
			translationSqr = ( body->current->worldOrigin - body->atRestOrigin ).LengthSqr();
			if( translationSqr > maxTranslationSqr )
			{
				maxTranslationSqr = translationSqr;
			}
			rotation = ( body->atRestAxis.Transpose() * body->current->worldAxis ).ToRotation().GetAngle();
			if( rotation > maxRotation )
			{
				maxRotation = rotation;
			}
		}
		
		if( maxTranslationSqr < Square( noMoveTranslation ) && maxRotation < noMoveRotation )
		{
			// hardly moved over a period of time so the articulated figure may come to rest
			return true;
		}
	}
	else
	{
		current.noMoveTime += timeStep;
	}
	
	// test if the velocity or acceleration of any body is still too large to come to rest
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		if( body->current->spatialVelocity.SubVec3( 0 ).LengthSqr() > Square( suspendVelocity[0] ) )
		{
			return false;
		}
		if( body->current->spatialVelocity.SubVec3( 1 ).LengthSqr() > Square( suspendVelocity[1] ) )
		{
			return false;
		}
		if( body->acceleration.SubVec3( 0 ).LengthSqr() > Square( suspendAcceleration[0] ) )
		{
			return false;
		}
		if( body->acceleration.SubVec3( 1 ).LengthSqr() > Square( suspendAcceleration[1] ) )
		{
			return false;
		}
	}
	
	// all bodies have a velocity and acceleration small enough to come to rest
	return true;
}

/*
================
idPhysics_AF::Rest
================
*/
void idPhysics_AF::Rest()
{
	int i;
	
	current.atRest = gameLocal.time;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		bodies[i]->current->spatialVelocity.Zero();
		bodies[i]->current->externalForce.Zero();
	}
	
	self->BecomeInactive( TH_PHYSICS );
}

/*
================
idPhysics_AF::Activate
================
*/
void idPhysics_AF::Activate()
{
	// if the articulated figure was at rest
	if( current.atRest >= 0 )
	{
		// normally gravity is added at the end of a simulation frame
		// if the figure was at rest add gravity here so it is applied this simulation frame
		AddGravity();
		// reset the active time for the max move time
		current.activateTime = 0.0f;
	}
	current.atRest = -1;
	current.noMoveTime = 0.0f;
	self->BecomeActive( TH_PHYSICS );
}

/*
================
idPhysics_AF::PutToRest

  put to rest untill something collides with this physics object
================
*/
void idPhysics_AF::PutToRest()
{
	Rest();
}

/*
================
idPhysics_AF::EnableImpact
================
*/
void idPhysics_AF::EnableImpact()
{
	noImpact = false;
}

/*
================
idPhysics_AF::DisableImpact
================
*/
void idPhysics_AF::DisableImpact()
{
	noImpact = true;
}

/*
================
idPhysics_AF::AddPushVelocity
================
*/
void idPhysics_AF::AddPushVelocity( const budVec6& pushVelocity )
{
	int i;
	
	if( pushVelocity != vec6_origin )
	{
		for( i = 0; i < bodies.Num(); i++ )
		{
			bodies[i]->current->spatialVelocity += pushVelocity;
		}
	}
}

/*
================
idPhysics_AF::SetClipModel
================
*/
void idPhysics_AF::SetClipModel( budClipModel* model, float density, int id, bool freeOld )
{
}

/*
================
idPhysics_AF::GetClipModel
================
*/
budClipModel* idPhysics_AF::GetClipModel( int id ) const
{
	if( id >= 0 && id < bodies.Num() )
	{
		return bodies[id]->GetClipModel();
	}
	return NULL;
}

/*
================
idPhysics_AF::GetNumClipModels
================
*/
int idPhysics_AF::GetNumClipModels() const
{
	return bodies.Num();
}

/*
================
idPhysics_AF::SetMass
================
*/
void idPhysics_AF::SetMass( float mass, int id )
{
	if( id >= 0 && id < bodies.Num() )
	{
	}
	else
	{
		forceTotalMass = mass;
	}
	SetChanged();
}

/*
================
idPhysics_AF::GetMass
================
*/
float idPhysics_AF::GetMass( int id ) const
{
	if( id >= 0 && id < bodies.Num() )
	{
		return bodies[id]->mass;
	}
	return totalMass;
}

/*
================
idPhysics_AF::SetContents
================
*/
void idPhysics_AF::SetContents( int contents, int id )
{
	int i;
	
	if( id >= 0 && id < bodies.Num() )
	{
		bodies[id]->GetClipModel()->SetContents( contents );
	}
	else
	{
		for( i = 0; i < bodies.Num(); i++ )
		{
			bodies[i]->GetClipModel()->SetContents( contents );
		}
	}
}

/*
================
idPhysics_AF::GetContents
================
*/
int idPhysics_AF::GetContents( int id ) const
{
	int i, contents;
	
	if( id >= 0 && id < bodies.Num() )
	{
		return bodies[id]->GetClipModel()->GetContents();
	}
	else
	{
		contents = 0;
		for( i = 0; i < bodies.Num(); i++ )
		{
			contents |= bodies[i]->GetClipModel()->GetContents();
		}
		return contents;
	}
}

/*
================
idPhysics_AF::GetBounds
================
*/
const budBounds& idPhysics_AF::GetBounds( int id ) const
{
	int i;
	static budBounds relBounds;
	
	if( id >= 0 && id < bodies.Num() )
	{
		return bodies[id]->GetClipModel()->GetBounds();
	}
	else if( !bodies.Num() )
	{
		relBounds.Zero();
		return relBounds;
	}
	else
	{
		relBounds = bodies[0]->GetClipModel()->GetBounds();
		for( i = 1; i < bodies.Num(); i++ )
		{
			budBounds bounds;
			budVec3 origin = ( bodies[i]->GetWorldOrigin() - bodies[0]->GetWorldOrigin() ) * bodies[0]->GetWorldAxis().Transpose();
			budMat3 axis = bodies[i]->GetWorldAxis() * bodies[0]->GetWorldAxis().Transpose();
			bounds.FromTransformedBounds( bodies[i]->GetClipModel()->GetBounds(), origin, axis );
			relBounds += bounds;
		}
		return relBounds;
	}
}

/*
================
idPhysics_AF::GetAbsBounds
================
*/
const budBounds& idPhysics_AF::GetAbsBounds( int id ) const
{
	int i;
	static budBounds absBounds;
	
	if( id >= 0 && id < bodies.Num() )
	{
		return bodies[id]->GetClipModel()->GetAbsBounds();
	}
	else if( !bodies.Num() )
	{
		absBounds.Zero();
		return absBounds;
	}
	else
	{
		absBounds = bodies[0]->GetClipModel()->GetAbsBounds();
		for( i = 1; i < bodies.Num(); i++ )
		{
			absBounds += bodies[i]->GetClipModel()->GetAbsBounds();
		}
		return absBounds;
	}
}

/*
================
idPhysics_AF::Evaluate
================
*/
bool idPhysics_AF::Evaluate( int timeStepMSec, int endTimeMSec )
{
	float timeStep;
	
	if( timeScaleRampStart < MS2SEC( endTimeMSec ) && timeScaleRampEnd > MS2SEC( endTimeMSec ) )
	{
		timeStep = MS2SEC( timeStepMSec ) * ( MS2SEC( endTimeMSec ) - timeScaleRampStart ) / ( timeScaleRampEnd - timeScaleRampStart );
	}
	else if( af_timeScale.GetFloat() != 1.0f )
	{
		timeStep = MS2SEC( timeStepMSec ) * af_timeScale.GetFloat();
	}
	else
	{
		timeStep = MS2SEC( timeStepMSec ) * timeScale;
	}
	current.lastTimeStep = timeStep;
	
	
	// if the articulated figure changed
	if( changedAF || ( linearTime != af_useLinearTime.GetBool() ) )
	{
		BuildTrees();
		changedAF = false;
		linearTime = af_useLinearTime.GetBool();
	}
	
	// get the new master position
	if( masterBody )
	{
		budVec3 masterOrigin;
		budMat3 masterAxis;
		self->GetMasterPosition( masterOrigin, masterAxis );
		if( current.atRest >= 0 && ( masterBody->current->worldOrigin != masterOrigin || masterBody->current->worldAxis != masterAxis ) )
		{
			Activate();
		}
		masterBody->current->worldOrigin = masterOrigin;
		masterBody->current->worldAxis = masterAxis;
	}
	
	// if the simulation is suspended because the figure is at rest
	if( current.atRest >= 0 || timeStep <= 0.0f )
	{
		DebugDraw();
		return false;
	}
	
	// move the af velocity into the frame of a pusher
	AddPushVelocity( -current.pushVelocity );
	
#ifdef AF_TIMINGS
	timer_total.Start();
#endif
	
#ifdef AF_TIMINGS
	timer_collision.Start();
#endif
	
	// evaluate contacts
	EvaluateContacts();
	
	// setup contact constraints
	SetupContactConstraints();
	
#ifdef AF_TIMINGS
	timer_collision.Stop();
#endif
	
	// evaluate constraint equations
	EvaluateConstraints( timeStep );
	
	// apply friction
	ApplyFriction( timeStep, endTimeMSec );
	
	// add frame constraints
	AddFrameConstraints();
	
#ifdef AF_TIMINGS
	int i, numPrimary = 0, numAuxiliary = 0;
	for( i = 0; i < primaryConstraints.Num(); i++ )
	{
		numPrimary += primaryConstraints[i]->J1.GetNumRows();
	}
	for( i = 0; i < auxiliaryConstraints.Num(); i++ )
	{
		numAuxiliary += auxiliaryConstraints[i]->J1.GetNumRows();
	}
	timer_pc.Start();
#endif
	
	// factor matrices for primary constraints
	PrimaryFactor();
	
	// calculate forces on bodies after applying primary constraints
	PrimaryForces( timeStep );
	
#ifdef AF_TIMINGS
	timer_pc.Stop();
	timer_ac.Start();
#endif
	
	// calculate and apply auxiliary constraint forces
	AuxiliaryForces( timeStep );
	
#ifdef AF_TIMINGS
	timer_ac.Stop();
#endif
	
	// evolve current state to next state
	Evolve( timeStep );
	
	// debug graphics
	DebugDraw();
	
	// clear external forces on all bodies
	ClearExternalForce();
	
	// apply contact force to other entities
	ApplyContactForces();
	
	// remove all frame constraints
	RemoveFrameConstraints();
	
#ifdef AF_TIMINGS
	timer_collision.Start();
#endif
	
	// check for collisions between current and next state
	CheckForCollisions( timeStep );
	
#ifdef AF_TIMINGS
	timer_collision.Stop();
#endif
	
	// swap the current and next state
	SwapStates();
	
	// make sure all clip models are disabled in case they were enabled for self collision
	if( selfCollision && !af_skipSelfCollision.GetBool() )
	{
		DisableClip();
	}
	
	// apply collision impulses
	if( ApplyCollisions( timeStep ) )
	{
		current.atRest = gameLocal.time;
		comeToRest = true;
	}
	
	// test if the simulation can be suspended because the whole figure is at rest
	if( comeToRest && TestIfAtRest( timeStep ) )
	{
		Rest();
	}
	else
	{
		ActivateContactEntities();
	}
	
	// add gravitational force
	AddGravity();
	
	// move the af velocity back into the world frame
	AddPushVelocity( current.pushVelocity );
	current.pushVelocity.Zero();
	
	if( IsOutsideWorld() )
	{
		gameLocal.Warning( "articulated figure moved outside world bounds for entity '%s' type '%s' at (%s)",
						   self->name.c_str(), self->GetType()->classname, bodies[0]->current->worldOrigin.ToString( 0 ) );
		Rest();
	}
	
#ifdef AF_TIMINGS
	timer_total.Stop();
	
	if( af_showTimings.GetInteger() == 1 )
	{
		gameLocal.Printf( "%12s: t %1.4f pc %2d, %1.4f ac %2d %1.4f lcp %1.4f cd %1.4f\n",
						  self->name.c_str(),
						  timer_total.Milliseconds(),
						  numPrimary, timer_pc.Milliseconds(),
						  numAuxiliary, timer_ac.Milliseconds() - timer_lcp.Milliseconds(),
						  timer_lcp.Milliseconds(), timer_collision.Milliseconds() );
	}
	else if( af_showTimings.GetInteger() == 2 )
	{
		numArticulatedFigures++;
		if( endTimeMSec > lastTimerReset )
		{
			gameLocal.Printf( "af %d: t %1.4f pc %2d, %1.4f ac %2d %1.4f lcp %1.4f cd %1.4f\n",
							  numArticulatedFigures,
							  timer_total.Milliseconds(),
							  numPrimary, timer_pc.Milliseconds(),
							  numAuxiliary, timer_ac.Milliseconds() - timer_lcp.Milliseconds(),
							  timer_lcp.Milliseconds(), timer_collision.Milliseconds() );
		}
	}
	
	if( endTimeMSec > lastTimerReset )
	{
		lastTimerReset = endTimeMSec;
		numArticulatedFigures = 0;
		timer_total.Clear();
		timer_pc.Clear();
		timer_ac.Clear();
		timer_collision.Clear();
		timer_lcp.Clear();
	}
#endif
	
	return true;
}

/*
================
idPhysics_AF::UpdateTime
================
*/
void idPhysics_AF::UpdateTime( int endTimeMSec )
{
}

/*
================
idPhysics_AF::GetTime
================
*/
int idPhysics_AF::GetTime() const
{
	return gameLocal.time;
}

/*
================
DrawTraceModelSilhouette
================
*/
void DrawTraceModelSilhouette( const budVec3& projectionOrigin, const budClipModel* clipModel )
{
	int i, numSilEdges;
	int silEdges[MAX_TRACEMODEL_EDGES];
	budVec3 v1, v2;
	const budTraceModel* trm = clipModel->GetTraceModel();
	const budVec3& origin = clipModel->GetOrigin();
	const budMat3& axis = clipModel->GetAxis();
	
	numSilEdges = trm->GetProjectionSilhouetteEdges( ( projectionOrigin - origin ) * axis.Transpose(), silEdges );
	for( i = 0; i < numSilEdges; i++ )
	{
		v1 = trm->verts[ trm->edges[ abs( silEdges[i] ) ].v[ INT32_SIGNBITSET( silEdges[i] ) ] ];
		v2 = trm->verts[ trm->edges[ abs( silEdges[i] ) ].v[ INT32_SIGNBITNOTSET( silEdges[i] ) ] ];
		gameRenderWorld->DebugArrow( colorRed, origin + v1 * axis, origin + v2 * axis, 1 );
	}
}

/*
================
idPhysics_AF::DebugDraw
================
*/
void idPhysics_AF::DebugDraw()
{
	int i;
	budAFBody* body, *highlightBody = NULL, *constrainedBody1 = NULL, *constrainedBody2 = NULL;
	budAFConstraint* constraint;
	budVec3 center;
	budMat3 axis;
	
	if( af_highlightConstraint.GetString()[0] )
	{
		constraint = GetConstraint( af_highlightConstraint.GetString() );
		if( constraint )
		{
			constraint->GetCenter( center );
			axis = gameLocal.GetLocalPlayer()->viewAngles.ToMat3();
			gameRenderWorld->DebugCone( colorYellow, center, ( axis[2] - axis[1] ) * 4.0f, 0.0f, 1.0f, 0 );
			
			if( af_showConstrainedBodies.GetBool() )
			{
				cvarSystem->SetCVarString( "cm_drawColor", colorCyan.ToString( 0 ) );
				constrainedBody1 = constraint->body1;
				if( constrainedBody1 )
				{
					collisionModelManager->DrawModel( constrainedBody1->clipModel->Handle(), constrainedBody1->clipModel->GetOrigin(),
													  constrainedBody1->clipModel->GetAxis(), vec3_origin, 0.0f );
				}
				cvarSystem->SetCVarString( "cm_drawColor", colorBlue.ToString( 0 ) );
				constrainedBody2 = constraint->body2;
				if( constrainedBody2 )
				{
					collisionModelManager->DrawModel( constrainedBody2->clipModel->Handle(), constrainedBody2->clipModel->GetOrigin(),
													  constrainedBody2->clipModel->GetAxis(), vec3_origin, 0.0f );
				}
				cvarSystem->SetCVarString( "cm_drawColor", colorRed.ToString( 0 ) );
			}
		}
	}
	
	if( af_highlightBody.GetString()[0] )
	{
		highlightBody = GetBody( af_highlightBody.GetString() );
		if( highlightBody )
		{
			cvarSystem->SetCVarString( "cm_drawColor", colorYellow.ToString( 0 ) );
			collisionModelManager->DrawModel( highlightBody->clipModel->Handle(), highlightBody->clipModel->GetOrigin(),
											  highlightBody->clipModel->GetAxis(), vec3_origin, 0.0f );
			cvarSystem->SetCVarString( "cm_drawColor", colorRed.ToString( 0 ) );
		}
	}
	
	if( af_showBodies.GetBool() )
	{
		for( i = 0; i < bodies.Num(); i++ )
		{
			body = bodies[i];
			if( body == constrainedBody1 || body == constrainedBody2 )
			{
				continue;
			}
			if( body == highlightBody )
			{
				continue;
			}
			collisionModelManager->DrawModel( body->clipModel->Handle(), body->clipModel->GetOrigin(),
											  body->clipModel->GetAxis(), vec3_origin, 0.0f );
			//DrawTraceModelSilhouette( gameLocal.GetLocalPlayer()->GetEyePosition(), body->clipModel );
		}
	}
	
	if( af_showBodyNames.GetBool() )
	{
		for( i = 0; i < bodies.Num(); i++ )
		{
			body = bodies[i];
			gameRenderWorld->DrawText( body->GetName().c_str(), body->GetWorldOrigin(), 0.08f, colorCyan, gameLocal.GetLocalPlayer()->viewAngles.ToMat3(), 1 );
		}
	}
	
	if( af_showMass.GetBool() )
	{
		for( i = 0; i < bodies.Num(); i++ )
		{
			body = bodies[i];
			gameRenderWorld->DrawText( va( "\n%1.2f", 1.0f / body->GetInverseMass() ), body->GetWorldOrigin(), 0.08f, colorCyan, gameLocal.GetLocalPlayer()->viewAngles.ToMat3(), 1 );
		}
	}
	
	if( af_showTotalMass.GetBool() )
	{
		axis = gameLocal.GetLocalPlayer()->viewAngles.ToMat3();
		gameRenderWorld->DrawText( va( "\n%1.2f", totalMass ), bodies[0]->GetWorldOrigin() + axis[2] * 8.0f, 0.15f, colorCyan, axis, 1 );
	}
	
	if( af_showInertia.GetBool() )
	{
		for( i = 0; i < bodies.Num(); i++ )
		{
			body = bodies[i];
			budMat3& I = body->inertiaTensor;
			gameRenderWorld->DrawText( va( "\n\n\n( %.1f %.1f %.1f )\n( %.1f %.1f %.1f )\n( %.1f %.1f %.1f )",
										   I[0].x, I[0].y, I[0].z,
										   I[1].x, I[1].y, I[1].z,
										   I[2].x, I[2].y, I[2].z ),
									   body->GetWorldOrigin(), 0.05f, colorCyan, gameLocal.GetLocalPlayer()->viewAngles.ToMat3(), 1 );
		}
	}
	
	if( af_showVelocity.GetBool() )
	{
		for( i = 0; i < bodies.Num(); i++ )
		{
			DrawVelocity( bodies[i]->clipModel->GetId(), 0.1f, 4.0f );
		}
	}
	
	if( af_showConstraints.GetBool() )
	{
		for( i = 0; i < primaryConstraints.Num(); i++ )
		{
			constraint = primaryConstraints[i];
			constraint->DebugDraw();
		}
		if( !af_showPrimaryOnly.GetBool() )
		{
			for( i = 0; i < auxiliaryConstraints.Num(); i++ )
			{
				constraint = auxiliaryConstraints[i];
				constraint->DebugDraw();
			}
		}
	}
	
	if( af_showConstraintNames.GetBool() )
	{
		for( i = 0; i < primaryConstraints.Num(); i++ )
		{
			constraint = primaryConstraints[i];
			constraint->GetCenter( center );
			gameRenderWorld->DrawText( constraint->GetName().c_str(), center, 0.08f, colorCyan, gameLocal.GetLocalPlayer()->viewAngles.ToMat3(), 1 );
		}
		if( !af_showPrimaryOnly.GetBool() )
		{
			for( i = 0; i < auxiliaryConstraints.Num(); i++ )
			{
				constraint = auxiliaryConstraints[i];
				constraint->GetCenter( center );
				gameRenderWorld->DrawText( constraint->GetName().c_str(), center, 0.08f, colorCyan, gameLocal.GetLocalPlayer()->viewAngles.ToMat3(), 1 );
			}
		}
	}
	
	if( af_showTrees.GetBool() || ( af_showActive.GetBool() && current.atRest < 0 ) )
	{
		for( i = 0; i < trees.Num(); i++ )
		{
			trees[i]->DebugDraw( budStr::ColorForIndex( i + 3 ) );
		}
	}
}

/*
================
idPhysics_AF::idPhysics_AF
================
*/
idPhysics_AF::idPhysics_AF()
{
	trees.Clear();
	bodies.Clear();
	constraints.Clear();
	primaryConstraints.Clear();
	auxiliaryConstraints.Clear();
	frameConstraints.Clear();
	contacts.Clear();
	collisions.Clear();
	changedAF = true;
	masterBody = NULL;
	
	lcp = idLCP::AllocSymmetric();
	
	memset( &current, 0, sizeof( current ) );
	current.atRest = -1;
	current.lastTimeStep = 0.0f;
	saved = current;
	
	linearFriction = 0.005f;
	angularFriction = 0.005f;
	contactFriction = 0.8f;
	bouncyness = 0.4f;
	totalMass = 0.0f;
	forceTotalMass = -1.0f;
	
	suspendVelocity.Set( SUSPEND_LINEAR_VELOCITY, SUSPEND_ANGULAR_VELOCITY );
	suspendAcceleration.Set( SUSPEND_LINEAR_ACCELERATION, SUSPEND_LINEAR_ACCELERATION );
	noMoveTime = NO_MOVE_TIME;
	noMoveTranslation = NO_MOVE_TRANSLATION_TOLERANCE;
	noMoveRotation = NO_MOVE_ROTATION_TOLERANCE;
	minMoveTime = MIN_MOVE_TIME;
	maxMoveTime = MAX_MOVE_TIME;
	impulseThreshold = IMPULSE_THRESHOLD;
	
	timeScale = 1.0f;
	timeScaleRampStart = 0.0f;
	timeScaleRampEnd = 0.0f;
	
	jointFrictionScale = 0.0f;
	jointFrictionDent = 0.0f;
	jointFrictionDentStart = 0.0f;
	jointFrictionDentEnd = 0.0f;
	jointFrictionDentScale = 0.0f;
	
	contactFrictionScale = 0.0f;
	contactFrictionDent = 0.0f;
	contactFrictionDentStart = 0.0f;
	contactFrictionDentEnd = 0.0f;
	contactFrictionDentScale = 0.0f;
	
	enableCollision = true;
	selfCollision = true;
	comeToRest = true;
	linearTime = true;
	noImpact = false;
	worldConstraintsLocked = false;
	forcePushable = false;
	
#ifdef AF_TIMINGS
	lastTimerReset = 0;
#endif
}

/*
================
idPhysics_AF::~idPhysics_AF
================
*/
idPhysics_AF::~idPhysics_AF()
{
	int i;
	
	trees.DeleteContents( true );
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		delete bodies[i];
	}
	
	for( i = 0; i < constraints.Num(); i++ )
	{
		delete constraints[i];
	}
	
	contactConstraints.SetNum( contactConstraints.NumAllocated() );
	for( i = 0; i < contactConstraints.Num(); i++ )
	{
		delete contactConstraints[i];
	}
	
	delete lcp;
	
	if( masterBody )
	{
		delete masterBody;
	}
}

/*
================
idPhysics_AF_SavePState
================
*/
void idPhysics_AF_SavePState( idSaveGame* saveFile, const AFPState_t& state )
{
	saveFile->WriteInt( state.atRest );
	saveFile->WriteFloat( state.noMoveTime );
	saveFile->WriteFloat( state.activateTime );
	saveFile->WriteFloat( state.lastTimeStep );
	saveFile->WriteVec6( state.pushVelocity );
}

/*
================
idPhysics_AF_RestorePState
================
*/
void idPhysics_AF_RestorePState( idRestoreGame* saveFile, AFPState_t& state )
{
	saveFile->ReadInt( state.atRest );
	saveFile->ReadFloat( state.noMoveTime );
	saveFile->ReadFloat( state.activateTime );
	saveFile->ReadFloat( state.lastTimeStep );
	saveFile->ReadVec6( state.pushVelocity );
}

/*
================
idPhysics_AF::Save
================
*/
void idPhysics_AF::Save( idSaveGame* saveFile ) const
{
	int i;
	
	// the articulated figure structure is handled by the owner
	
	idPhysics_AF_SavePState( saveFile, current );
	idPhysics_AF_SavePState( saveFile, saved );
	
	saveFile->WriteInt( bodies.Num() );
	for( i = 0; i < bodies.Num(); i++ )
	{
		bodies[i]->Save( saveFile );
	}
	if( masterBody )
	{
		saveFile->WriteBool( true );
		masterBody->Save( saveFile );
	}
	else
	{
		saveFile->WriteBool( false );
	}
	
	saveFile->WriteInt( constraints.Num() );
	for( i = 0; i < constraints.Num(); i++ )
	{
		constraints[i]->Save( saveFile );
	}
	
	saveFile->WriteBool( changedAF );
	
	saveFile->WriteFloat( linearFriction );
	saveFile->WriteFloat( angularFriction );
	saveFile->WriteFloat( contactFriction );
	saveFile->WriteFloat( bouncyness );
	saveFile->WriteFloat( totalMass );
	saveFile->WriteFloat( forceTotalMass );
	
	saveFile->WriteVec2( suspendVelocity );
	saveFile->WriteVec2( suspendAcceleration );
	saveFile->WriteFloat( noMoveTime );
	saveFile->WriteFloat( noMoveTranslation );
	saveFile->WriteFloat( noMoveRotation );
	saveFile->WriteFloat( minMoveTime );
	saveFile->WriteFloat( maxMoveTime );
	saveFile->WriteFloat( impulseThreshold );
	
	saveFile->WriteFloat( timeScale );
	saveFile->WriteFloat( timeScaleRampStart );
	saveFile->WriteFloat( timeScaleRampEnd );
	
	saveFile->WriteFloat( jointFrictionScale );
	saveFile->WriteFloat( jointFrictionDent );
	saveFile->WriteFloat( jointFrictionDentStart );
	saveFile->WriteFloat( jointFrictionDentEnd );
	saveFile->WriteFloat( jointFrictionDentScale );
	
	saveFile->WriteFloat( contactFrictionScale );
	saveFile->WriteFloat( contactFrictionDent );
	saveFile->WriteFloat( contactFrictionDentStart );
	saveFile->WriteFloat( contactFrictionDentEnd );
	saveFile->WriteFloat( contactFrictionDentScale );
	
	saveFile->WriteBool( enableCollision );
	saveFile->WriteBool( selfCollision );
	saveFile->WriteBool( comeToRest );
	saveFile->WriteBool( linearTime );
	saveFile->WriteBool( noImpact );
	saveFile->WriteBool( worldConstraintsLocked );
	saveFile->WriteBool( forcePushable );
}

/*
================
idPhysics_AF::Restore
================
*/
void idPhysics_AF::Restore( idRestoreGame* saveFile )
{
	int i, num;
	bool hasMaster;
	
	// the articulated figure structure should have already been restored
	
	idPhysics_AF_RestorePState( saveFile, current );
	idPhysics_AF_RestorePState( saveFile, saved );
	
	saveFile->ReadInt( num );
	assert( num == bodies.Num() );
	for( i = 0; i < bodies.Num(); i++ )
	{
		bodies[i]->Restore( saveFile );
	}
	saveFile->ReadBool( hasMaster );
	if( hasMaster )
	{
		masterBody = new( TAG_PHYSICS_AF ) budAFBody();
		masterBody->Restore( saveFile );
	}
	
	saveFile->ReadInt( num );
	assert( num == constraints.Num() );
	for( i = 0; i < constraints.Num(); i++ )
	{
		constraints[i]->Restore( saveFile );
	}
	
	saveFile->ReadBool( changedAF );
	
	saveFile->ReadFloat( linearFriction );
	saveFile->ReadFloat( angularFriction );
	saveFile->ReadFloat( contactFriction );
	saveFile->ReadFloat( bouncyness );
	saveFile->ReadFloat( totalMass );
	saveFile->ReadFloat( forceTotalMass );
	
	saveFile->ReadVec2( suspendVelocity );
	saveFile->ReadVec2( suspendAcceleration );
	saveFile->ReadFloat( noMoveTime );
	saveFile->ReadFloat( noMoveTranslation );
	saveFile->ReadFloat( noMoveRotation );
	saveFile->ReadFloat( minMoveTime );
	saveFile->ReadFloat( maxMoveTime );
	saveFile->ReadFloat( impulseThreshold );
	
	saveFile->ReadFloat( timeScale );
	saveFile->ReadFloat( timeScaleRampStart );
	saveFile->ReadFloat( timeScaleRampEnd );
	
	saveFile->ReadFloat( jointFrictionScale );
	saveFile->ReadFloat( jointFrictionDent );
	saveFile->ReadFloat( jointFrictionDentStart );
	saveFile->ReadFloat( jointFrictionDentEnd );
	saveFile->ReadFloat( jointFrictionDentScale );
	
	saveFile->ReadFloat( contactFrictionScale );
	saveFile->ReadFloat( contactFrictionDent );
	saveFile->ReadFloat( contactFrictionDentStart );
	saveFile->ReadFloat( contactFrictionDentEnd );
	saveFile->ReadFloat( contactFrictionDentScale );
	
	saveFile->ReadBool( enableCollision );
	saveFile->ReadBool( selfCollision );
	saveFile->ReadBool( comeToRest );
	saveFile->ReadBool( linearTime );
	saveFile->ReadBool( noImpact );
	saveFile->ReadBool( worldConstraintsLocked );
	saveFile->ReadBool( forcePushable );
	
	changedAF = true;
	
	UpdateClipModels();
}

/*
================
idPhysics_AF::IsClosedLoop
================
*/
bool idPhysics_AF::IsClosedLoop( const budAFBody* body1, const budAFBody* body2 ) const
{
	const budAFBody* b1, *b2;
	
	for( b1 = body1; b1->parent; b1 = b1->parent )
	{
	}
	for( b2 = body2; b2->parent; b2 = b2->parent )
	{
	}
	return ( b1 == b2 );
}

/*
================
idPhysics_AF::BuildTrees
================
*/
void idPhysics_AF::BuildTrees()
{
	int i;
	float scale;
	budAFBody* b;
	budAFConstraint* c;
	budAFTree* tree;
	
	primaryConstraints.Clear();
	auxiliaryConstraints.Clear();
	trees.DeleteContents( true );
	
	totalMass = 0.0f;
	for( i = 0; i < bodies.Num(); i++ )
	{
		b = bodies[i];
		b->parent = NULL;
		b->primaryConstraint = NULL;
		b->constraints.SetNum( 0 );
		b->children.Clear();
		b->tree = NULL;
		totalMass += b->mass;
	}
	
	if( forceTotalMass > 0.0f )
	{
		scale = forceTotalMass / totalMass;
		for( i = 0; i < bodies.Num(); i++ )
		{
			b = bodies[i];
			b->mass *= scale;
			b->invMass = 1.0f / b->mass;
			b->inertiaTensor *= scale;
			b->inverseInertiaTensor = b->inertiaTensor.Inverse();
		}
		totalMass = forceTotalMass;
	}
	
	if( af_useLinearTime.GetBool() )
	{
	
		for( i = 0; i < constraints.Num(); i++ )
		{
			c = constraints[i];
			
			c->body1->constraints.Append( c );
			if( c->body2 )
			{
				c->body2->constraints.Append( c );
			}
			
			// only bilateral constraints between two non-world bodies that do not
			// create loops can be used as primary constraints
			if( !c->body1->primaryConstraint && c->fl.allowPrimary && c->body2 != NULL && !IsClosedLoop( c->body1, c->body2 ) )
			{
				c->body1->primaryConstraint = c;
				c->body1->parent = c->body2;
				c->body2->children.Append( c->body1 );
				c->fl.isPrimary = true;
				c->firstIndex = 0;
				primaryConstraints.Append( c );
			}
			else
			{
				c->fl.isPrimary = false;
				auxiliaryConstraints.Append( c );
			}
		}
		
		// create trees for all parent bodies
		for( i = 0; i < bodies.Num(); i++ )
		{
			if( !bodies[i]->parent )
			{
				tree = new( TAG_PHYSICS_AF ) budAFTree();
				tree->sortedBodies.Clear();
				tree->sortedBodies.Append( bodies[i] );
				bodies[i]->tree = tree;
				trees.Append( tree );
			}
		}
		
		// add each child body to the appropriate tree
		for( i = 0; i < bodies.Num(); i++ )
		{
			if( bodies[i]->parent )
			{
				for( b = bodies[i]->parent; !b->tree; b = b->parent )
				{
				}
				b->tree->sortedBodies.Append( bodies[i] );
				bodies[i]->tree = b->tree;
			}
		}
		
		if( trees.Num() > 1 )
		{
			gameLocal.Warning( "Articulated figure has multiple separate tree structures for entity '%s' type '%s'.",
							   self->name.c_str(), self->GetType()->classname );
		}
		
		// sort bodies in each tree to make sure parents come first
		for( i = 0; i < trees.Num(); i++ )
		{
			trees[i]->SortBodies();
		}
		
	}
	else
	{
	
		// create a tree for each body
		for( i = 0; i < bodies.Num(); i++ )
		{
			tree = new( TAG_PHYSICS_AF ) budAFTree();
			tree->sortedBodies.Clear();
			tree->sortedBodies.Append( bodies[i] );
			bodies[i]->tree = tree;
			trees.Append( tree );
		}
		
		for( i = 0; i < constraints.Num(); i++ )
		{
			c = constraints[i];
			
			c->body1->constraints.Append( c );
			if( c->body2 )
			{
				c->body2->constraints.Append( c );
			}
			
			c->fl.isPrimary = false;
			auxiliaryConstraints.Append( c );
		}
	}
}

/*
================
idPhysics_AF::AddBody

  bodies get an id in the order they are added starting at zero
  as such the first body added will get id zero
================
*/
int idPhysics_AF::AddBody( budAFBody* body )
{
	int id = 0;
	
	if( body->clipModel == NULL )
	{
		gameLocal.Error( "idPhysics_AF::AddBody: body '%s' has no clip model.", body->name.c_str() );
		return 0;
	}
	
	if( bodies.Find( body ) )
	{
		gameLocal.Error( "idPhysics_AF::AddBody: body '%s' added twice.", body->name.c_str() );
	}
	
	if( GetBody( body->name ) )
	{
		gameLocal.Error( "idPhysics_AF::AddBody: a body with the name '%s' already exists.", body->name.c_str() );
	}
	
	id = bodies.Num();
	body->clipModel->SetId( id );
	if( body->linearFriction < 0.0f )
	{
		body->linearFriction = linearFriction;
		body->angularFriction = angularFriction;
		body->contactFriction = contactFriction;
	}
	if( body->bouncyness < 0.0f )
	{
		body->bouncyness = bouncyness;
	}
	if( !body->fl.clipMaskSet )
	{
		body->clipMask = clipMask;
	}
	
	bodies.Append( body );
	
	changedAF = true;
	
	return id;
}

/*
================
idPhysics_AF::AddConstraint
================
*/
void idPhysics_AF::AddConstraint( budAFConstraint* constraint )
{

	if( constraints.Find( constraint ) )
	{
		gameLocal.Error( "idPhysics_AF::AddConstraint: constraint '%s' added twice.", constraint->name.c_str() );
	}
	if( GetConstraint( constraint->name ) )
	{
		gameLocal.Error( "idPhysics_AF::AddConstraint: a constraint with the name '%s' already exists.", constraint->name.c_str() );
	}
	if( !constraint->body1 )
	{
		gameLocal.Error( "idPhysics_AF::AddConstraint: body1 == NULL on constraint '%s'.", constraint->name.c_str() );
	}
	if( !bodies.Find( constraint->body1 ) )
	{
		gameLocal.Error( "idPhysics_AF::AddConstraint: body1 of constraint '%s' is not part of the articulated figure.", constraint->name.c_str() );
	}
	if( constraint->body2 && !bodies.Find( constraint->body2 ) )
	{
		gameLocal.Error( "idPhysics_AF::AddConstraint: body2 of constraint '%s' is not part of the articulated figure.", constraint->name.c_str() );
	}
	if( constraint->body1 == constraint->body2 )
	{
		gameLocal.Error( "idPhysics_AF::AddConstraint: body1 and body2 of constraint '%s' are the same.", constraint->name.c_str() );
	}
	
	constraints.Append( constraint );
	constraint->physics = this;
	
	changedAF = true;
}

/*
================
idPhysics_AF::AddFrameConstraint
================
*/
void idPhysics_AF::AddFrameConstraint( budAFConstraint* constraint )
{
	frameConstraints.Append( constraint );
	constraint->physics = this;
}

/*
================
idPhysics_AF::ForceBodyId
================
*/
void idPhysics_AF::ForceBodyId( budAFBody* body, int newId )
{
	int id;
	
	id = bodies.FindIndex( body );
	if( id == -1 )
	{
		gameLocal.Error( "ForceBodyId: body '%s' is not part of the articulated figure.\n", body->name.c_str() );
	}
	if( id != newId )
	{
		budAFBody* b = bodies[newId];
		bodies[newId] = bodies[id];
		bodies[id] = b;
		changedAF = true;
	}
}

/*
================
idPhysics_AF::GetBodyId
================
*/
int idPhysics_AF::GetBodyId( budAFBody* body ) const
{
	int id;
	
	id = bodies.FindIndex( body );
	if( id == -1 && body )
	{
		gameLocal.Error( "GetBodyId: body '%s' is not part of the articulated figure.\n", body->name.c_str() );
	}
	return id;
}

/*
================
idPhysics_AF::GetBodyId
================
*/
int idPhysics_AF::GetBodyId( const char* bodyName ) const
{
	int i;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		if( !bodies[i]->name.Icmp( bodyName ) )
		{
			return i;
		}
	}
	gameLocal.Error( "GetBodyId: no body with the name '%s' is not part of the articulated figure.\n", bodyName );
	return 0;
}

/*
================
idPhysics_AF::GetConstraintId
================
*/
int idPhysics_AF::GetConstraintId( budAFConstraint* constraint ) const
{
	int id;
	
	id = constraints.FindIndex( constraint );
	if( id == -1 && constraint )
	{
		gameLocal.Error( "GetConstraintId: constraint '%s' is not part of the articulated figure.\n", constraint->name.c_str() );
	}
	return id;
}

/*
================
idPhysics_AF::GetConstraintId
================
*/
int idPhysics_AF::GetConstraintId( const char* constraintName ) const
{
	int i;
	
	for( i = 0; i < constraints.Num(); i++ )
	{
		if( constraints[i]->name.Icmp( constraintName ) == 0 )
		{
			return i;
		}
	}
	gameLocal.Error( "GetConstraintId: no constraint with the name '%s' is not part of the articulated figure.\n", constraintName );
	return 0;
}

/*
================
idPhysics_AF::GetNumBodies
================
*/
int idPhysics_AF::GetNumBodies() const
{
	return bodies.Num();
}

/*
================
idPhysics_AF::GetNumConstraints
================
*/
int idPhysics_AF::GetNumConstraints() const
{
	return constraints.Num();
}

/*
================
idPhysics_AF::GetBody
================
*/
budAFBody* idPhysics_AF::GetBody( const char* bodyName ) const
{
	int i;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		if( !bodies[i]->name.Icmp( bodyName ) )
		{
			return bodies[i];
		}
	}
	
	return NULL;
}

/*
================
idPhysics_AF::GetBody
================
*/
budAFBody* idPhysics_AF::GetBody( const int id ) const
{
	if( id < 0 || id >= bodies.Num() )
	{
		gameLocal.Error( "GetBody: no body with id %d exists\n", id );
		return NULL;
	}
	return bodies[id];
}

/*
================
idPhysics_AF::GetConstraint
================
*/
budAFConstraint* idPhysics_AF::GetConstraint( const char* constraintName ) const
{
	int i;
	
	for( i = 0; i < constraints.Num(); i++ )
	{
		if( constraints[i]->name.Icmp( constraintName ) == 0 )
		{
			return constraints[i];
		}
	}
	
	return NULL;
}

/*
================
idPhysics_AF::GetConstraint
================
*/
budAFConstraint* idPhysics_AF::GetConstraint( const int id ) const
{
	if( id < 0 || id >= constraints.Num() )
	{
		gameLocal.Error( "GetConstraint: no constraint with id %d exists\n", id );
		return NULL;
	}
	return constraints[id];
}

/*
================
idPhysics_AF::DeleteBody
================
*/
void idPhysics_AF::DeleteBody( const char* bodyName )
{
	int i;
	
	// find the body with the given name
	for( i = 0; i < bodies.Num(); i++ )
	{
		if( !bodies[i]->name.Icmp( bodyName ) )
		{
			break;
		}
	}
	
	if( i >= bodies.Num() )
	{
		gameLocal.Warning( "DeleteBody: no body found in the articulated figure with the name '%s' for entity '%s' type '%s'.",
						   bodyName, self->name.c_str(), self->GetType()->classname );
		return;
	}
	
	DeleteBody( i );
}

/*
================
idPhysics_AF::DeleteBody
================
*/
void idPhysics_AF::DeleteBody( const int id )
{
	int j;
	
	if( id < 0 || id > bodies.Num() )
	{
		gameLocal.Error( "DeleteBody: no body with id %d.", id );
		return;
	}
	
	// remove any constraints attached to this body
	for( j = 0; j < constraints.Num(); j++ )
	{
		if( constraints[j]->body1 == bodies[id] || constraints[j]->body2 == bodies[id] )
		{
			delete constraints[j];
			constraints.RemoveIndex( j );
			j--;
		}
	}
	
	// remove the body
	delete bodies[id];
	bodies.RemoveIndex( id );
	
	// set new body ids
	for( j = 0; j < bodies.Num(); j++ )
	{
		bodies[j]->clipModel->SetId( j );
	}
	
	changedAF = true;
}

/*
================
idPhysics_AF::DeleteConstraint
================
*/
void idPhysics_AF::DeleteConstraint( const char* constraintName )
{
	int i;
	
	// find the constraint with the given name
	for( i = 0; i < constraints.Num(); i++ )
	{
		if( !constraints[i]->name.Icmp( constraintName ) )
		{
			break;
		}
	}
	
	if( i >= constraints.Num() )
	{
		gameLocal.Warning( "DeleteConstraint: no constriant found in the articulated figure with the name '%s' for entity '%s' type '%s'.",
						   constraintName, self->name.c_str(), self->GetType()->classname );
		return;
	}
	
	DeleteConstraint( i );
}

/*
================
idPhysics_AF::DeleteConstraint
================
*/
void idPhysics_AF::DeleteConstraint( const int id )
{

	if( id < 0 || id >= constraints.Num() )
	{
		gameLocal.Error( "DeleteConstraint: no constraint with id %d.", id );
		return;
	}
	
	// remove the constraint
	delete constraints[id];
	constraints.RemoveIndex( id );
	
	changedAF = true;
}

/*
================
idPhysics_AF::GetBodyContactConstraints
================
*/
int idPhysics_AF::GetBodyContactConstraints( const int id, budAFConstraint_Contact* contacts[], int maxContacts ) const
{
	int i, numContacts;
	budAFBody* body;
	budAFConstraint_Contact* contact;
	
	if( id < 0 || id >= bodies.Num() || maxContacts <= 0 )
	{
		return 0;
	}
	
	numContacts = 0;
	body = bodies[id];
	for( i = 0; i < contactConstraints.Num(); i++ )
	{
		contact = contactConstraints[i];
		if( contact->body1 == body || contact->body2 == body )
		{
			contacts[numContacts++] = contact;
			if( numContacts >= maxContacts )
			{
				return numContacts;
			}
		}
	}
	return numContacts;
}

/*
================
idPhysics_AF::SetDefaultFriction
================
*/
void idPhysics_AF::SetDefaultFriction( float linear, float angular, float contact )
{
	if(	linear < 0.0f || linear > 1.0f ||
			angular < 0.0f || angular > 1.0f ||
			contact < 0.0f || contact > 1.0f )
	{
		return;
	}
	linearFriction = linear;
	angularFriction = angular;
	contactFriction = contact;
}

/*
================
idPhysics_AF::GetImpactInfo
================
*/
void idPhysics_AF::GetImpactInfo( const int id, const budVec3& point, impactInfo_t* info ) const
{
	if( id < 0 || id >= bodies.Num() )
	{
		memset( info, 0, sizeof( *info ) );
		return;
	}
	info->invMass = 1.0f / bodies[id]->mass;
	info->invInertiaTensor = bodies[id]->current->worldAxis.Transpose() * bodies[id]->inverseInertiaTensor * bodies[id]->current->worldAxis;
	info->position = point - bodies[id]->current->worldOrigin;
	info->velocity = bodies[id]->current->spatialVelocity.SubVec3( 0 ) + bodies[id]->current->spatialVelocity.SubVec3( 1 ).Cross( info->position );
}

/*
================
idPhysics_AF::ApplyImpulse
================
*/
void idPhysics_AF::ApplyImpulse( const int id, const budVec3& point, const budVec3& impulse )
{
	if( id < 0 || id >= bodies.Num() )
	{
		return;
	}
	if( noImpact || impulse.LengthSqr() < Square( impulseThreshold ) )
	{
		return;
	}
	const float maxImpulse =  100000.0f;
	const float maxRotation = 100000.0f;
	budMat3 invWorldInertiaTensor = bodies[id]->current->worldAxis.Transpose() * bodies[id]->inverseInertiaTensor * bodies[id]->current->worldAxis;
	bodies[id]->current->spatialVelocity.SubVec3( 0 ) += bodies[id]->invMass * impulse.Truncate( maxImpulse );
	bodies[id]->current->spatialVelocity.SubVec3( 1 ) += invWorldInertiaTensor * ( point - bodies[id]->current->worldOrigin ).Cross( impulse ).Truncate( maxRotation );
	Activate();
}

/*
================
idPhysics_AF::AddForce
================
*/
void idPhysics_AF::AddForce( const int id, const budVec3& point, const budVec3& force )
{
	if( noImpact )
	{
		return;
	}
	if( id < 0 || id >= bodies.Num() )
	{
		return;
	}
	bodies[id]->current->externalForce.SubVec3( 0 ) += force;
	bodies[id]->current->externalForce.SubVec3( 1 ) += ( point - bodies[id]->current->worldOrigin ).Cross( force );
	Activate();
}

/*
================
idPhysics_AF::IsAtRest
================
*/
bool idPhysics_AF::IsAtRest() const
{
	return current.atRest >= 0;
}

/*
================
idPhysics_AF::GetRestStartTime
================
*/
int idPhysics_AF::GetRestStartTime() const
{
	return current.atRest;
}

/*
================
idPhysics_AF::IsPushable
================
*/
bool idPhysics_AF::IsPushable() const
{
	return ( !noImpact && ( masterBody == NULL || forcePushable ) );
}

/*
================
idPhysics_AF::SaveState
================
*/
void idPhysics_AF::SaveState()
{
	int i;
	
	saved = current;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		memcpy( &bodies[i]->saved, bodies[i]->current, sizeof( AFBodyPState_t ) );
	}
}

/*
================
idPhysics_AF::RestoreState
================
*/
void idPhysics_AF::RestoreState()
{
	int i;
	
	current = saved;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		*( bodies[i]->current ) = bodies[i]->saved;
	}
	
	EvaluateContacts();
}

/*
================
idPhysics_AF::SetOrigin
================
*/
void idPhysics_AF::SetOrigin( const budVec3& newOrigin, int id )
{
	if( masterBody )
	{
		Translate( masterBody->current->worldOrigin + masterBody->current->worldAxis * newOrigin - bodies[0]->current->worldOrigin );
	}
	else
	{
		Translate( newOrigin - bodies[0]->current->worldOrigin );
	}
}

/*
================
idPhysics_AF::SetAxis
================
*/
void idPhysics_AF::SetAxis( const budMat3& newAxis, int id )
{
	budMat3 axis;
	budRotation rotation;
	
	if( masterBody )
	{
		axis = bodies[0]->current->worldAxis.Transpose() * ( newAxis * masterBody->current->worldAxis );
	}
	else
	{
		axis = bodies[0]->current->worldAxis.Transpose() * newAxis;
	}
	rotation = axis.ToRotation();
	rotation.SetOrigin( bodies[0]->current->worldOrigin );
	
	Rotate( rotation );
}

/*
================
idPhysics_AF::Translate
================
*/
void idPhysics_AF::Translate( const budVec3& translation, int id )
{
	int i;
	budAFBody* body;
	
	if( !worldConstraintsLocked )
	{
		// translate constraints attached to the world
		for( i = 0; i < constraints.Num(); i++ )
		{
			constraints[i]->Translate( translation );
		}
	}
	
	// translate all the bodies
	for( i = 0; i < bodies.Num(); i++ )
	{
	
		body = bodies[i];
		body->current->worldOrigin += translation;
	}
	
	Activate();
	
	UpdateClipModels();
}

/*
================
idPhysics_AF::Rotate
================
*/
void idPhysics_AF::Rotate( const budRotation& rotation, int id )
{
	int i;
	budAFBody* body;
	
	if( !worldConstraintsLocked )
	{
		// rotate constraints attached to the world
		for( i = 0; i < constraints.Num(); i++ )
		{
			constraints[i]->Rotate( rotation );
		}
	}
	
	// rotate all the bodies
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		body->current->worldOrigin *= rotation;
		body->current->worldAxis *= rotation.ToMat3();
	}
	
	Activate();
	
	UpdateClipModels();
}

/*
================
idPhysics_AF::GetOrigin
================
*/
const budVec3& idPhysics_AF::GetOrigin( int id ) const
{
	if( id < 0 || id >= bodies.Num() )
	{
		return vec3_origin;
	}
	else
	{
		return bodies[id]->current->worldOrigin;
	}
}

/*
================
idPhysics_AF::GetAxis
================
*/
const budMat3& idPhysics_AF::GetAxis( int id ) const
{
	if( id < 0 || id >= bodies.Num() )
	{
		return mat3_identity;
	}
	else
	{
		return bodies[id]->current->worldAxis;
	}
}

/*
================
idPhysics_AF::SetLinearVelocity
================
*/
void idPhysics_AF::SetLinearVelocity( const budVec3& newLinearVelocity, int id )
{
	if( id < 0 || id >= bodies.Num() )
	{
		return;
	}
	bodies[id]->current->spatialVelocity.SubVec3( 0 ) = newLinearVelocity;
	Activate();
}

/*
================
idPhysics_AF::SetAngularVelocity
================
*/
void idPhysics_AF::SetAngularVelocity( const budVec3& newAngularVelocity, int id )
{
	if( id < 0 || id >= bodies.Num() )
	{
		return;
	}
	bodies[id]->current->spatialVelocity.SubVec3( 1 ) = newAngularVelocity;
	Activate();
}

/*
================
idPhysics_AF::GetLinearVelocity
================
*/
const budVec3& idPhysics_AF::GetLinearVelocity( int id ) const
{
	if( id < 0 || id >= bodies.Num() )
	{
		return vec3_origin;
	}
	else
	{
		return bodies[id]->current->spatialVelocity.SubVec3( 0 );
	}
}

/*
================
idPhysics_AF::GetAngularVelocity
================
*/
const budVec3& idPhysics_AF::GetAngularVelocity( int id ) const
{
	if( id < 0 || id >= bodies.Num() )
	{
		return vec3_origin;
	}
	else
	{
		return bodies[id]->current->spatialVelocity.SubVec3( 1 );
	}
}

/*
================
idPhysics_AF::ClipTranslation
================
*/
void idPhysics_AF::ClipTranslation( trace_t& results, const budVec3& translation, const budClipModel* model ) const
{
	int i;
	budAFBody* body;
	trace_t bodyResults;
	
	results.fraction = 1.0f;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		if( body->clipModel->IsTraceModel() )
		{
			if( model )
			{
				gameLocal.clip.TranslationModel( bodyResults, body->current->worldOrigin, body->current->worldOrigin + translation,
												 body->clipModel, body->current->worldAxis, body->clipMask,
												 model->Handle(), model->GetOrigin(), model->GetAxis() );
			}
			else
			{
				gameLocal.clip.Translation( bodyResults, body->current->worldOrigin, body->current->worldOrigin + translation,
											body->clipModel, body->current->worldAxis, body->clipMask, self );
			}
			if( bodyResults.fraction < results.fraction )
			{
				results = bodyResults;
			}
		}
	}
	
	results.endpos = bodies[0]->current->worldOrigin + results.fraction * translation;
	results.endAxis = bodies[0]->current->worldAxis;
}

/*
================
idPhysics_AF::ClipRotation
================
*/
void idPhysics_AF::ClipRotation( trace_t& results, const budRotation& rotation, const budClipModel* model ) const
{
	int i;
	budAFBody* body;
	trace_t bodyResults;
	budRotation partialRotation;
	
	results.fraction = 1.0f;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		if( body->clipModel->IsTraceModel() )
		{
			if( model )
			{
				gameLocal.clip.RotationModel( bodyResults, body->current->worldOrigin, rotation,
											  body->clipModel, body->current->worldAxis, body->clipMask,
											  model->Handle(), model->GetOrigin(), model->GetAxis() );
			}
			else
			{
				gameLocal.clip.Rotation( bodyResults, body->current->worldOrigin, rotation,
										 body->clipModel, body->current->worldAxis, body->clipMask, self );
			}
			if( bodyResults.fraction < results.fraction )
			{
				results = bodyResults;
			}
		}
	}
	
	partialRotation = rotation * results.fraction;
	results.endpos = bodies[0]->current->worldOrigin * partialRotation;
	results.endAxis = bodies[0]->current->worldAxis * partialRotation.ToMat3();
}

/*
================
idPhysics_AF::ClipContents
================
*/
int idPhysics_AF::ClipContents( const budClipModel* model ) const
{
	int i, contents;
	budAFBody* body;
	
	contents = 0;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		body = bodies[i];
		
		if( body->clipModel->IsTraceModel() )
		{
			if( model )
			{
				contents |= gameLocal.clip.ContentsModel( body->current->worldOrigin,
							body->clipModel, body->current->worldAxis, -1,
							model->Handle(), model->GetOrigin(), model->GetAxis() );
			}
			else
			{
				contents |= gameLocal.clip.Contents( body->current->worldOrigin,
													 body->clipModel, body->current->worldAxis, -1, NULL );
			}
		}
	}
	
	return contents;
}

/*
================
idPhysics_AF::DisableClip
================
*/
void idPhysics_AF::DisableClip()
{
	int i;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		bodies[i]->clipModel->Disable();
	}
}

/*
================
idPhysics_AF::EnableClip
================
*/
void idPhysics_AF::EnableClip()
{
	int i;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		bodies[i]->clipModel->Enable();
	}
}

/*
================
idPhysics_AF::UnlinkClip
================
*/
void idPhysics_AF::UnlinkClip()
{
	int i;
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		bodies[i]->clipModel->Unlink();
	}
}

/*
================
idPhysics_AF::LinkClip
================
*/
void idPhysics_AF::LinkClip()
{
	UpdateClipModels();
}

/*
================
idPhysics_AF::SetPushed
================
*/
void idPhysics_AF::SetPushed( int deltaTime )
{
	budAFBody* body;
	budRotation rotation;
	
	if( bodies.Num() )
	{
		body = bodies[0];
		rotation = ( body->saved.worldAxis.Transpose() * body->current->worldAxis ).ToRotation();
		
		// velocity with which the af is pushed
		current.pushVelocity.SubVec3( 0 ) += ( body->current->worldOrigin - body->saved.worldOrigin ) / ( deltaTime * budMath::M_MS2SEC );
		current.pushVelocity.SubVec3( 1 ) += rotation.GetVec() * -DEG2RAD( rotation.GetAngle() ) / ( deltaTime * budMath::M_MS2SEC );
	}
}

/*
================
idPhysics_AF::GetPushedLinearVelocity
================
*/
const budVec3& idPhysics_AF::GetPushedLinearVelocity( const int id ) const
{
	return current.pushVelocity.SubVec3( 0 );
}

/*
================
idPhysics_AF::GetPushedAngularVelocity
================
*/
const budVec3& idPhysics_AF::GetPushedAngularVelocity( const int id ) const
{
	return current.pushVelocity.SubVec3( 1 );
}

/*
================
idPhysics_AF::SetMaster

   the binding is orientated based on the constraints being used
================
*/
void idPhysics_AF::SetMaster( idEntity* master, const bool orientated )
{
	int i;
	budVec3 masterOrigin;
	budMat3 masterAxis;
	budRotation rotation;
	
	if( master )
	{
		self->GetMasterPosition( masterOrigin, masterAxis );
		if( !masterBody )
		{
			masterBody = new( TAG_PHYSICS_AF ) budAFBody();
			// translate and rotate all the constraints with body2 == NULL from world space to master space
			rotation = masterAxis.Transpose().ToRotation();
			for( i = 0; i < constraints.Num(); i++ )
			{
				if( constraints[i]->GetBody2() == NULL )
				{
					constraints[i]->Translate( -masterOrigin );
					constraints[i]->Rotate( rotation );
				}
			}
			Activate();
		}
		masterBody->current->worldOrigin = masterOrigin;
		masterBody->current->worldAxis = masterAxis;
	}
	else
	{
		if( masterBody )
		{
			// translate and rotate all the constraints with body2 == NULL from master space to world space
			rotation = masterBody->current->worldAxis.ToRotation();
			for( i = 0; i < constraints.Num(); i++ )
			{
				if( constraints[i]->GetBody2() == NULL )
				{
					constraints[i]->Rotate( rotation );
					constraints[i]->Translate( masterBody->current->worldOrigin );
				}
			}
			delete masterBody;
			masterBody = NULL;
			Activate();
		}
	}
}


const float	AF_VELOCITY_MAX				= 16000;
const int	AF_VELOCITY_TOTAL_BITS		= 16;
const int	AF_VELOCITY_EXPONENT_BITS	= budMath::BitsForInteger( budMath::BitsForFloat( AF_VELOCITY_MAX ) ) + 1;
const int	AF_VELOCITY_MANTISSA_BITS	= AF_VELOCITY_TOTAL_BITS - 1 - AF_VELOCITY_EXPONENT_BITS;
const float	AF_FORCE_MAX				= 1e20f;
const int	AF_FORCE_TOTAL_BITS			= 16;
const int	AF_FORCE_EXPONENT_BITS		= budMath::BitsForInteger( budMath::BitsForFloat( AF_FORCE_MAX ) ) + 1;
const int	AF_FORCE_MANTISSA_BITS		= AF_FORCE_TOTAL_BITS - 1 - AF_FORCE_EXPONENT_BITS;

/*
================
idPhysics_AF::WriteToSnapshot
================
*/
void idPhysics_AF::WriteToSnapshot( budBitMsg& msg ) const
{
	int i;
	idCQuat quat;
	
	msg.WriteLong( current.atRest );
	msg.WriteFloat( current.noMoveTime );
	msg.WriteFloat( current.activateTime );
	msg.WriteDeltaFloat( 0.0f, current.pushVelocity[0], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	msg.WriteDeltaFloat( 0.0f, current.pushVelocity[1], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	msg.WriteDeltaFloat( 0.0f, current.pushVelocity[2], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	msg.WriteDeltaFloat( 0.0f, current.pushVelocity[3], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	msg.WriteDeltaFloat( 0.0f, current.pushVelocity[4], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	msg.WriteDeltaFloat( 0.0f, current.pushVelocity[5], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	
	msg.WriteByte( bodies.Num() );
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		AFBodyPState_t* state = bodies[i]->current;
		quat = state->worldAxis.ToCQuat();
		
		msg.WriteFloat( state->worldOrigin[0] );
		msg.WriteFloat( state->worldOrigin[1] );
		msg.WriteFloat( state->worldOrigin[2] );
		msg.WriteFloat( quat.x );
		msg.WriteFloat( quat.y );
		msg.WriteFloat( quat.z );
		msg.WriteDeltaFloat( 0.0f, state->spatialVelocity[0], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		msg.WriteDeltaFloat( 0.0f, state->spatialVelocity[1], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		msg.WriteDeltaFloat( 0.0f, state->spatialVelocity[2], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		msg.WriteDeltaFloat( 0.0f, state->spatialVelocity[3], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		msg.WriteDeltaFloat( 0.0f, state->spatialVelocity[4], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		msg.WriteDeltaFloat( 0.0f, state->spatialVelocity[5], AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		/*		msg.WriteDeltaFloat( 0.0f, state->externalForce[0], AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
				msg.WriteDeltaFloat( 0.0f, state->externalForce[1], AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
				msg.WriteDeltaFloat( 0.0f, state->externalForce[2], AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
				msg.WriteDeltaFloat( 0.0f, state->externalForce[3], AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
				msg.WriteDeltaFloat( 0.0f, state->externalForce[4], AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
				msg.WriteDeltaFloat( 0.0f, state->externalForce[5], AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
		*/
	}
}

/*
================
idPhysics_AF::ReadFromSnapshot
================
*/
void idPhysics_AF::ReadFromSnapshot( const budBitMsg& msg )
{
	int i, num;
	idCQuat quat;
	
	current.atRest = msg.ReadLong();
	current.noMoveTime = msg.ReadFloat();
	current.activateTime = msg.ReadFloat();
	current.pushVelocity[0] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	current.pushVelocity[1] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	current.pushVelocity[2] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	current.pushVelocity[3] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	current.pushVelocity[4] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	current.pushVelocity[5] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
	
	num = msg.ReadByte();
	assert( num == bodies.Num() );
	
	for( i = 0; i < bodies.Num(); i++ )
	{
		AFBodyPState_t* state = bodies[i]->current;
		
		state->worldOrigin[0] = msg.ReadFloat();
		state->worldOrigin[1] = msg.ReadFloat();
		state->worldOrigin[2] = msg.ReadFloat();
		quat.x = msg.ReadFloat();
		quat.y = msg.ReadFloat();
		quat.z = msg.ReadFloat();
		state->spatialVelocity[0] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		state->spatialVelocity[1] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		state->spatialVelocity[2] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		state->spatialVelocity[3] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		state->spatialVelocity[4] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		state->spatialVelocity[5] = msg.ReadDeltaFloat( 0.0f, AF_VELOCITY_EXPONENT_BITS, AF_VELOCITY_MANTISSA_BITS );
		/*		state->externalForce[0] = msg.ReadDeltaFloat( 0.0f, AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
				state->externalForce[1] = msg.ReadDeltaFloat( 0.0f, AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
				state->externalForce[2] = msg.ReadDeltaFloat( 0.0f, AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
				state->externalForce[3] = msg.ReadDeltaFloat( 0.0f, AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
				state->externalForce[4] = msg.ReadDeltaFloat( 0.0f, AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
				state->externalForce[5] = msg.ReadDeltaFloat( 0.0f, AF_FORCE_EXPONENT_BITS, AF_FORCE_MANTISSA_BITS );
		*/
		state->worldAxis = quat.ToMat3();
	}
	
	UpdateClipModels();
}
