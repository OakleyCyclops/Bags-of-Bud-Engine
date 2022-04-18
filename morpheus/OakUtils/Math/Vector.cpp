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
#include "oakUtilsPCH.hpp"

Vector2 Vector2_Origin( 0.0f, 0.0f );
Vector3 Vector3_Origin( 0.0f, 0.0f, 0.0f );
Vector4 Vector4_Origin( 0.0f, 0.0f, 0.0f, 0.0f );
Vector5 Vector5_Origin( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
Vector6 Vector6_Origin( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
Vector6 Vector6_Infinity( Math::INFINITY, Math::INFINITY, Math::INFINITY, Math::INFINITY, Math::INFINITY, Math::INFINITY );


//===============================================================
//
//	Vector2
//
//===============================================================

/*
=============
Vector2::ToString
=============
*/
const char* Vector2::ToString( int precision ) const
{
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
void Vector2::Lerp( const Vector2& v1, const Vector2& v2, const float l )
{
	if( l <= 0.0f )
	{
		( *this ) = v1;
	}
	else if( l >= 1.0f )
	{
		( *this ) = v2;
	}
	else
	{
		( *this ) = v1 + l * ( v2 - v1 );
	}
}


//===============================================================
//
//	Vector3
//
//===============================================================

/*
=============
Vector3::ToYaw
=============
*/
float Vector3::ToYaw() const
{
	float yaw;
	
	if( ( y == 0.0f ) && ( x == 0.0f ) )
	{
		yaw = 0.0f;
	}
	else
	{
		yaw = RAD2DEG( atan2( y, x ) );
		if( yaw < 0.0f )
		{
			yaw += 360.0f;
		}
	}
	
	return yaw;
}

/*
=============
Vector3::ToPitch
=============
*/
float Vector3::ToPitch() const
{
	float	forward;
	float	pitch;
	
	if( ( x == 0.0f ) && ( y == 0.0f ) )
	{
		if( z > 0.0f )
		{
			pitch = 90.0f;
		}
		else
		{
			pitch = 270.0f;
		}
	}
	else
	{
		forward = ( float )Math::Sqrt( x * x + y * y );
		pitch = RAD2DEG( atan2( z, forward ) );
		if( pitch < 0.0f )
		{
			pitch += 360.0f;
		}
	}
	
	return pitch;
}

/*
=============
Vector3::ToAngles
=============
*/
Angles Vector3::ToAngles() const
{
	float forward;
	float yaw;
	float pitch;
	
	if( ( x == 0.0f ) && ( y == 0.0f ) )
	{
		yaw = 0.0f;
		if( z > 0.0f )
		{
			pitch = 90.0f;
		}
		else
		{
			pitch = 270.0f;
		}
	}
	else
	{
		yaw = RAD2DEG( atan2( y, x ) );
		if( yaw < 0.0f )
		{
			yaw += 360.0f;
		}
		
		forward = ( float )Math::Sqrt( x * x + y * y );
		pitch = RAD2DEG( atan2( z, forward ) );
		if( pitch < 0.0f )
		{
			pitch += 360.0f;
		}
	}
	
	return Angles( -pitch, yaw, 0.0f );
}

/*
=============
Vector3::ToPolar
=============
*/
Polar3 Vector3::ToPolar() const
{
	float forward;
	float yaw;
	float pitch;
	
	if( ( x == 0.0f ) && ( y == 0.0f ) )
	{
		yaw = 0.0f;
		if( z > 0.0f )
		{
			pitch = 90.0f;
		}
		else
		{
			pitch = 270.0f;
		}
	}
	else
	{
		yaw = RAD2DEG( atan2( y, x ) );
		if( yaw < 0.0f )
		{
			yaw += 360.0f;
		}
		
		forward = ( float )Math::Sqrt( x * x + y * y );
		pitch = RAD2DEG( atan2( z, forward ) );
		if( pitch < 0.0f )
		{
			pitch += 360.0f;
		}
	}
	return Polar3( Math::Sqrt( x * x + y * y + z * z ), yaw, -pitch );
}

/*
=============
Vector3::ToMat3
=============
*/
Matrix3 Vector3::ToMat3() const
{
	Matrix3	mat;
	float	d;
	
	mat[0] = *this;
	d = x * x + y * y;
	if( !d )
	{
		mat[1][0] = 1.0f;
		mat[1][1] = 0.0f;
		mat[1][2] = 0.0f;
	}
	else
	{
		d = Math::InvSqrt( d );
		mat[1][0] = -y * d;
		mat[1][1] = x * d;
		mat[1][2] = 0.0f;
	}
	mat[2] = Cross( mat[1] );
	
	return mat;
}

/*
=============
Vector3::ToString
=============
*/
const char* Vector3::ToString( int precision ) const
{
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
void Vector3::Lerp( const Vector3& v1, const Vector3& v2, const float l )
{
	if( l <= 0.0f )
	{
		( *this ) = v1;
	}
	else if( l >= 1.0f )
	{
		( *this ) = v2;
	}
	else
	{
		( *this ) = v1 + l * ( v2 - v1 );
	}
}

/*
=============
SLerp

Spherical linear interpolation from v1 to v2.
Vectors are expected to be normalized.
=============
*/
#define LERP_DELTA 1e-6

void Vector3::SLerp( const Vector3& v1, const Vector3& v2, const float t )
{
	float omega, cosom, sinom, scale0, scale1;
	
	if( t <= 0.0f )
	{
		( *this ) = v1;
		return;
	}
	else if( t >= 1.0f )
	{
		( *this ) = v2;
		return;
	}
	
	cosom = v1 * v2;
	if( ( 1.0f - cosom ) > LERP_DELTA )
	{
		omega = acos( cosom );
		sinom = sin( omega );
		scale0 = sin( ( 1.0f - t ) * omega ) / sinom;
		scale1 = sin( t * omega ) / sinom;
	}
	else
	{
		scale0 = 1.0f - t;
		scale1 = t;
	}
	
	( *this ) = ( v1 * scale0 + v2 * scale1 );
}

/*
=============
ProjectSelfOntoSphere

Projects the z component onto a sphere.
=============
*/
void Vector3::ProjectSelfOntoSphere( const float radius )
{
	float rsqr = radius * radius;
	float len = Length();
	if( len  < rsqr * 0.5f )
	{
		z = sqrt( rsqr - len );
	}
	else
	{
		z = rsqr / ( 2.0f * sqrt( len ) );
	}
}



//===============================================================
//
//	Vector4
//
//===============================================================

/*
=============
Vector4::ToString
=============
*/
const char* Vector4::ToString( int precision ) const
{
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

/*
=============
Lerp

Linearly inperpolates one vector to another.
=============
*/
void Vector4::Lerp( const Vector4& v1, const Vector4& v2, const float l )
{
	if( l <= 0.0f )
	{
		( *this ) = v1;
	}
	else if( l >= 1.0f )
	{
		( *this ) = v2;
	}
	else
	{
		( *this ) = v1 + l * ( v2 - v1 );
	}
}


//===============================================================
//
//	Vector5
//
//===============================================================

/*
=============
Vector5::ToString
=============
*/
const char* Vector5::ToString( int precision ) const
{
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}

/*
=============
Vector5::Lerp
=============
*/
void Vector5::Lerp( const Vector5& v1, const Vector5& v2, const float l )
{
	if( l <= 0.0f )
	{
		( *this ) = v1;
	}
	else if( l >= 1.0f )
	{
		( *this ) = v2;
	}
	else
	{
		x = v1.x + l * ( v2.x - v1.x );
		y = v1.y + l * ( v2.y - v1.y );
		z = v1.z + l * ( v2.z - v1.z );
		s = v1.s + l * ( v2.s - v1.s );
		t = v1.t + l * ( v2.t - v1.t );
	}
}


//===============================================================
//
//	Vector6
//
//===============================================================

/*
=============
Vector6::ToString
=============
*/
const char* Vector6::ToString( int precision ) const
{
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}
