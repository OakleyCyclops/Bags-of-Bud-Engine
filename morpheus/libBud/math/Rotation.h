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

#ifndef __MATH_ROTATION_H__
#define __MATH_ROTATION_H__

/*
===============================================================================

	Describes a complete rotation in degrees about an abritray axis.
	A local rotation matrix is stored for fast rotation of multiple points.

===============================================================================
*/


class budAngles;
class idQuat;
class budMat3;

class budRotation
{

	friend class budAngles;
	friend class idQuat;
	friend class budMat3;
	
public:
	budRotation();
	budRotation( const budVec3& rotationOrigin, const budVec3& rotationVec, const float rotationAngle );
	
	void				Set( const budVec3& rotationOrigin, const budVec3& rotationVec, const float rotationAngle );
	void				SetOrigin( const budVec3& rotationOrigin );
	void				SetVec( const budVec3& rotationVec );					// has to be normalized
	void				SetVec( const float x, const float y, const float z );	// has to be normalized
	void				SetAngle( const float rotationAngle );
	void				Scale( const float s );
	void				ReCalculateMatrix();
	const budVec3& 		GetOrigin() const;
	const budVec3& 		GetVec() const;
	float				GetAngle() const;
	
	budRotation			operator-() const;										// flips rotation
	budRotation			operator*( const float s ) const;						// scale rotation
	budRotation			operator/( const float s ) const;						// scale rotation
	budRotation& 		operator*=( const float s );							// scale rotation
	budRotation& 		operator/=( const float s );							// scale rotation
	budVec3				operator*( const budVec3& v ) const;						// rotate vector
	
	friend budRotation	operator*( const float s, const budRotation& r );		// scale rotation
	friend budVec3		operator*( const budVec3& v, const budRotation& r );		// rotate vector
	friend budVec3& 		operator*=( budVec3& v, const budRotation& r );			// rotate vector
	
	budAngles			ToAngles() const;
	idQuat				ToQuat() const;
	const budMat3& 		ToMat3() const;
	budMat4				ToMat4() const;
	budVec3				ToAngularVelocity() const;
	
	void				RotatePoint( budVec3& point ) const;
	
	void				Normalize180();
	void				Normalize360();
	
private:
	budVec3				origin;			// origin of rotation
	budVec3				vec;			// normalized vector to rotate around
	float				angle;			// angle of rotation in degrees
	mutable budMat3		axis;			// rotation axis
	mutable bool		axisValid;		// true if rotation axis is valid
};


BUD_INLINE budRotation::budRotation()
{
}

BUD_INLINE budRotation::budRotation( const budVec3& rotationOrigin, const budVec3& rotationVec, const float rotationAngle )
{
	origin = rotationOrigin;
	vec = rotationVec;
	angle = rotationAngle;
	axisValid = false;
}

BUD_INLINE void budRotation::Set( const budVec3& rotationOrigin, const budVec3& rotationVec, const float rotationAngle )
{
	origin = rotationOrigin;
	vec = rotationVec;
	angle = rotationAngle;
	axisValid = false;
}

BUD_INLINE void budRotation::SetOrigin( const budVec3& rotationOrigin )
{
	origin = rotationOrigin;
}

BUD_INLINE void budRotation::SetVec( const budVec3& rotationVec )
{
	vec = rotationVec;
	axisValid = false;
}

BUD_INLINE void budRotation::SetVec( float x, float y, float z )
{
	vec[0] = x;
	vec[1] = y;
	vec[2] = z;
	axisValid = false;
}

BUD_INLINE void budRotation::SetAngle( const float rotationAngle )
{
	angle = rotationAngle;
	axisValid = false;
}

BUD_INLINE void budRotation::Scale( const float s )
{
	angle *= s;
	axisValid = false;
}

BUD_INLINE void budRotation::ReCalculateMatrix()
{
	axisValid = false;
	ToMat3();
}

BUD_INLINE const budVec3& budRotation::GetOrigin() const
{
	return origin;
}

BUD_INLINE const budVec3& budRotation::GetVec() const
{
	return vec;
}

BUD_INLINE float budRotation::GetAngle() const
{
	return angle;
}

BUD_INLINE budRotation budRotation::operator-() const
{
	return budRotation( origin, vec, -angle );
}

BUD_INLINE budRotation budRotation::operator*( const float s ) const
{
	return budRotation( origin, vec, angle * s );
}

BUD_INLINE budRotation budRotation::operator/( const float s ) const
{
	assert( s != 0.0f );
	return budRotation( origin, vec, angle / s );
}

BUD_INLINE budRotation& budRotation::operator*=( const float s )
{
	angle *= s;
	axisValid = false;
	return *this;
}

BUD_INLINE budRotation& budRotation::operator/=( const float s )
{
	assert( s != 0.0f );
	angle /= s;
	axisValid = false;
	return *this;
}

BUD_INLINE budVec3 budRotation::operator*( const budVec3& v ) const
{
	if( !axisValid )
	{
		ToMat3();
	}
	return ( ( v - origin ) * axis + origin );
}

BUD_INLINE budRotation operator*( const float s, const budRotation& r )
{
	return r * s;
}

BUD_INLINE budVec3 operator*( const budVec3& v, const budRotation& r )
{
	return r * v;
}

BUD_INLINE budVec3& operator*=( budVec3& v, const budRotation& r )
{
	v = r * v;
	return v;
}

BUD_INLINE void budRotation::RotatePoint( budVec3& point ) const
{
	if( !axisValid )
	{
		ToMat3();
	}
	point = ( ( point - origin ) * axis + origin );
}

#endif /* !__MATH_ROTATION_H__ */
