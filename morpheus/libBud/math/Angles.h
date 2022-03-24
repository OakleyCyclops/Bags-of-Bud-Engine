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

class budVec3;
class idQuat;
class budRotation;
class budMat3;
class budMat4;

class budAngles
{
public:
	float			pitch;
	float			yaw;
	float			roll;
	
	budAngles();
	budAngles( float pitch, float yaw, float roll );
	explicit budAngles( const budVec3& v );
	
	void 			Set( float pitch, float yaw, float roll );
	budAngles& 		Zero();
	
	float			operator[]( int index ) const;
	float& 			operator[]( int index );
	budAngles		operator-() const;			// negate angles, in general not the inverse rotation
	budAngles& 		operator=( const budAngles& a );
	budAngles		operator+( const budAngles& a ) const;
	budAngles& 		operator+=( const budAngles& a );
	budAngles		operator-( const budAngles& a ) const;
	budAngles& 		operator-=( const budAngles& a );
	budAngles		operator*( const float a ) const;
	budAngles& 		operator*=( const float a );
	budAngles		operator/( const float a ) const;
	budAngles& 		operator/=( const float a );
	
	friend budAngles	operator*( const float a, const budAngles& b );
	
	bool			Compare( const budAngles& a ) const;							// exact compare, no epsilon
	bool			Compare( const budAngles& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const budAngles& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const budAngles& a ) const;						// exact compare, no epsilon
	
	budAngles& 		Normalize360();	// normalizes 'this'
	budAngles& 		Normalize180();	// normalizes 'this'
	
	void			Clamp( const budAngles& min, const budAngles& max );
	
	int				GetDimension() const;
	
	void			ToVectors( budVec3* forward, budVec3* right = NULL, budVec3* up = NULL ) const;
	budVec3			ToForward() const;
	idQuat			ToQuat() const;
	budRotation		ToRotation() const;
	budMat3			ToMat3() const;
	budMat4			ToMat4() const;
	budVec3			ToAngularVelocity() const;
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
};

extern budAngles ang_zero;

BUD_INLINE budAngles::budAngles()
{
}

BUD_INLINE budAngles::budAngles( float pitch, float yaw, float roll )
{
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

BUD_INLINE budAngles::budAngles( const budVec3& v )
{
	this->pitch = v[0];
	this->yaw	= v[1];
	this->roll	= v[2];
}

BUD_INLINE void budAngles::Set( float pitch, float yaw, float roll )
{
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

BUD_INLINE budAngles& budAngles::Zero()
{
	pitch = yaw = roll = 0.0f;
	return *this;
}

BUD_INLINE float budAngles::operator[]( int index ) const
{
	assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &pitch )[ index ];
}

BUD_INLINE float& budAngles::operator[]( int index )
{
	assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &pitch )[ index ];
}

BUD_INLINE budAngles budAngles::operator-() const
{
	return budAngles( -pitch, -yaw, -roll );
}

BUD_INLINE budAngles& budAngles::operator=( const budAngles& a )
{
	pitch	= a.pitch;
	yaw		= a.yaw;
	roll	= a.roll;
	return *this;
}

BUD_INLINE budAngles budAngles::operator+( const budAngles& a ) const
{
	return budAngles( pitch + a.pitch, yaw + a.yaw, roll + a.roll );
}

BUD_INLINE budAngles& budAngles::operator+=( const budAngles& a )
{
	pitch	+= a.pitch;
	yaw		+= a.yaw;
	roll	+= a.roll;
	
	return *this;
}

BUD_INLINE budAngles budAngles::operator-( const budAngles& a ) const
{
	return budAngles( pitch - a.pitch, yaw - a.yaw, roll - a.roll );
}

BUD_INLINE budAngles& budAngles::operator-=( const budAngles& a )
{
	pitch	-= a.pitch;
	yaw		-= a.yaw;
	roll	-= a.roll;
	
	return *this;
}

BUD_INLINE budAngles budAngles::operator*( const float a ) const
{
	return budAngles( pitch * a, yaw * a, roll * a );
}

BUD_INLINE budAngles& budAngles::operator*=( float a )
{
	pitch	*= a;
	yaw		*= a;
	roll	*= a;
	return *this;
}

BUD_INLINE budAngles budAngles::operator/( const float a ) const
{
	float inva = 1.0f / a;
	return budAngles( pitch * inva, yaw * inva, roll * inva );
}

BUD_INLINE budAngles& budAngles::operator/=( float a )
{
	float inva = 1.0f / a;
	pitch	*= inva;
	yaw		*= inva;
	roll	*= inva;
	return *this;
}

BUD_INLINE budAngles operator*( const float a, const budAngles& b )
{
	return budAngles( a * b.pitch, a * b.yaw, a * b.roll );
}

BUD_INLINE bool budAngles::Compare( const budAngles& a ) const
{
	return ( ( a.pitch == pitch ) && ( a.yaw == yaw ) && ( a.roll == roll ) );
}

BUD_INLINE bool budAngles::Compare( const budAngles& a, const float epsilon ) const
{
	if( budMath::Fabs( pitch - a.pitch ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( yaw - a.yaw ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( roll - a.roll ) > epsilon )
	{
		return false;
	}
	
	return true;
}

BUD_INLINE bool budAngles::operator==( const budAngles& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budAngles::operator!=( const budAngles& a ) const
{
	return !Compare( a );
}

BUD_INLINE void budAngles::Clamp( const budAngles& min, const budAngles& max )
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

BUD_INLINE int budAngles::GetDimension() const
{
	return 3;
}

BUD_INLINE const float* budAngles::ToFloatPtr() const
{
	return &pitch;
}

BUD_INLINE float* budAngles::ToFloatPtr()
{
	return &pitch;
}

#endif /* !__MATH_ANGLES_H__ */
