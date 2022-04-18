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

  Matrix classes, all matrices are row-major except Matrix3

===============================================================================
*/

#define MATRIX_INVERSE_EPSILON		1e-14
#define MATRIX_EPSILON				1e-6

class Angles;
class Quat;
class CMPQuat;
class Rotation;
class Matrix4;

//===============================================================
//
//	Matrix2 - 2x2 matrix
//
//===============================================================

class Matrix2
{
public:
	Matrix2();
	explicit Matrix2( const Vector2& x, const Vector2& y );
	explicit Matrix2( const float xx, const float xy, const float yx, const float yy );
	explicit Matrix2( const float src[ 2 ][ 2 ] );
	
	const Vector2& 	operator[]( int index ) const;
	Vector2& 		operator[]( int index );
	Matrix2			operator-() const;
	Matrix2			operator*( const float a ) const;
	Vector2			operator*( const Vector2& vec ) const;
	Matrix2			operator*( const Matrix2& a ) const;
	Matrix2			operator+( const Matrix2& a ) const;
	Matrix2			operator-( const Matrix2& a ) const;
	Matrix2& 		operator*=( const float a );
	Matrix2& 		operator*=( const Matrix2& a );
	Matrix2& 		operator+=( const Matrix2& a );
	Matrix2& 		operator-=( const Matrix2& a );
	
	friend Matrix2	operator*( const float a, const Matrix2& mat );
	friend Vector2	operator*( const Vector2& vec, const Matrix2& mat );
	friend Vector2& 	operator*=( Vector2& vec, const Matrix2& mat );
	
	bool			Compare( const Matrix2& a ) const;						// exact compare, no epsilon
	bool			Compare( const Matrix2& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==( const Matrix2& a ) const;					// exact compare, no epsilon
	bool			operator!=( const Matrix2& a ) const;					// exact compare, no epsilon
	
	void			Zero();
	void			Identity();
	bool			IsIdentity( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	
	float			Trace() const;
	float			Determinant() const;
	Matrix2			Transpose() const;	// returns transpose
	Matrix2& 		TransposeSelf();
	Matrix2			Inverse() const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf();		// returns false if determinant is zero
	Matrix2			InverseFast() const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf();	// returns false if determinant is zero
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	Vector2			mat[ 2 ];
};

extern Matrix2 mat2_zero;
extern Matrix2 mat2_identity;
#define mat2_default	mat2_identity

BUD_INLINE Matrix2::Matrix2()
{
}

BUD_INLINE Matrix2::Matrix2( const Vector2& x, const Vector2& y )
{
	mat[ 0 ].x = x.x;
	mat[ 0 ].y = x.y;
	mat[ 1 ].x = y.x;
	mat[ 1 ].y = y.y;
}

BUD_INLINE Matrix2::Matrix2( const float xx, const float xy, const float yx, const float yy )
{
	mat[ 0 ].x = xx;
	mat[ 0 ].y = xy;
	mat[ 1 ].x = yx;
	mat[ 1 ].y = yy;
}

BUD_INLINE Matrix2::Matrix2( const float src[ 2 ][ 2 ] )
{
	memcpy( mat, src, 2 * 2 * sizeof( float ) );
}

BUD_INLINE const Vector2& Matrix2::operator[]( int index ) const
{
	//assert( ( index >= 0 ) && ( index < 2 ) );
	return mat[ index ];
}

BUD_INLINE Vector2& Matrix2::operator[]( int index )
{
	//assert( ( index >= 0 ) && ( index < 2 ) );
	return mat[ index ];
}

BUD_INLINE Matrix2 Matrix2::operator-() const
{
	return Matrix2(	-mat[0][0], -mat[0][1],
					-mat[1][0], -mat[1][1] );
}

BUD_INLINE Vector2 Matrix2::operator*( const Vector2& vec ) const
{
	return Vector2(
			   mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y,
			   mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y );
}

BUD_INLINE Matrix2 Matrix2::operator*( const Matrix2& a ) const
{
	return Matrix2(
			   mat[0].x * a[0].x + mat[0].y * a[1].x,
			   mat[0].x * a[0].y + mat[0].y * a[1].y,
			   mat[1].x * a[0].x + mat[1].y * a[1].x,
			   mat[1].x * a[0].y + mat[1].y * a[1].y );
}

BUD_INLINE Matrix2 Matrix2::operator*( const float a ) const
{
	return Matrix2(
			   mat[0].x * a, mat[0].y * a,
			   mat[1].x * a, mat[1].y * a );
}

BUD_INLINE Matrix2 Matrix2::operator+( const Matrix2& a ) const
{
	return Matrix2(
			   mat[0].x + a[0].x, mat[0].y + a[0].y,
			   mat[1].x + a[1].x, mat[1].y + a[1].y );
}

BUD_INLINE Matrix2 Matrix2::operator-( const Matrix2& a ) const
{
	return Matrix2(
			   mat[0].x - a[0].x, mat[0].y - a[0].y,
			   mat[1].x - a[1].x, mat[1].y - a[1].y );
}

BUD_INLINE Matrix2& Matrix2::operator*=( const float a )
{
	mat[0].x *= a;
	mat[0].y *= a;
	mat[1].x *= a;
	mat[1].y *= a;
	
	return *this;
}

BUD_INLINE Matrix2& Matrix2::operator*=( const Matrix2& a )
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

BUD_INLINE Matrix2& Matrix2::operator+=( const Matrix2& a )
{
	mat[0].x += a[0].x;
	mat[0].y += a[0].y;
	mat[1].x += a[1].x;
	mat[1].y += a[1].y;
	
	return *this;
}

BUD_INLINE Matrix2& Matrix2::operator-=( const Matrix2& a )
{
	mat[0].x -= a[0].x;
	mat[0].y -= a[0].y;
	mat[1].x -= a[1].x;
	mat[1].y -= a[1].y;
	
	return *this;
}

BUD_INLINE Vector2 operator*( const Vector2& vec, const Matrix2& mat )
{
	return mat * vec;
}

BUD_INLINE Matrix2 operator*( const float a, Matrix2 const& mat )
{
	return mat * a;
}

BUD_INLINE Vector2& operator*=( Vector2& vec, const Matrix2& mat )
{
	vec = mat * vec;
	return vec;
}

BUD_INLINE bool Matrix2::Compare( const Matrix2& a ) const
{
	if( mat[0].Compare( a[0] ) &&
			mat[1].Compare( a[1] ) )
	{
		return true;
	}
	return false;
}

BUD_INLINE bool Matrix2::Compare( const Matrix2& a, const float epsilon ) const
{
	if( mat[0].Compare( a[0], epsilon ) &&
			mat[1].Compare( a[1], epsilon ) )
	{
		return true;
	}
	return false;
}

BUD_INLINE bool Matrix2::operator==( const Matrix2& a ) const
{
	return Compare( a );
}

BUD_INLINE bool Matrix2::operator!=( const Matrix2& a ) const
{
	return !Compare( a );
}

BUD_INLINE void Matrix2::Zero()
{
	mat[0].Zero();
	mat[1].Zero();
}

BUD_INLINE void Matrix2::Identity()
{
	*this = mat2_identity;
}

BUD_INLINE bool Matrix2::IsIdentity( const float epsilon ) const
{
	return Compare( mat2_identity, epsilon );
}

BUD_INLINE bool Matrix2::IsSymmetric( const float epsilon ) const
{
	return ( Math::Fabs( mat[0][1] - mat[1][0] ) < epsilon );
}

BUD_INLINE bool Matrix2::IsDiagonal( const float epsilon ) const
{
	if( Math::Fabs( mat[0][1] ) > epsilon ||
			Math::Fabs( mat[1][0] ) > epsilon )
	{
		return false;
	}
	return true;
}

BUD_INLINE float Matrix2::Trace() const
{
	return ( mat[0][0] + mat[1][1] );
}

BUD_INLINE float Matrix2::Determinant() const
{
	return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
}

BUD_INLINE Matrix2 Matrix2::Transpose() const
{
	return Matrix2(	mat[0][0], mat[1][0],
					mat[0][1], mat[1][1] );
}

BUD_INLINE Matrix2& Matrix2::TransposeSelf()
{
	float tmp;
	
	tmp = mat[0][1];
	mat[0][1] = mat[1][0];
	mat[1][0] = tmp;
	
	return *this;
}

BUD_INLINE Matrix2 Matrix2::Inverse() const
{
	Matrix2 invMat;
	
	invMat = *this;
	return invMat;
}

BUD_INLINE Matrix2 Matrix2::InverseFast() const
{
	Matrix2 invMat;
	
	invMat = *this;
	return invMat;
}

BUD_INLINE int Matrix2::GetDimension() const
{
	return 4;
}

BUD_INLINE const float* Matrix2::ToFloatPtr() const
{
	return mat[0].ToFloatPtr();
}

BUD_INLINE float* Matrix2::ToFloatPtr()
{
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	Matrix3 - 3x3 matrix
//
//	NOTE:	matrix is column-major
//
//===============================================================

class Matrix3
{
public:
	Matrix3();
	explicit Matrix3( const Vector3& x, const Vector3& y, const Vector3& z );
	explicit Matrix3( const float xx, const float xy, const float xz, const float yx, const float yy, const float yz, const float zx, const float zy, const float zz );
	explicit Matrix3( const float src[ 3 ][ 3 ] );
	
	const Vector3& 	operator[]( int index ) const;
	Vector3& 		operator[]( int index );
	Matrix3			operator-() const;
	Matrix3			operator*( const float a ) const;
	Vector3			operator*( const Vector3& vec ) const;
	Matrix3			operator*( const Matrix3& a ) const;
	Matrix3			operator+( const Matrix3& a ) const;
	Matrix3			operator-( const Matrix3& a ) const;
	Matrix3& 		operator*=( const float a );
	Matrix3& 		operator*=( const Matrix3& a );
	Matrix3& 		operator+=( const Matrix3& a );
	Matrix3& 		operator-=( const Matrix3& a );
	
	friend Matrix3	operator*( const float a, const Matrix3& mat );
	friend Vector3	operator*( const Vector3& vec, const Matrix3& mat );
	friend Vector3& 	operator*=( Vector3& vec, const Matrix3& mat );
	
	bool			Compare( const Matrix3& a ) const;						// exact compare, no epsilon
	bool			Compare( const Matrix3& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==( const Matrix3& a ) const;					// exact compare, no epsilon
	bool			operator!=( const Matrix3& a ) const;					// exact compare, no epsilon
	
	void			Zero();
	void			Identity();
	bool			IsIdentity( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsRotated() const;
	
	void			ProjectVector( const Vector3& src, Vector3& dst ) const;
	void			UnprojectVector( const Vector3& src, Vector3& dst ) const;
	
	bool			FixDegeneracies();	// fix degenerate axial cases
	bool			FixDenormals();		// change tiny numbers to zero
	
	float			Trace() const;
	float			Determinant() const;
	Matrix3			OrthoNormalize() const;
	Matrix3& 		OrthoNormalizeSelf();
	Matrix3			Transpose() const;	// returns transpose
	Matrix3& 		TransposeSelf();
	Matrix3			Inverse() const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf();		// returns false if determinant is zero
	Matrix3			InverseFast() const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf();	// returns false if determinant is zero
	Matrix3			TransposeMultiply( const Matrix3& b ) const;
	
	Matrix3			InertiaTranslate( const float mass, const Vector3& centerOfMass, const Vector3& translation ) const;
	Matrix3& 		InertiaTranslateSelf( const float mass, const Vector3& centerOfMass, const Vector3& translation );
	Matrix3			InertiaRotate( const Matrix3& rotation ) const;
	Matrix3& 		InertiaRotateSelf( const Matrix3& rotation );
	
	int				GetDimension() const;
	
	Angles		ToAngles() const;
	Quat			ToQuat() const;
	CMPQuat			ToCQuat() const;
	Rotation		ToRotation() const;
	Matrix4			ToMat4() const;
	Vector3			ToAngularVelocity() const;
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
	friend void		TransposeMultiply( const Matrix3& inv, const Matrix3& b, Matrix3& dst );
	friend Matrix3	SkewSymmetric( Vector3 const& src );
	
private:
	Vector3			mat[ 3 ];
};

extern Matrix3 mat3_zero;
extern Matrix3 mat3_identity;
#define mat3_default	mat3_identity

BUD_INLINE Matrix3::Matrix3()
{
}

BUD_INLINE Matrix3::Matrix3( const Vector3& x, const Vector3& y, const Vector3& z )
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

BUD_INLINE Matrix3::Matrix3( const float xx, const float xy, const float xz, const float yx, const float yy, const float yz, const float zx, const float zy, const float zz )
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

BUD_INLINE Matrix3::Matrix3( const float src[ 3 ][ 3 ] )
{
	memcpy( mat, src, 3 * 3 * sizeof( float ) );
}

BUD_INLINE const Vector3& Matrix3::operator[]( int index ) const
{
	//assert( ( index >= 0 ) && ( index < 3 ) );
	return mat[ index ];
}

BUD_INLINE Vector3& Matrix3::operator[]( int index )
{
	//assert( ( index >= 0 ) && ( index < 3 ) );
	return mat[ index ];
}

BUD_INLINE Matrix3 Matrix3::operator-() const
{
	return Matrix3(	-mat[0][0], -mat[0][1], -mat[0][2],
					-mat[1][0], -mat[1][1], -mat[1][2],
					-mat[2][0], -mat[2][1], -mat[2][2] );
}

BUD_INLINE Vector3 Matrix3::operator*( const Vector3& vec ) const
{
	return Vector3(
			   mat[ 0 ].x * vec.x + mat[ 1 ].x * vec.y + mat[ 2 ].x * vec.z,
			   mat[ 0 ].y * vec.x + mat[ 1 ].y * vec.y + mat[ 2 ].y * vec.z,
			   mat[ 0 ].z * vec.x + mat[ 1 ].z * vec.y + mat[ 2 ].z * vec.z );
}

BUD_INLINE Matrix3 Matrix3::operator*( const Matrix3& a ) const
{
	int i, j;
	const float* m1Ptr, *m2Ptr;
	float* dstPtr;
	Matrix3 dst;
	
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

BUD_INLINE Matrix3 Matrix3::operator*( const float a ) const
{
	return Matrix3(
			   mat[0].x * a, mat[0].y * a, mat[0].z * a,
			   mat[1].x * a, mat[1].y * a, mat[1].z * a,
			   mat[2].x * a, mat[2].y * a, mat[2].z * a );
}

BUD_INLINE Matrix3 Matrix3::operator+( const Matrix3& a ) const
{
	return Matrix3(
			   mat[0].x + a[0].x, mat[0].y + a[0].y, mat[0].z + a[0].z,
			   mat[1].x + a[1].x, mat[1].y + a[1].y, mat[1].z + a[1].z,
			   mat[2].x + a[2].x, mat[2].y + a[2].y, mat[2].z + a[2].z );
}

BUD_INLINE Matrix3 Matrix3::operator-( const Matrix3& a ) const
{
	return Matrix3(
			   mat[0].x - a[0].x, mat[0].y - a[0].y, mat[0].z - a[0].z,
			   mat[1].x - a[1].x, mat[1].y - a[1].y, mat[1].z - a[1].z,
			   mat[2].x - a[2].x, mat[2].y - a[2].y, mat[2].z - a[2].z );
}

BUD_INLINE Matrix3& Matrix3::operator*=( const float a )
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

BUD_INLINE Matrix3& Matrix3::operator*=( const Matrix3& a )
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

BUD_INLINE Matrix3& Matrix3::operator+=( const Matrix3& a )
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

BUD_INLINE Matrix3& Matrix3::operator-=( const Matrix3& a )
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

BUD_INLINE Vector3 operator*( const Vector3& vec, const Matrix3& mat )
{
	return mat * vec;
}

BUD_INLINE Matrix3 operator*( const float a, const Matrix3& mat )
{
	return mat * a;
}

BUD_INLINE Vector3& operator*=( Vector3& vec, const Matrix3& mat )
{
	float x = mat[ 0 ].x * vec.x + mat[ 1 ].x * vec.y + mat[ 2 ].x * vec.z;
	float y = mat[ 0 ].y * vec.x + mat[ 1 ].y * vec.y + mat[ 2 ].y * vec.z;
	vec.z = mat[ 0 ].z * vec.x + mat[ 1 ].z * vec.y + mat[ 2 ].z * vec.z;
	vec.x = x;
	vec.y = y;
	return vec;
}

BUD_INLINE bool Matrix3::Compare( const Matrix3& a ) const
{
	if( mat[0].Compare( a[0] ) &&
			mat[1].Compare( a[1] ) &&
			mat[2].Compare( a[2] ) )
	{
		return true;
	}
	return false;
}

BUD_INLINE bool Matrix3::Compare( const Matrix3& a, const float epsilon ) const
{
	if( mat[0].Compare( a[0], epsilon ) &&
			mat[1].Compare( a[1], epsilon ) &&
			mat[2].Compare( a[2], epsilon ) )
	{
		return true;
	}
	return false;
}

BUD_INLINE bool Matrix3::operator==( const Matrix3& a ) const
{
	return Compare( a );
}

BUD_INLINE bool Matrix3::operator!=( const Matrix3& a ) const
{
	return !Compare( a );
}

BUD_INLINE void Matrix3::Zero()
{
	memset( mat, 0, sizeof( Matrix3 ) );
}

BUD_INLINE void Matrix3::Identity()
{
	*this = mat3_identity;
}

BUD_INLINE bool Matrix3::IsIdentity( const float epsilon ) const
{
	return Compare( mat3_identity, epsilon );
}

BUD_INLINE bool Matrix3::IsSymmetric( const float epsilon ) const
{
	if( Math::Fabs( mat[0][1] - mat[1][0] ) > epsilon )
	{
		return false;
	}
	if( Math::Fabs( mat[0][2] - mat[2][0] ) > epsilon )
	{
		return false;
	}
	if( Math::Fabs( mat[1][2] - mat[2][1] ) > epsilon )
	{
		return false;
	}
	return true;
}

BUD_INLINE bool Matrix3::IsDiagonal( const float epsilon ) const
{
	if( Math::Fabs( mat[0][1] ) > epsilon ||
			Math::Fabs( mat[0][2] ) > epsilon ||
			Math::Fabs( mat[1][0] ) > epsilon ||
			Math::Fabs( mat[1][2] ) > epsilon ||
			Math::Fabs( mat[2][0] ) > epsilon ||
			Math::Fabs( mat[2][1] ) > epsilon )
	{
		return false;
	}
	return true;
}

BUD_INLINE bool Matrix3::IsRotated() const
{
	return !Compare( mat3_identity );
}

BUD_INLINE void Matrix3::ProjectVector( const Vector3& src, Vector3& dst ) const
{
	dst.x = src * mat[ 0 ];
	dst.y = src * mat[ 1 ];
	dst.z = src * mat[ 2 ];
}

BUD_INLINE void Matrix3::UnprojectVector( const Vector3& src, Vector3& dst ) const
{
	dst = mat[ 0 ] * src.x + mat[ 1 ] * src.y + mat[ 2 ] * src.z;
}

BUD_INLINE bool Matrix3::FixDegeneracies()
{
	bool r = mat[0].FixDegenerateNormal();
	r |= mat[1].FixDegenerateNormal();
	r |= mat[2].FixDegenerateNormal();
	return r;
}

BUD_INLINE bool Matrix3::FixDenormals()
{
	bool r = mat[0].FixDenormals();
	r |= mat[1].FixDenormals();
	r |= mat[2].FixDenormals();
	return r;
}

BUD_INLINE float Matrix3::Trace() const
{
	return ( mat[0][0] + mat[1][1] + mat[2][2] );
}

BUD_INLINE Matrix3 Matrix3::OrthoNormalize() const
{
	Matrix3 ortho;
	
	ortho = *this;
	ortho[ 0 ].Normalize();
	ortho[ 2 ].Cross( mat[ 0 ], mat[ 1 ] );
	ortho[ 2 ].Normalize();
	ortho[ 1 ].Cross( mat[ 2 ], mat[ 0 ] );
	ortho[ 1 ].Normalize();
	return ortho;
}

BUD_INLINE Matrix3& Matrix3::OrthoNormalizeSelf()
{
	mat[ 0 ].Normalize();
	mat[ 2 ].Cross( mat[ 0 ], mat[ 1 ] );
	mat[ 2 ].Normalize();
	mat[ 1 ].Cross( mat[ 2 ], mat[ 0 ] );
	mat[ 1 ].Normalize();
	return *this;
}

BUD_INLINE Matrix3 Matrix3::Transpose() const
{
	return Matrix3(	mat[0][0], mat[1][0], mat[2][0],
					mat[0][1], mat[1][1], mat[2][1],
					mat[0][2], mat[1][2], mat[2][2] );
}

BUD_INLINE Matrix3& Matrix3::TransposeSelf()
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

BUD_INLINE Matrix3 Matrix3::Inverse() const
{
	Matrix3 invMat;
	
	invMat = *this;
	return invMat;
}

BUD_INLINE Matrix3 Matrix3::InverseFast() const
{
	Matrix3 invMat;
	
	invMat = *this;
	return invMat;
}

BUD_INLINE Matrix3 Matrix3::TransposeMultiply( const Matrix3& b ) const
{
	return Matrix3(	mat[0].x * b[0].x + mat[1].x * b[1].x + mat[2].x * b[2].x,
					mat[0].x * b[0].y + mat[1].x * b[1].y + mat[2].x * b[2].y,
					mat[0].x * b[0].z + mat[1].x * b[1].z + mat[2].x * b[2].z,
					mat[0].y * b[0].x + mat[1].y * b[1].x + mat[2].y * b[2].x,
					mat[0].y * b[0].y + mat[1].y * b[1].y + mat[2].y * b[2].y,
					mat[0].y * b[0].z + mat[1].y * b[1].z + mat[2].y * b[2].z,
					mat[0].z * b[0].x + mat[1].z * b[1].x + mat[2].z * b[2].x,
					mat[0].z * b[0].y + mat[1].z * b[1].y + mat[2].z * b[2].y,
					mat[0].z * b[0].z + mat[1].z * b[1].z + mat[2].z * b[2].z );
}

BUD_INLINE void TransposeMultiply( const Matrix3& transpose, const Matrix3& b, Matrix3& dst )
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

BUD_INLINE Matrix3 SkewSymmetric( Vector3 const& src )
{
	return Matrix3( 0.0f, -src.z,  src.y, src.z,   0.0f, -src.x, -src.y,  src.x,   0.0f );
}

BUD_INLINE int Matrix3::GetDimension() const
{
	return 9;
}

BUD_INLINE const float* Matrix3::ToFloatPtr() const
{
	return mat[0].ToFloatPtr();
}

BUD_INLINE float* Matrix3::ToFloatPtr()
{
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	Matrix4 - 4x4 matrix
//
//===============================================================

class Matrix4
{
public:
	Matrix4();
	explicit Matrix4( const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w );
	explicit Matrix4( const float xx, const float xy, const float xz, const float xw,
					 const float yx, const float yy, const float yz, const float yw,
					 const float zx, const float zy, const float zz, const float zw,
					 const float wx, const float wy, const float wz, const float ww );
	explicit Matrix4( const Matrix3& rotation, const Vector3& translation );
	explicit Matrix4( const float src[ 4 ][ 4 ] );
	
	const Vector4& 	operator[]( int index ) const;
	Vector4& 		operator[]( int index );
	Matrix4			operator*( const float a ) const;
	Vector4			operator*( const Vector4& vec ) const;
	Vector3			operator*( const Vector3& vec ) const;
	Matrix4			operator*( const Matrix4& a ) const;
	Matrix4			operator+( const Matrix4& a ) const;
	Matrix4			operator-( const Matrix4& a ) const;
	Matrix4& 		operator*=( const float a );
	Matrix4& 		operator*=( const Matrix4& a );
	Matrix4& 		operator+=( const Matrix4& a );
	Matrix4& 		operator-=( const Matrix4& a );
	
	friend Matrix4	operator*( const float a, const Matrix4& mat );
	friend Vector4	operator*( const Vector4& vec, const Matrix4& mat );
	friend Vector3	operator*( const Vector3& vec, const Matrix4& mat );
	friend Vector4& 	operator*=( Vector4& vec, const Matrix4& mat );
	friend Vector3& 	operator*=( Vector3& vec, const Matrix4& mat );
	
	bool			Compare( const Matrix4& a ) const;						// exact compare, no epsilon
	bool			Compare( const Matrix4& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==( const Matrix4& a ) const;					// exact compare, no epsilon
	bool			operator!=( const Matrix4& a ) const;					// exact compare, no epsilon
	
	void			Zero();
	void			Identity();
	bool			IsIdentity( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsRotated() const;
	
	void			ProjectVector( const Vector4& src, Vector4& dst ) const;
	void			UnprojectVector( const Vector4& src, Vector4& dst ) const;
	
	float			Trace() const;
	float			Determinant() const;
	Matrix4			Transpose() const;	// returns transpose
	Matrix4& 		TransposeSelf();
	Matrix4			Inverse() const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf();		// returns false if determinant is zero
	Matrix4			InverseFast() const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf();	// returns false if determinant is zero
	Matrix4			TransposeMultiply( const Matrix4& b ) const;
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	Vector4			mat[ 4 ];
};

extern Matrix4 mat4_zero;
extern Matrix4 mat4_identity;
#define mat4_default	mat4_identity

BUD_INLINE Matrix4::Matrix4()
{
}

BUD_INLINE Matrix4::Matrix4( const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w )
{
	mat[ 0 ] = x;
	mat[ 1 ] = y;
	mat[ 2 ] = z;
	mat[ 3 ] = w;
}

BUD_INLINE Matrix4::Matrix4( const float xx, const float xy, const float xz, const float xw,
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

BUD_INLINE Matrix4::Matrix4( const Matrix3& rotation, const Vector3& translation )
{
	// NOTE: Matrix3 is transposed because it is column-major
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

BUD_INLINE Matrix4::Matrix4( const float src[ 4 ][ 4 ] )
{
	memcpy( mat, src, 4 * 4 * sizeof( float ) );
}

BUD_INLINE const Vector4& Matrix4::operator[]( int index ) const
{
	//assert( ( index >= 0 ) && ( index < 4 ) );
	return mat[ index ];
}

BUD_INLINE Vector4& Matrix4::operator[]( int index )
{
	//assert( ( index >= 0 ) && ( index < 4 ) );
	return mat[ index ];
}

BUD_INLINE Matrix4 Matrix4::operator*( const float a ) const
{
	return Matrix4(
			   mat[0].x * a, mat[0].y * a, mat[0].z * a, mat[0].w * a,
			   mat[1].x * a, mat[1].y * a, mat[1].z * a, mat[1].w * a,
			   mat[2].x * a, mat[2].y * a, mat[2].z * a, mat[2].w * a,
			   mat[3].x * a, mat[3].y * a, mat[3].z * a, mat[3].w * a );
}

BUD_INLINE Vector4 Matrix4::operator*( const Vector4& vec ) const
{
	return Vector4(
			   mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w * vec.w,
			   mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w * vec.w,
			   mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w * vec.w,
			   mat[ 3 ].x * vec.x + mat[ 3 ].y * vec.y + mat[ 3 ].z * vec.z + mat[ 3 ].w * vec.w );
}

BUD_INLINE Vector3 Matrix4::operator*( const Vector3& vec ) const
{
	float s = mat[ 3 ].x * vec.x + mat[ 3 ].y * vec.y + mat[ 3 ].z * vec.z + mat[ 3 ].w;
	if( s == 0.0f )
	{
		return Vector3( 0.0f, 0.0f, 0.0f );
	}
	if( s == 1.0f )
	{
		return Vector3(
				   mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w,
				   mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w,
				   mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w );
	}
	else
	{
		float invS = 1.0f / s;
		return Vector3(
				   ( mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z + mat[ 0 ].w ) * invS,
				   ( mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z + mat[ 1 ].w ) * invS,
				   ( mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z + mat[ 2 ].w ) * invS );
	}
}

BUD_INLINE Matrix4 Matrix4::operator*( const Matrix4& a ) const
{
	int i, j;
	const float* m1Ptr, *m2Ptr;
	float* dstPtr;
	Matrix4 dst;
	
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

BUD_INLINE Matrix4 Matrix4::operator+( const Matrix4& a ) const
{
	return Matrix4(
			   mat[0].x + a[0].x, mat[0].y + a[0].y, mat[0].z + a[0].z, mat[0].w + a[0].w,
			   mat[1].x + a[1].x, mat[1].y + a[1].y, mat[1].z + a[1].z, mat[1].w + a[1].w,
			   mat[2].x + a[2].x, mat[2].y + a[2].y, mat[2].z + a[2].z, mat[2].w + a[2].w,
			   mat[3].x + a[3].x, mat[3].y + a[3].y, mat[3].z + a[3].z, mat[3].w + a[3].w );
}

BUD_INLINE Matrix4 Matrix4::operator-( const Matrix4& a ) const
{
	return Matrix4(
			   mat[0].x - a[0].x, mat[0].y - a[0].y, mat[0].z - a[0].z, mat[0].w - a[0].w,
			   mat[1].x - a[1].x, mat[1].y - a[1].y, mat[1].z - a[1].z, mat[1].w - a[1].w,
			   mat[2].x - a[2].x, mat[2].y - a[2].y, mat[2].z - a[2].z, mat[2].w - a[2].w,
			   mat[3].x - a[3].x, mat[3].y - a[3].y, mat[3].z - a[3].z, mat[3].w - a[3].w );
}

BUD_INLINE Matrix4& Matrix4::operator*=( const float a )
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

BUD_INLINE Matrix4& Matrix4::operator*=( const Matrix4& a )
{
	*this = ( *this ) * a;
	return *this;
}

BUD_INLINE Matrix4& Matrix4::operator+=( const Matrix4& a )
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

BUD_INLINE Matrix4& Matrix4::operator-=( const Matrix4& a )
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

BUD_INLINE Matrix4 operator*( const float a, const Matrix4& mat )
{
	return mat * a;
}

BUD_INLINE Vector4 operator*( const Vector4& vec, const Matrix4& mat )
{
	return mat * vec;
}

BUD_INLINE Vector3 operator*( const Vector3& vec, const Matrix4& mat )
{
	return mat * vec;
}

BUD_INLINE Vector4& operator*=( Vector4& vec, const Matrix4& mat )
{
	vec = mat * vec;
	return vec;
}

BUD_INLINE Vector3& operator*=( Vector3& vec, const Matrix4& mat )
{
	vec = mat * vec;
	return vec;
}

BUD_INLINE bool Matrix4::Compare( const Matrix4& a ) const
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

BUD_INLINE bool Matrix4::Compare( const Matrix4& a, const float epsilon ) const
{
	dword i;
	const float* ptr1, *ptr2;
	
	ptr1 = reinterpret_cast<const float*>( mat );
	ptr2 = reinterpret_cast<const float*>( a.mat );
	for( i = 0; i < 4 * 4; i++ )
	{
		if( Math::Fabs( ptr1[i] - ptr2[i] ) > epsilon )
		{
			return false;
		}
	}
	return true;
}

BUD_INLINE bool Matrix4::operator==( const Matrix4& a ) const
{
	return Compare( a );
}

BUD_INLINE bool Matrix4::operator!=( const Matrix4& a ) const
{
	return !Compare( a );
}

BUD_INLINE void Matrix4::Zero()
{
	memset( mat, 0, sizeof( Matrix4 ) );
}

BUD_INLINE void Matrix4::Identity()
{
	*this = mat4_identity;
}

BUD_INLINE bool Matrix4::IsIdentity( const float epsilon ) const
{
	return Compare( mat4_identity, epsilon );
}

BUD_INLINE bool Matrix4::IsSymmetric( const float epsilon ) const
{
	for( int i = 1; i < 4; i++ )
	{
		for( int j = 0; j < i; j++ )
		{
			if( Math::Fabs( mat[i][j] - mat[j][i] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE bool Matrix4::IsDiagonal( const float epsilon ) const
{
	for( int i = 0; i < 4; i++ )
	{
		for( int j = 0; j < 4; j++ )
		{
			if( i != j && Math::Fabs( mat[i][j] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE bool Matrix4::IsRotated() const
{
	if( !mat[ 0 ][ 1 ] && !mat[ 0 ][ 2 ] &&
			!mat[ 1 ][ 0 ] && !mat[ 1 ][ 2 ] &&
			!mat[ 2 ][ 0 ] && !mat[ 2 ][ 1 ] )
	{
		return false;
	}
	return true;
}

BUD_INLINE void Matrix4::ProjectVector( const Vector4& src, Vector4& dst ) const
{
	dst.x = src * mat[ 0 ];
	dst.y = src * mat[ 1 ];
	dst.z = src * mat[ 2 ];
	dst.w = src * mat[ 3 ];
}

BUD_INLINE void Matrix4::UnprojectVector( const Vector4& src, Vector4& dst ) const
{
	dst = mat[ 0 ] * src.x + mat[ 1 ] * src.y + mat[ 2 ] * src.z + mat[ 3 ] * src.w;
}

BUD_INLINE float Matrix4::Trace() const
{
	return ( mat[0][0] + mat[1][1] + mat[2][2] + mat[3][3] );
}

BUD_INLINE Matrix4 Matrix4::Inverse() const
{
	Matrix4 invMat;
	
	invMat = *this;
	return invMat;
}

BUD_INLINE Matrix4 Matrix4::InverseFast() const
{
	Matrix4 invMat;
	
	invMat = *this;
	return invMat;
}

BUD_INLINE Matrix4 Matrix3::ToMat4() const
{
	// NOTE: Matrix3 is transposed because it is column-major
	return Matrix4(	mat[0][0],	mat[1][0],	mat[2][0],	0.0f,
					mat[0][1],	mat[1][1],	mat[2][1],	0.0f,
					mat[0][2],	mat[1][2],	mat[2][2],	0.0f,
					0.0f,		0.0f,		0.0f,		1.0f );
}

BUD_INLINE int Matrix4::GetDimension() const
{
	return 16;
}

BUD_INLINE const float* Matrix4::ToFloatPtr() const
{
	return mat[0].ToFloatPtr();
}

BUD_INLINE float* Matrix4::ToFloatPtr()
{
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	Matrix5 - 5x5 matrix
//
//===============================================================

class Matrix5
{
public:
	Matrix5();
	explicit Matrix5( const Vector5& v0, const Vector5& v1, const Vector5& v2, const Vector5& v3, const Vector5& v4 );
	explicit Matrix5( const float src[ 5 ][ 5 ] );
	
	const Vector5& 	operator[]( int index ) const;
	Vector5& 		operator[]( int index );
	Matrix5			operator*( const float a ) const;
	Vector5			operator*( const Vector5& vec ) const;
	Matrix5			operator*( const Matrix5& a ) const;
	Matrix5			operator+( const Matrix5& a ) const;
	Matrix5			operator-( const Matrix5& a ) const;
	Matrix5& 		operator*=( const float a );
	Matrix5& 		operator*=( const Matrix5& a );
	Matrix5& 		operator+=( const Matrix5& a );
	Matrix5& 		operator-=( const Matrix5& a );
	
	friend Matrix5	operator*( const float a, const Matrix5& mat );
	friend Vector5	operator*( const Vector5& vec, const Matrix5& mat );
	friend Vector5& 	operator*=( Vector5& vec, const Matrix5& mat );
	
	bool			Compare( const Matrix5& a ) const;						// exact compare, no epsilon
	bool			Compare( const Matrix5& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==( const Matrix5& a ) const;					// exact compare, no epsilon
	bool			operator!=( const Matrix5& a ) const;					// exact compare, no epsilon
	
	void			Zero();
	void			Identity();
	bool			IsIdentity( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	
	float			Trace() const;
	float			Determinant() const;
	Matrix5			Transpose() const;	// returns transpose
	Matrix5& 		TransposeSelf();
	Matrix5			Inverse() const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf();		// returns false if determinant is zero
	Matrix5			InverseFast() const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf();	// returns false if determinant is zero
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	Vector5			mat[ 5 ];
};

extern Matrix5 mat5_zero;
extern Matrix5 mat5_identity;
#define mat5_default	mat5_identity

BUD_INLINE Matrix5::Matrix5()
{
}

BUD_INLINE Matrix5::Matrix5( const float src[ 5 ][ 5 ] )
{
	memcpy( mat, src, 5 * 5 * sizeof( float ) );
}

BUD_INLINE Matrix5::Matrix5( const Vector5& v0, const Vector5& v1, const Vector5& v2, const Vector5& v3, const Vector5& v4 )
{
	mat[0] = v0;
	mat[1] = v1;
	mat[2] = v2;
	mat[3] = v3;
	mat[4] = v4;
}

BUD_INLINE const Vector5& Matrix5::operator[]( int index ) const
{
	//assert( ( index >= 0 ) && ( index < 5 ) );
	return mat[ index ];
}

BUD_INLINE Vector5& Matrix5::operator[]( int index )
{
	//assert( ( index >= 0 ) && ( index < 5 ) );
	return mat[ index ];
}

BUD_INLINE Matrix5 Matrix5::operator*( const Matrix5& a ) const
{
	int i, j;
	const float* m1Ptr, *m2Ptr;
	float* dstPtr;
	Matrix5 dst;
	
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

BUD_INLINE Matrix5 Matrix5::operator*( const float a ) const
{
	return Matrix5(
			   Vector5( mat[0][0] * a, mat[0][1] * a, mat[0][2] * a, mat[0][3] * a, mat[0][4] * a ),
			   Vector5( mat[1][0] * a, mat[1][1] * a, mat[1][2] * a, mat[1][3] * a, mat[1][4] * a ),
			   Vector5( mat[2][0] * a, mat[2][1] * a, mat[2][2] * a, mat[2][3] * a, mat[2][4] * a ),
			   Vector5( mat[3][0] * a, mat[3][1] * a, mat[3][2] * a, mat[3][3] * a, mat[3][4] * a ),
			   Vector5( mat[4][0] * a, mat[4][1] * a, mat[4][2] * a, mat[4][3] * a, mat[4][4] * a ) );
}

BUD_INLINE Vector5 Matrix5::operator*( const Vector5& vec ) const
{
	return Vector5(
			   mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2] + mat[0][3] * vec[3] + mat[0][4] * vec[4],
			   mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2] + mat[1][3] * vec[3] + mat[1][4] * vec[4],
			   mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2] + mat[2][3] * vec[3] + mat[2][4] * vec[4],
			   mat[3][0] * vec[0] + mat[3][1] * vec[1] + mat[3][2] * vec[2] + mat[3][3] * vec[3] + mat[3][4] * vec[4],
			   mat[4][0] * vec[0] + mat[4][1] * vec[1] + mat[4][2] * vec[2] + mat[4][3] * vec[3] + mat[4][4] * vec[4] );
}

BUD_INLINE Matrix5 Matrix5::operator+( const Matrix5& a ) const
{
	return Matrix5(
			   Vector5( mat[0][0] + a[0][0], mat[0][1] + a[0][1], mat[0][2] + a[0][2], mat[0][3] + a[0][3], mat[0][4] + a[0][4] ),
			   Vector5( mat[1][0] + a[1][0], mat[1][1] + a[1][1], mat[1][2] + a[1][2], mat[1][3] + a[1][3], mat[1][4] + a[1][4] ),
			   Vector5( mat[2][0] + a[2][0], mat[2][1] + a[2][1], mat[2][2] + a[2][2], mat[2][3] + a[2][3], mat[2][4] + a[2][4] ),
			   Vector5( mat[3][0] + a[3][0], mat[3][1] + a[3][1], mat[3][2] + a[3][2], mat[3][3] + a[3][3], mat[3][4] + a[3][4] ),
			   Vector5( mat[4][0] + a[4][0], mat[4][1] + a[4][1], mat[4][2] + a[4][2], mat[4][3] + a[4][3], mat[4][4] + a[4][4] ) );
}

BUD_INLINE Matrix5 Matrix5::operator-( const Matrix5& a ) const
{
	return Matrix5(
			   Vector5( mat[0][0] - a[0][0], mat[0][1] - a[0][1], mat[0][2] - a[0][2], mat[0][3] - a[0][3], mat[0][4] - a[0][4] ),
			   Vector5( mat[1][0] - a[1][0], mat[1][1] - a[1][1], mat[1][2] - a[1][2], mat[1][3] - a[1][3], mat[1][4] - a[1][4] ),
			   Vector5( mat[2][0] - a[2][0], mat[2][1] - a[2][1], mat[2][2] - a[2][2], mat[2][3] - a[2][3], mat[2][4] - a[2][4] ),
			   Vector5( mat[3][0] - a[3][0], mat[3][1] - a[3][1], mat[3][2] - a[3][2], mat[3][3] - a[3][3], mat[3][4] - a[3][4] ),
			   Vector5( mat[4][0] - a[4][0], mat[4][1] - a[4][1], mat[4][2] - a[4][2], mat[4][3] - a[4][3], mat[4][4] - a[4][4] ) );
}

BUD_INLINE Matrix5& Matrix5::operator*=( const float a )
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

BUD_INLINE Matrix5& Matrix5::operator*=( const Matrix5& a )
{
	*this = *this * a;
	return *this;
}

BUD_INLINE Matrix5& Matrix5::operator+=( const Matrix5& a )
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

BUD_INLINE Matrix5& Matrix5::operator-=( const Matrix5& a )
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

BUD_INLINE Vector5 operator*( const Vector5& vec, const Matrix5& mat )
{
	return mat * vec;
}

BUD_INLINE Matrix5 operator*( const float a, Matrix5 const& mat )
{
	return mat * a;
}

BUD_INLINE Vector5& operator*=( Vector5& vec, const Matrix5& mat )
{
	vec = mat * vec;
	return vec;
}

BUD_INLINE bool Matrix5::Compare( const Matrix5& a ) const
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

BUD_INLINE bool Matrix5::Compare( const Matrix5& a, const float epsilon ) const
{
	dword i;
	const float* ptr1, *ptr2;
	
	ptr1 = reinterpret_cast<const float*>( mat );
	ptr2 = reinterpret_cast<const float*>( a.mat );
	for( i = 0; i < 5 * 5; i++ )
	{
		if( Math::Fabs( ptr1[i] - ptr2[i] ) > epsilon )
		{
			return false;
		}
	}
	return true;
}

BUD_INLINE bool Matrix5::operator==( const Matrix5& a ) const
{
	return Compare( a );
}

BUD_INLINE bool Matrix5::operator!=( const Matrix5& a ) const
{
	return !Compare( a );
}

BUD_INLINE void Matrix5::Zero()
{
	memset( mat, 0, sizeof( Matrix5 ) );
}

BUD_INLINE void Matrix5::Identity()
{
	*this = mat5_identity;
}

BUD_INLINE bool Matrix5::IsIdentity( const float epsilon ) const
{
	return Compare( mat5_identity, epsilon );
}

BUD_INLINE bool Matrix5::IsSymmetric( const float epsilon ) const
{
	for( int i = 1; i < 5; i++ )
	{
		for( int j = 0; j < i; j++ )
		{
			if( Math::Fabs( mat[i][j] - mat[j][i] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE bool Matrix5::IsDiagonal( const float epsilon ) const
{
	for( int i = 0; i < 5; i++ )
	{
		for( int j = 0; j < 5; j++ )
		{
			if( i != j && Math::Fabs( mat[i][j] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE float Matrix5::Trace() const
{
	return ( mat[0][0] + mat[1][1] + mat[2][2] + mat[3][3] + mat[4][4] );
}

BUD_INLINE Matrix5 Matrix5::Inverse() const
{
	Matrix5 invMat;
	
	invMat = *this;
	return invMat;
}

BUD_INLINE Matrix5 Matrix5::InverseFast() const
{
	Matrix5 invMat;
	
	invMat = *this;
	return invMat;
}

BUD_INLINE int Matrix5::GetDimension() const
{
	return 25;
}

BUD_INLINE const float* Matrix5::ToFloatPtr() const
{
	return mat[0].ToFloatPtr();
}

BUD_INLINE float* Matrix5::ToFloatPtr()
{
	return mat[0].ToFloatPtr();
}


//===============================================================
//
//	Matrix6 - 6x6 matrix
//
//===============================================================

class Matrix6
{
public:
	Matrix6();
	explicit Matrix6( const Vector6& v0, const Vector6& v1, const Vector6& v2, const Vector6& v3, const Vector6& v4, const Vector6& v5 );
	explicit Matrix6( const Matrix3& m0, const Matrix3& m1, const Matrix3& m2, const Matrix3& m3 );
	explicit Matrix6( const float src[ 6 ][ 6 ] );
	
	const Vector6& 	operator[]( int index ) const;
	Vector6& 		operator[]( int index );
	Matrix6			operator*( const float a ) const;
	Vector6			operator*( const Vector6& vec ) const;
	Matrix6			operator*( const Matrix6& a ) const;
	Matrix6			operator+( const Matrix6& a ) const;
	Matrix6			operator-( const Matrix6& a ) const;
	Matrix6& 		operator*=( const float a );
	Matrix6& 		operator*=( const Matrix6& a );
	Matrix6& 		operator+=( const Matrix6& a );
	Matrix6& 		operator-=( const Matrix6& a );
	
	friend Matrix6	operator*( const float a, const Matrix6& mat );
	friend Vector6	operator*( const Vector6& vec, const Matrix6& mat );
	friend Vector6& 	operator*=( Vector6& vec, const Matrix6& mat );
	
	bool			Compare( const Matrix6& a ) const;						// exact compare, no epsilon
	bool			Compare( const Matrix6& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==( const Matrix6& a ) const;					// exact compare, no epsilon
	bool			operator!=( const Matrix6& a ) const;					// exact compare, no epsilon
	
	void			Zero();
	void			Identity();
	bool			IsIdentity( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetric( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	
	Matrix3			SubMat3( int n ) const;
	float			Trace() const;
	float			Determinant() const;
	Matrix6			Transpose() const;	// returns transpose
	Matrix6& 		TransposeSelf();
	Matrix6			Inverse() const;		// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseSelf();		// returns false if determinant is zero
	Matrix6			InverseFast() const;	// returns the inverse ( m * m.Inverse() = identity )
	bool			InverseFastSelf();	// returns false if determinant is zero
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	Vector6			mat[ 6 ];
};

extern Matrix6 mat6_zero;
extern Matrix6 mat6_identity;
#define mat6_default	mat6_identity

BUD_INLINE Matrix6::Matrix6()
{
}

BUD_INLINE Matrix6::Matrix6( const Matrix3& m0, const Matrix3& m1, const Matrix3& m2, const Matrix3& m3 )
{
	mat[0] = Vector6( m0[0][0], m0[0][1], m0[0][2], m1[0][0], m1[0][1], m1[0][2] );
	mat[1] = Vector6( m0[1][0], m0[1][1], m0[1][2], m1[1][0], m1[1][1], m1[1][2] );
	mat[2] = Vector6( m0[2][0], m0[2][1], m0[2][2], m1[2][0], m1[2][1], m1[2][2] );
	mat[3] = Vector6( m2[0][0], m2[0][1], m2[0][2], m3[0][0], m3[0][1], m3[0][2] );
	mat[4] = Vector6( m2[1][0], m2[1][1], m2[1][2], m3[1][0], m3[1][1], m3[1][2] );
	mat[5] = Vector6( m2[2][0], m2[2][1], m2[2][2], m3[2][0], m3[2][1], m3[2][2] );
}

BUD_INLINE Matrix6::Matrix6( const Vector6& v0, const Vector6& v1, const Vector6& v2, const Vector6& v3, const Vector6& v4, const Vector6& v5 )
{
	mat[0] = v0;
	mat[1] = v1;
	mat[2] = v2;
	mat[3] = v3;
	mat[4] = v4;
	mat[5] = v5;
}

BUD_INLINE Matrix6::Matrix6( const float src[ 6 ][ 6 ] )
{
	memcpy( mat, src, 6 * 6 * sizeof( float ) );
}

BUD_INLINE const Vector6& Matrix6::operator[]( int index ) const
{
	//assert( ( index >= 0 ) && ( index < 6 ) );
	return mat[ index ];
}

BUD_INLINE Vector6& Matrix6::operator[]( int index )
{
	//assert( ( index >= 0 ) && ( index < 6 ) );
	return mat[ index ];
}

BUD_INLINE Matrix6 Matrix6::operator*( const Matrix6& a ) const
{
	int i, j;
	const float* m1Ptr, *m2Ptr;
	float* dstPtr;
	Matrix6 dst;
	
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

BUD_INLINE Matrix6 Matrix6::operator*( const float a ) const
{
	return Matrix6(
			   Vector6( mat[0][0] * a, mat[0][1] * a, mat[0][2] * a, mat[0][3] * a, mat[0][4] * a, mat[0][5] * a ),
			   Vector6( mat[1][0] * a, mat[1][1] * a, mat[1][2] * a, mat[1][3] * a, mat[1][4] * a, mat[1][5] * a ),
			   Vector6( mat[2][0] * a, mat[2][1] * a, mat[2][2] * a, mat[2][3] * a, mat[2][4] * a, mat[2][5] * a ),
			   Vector6( mat[3][0] * a, mat[3][1] * a, mat[3][2] * a, mat[3][3] * a, mat[3][4] * a, mat[3][5] * a ),
			   Vector6( mat[4][0] * a, mat[4][1] * a, mat[4][2] * a, mat[4][3] * a, mat[4][4] * a, mat[4][5] * a ),
			   Vector6( mat[5][0] * a, mat[5][1] * a, mat[5][2] * a, mat[5][3] * a, mat[5][4] * a, mat[5][5] * a ) );
}

BUD_INLINE Vector6 Matrix6::operator*( const Vector6& vec ) const
{
	return Vector6(
			   mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2] + mat[0][3] * vec[3] + mat[0][4] * vec[4] + mat[0][5] * vec[5],
			   mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2] + mat[1][3] * vec[3] + mat[1][4] * vec[4] + mat[1][5] * vec[5],
			   mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2] + mat[2][3] * vec[3] + mat[2][4] * vec[4] + mat[2][5] * vec[5],
			   mat[3][0] * vec[0] + mat[3][1] * vec[1] + mat[3][2] * vec[2] + mat[3][3] * vec[3] + mat[3][4] * vec[4] + mat[3][5] * vec[5],
			   mat[4][0] * vec[0] + mat[4][1] * vec[1] + mat[4][2] * vec[2] + mat[4][3] * vec[3] + mat[4][4] * vec[4] + mat[4][5] * vec[5],
			   mat[5][0] * vec[0] + mat[5][1] * vec[1] + mat[5][2] * vec[2] + mat[5][3] * vec[3] + mat[5][4] * vec[4] + mat[5][5] * vec[5] );
}

BUD_INLINE Matrix6 Matrix6::operator+( const Matrix6& a ) const
{
	return Matrix6(
			   Vector6( mat[0][0] + a[0][0], mat[0][1] + a[0][1], mat[0][2] + a[0][2], mat[0][3] + a[0][3], mat[0][4] + a[0][4], mat[0][5] + a[0][5] ),
			   Vector6( mat[1][0] + a[1][0], mat[1][1] + a[1][1], mat[1][2] + a[1][2], mat[1][3] + a[1][3], mat[1][4] + a[1][4], mat[1][5] + a[1][5] ),
			   Vector6( mat[2][0] + a[2][0], mat[2][1] + a[2][1], mat[2][2] + a[2][2], mat[2][3] + a[2][3], mat[2][4] + a[2][4], mat[2][5] + a[2][5] ),
			   Vector6( mat[3][0] + a[3][0], mat[3][1] + a[3][1], mat[3][2] + a[3][2], mat[3][3] + a[3][3], mat[3][4] + a[3][4], mat[3][5] + a[3][5] ),
			   Vector6( mat[4][0] + a[4][0], mat[4][1] + a[4][1], mat[4][2] + a[4][2], mat[4][3] + a[4][3], mat[4][4] + a[4][4], mat[4][5] + a[4][5] ),
			   Vector6( mat[5][0] + a[5][0], mat[5][1] + a[5][1], mat[5][2] + a[5][2], mat[5][3] + a[5][3], mat[5][4] + a[5][4], mat[5][5] + a[5][5] ) );
}

BUD_INLINE Matrix6 Matrix6::operator-( const Matrix6& a ) const
{
	return Matrix6(
			   Vector6( mat[0][0] - a[0][0], mat[0][1] - a[0][1], mat[0][2] - a[0][2], mat[0][3] - a[0][3], mat[0][4] - a[0][4], mat[0][5] - a[0][5] ),
			   Vector6( mat[1][0] - a[1][0], mat[1][1] - a[1][1], mat[1][2] - a[1][2], mat[1][3] - a[1][3], mat[1][4] - a[1][4], mat[1][5] - a[1][5] ),
			   Vector6( mat[2][0] - a[2][0], mat[2][1] - a[2][1], mat[2][2] - a[2][2], mat[2][3] - a[2][3], mat[2][4] - a[2][4], mat[2][5] - a[2][5] ),
			   Vector6( mat[3][0] - a[3][0], mat[3][1] - a[3][1], mat[3][2] - a[3][2], mat[3][3] - a[3][3], mat[3][4] - a[3][4], mat[3][5] - a[3][5] ),
			   Vector6( mat[4][0] - a[4][0], mat[4][1] - a[4][1], mat[4][2] - a[4][2], mat[4][3] - a[4][3], mat[4][4] - a[4][4], mat[4][5] - a[4][5] ),
			   Vector6( mat[5][0] - a[5][0], mat[5][1] - a[5][1], mat[5][2] - a[5][2], mat[5][3] - a[5][3], mat[5][4] - a[5][4], mat[5][5] - a[5][5] ) );
}

BUD_INLINE Matrix6& Matrix6::operator*=( const float a )
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

BUD_INLINE Matrix6& Matrix6::operator*=( const Matrix6& a )
{
	*this = *this * a;
	return *this;
}

BUD_INLINE Matrix6& Matrix6::operator+=( const Matrix6& a )
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

BUD_INLINE Matrix6& Matrix6::operator-=( const Matrix6& a )
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

BUD_INLINE Vector6 operator*( const Vector6& vec, const Matrix6& mat )
{
	return mat * vec;
}

BUD_INLINE Matrix6 operator*( const float a, Matrix6 const& mat )
{
	return mat * a;
}

BUD_INLINE Vector6& operator*=( Vector6& vec, const Matrix6& mat )
{
	vec = mat * vec;
	return vec;
}

BUD_INLINE bool Matrix6::Compare( const Matrix6& a ) const
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

BUD_INLINE bool Matrix6::Compare( const Matrix6& a, const float epsilon ) const
{
	dword i;
	const float* ptr1, *ptr2;
	
	ptr1 = reinterpret_cast<const float*>( mat );
	ptr2 = reinterpret_cast<const float*>( a.mat );
	for( i = 0; i < 6 * 6; i++ )
	{
		if( Math::Fabs( ptr1[i] - ptr2[i] ) > epsilon )
		{
			return false;
		}
	}
	return true;
}

BUD_INLINE bool Matrix6::operator==( const Matrix6& a ) const
{
	return Compare( a );
}

BUD_INLINE bool Matrix6::operator!=( const Matrix6& a ) const
{
	return !Compare( a );
}

BUD_INLINE void Matrix6::Zero()
{
	memset( mat, 0, sizeof( Matrix6 ) );
}

BUD_INLINE void Matrix6::Identity()
{
	*this = mat6_identity;
}

BUD_INLINE bool Matrix6::IsIdentity( const float epsilon ) const
{
	return Compare( mat6_identity, epsilon );
}

BUD_INLINE bool Matrix6::IsSymmetric( const float epsilon ) const
{
	for( int i = 1; i < 6; i++ )
	{
		for( int j = 0; j < i; j++ )
		{
			if( Math::Fabs( mat[i][j] - mat[j][i] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE bool Matrix6::IsDiagonal( const float epsilon ) const
{
	for( int i = 0; i < 6; i++ )
	{
		for( int j = 0; j < 6; j++ )
		{
			if( i != j && Math::Fabs( mat[i][j] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

BUD_INLINE Matrix3 Matrix6::SubMat3( int n ) const
{
	assert( n >= 0 && n < 4 );
	int b0 = ( ( n & 2 ) >> 1 ) * 3;
	int b1 = ( n & 1 ) * 3;
	return Matrix3(
			   mat[b0 + 0][b1 + 0], mat[b0 + 0][b1 + 1], mat[b0 + 0][b1 + 2],
			   mat[b0 + 1][b1 + 0], mat[b0 + 1][b1 + 1], mat[b0 + 1][b1 + 2],
			   mat[b0 + 2][b1 + 0], mat[b0 + 2][b1 + 1], mat[b0 + 2][b1 + 2] );
}

BUD_INLINE float Matrix6::Trace() const
{
	return ( mat[0][0] + mat[1][1] + mat[2][2] + mat[3][3] + mat[4][4] + mat[5][5] );
}

BUD_INLINE Matrix6 Matrix6::Inverse() const
{
	Matrix6 invMat;
	
	invMat = *this;
	return invMat;
}

BUD_INLINE Matrix6 Matrix6::InverseFast() const
{
	Matrix6 invMat;
	
	invMat = *this;
	return invMat;
}

BUD_INLINE int Matrix6::GetDimension() const
{
	return 36;
}

BUD_INLINE const float* Matrix6::ToFloatPtr() const
{
	return mat[0].ToFloatPtr();
}

BUD_INLINE float* Matrix6::ToFloatPtr()
{
	return mat[0].ToFloatPtr();
}

#endif /* !__MATH_MATRIX_H__ */
