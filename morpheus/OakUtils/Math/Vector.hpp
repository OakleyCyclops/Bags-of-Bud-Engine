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

#define VECTOR_EPSILON		0.001f

class Angles;
class Polar3;
class Matrix3;

//===============================================================
//
//	Vector2 - 2D vector
//
//===============================================================

class Vector2
{
public:
	float			x;
	float			y;
	
	Vector2();
	explicit Vector2( const float x, const float y );
	
	void 			Set( const float x, const float y );
	void			Zero();
	
	float			operator[]( int index ) const;
	float& 			operator[]( int index );
	Vector2			operator-() const;
	float			operator*( const Vector2& a ) const;
	Vector2			operator*( const float a ) const;
	Vector2			operator/( const float a ) const;
	Vector2			operator+( const Vector2& a ) const;
	Vector2			operator-( const Vector2& a ) const;
	Vector2& 		operator+=( const Vector2& a );
	Vector2& 		operator-=( const Vector2& a );
	Vector2& 		operator/=( const Vector2& a );
	Vector2& 		operator/=( const float a );
	Vector2& 		operator*=( const float a );
	
	friend Vector2	operator*( const float a, const Vector2 b );
	
	Vector2			Scale( const Vector2& a ) const;
	
	bool			Compare( const Vector2& a ) const;							// exact compare, no epsilon
	bool			Compare( const Vector2& a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const Vector2& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const Vector2& a ) const;						// exact compare, no epsilon
	
	float			Length() const;
	float			LengthFast() const;
	float			LengthSqr() const;
	float			Normalize();			// returns length
	float			NormalizeFast();		// returns length
	Vector2			Truncate( float length ) const;	// cap length
	void			Clamp( const Vector2& min, const Vector2& max );
	void			Snap();				// snap to closest integer value
	void			SnapInt();			// snap towards integer (floor)
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
	void			Lerp( const Vector2& v1, const Vector2& v2, const float l );
};

extern Vector2 Vector2_Origin;
#define vec2_zero Vector2_Origin

BUD_INLINE Vector2::Vector2()
{
}

BUD_INLINE Vector2::Vector2( const float x, const float y )
{
	this->x = x;
	this->y = y;
}

BUD_INLINE void Vector2::Set( const float x, const float y )
{
	this->x = x;
	this->y = y;
}

BUD_INLINE void Vector2::Zero()
{
	x = y = 0.0f;
}

BUD_INLINE bool Vector2::Compare( const Vector2& a ) const
{
	return ( ( x == a.x ) && ( y == a.y ) );
}

BUD_INLINE bool Vector2::operator==( const Vector2& a ) const
{
	return Compare( a );
}

BUD_INLINE bool Vector2::operator!=( const Vector2& a ) const
{
	return !Compare( a );
}

BUD_INLINE float Vector2::operator[]( int index ) const
{
	return ( &x )[ index ];
}

BUD_INLINE float& Vector2::operator[]( int index )
{
	return ( &x )[ index ];
}

BUD_INLINE float Vector2::Length() const
{
	return ( float )Math::Sqrt( x * x + y * y );
}

BUD_INLINE float Vector2::LengthFast() const
{
	float sqrLength;
	
	sqrLength = x * x + y * y;
	return sqrLength * Math::InvSqrt( sqrLength );
}

BUD_INLINE float Vector2::LengthSqr() const
{
	return ( x * x + y * y );
}

BUD_INLINE float Vector2::Normalize()
{
	float sqrLength, invLength;
	
	sqrLength = x * x + y * y;
	invLength = Math::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE float Vector2::NormalizeFast()
{
	float lengthSqr, invLength;
	
	lengthSqr = x * x + y * y;
	invLength = Math::InvSqrt( lengthSqr );
	x *= invLength;
	y *= invLength;
	return invLength * lengthSqr;
}

BUD_INLINE Vector2 Vector2::Truncate( float length ) const
{
	if( length < Math::FLT_SMALLEST_NON_DENORMAL )
	{
		return vec2_zero;
	}
	else
	{
		float length2 = LengthSqr();
		if( length2 > length * length )
		{
			float ilength = length * Math::InvSqrt( length2 );
			return *this * ilength;
		}
	}
	return *this;
}

BUD_INLINE void Vector2::Clamp( const Vector2& min, const Vector2& max )
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

BUD_INLINE void Vector2::Snap()
{
	x = floor( x + 0.5f );
	y = floor( y + 0.5f );
}

BUD_INLINE void Vector2::SnapInt()
{
	x = float( int( x ) );
	y = float( int( y ) );
}

BUD_INLINE Vector2 Vector2::operator-() const
{
	return Vector2( -x, -y );
}

BUD_INLINE Vector2 Vector2::operator-( const Vector2& a ) const
{
	return Vector2( x - a.x, y - a.y );
}

BUD_INLINE float Vector2::operator*( const Vector2& a ) const
{
	return x * a.x + y * a.y;
}

BUD_INLINE Vector2 Vector2::operator*( const float a ) const
{
	return Vector2( x * a, y * a );
}

BUD_INLINE Vector2 Vector2::operator/( const float a ) const
{
	float inva = 1.0f / a;
	return Vector2( x * inva, y * inva );
}

BUD_INLINE Vector2 operator*( const float a, const Vector2 b )
{
	return Vector2( b.x * a, b.y * a );
}

BUD_INLINE Vector2 Vector2::operator+( const Vector2& a ) const
{
	return Vector2( x + a.x, y + a.y );
}

BUD_INLINE Vector2& Vector2::operator+=( const Vector2& a )
{
	x += a.x;
	y += a.y;
	
	return *this;
}

BUD_INLINE Vector2& Vector2::operator/=( const Vector2& a )
{
	x /= a.x;
	y /= a.y;
	
	return *this;
}

BUD_INLINE Vector2& Vector2::operator/=( const float a )
{
	float inva = 1.0f / a;
	x *= inva;
	y *= inva;
	
	return *this;
}

BUD_INLINE Vector2& Vector2::operator-=( const Vector2& a )
{
	x -= a.x;
	y -= a.y;
	
	return *this;
}

BUD_INLINE Vector2& Vector2::operator*=( const float a )
{
	x *= a;
	y *= a;
	
	return *this;
}

BUD_INLINE Vector2 Vector2::Scale( const Vector2& a ) const
{
	return Vector2( x * a.x, y * a.y );
}

BUD_INLINE int Vector2::GetDimension() const
{
	return 2;
}

BUD_INLINE const float* Vector2::ToFloatPtr() const
{
	return &x;
}

BUD_INLINE float* Vector2::ToFloatPtr()
{
	return &x;
}


//===============================================================
//
//	Vector3 - 3D vector
//
//===============================================================

class Vector3
{
public:
	float			x;
	float			y;
	float			z;
	
	Vector3();
	explicit Vector3( const float xyz )
	{
		Set( xyz, xyz, xyz );
	}
	explicit Vector3( const float x, const float y, const float z );
	
	void 			Set( const float x, const float y, const float z );
	void			Zero();
	
	float			operator[]( const int index ) const;
	float& 			operator[]( const int index );
	Vector3			operator-() const;
	Vector3& 		operator=( const Vector3& a );		// required because of a msvc 6 & 7 bug
	float			operator*( const Vector3& a ) const;
	Vector3			operator*( const float a ) const;
	Vector3			operator/( const float a ) const;
	Vector3			operator+( const Vector3& a ) const;
	Vector3			operator-( const Vector3& a ) const;
	Vector3& 		operator+=( const Vector3& a );
	Vector3& 		operator-=( const Vector3& a );
	Vector3& 		operator/=( const Vector3& a );
	Vector3& 		operator/=( const float a );
	Vector3& 		operator*=( const float a );
	
	friend Vector3	operator*( const float a, const Vector3 b );
	
	bool			Compare( const Vector3& a ) const;							// exact compare, no epsilon
	bool			Compare( const Vector3& a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const Vector3& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const Vector3& a ) const;						// exact compare, no epsilon
	
	bool			FixDegenerateNormal();	// fix degenerate axial cases
	bool			FixDenormals();			// change tiny numbers to zero
	
	Vector3			Cross( const Vector3& a ) const;
	Vector3& 		Cross( const Vector3& a, const Vector3& b );
	float			Length() const;
	float			LengthSqr() const;
	float			LengthFast() const;
	float			Normalize();				// returns length
	float			NormalizeFast();			// returns length
	Vector3			Truncate( float length ) const;		// cap length
	void			Clamp( const Vector3& min, const Vector3& max );
	void			Snap();					// snap to closest integer value
	void			SnapInt();				// snap towards integer (floor)
	
	int				GetDimension() const;
	
	float			ToYaw() const;
	float			ToPitch() const;
	Angles		ToAngles() const;
	Polar3		ToPolar() const;
	Matrix3			ToMat3() const;		// vector should be normalized
	const Vector2& 	ToVec2() const;
	Vector2& 		ToVec2();
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
	void			NormalVectors( Vector3& left, Vector3& down ) const;	// vector should be normalized
	void			OrthogonalBasis( Vector3& left, Vector3& up ) const;
	
	void			ProjectOntoPlane( const Vector3& normal, const float overBounce = 1.0f );
	bool			ProjectAlongPlane( const Vector3& normal, const float epsilon, const float overBounce = 1.0f );
	void			ProjectSelfOntoSphere( const float radius );
	
	void			Lerp( const Vector3& v1, const Vector3& v2, const float l );
	void			SLerp( const Vector3& v1, const Vector3& v2, const float l );
};

extern Vector3 Vector3_Origin;
#define vec3_zero Vector3_Origin

BUD_INLINE Vector3::Vector3()
{
}

BUD_INLINE Vector3::Vector3( const float x, const float y, const float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

BUD_INLINE float Vector3::operator[]( const int index ) const
{
	return ( &x )[ index ];
}

BUD_INLINE float& Vector3::operator[]( const int index )
{
	return ( &x )[ index ];
}

BUD_INLINE void Vector3::Set( const float x, const float y, const float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

BUD_INLINE void Vector3::Zero()
{
	x = y = z = 0.0f;
}

BUD_INLINE Vector3 Vector3::operator-() const
{
	return Vector3( -x, -y, -z );
}

BUD_INLINE Vector3& Vector3::operator=( const Vector3& a )
{
	x = a.x;
	y = a.y;
	z = a.z;
	return *this;
}

BUD_INLINE Vector3 Vector3::operator-( const Vector3& a ) const
{
	return Vector3( x - a.x, y - a.y, z - a.z );
}

BUD_INLINE float Vector3::operator*( const Vector3& a ) const
{
	return x * a.x + y * a.y + z * a.z;
}

BUD_INLINE Vector3 Vector3::operator*( const float a ) const
{
	return Vector3( x * a, y * a, z * a );
}

BUD_INLINE Vector3 Vector3::operator/( const float a ) const
{
	float inva = 1.0f / a;
	return Vector3( x * inva, y * inva, z * inva );
}

BUD_INLINE Vector3 operator*( const float a, const Vector3 b )
{
	return Vector3( b.x * a, b.y * a, b.z * a );
}

BUD_INLINE Vector3 operator/( const float a, const Vector3 b )
{
	return Vector3( a / b.x, a / b.y, a / b.z );
}

BUD_INLINE Vector3 Vector3::operator+( const Vector3& a ) const
{
	return Vector3( x + a.x, y + a.y, z + a.z );
}

BUD_INLINE Vector3& Vector3::operator+=( const Vector3& a )
{
	x += a.x;
	y += a.y;
	z += a.z;
	
	return *this;
}

BUD_INLINE Vector3& Vector3::operator/=( const Vector3& a )
{
	x /= a.x;
	y /= a.y;
	z /= a.z;
	
	return *this;
}

BUD_INLINE Vector3& Vector3::operator/=( const float a )
{
	float inva = 1.0f / a;
	x *= inva;
	y *= inva;
	z *= inva;
	
	return *this;
}

BUD_INLINE Vector3& Vector3::operator-=( const Vector3& a )
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	
	return *this;
}

BUD_INLINE Vector3& Vector3::operator*=( const float a )
{
	x *= a;
	y *= a;
	z *= a;
	
	return *this;
}

BUD_INLINE bool Vector3::Compare( const Vector3& a ) const
{
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) );
}

BUD_INLINE bool Vector3::Compare( const Vector3& a, const float epsilon ) const
{
	if( Math::Fabs( x - a.x ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( y - a.y ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( z - a.z ) > epsilon )
	{
		return false;
	}
	
	return true;
}

BUD_INLINE bool Vector3::operator==( const Vector3& a ) const
{
	return Compare( a );
}

BUD_INLINE bool Vector3::operator!=( const Vector3& a ) const
{
	return !Compare( a );
}

BUD_INLINE float Vector3::NormalizeFast()
{
	float sqrLength, invLength;
	
	sqrLength = x * x + y * y + z * z;
	invLength = Math::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE bool Vector3::FixDegenerateNormal()
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
	if( Math::Fabs( x ) == 1.0f )
	{
		if( y != 0.0f || z != 0.0f )
		{
			y = z = 0.0f;
			return true;
		}
		return false;
	}
	else if( Math::Fabs( y ) == 1.0f )
	{
		if( x != 0.0f || z != 0.0f )
		{
			x = z = 0.0f;
			return true;
		}
		return false;
	}
	else if( Math::Fabs( z ) == 1.0f )
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

BUD_INLINE bool Vector3::FixDenormals()
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

BUD_INLINE Vector3 Vector3::Cross( const Vector3& a ) const
{
	return Vector3( y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x );
}

BUD_INLINE Vector3& Vector3::Cross( const Vector3& a, const Vector3& b )
{
	x = a.y * b.z - a.z * b.y;
	y = a.z * b.x - a.x * b.z;
	z = a.x * b.y - a.y * b.x;
	
	return *this;
}

BUD_INLINE float Vector3::Length() const
{
	return ( float )Math::Sqrt( x * x + y * y + z * z );
}

BUD_INLINE float Vector3::LengthSqr() const
{
	return ( x * x + y * y + z * z );
}

BUD_INLINE float Vector3::LengthFast() const
{
	float sqrLength;
	
	sqrLength = x * x + y * y + z * z;
	return sqrLength * Math::InvSqrt( sqrLength );
}

BUD_INLINE float Vector3::Normalize()
{
	float sqrLength, invLength;
	
	sqrLength = x * x + y * y + z * z;
	invLength = Math::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE Vector3 Vector3::Truncate( float length ) const
{
	if( length < Math::FLT_SMALLEST_NON_DENORMAL )
	{
		return vec3_zero;
	}
	else
	{
		float length2 = LengthSqr();
		if( length2 > length * length )
		{
			float ilength = length * Math::InvSqrt( length2 );
			return *this * ilength;
		}
	}
	return *this;
}

BUD_INLINE void Vector3::Clamp( const Vector3& min, const Vector3& max )
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

BUD_INLINE void Vector3::Snap()
{
	x = floor( x + 0.5f );
	y = floor( y + 0.5f );
	z = floor( z + 0.5f );
}

BUD_INLINE void Vector3::SnapInt()
{
	x = float( int( x ) );
	y = float( int( y ) );
	z = float( int( z ) );
}

BUD_INLINE int Vector3::GetDimension() const
{
	return 3;
}

BUD_INLINE const Vector2& Vector3::ToVec2() const
{
	return *reinterpret_cast<const Vector2*>( this );
}

BUD_INLINE Vector2& Vector3::ToVec2()
{
	return *reinterpret_cast<Vector2*>( this );
}

BUD_INLINE const float* Vector3::ToFloatPtr() const
{
	return &x;
}

BUD_INLINE float* Vector3::ToFloatPtr()
{
	return &x;
}

BUD_INLINE void Vector3::NormalVectors( Vector3& left, Vector3& down ) const
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
		d = Math::InvSqrt( d );
		left[0] = -y * d;
		left[1] = x * d;
		left[2] = 0;
	}
	down = left.Cross( *this );
}

BUD_INLINE void Vector3::OrthogonalBasis( Vector3& left, Vector3& up ) const
{
	float l, s;
	
	if( Math::Fabs( z ) > 0.7f )
	{
		l = y * y + z * z;
		s = Math::InvSqrt( l );
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
		s = Math::InvSqrt( l );
		left[0] = -y * s;
		left[1] = x * s;
		left[2] = 0;
		up[0] = -z * left[1];
		up[1] = z * left[0];
		up[2] = l * s;
	}
}

BUD_INLINE void Vector3::ProjectOntoPlane( const Vector3& normal, const float overBounce )
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

BUD_INLINE bool Vector3::ProjectAlongPlane( const Vector3& normal, const float epsilon, const float overBounce )
{
	Vector3 cross;
	float len;
	
	cross = this->Cross( normal ).Cross( ( *this ) );
	// normalize so a fixed epsilon can be used
	cross.Normalize();
	len = normal * cross;
	if( Math::Fabs( len ) < epsilon )
	{
		return false;
	}
	cross *= overBounce * ( normal * ( *this ) ) / len;
	( *this ) -= cross;
	return true;
}

//===============================================================
//
//	TupleSize< Vector3 > - Specialization to get the size
//	of an Vector3 generically.
//
//===============================================================

template<>
struct TupleSize< Vector3 >
{
	enum { value = 3 };
};

//===============================================================
//
//	Vector4 - 4D vector
//
//===============================================================

class Vector4
{
public:
	float			x;
	float			y;
	float			z;
	float			w;
	
	Vector4() { }
	explicit Vector4( const float x )
	{
		Set( x, x, x, x );
	}
	explicit Vector4( const float x, const float y, const float z, const float w )
	{
		Set( x, y, z, w );
	}
	
	void 			Set( const float x, const float y, const float z, const float w );
	void			Zero();
	
	float			operator[]( const int index ) const;
	float& 			operator[]( const int index );
	Vector4			operator-() const;
	float			operator*( const Vector4& a ) const;
	Vector4			operator*( const float a ) const;
	Vector4			operator/( const float a ) const;
	Vector4			operator+( const Vector4& a ) const;
	Vector4			operator-( const Vector4& a ) const;
	Vector4& 		operator+=( const Vector4& a );
	Vector4& 		operator-=( const Vector4& a );
	Vector4& 		operator/=( const Vector4& a );
	Vector4& 		operator/=( const float a );
	Vector4& 		operator*=( const float a );
	
	friend Vector4	operator*( const float a, const Vector4 b );
	
	Vector4			Multiply( const Vector4& a ) const;
	
	bool			Compare( const Vector4& a ) const;							// exact compare, no epsilon
	bool			Compare( const Vector4& a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const Vector4& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const Vector4& a ) const;						// exact compare, no epsilon
	
	float			Length() const;
	float			LengthSqr() const;
	float			Normalize();			// returns length
	float			NormalizeFast();		// returns length
	
	int				GetDimension() const;
	
	const Vector2& 	ToVec2() const;
	Vector2& 		ToVec2();
	const Vector3& 	ToVec3() const;
	Vector3& 		ToVec3();
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
	void			Lerp( const Vector4& v1, const Vector4& v2, const float l );
};

extern Vector4 Vector4_Origin;
#define vec4_zero Vector4_Origin

BUD_INLINE void Vector4::Set( const float x, const float y, const float z, const float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

BUD_INLINE void Vector4::Zero()
{
	x = y = z = w = 0.0f;
}

BUD_INLINE float Vector4::operator[]( int index ) const
{
	return ( &x )[ index ];
}

BUD_INLINE float& Vector4::operator[]( int index )
{
	return ( &x )[ index ];
}

BUD_INLINE Vector4 Vector4::operator-() const
{
	return Vector4( -x, -y, -z, -w );
}

BUD_INLINE Vector4 Vector4::operator-( const Vector4& a ) const
{
	return Vector4( x - a.x, y - a.y, z - a.z, w - a.w );
}

BUD_INLINE float Vector4::operator*( const Vector4& a ) const
{
	return x * a.x + y * a.y + z * a.z + w * a.w;
}

BUD_INLINE Vector4 Vector4::operator*( const float a ) const
{
	return Vector4( x * a, y * a, z * a, w * a );
}

BUD_INLINE Vector4 Vector4::operator/( const float a ) const
{
	float inva = 1.0f / a;
	return Vector4( x * inva, y * inva, z * inva, w * inva );
}

BUD_INLINE Vector4 operator*( const float a, const Vector4 b )
{
	return Vector4( b.x * a, b.y * a, b.z * a, b.w * a );
}

BUD_INLINE Vector4 Vector4::operator+( const Vector4& a ) const
{
	return Vector4( x + a.x, y + a.y, z + a.z, w + a.w );
}

BUD_INLINE Vector4& Vector4::operator+=( const Vector4& a )
{
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;
	
	return *this;
}

BUD_INLINE Vector4& Vector4::operator/=( const Vector4& a )
{
	x /= a.x;
	y /= a.y;
	z /= a.z;
	w /= a.w;
	
	return *this;
}

BUD_INLINE Vector4& Vector4::operator/=( const float a )
{
	float inva = 1.0f / a;
	x *= inva;
	y *= inva;
	z *= inva;
	w *= inva;
	
	return *this;
}

BUD_INLINE Vector4& Vector4::operator-=( const Vector4& a )
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;
	
	return *this;
}

BUD_INLINE Vector4& Vector4::operator*=( const float a )
{
	x *= a;
	y *= a;
	z *= a;
	w *= a;
	
	return *this;
}

BUD_INLINE Vector4 Vector4::Multiply( const Vector4& a ) const
{
	return Vector4( x * a.x, y * a.y, z * a.z, w * a.w );
}

BUD_INLINE bool Vector4::Compare( const Vector4& a ) const
{
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) && w == a.w );
}

BUD_INLINE bool Vector4::Compare( const Vector4& a, const float epsilon ) const
{
	if( Math::Fabs( x - a.x ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( y - a.y ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( z - a.z ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( w - a.w ) > epsilon )
	{
		return false;
	}
	
	return true;
}

BUD_INLINE bool Vector4::operator==( const Vector4& a ) const
{
	return Compare( a );
}

BUD_INLINE bool Vector4::operator!=( const Vector4& a ) const
{
	return !Compare( a );
}

BUD_INLINE float Vector4::Length() const
{
	return ( float )Math::Sqrt( x * x + y * y + z * z + w * w );
}

BUD_INLINE float Vector4::LengthSqr() const
{
	return ( x * x + y * y + z * z + w * w );
}

BUD_INLINE float Vector4::Normalize()
{
	float sqrLength, invLength;
	
	sqrLength = x * x + y * y + z * z + w * w;
	invLength = Math::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	w *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE float Vector4::NormalizeFast()
{
	float sqrLength, invLength;
	
	sqrLength = x * x + y * y + z * z + w * w;
	invLength = Math::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	w *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE int Vector4::GetDimension() const
{
	return 4;
}

BUD_INLINE const Vector2& Vector4::ToVec2() const
{
	return *reinterpret_cast<const Vector2*>( this );
}

BUD_INLINE Vector2& Vector4::ToVec2()
{
	return *reinterpret_cast<Vector2*>( this );
}

BUD_INLINE const Vector3& Vector4::ToVec3() const
{
	return *reinterpret_cast<const Vector3*>( this );
}

BUD_INLINE Vector3& Vector4::ToVec3()
{
	return *reinterpret_cast<Vector3*>( this );
}

BUD_INLINE const float* Vector4::ToFloatPtr() const
{
	return &x;
}

BUD_INLINE float* Vector4::ToFloatPtr()
{
	return &x;
}


//===============================================================
//
//	Vector5 - 5D vector
//
//===============================================================

class Vector5
{
public:
	float			x;
	float			y;
	float			z;
	float			s;
	float			t;
	
	Vector5();
	explicit Vector5( const Vector3& xyz, const Vector2& st );
	explicit Vector5( const float x, const float y, const float z, const float s, const float t );
	
	float			operator[]( int index ) const;
	float& 			operator[]( int index );
	Vector5& 		operator=( const Vector3& a );
	
	int				GetDimension() const;
	
	const Vector3& 	ToVec3() const;
	Vector3& 		ToVec3();
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
	void			Lerp( const Vector5& v1, const Vector5& v2, const float l );
};

extern Vector5 Vector5_Origin;
#define vec5_zero Vector5_Origin

BUD_INLINE Vector5::Vector5()
{
}

BUD_INLINE Vector5::Vector5( const Vector3& xyz, const Vector2& st )
{
	x = xyz.x;
	y = xyz.y;
	z = xyz.z;
	s = st[0];
	t = st[1];
}

BUD_INLINE Vector5::Vector5( const float x, const float y, const float z, const float s, const float t )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->s = s;
	this->t = t;
}

BUD_INLINE float Vector5::operator[]( int index ) const
{
	return ( &x )[ index ];
}

BUD_INLINE float& Vector5::operator[]( int index )
{
	return ( &x )[ index ];
}

BUD_INLINE Vector5& Vector5::operator=( const Vector3& a )
{
	x = a.x;
	y = a.y;
	z = a.z;
	s = t = 0;
	return *this;
}

BUD_INLINE int Vector5::GetDimension() const
{
	return 5;
}

BUD_INLINE const Vector3& Vector5::ToVec3() const
{
	return *reinterpret_cast<const Vector3*>( this );
}

BUD_INLINE Vector3& Vector5::ToVec3()
{
	return *reinterpret_cast<Vector3*>( this );
}

BUD_INLINE const float* Vector5::ToFloatPtr() const
{
	return &x;
}

BUD_INLINE float* Vector5::ToFloatPtr()
{
	return &x;
}


//===============================================================
//
//	Vector6 - 6D vector
//
//===============================================================

class Vector6
{
public:
	Vector6();
	explicit Vector6( const float* a );
	explicit Vector6( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 );
	
	void 			Set( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 );
	void			Zero();
	
	float			operator[]( const int index ) const;
	float& 			operator[]( const int index );
	Vector6			operator-() const;
	Vector6			operator*( const float a ) const;
	Vector6			operator/( const float a ) const;
	float			operator*( const Vector6& a ) const;
	Vector6			operator-( const Vector6& a ) const;
	Vector6			operator+( const Vector6& a ) const;
	Vector6& 		operator*=( const float a );
	Vector6& 		operator/=( const float a );
	Vector6& 		operator+=( const Vector6& a );
	Vector6& 		operator-=( const Vector6& a );
	
	friend Vector6	operator*( const float a, const Vector6 b );
	
	bool			Compare( const Vector6& a ) const;							// exact compare, no epsilon
	bool			Compare( const Vector6& a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const Vector6& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const Vector6& a ) const;						// exact compare, no epsilon
	
	float			Length() const;
	float			LengthSqr() const;
	float			Normalize();			// returns length
	float			NormalizeFast();		// returns length
	
	int				GetDimension() const;
	
	const Vector3& 	SubVec3( int index ) const;
	Vector3& 		SubVec3( int index );
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	float			p[6];
};

extern Vector6 Vector6_Origin;
#define vec6_zero Vector6_Origin
extern Vector6 Vector6_Infinity;

BUD_INLINE Vector6::Vector6()
{
}

BUD_INLINE Vector6::Vector6( const float* a )
{
	memcpy( p, a, 6 * sizeof( float ) );
}

BUD_INLINE Vector6::Vector6( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 )
{
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

BUD_INLINE Vector6 Vector6::operator-() const
{
	return Vector6( -p[0], -p[1], -p[2], -p[3], -p[4], -p[5] );
}

BUD_INLINE float Vector6::operator[]( const int index ) const
{
	return p[index];
}

BUD_INLINE float& Vector6::operator[]( const int index )
{
	return p[index];
}

BUD_INLINE Vector6 Vector6::operator*( const float a ) const
{
	return Vector6( p[0] * a, p[1] * a, p[2] * a, p[3] * a, p[4] * a, p[5] * a );
}

BUD_INLINE float Vector6::operator*( const Vector6& a ) const
{
	return p[0] * a[0] + p[1] * a[1] + p[2] * a[2] + p[3] * a[3] + p[4] * a[4] + p[5] * a[5];
}

BUD_INLINE Vector6 Vector6::operator/( const float a ) const
{
	float inva;
	
	assert( a != 0.0f );
	inva = 1.0f / a;
	return Vector6( p[0] * inva, p[1] * inva, p[2] * inva, p[3] * inva, p[4] * inva, p[5] * inva );
}

BUD_INLINE Vector6 Vector6::operator+( const Vector6& a ) const
{
	return Vector6( p[0] + a[0], p[1] + a[1], p[2] + a[2], p[3] + a[3], p[4] + a[4], p[5] + a[5] );
}

BUD_INLINE Vector6 Vector6::operator-( const Vector6& a ) const
{
	return Vector6( p[0] - a[0], p[1] - a[1], p[2] - a[2], p[3] - a[3], p[4] - a[4], p[5] - a[5] );
}

BUD_INLINE Vector6& Vector6::operator*=( const float a )
{
	p[0] *= a;
	p[1] *= a;
	p[2] *= a;
	p[3] *= a;
	p[4] *= a;
	p[5] *= a;
	return *this;
}

BUD_INLINE Vector6& Vector6::operator/=( const float a )
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

BUD_INLINE Vector6& Vector6::operator+=( const Vector6& a )
{
	p[0] += a[0];
	p[1] += a[1];
	p[2] += a[2];
	p[3] += a[3];
	p[4] += a[4];
	p[5] += a[5];
	return *this;
}

BUD_INLINE Vector6& Vector6::operator-=( const Vector6& a )
{
	p[0] -= a[0];
	p[1] -= a[1];
	p[2] -= a[2];
	p[3] -= a[3];
	p[4] -= a[4];
	p[5] -= a[5];
	return *this;
}

BUD_INLINE Vector6 operator*( const float a, const Vector6 b )
{
	return b * a;
}

BUD_INLINE bool Vector6::Compare( const Vector6& a ) const
{
	return ( ( p[0] == a[0] ) && ( p[1] == a[1] ) && ( p[2] == a[2] ) &&
			 ( p[3] == a[3] ) && ( p[4] == a[4] ) && ( p[5] == a[5] ) );
}

BUD_INLINE bool Vector6::Compare( const Vector6& a, const float epsilon ) const
{
	if( Math::Fabs( p[0] - a[0] ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( p[1] - a[1] ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( p[2] - a[2] ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( p[3] - a[3] ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( p[4] - a[4] ) > epsilon )
	{
		return false;
	}
	
	if( Math::Fabs( p[5] - a[5] ) > epsilon )
	{
		return false;
	}
	
	return true;
}

BUD_INLINE bool Vector6::operator==( const Vector6& a ) const
{
	return Compare( a );
}

BUD_INLINE bool Vector6::operator!=( const Vector6& a ) const
{
	return !Compare( a );
}

BUD_INLINE void Vector6::Set( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 )
{
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

BUD_INLINE void Vector6::Zero()
{
	p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = 0.0f;
}

BUD_INLINE float Vector6::Length() const
{
	return ( float )Math::Sqrt( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5] );
}

BUD_INLINE float Vector6::LengthSqr() const
{
	return ( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5] );
}

BUD_INLINE float Vector6::Normalize()
{
	float sqrLength, invLength;
	
	sqrLength = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5];
	invLength = Math::InvSqrt( sqrLength );
	p[0] *= invLength;
	p[1] *= invLength;
	p[2] *= invLength;
	p[3] *= invLength;
	p[4] *= invLength;
	p[5] *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE float Vector6::NormalizeFast()
{
	float sqrLength, invLength;
	
	sqrLength = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5];
	invLength = Math::InvSqrt( sqrLength );
	p[0] *= invLength;
	p[1] *= invLength;
	p[2] *= invLength;
	p[3] *= invLength;
	p[4] *= invLength;
	p[5] *= invLength;
	return invLength * sqrLength;
}

BUD_INLINE int Vector6::GetDimension() const
{
	return 6;
}

BUD_INLINE const Vector3& Vector6::SubVec3( int index ) const
{
	return *reinterpret_cast<const Vector3*>( p + index * 3 );
}

BUD_INLINE Vector3& Vector6::SubVec3( int index )
{
	return *reinterpret_cast<Vector3*>( p + index * 3 );
}

BUD_INLINE const float* Vector6::ToFloatPtr() const
{
	return p;
}

BUD_INLINE float* Vector6::ToFloatPtr()
{
	return p;
}

//===============================================================
//
//	Polar3
//
//===============================================================

class Polar3
{
public:
	float			radius, theta, phi;
	
	Polar3();
	explicit Polar3( const float radius, const float theta, const float phi );
	
	void 			Set( const float radius, const float theta, const float phi );
	
	float			operator[]( const int index ) const;
	float& 			operator[]( const int index );
	Polar3		operator-() const;
	Polar3& 		operator=( const Polar3& a );
	
	Vector3			ToVec3() const;
};

BUD_INLINE Polar3::Polar3()
{
}

BUD_INLINE Polar3::Polar3( const float radius, const float theta, const float phi )
{
	assert( radius > 0 );
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}

BUD_INLINE void Polar3::Set( const float radius, const float theta, const float phi )
{
	assert( radius > 0 );
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}

BUD_INLINE float Polar3::operator[]( const int index ) const
{
	return ( &radius )[ index ];
}

BUD_INLINE float& Polar3::operator[]( const int index )
{
	return ( &radius )[ index ];
}

BUD_INLINE Polar3 Polar3::operator-() const
{
	return Polar3( radius, -theta, -phi );
}

BUD_INLINE Polar3& Polar3::operator=( const Polar3& a )
{
	radius = a.radius;
	theta = a.theta;
	phi = a.phi;
	return *this;
}

BUD_INLINE Vector3 Polar3::ToVec3() const
{
	float sp, cp, st, ct;
	Math::SinCos( phi, sp, cp );
	Math::SinCos( theta, st, ct );
	return Vector3( cp * radius * ct, cp * radius * st, radius * sp );
}


/*
===============================================================================

	Old 3D vector macros, should no longer be used.

===============================================================================
*/

#define	VectorMA( v, s, b, o )		((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))

#endif /* !__MATH_VECTOR_H__ */
