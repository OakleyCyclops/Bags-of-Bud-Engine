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


class Angles;
class Quat;
class Matrix3;

class Rotation
{

	friend class Angles;
	friend class Quat;
	friend class Matrix3;
	
public:
	Rotation();
	Rotation( const Vector3& rotationOrigin, const Vector3& rotationVec, const float rotationAngle );
	
	void				Set( const Vector3& rotationOrigin, const Vector3& rotationVec, const float rotationAngle );
	void				SetOrigin( const Vector3& rotationOrigin );
	void				SetVec( const Vector3& rotationVec );					// has to be normalized
	void				SetVec( const float x, const float y, const float z );	// has to be normalized
	void				SetAngle( const float rotationAngle );
	void				Scale( const float s );
	void				ReCalculateMatrix();
	const Vector3& 		GetOrigin() const;
	const Vector3& 		GetVec() const;
	float				GetAngle() const;
	
	Rotation			operator-() const;										// flips rotation
	Rotation			operator*( const float s ) const;						// scale rotation
	Rotation			operator/( const float s ) const;						// scale rotation
	Rotation& 		operator*=( const float s );							// scale rotation
	Rotation& 		operator/=( const float s );							// scale rotation
	Vector3				operator*( const Vector3& v ) const;						// rotate vector
	
	friend Rotation	operator*( const float s, const Rotation& r );		// scale rotation
	friend Vector3		operator*( const Vector3& v, const Rotation& r );		// rotate vector
	friend Vector3& 		operator*=( Vector3& v, const Rotation& r );			// rotate vector
	
	Angles			ToAngles() const;
	Quat				ToQuat() const;
	const Matrix3& 		ToMat3() const;
	Matrix4				ToMat4() const;
	Vector3				ToAngularVelocity() const;
	
	void				RotatePoint( Vector3& point ) const;
	
	void				Normalize180();
	void				Normalize360();
	
private:
	Vector3				origin;			// origin of rotation
	Vector3				vec;			// normalized vector to rotate around
	float				angle;			// angle of rotation in degrees
	mutable Matrix3		axis;			// rotation axis
	mutable bool		axisValid;		// true if rotation axis is valid
};

BUD_INLINE Rotation::Rotation()
{
}

BUD_INLINE Rotation::Rotation( const Vector3& rotationOrigin, const Vector3& rotationVec, const float rotationAngle )
{
	origin = rotationOrigin;
	vec = rotationVec;
	angle = rotationAngle;
	axisValid = false;
}

BUD_INLINE void Rotation::Set( const Vector3& rotationOrigin, const Vector3& rotationVec, const float rotationAngle )
{
	origin = rotationOrigin;
	vec = rotationVec;
	angle = rotationAngle;
	axisValid = false;
}

BUD_INLINE void Rotation::SetOrigin( const Vector3& rotationOrigin )
{
	origin = rotationOrigin;
}

BUD_INLINE void Rotation::SetVec( const Vector3& rotationVec )
{
	vec = rotationVec;
	axisValid = false;
}

BUD_INLINE void Rotation::SetVec( float x, float y, float z )
{
	vec[0] = x;
	vec[1] = y;
	vec[2] = z;
	axisValid = false;
}

BUD_INLINE void Rotation::SetAngle( const float rotationAngle )
{
	angle = rotationAngle;
	axisValid = false;
}

BUD_INLINE void Rotation::Scale( const float s )
{
	angle *= s;
	axisValid = false;
}

BUD_INLINE void Rotation::ReCalculateMatrix()
{
	axisValid = false;
	ToMat3();
}

BUD_INLINE const Vector3& Rotation::GetOrigin() const
{
	return origin;
}

BUD_INLINE const Vector3& Rotation::GetVec() const
{
	return vec;
}

BUD_INLINE float Rotation::GetAngle() const
{
	return angle;
}

BUD_INLINE Rotation Rotation::operator-() const
{
	return Rotation( origin, vec, -angle );
}

BUD_INLINE Rotation Rotation::operator*( const float s ) const
{
	return Rotation( origin, vec, angle * s );
}

BUD_INLINE Rotation Rotation::operator/( const float s ) const
{
	assert( s != 0.0f );
	return Rotation( origin, vec, angle / s );
}

BUD_INLINE Rotation& Rotation::operator*=( const float s )
{
	angle *= s;
	axisValid = false;
	return *this;
}

BUD_INLINE Rotation& Rotation::operator/=( const float s )
{
	assert( s != 0.0f );
	angle /= s;
	axisValid = false;
	return *this;
}

BUD_INLINE Vector3 Rotation::operator*( const Vector3& v ) const
{
	if( !axisValid )
	{
		ToMat3();
	}
	return ( ( v - origin ) * axis + origin );
}

BUD_INLINE Rotation operator*( const float s, const Rotation& r )
{
	return r * s;
}

BUD_INLINE Vector3 operator*( const Vector3& v, const Rotation& r )
{
	return r * v;
}

BUD_INLINE Vector3& operator*=( Vector3& v, const Rotation& r )
{
	v = r * v;
	return v;
}

BUD_INLINE void Rotation::RotatePoint( Vector3& point ) const
{
	if( !axisValid )
	{
		ToMat3();
	}
	point = ( ( point - origin ) * axis + origin );
}

#endif /* !__MATH_ROTATION_H__ */
