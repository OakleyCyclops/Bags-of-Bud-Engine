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

#ifndef __MATH_MATRIX_H__
#define __MATH_MATRIX_H__

/*
===============================================================================

  Matrix classes, all matrices are row-major except budMat3

===============================================================================
*/

#define MATRIX_INVERSE_EPSILON		1e-14
#define MATRIX_EPSILON				1e-6

class budAngles;
class idQuat;
class idCQuat;
class budRotation;
class budMat4;

//===============================================================
//
//	budMat2 - 2x2 matrix
//
//===============================================================

class budMat2
{
public:
	budMat2();
	explicit budMat2( const budVec2& x, const budVec2& y );
	explicit budMat2( const float xx, const float xy, const float yx, const float yy );
	explicit budMat2( const float src[ 2 ][ 2 ] );
	
	const budVec2& 	operator[]( int index ) const;
	budVec2& 		operator[]( int index );
	budMat2			operator-() const;
	budMat2			operator*( const float a ) const;
	budVec2			operator*( const budVec2& vec ) const;
	budMat2			operator*( const budMat2& a ) const;
	budMat2			operator+( const budMat2& a ) const;
	budMat2			operator-( const budMat2& a ) const;
	budMat2& 		operator*=( const float a );
	budMat2& 		operator*=( const budMat2& a );
	budMat2& 		operator+=( const budMat2& a );
	budMat2& 		operator-=( const budMat2& a );
	
	friend budMat2	operator*( const float a, const budMat2& mat );
	friend budVec2	operator*( const budVec2& vec, const budMat2& mat );
	friend budVec2& 	operator*=( budVec2& vec, const budMat2& mat );
	
	bool			Compare( const budMat2& a ) const;						// exact compare, no epsilon
	bool			Compare( const budMat2& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==( const budMat2& a ) const;					// exact compare, no epsilon
	bool			operator!=( const budMat2& a ) const;					// exact compare, no epsilon
	
	void			Zero();
	void			Identity();
	bool			IsIdentity( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	
	float			Trace() const;
	float			Determinant() const;
	budMat2			Transpose() const;	// returns transpose
	budMat2& 		TransposeSelf();
	budMat2			Inverse() const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf();		// returns false if determinant is zero
	budMat2			InverseFast() const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf();	// returns false if determinant is zero
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	budVec2			mat[ 2 ];
};

extern budMat2 mat2_zero;
extern budMat2 mat2_identity;
#define mat2_default	mat2_identity

BUD_INLINE budMat2::budMat2()
{
}

BUD_INLINE budMat2::budMat2( const budVec2& x, const budVec2& y )
{
	mat[ 0 ].x = x.x;
	mat[ 0 ].y = x.y;
	mat[ 1 ].x = y.x;
	mat[ 1 ].y = y.y;
}

BUD_INLINE budMat2::budMat2( const float xx, const float xy, const float yx, const float yy )
{
	mat[ 0 ].x = xx;
	mat[ 0 ].y = xy;
	mat[ 1 ].x = yx;
	mat[ 1 ].y = yy;
}

BUD_INLINE budMat2::budMat2( const float src[ 2 ][ 2 ] )
{
	memcpy( mat, src, 2 * 2 * sizeof( float ) );
}

BUD_INLINE const budVec2& budMat2::operator[]( int index ) const
{
	//assert( ( index >= 0 ) && ( index < 2 ) );
	return mat[ index ];
}

BUD_INLINE budVec2& budMat2::operator[]( int index )
{
	//assert( ( index >= 0 ) && ( index < 2 ) );
	return mat[ index ];
}

BUD_INLINE budMat2 budMat2::operator-() const
{
	return budMat2(	-mat[0][0], -mat[0][1],
					-mat[1][0], -mat[1][1] );
}

BUD_INLINE budVec2 budMat2::operator*( const budVec2& vec ) const
{
	return budVec2(
			   mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y,
			   mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y );
}

BUD_INLINE budMat2 budMat2::operator*( const budMat2& a ) const
{
	return budMat2(
			   mat[0].x * a[0].x + mat[0].y * a[1].x,
			   mat[0].x * a[0].y + mat[0].y * a[1].y,
			   mat[1].x * a[0].x + mat[1].y * a[1].x,
			   mat[1].x * a[0].y + mat[1].y * a[1].y );
}

BUD_INLINE budMat2 budMat2::operator*( const float a ) const
{
	return budMat2(
			   mat[0].x * a, mat[0].y * a,
			   mat[1].x * a, mat[1].y * a );
}

BUD_INLINE budMat2 budMat2::operator+( const budMat2& a ) const
{
	return budMat2(
			   mat[0].x + a[0].x, mat[0].y + a[0].y,
			   mat[1].x + a[1].x, mat[1].y + a[1].y );
}

BUD_INLINE budMat2 budMat2::operator-( const budMat2& a ) const
{
	return budMat2(
			   mat[0].x - a[0].x, mat[0].y - a[0].y,
			   mat[1].x - a[1].x, mat[1].y - a[1].y );
}

BUD_INLINE budMat2& budMat2::operator*=( const float a )
{
	mat[0].x *= a;
	mat[0].y *= a;
	mat[1].x *= a;
	mat[1].y *= a;
	
	return *this;
}

BUD_INLINE budMat2& budMat2::operator*=( const budMat2& a )
{
	float x, y;
	x = mat[0].x;
	y = mat[0].y;
	mat[0].x = x * a[0].x + y * a[1].x;
	mat[0].y = x * a[0].y + y * a[1].y;
	x = mat[1].x;
	y = mat[1].y;
	mat[1].x = x * a[0].x + y * a[1].x;
	mat[1].y = x * a[0].y + y * a[1].y;
	return *this;
}

BUD_INLINE budMat2& budMat2::operator+=( const budMat2& a )
{
	mat[0].x += a[0].x;
	mat[0].y += a[0].y;
	mat[1].x += a[1].x;
	mat[1].y += a[1].y;
	
	return *this;
}

BUD_INLINE budMat2& budMat2::operator-=( const budMat2& a )
{
	mat[0].x -= a[0].x;
	mat[0].y -= a[0].y;
	mat[1].x -= a[1].x;
	mat[1].y -= a[1].y;
	
	return *this;
}

BUD_INLINE budVec2 operator*( const budVec2& vec, const budMat2& mat )
{
	return mat * vec;
}

BUD_INLINE budMat2 operator*( const float a, budMat2 const& mat )
{
	return mat * a;
}

BUD_INLINE budVec2& operator*=( budVec2& vec, const budMat2& mat )
{
	vec = mat * vec;
	return vec;
}

BUD_INLINE bool budMat2::Compare( const budMat2& a ) const
{
	if( mat[0].Compare( a[0] ) &&
			mat[1].Compare( a[1] ) )
	{
		return true;
	}
	return false;
}

BUD_INLINE bool budMat2::Compare( const budMat2& a, const float epsilon ) const
{
	if( mat[0].Compare( a[0], epsilon ) &&
			mat[1].Compare( a[1], epsilon ) )
	{
		return true;
	}
	return false;
}

BUD_INLINE bool budMat2::operator==( const budMat2& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budMat2::operator!=( const budMat2& a ) const
{
	return !Compare( a );
}

BUD_INLINE void budMat2::Zero()
{
	mat[0].Zero();
	mat[1].Zero();
}

BUD_INLINE void budMat2::Identity()
{
	*this = mat2_identity;
}

BUD_INLINE bool budMat2::IsIdentity( const float epsilon ) const
{
	return Compare( mat2_identity, epsilon );
}

BUD_INLINE bool budMat2::IsSymmetric( const float epsilon ) const
{
	return ( budMath::Fabs( mat[0][1] - mat[1][0] ) < epsilon );
}

BUD_INLINE bool budMat2::IsDiagonal( const float epsilon ) const
{
	if( budMath::Fabs( mat[0][1] ) > epsilon ||
			budMath::Fabs( mat[1][0] ) > epsilon )
	{
		return false;
	}
	return true;
}

BUD_INLINE float budMat2::Trace() const
{
	return ( mat[0][0] + mat[1][1] );
}

BUD_INLINE float budMat2::Determinant() const
{
	return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
}

BUD_INLINE budMat2 budMat2::Transpose() const
{
	return budMat2(	mat[0][0], mat[1][0],
					mat[0][1], mat[1][1] );
}

BUD_INLINE budMat2& budMat2::TransposeSelf()
{
	float tmp;
	
	tmp = mat[0][1];
	mat[0][1] = mat[1][0];
	mat[1][0] = tmp;
	
	return *this;
}

BUD_INLINE budMat2 budMat2::Inverse() const
{
	budMat2 invMat;
	
	invMat = *this;
	verify( invMat.InverseSelf() );
	return invMat;
}

BUD_INLINE budMat2 budMat2::InverseFast() const
{
	budMat2 invMat;
	
	invMat = *this;
	verify( invMat.InverseFastSelf() );
	return invMat;
}

BUD_INLINE int budMat2::GetDimension() const
{
	return 4;
}

BUD_INLINE const float* budMat2::ToFloatPtr() const
{
	return mat[0].ToFloatPtr();
}

BUD_INLINE float* budMat2::ToFloatPtr()
{
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	budMat3 - 3x3 matrix
//
//	NOTE:	matrix is column-major
//
//===============================================================

class budMat3
{
public:
	budMat3();
	explicit budMat3( const budVec3& x, const budVec3& y, const budVec3& z );
	explicit budMat3( const float xx, const float xy, const float xz, const float yx, const float yy, const float yz, const float zx, const float zy, const float zz );
	explicit budMat3( const float src[ 3 ][ 3 ] );
	
	const budVec3& 	operator[]( int index ) const;
	budVec3& 		operator[]( int index );
	budMat3			operator-() const;
	budMat3			operator*( const float a ) const;
	budVec3			operator*( const budVec3& vec ) const;
	budMat3			operator*( const budMat3& a ) const;
	budMat3			operator+( const budMat3& a ) const;
	budMat3			operator-( const budMat3& a ) const;
	budMat3& 		operator*=( const float a );
	budMat3& 		operator*=( const budMat3& a );
	budMat3& 		operator+=( const budMat3& a );
	budMat3& 		operator-=( const budMat3& a );
	
	friend budMat3	operator*( const float a, const budMat3& mat );
	friend budVec3	operator*( const budVec3& vec, const budMat3& mat );
	friend budVec3& 	operator*=( budVec3& vec, const budMat3& mat );
	
	bool			Compare( const budMat3& a ) const;						// exact compare, no epsilon
	bool			Compare( const budMat3& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==( const budMat3& a ) const;					// exact compare, no epsilon
	bool			operator!=( const budMat3& a ) const;					// exact compare, no epsilon
	
	void			Zero();
	void			Identity();
	bool			IsIdentity( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsRotated() const;
	
	void			ProjectVector( const budVec3& src, budVec3& dst ) const;
	void			UnprojectVector( const budVec3& src, budVec3& dst ) const;
	
	bool			FixDegeneracies();	// fix degenerate axial cases
	bool			FixDenormals();		// change tiny numbers to zero
	
	float			Trace() const;
	float			Determinant() const;
	budMat3			OrthoNormalize() const;
	budMat3& 		OrthoNormalizeSelf();
	budMat3			Transpose() const;	// returns transpose
	budMat3& 		TransposeSelf();
	budMat3			Inverse() const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf();		// returns false if determinant is zero
	budMat3			InverseFast() const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf();	// returns false if determinant is zero
	budMat3			TransposeMultiply( const budMat3& b ) const;
	
	budMat3			InertiaTranslate( const float mass, const budVec3& centerOfMass, const budVec3& translation ) const;
	budMat3& 		InertiaTranslateSelf( const float mass, const budVec3& centerOfMass, const budVec3& translation );
	budMat3			InertiaRotate( const budMat3& rotation ) const;
	budMat3& 		InertiaRotateSelf( const budMat3& rotation );
	
	int				GetDimension() const;
	
	budAngles		ToAngles() const;
	idQuat			ToQuat() const;
	idCQuat			ToCQuat() const;
	budRotation		ToRotation() const;
	budMat4			ToMat4() const;
	budVec3			ToAngularVelocity() const;
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
	friend void		TransposeMultiply( const budMat3& inv, const budMat3& b, budMat3& dst );
	friend budMat3	SkewSymmetric( budVec3 const& src );
	
private:
	budVec3			mat[ 3 ];
};

extern budMat3 mat3_zero;
extern budMat3 mat3_identity;
#define mat3_default	mat3_identity

BUD_INLINE budMat3::budMat3()
{
}

BUD_INLINE budMat3::budMat3( const budVec3& x, const budVec3& y, const budVec3& z )
{
	mat[ 0 ].x = x.x;
	mat[ 0 ].y = x.y;
	mat[ 0 ].z = x.z;
	mat[ 1 ].x = y.x;
	mat[ 1 ].y = y.y;
	mat[ 1 ].z = y.z;
	mat[ 2 ].x = z.x;
	mat[ 2 ].y = z.y;
	mat[ 2 ].z = z.z;
}

BUD_INLINE budMat3::budMat3( const float xx, const float xy, const float xz, const float yx, const float yy, const float yz, const float zx, const float zy, const float zz )
{
	mat[ 0 ].x = xx;
	mat[ 0 ].y = xy;
	mat[ 0 ].z = xz;
	mat[ 1 ].x = yx;
	mat[ 1 ].y = yy;
	mat[ 1 ].z = yz;
	mat[ 2 ].x = zx;
	mat[ 2 ].y = zy;
	mat[ 2 ].z = zz;
}

BUD_INLINE budMat3::budMat3( const float src[ 3 ][ 3 ] )
{
	memcpy( mat, src, 3 * 3 * sizeof( float ) );
}

BUD_INLINE const budVec3& budMat3::operator[]( int index ) const
{
	//assert( ( index >= 0 ) && ( index < 3 ) );
	return mat[ index ];
}

BUD_INLINE budVec3& budMat3::operator[]( int index )
{
	//assert( ( index >= 0 ) && ( index < 3 ) );
	return mat[ index ];
}

BUD_INLINE budMat3 budMat3::operator-() const
{
	return budMat3(	-mat[0][0], -mat[0][1], -mat[0][2],
					-mat[1][0], -mat[1][1], -mat[1][2],
					-mat[2][0], -mat[2][1], -mat[2][2] );
}

BUD_INLINE budVec3 budMat3::operator*( const budVec3& vec ) const
{
	return budVec3(
			   mat[ 0 ].x * vec.x + mat[ 1 ].x * vec.y + mat[ 2 ].x * vec.z,
			   mat[ 0 ].y * vec.x + mat[ 1 ].y * vec.y + mat[ 2 ].y * vec.z,
			   mat[ 0 ].z * vec.x + mat[ 1 ].z * vec.y + mat[ 2 ].z * vec.z );
}

BUD_INLINE budMat3 budMat3::operator*( const budMat3& a ) const
{
	int i, j;
	const float* m1Ptr, *m2Ptr;
	float* dstPtr;
	budMat3 dst;
	
	m1Ptr = reinterpret_cast<const float*>( this );
	m2Ptr = reinterpret_cast<const float*>( &a );
	dstPtr = reinterpret_cast<float*>( &dst );
	
	for( i = 0; i < 3; i++ )
	{
		for( j = 0; j < 3; j++ )
		{
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 3 + j ]
					  + m1Ptr[1] * m2Ptr[ 1 * 3 + j ]
					  + m1Ptr[2] * m2Ptr[ 2 * 3 + j ];
			dstPtr++;
		}
		m1Ptr += 3;
	}
	return dst;
}

BUD_INLINE budMat3 budMat3::operator*( const float a ) const
{
	return budMat3(
			   mat[0].x * a, mat[0].y * a, mat[0].z * a,
			   mat[1].x * a, mat[1].y * a, mat[1].z * a,
			   mat[2].x * a, mat[2].y * a, mat[2].z * a );
}

BUD_INLINE budMat3 budMat3::operator+( const budMat3& a ) const
{
	return budMat3(
			   mat[0].x + a[0].x, mat[0].y + a[0].y, mat[0].z + a[0].z,
			   mat[1].x + a[1].x, mat[1].y + a[1].y, mat[1].z + a[1].z,
			   mat[2].x + a[2].x, mat[2].y + a[2].y, mat[2].z + a[2].z );
}

BUD_INLINE budMat3 budMat3::operator-( const budMat3& a ) const
{
	return budMat3(
			   mat[0].x - a[0].x, mat[0].y - a[0].y, mat[0].z - a[0].z,
			   mat[1].x - a[1].x, mat[1].y - a[1].y, mat[1].z - a[1].z,
			   mat[2].x - a[2].x, mat[2].y - a[2].y, mat[2].z - a[2].z );
}

BUD_INLINE budMat3& budMat3::operator*=( const float a )
{
	mat[0].x *= a;
	mat[0].y *= a;
	mat[0].z *= a;
	mat[1].x *= a;
	mat[1].y *= a;
	mat[1].z *= a;
	mat[2].x *= a;
	mat[2].y *= a;
	mat[2].z *= a;
	
	return *this;
}

BUD_INLINE budMat3& budMat3::operator*=( const budMat3& a )
{
	int i, j;
	const float* m2Ptr;
	float* m1Ptr, dst[3];
	
	m1Ptr = reinterpret_cast<float*>( this );
	m2Ptr = reinterpret_cast<const float*>( &a );
	
	for( i = 0; i < 3; i++ )
	{
		for( j = 0; j < 3; j++ )
		{
			dst[j]  = m1Ptr[0] * m2Ptr[ 0 * 3 + j ]
					  + m1Ptr[1] * m2Ptr[ 1 * 3 + j ]
					  + m1Ptr[2] * m2Ptr[ 2 * 3 + j ];
		}
		m1Ptr[0] = dst[0];
		m1Ptr[1] = dst[1];
		m1Ptr[2] = dst[2];
		m1Ptr += 3;
	}
	return *this;
}

BUD_INLINE budMat3& budMat3::operator+=( const budMat3& a )
{
	mat[0].x += a[0].x;
	mat[0].y += a[0].y;
	mat[0].z += a[0].z;
	mat[1].x += a[1].x;
	mat[1].y += a[1].y;
	mat[1].z += a[1].z;
	mat[2].x += a[2].x;
	mat[2].y += a[2].y;
	mat[2].z += a[2].z;
	
	return *this;
}

BUD_INLINE budMat3& budMat3::operator-=( const budMat3& a )
{
	mat[0].x -= a[0].x;
	mat[0].y -= a[0].y;
	mat[0].z -= a[0].z;
	mat[1].x -= a[1].x;
	mat[1].y -= a[1].y;
	mat[1].z -= a[1].z;
	mat[2].x -= a[2].x;
	mat[2].y -= a[2].y;
	mat[2].z -= a[2].z;
	
	return *this;
}

BUD_INLINE budVec3 operator*( const budVec3& vec, const budMat3& mat )
{
	return mat * vec;
}

BUD_INLINE budMat3 operator*( const float a, const budMat3& mat )
{
	return mat * a;
}

BUD_INLINE budVec3& operator*=( budVec3& vec, const budMat3& mat )
{
	float x = mat[ 0 ].x * vec.x + mat[ 1 ].x * vec.y + mat[ 2 ].x * vec.z;
	float y = mat[ 0 ].y * vec.x + mat[ 1 ].y * vec.y + mat[ 2 ].y * vec.z;
	vec.z = mat[ 0 ].z * vec.x + mat[ 1 ].z * vec.y + mat[ 2 ].z * vec.z;
	vec.x = x;
	vec.y = y;
	return vec;
}

BUD_INLINE bool budMat3::Compare( const budMat3& a ) const
{
	if( mat[0].Compare( a[0] ) &&
			mat[1].Compare( a[1] ) &&
			mat[2].Compare( a[2] ) )
	{
		return true;
	}
	return false;
}

BUD_INLINE bool budMat3::Compare( const budMat3& a, const float epsilon ) const
{
	if( mat[0].Compare( a[0], epsilon ) &&
			mat[1].Compare( a[1], epsilon ) &&
			mat[2].Compare( a[2], epsilon ) )
	{
		return true;
	}
	return false;
}

BUD_INLINE bool budMat3::operator==( const budMat3& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budMat3::operator!=( const budMat3& a ) const
{
	return !Compare( a );
}

BUD_INLINE void budMat3::Zero()
{
	memset( mat, 0, sizeof( budMat3 ) );
}

BUD_INLINE void budMat3::Identity()
{
	*this = mat3_identity;
}

BUD_INLINE bool budMat3::IsIdentity( const float epsilon ) const
{
	return Compare( mat3_identity, epsilon );
}

BUD_INLINE bool budMat3::IsSymmetric( const float epsilon ) const
{
	if( budMath::Fabs( mat[0][1] - mat[1][0] ) > epsilon )
	{
		return false;
	}
	if( budMath::Fabs( mat[0][2] - mat[2][0] ) > epsilon )
	{
		return false;
	}
	if( budMath::Fabs( mat[1][2] - mat[2][1] ) > epsilon )
	{
		return false;
	}
	return true;
}

BUD_INLINE bool budMat3::IsDiagonal( const float epsilon ) const
{
	if( budMath::Fabs( mat[0][1] ) > epsilon ||
			budMath::Fabs( mat[0][2] ) > epsilon ||
			budMath::Fabs( mat[1][0] ) > epsilon ||
			budMath::Fabs( mat[1][2] ) > epsilon ||
			budMath::Fabs( mat[2][0] ) > epsilon ||
			budMath::Fabs( mat[2][1] ) > epsilon )
	{
		return false;
	}
	return true;
}

BUD_INLINE bool budMat3::IsRotated() const
{
	return !Compare( mat3_identity );
}

BUD_INLINE void budMat3::ProjectVector( const budVec3& src, budVec3& dst ) const
{
	dst.x = src * mat[ 0 ];
	dst.y = src * mat[ 1 ];
	dst.z = src * mat[ 2 ];
}

BUD_INLINE void budMat3::UnprojectVector( const budVec3& src, budVec3& dst ) const
{
	dst = mat[ 0 ] * src.x + mat[ 1 ] * src.y + mat[ 2 ] * src.z;
}

BUD_INLINE bool budMat3::FixDegeneracies()
{
	bool r = mat[0].FixDegenerateNormal();
	r |= mat[1].FixDegenerateNormal();
	r |= mat[2].FixDegenerateNormal();
	return r;
}

BUD_INLINE bool budMat3::FixDenormals()
{
	bool r = mat[0].FixDenormals();
	r |= mat[1].FixDenormals();
	r |= mat[2].FixDenormals();
	return r;
}

BUD_INLINE float budMat3::Trace() const
{
	return ( mat[0][0] + mat[1][1] + mat[2][2] );
}

BUD_INLINE budMat3 budMat3::OrthoNormalize() const
{
	budMat3 ortho;
	
	ortho = *this;
	ortho[ 0 ].Normalize();
	ortho[ 2 ].Cross( mat[ 0 ], mat[ 1 ] );
	ortho[ 2 ].Normalize();
	ortho[ 1 ].Cross( mat[ 2 ], mat[ 0 ] );
	ortho[ 1 ].Normalize();
	return ortho;
}

BUD_INLINE budMat3& budMat3::OrthoNormalizeSelf()
{
	mat[ 0 ].Normalize();
	mat[ 2 ].Cross( mat[ 0 ], mat[ 1 ] );
	mat[ 2 ].Normalize();
	mat[ 1 ].Cross( mat[ 2 ], mat[ 0 ] );
	mat[ 1 ].Normalize();
	return *this;
}

BUD_INLINE budMat3 budMat3::Transpose() const
{
	return budMat3(	mat[0][0], mat[1][0], mat[2][0],
					mat[0][1], mat[1][1], mat[2][1],
					mat[0][2], mat[1][2], mat[2][2] );
}

BUD_INLINE budMat3& budMat3::TransposeSelf()
{
	float tmp0, tmp1, tmp2;
	
	tmp0 = mat[0][1];
	mat[0][1] = mat[1][0];
	mat[1][0] = tmp0;
	tmp1 = mat[0][2];
	mat[0][2] = mat[2][0];
	mat[2][0] = tmp1;
	tmp2 = mat[1][2];
	mat[1][2] = mat[2][1];
	mat[2][1] = tmp2;
	
	return *this;
}

BUD_INLINE budMat3 budMat3::Inverse() const
{
	budMat3 invMat;
	
	invMat = *this;
	verify( invMat.InverseSelf() );
	return invMat;
}

BUD_INLINE budMat3 budMat3::InverseFast() const
{
	budMat3 invMat;
	
	invMat = *this;
	verify( invMat.InverseFastSelf() );
	return invMat;
}

BUD_INLINE budMat3 budMat3::TransposeMultiply( const budMat3& b ) const
{
	return budMat3(	mat[0].x * b[0].x + mat[1].x * b[1].x + mat[2].x * b[2].x,
					mat[0].x * b[0].y + mat[1].x * b[1].y + mat[2].x * b[2].y,
					mat[0].x * b[0].z + mat[1].x * b[1].z + mat[2].x * b[2].z,
					mat[0].y * b[0].x + mat[1].y * b[1].x + mat[2].y * b[2].x,
					mat[0].y * b[0].y + mat[1].y * b[1].y + mat[2].y * b[2].y,
					mat[0].y * b[0].z + mat[1].y * b[1].z + mat[2].y * b[2].z,
					mat[0].z * b[0].x + mat[1].z * b[1].x + mat[2].z * b[2].x,
					mat[0].z * b[0].y + mat[1].z * b[1].y + mat[2].z * b[2].y,
					mat[0].z * b[0].z + mat[1].z * b[1].z + mat[2].z * b[2].z );
}

BUD_INLINE void TransposeMultiply( const budMat3& transpose, const budMat3& b, budMat3& dst )
{
	dst[0].x = transpose[0].x * b[0].x + transpose[1].x * b[1].x + transpose[2].x * b[2].x;
	dst[0].y = transpose[0].x * b[0].y + transpose[1].x * b[1].y + transpose[2].x * b[2].y;
	dst[0].z = transpose[0].x * b[0].z + transpose[1].x * b[1].z + transpose[2].x * b[2].z;
	dst[1].x = transpose[0].y * b[0].x + transpose[1].y * b[1].x + transpose[2].y * b[2].x;
	dst[1].y = transpose[0].y * b[0].y + transpose[1].y * b[1].y + transpose[2].y * b[2].y;
	dst[1].z = transpose[0].y * b[0].z + transpose[1].y * b[1].z + transpose[2].y * b[2].z;
	dst[2].x = transpose[0].z * b[0].x + transpose[1].z * b[1].x + transpose[2].z * b[2].x;
	dst[2].y = transpose[0].z * b[0].y + transpose[1].z * b[1].y + transpose[2].z * b[2].y;
	dst[2].z = transpose[0].z * b[0].z + transpose[1].z * b[1].z + transpose[2].z * b[2].z;
}

BUD_INLINE budMat3 SkewSymmetric( budVec3 const& src )
{
	return budMat3( 0.0f, -src.z,  src.y, src.z,   0.0f, -src.x, -src.y,  src.x,   0.0f );
}

BUD_INLINE int budMat3::GetDimension() const
{
	return 9;
}

BUD_INLINE const float* budMat3::ToFloatPtr() const
{
	return mat[0].ToFloatPtr();
}

BUD_INLINE float* budMat3::ToFloatPtr()
{
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	budMat4 - 4x4 matrix
//
//===============================================================

class budMat4
{
public:
	budMat4();
	explicit budMat4( const budVec4& x, const budVec4& y, const budVec4& z, const budVec4& w );
	explicit budMat4( const float xx, const float xy, const float xz, const float xw,
					 const float yx, const float yy, const float yz, const float yw,
					 const float zx, const float zy, const float zz, const float zw,
					 const float wx, const float wy, const float wz, const float ww );
	explicit budMat4( const budMat3& rotation, const budVec3& translation );
	explicit budMat4( const float src[ 4 ][ 4 ] );
	
	const budVec4& 	operator[]( int index ) const;
	budVec4& 		operator[]( int index );
	budMat4			operator*( const float a ) const;
	budVec4			operator*( const budVec4& vec ) const;
	budVec3			operator*( const budVec3& vec ) const;
	budMat4			operator*( const budMat4& a ) const;
	budMat4			operator+( const budMat4& a ) const;
	budMat4			operator-( const budMat4& a ) const;
	budMat4& 		operator*=( const float a );
	budMat4& 		operator*=( const budMat4& a );
	budMat4& 		operator+=( const budMat4& a );
	budMat4& 		operator-=( const budMat4& a );
	
	friend budMat4	operator*( const float a, const budMat4& mat );
	friend budVec4	operator*( const budVec4& vec, const budMat4& mat );
	friend budVec3	operator*( const budVec3& vec, const budMat4& mat );
	friend budVec4& 	operator*=( budVec4& vec, const budMat4& mat );
	friend budVec3& 	operator*=( budVec3& vec, const budMat4& mat );
	
	bool			Compare( const budMat4& a ) const;						// exact compare, no epsilon
	bool			Compare( const budMat4& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==( const budMat4& a ) const;					// exact compare, no epsilon
	bool			operator!=( const budMat4& a ) const;					// exact compare, no epsilon
	
	void			Zero();
	void			Identity();
	bool			IsIdentity( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsRotated() const;
	
	void			ProjectVector( const budVec4& src, budVec4& dst ) const;
	void			UnprojectVector( const budVec4& src, budVec4& dst ) const;
	
	float			Trace() const;
	float			Determinant() const;
	budMat4			Transpose() const;	// returns transpose
	budMat4& 		TransposeSelf();
	budMat4			Inverse() const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf();		// returns false if determinant is zero
	budMat4			InverseFast() const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf();	// returns false if determinant is zero
	budMat4			TransposeMultiply( const budMat4& b ) const;
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	budVec4			mat[ 4 ];
};

extern budMat4 mat4_zero;
extern budMat4 mat4_identity;
#define mat4_default	mat4_identity

BUD_INLINE budMat4::budMat4()
{
}

BUD_INLINE budMat4::budMat4( const budVec4& x, const budVec4& y, const budVec4& z, const budVec4& w )
{
	mat[ 0 ] = x;
	mat[ 1 ] = y;
	mat[ 2 ] = z;
	mat[ 3 ] = w;
}

BUD_INLINE budMat4::budMat4( const float xx, const float xy, const float xz, const float xw,
						  const float yx, const float yy, const float yz, const float yw,
						  const float zx, const float zy, const float zz, const float zw,
						  const float wx, const float wy, const float wz, const float ww )
{
	mat[0][0] = xx;
	mat[0][1] = xy;
	mat[0][2] = xz;
	mat[0][3] = xw;
	mat[1][0] = yx;
	mat[1][1] = yy;
	mat[1][2] = yz;
	mat[1][3] = yw;
	mat[2][0] = zx;
	mat[2][1] = zy;
	mat[2][2] = zz;
	mat[2][3] = zw;
	mat[3][0] = wx;
	mat[3][1] = wy;
	mat[3][2] = wz;
	mat[3][3] = ww;
}

BUD_INLINE budMat4::budMat4( const budMat3& rotation, const budVec3& translation )
{
	// NOTE: budMat3 is transposed because it is column-major
	mat[ 0 ][ 0 ] = rotation[0][0];
	mat[ 0 ][ 1 ] = rotation[1][0];
	mat[ 0 ][ 2 ] = rotation[2][0];
	mat[ 0 ][ 3 ] = translation[0];
	mat[ 1 ][ 0 ] = rotation[0][1];
	mat[ 1 ][ 1 ] = rotation[1][1];
	mat[ 1 ][ 2 ] = rotation[2][1];
	mat[ 1 ][ 3 ] = translation[1];
	mat[ 2 ][ 0 ] = rotation[0][2];
	mat[ 2 ][ 1 ] = rotation[1][2];
	mat[ 2 ][ 2 ] = rotation[2][2];
	mat[ 2 ][ 3 ] = translation[2];
	mat[ 3 ][ 0 ] = 0.0f;
	mat[ 3 ][ 1 ] = 0.0f;
	mat[ 3 ][ 2 ] = 0.0f;
	mat[ 3 ][ 3 ] = 1.0f;
}

BUD_INLINE budMat4::budMat4( const float src[ 4 ][ 4 ] )
{
	memcpy( mat, src, 4 * 4 * sizeof( float ) );
}

BUD_INLINE const budVec4& budMat4::operator[]( int index ) const
{
	//assert( ( index >= 0 ) && ( index < 4 ) );
	return mat[ index ];
}

BUD_INLINE budVec4& budMat4::operator[]( int index )
{
	//assert( ( index >= 0 ) && ( index < 4 ) );
	return mat[ index ];
}

BUD_INLINE budMat4 budMat4::operator*( const float a ) const
{
	return budMat4(
			   mat[0].x * a, mat[0].y * a, mat[0].z * a, mat[0].w * a,
			   mat[1].x * a, mat[1].y * a, mat[1].z * a, mat[1].w * a,
			   mat[2].x * a, mat[2].y * a, mat[2].z * a, mat[2].w * a,
			   mat[3].x * a, mat[3].y * a, mat[3].z * a, mat[3].w * a );
}

BUD_INLINE budVec4 budMat4::operator*( const budVec4& vec ) const
{
	return budVec4(
			   mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w * vec.w,
			   mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w * vec.w,
			   mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w * vec.w,
			   mat[ 3 ].x * vec.x + mat[ 3 ].y * vec.y + mat[ 3 ].z * vec.z + mat[ 3 ].w * vec.w );
}

BUD_INLINE budVec3 budMat4::operator*( const budVec3& vec ) const
{
	float s = mat[ 3 ].x * vec.x + mat[ 3 ].y * vec.y + mat[ 3 ].z * vec.z + mat[ 3 ].w;
	if( s == 0.0f )
	{
		return budVec3( 0.0f, 0.0f, 0.0f );
	}
	if( s == 1.0f )
	{
		return budVec3(
				   mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w,
				   mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w,
				   mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w );
	}
	else
	{
		float invS = 1.0f / s;
		return budVec3(
				   ( mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w ) * invS,
				   ( mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w ) * invS,
				   ( mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w ) * invS );
	}
}

BUD_INLINE budMat4 budMat4::operator*( const budMat4& a ) const
{
	int i, j;
	const float* m1Ptr, *m2Ptr;
	float* dstPtr;
	budMat4 dst;
	
	m1Ptr = reinterpret_cast<const float*>( this );
	m2Ptr = reinterpret_cast<const float*>( &a );
	dstPtr = reinterpret_cast<float*>( &dst );
	
	for( i = 0; i < 4; i++ )
	{
		for( j = 0; j < 4; j++ )
		{
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 4 + j ]
					  + m1Ptr[1] * m2Ptr[ 1 * 4 + j ]
					  + m1Ptr[2] * m2Ptr[ 2 * 4 + j ]
					  + m1Ptr[3] * m2Ptr[ 3 * 4 + j ];
			dstPtr++;
		}
		m1Ptr += 4;
	}
	return dst;
}

BUD_INLINE budMat4 budMat4::operator+( const budMat4& a ) const
{
	return budMat4(
			   mat[0].x + a[0].x, mat[0].y + a[0].y, mat[0].z + a[0].z, mat[0].w + a[0].w,
			   mat[1].x + a[1].x, mat[1].y + a[1].y, mat[1].z + a[1].z, mat[1].w + a[1].w,
			   mat[2].x + a[2].x, mat[2].y + a[2].y, mat[2].z + a[2].z, mat[2].w + a[2].w,
			   mat[3].x + a[3].x, mat[3].y + a[3].y, mat[3].z + a[3].z, mat[3].w + a[3].w );
}

BUD_INLINE budMat4 budMat4::operator-( const budMat4& a ) const
{
	return budMat4(
			   mat[0].x - a[0].x, mat[0].y - a[0].y, mat[0].z - a[0].z, mat[0].w - a[0].w,
			   mat[1].x - a[1].x, mat[1].y - a[1].y, mat[1].z - a[1].z, mat[1].w - a[1].w,
			   mat[2].x - a[2].x, mat[2].y - a[2].y, mat[2].z - a[2].z, mat[2].w - a[2].w,
			   mat[3].x - a[3].x, mat[3].y - a[3].y, mat[3].z - a[3].z, mat[3].w - a[3].w );
}

BUD_INLINE budMat4& budMat4::operator*=( const float a )
{
	mat[0].x *= a;
	mat[0].y *= a;
	mat[0].z *= a;
	mat[0].w *= a;
	mat[1].x *= a;
	mat[1].y *= a;
	mat[1].z *= a;
	mat[1].w *= a;
	mat[2].x *= a;
	mat[2].y *= a;
	mat[2].z *= a;
	mat[2].w *= a;
	mat[3].x *= a;
	mat[3].y *= a;
	mat[3].z *= a;
	mat[3].w *= a;
	return *this;
}

BUD_INLINE budMat4& budMat4::operator*=( const budMat4& a )
{
	*this = ( *this ) * a;
	return *this;
}

BUD_INLINE budMat4& budMat4::operator+=( const budMat4& a )
{
	mat[0].x += a[0].x;
	mat[0].y += a[0].y;
	mat[0].z += a[0].z;
	mat[0].w += a[0].w;
	mat[1].x += a[1].x;
	mat[1].y += a[1].y;
	mat[1].z += a[1].z;
	mat[1].w += a[1].w;
	mat[2].x += a[2].x;
	mat[2].y += a[2].y;
	mat[2].z += a[2].z;
	mat[2].w += a[2].w;
	mat[3].x += a[3].x;
	mat[3].y += a[3].y;
	mat[3].z += a[3].z;
	mat[3].w += a[3].w;
	return *this;
}

BUD_INLINE budMat4& budMat4::operator-=( const budMat4& a )
{
	mat[0].x -= a[0].x;
	mat[0].y -= a[0].y;
	mat[0].z -= a[0].z;
	mat[0].w -= a[0].w;
	mat[1].x -= a[1].x;
	mat[1].y -= a[1].y;
	mat[1].z -= a[1].z;
	mat[1].w -= a[1].w;
	mat[2].x -= a[2].x;
	mat[2].y -= a[2].y;
	mat[2].z -= a[2].z;
	mat[2].w -= a[2].w;
	mat[3].x -= a[3].x;
	mat[3].y -= a[3].y;
	mat[3].z -= a[3].z;
	mat[3].w -= a[3].w;
	return *this;
}

BUD_INLINE budMat4 operator*( const float a, const budMat4& mat )
{
	return mat * a;
}

BUD_INLINE budVec4 operator*( const budVec4& vec, const budMat4& mat )
{
	return mat * vec;
}

BUD_INLINE budVec3 operator*( const budVec3& vec, const budMat4& mat )
{
	return mat * vec;
}

BUD_INLINE budVec4& operator*=( budVec4& vec, const budMat4& mat )
{
	vec = mat * vec;
	return vec;
}

BUD_INLINE budVec3& operator*=( budVec3& vec, const budMat4& mat )
{
	vec = mat * vec;
	return vec;
}

BUD_INLINE bool budMat4::Compare( const budMat4& a ) const
{
	dword i;
	const float* ptr1, *ptr2;
	
	ptr1 = reinterpret_cast<const float*>( mat );
	ptr2 = reinterpret_cast<const float*>( a.mat );
	for( i = 0; i < 4 * 4; i++ )
	{
		if( ptr1[i] != ptr2[i] )
		{
			return false;
		}
	}
	return true;
}

BUD_INLINE bool budMat4::Compare( const budMat4& a, const float epsilon ) const
{
	dword i;
	const float* ptr1, *ptr2;
	
	ptr1 = reinterpret_cast<const float*>( mat );
	ptr2 = reinterpret_cast<const float*>( a.mat );
	for( i = 0; i < 4 * 4; i++ )
	{
		if( budMath::Fabs( ptr1[i] - ptr2[i] ) > epsilon )
		{
			return false;
		}
	}
	return true;
}

BUD_INLINE bool budMat4::operator==( const budMat4& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budMat4::operator!=( const budMat4& a ) const
{
	return !Compare( a );
}

BUD_INLINE void budMat4::Zero()
{
	memset( mat, 0, sizeof( budMat4 ) );
}

BUD_INLINE void budMat4::Identity()
{
	*this = mat4_identity;
}

BUD_INLINE bool budMat4::IsIdentity( const float epsilon ) const
{
	return Compare( mat4_identity, epsilon );
}

BUD_INLINE bool budMat4::IsSymmetric( const float epsilon ) const
{
	for( int i = 1; i < 4; i++ )
	{
		for( int j = 0; j < i; j++ )
		{
			if( budMath::Fabs( mat[i][j] - mat[j][i] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE bool budMat4::IsDiagonal( const float epsilon ) const
{
	for( int i = 0; i < 4; i++ )
	{
		for( int j = 0; j < 4; j++ )
		{
			if( i != j && budMath::Fabs( mat[i][j] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE bool budMat4::IsRotated() const
{
	if( !mat[ 0 ][ 1 ] && !mat[ 0 ][ 2 ] &&
			!mat[ 1 ][ 0 ] && !mat[ 1 ][ 2 ] &&
			!mat[ 2 ][ 0 ] && !mat[ 2 ][ 1 ] )
	{
		return false;
	}
	return true;
}

BUD_INLINE void budMat4::ProjectVector( const budVec4& src, budVec4& dst ) const
{
	dst.x = src * mat[ 0 ];
	dst.y = src * mat[ 1 ];
	dst.z = src * mat[ 2 ];
	dst.w = src * mat[ 3 ];
}

BUD_INLINE void budMat4::UnprojectVector( const budVec4& src, budVec4& dst ) const
{
	dst = mat[ 0 ] * src.x + mat[ 1 ] * src.y + mat[ 2 ] * src.z + mat[ 3 ] * src.w;
}

BUD_INLINE float budMat4::Trace() const
{
	return ( mat[0][0] + mat[1][1] + mat[2][2] + mat[3][3] );
}

BUD_INLINE budMat4 budMat4::Inverse() const
{
	budMat4 invMat;
	
	invMat = *this;
	verify( invMat.InverseSelf() );
	return invMat;
}

BUD_INLINE budMat4 budMat4::InverseFast() const
{
	budMat4 invMat;
	
	invMat = *this;
	verify( invMat.InverseFastSelf() );
	return invMat;
}

BUD_INLINE budMat4 budMat3::ToMat4() const
{
	// NOTE: budMat3 is transposed because it is column-major
	return budMat4(	mat[0][0],	mat[1][0],	mat[2][0],	0.0f,
					mat[0][1],	mat[1][1],	mat[2][1],	0.0f,
					mat[0][2],	mat[1][2],	mat[2][2],	0.0f,
					0.0f,		0.0f,		0.0f,		1.0f );
}

BUD_INLINE int budMat4::GetDimension() const
{
	return 16;
}

BUD_INLINE const float* budMat4::ToFloatPtr() const
{
	return mat[0].ToFloatPtr();
}

BUD_INLINE float* budMat4::ToFloatPtr()
{
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	budMat5 - 5x5 matrix
//
//===============================================================

class budMat5
{
public:
	budMat5();
	explicit budMat5( const budVec5& v0, const budVec5& v1, const budVec5& v2, const budVec5& v3, const budVec5& v4 );
	explicit budMat5( const float src[ 5 ][ 5 ] );
	
	const budVec5& 	operator[]( int index ) const;
	budVec5& 		operator[]( int index );
	budMat5			operator*( const float a ) const;
	budVec5			operator*( const budVec5& vec ) const;
	budMat5			operator*( const budMat5& a ) const;
	budMat5			operator+( const budMat5& a ) const;
	budMat5			operator-( const budMat5& a ) const;
	budMat5& 		operator*=( const float a );
	budMat5& 		operator*=( const budMat5& a );
	budMat5& 		operator+=( const budMat5& a );
	budMat5& 		operator-=( const budMat5& a );
	
	friend budMat5	operator*( const float a, const budMat5& mat );
	friend budVec5	operator*( const budVec5& vec, const budMat5& mat );
	friend budVec5& 	operator*=( budVec5& vec, const budMat5& mat );
	
	bool			Compare( const budMat5& a ) const;						// exact compare, no epsilon
	bool			Compare( const budMat5& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==( const budMat5& a ) const;					// exact compare, no epsilon
	bool			operator!=( const budMat5& a ) const;					// exact compare, no epsilon
	
	void			Zero();
	void			Identity();
	bool			IsIdentity( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	
	float			Trace() const;
	float			Determinant() const;
	budMat5			Transpose() const;	// returns transpose
	budMat5& 		TransposeSelf();
	budMat5			Inverse() const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf();		// returns false if determinant is zero
	budMat5			InverseFast() const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf();	// returns false if determinant is zero
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	budVec5			mat[ 5 ];
};

extern budMat5 mat5_zero;
extern budMat5 mat5_identity;
#define mat5_default	mat5_identity

BUD_INLINE budMat5::budMat5()
{
}

BUD_INLINE budMat5::budMat5( const float src[ 5 ][ 5 ] )
{
	memcpy( mat, src, 5 * 5 * sizeof( float ) );
}

BUD_INLINE budMat5::budMat5( const budVec5& v0, const budVec5& v1, const budVec5& v2, const budVec5& v3, const budVec5& v4 )
{
	mat[0] = v0;
	mat[1] = v1;
	mat[2] = v2;
	mat[3] = v3;
	mat[4] = v4;
}

BUD_INLINE const budVec5& budMat5::operator[]( int index ) const
{
	//assert( ( index >= 0 ) && ( index < 5 ) );
	return mat[ index ];
}

BUD_INLINE budVec5& budMat5::operator[]( int index )
{
	//assert( ( index >= 0 ) && ( index < 5 ) );
	return mat[ index ];
}

BUD_INLINE budMat5 budMat5::operator*( const budMat5& a ) const
{
	int i, j;
	const float* m1Ptr, *m2Ptr;
	float* dstPtr;
	budMat5 dst;
	
	m1Ptr = reinterpret_cast<const float*>( this );
	m2Ptr = reinterpret_cast<const float*>( &a );
	dstPtr = reinterpret_cast<float*>( &dst );
	
	for( i = 0; i < 5; i++ )
	{
		for( j = 0; j < 5; j++ )
		{
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 5 + j ]
					  + m1Ptr[1] * m2Ptr[ 1 * 5 + j ]
					  + m1Ptr[2] * m2Ptr[ 2 * 5 + j ]
					  + m1Ptr[3] * m2Ptr[ 3 * 5 + j ]
					  + m1Ptr[4] * m2Ptr[ 4 * 5 + j ];
			dstPtr++;
		}
		m1Ptr += 5;
	}
	return dst;
}

BUD_INLINE budMat5 budMat5::operator*( const float a ) const
{
	return budMat5(
			   budVec5( mat[0][0] * a, mat[0][1] * a, mat[0][2] * a, mat[0][3] * a, mat[0][4] * a ),
			   budVec5( mat[1][0] * a, mat[1][1] * a, mat[1][2] * a, mat[1][3] * a, mat[1][4] * a ),
			   budVec5( mat[2][0] * a, mat[2][1] * a, mat[2][2] * a, mat[2][3] * a, mat[2][4] * a ),
			   budVec5( mat[3][0] * a, mat[3][1] * a, mat[3][2] * a, mat[3][3] * a, mat[3][4] * a ),
			   budVec5( mat[4][0] * a, mat[4][1] * a, mat[4][2] * a, mat[4][3] * a, mat[4][4] * a ) );
}

BUD_INLINE budVec5 budMat5::operator*( const budVec5& vec ) const
{
	return budVec5(
			   mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2] + mat[0][3] * vec[3] + mat[0][4] * vec[4],
			   mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2] + mat[1][3] * vec[3] + mat[1][4] * vec[4],
			   mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2] + mat[2][3] * vec[3] + mat[2][4] * vec[4],
			   mat[3][0] * vec[0] + mat[3][1] * vec[1] + mat[3][2] * vec[2] + mat[3][3] * vec[3] + mat[3][4] * vec[4],
			   mat[4][0] * vec[0] + mat[4][1] * vec[1] + mat[4][2] * vec[2] + mat[4][3] * vec[3] + mat[4][4] * vec[4] );
}

BUD_INLINE budMat5 budMat5::operator+( const budMat5& a ) const
{
	return budMat5(
			   budVec5( mat[0][0] + a[0][0], mat[0][1] + a[0][1], mat[0][2] + a[0][2], mat[0][3] + a[0][3], mat[0][4] + a[0][4] ),
			   budVec5( mat[1][0] + a[1][0], mat[1][1] + a[1][1], mat[1][2] + a[1][2], mat[1][3] + a[1][3], mat[1][4] + a[1][4] ),
			   budVec5( mat[2][0] + a[2][0], mat[2][1] + a[2][1], mat[2][2] + a[2][2], mat[2][3] + a[2][3], mat[2][4] + a[2][4] ),
			   budVec5( mat[3][0] + a[3][0], mat[3][1] + a[3][1], mat[3][2] + a[3][2], mat[3][3] + a[3][3], mat[3][4] + a[3][4] ),
			   budVec5( mat[4][0] + a[4][0], mat[4][1] + a[4][1], mat[4][2] + a[4][2], mat[4][3] + a[4][3], mat[4][4] + a[4][4] ) );
}

BUD_INLINE budMat5 budMat5::operator-( const budMat5& a ) const
{
	return budMat5(
			   budVec5( mat[0][0] - a[0][0], mat[0][1] - a[0][1], mat[0][2] - a[0][2], mat[0][3] - a[0][3], mat[0][4] - a[0][4] ),
			   budVec5( mat[1][0] - a[1][0], mat[1][1] - a[1][1], mat[1][2] - a[1][2], mat[1][3] - a[1][3], mat[1][4] - a[1][4] ),
			   budVec5( mat[2][0] - a[2][0], mat[2][1] - a[2][1], mat[2][2] - a[2][2], mat[2][3] - a[2][3], mat[2][4] - a[2][4] ),
			   budVec5( mat[3][0] - a[3][0], mat[3][1] - a[3][1], mat[3][2] - a[3][2], mat[3][3] - a[3][3], mat[3][4] - a[3][4] ),
			   budVec5( mat[4][0] - a[4][0], mat[4][1] - a[4][1], mat[4][2] - a[4][2], mat[4][3] - a[4][3], mat[4][4] - a[4][4] ) );
}

BUD_INLINE budMat5& budMat5::operator*=( const float a )
{
	mat[0][0] *= a;
	mat[0][1] *= a;
	mat[0][2] *= a;
	mat[0][3] *= a;
	mat[0][4] *= a;
	mat[1][0] *= a;
	mat[1][1] *= a;
	mat[1][2] *= a;
	mat[1][3] *= a;
	mat[1][4] *= a;
	mat[2][0] *= a;
	mat[2][1] *= a;
	mat[2][2] *= a;
	mat[2][3] *= a;
	mat[2][4] *= a;
	mat[3][0] *= a;
	mat[3][1] *= a;
	mat[3][2] *= a;
	mat[3][3] *= a;
	mat[3][4] *= a;
	mat[4][0] *= a;
	mat[4][1] *= a;
	mat[4][2] *= a;
	mat[4][3] *= a;
	mat[4][4] *= a;
	return *this;
}

BUD_INLINE budMat5& budMat5::operator*=( const budMat5& a )
{
	*this = *this * a;
	return *this;
}

BUD_INLINE budMat5& budMat5::operator+=( const budMat5& a )
{
	mat[0][0] += a[0][0];
	mat[0][1] += a[0][1];
	mat[0][2] += a[0][2];
	mat[0][3] += a[0][3];
	mat[0][4] += a[0][4];
	mat[1][0] += a[1][0];
	mat[1][1] += a[1][1];
	mat[1][2] += a[1][2];
	mat[1][3] += a[1][3];
	mat[1][4] += a[1][4];
	mat[2][0] += a[2][0];
	mat[2][1] += a[2][1];
	mat[2][2] += a[2][2];
	mat[2][3] += a[2][3];
	mat[2][4] += a[2][4];
	mat[3][0] += a[3][0];
	mat[3][1] += a[3][1];
	mat[3][2] += a[3][2];
	mat[3][3] += a[3][3];
	mat[3][4] += a[3][4];
	mat[4][0] += a[4][0];
	mat[4][1] += a[4][1];
	mat[4][2] += a[4][2];
	mat[4][3] += a[4][3];
	mat[4][4] += a[4][4];
	return *this;
}

BUD_INLINE budMat5& budMat5::operator-=( const budMat5& a )
{
	mat[0][0] -= a[0][0];
	mat[0][1] -= a[0][1];
	mat[0][2] -= a[0][2];
	mat[0][3] -= a[0][3];
	mat[0][4] -= a[0][4];
	mat[1][0] -= a[1][0];
	mat[1][1] -= a[1][1];
	mat[1][2] -= a[1][2];
	mat[1][3] -= a[1][3];
	mat[1][4] -= a[1][4];
	mat[2][0] -= a[2][0];
	mat[2][1] -= a[2][1];
	mat[2][2] -= a[2][2];
	mat[2][3] -= a[2][3];
	mat[2][4] -= a[2][4];
	mat[3][0] -= a[3][0];
	mat[3][1] -= a[3][1];
	mat[3][2] -= a[3][2];
	mat[3][3] -= a[3][3];
	mat[3][4] -= a[3][4];
	mat[4][0] -= a[4][0];
	mat[4][1] -= a[4][1];
	mat[4][2] -= a[4][2];
	mat[4][3] -= a[4][3];
	mat[4][4] -= a[4][4];
	return *this;
}

BUD_INLINE budVec5 operator*( const budVec5& vec, const budMat5& mat )
{
	return mat * vec;
}

BUD_INLINE budMat5 operator*( const float a, budMat5 const& mat )
{
	return mat * a;
}

BUD_INLINE budVec5& operator*=( budVec5& vec, const budMat5& mat )
{
	vec = mat * vec;
	return vec;
}

BUD_INLINE bool budMat5::Compare( const budMat5& a ) const
{
	dword i;
	const float* ptr1, *ptr2;
	
	ptr1 = reinterpret_cast<const float*>( mat );
	ptr2 = reinterpret_cast<const float*>( a.mat );
	for( i = 0; i < 5 * 5; i++ )
	{
		if( ptr1[i] != ptr2[i] )
		{
			return false;
		}
	}
	return true;
}

BUD_INLINE bool budMat5::Compare( const budMat5& a, const float epsilon ) const
{
	dword i;
	const float* ptr1, *ptr2;
	
	ptr1 = reinterpret_cast<const float*>( mat );
	ptr2 = reinterpret_cast<const float*>( a.mat );
	for( i = 0; i < 5 * 5; i++ )
	{
		if( budMath::Fabs( ptr1[i] - ptr2[i] ) > epsilon )
		{
			return false;
		}
	}
	return true;
}

BUD_INLINE bool budMat5::operator==( const budMat5& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budMat5::operator!=( const budMat5& a ) const
{
	return !Compare( a );
}

BUD_INLINE void budMat5::Zero()
{
	memset( mat, 0, sizeof( budMat5 ) );
}

BUD_INLINE void budMat5::Identity()
{
	*this = mat5_identity;
}

BUD_INLINE bool budMat5::IsIdentity( const float epsilon ) const
{
	return Compare( mat5_identity, epsilon );
}

BUD_INLINE bool budMat5::IsSymmetric( const float epsilon ) const
{
	for( int i = 1; i < 5; i++ )
	{
		for( int j = 0; j < i; j++ )
		{
			if( budMath::Fabs( mat[i][j] - mat[j][i] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE bool budMat5::IsDiagonal( const float epsilon ) const
{
	for( int i = 0; i < 5; i++ )
	{
		for( int j = 0; j < 5; j++ )
		{
			if( i != j && budMath::Fabs( mat[i][j] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE float budMat5::Trace() const
{
	return ( mat[0][0] + mat[1][1] + mat[2][2] + mat[3][3] + mat[4][4] );
}

BUD_INLINE budMat5 budMat5::Inverse() const
{
	budMat5 invMat;
	
	invMat = *this;
	verify( invMat.InverseSelf() );
	return invMat;
}

BUD_INLINE budMat5 budMat5::InverseFast() const
{
	budMat5 invMat;
	
	invMat = *this;
	verify( invMat.InverseFastSelf() );
	return invMat;
}

BUD_INLINE int budMat5::GetDimension() const
{
	return 25;
}

BUD_INLINE const float* budMat5::ToFloatPtr() const
{
	return mat[0].ToFloatPtr();
}

BUD_INLINE float* budMat5::ToFloatPtr()
{
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	budMat6 - 6x6 matrix
//
//===============================================================

class budMat6
{
public:
	budMat6();
	explicit budMat6( const budVec6& v0, const budVec6& v1, const budVec6& v2, const budVec6& v3, const budVec6& v4, const budVec6& v5 );
	explicit budMat6( const budMat3& m0, const budMat3& m1, const budMat3& m2, const budMat3& m3 );
	explicit budMat6( const float src[ 6 ][ 6 ] );
	
	const budVec6& 	operator[]( int index ) const;
	budVec6& 		operator[]( int index );
	budMat6			operator*( const float a ) const;
	budVec6			operator*( const budVec6& vec ) const;
	budMat6			operator*( const budMat6& a ) const;
	budMat6			operator+( const budMat6& a ) const;
	budMat6			operator-( const budMat6& a ) const;
	budMat6& 		operator*=( const float a );
	budMat6& 		operator*=( const budMat6& a );
	budMat6& 		operator+=( const budMat6& a );
	budMat6& 		operator-=( const budMat6& a );
	
	friend budMat6	operator*( const float a, const budMat6& mat );
	friend budVec6	operator*( const budVec6& vec, const budMat6& mat );
	friend budVec6& 	operator*=( budVec6& vec, const budMat6& mat );
	
	bool			Compare( const budMat6& a ) const;						// exact compare, no epsilon
	bool			Compare( const budMat6& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==( const budMat6& a ) const;					// exact compare, no epsilon
	bool			operator!=( const budMat6& a ) const;					// exact compare, no epsilon
	
	void			Zero();
	void			Identity();
	bool			IsIdentity( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	
	budMat3			SubMat3( int n ) const;
	float			Trace() const;
	float			Determinant() const;
	budMat6			Transpose() const;	// returns transpose
	budMat6& 		TransposeSelf();
	budMat6			Inverse() const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf();		// returns false if determinant is zero
	budMat6			InverseFast() const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf();	// returns false if determinant is zero
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	budVec6			mat[ 6 ];
};

extern budMat6 mat6_zero;
extern budMat6 mat6_identity;
#define mat6_default	mat6_identity

BUD_INLINE budMat6::budMat6()
{
}

BUD_INLINE budMat6::budMat6( const budMat3& m0, const budMat3& m1, const budMat3& m2, const budMat3& m3 )
{
	mat[0] = budVec6( m0[0][0], m0[0][1], m0[0][2], m1[0][0], m1[0][1], m1[0][2] );
	mat[1] = budVec6( m0[1][0], m0[1][1], m0[1][2], m1[1][0], m1[1][1], m1[1][2] );
	mat[2] = budVec6( m0[2][0], m0[2][1], m0[2][2], m1[2][0], m1[2][1], m1[2][2] );
	mat[3] = budVec6( m2[0][0], m2[0][1], m2[0][2], m3[0][0], m3[0][1], m3[0][2] );
	mat[4] = budVec6( m2[1][0], m2[1][1], m2[1][2], m3[1][0], m3[1][1], m3[1][2] );
	mat[5] = budVec6( m2[2][0], m2[2][1], m2[2][2], m3[2][0], m3[2][1], m3[2][2] );
}

BUD_INLINE budMat6::budMat6( const budVec6& v0, const budVec6& v1, const budVec6& v2, const budVec6& v3, const budVec6& v4, const budVec6& v5 )
{
	mat[0] = v0;
	mat[1] = v1;
	mat[2] = v2;
	mat[3] = v3;
	mat[4] = v4;
	mat[5] = v5;
}

BUD_INLINE budMat6::budMat6( const float src[ 6 ][ 6 ] )
{
	memcpy( mat, src, 6 * 6 * sizeof( float ) );
}

BUD_INLINE const budVec6& budMat6::operator[]( int index ) const
{
	//assert( ( index >= 0 ) && ( index < 6 ) );
	return mat[ index ];
}

BUD_INLINE budVec6& budMat6::operator[]( int index )
{
	//assert( ( index >= 0 ) && ( index < 6 ) );
	return mat[ index ];
}

BUD_INLINE budMat6 budMat6::operator*( const budMat6& a ) const
{
	int i, j;
	const float* m1Ptr, *m2Ptr;
	float* dstPtr;
	budMat6 dst;
	
	m1Ptr = reinterpret_cast<const float*>( this );
	m2Ptr = reinterpret_cast<const float*>( &a );
	dstPtr = reinterpret_cast<float*>( &dst );
	
	for( i = 0; i < 6; i++ )
	{
		for( j = 0; j < 6; j++ )
		{
			*dstPtr = m1Ptr[0] * m2Ptr[ 0 * 6 + j ]
					  + m1Ptr[1] * m2Ptr[ 1 * 6 + j ]
					  + m1Ptr[2] * m2Ptr[ 2 * 6 + j ]
					  + m1Ptr[3] * m2Ptr[ 3 * 6 + j ]
					  + m1Ptr[4] * m2Ptr[ 4 * 6 + j ]
					  + m1Ptr[5] * m2Ptr[ 5 * 6 + j ];
			dstPtr++;
		}
		m1Ptr += 6;
	}
	return dst;
}

BUD_INLINE budMat6 budMat6::operator*( const float a ) const
{
	return budMat6(
			   budVec6( mat[0][0] * a, mat[0][1] * a, mat[0][2] * a, mat[0][3] * a, mat[0][4] * a, mat[0][5] * a ),
			   budVec6( mat[1][0] * a, mat[1][1] * a, mat[1][2] * a, mat[1][3] * a, mat[1][4] * a, mat[1][5] * a ),
			   budVec6( mat[2][0] * a, mat[2][1] * a, mat[2][2] * a, mat[2][3] * a, mat[2][4] * a, mat[2][5] * a ),
			   budVec6( mat[3][0] * a, mat[3][1] * a, mat[3][2] * a, mat[3][3] * a, mat[3][4] * a, mat[3][5] * a ),
			   budVec6( mat[4][0] * a, mat[4][1] * a, mat[4][2] * a, mat[4][3] * a, mat[4][4] * a, mat[4][5] * a ),
			   budVec6( mat[5][0] * a, mat[5][1] * a, mat[5][2] * a, mat[5][3] * a, mat[5][4] * a, mat[5][5] * a ) );
}

BUD_INLINE budVec6 budMat6::operator*( const budVec6& vec ) const
{
	return budVec6(
			   mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2] + mat[0][3] * vec[3] + mat[0][4] * vec[4] + mat[0][5] * vec[5],
			   mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2] + mat[1][3] * vec[3] + mat[1][4] * vec[4] + mat[1][5] * vec[5],
			   mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2] + mat[2][3] * vec[3] + mat[2][4] * vec[4] + mat[2][5] * vec[5],
			   mat[3][0] * vec[0] + mat[3][1] * vec[1] + mat[3][2] * vec[2] + mat[3][3] * vec[3] + mat[3][4] * vec[4] + mat[3][5] * vec[5],
			   mat[4][0] * vec[0] + mat[4][1] * vec[1] + mat[4][2] * vec[2] + mat[4][3] * vec[3] + mat[4][4] * vec[4] + mat[4][5] * vec[5],
			   mat[5][0] * vec[0] + mat[5][1] * vec[1] + mat[5][2] * vec[2] + mat[5][3] * vec[3] + mat[5][4] * vec[4] + mat[5][5] * vec[5] );
}

BUD_INLINE budMat6 budMat6::operator+( const budMat6& a ) const
{
	return budMat6(
			   budVec6( mat[0][0] + a[0][0], mat[0][1] + a[0][1], mat[0][2] + a[0][2], mat[0][3] + a[0][3], mat[0][4] + a[0][4], mat[0][5] + a[0][5] ),
			   budVec6( mat[1][0] + a[1][0], mat[1][1] + a[1][1], mat[1][2] + a[1][2], mat[1][3] + a[1][3], mat[1][4] + a[1][4], mat[1][5] + a[1][5] ),
			   budVec6( mat[2][0] + a[2][0], mat[2][1] + a[2][1], mat[2][2] + a[2][2], mat[2][3] + a[2][3], mat[2][4] + a[2][4], mat[2][5] + a[2][5] ),
			   budVec6( mat[3][0] + a[3][0], mat[3][1] + a[3][1], mat[3][2] + a[3][2], mat[3][3] + a[3][3], mat[3][4] + a[3][4], mat[3][5] + a[3][5] ),
			   budVec6( mat[4][0] + a[4][0], mat[4][1] + a[4][1], mat[4][2] + a[4][2], mat[4][3] + a[4][3], mat[4][4] + a[4][4], mat[4][5] + a[4][5] ),
			   budVec6( mat[5][0] + a[5][0], mat[5][1] + a[5][1], mat[5][2] + a[5][2], mat[5][3] + a[5][3], mat[5][4] + a[5][4], mat[5][5] + a[5][5] ) );
}

BUD_INLINE budMat6 budMat6::operator-( const budMat6& a ) const
{
	return budMat6(
			   budVec6( mat[0][0] - a[0][0], mat[0][1] - a[0][1], mat[0][2] - a[0][2], mat[0][3] - a[0][3], mat[0][4] - a[0][4], mat[0][5] - a[0][5] ),
			   budVec6( mat[1][0] - a[1][0], mat[1][1] - a[1][1], mat[1][2] - a[1][2], mat[1][3] - a[1][3], mat[1][4] - a[1][4], mat[1][5] - a[1][5] ),
			   budVec6( mat[2][0] - a[2][0], mat[2][1] - a[2][1], mat[2][2] - a[2][2], mat[2][3] - a[2][3], mat[2][4] - a[2][4], mat[2][5] - a[2][5] ),
			   budVec6( mat[3][0] - a[3][0], mat[3][1] - a[3][1], mat[3][2] - a[3][2], mat[3][3] - a[3][3], mat[3][4] - a[3][4], mat[3][5] - a[3][5] ),
			   budVec6( mat[4][0] - a[4][0], mat[4][1] - a[4][1], mat[4][2] - a[4][2], mat[4][3] - a[4][3], mat[4][4] - a[4][4], mat[4][5] - a[4][5] ),
			   budVec6( mat[5][0] - a[5][0], mat[5][1] - a[5][1], mat[5][2] - a[5][2], mat[5][3] - a[5][3], mat[5][4] - a[5][4], mat[5][5] - a[5][5] ) );
}

BUD_INLINE budMat6& budMat6::operator*=( const float a )
{
	mat[0][0] *= a;
	mat[0][1] *= a;
	mat[0][2] *= a;
	mat[0][3] *= a;
	mat[0][4] *= a;
	mat[0][5] *= a;
	mat[1][0] *= a;
	mat[1][1] *= a;
	mat[1][2] *= a;
	mat[1][3] *= a;
	mat[1][4] *= a;
	mat[1][5] *= a;
	mat[2][0] *= a;
	mat[2][1] *= a;
	mat[2][2] *= a;
	mat[2][3] *= a;
	mat[2][4] *= a;
	mat[2][5] *= a;
	mat[3][0] *= a;
	mat[3][1] *= a;
	mat[3][2] *= a;
	mat[3][3] *= a;
	mat[3][4] *= a;
	mat[3][5] *= a;
	mat[4][0] *= a;
	mat[4][1] *= a;
	mat[4][2] *= a;
	mat[4][3] *= a;
	mat[4][4] *= a;
	mat[4][5] *= a;
	mat[5][0] *= a;
	mat[5][1] *= a;
	mat[5][2] *= a;
	mat[5][3] *= a;
	mat[5][4] *= a;
	mat[5][5] *= a;
	return *this;
}

BUD_INLINE budMat6& budMat6::operator*=( const budMat6& a )
{
	*this = *this * a;
	return *this;
}

BUD_INLINE budMat6& budMat6::operator+=( const budMat6& a )
{
	mat[0][0] += a[0][0];
	mat[0][1] += a[0][1];
	mat[0][2] += a[0][2];
	mat[0][3] += a[0][3];
	mat[0][4] += a[0][4];
	mat[0][5] += a[0][5];
	mat[1][0] += a[1][0];
	mat[1][1] += a[1][1];
	mat[1][2] += a[1][2];
	mat[1][3] += a[1][3];
	mat[1][4] += a[1][4];
	mat[1][5] += a[1][5];
	mat[2][0] += a[2][0];
	mat[2][1] += a[2][1];
	mat[2][2] += a[2][2];
	mat[2][3] += a[2][3];
	mat[2][4] += a[2][4];
	mat[2][5] += a[2][5];
	mat[3][0] += a[3][0];
	mat[3][1] += a[3][1];
	mat[3][2] += a[3][2];
	mat[3][3] += a[3][3];
	mat[3][4] += a[3][4];
	mat[3][5] += a[3][5];
	mat[4][0] += a[4][0];
	mat[4][1] += a[4][1];
	mat[4][2] += a[4][2];
	mat[4][3] += a[4][3];
	mat[4][4] += a[4][4];
	mat[4][5] += a[4][5];
	mat[5][0] += a[5][0];
	mat[5][1] += a[5][1];
	mat[5][2] += a[5][2];
	mat[5][3] += a[5][3];
	mat[5][4] += a[5][4];
	mat[5][5] += a[5][5];
	return *this;
}

BUD_INLINE budMat6& budMat6::operator-=( const budMat6& a )
{
	mat[0][0] -= a[0][0];
	mat[0][1] -= a[0][1];
	mat[0][2] -= a[0][2];
	mat[0][3] -= a[0][3];
	mat[0][4] -= a[0][4];
	mat[0][5] -= a[0][5];
	mat[1][0] -= a[1][0];
	mat[1][1] -= a[1][1];
	mat[1][2] -= a[1][2];
	mat[1][3] -= a[1][3];
	mat[1][4] -= a[1][4];
	mat[1][5] -= a[1][5];
	mat[2][0] -= a[2][0];
	mat[2][1] -= a[2][1];
	mat[2][2] -= a[2][2];
	mat[2][3] -= a[2][3];
	mat[2][4] -= a[2][4];
	mat[2][5] -= a[2][5];
	mat[3][0] -= a[3][0];
	mat[3][1] -= a[3][1];
	mat[3][2] -= a[3][2];
	mat[3][3] -= a[3][3];
	mat[3][4] -= a[3][4];
	mat[3][5] -= a[3][5];
	mat[4][0] -= a[4][0];
	mat[4][1] -= a[4][1];
	mat[4][2] -= a[4][2];
	mat[4][3] -= a[4][3];
	mat[4][4] -= a[4][4];
	mat[4][5] -= a[4][5];
	mat[5][0] -= a[5][0];
	mat[5][1] -= a[5][1];
	mat[5][2] -= a[5][2];
	mat[5][3] -= a[5][3];
	mat[5][4] -= a[5][4];
	mat[5][5] -= a[5][5];
	return *this;
}

BUD_INLINE budVec6 operator*( const budVec6& vec, const budMat6& mat )
{
	return mat * vec;
}

BUD_INLINE budMat6 operator*( const float a, budMat6 const& mat )
{
	return mat * a;
}

BUD_INLINE budVec6& operator*=( budVec6& vec, const budMat6& mat )
{
	vec = mat * vec;
	return vec;
}

BUD_INLINE bool budMat6::Compare( const budMat6& a ) const
{
	dword i;
	const float* ptr1, *ptr2;
	
	ptr1 = reinterpret_cast<const float*>( mat );
	ptr2 = reinterpret_cast<const float*>( a.mat );
	for( i = 0; i < 6 * 6; i++ )
	{
		if( ptr1[i] != ptr2[i] )
		{
			return false;
		}
	}
	return true;
}

BUD_INLINE bool budMat6::Compare( const budMat6& a, const float epsilon ) const
{
	dword i;
	const float* ptr1, *ptr2;
	
	ptr1 = reinterpret_cast<const float*>( mat );
	ptr2 = reinterpret_cast<const float*>( a.mat );
	for( i = 0; i < 6 * 6; i++ )
	{
		if( budMath::Fabs( ptr1[i] - ptr2[i] ) > epsilon )
		{
			return false;
		}
	}
	return true;
}

BUD_INLINE bool budMat6::operator==( const budMat6& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budMat6::operator!=( const budMat6& a ) const
{
	return !Compare( a );
}

BUD_INLINE void budMat6::Zero()
{
	memset( mat, 0, sizeof( budMat6 ) );
}

BUD_INLINE void budMat6::Identity()
{
	*this = mat6_identity;
}

BUD_INLINE bool budMat6::IsIdentity( const float epsilon ) const
{
	return Compare( mat6_identity, epsilon );
}

BUD_INLINE bool budMat6::IsSymmetric( const float epsilon ) const
{
	for( int i = 1; i < 6; i++ )
	{
		for( int j = 0; j < i; j++ )
		{
			if( budMath::Fabs( mat[i][j] - mat[j][i] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE bool budMat6::IsDiagonal( const float epsilon ) const
{
	for( int i = 0; i < 6; i++ )
	{
		for( int j = 0; j < 6; j++ )
		{
			if( i != j && budMath::Fabs( mat[i][j] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE budMat3 budMat6::SubMat3( int n ) const
{
	assert( n >= 0 && n < 4 );
	int b0 = ( ( n & 2 ) >> 1 ) * 3;
	int b1 = ( n & 1 ) * 3;
	return budMat3(
			   mat[b0 + 0][b1 + 0], mat[b0 + 0][b1 + 1], mat[b0 + 0][b1 + 2],
			   mat[b0 + 1][b1 + 0], mat[b0 + 1][b1 + 1], mat[b0 + 1][b1 + 2],
			   mat[b0 + 2][b1 + 0], mat[b0 + 2][b1 + 1], mat[b0 + 2][b1 + 2] );
}

BUD_INLINE float budMat6::Trace() const
{
	return ( mat[0][0] + mat[1][1] + mat[2][2] + mat[3][3] + mat[4][4] + mat[5][5] );
}

BUD_INLINE budMat6 budMat6::Inverse() const
{
	budMat6 invMat;
	
	invMat = *this;
	verify( invMat.InverseSelf() );
	return invMat;
}

BUD_INLINE budMat6 budMat6::InverseFast() const
{
	budMat6 invMat;
	
	invMat = *this;
	verify( invMat.InverseFastSelf() );
	return invMat;
}

BUD_INLINE int budMat6::GetDimension() const
{
	return 36;
}

BUD_INLINE const float* budMat6::ToFloatPtr() const
{
	return mat[0].ToFloatPtr();
}

BUD_INLINE float* budMat6::ToFloatPtr()
{
	return mat[0].ToFloatPtr();
}

#endif /* !__MATH_MATRIX_H__ */
