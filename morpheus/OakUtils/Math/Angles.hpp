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

#ifndef __MATH_ANGLES_H__
#define __MATH_ANGLES_H__

/*
===============================================================================

	Euler angles

===============================================================================
*/

// angle indexes
#define	PITCH				0		// up / down
#define	YAW					1		// left / right
#define	ROLL				2		// fall over

class Vector3;
class Quat;
class Rotation;
class Matrix3;
class Matrix4;

class Angles
{
public:
	float			pitch;
	float			yaw;
	float			roll;
	
	Angles();
	Angles( float pitch, float yaw, float roll );
	explicit Angles( const Vector3& v );
	
	void 			Set( float pitch, float yaw, float roll );
	Angles& 		Zero();
	
	float			operator[]( int index ) const;
	float& 			operator[]( int index );
	Angles		operator-() const;			// negate angles, in general not the inverse rotation
	Angles& 		operator=( const Angles& a );
	Angles		operator+( const Angles& a ) const;
	Angles& 		operator+=( const Angles& a );
	Angles		operator-( const Angles& a ) const;
	Angles& 		operator-=( const Angles& a );
	Angles		operator*( const float a ) const;
	Angles& 		operator*=( const float a );
	Angles		operator/( const float a ) const;
	Angles& 		operator/=( const float a );
	
	friend Angles	operator*( const float a, const Angles& b );
	
	bool			Compare( const Angles& a ) const;							// exact compare, no epsilon
	bool			Compare( const Angles& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const Angles& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const Angles& a ) const;						// exact compare, no epsilon
	
	Angles& 		Normalize360();	// normalizes 'this'
	Angles& 		Normalize180();	// normalizes 'this'
	
	void			Clamp( const Angles& min, const Angles& max );
	
	int				GetDimension() const;
	
	void			ToVectors( Vector3* forward, Vector3* right = NULL, Vector3* up = NULL ) const;
	Vector3			ToForward() const;
	Quat			ToQuat() const;
	Rotation		ToRotation() const;
	Matrix3			ToMat3() const;
	Matrix4			ToMat4() const;
	Vector3			ToAngularVelocity() const;
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	String			ToString( int precision = 2 ) const;
};

extern Angles ang_zero;

BUD_INLINE Angles::Angles()
{
}

BUD_INLINE Angles::Angles( float pitch, float yaw, float roll )
{
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

BUD_INLINE Angles::Angles( const Vector3& v )
{
	this->pitch = v[0];
	this->yaw	= v[1];
	this->roll	= v[2];
}

BUD_INLINE void Angles::Set( float pitch, float yaw, float roll )
{
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

BUD_INLINE Angles& Angles::Zero()
{
	pitch = yaw = roll = 0.0f;
	return *this;
}

BUD_INLINE float Angles::operator[]( int index ) const
{
	assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &pitch )[ index ];
}

BUD_INLINE float& Angles::operator[]( int index )
{
	assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &pitch )[ index ];
}

BUD_INLINE Angles Angles::operator-() const
{
	return Angles( -pitch, -yaw, -roll );
}

BUD_INLINE Angles& Angles::operator=( const Angles& a )
{
	pitch	= a.pitch;
	yaw		= a.yaw;
	roll	= a.roll;
	return *this;
}

BUD_INLINE Angles Angles::operator+( const Angles& a ) const
{
	return Angles( pitch + a.pitch, yaw + a.yaw, roll + a.roll );
}

BUD_INLINE Angles& Angles::operator+=( const Angles& a )
{
	pitch	+= a.pitch;
	yaw		+= a.yaw;
	roll	+= a.roll;
	
	return *this;
}

BUD_INLINE Angles Angles::operator-( const Angles& a ) const
{
	return Angles( pitch - a.pitch, yaw - a.yaw, roll - a.roll );
}

BUD_INLINE Angles& Angles::operator-=( const Angles& a )
{
	pitch	-= a.pitch;
	yaw		-= a.yaw;
	roll	-= a.roll;
	
	return *this;
}

BUD_INLINE Angles Angles::operator*( const float a ) const
{
	return Angles( pitch * a, yaw * a, roll * a );
}

BUD_INLINE Angles& Angles::operator*=( float a )
{
	pitch	*= a;
	yaw		*= a;
	roll	*= a;
	return *this;
}

BUD_INLINE Angles Angles::operator/( const float a ) const
{
	float inva = 1.0f / a;
	return Angles( pitch * inva, yaw * inva, roll * inva );
}

BUD_INLINE Angles& Angles::operator/=( float a )
{
	float inva = 1.0f / a;
	pitch	*= inva;
	yaw		*= inva;
	roll	*= inva;
	return *this;
}

BUD_INLINE Angles operator*( const float a, const Angles& b )
{
	return Angles( a * b.pitch, a * b.yaw, a * b.roll );
}

BUD_INLINE bool Angles::Compare( const Angles& a ) const
{
	return ( ( a.pitch == pitch ) && ( a.yaw == yaw ) && ( a.roll == roll ) );
}

BUD_INLINE bool Angles::Compare( const Angles& a, const float epsilon ) const
{
	if( Math::Fabs( pitch - a.pitch ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( yaw - a.yaw ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( roll - a.roll ) > epsilon )
	{
		return false;
	}
	
	return true;
}

BUD_INLINE bool Angles::operator==( const Angles& a ) const
{
	return Compare( a );
}

BUD_INLINE bool Angles::operator!=( const Angles& a ) const
{
	return !Compare( a );
}

BUD_INLINE void Angles::Clamp( const Angles& min, const Angles& max )
{
	if( pitch < min.pitch )
	{
		pitch = min.pitch;
	}
	else if( pitch > max.pitch )
	{
		pitch = max.pitch;
	}
	if( yaw < min.yaw )
	{
		yaw = min.yaw;
	}
	else if( yaw > max.yaw )
	{
		yaw = max.yaw;
	}
	if( roll < min.roll )
	{
		roll = min.roll;
	}
	else if( roll > max.roll )
	{
		roll = max.roll;
	}
}

BUD_INLINE int Angles::GetDimension() const
{
	return 3;
}

BUD_INLINE const float* Angles::ToFloatPtr() const
{
	return &pitch;
}

BUD_INLINE float* Angles::ToFloatPtr()
{
	return &pitch;
}

#endif /* !__MATH_ANGLES_H__ */
