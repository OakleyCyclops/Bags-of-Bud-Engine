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

#include <float.h>

Angles ang_zero( 0.0f, 0.0f, 0.0f );


/*
=================
Angles::Normalize360

returns angles normalized to the range [0 <= angle < 360]
=================
*/
Angles& Angles::Normalize360()
{
	int i;
	
	for( i = 0; i < 3; i++ )
	{
		if( ( ( *this )[i] >= 360.0f ) || ( ( *this )[i] < 0.0f ) )
		{
			( *this )[i] -= floor( ( *this )[i] / 360.0f ) * 360.0f;
			
			if( ( *this )[i] >= 360.0f )
			{
				( *this )[i] -= 360.0f;
			}
			if( ( *this )[i] < 0.0f )
			{
				( *this )[i] += 360.0f;
			}
		}
	}
	
	return *this;
}

/*
=================
Angles::Normalize180

returns angles normalized to the range [-180 < angle <= 180]
=================
*/
Angles& Angles::Normalize180()
{
	Normalize360();
	
	if( pitch > 180.0f )
	{
		pitch -= 360.0f;
	}
	
	if( yaw > 180.0f )
	{
		yaw -= 360.0f;
	}
	
	if( roll > 180.0f )
	{
		roll -= 360.0f;
	}
	return *this;
}

/*
=================
Angles::ToVectors
=================
*/
void Angles::ToVectors( Vector3* forward, Vector3* right, Vector3* up ) const
{
	float sr, sp, sy, cr, cp, cy;
	
	Math::SinCos( DEG2RAD( yaw ), sy, cy );
	Math::SinCos( DEG2RAD( pitch ), sp, cp );
	Math::SinCos( DEG2RAD( roll ), sr, cr );
	
	if( forward )
	{
		forward->Set( cp * cy, cp * sy, -sp );
	}
	
	if( right )
	{
		right->Set( -sr * sp * cy + cr * sy, -sr * sp * sy + -cr * cy, -sr * cp );
	}
	
	if( up )
	{
		up->Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );
	}
}

/*
=================
Angles::ToForward
=================
*/
Vector3 Angles::ToForward() const
{
	float sp, sy, cp, cy;
	
	Math::SinCos( DEG2RAD( yaw ), sy, cy );
	Math::SinCos( DEG2RAD( pitch ), sp, cp );
	
	return Vector3( cp * cy, cp * sy, -sp );
}

/*
=================
Angles::ToQuat
=================
*/
Quat Angles::ToQuat() const
{
	float sx, cx, sy, cy, sz, cz;
	float sxcy, cxcy, sxsy, cxsy;
	
	Math::SinCos( DEG2RAD( yaw ) * 0.5f, sz, cz );
	Math::SinCos( DEG2RAD( pitch ) * 0.5f, sy, cy );
	Math::SinCos( DEG2RAD( roll ) * 0.5f, sx, cx );
	
	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;
	
	return Quat( cxsy * sz - sxcy * cz, -cxsy * cz - sxcy * sz, sxsy * cz - cxcy * sz, cxcy * cz + sxsy * sz );
}

/*
=================
Angles::ToRotation
=================
*/
Rotation Angles::ToRotation() const
{
	Vector3 vec;
	float angle, w;
	float sx, cx, sy, cy, sz, cz;
	float sxcy, cxcy, sxsy, cxsy;
	
	if( pitch == 0.0f )
	{
		if( yaw == 0.0f )
		{
			return Rotation( Vector3_Origin, Vector3( -1.0f, 0.0f, 0.0f ), roll );
		}
		if( roll == 0.0f )
		{
			return Rotation( Vector3_Origin, Vector3( 0.0f, 0.0f, -1.0f ), yaw );
		}
	}
	else if( yaw == 0.0f && roll == 0.0f )
	{
		return Rotation( Vector3_Origin, Vector3( 0.0f, -1.0f, 0.0f ), pitch );
	}
	
	Math::SinCos( DEG2RAD( yaw ) * 0.5f, sz, cz );
	Math::SinCos( DEG2RAD( pitch ) * 0.5f, sy, cy );
	Math::SinCos( DEG2RAD( roll ) * 0.5f, sx, cx );
	
	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;
	
	vec.x =  cxsy * sz - sxcy * cz;
	vec.y = -cxsy * cz - sxcy * sz;
	vec.z =  sxsy * cz - cxcy * sz;
	w =		 cxcy * cz + sxsy * sz;
	angle = Math::ACos( w );
	if( angle == 0.0f )
	{
		vec.Set( 0.0f, 0.0f, 1.0f );
	}
	else
	{
		//vec *= (1.0f / sin( angle ));
		vec.Normalize();
		vec.FixDegenerateNormal();
		angle *= 2.0f * Math::M_RAD2DEG;
	}
	return Rotation( Vector3_Origin, vec, angle );
}

/*
=================
Angles::ToMat3
=================
*/
Matrix3 Angles::ToMat3() const
{
	Matrix3 mat;
	float sr, sp, sy, cr, cp, cy;
	
	Math::SinCos( DEG2RAD( yaw ), sy, cy );
	Math::SinCos( DEG2RAD( pitch ), sp, cp );
	Math::SinCos( DEG2RAD( roll ), sr, cr );
	
	mat[ 0 ].Set( cp * cy, cp * sy, -sp );
	mat[ 1 ].Set( sr * sp * cy + cr * -sy, sr * sp * sy + cr * cy, sr * cp );
	mat[ 2 ].Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );
	
	return mat;
}

/*
=================
Angles::ToMat4
=================
*/
Matrix4 Angles::ToMat4() const
{
	return ToMat3().ToMat4();
}

/*
=================
Angles::ToAngularVelocity
=================
*/
Vector3 Angles::ToAngularVelocity() const
{
	Rotation rotation = Angles::ToRotation();
	return rotation.GetVec() * DEG2RAD( rotation.GetAngle() );
}

/*
=============
Angles::ToString
=============
*/
const char* Angles::ToString( int precision ) const
{
	return String::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}
