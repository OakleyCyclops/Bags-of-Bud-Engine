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

#include <float.h>

budAngles ang_zero( 0.0f, 0.0f, 0.0f );


/*
=================
budAngles::Normalize360

returns angles normalized to the range [0 <= angle < 360]
=================
*/
budAngles& budAngles::Normalize360()
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
budAngles::Normalize180

returns angles normalized to the range [-180 < angle <= 180]
=================
*/
budAngles& budAngles::Normalize180()
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
budAngles::ToVectors
=================
*/
void budAngles::ToVectors( budVec3* forward, budVec3* right, budVec3* up ) const
{
	float sr, sp, sy, cr, cp, cy;
	
	budMath::SinCos( DEG2RAD( yaw ), sy, cy );
	budMath::SinCos( DEG2RAD( pitch ), sp, cp );
	budMath::SinCos( DEG2RAD( roll ), sr, cr );
	
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
budAngles::ToForward
=================
*/
budVec3 budAngles::ToForward() const
{
	float sp, sy, cp, cy;
	
	budMath::SinCos( DEG2RAD( yaw ), sy, cy );
	budMath::SinCos( DEG2RAD( pitch ), sp, cp );
	
	return budVec3( cp * cy, cp * sy, -sp );
}

/*
=================
budAngles::ToQuat
=================
*/
idQuat budAngles::ToQuat() const
{
	float sx, cx, sy, cy, sz, cz;
	float sxcy, cxcy, sxsy, cxsy;
	
	budMath::SinCos( DEG2RAD( yaw ) * 0.5f, sz, cz );
	budMath::SinCos( DEG2RAD( pitch ) * 0.5f, sy, cy );
	budMath::SinCos( DEG2RAD( roll ) * 0.5f, sx, cx );
	
	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;
	
	return idQuat( cxsy * sz - sxcy * cz, -cxsy * cz - sxcy * sz, sxsy * cz - cxcy * sz, cxcy * cz + sxsy * sz );
}

/*
=================
budAngles::ToRotation
=================
*/
budRotation budAngles::ToRotation() const
{
	budVec3 vec;
	float angle, w;
	float sx, cx, sy, cy, sz, cz;
	float sxcy, cxcy, sxsy, cxsy;
	
	if( pitch == 0.0f )
	{
		if( yaw == 0.0f )
		{
			return budRotation( vec3_origin, budVec3( -1.0f, 0.0f, 0.0f ), roll );
		}
		if( roll == 0.0f )
		{
			return budRotation( vec3_origin, budVec3( 0.0f, 0.0f, -1.0f ), yaw );
		}
	}
	else if( yaw == 0.0f && roll == 0.0f )
	{
		return budRotation( vec3_origin, budVec3( 0.0f, -1.0f, 0.0f ), pitch );
	}
	
	budMath::SinCos( DEG2RAD( yaw ) * 0.5f, sz, cz );
	budMath::SinCos( DEG2RAD( pitch ) * 0.5f, sy, cy );
	budMath::SinCos( DEG2RAD( roll ) * 0.5f, sx, cx );
	
	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;
	
	vec.x =  cxsy * sz - sxcy * cz;
	vec.y = -cxsy * cz - sxcy * sz;
	vec.z =  sxsy * cz - cxcy * sz;
	w =		 cxcy * cz + sxsy * sz;
	angle = budMath::ACos( w );
	if( angle == 0.0f )
	{
		vec.Set( 0.0f, 0.0f, 1.0f );
	}
	else
	{
		//vec *= (1.0f / sin( angle ));
		vec.Normalize();
		vec.FixDegenerateNormal();
		angle *= 2.0f * budMath::M_RAD2DEG;
	}
	return budRotation( vec3_origin, vec, angle );
}

/*
=================
budAngles::ToMat3
=================
*/
budMat3 budAngles::ToMat3() const
{
	budMat3 mat;
	float sr, sp, sy, cr, cp, cy;
	
	budMath::SinCos( DEG2RAD( yaw ), sy, cy );
	budMath::SinCos( DEG2RAD( pitch ), sp, cp );
	budMath::SinCos( DEG2RAD( roll ), sr, cr );
	
	mat[ 0 ].Set( cp * cy, cp * sy, -sp );
	mat[ 1 ].Set( sr * sp * cy + cr * -sy, sr * sp * sy + cr * cy, sr * cp );
	mat[ 2 ].Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );
	
	return mat;
}

/*
=================
budAngles::ToMat4
=================
*/
budMat4 budAngles::ToMat4() const
{
	return ToMat3().ToMat4();
}

/*
=================
budAngles::ToAngularVelocity
=================
*/
budVec3 budAngles::ToAngularVelocity() const
{
	budRotation rotation = budAngles::ToRotation();
	return rotation.GetVec() * DEG2RAD( rotation.GetAngle() );
}

/*
=============
budAngles::ToString
=============
*/
const char* budAngles::ToString( int precision ) const
{
	return budStr::FloatArrayToString( ToFloatPtr(), GetDimension(), precision );
}
