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

#ifndef __MATH_PLUECKER_H__
#define __MATH_PLUECKER_H__

/*
===============================================================================

	Pluecker coordinate

===============================================================================
*/

class idPluecker
{
public:
	idPluecker();
	explicit idPluecker( const float* a );
	explicit idPluecker( const budVec3& start, const budVec3& end );
	explicit idPluecker( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 );
	
	float			operator[]( const int index ) const;
	float& 			operator[]( const int index );
	idPluecker		operator-() const;											// flips the direction
	idPluecker		operator*( const float a ) const;
	idPluecker		operator/( const float a ) const;
	float			operator*( const idPluecker& a ) const;						// permuted inner product
	idPluecker		operator-( const idPluecker& a ) const;
	idPluecker		operator+( const idPluecker& a ) const;
	idPluecker& 	operator*=( const float a );
	idPluecker& 	operator/=( const float a );
	idPluecker& 	operator+=( const idPluecker& a );
	idPluecker& 	operator-=( const idPluecker& a );
	
	bool			Compare( const idPluecker& a ) const;						// exact compare, no epsilon
	bool			Compare( const idPluecker& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const idPluecker& a ) const;					// exact compare, no epsilon
	bool			operator!=(	const idPluecker& a ) const;					// exact compare, no epsilon
	
	void 			Set( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 );
	void			Zero();
	
	void			FromLine( const budVec3& start, const budVec3& end );			// pluecker from line
	void			FromRay( const budVec3& start, const budVec3& dir );			// pluecker from ray
	bool			FromPlanes( const budPlane& p1, const budPlane& p2 );			// pluecker from intersection of planes
	bool			ToLine( budVec3& start, budVec3& end ) const;					// pluecker to line
	bool			ToRay( budVec3& start, budVec3& dir ) const;					// pluecker to ray
	void			ToDir( budVec3& dir ) const;									// pluecker to direction
	float			PermutedInnerProduct( const idPluecker& a ) const;			// pluecker permuted inner product
	float			Distance3DSqr( const idPluecker& a ) const;					// pluecker line distance
	
	float			Length() const;										// pluecker length
	float			LengthSqr() const;									// pluecker squared length
	idPluecker		Normalize() const;									// pluecker normalize
	float			NormalizeSelf();										// pluecker normalize
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	float			p[6];
};

extern idPluecker pluecker_origin;
#define pluecker_zero pluecker_origin

BUD_INLINE idPluecker::idPluecker()
{
}

BUD_INLINE idPluecker::idPluecker( const float* a )
{
	memcpy( p, a, 6 * sizeof( float ) );
}

BUD_INLINE idPluecker::idPluecker( const budVec3& start, const budVec3& end )
{
	FromLine( start, end );
}

BUD_INLINE idPluecker::idPluecker( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 )
{
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

BUD_INLINE idPluecker idPluecker::operator-() const
{
	return idPluecker( -p[0], -p[1], -p[2], -p[3], -p[4], -p[5] );
}

BUD_INLINE float idPluecker::operator[]( const int index ) const
{
	return p[index];
}

BUD_INLINE float& idPluecker::operator[]( const int index )
{
	return p[index];
}

BUD_INLINE idPluecker idPluecker::operator*( const float a ) const
{
	return idPluecker( p[0] * a, p[1] * a, p[2] * a, p[3] * a, p[4] * a, p[5] * a );
}

BUD_INLINE float idPluecker::operator*( const idPluecker& a ) const
{
	return p[0] * a.p[4] + p[1] * a.p[5] + p[2] * a.p[3] + p[4] * a.p[0] + p[5] * a.p[1] + p[3] * a.p[2];
}

BUD_INLINE idPluecker idPluecker::operator/( const float a ) const
{
	float inva;
	
	assert( a != 0.0f );
	inva = 1.0f / a;
	return idPluecker( p[0] * inva, p[1] * inva, p[2] * inva, p[3] * inva, p[4] * inva, p[5] * inva );
}

BUD_INLINE idPluecker idPluecker::operator+( const idPluecker& a ) const
{
	return idPluecker( p[0] + a[0], p[1] + a[1], p[2] + a[2], p[3] + a[3], p[4] + a[4], p[5] + a[5] );
}

BUD_INLINE idPluecker idPluecker::operator-( const idPluecker& a ) const
{
	return idPluecker( p[0] - a[0], p[1] - a[1], p[2] - a[2], p[3] - a[3], p[4] - a[4], p[5] - a[5] );
}

BUD_INLINE idPluecker& idPluecker::operator*=( const float a )
{
	p[0] *= a;
	p[1] *= a;
	p[2] *= a;
	p[3] *= a;
	p[4] *= a;
	p[5] *= a;
	return *this;
}

BUD_INLINE idPluecker& idPluecker::operator/=( const float a )
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

BUD_INLINE idPluecker& idPluecker::operator+=( const idPluecker& a )
{
	p[0] += a[0];
	p[1] += a[1];
	p[2] += a[2];
	p[3] += a[3];
	p[4] += a[4];
	p[5] += a[5];
	return *this;
}

BUD_INLINE idPluecker& idPluecker::operator-=( const idPluecker& a )
{
	p[0] -= a[0];
	p[1] -= a[1];
	p[2] -= a[2];
	p[3] -= a[3];
	p[4] -= a[4];
	p[5] -= a[5];
	return *this;
}

BUD_INLINE bool idPluecker::Compare( const idPluecker& a ) const
{
	return ( ( p[0] == a[0] ) && ( p[1] == a[1] ) && ( p[2] == a[2] ) &&
			 ( p[3] == a[3] ) && ( p[4] == a[4] ) && ( p[5] == a[5] ) );
}

BUD_INLINE bool idPluecker::Compare( const idPluecker& a, const float epsilon ) const
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

BUD_INLINE bool idPluecker::operator==( const idPluecker& a ) const
{
	return Compare( a );
}

BUD_INLINE bool idPluecker::operator!=( const idPluecker& a ) const
{
	return !Compare( a );
}

BUD_INLINE void idPluecker::Set( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 )
{
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

BUD_INLINE void idPluecker::Zero()
{
	p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = 0.0f;
}

BUD_INLINE void idPluecker::FromLine( const budVec3& start, const budVec3& end )
{
	p[0] = start[0] * end[1] - end[0] * start[1];
	p[1] = start[0] * end[2] - end[0] * start[2];
	p[2] = start[0] - end[0];
	p[3] = start[1] * end[2] - end[1] * start[2];
	p[4] = start[2] - end[2];
	p[5] = end[1] - start[1];
}

BUD_INLINE void idPluecker::FromRay( const budVec3& start, const budVec3& dir )
{
	p[0] = start[0] * dir[1] - dir[0] * start[1];
	p[1] = start[0] * dir[2] - dir[0] * start[2];
	p[2] = -dir[0];
	p[3] = start[1] * dir[2] - dir[1] * start[2];
	p[4] = -dir[2];
	p[5] = dir[1];
}

BUD_INLINE bool idPluecker::ToLine( budVec3& start, budVec3& end ) const
{
	budVec3 dir1, dir2;
	float d;
	
	dir1[0] = p[3];
	dir1[1] = -p[1];
	dir1[2] = p[0];
	
	dir2[0] = -p[2];
	dir2[1] = p[5];
	dir2[2] = -p[4];
	
	d = dir2 * dir2;
	if( d == 0.0f )
	{
		return false; // pluecker coordinate does not represent a line
	}
	
	start = dir2.Cross( dir1 ) * ( 1.0f / d );
	end = start + dir2;
	return true;
}

BUD_INLINE bool idPluecker::ToRay( budVec3& start, budVec3& dir ) const
{
	budVec3 dir1;
	float d;
	
	dir1[0] = p[3];
	dir1[1] = -p[1];
	dir1[2] = p[0];
	
	dir[0] = -p[2];
	dir[1] = p[5];
	dir[2] = -p[4];
	
	d = dir * dir;
	if( d == 0.0f )
	{
		return false; // pluecker coordinate does not represent a line
	}
	
	start = dir.Cross( dir1 ) * ( 1.0f / d );
	return true;
}

BUD_INLINE void idPluecker::ToDir( budVec3& dir ) const
{
	dir[0] = -p[2];
	dir[1] = p[5];
	dir[2] = -p[4];
}

BUD_INLINE float idPluecker::PermutedInnerProduct( const idPluecker& a ) const
{
	return p[0] * a.p[4] + p[1] * a.p[5] + p[2] * a.p[3] + p[4] * a.p[0] + p[5] * a.p[1] + p[3] * a.p[2];
}

BUD_INLINE float idPluecker::Length() const
{
	return ( float )budMath::Sqrt( p[5] * p[5] + p[4] * p[4] + p[2] * p[2] );
}

BUD_INLINE float idPluecker::LengthSqr() const
{
	return ( p[5] * p[5] + p[4] * p[4] + p[2] * p[2] );
}

BUD_INLINE float idPluecker::NormalizeSelf()
{
	float l, d;
	
	l = LengthSqr();
	if( l == 0.0f )
	{
		return l; // pluecker coordinate does not represent a line
	}
	d = budMath::InvSqrt( l );
	p[0] *= d;
	p[1] *= d;
	p[2] *= d;
	p[3] *= d;
	p[4] *= d;
	p[5] *= d;
	return d * l;
}

BUD_INLINE idPluecker idPluecker::Normalize() const
{
	float d;
	
	d = LengthSqr();
	if( d == 0.0f )
	{
		return *this; // pluecker coordinate does not represent a line
	}
	d = budMath::InvSqrt( d );
	return idPluecker( p[0] * d, p[1] * d, p[2] * d, p[3] * d, p[4] * d, p[5] * d );
}

BUD_INLINE int idPluecker::GetDimension() const
{
	return 6;
}

BUD_INLINE const float* idPluecker::ToFloatPtr() const
{
	return p;
}

BUD_INLINE float* idPluecker::ToFloatPtr()
{
	return p;
}

#endif /* !__MATH_PLUECKER_H__ */
