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

#ifndef __JOINTTRANSFORM_H__
#define __JOINTTRANSFORM_H__

/*
===============================================================================

  Joint Quaternion

===============================================================================
*/

class budJointQuat
{
public:
	const float* 	ToFloatPtr() const
	{
		return q.ToFloatPtr();
	}
	float* 			ToFloatPtr()
	{
		return q.ToFloatPtr();
	}
	
	idQuat			q;
	budVec3			t;
	float			w;
};

// offsets for SIMD code
#define JOINTQUAT_SIZE				(8*4)		// sizeof( budJointQuat )
#define JOINTQUAT_SIZE_SHIFT		5			// log2( sizeof( budJointQuat ) )
#define JOINTQUAT_Q_OFFSET			(0*4)		// offsetof( budJointQuat, q )
#define JOINTQUAT_T_OFFSET			(4*4)		// offsetof( budJointQuat, t )

assert_sizeof( budJointQuat, JOINTQUAT_SIZE );
assert_sizeof( budJointQuat, ( 1 << JOINTQUAT_SIZE_SHIFT ) );
assert_offsetof( budJointQuat, q, JOINTQUAT_Q_OFFSET );
assert_offsetof( budJointQuat, t, JOINTQUAT_T_OFFSET );

/*
===============================================================================

	Joint Matrix

===============================================================================
*/

/*
================================================
budJointMat has the following structure:

	budMat3 m;
	budVec3 t;

	m[0][0], m[1][0], m[2][0], t[0]
	m[0][1], m[1][1], m[2][1], t[1]
	m[0][2], m[1][2], m[2][2], t[2]

================================================
*/
class budJointMat
{
public:

	void			SetRotation( const budMat3& m );
	budMat3			GetRotation() const;
	void			SetTranslation( const budVec3& t );
	budVec3			GetTranslation() const;
	
	budVec3			operator*( const budVec3& v ) const;							// only rotate
	budVec3			operator*( const budVec4& v ) const;							// rotate and translate
	
	budJointMat& 	operator*=( const budJointMat& a );							// transform
	budJointMat& 	operator/=( const budJointMat& a );							// untransform
	
	bool			Compare( const budJointMat& a ) const;						// exact compare, no epsilon
	bool			Compare( const budJointMat& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const budJointMat& a ) const;					// exact compare, no epsilon
	bool			operator!=(	const budJointMat& a ) const;					// exact compare, no epsilon
	
	void			Identity();
	void			Invert();
	
	void			FromMat4( const budMat4& m );
	
	budMat3			ToMat3() const;
	budMat4			ToMat4() const;
	budVec3			ToVec3() const;
	const float* 	ToFloatPtr() const
	{
		return mat;
	}
	float* 			ToFloatPtr()
	{
		return mat;
	}
	budJointQuat		ToJointQuat() const;
	
	void			Transform( budVec3& result, const budVec3& v ) const;
	void			Rotate( budVec3& result, const budVec3& v ) const;
	
	static void		Mul( budJointMat& result, const budJointMat& mat, const float s );
	static void		Mad( budJointMat& result, const budJointMat& mat, const float s );
	static void		Multiply( budJointMat& result, const budJointMat& m1, const budJointMat& m2 );
	static void		InverseMultiply( budJointMat& result, const budJointMat& m1, const budJointMat& m2 );
	
	float			mat[3 * 4];
};

// offsets for SIMD code
#define JOINTMAT_SIZE				(4*3*4)		// sizeof( budJointMat )
assert_sizeof( budJointMat,			JOINTMAT_SIZE );

#define JOINTMAT_TYPESIZE			( 4 * 3 )
/*
========================
budJointMat::SetRotation
========================
*/
BUD_INLINE void budJointMat::SetRotation( const budMat3& m )
{
	// NOTE: budMat3 is transposed because it is column-major
	mat[0 * 4 + 0] = m[0][0];
	mat[0 * 4 + 1] = m[1][0];
	mat[0 * 4 + 2] = m[2][0];
	mat[1 * 4 + 0] = m[0][1];
	mat[1 * 4 + 1] = m[1][1];
	mat[1 * 4 + 2] = m[2][1];
	mat[2 * 4 + 0] = m[0][2];
	mat[2 * 4 + 1] = m[1][2];
	mat[2 * 4 + 2] = m[2][2];
}

/*
========================
budJointMat::GetRotation
========================
*/
BUD_INLINE budMat3 budJointMat::GetRotation() const
{
	budMat3 m;
	m[0][0] = mat[0 * 4 + 0];
	m[1][0] = mat[0 * 4 + 1];
	m[2][0] = mat[0 * 4 + 2];
	m[0][1] = mat[1 * 4 + 0];
	m[1][1] = mat[1 * 4 + 1];
	m[2][1] = mat[1 * 4 + 2];
	m[0][2] = mat[2 * 4 + 0];
	m[1][2] = mat[2 * 4 + 1];
	m[2][2] = mat[2 * 4 + 2];
	return m;
}

/*
========================
budJointMat::SetTranslation
========================
*/
BUD_INLINE void budJointMat::SetTranslation( const budVec3& t )
{
	mat[0 * 4 + 3] = t[0];
	mat[1 * 4 + 3] = t[1];
	mat[2 * 4 + 3] = t[2];
}

/*
========================
budJointMat::GetTranslation
========================
*/
BUD_INLINE budVec3 budJointMat::GetTranslation() const
{
	budVec3 t;
	t[0] = mat[0 * 4 + 3];
	t[1] = mat[1 * 4 + 3];
	t[2] = mat[2 * 4 + 3];
	return t;
}

/*
========================
budJointMat::operator*
========================
*/
BUD_INLINE budVec3 budJointMat::operator*( const budVec3& v ) const
{
	return budVec3(	mat[0 * 4 + 0] * v[0] + mat[0 * 4 + 1] * v[1] + mat[0 * 4 + 2] * v[2],
					mat[1 * 4 + 0] * v[0] + mat[1 * 4 + 1] * v[1] + mat[1 * 4 + 2] * v[2],
					mat[2 * 4 + 0] * v[0] + mat[2 * 4 + 1] * v[1] + mat[2 * 4 + 2] * v[2] );
}

BUD_INLINE budVec3 budJointMat::operator*( const budVec4& v ) const
{
	return budVec3(	mat[0 * 4 + 0] * v[0] + mat[0 * 4 + 1] * v[1] + mat[0 * 4 + 2] * v[2] + mat[0 * 4 + 3] * v[3],
					mat[1 * 4 + 0] * v[0] + mat[1 * 4 + 1] * v[1] + mat[1 * 4 + 2] * v[2] + mat[1 * 4 + 3] * v[3],
					mat[2 * 4 + 0] * v[0] + mat[2 * 4 + 1] * v[1] + mat[2 * 4 + 2] * v[2] + mat[2 * 4 + 3] * v[3] );
}

/*
========================
budJointMat::operator*=
========================
*/
BUD_INLINE budJointMat& budJointMat::operator*=( const budJointMat& a )
{
	float tmp[3];
	
	tmp[0] = mat[0 * 4 + 0] * a.mat[0 * 4 + 0] + mat[1 * 4 + 0] * a.mat[0 * 4 + 1] + mat[2 * 4 + 0] * a.mat[0 * 4 + 2];
	tmp[1] = mat[0 * 4 + 0] * a.mat[1 * 4 + 0] + mat[1 * 4 + 0] * a.mat[1 * 4 + 1] + mat[2 * 4 + 0] * a.mat[1 * 4 + 2];
	tmp[2] = mat[0 * 4 + 0] * a.mat[2 * 4 + 0] + mat[1 * 4 + 0] * a.mat[2 * 4 + 1] + mat[2 * 4 + 0] * a.mat[2 * 4 + 2];
	mat[0 * 4 + 0] = tmp[0];
	mat[1 * 4 + 0] = tmp[1];
	mat[2 * 4 + 0] = tmp[2];
	
	tmp[0] = mat[0 * 4 + 1] * a.mat[0 * 4 + 0] + mat[1 * 4 + 1] * a.mat[0 * 4 + 1] + mat[2 * 4 + 1] * a.mat[0 * 4 + 2];
	tmp[1] = mat[0 * 4 + 1] * a.mat[1 * 4 + 0] + mat[1 * 4 + 1] * a.mat[1 * 4 + 1] + mat[2 * 4 + 1] * a.mat[1 * 4 + 2];
	tmp[2] = mat[0 * 4 + 1] * a.mat[2 * 4 + 0] + mat[1 * 4 + 1] * a.mat[2 * 4 + 1] + mat[2 * 4 + 1] * a.mat[2 * 4 + 2];
	mat[0 * 4 + 1] = tmp[0];
	mat[1 * 4 + 1] = tmp[1];
	mat[2 * 4 + 1] = tmp[2];
	
	tmp[0] = mat[0 * 4 + 2] * a.mat[0 * 4 + 0] + mat[1 * 4 + 2] * a.mat[0 * 4 + 1] + mat[2 * 4 + 2] * a.mat[0 * 4 + 2];
	tmp[1] = mat[0 * 4 + 2] * a.mat[1 * 4 + 0] + mat[1 * 4 + 2] * a.mat[1 * 4 + 1] + mat[2 * 4 + 2] * a.mat[1 * 4 + 2];
	tmp[2] = mat[0 * 4 + 2] * a.mat[2 * 4 + 0] + mat[1 * 4 + 2] * a.mat[2 * 4 + 1] + mat[2 * 4 + 2] * a.mat[2 * 4 + 2];
	mat[0 * 4 + 2] = tmp[0];
	mat[1 * 4 + 2] = tmp[1];
	mat[2 * 4 + 2] = tmp[2];
	
	tmp[0] = mat[0 * 4 + 3] * a.mat[0 * 4 + 0] + mat[1 * 4 + 3] * a.mat[0 * 4 + 1] + mat[2 * 4 + 3] * a.mat[0 * 4 + 2];
	tmp[1] = mat[0 * 4 + 3] * a.mat[1 * 4 + 0] + mat[1 * 4 + 3] * a.mat[1 * 4 + 1] + mat[2 * 4 + 3] * a.mat[1 * 4 + 2];
	tmp[2] = mat[0 * 4 + 3] * a.mat[2 * 4 + 0] + mat[1 * 4 + 3] * a.mat[2 * 4 + 1] + mat[2 * 4 + 3] * a.mat[2 * 4 + 2];
	mat[0 * 4 + 3] = tmp[0];
	mat[1 * 4 + 3] = tmp[1];
	mat[2 * 4 + 3] = tmp[2];
	
	mat[0 * 4 + 3] += a.mat[0 * 4 + 3];
	mat[1 * 4 + 3] += a.mat[1 * 4 + 3];
	mat[2 * 4 + 3] += a.mat[2 * 4 + 3];
	
	return *this;
}

/*
========================
budJointMat::operator/=
========================
*/
BUD_INLINE budJointMat& budJointMat::operator/=( const budJointMat& a )
{
	float tmp[3];
	
	mat[0 * 4 + 3] -= a.mat[0 * 4 + 3];
	mat[1 * 4 + 3] -= a.mat[1 * 4 + 3];
	mat[2 * 4 + 3] -= a.mat[2 * 4 + 3];
	
	tmp[0] = mat[0 * 4 + 0] * a.mat[0 * 4 + 0] + mat[1 * 4 + 0] * a.mat[1 * 4 + 0] + mat[2 * 4 + 0] * a.mat[2 * 4 + 0];
	tmp[1] = mat[0 * 4 + 0] * a.mat[0 * 4 + 1] + mat[1 * 4 + 0] * a.mat[1 * 4 + 1] + mat[2 * 4 + 0] * a.mat[2 * 4 + 1];
	tmp[2] = mat[0 * 4 + 0] * a.mat[0 * 4 + 2] + mat[1 * 4 + 0] * a.mat[1 * 4 + 2] + mat[2 * 4 + 0] * a.mat[2 * 4 + 2];
	mat[0 * 4 + 0] = tmp[0];
	mat[1 * 4 + 0] = tmp[1];
	mat[2 * 4 + 0] = tmp[2];
	
	tmp[0] = mat[0 * 4 + 1] * a.mat[0 * 4 + 0] + mat[1 * 4 + 1] * a.mat[1 * 4 + 0] + mat[2 * 4 + 1] * a.mat[2 * 4 + 0];
	tmp[1] = mat[0 * 4 + 1] * a.mat[0 * 4 + 1] + mat[1 * 4 + 1] * a.mat[1 * 4 + 1] + mat[2 * 4 + 1] * a.mat[2 * 4 + 1];
	tmp[2] = mat[0 * 4 + 1] * a.mat[0 * 4 + 2] + mat[1 * 4 + 1] * a.mat[1 * 4 + 2] + mat[2 * 4 + 1] * a.mat[2 * 4 + 2];
	mat[0 * 4 + 1] = tmp[0];
	mat[1 * 4 + 1] = tmp[1];
	mat[2 * 4 + 1] = tmp[2];
	
	tmp[0] = mat[0 * 4 + 2] * a.mat[0 * 4 + 0] + mat[1 * 4 + 2] * a.mat[1 * 4 + 0] + mat[2 * 4 + 2] * a.mat[2 * 4 + 0];
	tmp[1] = mat[0 * 4 + 2] * a.mat[0 * 4 + 1] + mat[1 * 4 + 2] * a.mat[1 * 4 + 1] + mat[2 * 4 + 2] * a.mat[2 * 4 + 1];
	tmp[2] = mat[0 * 4 + 2] * a.mat[0 * 4 + 2] + mat[1 * 4 + 2] * a.mat[1 * 4 + 2] + mat[2 * 4 + 2] * a.mat[2 * 4 + 2];
	mat[0 * 4 + 2] = tmp[0];
	mat[1 * 4 + 2] = tmp[1];
	mat[2 * 4 + 2] = tmp[2];
	
	tmp[0] = mat[0 * 4 + 3] * a.mat[0 * 4 + 0] + mat[1 * 4 + 3] * a.mat[1 * 4 + 0] + mat[2 * 4 + 3] * a.mat[2 * 4 + 0];
	tmp[1] = mat[0 * 4 + 3] * a.mat[0 * 4 + 1] + mat[1 * 4 + 3] * a.mat[1 * 4 + 1] + mat[2 * 4 + 3] * a.mat[2 * 4 + 1];
	tmp[2] = mat[0 * 4 + 3] * a.mat[0 * 4 + 2] + mat[1 * 4 + 3] * a.mat[1 * 4 + 2] + mat[2 * 4 + 3] * a.mat[2 * 4 + 2];
	mat[0 * 4 + 3] = tmp[0];
	mat[1 * 4 + 3] = tmp[1];
	mat[2 * 4 + 3] = tmp[2];
	
	return *this;
}

/*
========================
budJointMat::Compare
========================
*/
BUD_INLINE bool budJointMat::Compare( const budJointMat& a ) const
{
	int i;
	
	for( i = 0; i < 12; i++ )
	{
		if( mat[i] != a.mat[i] )
		{
			return false;
		}
	}
	return true;
}

/*
========================
budJointMat::Compare
========================
*/
BUD_INLINE bool budJointMat::Compare( const budJointMat& a, const float epsilon ) const
{
	int i;
	
	for( i = 0; i < 12; i++ )
	{
		if( budMath::Fabs( mat[i] - a.mat[i] ) > epsilon )
		{
			return false;
		}
	}
	return true;
}

/*
========================
budJointMat::operator==
========================
*/
BUD_INLINE bool budJointMat::operator==( const budJointMat& a ) const
{
	return Compare( a );
}

/*
========================
budJointMat::operator!=
========================
*/
BUD_INLINE bool budJointMat::operator!=( const budJointMat& a ) const
{
	return !Compare( a );
}

/*
========================
budJointMat::Identity
========================
*/
BUD_INLINE void budJointMat::Identity()
{
	mat[0 * 4 + 0] = 1.0f;
	mat[0 * 4 + 1] = 0.0f;
	mat[0 * 4 + 2] = 0.0f;
	mat[0 * 4 + 3] = 0.0f;
	mat[1 * 4 + 0] = 0.0f;
	mat[1 * 4 + 1] = 1.0f;
	mat[1 * 4 + 2] = 0.0f;
	mat[1 * 4 + 3] = 0.0f;
	mat[2 * 4 + 0] = 0.0f;
	mat[2 * 4 + 1] = 0.0f;
	mat[2 * 4 + 2] = 1.0f;
	mat[2 * 4 + 3] = 0.0f;
}

/*
========================
budJointMat::Invert
========================
*/
BUD_INLINE void budJointMat::Invert()
{
	float tmp[3];
	
	// negate inverse rotated translation part
	tmp[0] = mat[0 * 4 + 0] * mat[0 * 4 + 3] + mat[1 * 4 + 0] * mat[1 * 4 + 3] + mat[2 * 4 + 0] * mat[2 * 4 + 3];
	tmp[1] = mat[0 * 4 + 1] * mat[0 * 4 + 3] + mat[1 * 4 + 1] * mat[1 * 4 + 3] + mat[2 * 4 + 1] * mat[2 * 4 + 3];
	tmp[2] = mat[0 * 4 + 2] * mat[0 * 4 + 3] + mat[1 * 4 + 2] * mat[1 * 4 + 3] + mat[2 * 4 + 2] * mat[2 * 4 + 3];
	mat[0 * 4 + 3] = -tmp[0];
	mat[1 * 4 + 3] = -tmp[1];
	mat[2 * 4 + 3] = -tmp[2];
	
	// transpose rotation part
	tmp[0] = mat[0 * 4 + 1];
	mat[0 * 4 + 1] = mat[1 * 4 + 0];
	mat[1 * 4 + 0] = tmp[0];
	tmp[1] = mat[0 * 4 + 2];
	mat[0 * 4 + 2] = mat[2 * 4 + 0];
	mat[2 * 4 + 0] = tmp[1];
	tmp[2] = mat[1 * 4 + 2];
	mat[1 * 4 + 2] = mat[2 * 4 + 1];
	mat[2 * 4 + 1] = tmp[2];
}

/*
========================
budJointMat::ToMat3
========================
*/
BUD_INLINE budMat3 budJointMat::ToMat3() const
{
	return budMat3(	mat[0 * 4 + 0], mat[1 * 4 + 0], mat[2 * 4 + 0],
					mat[0 * 4 + 1], mat[1 * 4 + 1], mat[2 * 4 + 1],
					mat[0 * 4 + 2], mat[1 * 4 + 2], mat[2 * 4 + 2] );
}

/*
========================
budJointMat::ToMat4
========================
*/
BUD_INLINE budMat4 budJointMat::ToMat4() const
{
	return budMat4(
			   mat[0 * 4 + 0], mat[0 * 4 + 1], mat[0 * 4 + 2], mat[0 * 4 + 3],
			   mat[1 * 4 + 0], mat[1 * 4 + 1], mat[1 * 4 + 2], mat[1 * 4 + 3],
			   mat[2 * 4 + 0], mat[2 * 4 + 1], mat[2 * 4 + 2], mat[2 * 4 + 3],
			   0.0f, 0.0f, 0.0f, 1.0f
		   );
}

/*
========================
budJointMat::FromMat4
========================
*/
BUD_INLINE void budJointMat::FromMat4( const budMat4& m )
{
	mat[0 * 4 + 0] = m[0][0], mat[0 * 4 + 1] = m[0][1], mat[0 * 4 + 2] = m[0][2], mat[0 * 4 + 3] = m[0][3];
	mat[1 * 4 + 0] = m[1][0], mat[1 * 4 + 1] = m[1][1], mat[1 * 4 + 2] = m[1][2], mat[1 * 4 + 3] = m[1][3];
	mat[2 * 4 + 0] = m[2][0], mat[2 * 4 + 1] = m[2][1], mat[2 * 4 + 2] = m[2][2], mat[2 * 4 + 3] = m[2][3];
	assert( m[3][0] == 0.0f );
	assert( m[3][1] == 0.0f );
	assert( m[3][2] == 0.0f );
	assert( m[3][3] == 1.0f );
}

/*
========================
budJointMat::ToVec3
========================
*/
BUD_INLINE budVec3 budJointMat::ToVec3() const
{
	return budVec3( mat[0 * 4 + 3], mat[1 * 4 + 3], mat[2 * 4 + 3] );
}

/*
========================
budJointMat::Transform
========================
*/
BUD_INLINE void budJointMat::Transform( budVec3& result, const budVec3& v ) const
{
	result.x = mat[0 * 4 + 0] * v.x + mat[0 * 4 + 1] * v.y + mat[0 * 4 + 2] * v.z + mat[0 * 4 + 3];
	result.y = mat[1 * 4 + 0] * v.x + mat[1 * 4 + 1] * v.y + mat[1 * 4 + 2] * v.z + mat[1 * 4 + 3];
	result.z = mat[2 * 4 + 0] * v.x + mat[2 * 4 + 1] * v.y + mat[2 * 4 + 2] * v.z + mat[2 * 4 + 3];
}

/*
========================
budJointMat::Rotate
========================
*/
BUD_INLINE void budJointMat::Rotate( budVec3& result, const budVec3& v ) const
{
	result.x = mat[0 * 4 + 0] * v.x + mat[0 * 4 + 1] * v.y + mat[0 * 4 + 2] * v.z;
	result.y = mat[1 * 4 + 0] * v.x + mat[1 * 4 + 1] * v.y + mat[1 * 4 + 2] * v.z;
	result.z = mat[2 * 4 + 0] * v.x + mat[2 * 4 + 1] * v.y + mat[2 * 4 + 2] * v.z;
}

/*
========================
budJointMat::Mul
========================
*/
BUD_INLINE void budJointMat::Mul( budJointMat& result, const budJointMat& mat, const float s )
{
	result.mat[0 * 4 + 0] = s * mat.mat[0 * 4 + 0];
	result.mat[0 * 4 + 1] = s * mat.mat[0 * 4 + 1];
	result.mat[0 * 4 + 2] = s * mat.mat[0 * 4 + 2];
	result.mat[0 * 4 + 3] = s * mat.mat[0 * 4 + 3];
	result.mat[1 * 4 + 0] = s * mat.mat[1 * 4 + 0];
	result.mat[1 * 4 + 1] = s * mat.mat[1 * 4 + 1];
	result.mat[1 * 4 + 2] = s * mat.mat[1 * 4 + 2];
	result.mat[1 * 4 + 3] = s * mat.mat[1 * 4 + 3];
	result.mat[2 * 4 + 0] = s * mat.mat[2 * 4 + 0];
	result.mat[2 * 4 + 1] = s * mat.mat[2 * 4 + 1];
	result.mat[2 * 4 + 2] = s * mat.mat[2 * 4 + 2];
	result.mat[2 * 4 + 3] = s * mat.mat[2 * 4 + 3];
}

/*
========================
budJointMat::Mad
========================
*/
BUD_INLINE void budJointMat::Mad( budJointMat& result, const budJointMat& mat, const float s )
{
	result.mat[0 * 4 + 0] += s * mat.mat[0 * 4 + 0];
	result.mat[0 * 4 + 1] += s * mat.mat[0 * 4 + 1];
	result.mat[0 * 4 + 2] += s * mat.mat[0 * 4 + 2];
	result.mat[0 * 4 + 3] += s * mat.mat[0 * 4 + 3];
	result.mat[1 * 4 + 0] += s * mat.mat[1 * 4 + 0];
	result.mat[1 * 4 + 1] += s * mat.mat[1 * 4 + 1];
	result.mat[1 * 4 + 2] += s * mat.mat[1 * 4 + 2];
	result.mat[1 * 4 + 3] += s * mat.mat[1 * 4 + 3];
	result.mat[2 * 4 + 0] += s * mat.mat[2 * 4 + 0];
	result.mat[2 * 4 + 1] += s * mat.mat[2 * 4 + 1];
	result.mat[2 * 4 + 2] += s * mat.mat[2 * 4 + 2];
	result.mat[2 * 4 + 3] += s * mat.mat[2 * 4 + 3];
}

/*
========================
budJointMat::Multiply
========================
*/
BUD_INLINE void budJointMat::Multiply( budJointMat& result, const budJointMat& m1, const budJointMat& m2 )
{
	result.mat[0 * 4 + 0] = m1.mat[0 * 4 + 0] * m2.mat[0 * 4 + 0] + m1.mat[0 * 4 + 1] * m2.mat[1 * 4 + 0] + m1.mat[0 * 4 + 2] * m2.mat[2 * 4 + 0];
	result.mat[0 * 4 + 1] = m1.mat[0 * 4 + 0] * m2.mat[0 * 4 + 1] + m1.mat[0 * 4 + 1] * m2.mat[1 * 4 + 1] + m1.mat[0 * 4 + 2] * m2.mat[2 * 4 + 1];
	result.mat[0 * 4 + 2] = m1.mat[0 * 4 + 0] * m2.mat[0 * 4 + 2] + m1.mat[0 * 4 + 1] * m2.mat[1 * 4 + 2] + m1.mat[0 * 4 + 2] * m2.mat[2 * 4 + 2];
	result.mat[0 * 4 + 3] = m1.mat[0 * 4 + 0] * m2.mat[0 * 4 + 3] + m1.mat[0 * 4 + 1] * m2.mat[1 * 4 + 3] + m1.mat[0 * 4 + 2] * m2.mat[2 * 4 + 3] + m1.mat[0 * 4 + 3];
	
	result.mat[1 * 4 + 0] = m1.mat[1 * 4 + 0] * m2.mat[0 * 4 + 0] + m1.mat[1 * 4 + 1] * m2.mat[1 * 4 + 0] + m1.mat[1 * 4 + 2] * m2.mat[2 * 4 + 0];
	result.mat[1 * 4 + 1] = m1.mat[1 * 4 + 0] * m2.mat[0 * 4 + 1] + m1.mat[1 * 4 + 1] * m2.mat[1 * 4 + 1] + m1.mat[1 * 4 + 2] * m2.mat[2 * 4 + 1];
	result.mat[1 * 4 + 2] = m1.mat[1 * 4 + 0] * m2.mat[0 * 4 + 2] + m1.mat[1 * 4 + 1] * m2.mat[1 * 4 + 2] + m1.mat[1 * 4 + 2] * m2.mat[2 * 4 + 2];
	result.mat[1 * 4 + 3] = m1.mat[1 * 4 + 0] * m2.mat[0 * 4 + 3] + m1.mat[1 * 4 + 1] * m2.mat[1 * 4 + 3] + m1.mat[1 * 4 + 2] * m2.mat[2 * 4 + 3] + m1.mat[1 * 4 + 3];
	
	result.mat[2 * 4 + 0] = m1.mat[2 * 4 + 0] * m2.mat[0 * 4 + 0] + m1.mat[2 * 4 + 1] * m2.mat[1 * 4 + 0] + m1.mat[2 * 4 + 2] * m2.mat[2 * 4 + 0];
	result.mat[2 * 4 + 1] = m1.mat[2 * 4 + 0] * m2.mat[0 * 4 + 1] + m1.mat[2 * 4 + 1] * m2.mat[1 * 4 + 1] + m1.mat[2 * 4 + 2] * m2.mat[2 * 4 + 1];
	result.mat[2 * 4 + 2] = m1.mat[2 * 4 + 0] * m2.mat[0 * 4 + 2] + m1.mat[2 * 4 + 1] * m2.mat[1 * 4 + 2] + m1.mat[2 * 4 + 2] * m2.mat[2 * 4 + 2];
	result.mat[2 * 4 + 3] = m1.mat[2 * 4 + 0] * m2.mat[0 * 4 + 3] + m1.mat[2 * 4 + 1] * m2.mat[1 * 4 + 3] + m1.mat[2 * 4 + 2] * m2.mat[2 * 4 + 3] + m1.mat[2 * 4 + 3];
}

/*
========================
budJointMat::InverseMultiply
========================
*/
BUD_INLINE void budJointMat::InverseMultiply( budJointMat& result, const budJointMat& m1, const budJointMat& m2 )
{
	float dst[3];
	
	result.mat[0 * 4 + 0] = m1.mat[0 * 4 + 0] * m2.mat[0 * 4 + 0] + m1.mat[0 * 4 + 1] * m2.mat[0 * 4 + 1] + m1.mat[0 * 4 + 2] * m2.mat[0 * 4 + 2];
	result.mat[0 * 4 + 1] = m1.mat[0 * 4 + 0] * m2.mat[1 * 4 + 0] + m1.mat[0 * 4 + 1] * m2.mat[1 * 4 + 1] + m1.mat[0 * 4 + 2] * m2.mat[1 * 4 + 2];
	result.mat[0 * 4 + 2] = m1.mat[0 * 4 + 0] * m2.mat[2 * 4 + 0] + m1.mat[0 * 4 + 1] * m2.mat[2 * 4 + 1] + m1.mat[0 * 4 + 2] * m2.mat[2 * 4 + 2];
	
	result.mat[1 * 4 + 0] = m1.mat[1 * 4 + 0] * m2.mat[0 * 4 + 0] + m1.mat[1 * 4 + 1] * m2.mat[0 * 4 + 1] + m1.mat[1 * 4 + 2] * m2.mat[0 * 4 + 2];
	result.mat[1 * 4 + 1] = m1.mat[1 * 4 + 0] * m2.mat[1 * 4 + 0] + m1.mat[1 * 4 + 1] * m2.mat[1 * 4 + 1] + m1.mat[1 * 4 + 2] * m2.mat[1 * 4 + 2];
	result.mat[1 * 4 + 2] = m1.mat[1 * 4 + 0] * m2.mat[2 * 4 + 0] + m1.mat[1 * 4 + 1] * m2.mat[2 * 4 + 1] + m1.mat[1 * 4 + 2] * m2.mat[2 * 4 + 2];
	
	result.mat[2 * 4 + 0] = m1.mat[2 * 4 + 0] * m2.mat[0 * 4 + 0] + m1.mat[2 * 4 + 1] * m2.mat[0 * 4 + 1] + m1.mat[2 * 4 + 2] * m2.mat[0 * 4 + 2];
	result.mat[2 * 4 + 1] = m1.mat[2 * 4 + 0] * m2.mat[1 * 4 + 0] + m1.mat[2 * 4 + 1] * m2.mat[1 * 4 + 1] + m1.mat[2 * 4 + 2] * m2.mat[1 * 4 + 2];
	result.mat[2 * 4 + 2] = m1.mat[2 * 4 + 0] * m2.mat[2 * 4 + 0] + m1.mat[2 * 4 + 1] * m2.mat[2 * 4 + 1] + m1.mat[2 * 4 + 2] * m2.mat[2 * 4 + 2];
	
	dst[0] = - ( m2.mat[0 * 4 + 0] * m2.mat[0 * 4 + 3] + m2.mat[1 * 4 + 0] * m2.mat[1 * 4 + 3] + m2.mat[2 * 4 + 0] * m2.mat[2 * 4 + 3] );
	dst[1] = - ( m2.mat[0 * 4 + 1] * m2.mat[0 * 4 + 3] + m2.mat[1 * 4 + 1] * m2.mat[1 * 4 + 3] + m2.mat[2 * 4 + 1] * m2.mat[2 * 4 + 3] );
	dst[2] = - ( m2.mat[0 * 4 + 2] * m2.mat[0 * 4 + 3] + m2.mat[1 * 4 + 2] * m2.mat[1 * 4 + 3] + m2.mat[2 * 4 + 2] * m2.mat[2 * 4 + 3] );
	
	result.mat[0 * 4 + 3] = m1.mat[0 * 4 + 0] * dst[0] + m1.mat[0 * 4 + 1] * dst[1] + m1.mat[0 * 4 + 2] * dst[2] + m1.mat[0 * 4 + 3];
	result.mat[1 * 4 + 3] = m1.mat[1 * 4 + 0] * dst[0] + m1.mat[1 * 4 + 1] * dst[1] + m1.mat[1 * 4 + 2] * dst[2] + m1.mat[1 * 4 + 3];
	result.mat[2 * 4 + 3] = m1.mat[2 * 4 + 0] * dst[0] + m1.mat[2 * 4 + 1] * dst[1] + m1.mat[2 * 4 + 2] * dst[2] + m1.mat[2 * 4 + 3];
}

#endif /* !__JOINTTRANSFORM_H__ */
