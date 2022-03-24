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

#ifndef __MATH_VECTOR_H__
#define __MATH_VECTOR_H__

/*
===============================================================================

  Vector classes

===============================================================================
*/

#include "../containers/Array.h" // for idTupleSize

#define VECTOR_EPSILON		0.001f

class budAngles;
class idPolar3;
class budMat3;

//===============================================================
//
//	budVec2 - 2D vector
//
//===============================================================

class budVec2
{
public:
	float			x;
	float			y;
	
	budVec2();
	explicit budVec2( const float x, const float y );
	
	void 			Set( const float x, const float y );
	void			Zero();
	
	float			operator[]( int index ) const;
	float& 			operator[]( int index );
	budVec2			operator-() const;
	float			operator*( const budVec2& a ) const;
	budVec2			operator*( const float a ) const;
	budVec2			operator/( const float a ) const;
	budVec2			operator+( const budVec2& a ) const;
	budVec2			operator-( const budVec2& a ) const;
	budVec2& 		operator+=( const budVec2& a );
	budVec2& 		operator-=( const budVec2& a );
	budVec2& 		operator/=( const budVec2& a );
	budVec2& 		operator/=( const float a );
	budVec2& 		operator*=( const float a );
	
	friend budVec2	operator*( const float a, const budVec2 b );
	
	budVec2			Scale( const budVec2& a ) const;
	
	bool			Compare( const budVec2& a ) const;							// exact compare, no epsilon
	bool			Compare( const budVec2& a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const budVec2& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const budVec2& a ) const;						// exact compare, no epsilon
	
	float			Length() const;
	float			LengthFast() const;
	float			LengthSqr() const;
	float			Normalize();			// returns length
	float			NormalizeFast();		// returns length
	budVec2			Truncate( float length ) const;	// cap length
	void			Clamp( const budVec2& min, const budVec2& max );
	void			Snap();				// snap to closest integer value
	void			SnapInt();			// snap towards integer (floor)
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
	void			Lerp( const budVec2& v1, const budVec2& v2, const float l );
};

extern budVec2 vec2_origin;
#define vec2_zero vec2_origin

BUD_INLINE budVec2::budVec2()
{
}

BUD_INLINE budVec2::budVec2( const float x, const float y )
{
	this->x = x;
	this->y = y;
}

BUD_INLINE void budVec2::Set( const float x, const float y )
{
	this->x = x;
	this->y = y;
}

BUD_INLINE void budVec2::Zero()
{
	x = y = 0.0f;
}

BUD_INLINE bool budVec2::Compare( const budVec2& a ) const
{
	return ( ( x == a.x ) && ( y == a.y ) );
}

BUD_INLINE bool budVec2::Compare( const budVec2& a, const float epsilon ) const
{
	if( budMath::Fabs( x - a.x ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( y - a.y ) > epsilon )
	{
		return false;
	}
	
	return true;
}

BUD_INLINE bool budVec2::operator==( const budVec2& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budVec2::operator!=( const budVec2& a ) const
{
	return !Compare( a );
}

BUD_INLINE float budVec2::operator[]( int index ) const
{
	return ( &x )[ index ];
}

BUD_INLINE float& budVec2::operator[]( int index )
{
	return ( &x )[ index ];
}

BUD_INLINE float budVec2::Length() const
{
	return ( float )budMath::Sqrt( x * x + y * y );
}

BUD_INLINE float budVec2::LengthFast() const
{
	float sqrLength;
	
	sqrLength = x * x + y * y;
	return sqrLength * budMath::InvSqrt( sqrLength );
}

BUD_INLINE float budVec2::LengthSqr() const
{
	return ( x * x + y * y );
}

BUD_INLINE float budVec2::Normalize()
{
	float sqrLength, invLength;
	
	sqrLength = x * x + y * y;
	invLength = budMath::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE float budVec2::NormalizeFast()
{
	float lengthSqr, invLength;
	
	lengthSqr = x * x + y * y;
	invLength = budMath::InvSqrt( lengthSqr );
	x *= invLength;
	y *= invLength;
	return invLength * lengthSqr;
}

BUD_INLINE budVec2 budVec2::Truncate( float length ) const
{
	if( length < budMath::FLT_SMALLEST_NON_DENORMAL )
	{
		return vec2_zero;
	}
	else
	{
		float length2 = LengthSqr();
		if( length2 > length * length )
		{
			float ilength = length * budMath::InvSqrt( length2 );
			return *this * ilength;
		}
	}
	return *this;
}

BUD_INLINE void budVec2::Clamp( const budVec2& min, const budVec2& max )
{
	if( x < min.x )
	{
		x = min.x;
	}
	else if( x > max.x )
	{
		x = max.x;
	}
	if( y < min.y )
	{
		y = min.y;
	}
	else if( y > max.y )
	{
		y = max.y;
	}
}

BUD_INLINE void budVec2::Snap()
{
	x = floor( x + 0.5f );
	y = floor( y + 0.5f );
}

BUD_INLINE void budVec2::SnapInt()
{
	x = float( int( x ) );
	y = float( int( y ) );
}

BUD_INLINE budVec2 budVec2::operator-() const
{
	return budVec2( -x, -y );
}

BUD_INLINE budVec2 budVec2::operator-( const budVec2& a ) const
{
	return budVec2( x - a.x, y - a.y );
}

BUD_INLINE float budVec2::operator*( const budVec2& a ) const
{
	return x * a.x + y * a.y;
}

BUD_INLINE budVec2 budVec2::operator*( const float a ) const
{
	return budVec2( x * a, y * a );
}

BUD_INLINE budVec2 budVec2::operator/( const float a ) const
{
	float inva = 1.0f / a;
	return budVec2( x * inva, y * inva );
}

BUD_INLINE budVec2 operator*( const float a, const budVec2 b )
{
	return budVec2( b.x * a, b.y * a );
}

BUD_INLINE budVec2 budVec2::operator+( const budVec2& a ) const
{
	return budVec2( x + a.x, y + a.y );
}

BUD_INLINE budVec2& budVec2::operator+=( const budVec2& a )
{
	x += a.x;
	y += a.y;
	
	return *this;
}

BUD_INLINE budVec2& budVec2::operator/=( const budVec2& a )
{
	x /= a.x;
	y /= a.y;
	
	return *this;
}

BUD_INLINE budVec2& budVec2::operator/=( const float a )
{
	float inva = 1.0f / a;
	x *= inva;
	y *= inva;
	
	return *this;
}

BUD_INLINE budVec2& budVec2::operator-=( const budVec2& a )
{
	x -= a.x;
	y -= a.y;
	
	return *this;
}

BUD_INLINE budVec2& budVec2::operator*=( const float a )
{
	x *= a;
	y *= a;
	
	return *this;
}

BUD_INLINE budVec2 budVec2::Scale( const budVec2& a ) const
{
	return budVec2( x * a.x, y * a.y );
}

BUD_INLINE int budVec2::GetDimension() const
{
	return 2;
}

BUD_INLINE const float* budVec2::ToFloatPtr() const
{
	return &x;
}

BUD_INLINE float* budVec2::ToFloatPtr()
{
	return &x;
}


//===============================================================
//
//	budVec3 - 3D vector
//
//===============================================================

class budVec3
{
public:
	float			x;
	float			y;
	float			z;
	
	budVec3();
	explicit budVec3( const float xyz )
	{
		Set( xyz, xyz, xyz );
	}
	explicit budVec3( const float x, const float y, const float z );
	
	void 			Set( const float x, const float y, const float z );
	void			Zero();
	
	float			operator[]( const int index ) const;
	float& 			operator[]( const int index );
	budVec3			operator-() const;
	budVec3& 		operator=( const budVec3& a );		// required because of a msvc 6 & 7 bug
	float			operator*( const budVec3& a ) const;
	budVec3			operator*( const float a ) const;
	budVec3			operator/( const float a ) const;
	budVec3			operator+( const budVec3& a ) const;
	budVec3			operator-( const budVec3& a ) const;
	budVec3& 		operator+=( const budVec3& a );
	budVec3& 		operator-=( const budVec3& a );
	budVec3& 		operator/=( const budVec3& a );
	budVec3& 		operator/=( const float a );
	budVec3& 		operator*=( const float a );
	
	friend budVec3	operator*( const float a, const budVec3 b );
	
	bool			Compare( const budVec3& a ) const;							// exact compare, no epsilon
	bool			Compare( const budVec3& a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const budVec3& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const budVec3& a ) const;						// exact compare, no epsilon
	
	bool			FixDegenerateNormal();	// fix degenerate axial cases
	bool			FixDenormals();			// change tiny numbers to zero
	
	budVec3			Cross( const budVec3& a ) const;
	budVec3& 		Cross( const budVec3& a, const budVec3& b );
	float			Length() const;
	float			LengthSqr() const;
	float			LengthFast() const;
	float			Normalize();				// returns length
	float			NormalizeFast();			// returns length
	budVec3			Truncate( float length ) const;		// cap length
	void			Clamp( const budVec3& min, const budVec3& max );
	void			Snap();					// snap to closest integer value
	void			SnapInt();				// snap towards integer (floor)
	
	int				GetDimension() const;
	
	float			ToYaw() const;
	float			ToPitch() const;
	budAngles		ToAngles() const;
	idPolar3		ToPolar() const;
	budMat3			ToMat3() const;		// vector should be normalized
	const budVec2& 	ToVec2() const;
	budVec2& 		ToVec2();
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
	void			NormalVectors( budVec3& left, budVec3& down ) const;	// vector should be normalized
	void			OrthogonalBasis( budVec3& left, budVec3& up ) const;
	
	void			ProjectOntoPlane( const budVec3& normal, const float overBounce = 1.0f );
	bool			ProjectAlongPlane( const budVec3& normal, const float epsilon, const float overBounce = 1.0f );
	void			ProjectSelfOntoSphere( const float radius );
	
	void			Lerp( const budVec3& v1, const budVec3& v2, const float l );
	void			SLerp( const budVec3& v1, const budVec3& v2, const float l );
};

extern budVec3 vec3_origin;
#define vec3_zero vec3_origin

BUD_INLINE budVec3::budVec3()
{
}

BUD_INLINE budVec3::budVec3( const float x, const float y, const float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

BUD_INLINE float budVec3::operator[]( const int index ) const
{
	return ( &x )[ index ];
}

BUD_INLINE float& budVec3::operator[]( const int index )
{
	return ( &x )[ index ];
}

BUD_INLINE void budVec3::Set( const float x, const float y, const float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

BUD_INLINE void budVec3::Zero()
{
	x = y = z = 0.0f;
}

BUD_INLINE budVec3 budVec3::operator-() const
{
	return budVec3( -x, -y, -z );
}

BUD_INLINE budVec3& budVec3::operator=( const budVec3& a )
{
	x = a.x;
	y = a.y;
	z = a.z;
	return *this;
}

BUD_INLINE budVec3 budVec3::operator-( const budVec3& a ) const
{
	return budVec3( x - a.x, y - a.y, z - a.z );
}

BUD_INLINE float budVec3::operator*( const budVec3& a ) const
{
	return x * a.x + y * a.y + z * a.z;
}

BUD_INLINE budVec3 budVec3::operator*( const float a ) const
{
	return budVec3( x * a, y * a, z * a );
}

BUD_INLINE budVec3 budVec3::operator/( const float a ) const
{
	float inva = 1.0f / a;
	return budVec3( x * inva, y * inva, z * inva );
}

BUD_INLINE budVec3 operator*( const float a, const budVec3 b )
{
	return budVec3( b.x * a, b.y * a, b.z * a );
}

BUD_INLINE budVec3 operator/( const float a, const budVec3 b )
{
	return budVec3( a / b.x, a / b.y, a / b.z );
}

BUD_INLINE budVec3 budVec3::operator+( const budVec3& a ) const
{
	return budVec3( x + a.x, y + a.y, z + a.z );
}

BUD_INLINE budVec3& budVec3::operator+=( const budVec3& a )
{
	x += a.x;
	y += a.y;
	z += a.z;
	
	return *this;
}

BUD_INLINE budVec3& budVec3::operator/=( const budVec3& a )
{
	x /= a.x;
	y /= a.y;
	z /= a.z;
	
	return *this;
}

BUD_INLINE budVec3& budVec3::operator/=( const float a )
{
	float inva = 1.0f / a;
	x *= inva;
	y *= inva;
	z *= inva;
	
	return *this;
}

BUD_INLINE budVec3& budVec3::operator-=( const budVec3& a )
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	
	return *this;
}

BUD_INLINE budVec3& budVec3::operator*=( const float a )
{
	x *= a;
	y *= a;
	z *= a;
	
	return *this;
}

BUD_INLINE bool budVec3::Compare( const budVec3& a ) const
{
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) );
}

BUD_INLINE bool budVec3::Compare( const budVec3& a, const float epsilon ) const
{
	if( budMath::Fabs( x - a.x ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( y - a.y ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( z - a.z ) > epsilon )
	{
		return false;
	}
	
	return true;
}

BUD_INLINE bool budVec3::operator==( const budVec3& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budVec3::operator!=( const budVec3& a ) const
{
	return !Compare( a );
}

BUD_INLINE float budVec3::NormalizeFast()
{
	float sqrLength, invLength;
	
	sqrLength = x * x + y * y + z * z;
	invLength = budMath::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE bool budVec3::FixDegenerateNormal()
{
	if( x == 0.0f )
	{
		if( y == 0.0f )
		{
			if( z > 0.0f )
			{
				if( z != 1.0f )
				{
					z = 1.0f;
					return true;
				}
			}
			else
			{
				if( z != -1.0f )
				{
					z = -1.0f;
					return true;
				}
			}
			return false;
		}
		else if( z == 0.0f )
		{
			if( y > 0.0f )
			{
				if( y != 1.0f )
				{
					y = 1.0f;
					return true;
				}
			}
			else
			{
				if( y != -1.0f )
				{
					y = -1.0f;
					return true;
				}
			}
			return false;
		}
	}
	else if( y == 0.0f )
	{
		if( z == 0.0f )
		{
			if( x > 0.0f )
			{
				if( x != 1.0f )
				{
					x = 1.0f;
					return true;
				}
			}
			else
			{
				if( x != -1.0f )
				{
					x = -1.0f;
					return true;
				}
			}
			return false;
		}
	}
	if( budMath::Fabs( x ) == 1.0f )
	{
		if( y != 0.0f || z != 0.0f )
		{
			y = z = 0.0f;
			return true;
		}
		return false;
	}
	else if( budMath::Fabs( y ) == 1.0f )
	{
		if( x != 0.0f || z != 0.0f )
		{
			x = z = 0.0f;
			return true;
		}
		return false;
	}
	else if( budMath::Fabs( z ) == 1.0f )
	{
		if( x != 0.0f || y != 0.0f )
		{
			x = y = 0.0f;
			return true;
		}
		return false;
	}
	return false;
}

BUD_INLINE bool budVec3::FixDenormals()
{
	bool denormal = false;
	if( fabs( x ) < 1e-30f )
	{
		x = 0.0f;
		denormal = true;
	}
	if( fabs( y ) < 1e-30f )
	{
		y = 0.0f;
		denormal = true;
	}
	if( fabs( z ) < 1e-30f )
	{
		z = 0.0f;
		denormal = true;
	}
	return denormal;
}

BUD_INLINE budVec3 budVec3::Cross( const budVec3& a ) const
{
	return budVec3( y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x );
}

BUD_INLINE budVec3& budVec3::Cross( const budVec3& a, const budVec3& b )
{
	x = a.y * b.z - a.z * b.y;
	y = a.z * b.x - a.x * b.z;
	z = a.x * b.y - a.y * b.x;
	
	return *this;
}

BUD_INLINE float budVec3::Length() const
{
	return ( float )budMath::Sqrt( x * x + y * y + z * z );
}

BUD_INLINE float budVec3::LengthSqr() const
{
	return ( x * x + y * y + z * z );
}

BUD_INLINE float budVec3::LengthFast() const
{
	float sqrLength;
	
	sqrLength = x * x + y * y + z * z;
	return sqrLength * budMath::InvSqrt( sqrLength );
}

BUD_INLINE float budVec3::Normalize()
{
	float sqrLength, invLength;
	
	sqrLength = x * x + y * y + z * z;
	invLength = budMath::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE budVec3 budVec3::Truncate( float length ) const
{
	if( length < budMath::FLT_SMALLEST_NON_DENORMAL )
	{
		return vec3_zero;
	}
	else
	{
		float length2 = LengthSqr();
		if( length2 > length * length )
		{
			float ilength = length * budMath::InvSqrt( length2 );
			return *this * ilength;
		}
	}
	return *this;
}

BUD_INLINE void budVec3::Clamp( const budVec3& min, const budVec3& max )
{
	if( x < min.x )
	{
		x = min.x;
	}
	else if( x > max.x )
	{
		x = max.x;
	}
	if( y < min.y )
	{
		y = min.y;
	}
	else if( y > max.y )
	{
		y = max.y;
	}
	if( z < min.z )
	{
		z = min.z;
	}
	else if( z > max.z )
	{
		z = max.z;
	}
}

BUD_INLINE void budVec3::Snap()
{
	x = floor( x + 0.5f );
	y = floor( y + 0.5f );
	z = floor( z + 0.5f );
}

BUD_INLINE void budVec3::SnapInt()
{
	x = float( int( x ) );
	y = float( int( y ) );
	z = float( int( z ) );
}

BUD_INLINE int budVec3::GetDimension() const
{
	return 3;
}

BUD_INLINE const budVec2& budVec3::ToVec2() const
{
	return *reinterpret_cast<const budVec2*>( this );
}

BUD_INLINE budVec2& budVec3::ToVec2()
{
	return *reinterpret_cast<budVec2*>( this );
}

BUD_INLINE const float* budVec3::ToFloatPtr() const
{
	return &x;
}

BUD_INLINE float* budVec3::ToFloatPtr()
{
	return &x;
}

BUD_INLINE void budVec3::NormalVectors( budVec3& left, budVec3& down ) const
{
	float d;
	
	d = x * x + y * y;
	if( !d )
	{
		left[0] = 1;
		left[1] = 0;
		left[2] = 0;
	}
	else
	{
		d = budMath::InvSqrt( d );
		left[0] = -y * d;
		left[1] = x * d;
		left[2] = 0;
	}
	down = left.Cross( *this );
}

BUD_INLINE void budVec3::OrthogonalBasis( budVec3& left, budVec3& up ) const
{
	float l, s;
	
	if( budMath::Fabs( z ) > 0.7f )
	{
		l = y * y + z * z;
		s = budMath::InvSqrt( l );
		up[0] = 0;
		up[1] = z * s;
		up[2] = -y * s;
		left[0] = l * s;
		left[1] = -x * up[2];
		left[2] = x * up[1];
	}
	else
	{
		l = x * x + y * y;
		s = budMath::InvSqrt( l );
		left[0] = -y * s;
		left[1] = x * s;
		left[2] = 0;
		up[0] = -z * left[1];
		up[1] = z * left[0];
		up[2] = l * s;
	}
}

BUD_INLINE void budVec3::ProjectOntoPlane( const budVec3& normal, const float overBounce )
{
	float backoff;
	
	backoff = *this * normal;
	
	if( overBounce != 1.0 )
	{
		if( backoff < 0 )
		{
			backoff *= overBounce;
		}
		else
		{
			backoff /= overBounce;
		}
	}
	
	*this -= backoff * normal;
}

BUD_INLINE bool budVec3::ProjectAlongPlane( const budVec3& normal, const float epsilon, const float overBounce )
{
	budVec3 cross;
	float len;
	
	cross = this->Cross( normal ).Cross( ( *this ) );
	// normalize so a fixed epsilon can be used
	cross.Normalize();
	len = normal * cross;
	if( budMath::Fabs( len ) < epsilon )
	{
		return false;
	}
	cross *= overBounce * ( normal * ( *this ) ) / len;
	( *this ) -= cross;
	return true;
}

//===============================================================
//
//	idTupleSize< budVec3 > - Specialization to get the size
//	of an budVec3 generically.
//
//===============================================================

template<>
struct idTupleSize< budVec3 >
{
	enum { value = 3 };
};

//===============================================================
//
//	budVec4 - 4D vector
//
//===============================================================

class budVec4
{
public:
	float			x;
	float			y;
	float			z;
	float			w;
	
	budVec4() { }
	explicit budVec4( const float x )
	{
		Set( x, x, x, x );
	}
	explicit budVec4( const float x, const float y, const float z, const float w )
	{
		Set( x, y, z, w );
	}
	
	void 			Set( const float x, const float y, const float z, const float w );
	void			Zero();
	
	float			operator[]( const int index ) const;
	float& 			operator[]( const int index );
	budVec4			operator-() const;
	float			operator*( const budVec4& a ) const;
	budVec4			operator*( const float a ) const;
	budVec4			operator/( const float a ) const;
	budVec4			operator+( const budVec4& a ) const;
	budVec4			operator-( const budVec4& a ) const;
	budVec4& 		operator+=( const budVec4& a );
	budVec4& 		operator-=( const budVec4& a );
	budVec4& 		operator/=( const budVec4& a );
	budVec4& 		operator/=( const float a );
	budVec4& 		operator*=( const float a );
	
	friend budVec4	operator*( const float a, const budVec4 b );
	
	budVec4			Multiply( const budVec4& a ) const;
	
	bool			Compare( const budVec4& a ) const;							// exact compare, no epsilon
	bool			Compare( const budVec4& a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const budVec4& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const budVec4& a ) const;						// exact compare, no epsilon
	
	float			Length() const;
	float			LengthSqr() const;
	float			Normalize();			// returns length
	float			NormalizeFast();		// returns length
	
	int				GetDimension() const;
	
	const budVec2& 	ToVec2() const;
	budVec2& 		ToVec2();
	const budVec3& 	ToVec3() const;
	budVec3& 		ToVec3();
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
	void			Lerp( const budVec4& v1, const budVec4& v2, const float l );
};

extern budVec4 vec4_origin;
#define vec4_zero vec4_origin

BUD_INLINE void budVec4::Set( const float x, const float y, const float z, const float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

BUD_INLINE void budVec4::Zero()
{
	x = y = z = w = 0.0f;
}

BUD_INLINE float budVec4::operator[]( int index ) const
{
	return ( &x )[ index ];
}

BUD_INLINE float& budVec4::operator[]( int index )
{
	return ( &x )[ index ];
}

BUD_INLINE budVec4 budVec4::operator-() const
{
	return budVec4( -x, -y, -z, -w );
}

BUD_INLINE budVec4 budVec4::operator-( const budVec4& a ) const
{
	return budVec4( x - a.x, y - a.y, z - a.z, w - a.w );
}

BUD_INLINE float budVec4::operator*( const budVec4& a ) const
{
	return x * a.x + y * a.y + z * a.z + w * a.w;
}

BUD_INLINE budVec4 budVec4::operator*( const float a ) const
{
	return budVec4( x * a, y * a, z * a, w * a );
}

BUD_INLINE budVec4 budVec4::operator/( const float a ) const
{
	float inva = 1.0f / a;
	return budVec4( x * inva, y * inva, z * inva, w * inva );
}

BUD_INLINE budVec4 operator*( const float a, const budVec4 b )
{
	return budVec4( b.x * a, b.y * a, b.z * a, b.w * a );
}

BUD_INLINE budVec4 budVec4::operator+( const budVec4& a ) const
{
	return budVec4( x + a.x, y + a.y, z + a.z, w + a.w );
}

BUD_INLINE budVec4& budVec4::operator+=( const budVec4& a )
{
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;
	
	return *this;
}

BUD_INLINE budVec4& budVec4::operator/=( const budVec4& a )
{
	x /= a.x;
	y /= a.y;
	z /= a.z;
	w /= a.w;
	
	return *this;
}

BUD_INLINE budVec4& budVec4::operator/=( const float a )
{
	float inva = 1.0f / a;
	x *= inva;
	y *= inva;
	z *= inva;
	w *= inva;
	
	return *this;
}

BUD_INLINE budVec4& budVec4::operator-=( const budVec4& a )
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;
	
	return *this;
}

BUD_INLINE budVec4& budVec4::operator*=( const float a )
{
	x *= a;
	y *= a;
	z *= a;
	w *= a;
	
	return *this;
}

BUD_INLINE budVec4 budVec4::Multiply( const budVec4& a ) const
{
	return budVec4( x * a.x, y * a.y, z * a.z, w * a.w );
}

BUD_INLINE bool budVec4::Compare( const budVec4& a ) const
{
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) && w == a.w );
}

BUD_INLINE bool budVec4::Compare( const budVec4& a, const float epsilon ) const
{
	if( budMath::Fabs( x - a.x ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( y - a.y ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( z - a.z ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( w - a.w ) > epsilon )
	{
		return false;
	}
	
	return true;
}

BUD_INLINE bool budVec4::operator==( const budVec4& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budVec4::operator!=( const budVec4& a ) const
{
	return !Compare( a );
}

BUD_INLINE float budVec4::Length() const
{
	return ( float )budMath::Sqrt( x * x + y * y + z * z + w * w );
}

BUD_INLINE float budVec4::LengthSqr() const
{
	return ( x * x + y * y + z * z + w * w );
}

BUD_INLINE float budVec4::Normalize()
{
	float sqrLength, invLength;
	
	sqrLength = x * x + y * y + z * z + w * w;
	invLength = budMath::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	w *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE float budVec4::NormalizeFast()
{
	float sqrLength, invLength;
	
	sqrLength = x * x + y * y + z * z + w * w;
	invLength = budMath::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	w *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE int budVec4::GetDimension() const
{
	return 4;
}

BUD_INLINE const budVec2& budVec4::ToVec2() const
{
	return *reinterpret_cast<const budVec2*>( this );
}

BUD_INLINE budVec2& budVec4::ToVec2()
{
	return *reinterpret_cast<budVec2*>( this );
}

BUD_INLINE const budVec3& budVec4::ToVec3() const
{
	return *reinterpret_cast<const budVec3*>( this );
}

BUD_INLINE budVec3& budVec4::ToVec3()
{
	return *reinterpret_cast<budVec3*>( this );
}

BUD_INLINE const float* budVec4::ToFloatPtr() const
{
	return &x;
}

BUD_INLINE float* budVec4::ToFloatPtr()
{
	return &x;
}


//===============================================================
//
//	budVec5 - 5D vector
//
//===============================================================

class budVec5
{
public:
	float			x;
	float			y;
	float			z;
	float			s;
	float			t;
	
	budVec5();
	explicit budVec5( const budVec3& xyz, const budVec2& st );
	explicit budVec5( const float x, const float y, const float z, const float s, const float t );
	
	float			operator[]( int index ) const;
	float& 			operator[]( int index );
	budVec5& 		operator=( const budVec3& a );
	
	int				GetDimension() const;
	
	const budVec3& 	ToVec3() const;
	budVec3& 		ToVec3();
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
	void			Lerp( const budVec5& v1, const budVec5& v2, const float l );
};

extern budVec5 vec5_origin;
#define vec5_zero vec5_origin

BUD_INLINE budVec5::budVec5()
{
}

BUD_INLINE budVec5::budVec5( const budVec3& xyz, const budVec2& st )
{
	x = xyz.x;
	y = xyz.y;
	z = xyz.z;
	s = st[0];
	t = st[1];
}

BUD_INLINE budVec5::budVec5( const float x, const float y, const float z, const float s, const float t )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->s = s;
	this->t = t;
}

BUD_INLINE float budVec5::operator[]( int index ) const
{
	return ( &x )[ index ];
}

BUD_INLINE float& budVec5::operator[]( int index )
{
	return ( &x )[ index ];
}

BUD_INLINE budVec5& budVec5::operator=( const budVec3& a )
{
	x = a.x;
	y = a.y;
	z = a.z;
	s = t = 0;
	return *this;
}

BUD_INLINE int budVec5::GetDimension() const
{
	return 5;
}

BUD_INLINE const budVec3& budVec5::ToVec3() const
{
	return *reinterpret_cast<const budVec3*>( this );
}

BUD_INLINE budVec3& budVec5::ToVec3()
{
	return *reinterpret_cast<budVec3*>( this );
}

BUD_INLINE const float* budVec5::ToFloatPtr() const
{
	return &x;
}

BUD_INLINE float* budVec5::ToFloatPtr()
{
	return &x;
}


//===============================================================
//
//	budVec6 - 6D vector
//
//===============================================================

class budVec6
{
public:
	budVec6();
	explicit budVec6( const float* a );
	explicit budVec6( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 );
	
	void 			Set( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 );
	void			Zero();
	
	float			operator[]( const int index ) const;
	float& 			operator[]( const int index );
	budVec6			operator-() const;
	budVec6			operator*( const float a ) const;
	budVec6			operator/( const float a ) const;
	float			operator*( const budVec6& a ) const;
	budVec6			operator-( const budVec6& a ) const;
	budVec6			operator+( const budVec6& a ) const;
	budVec6& 		operator*=( const float a );
	budVec6& 		operator/=( const float a );
	budVec6& 		operator+=( const budVec6& a );
	budVec6& 		operator-=( const budVec6& a );
	
	friend budVec6	operator*( const float a, const budVec6 b );
	
	bool			Compare( const budVec6& a ) const;							// exact compare, no epsilon
	bool			Compare( const budVec6& a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const budVec6& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const budVec6& a ) const;						// exact compare, no epsilon
	
	float			Length() const;
	float			LengthSqr() const;
	float			Normalize();			// returns length
	float			NormalizeFast();		// returns length
	
	int				GetDimension() const;
	
	const budVec3& 	SubVec3( int index ) const;
	budVec3& 		SubVec3( int index );
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	float			p[6];
};

extern budVec6 vec6_origin;
#define vec6_zero vec6_origin
extern budVec6 vec6_infinity;

BUD_INLINE budVec6::budVec6()
{
}

BUD_INLINE budVec6::budVec6( const float* a )
{
	memcpy( p, a, 6 * sizeof( float ) );
}

BUD_INLINE budVec6::budVec6( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 )
{
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

BUD_INLINE budVec6 budVec6::operator-() const
{
	return budVec6( -p[0], -p[1], -p[2], -p[3], -p[4], -p[5] );
}

BUD_INLINE float budVec6::operator[]( const int index ) const
{
	return p[index];
}

BUD_INLINE float& budVec6::operator[]( const int index )
{
	return p[index];
}

BUD_INLINE budVec6 budVec6::operator*( const float a ) const
{
	return budVec6( p[0] * a, p[1] * a, p[2] * a, p[3] * a, p[4] * a, p[5] * a );
}

BUD_INLINE float budVec6::operator*( const budVec6& a ) const
{
	return p[0] * a[0] + p[1] * a[1] + p[2] * a[2] + p[3] * a[3] + p[4] * a[4] + p[5] * a[5];
}

BUD_INLINE budVec6 budVec6::operator/( const float a ) const
{
	float inva;
	
	assert( a != 0.0f );
	inva = 1.0f / a;
	return budVec6( p[0] * inva, p[1] * inva, p[2] * inva, p[3] * inva, p[4] * inva, p[5] * inva );
}

BUD_INLINE budVec6 budVec6::operator+( const budVec6& a ) const
{
	return budVec6( p[0] + a[0], p[1] + a[1], p[2] + a[2], p[3] + a[3], p[4] + a[4], p[5] + a[5] );
}

BUD_INLINE budVec6 budVec6::operator-( const budVec6& a ) const
{
	return budVec6( p[0] - a[0], p[1] - a[1], p[2] - a[2], p[3] - a[3], p[4] - a[4], p[5] - a[5] );
}

BUD_INLINE budVec6& budVec6::operator*=( const float a )
{
	p[0] *= a;
	p[1] *= a;
	p[2] *= a;
	p[3] *= a;
	p[4] *= a;
	p[5] *= a;
	return *this;
}

BUD_INLINE budVec6& budVec6::operator/=( const float a )
{
	float inva;
	
	assert( a != 0.0f );
	inva = 1.0f / a;
	p[0] *= inva;
	p[1] *= inva;
	p[2] *= inva;
	p[3] *= inva;
	p[4] *= inva;
	p[5] *= inva;
	return *this;
}

BUD_INLINE budVec6& budVec6::operator+=( const budVec6& a )
{
	p[0] += a[0];
	p[1] += a[1];
	p[2] += a[2];
	p[3] += a[3];
	p[4] += a[4];
	p[5] += a[5];
	return *this;
}

BUD_INLINE budVec6& budVec6::operator-=( const budVec6& a )
{
	p[0] -= a[0];
	p[1] -= a[1];
	p[2] -= a[2];
	p[3] -= a[3];
	p[4] -= a[4];
	p[5] -= a[5];
	return *this;
}

BUD_INLINE budVec6 operator*( const float a, const budVec6 b )
{
	return b * a;
}

BUD_INLINE bool budVec6::Compare( const budVec6& a ) const
{
	return ( ( p[0] == a[0] ) && ( p[1] == a[1] ) && ( p[2] == a[2] ) &&
			 ( p[3] == a[3] ) && ( p[4] == a[4] ) && ( p[5] == a[5] ) );
}

BUD_INLINE bool budVec6::Compare( const budVec6& a, const float epsilon ) const
{
	if( budMath::Fabs( p[0] - a[0] ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( p[1] - a[1] ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( p[2] - a[2] ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( p[3] - a[3] ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( p[4] - a[4] ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( p[5] - a[5] ) > epsilon )
	{
		return false;
	}
	
	return true;
}

BUD_INLINE bool budVec6::operator==( const budVec6& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budVec6::operator!=( const budVec6& a ) const
{
	return !Compare( a );
}

BUD_INLINE void budVec6::Set( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 )
{
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

BUD_INLINE void budVec6::Zero()
{
	p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = 0.0f;
}

BUD_INLINE float budVec6::Length() const
{
	return ( float )budMath::Sqrt( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5] );
}

BUD_INLINE float budVec6::LengthSqr() const
{
	return ( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5] );
}

BUD_INLINE float budVec6::Normalize()
{
	float sqrLength, invLength;
	
	sqrLength = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5];
	invLength = budMath::InvSqrt( sqrLength );
	p[0] *= invLength;
	p[1] *= invLength;
	p[2] *= invLength;
	p[3] *= invLength;
	p[4] *= invLength;
	p[5] *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE float budVec6::NormalizeFast()
{
	float sqrLength, invLength;
	
	sqrLength = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5];
	invLength = budMath::InvSqrt( sqrLength );
	p[0] *= invLength;
	p[1] *= invLength;
	p[2] *= invLength;
	p[3] *= invLength;
	p[4] *= invLength;
	p[5] *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE int budVec6::GetDimension() const
{
	return 6;
}

BUD_INLINE const budVec3& budVec6::SubVec3( int index ) const
{
	return *reinterpret_cast<const budVec3*>( p + index * 3 );
}

BUD_INLINE budVec3& budVec6::SubVec3( int index )
{
	return *reinterpret_cast<budVec3*>( p + index * 3 );
}

BUD_INLINE const float* budVec6::ToFloatPtr() const
{
	return p;
}

BUD_INLINE float* budVec6::ToFloatPtr()
{
	return p;
}

//===============================================================
//
//	idPolar3
//
//===============================================================

class idPolar3
{
public:
	float			radius, theta, phi;
	
	idPolar3();
	explicit idPolar3( const float radius, const float theta, const float phi );
	
	void 			Set( const float radius, const float theta, const float phi );
	
	float			operator[]( const int index ) const;
	float& 			operator[]( const int index );
	idPolar3		operator-() const;
	idPolar3& 		operator=( const idPolar3& a );
	
	budVec3			ToVec3() const;
};

BUD_INLINE idPolar3::idPolar3()
{
}

BUD_INLINE idPolar3::idPolar3( const float radius, const float theta, const float phi )
{
	assert( radius > 0 );
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}

BUD_INLINE void idPolar3::Set( const float radius, const float theta, const float phi )
{
	assert( radius > 0 );
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}

BUD_INLINE float idPolar3::operator[]( const int index ) const
{
	return ( &radius )[ index ];
}

BUD_INLINE float& idPolar3::operator[]( const int index )
{
	return ( &radius )[ index ];
}

BUD_INLINE idPolar3 idPolar3::operator-() const
{
	return idPolar3( radius, -theta, -phi );
}

BUD_INLINE idPolar3& idPolar3::operator=( const idPolar3& a )
{
	radius = a.radius;
	theta = a.theta;
	phi = a.phi;
	return *this;
}

BUD_INLINE budVec3 idPolar3::ToVec3() const
{
	float sp, cp, st, ct;
	budMath::SinCos( phi, sp, cp );
	budMath::SinCos( theta, st, ct );
	return budVec3( cp * radius * ct, cp * radius * st, radius * sp );
}


/*
===============================================================================

	Old 3D vector macros, should no longer be used.

===============================================================================
*/

#define	VectorMA( v, s, b, o )		((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))

#endif /* !__MATH_VECTOR_H__ */
