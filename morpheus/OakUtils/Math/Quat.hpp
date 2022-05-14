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

#ifndef __MATH_QUAT_H__
#define __MATH_QUAT_H__

/*
===============================================================================

	Quaternion

===============================================================================
*/

class Vector3;
class Angles;
class Rotation;
class Matrix3;
class Matrix4;
class CMPQuat;

class Quat
{
public:
	float			x;
	float			y;
	float			z;
	float			w;
	
	Quat();
	Quat( float x, float y, float z, float w );
	
	void 			Set( float x, float y, float z, float w );
	
	float			operator[]( int index ) const;
	float& 			operator[]( int index );
	Quat			operator-() const;
	Quat& 		operator=( const Quat& a );
	Quat			operator+( const Quat& a ) const;
	Quat& 		operator+=( const Quat& a );
	Quat			operator-( const Quat& a ) const;
	Quat& 		operator-=( const Quat& a );
	Quat			operator*( const Quat& a ) const;
	Vector3			operator*( const Vector3& a ) const;
	Quat			operator*( float a ) const;
	Quat& 		operator*=( const Quat& a );
	Quat& 		operator*=( float a );
	
	friend Quat	operator*( const float a, const Quat& b );
	friend Vector3	operator*( const Vector3& a, const Quat& b );
	
	bool			Compare( const Quat& a ) const;						// exact compare, no epsilon
	bool			Compare( const Quat& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const Quat& a ) const;					// exact compare, no epsilon
	bool			operator!=(	const Quat& a ) const;					// exact compare, no epsilon
	
	Quat			Inverse() const;
	float			Length() const;
	Quat& 			Normalize();
	
	float			CalcW() const;
	int				GetDimension() const;
	
	Angles			ToAngles() const;
	Rotation		ToRotation() const;
	Matrix3			ToMat3() const;
	Matrix4			ToMat4() const;
	CMPQuat			ToCQuat() const;
	Vector3			ToAngularVelocity() const;
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	std::string 	ToString( int precision = 2 ) const;
	
	Quat& 		Slerp( const Quat& from, const Quat& to, float t );
	Quat& 		Lerp( const Quat& from, const Quat& to, const float t );
};

// A non-member slerp function allows constructing a const Quat object with the result of a slerp,
// but without having to explicity create a temporary Quat object.
Quat Slerp( const Quat& from, const Quat& to, const float t );

BUD_INLINE Quat::Quat()
{
}

BUD_INLINE Quat::Quat( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

BUD_INLINE float Quat::operator[]( int index ) const
{
	assert( ( index >= 0 ) && ( index < 4 ) );
	return ( &x )[ index ];
}

BUD_INLINE float& Quat::operator[]( int index )
{
	assert( ( index >= 0 ) && ( index < 4 ) );
	return ( &x )[ index ];
}

BUD_INLINE Quat Quat::operator-() const
{
	return Quat( -x, -y, -z, -w );
}

BUD_INLINE Quat& Quat::operator=( const Quat& a )
{
	x = a.x;
	y = a.y;
	z = a.z;
	w = a.w;
	
	return *this;
}

BUD_INLINE Quat Quat::operator+( const Quat& a ) const
{
	return Quat( x + a.x, y + a.y, z + a.z, w + a.w );
}

BUD_INLINE Quat& Quat::operator+=( const Quat& a )
{
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;
	
	return *this;
}

BUD_INLINE Quat Quat::operator-( const Quat& a ) const
{
	return Quat( x - a.x, y - a.y, z - a.z, w - a.w );
}

BUD_INLINE Quat& Quat::operator-=( const Quat& a )
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;
	
	return *this;
}

BUD_INLINE Quat Quat::operator*( const Quat& a ) const
{
	return Quat(	w * a.x + x * a.w + y * a.z - z * a.y,
					w * a.y + y * a.w + z * a.x - x * a.z,
					w * a.z + z * a.w + x * a.y - y * a.x,
					w * a.w - x * a.x - y * a.y - z * a.z );
}

BUD_INLINE Vector3 Quat::operator*( const Vector3& a ) const
{
#if 0
	// it's faster to do the conversion to a 3x3 matrix and multiply the vector by this 3x3 matrix
	return ( ToMat3() * a );
#else
	// result = this->Inverse() * Quat( a.x, a.y, a.z, 0.0f ) * (*this)
	float xxzz = x * x - z * z;
	float wwyy = w * w - y * y;
	
	float xw2 = x * w * 2.0f;
	float xy2 = x * y * 2.0f;
	float xz2 = x * z * 2.0f;
	float yw2 = y * w * 2.0f;
	float yz2 = y * z * 2.0f;
	float zw2 = z * w * 2.0f;
	
	return Vector3(
			   ( xxzz + wwyy ) * a.x		+ ( xy2 + zw2 ) * a.y		+ ( xz2 - yw2 ) * a.z,
			   ( xy2 - zw2 ) * a.x			+ ( y * y + w * w - x * x - z * z ) * a.y	+ ( yz2 + xw2 ) * a.z,
			   ( xz2 + yw2 ) * a.x			+ ( yz2 - xw2 ) * a.y		+ ( wwyy - xxzz ) * a.z
		   );
#endif
}

BUD_INLINE Quat Quat::operator*( float a ) const
{
	return Quat( x * a, y * a, z * a, w * a );
}

BUD_INLINE Quat operator*( const float a, const Quat& b )
{
	return b * a;
}

BUD_INLINE Vector3 operator*( const Vector3& a, const Quat& b )
{
	return b * a;
}

BUD_INLINE Quat& Quat::operator*=( const Quat& a )
{
	*this = *this * a;
	
	return *this;
}

BUD_INLINE Quat& Quat::operator*=( float a )
{
	x *= a;
	y *= a;
	z *= a;
	w *= a;
	
	return *this;
}

BUD_INLINE bool Quat::Compare( const Quat& a ) const
{
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) && ( w == a.w ) );
}

BUD_INLINE bool Quat::Compare( const Quat& a, const float epsilon ) const
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

BUD_INLINE bool Quat::operator==( const Quat& a ) const
{
	return Compare( a );
}

BUD_INLINE bool Quat::operator!=( const Quat& a ) const
{
	return !Compare( a );
}

BUD_INLINE void Quat::Set( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

BUD_INLINE Quat Quat::Inverse() const
{
	return Quat( -x, -y, -z, w );
}

BUD_INLINE float Quat::Length() const
{
	float len;
	
	len = x * x + y * y + z * z + w * w;
	return Math::Sqrt( len );
}

BUD_INLINE Quat& Quat::Normalize()
{
	float len;
	float ilength;
	
	len = this->Length();
	if( len )
	{
		ilength = 1 / len;
		x *= ilength;
		y *= ilength;
		z *= ilength;
		w *= ilength;
	}
	return *this;
}

BUD_INLINE float Quat::CalcW() const
{
	// take the absolute value because floating point rounding may cause the dot of x,y,z to be larger than 1
	return sqrt( fabs( 1.0f - ( x * x + y * y + z * z ) ) );
}

BUD_INLINE int Quat::GetDimension() const
{
	return 4;
}

BUD_INLINE const float* Quat::ToFloatPtr() const
{
	return &x;
}

BUD_INLINE float* Quat::ToFloatPtr()
{
	return &x;
}

/*
===============================================================================

	Specialization to get size of an Quat generically.

===============================================================================
*/
template<>
struct TupleSize< Quat >
{
	enum { value = 4 };
};

/*
===============================================================================

	Compressed quaternion

===============================================================================
*/

class CMPQuat
{
public:
	float			x;
	float			y;
	float			z;
	
	CMPQuat();
	CMPQuat( float x, float y, float z );
	
	void 			Set( float x, float y, float z );
	
	float			operator[]( int index ) const;
	float& 			operator[]( int index );
	
	bool			Compare( const CMPQuat& a ) const;						// exact compare, no epsilon
	bool			Compare( const CMPQuat& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const CMPQuat& a ) const;					// exact compare, no epsilon
	bool			operator!=(	const CMPQuat& a ) const;					// exact compare, no epsilon
	
	int				GetDimension() const;
	
	Angles			ToAngles() const;
	Rotation		ToRotation() const;
	Matrix3			ToMat3() const;
	Matrix4			ToMat4() const;
	Quat			ToQuat() const;
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	std::string 	ToString( int precision = 2 ) const;
};

BUD_INLINE CMPQuat::CMPQuat()
{
}

BUD_INLINE CMPQuat::CMPQuat( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

BUD_INLINE void CMPQuat::Set( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

BUD_INLINE float CMPQuat::operator[]( int index ) const
{
	assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &x )[ index ];
}

BUD_INLINE float& CMPQuat::operator[]( int index )
{
	assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &x )[ index ];
}

BUD_INLINE bool CMPQuat::Compare( const CMPQuat& a ) const
{
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) );
}

BUD_INLINE bool CMPQuat::Compare( const CMPQuat& a, const float epsilon ) const
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

BUD_INLINE bool CMPQuat::operator==( const CMPQuat& a ) const
{
	return Compare( a );
}

BUD_INLINE bool CMPQuat::operator!=( const CMPQuat& a ) const
{
	return !Compare( a );
}

BUD_INLINE int CMPQuat::GetDimension() const
{
	return 3;
}

BUD_INLINE Quat CMPQuat::ToQuat() const
{
	// take the absolute value because floating point rounding may cause the dot of x,y,z to be larger than 1
	return Quat( x, y, z, sqrt( fabs( 1.0f - ( x * x + y * y + z * z ) ) ) );
}

BUD_INLINE const float* CMPQuat::ToFloatPtr() const
{
	return &x;
}

BUD_INLINE float* CMPQuat::ToFloatPtr()
{
	return &x;
}

/*
===============================================================================

	Specialization to get size of an CMPQuat generically.

===============================================================================
*/
template<>
struct TupleSize< CMPQuat >
{
	enum { value = 3 };
};

#endif /* !__MATH_QUAT_H__ */
