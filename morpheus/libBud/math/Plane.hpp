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

#ifndef __MATH_PLANE_H__
#define __MATH_PLANE_H__

/*
===============================================================================

	3D plane with equation: a * x + b * y + c * z + d = 0

===============================================================================
*/


class budVec3;
class budMat3;

#define	ON_EPSILON					0.1f
#define DEGENERATE_DIST_EPSILON		1e-4f

#define	SIDE_FRONT					0
#define	SIDE_BACK					1
#define	SIDE_ON						2
#define	SIDE_CROSS					3

// plane sides
#define PLANESIDE_FRONT				0
#define PLANESIDE_BACK				1
#define PLANESIDE_ON				2
#define PLANESIDE_CROSS				3

// plane types
#define PLANETYPE_X					0
#define PLANETYPE_Y					1
#define PLANETYPE_Z					2
#define PLANETYPE_NEGX				3
#define PLANETYPE_NEGY				4
#define PLANETYPE_NEGZ				5
#define PLANETYPE_TRUEAXIAL			6	// all types < 6 are true axial planes
#define PLANETYPE_ZEROX				6
#define PLANETYPE_ZEROY				7
#define PLANETYPE_ZEROZ				8
#define PLANETYPE_NONAXIAL			9

class budPlane
{
public:
	budPlane();
	explicit budPlane( float a, float b, float c, float d );
	explicit budPlane( const budVec3& normal, const float dist );
	explicit budPlane( const budVec3& v0, const budVec3& v1, const budVec3& v2, bool fixDegenerate = false );
	
	float			operator[]( int index ) const;
	float& 			operator[]( int index );
	budPlane			operator-() const;						// flips plane
	budPlane& 		operator=( const budVec3& v );			// sets normal and sets budPlane::d to zero
	budPlane			operator+( const budPlane& p ) const;	// add plane equations
	budPlane			operator-( const budPlane& p ) const;	// subtract plane equations
	budPlane			operator*( const float s ) const;		// scale plane
	budPlane& 		operator*=( const budMat3& m );			// Normal() *= m
	
	bool			Compare( const budPlane& p ) const;						// exact compare, no epsilon
	bool			Compare( const budPlane& p, const float epsilon ) const;	// compare with epsilon
	bool			Compare( const budPlane& p, const float normalEps, const float distEps ) const;	// compare with epsilon
	bool			operator==(	const budPlane& p ) const;					// exact compare, no epsilon
	bool			operator!=(	const budPlane& p ) const;					// exact compare, no epsilon
	
	void			Zero();							// zero plane
	void			SetNormal( const budVec3& normal );		// sets the normal
	const budVec3& 	Normal() const;					// reference to const normal
	budVec3& 		Normal();							// reference to normal
	float			Normalize( bool fixDegenerate = true );	// only normalizes the plane normal, does not adjust d
	bool			FixDegenerateNormal();			// fix degenerate normal
	bool			FixDegeneracies( float distEpsilon );	// fix degenerate normal and dist
	float			Dist() const;						// returns: -d
	void			SetDist( const float dist );			// sets: d = -dist
	int				Type() const;						// returns plane type
	
	bool			FromPoints( const budVec3& p1, const budVec3& p2, const budVec3& p3, bool fixDegenerate = true );
	bool			FromVecs( const budVec3& dir1, const budVec3& dir2, const budVec3& p, bool fixDegenerate = true );
	void			FitThroughPoint( const budVec3& p );	// assumes normal is valid
	bool			HeightFit( const budVec3* points, const int numPoints );
	budPlane			Translate( const budVec3& translation ) const;
	budPlane& 		TranslateSelf( const budVec3& translation );
	budPlane			Rotate( const budVec3& origin, const budMat3& axis ) const;
	budPlane& 		RotateSelf( const budVec3& origin, const budMat3& axis );
	
	float			Distance( const budVec3& v ) const;
	int				Side( const budVec3& v, const float epsilon = 0.0f ) const;
	
	bool			LineIntersection( const budVec3& start, const budVec3& end ) const;
	// intersection point is start + dir * scale
	bool			RayIntersection( const budVec3& start, const budVec3& dir, float& scale ) const;
	bool			PlaneIntersection( const budPlane& plane, budVec3& start, budVec3& dir ) const;
	
	int				GetDimension() const;
	
	const budVec4& 	ToVec4() const;
	budVec4& 		ToVec4();
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	float			a;
	float			b;
	float			c;
	float			d;
};

extern budPlane plane_origin;
#define plane_zero plane_origin

BUD_INLINE budPlane::budPlane()
{
}

BUD_INLINE budPlane::budPlane( float a, float b, float c, float d )
{
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

BUD_INLINE budPlane::budPlane( const budVec3& normal, const float dist )
{
	this->a = normal.x;
	this->b = normal.y;
	this->c = normal.z;
	this->d = -dist;
}

BUD_INLINE budPlane::budPlane( const budVec3& v0, const budVec3& v1, const budVec3& v2, bool fixDegenerate )
{
	FromPoints( v0, v1, v2, fixDegenerate );
}

BUD_INLINE float budPlane::operator[]( int index ) const
{
	return ( &a )[ index ];
}

BUD_INLINE float& budPlane::operator[]( int index )
{
	return ( &a )[ index ];
}

BUD_INLINE budPlane budPlane::operator-() const
{
	return budPlane( -a, -b, -c, -d );
}

BUD_INLINE budPlane& budPlane::operator=( const budVec3& v )
{
	a = v.x;
	b = v.y;
	c = v.z;
	d = 0;
	return *this;
}

BUD_INLINE budPlane budPlane::operator+( const budPlane& p ) const
{
	return budPlane( a + p.a, b + p.b, c + p.c, d + p.d );
}

BUD_INLINE budPlane budPlane::operator-( const budPlane& p ) const
{
	return budPlane( a - p.a, b - p.b, c - p.c, d - p.d );
}

BUD_INLINE budPlane budPlane::operator*( const float s ) const
{
	return budPlane( a * s, b * s, c * s, d * s );
}

BUD_INLINE budPlane& budPlane::operator*=( const budMat3& m )
{
	Normal() *= m;
	return *this;
}

BUD_INLINE bool budPlane::Compare( const budPlane& p ) const
{
	return ( a == p.a && b == p.b && c == p.c && d == p.d );
}

BUD_INLINE bool budPlane::Compare( const budPlane& p, const float epsilon ) const
{
	if( budMath::Fabs( a - p.a ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( b - p.b ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( c - p.c ) > epsilon )
	{
		return false;
	}
	
	if( budMath::Fabs( d - p.d ) > epsilon )
	{
		return false;
	}
	
	return true;
}

BUD_INLINE bool budPlane::Compare( const budPlane& p, const float normalEps, const float distEps ) const
{
	if( budMath::Fabs( d - p.d ) > distEps )
	{
		return false;
	}
	if( !Normal().Compare( p.Normal(), normalEps ) )
	{
		return false;
	}
	return true;
}

BUD_INLINE bool budPlane::operator==( const budPlane& p ) const
{
	return Compare( p );
}

BUD_INLINE bool budPlane::operator!=( const budPlane& p ) const
{
	return !Compare( p );
}

BUD_INLINE void budPlane::Zero()
{
	a = b = c = d = 0.0f;
}

BUD_INLINE void budPlane::SetNormal( const budVec3& normal )
{
	a = normal.x;
	b = normal.y;
	c = normal.z;
}

BUD_INLINE const budVec3& budPlane::Normal() const
{
	return *reinterpret_cast<const budVec3*>( &a );
}

BUD_INLINE budVec3& budPlane::Normal()
{
	return *reinterpret_cast<budVec3*>( &a );
}

BUD_INLINE float budPlane::Normalize( bool fixDegenerate )
{
	float length = reinterpret_cast<budVec3*>( &a )->Normalize();
	
	if( fixDegenerate )
	{
		FixDegenerateNormal();
	}
	return length;
}

BUD_INLINE bool budPlane::FixDegenerateNormal()
{
	return Normal().FixDegenerateNormal();
}

BUD_INLINE bool budPlane::FixDegeneracies( float distEpsilon )
{
	bool fixedNormal = FixDegenerateNormal();
	// only fix dist if the normal was degenerate
	if( fixedNormal )
	{
		if( budMath::Fabs( d - budMath::Rint( d ) ) < distEpsilon )
		{
			d = budMath::Rint( d );
		}
	}
	return fixedNormal;
}

BUD_INLINE float budPlane::Dist() const
{
	return -d;
}

BUD_INLINE void budPlane::SetDist( const float dist )
{
	d = -dist;
}

BUD_INLINE bool budPlane::FromPoints( const budVec3& p1, const budVec3& p2, const budVec3& p3, bool fixDegenerate )
{
	Normal() = ( p1 - p2 ).Cross( p3 - p2 );
	if( Normalize( fixDegenerate ) == 0.0f )
	{
		return false;
	}
	d = -( Normal() * p2 );
	return true;
}

BUD_INLINE bool budPlane::FromVecs( const budVec3& dir1, const budVec3& dir2, const budVec3& p, bool fixDegenerate )
{
	Normal() = dir1.Cross( dir2 );
	if( Normalize( fixDegenerate ) == 0.0f )
	{
		return false;
	}
	d = -( Normal() * p );
	return true;
}

BUD_INLINE void budPlane::FitThroughPoint( const budVec3& p )
{
	d = -( Normal() * p );
}

BUD_INLINE budPlane budPlane::Translate( const budVec3& translation ) const
{
	return budPlane( a, b, c, d - translation * Normal() );
}

BUD_INLINE budPlane& budPlane::TranslateSelf( const budVec3& translation )
{
	d -= translation * Normal();
	return *this;
}

BUD_INLINE budPlane budPlane::Rotate( const budVec3& origin, const budMat3& axis ) const
{
	budPlane p;
	p.Normal() = Normal() * axis;
	p.d = d + origin * Normal() - origin * p.Normal();
	return p;
}

BUD_INLINE budPlane& budPlane::RotateSelf( const budVec3& origin, const budMat3& axis )
{
	d += origin * Normal();
	Normal() *= axis;
	d -= origin * Normal();
	return *this;
}

BUD_INLINE float budPlane::Distance( const budVec3& v ) const
{
	return a * v.x + b * v.y + c * v.z + d;
}

BUD_INLINE int budPlane::Side( const budVec3& v, const float epsilon ) const
{
	float dist = Distance( v );
	if( dist > epsilon )
	{
		return PLANESIDE_FRONT;
	}
	else if( dist < -epsilon )
	{
		return PLANESIDE_BACK;
	}
	else
	{
		return PLANESIDE_ON;
	}
}

BUD_INLINE bool budPlane::LineIntersection( const budVec3& start, const budVec3& end ) const
{
	float d1, d2, fraction;
	
	d1 = Normal() * start + d;
	d2 = Normal() * end + d;
	if( d1 == d2 )
	{
		return false;
	}
	if( d1 > 0.0f && d2 > 0.0f )
	{
		return false;
	}
	if( d1 < 0.0f && d2 < 0.0f )
	{
		return false;
	}
	fraction = ( d1 / ( d1 - d2 ) );
	return ( fraction >= 0.0f && fraction <= 1.0f );
}

BUD_INLINE bool budPlane::RayIntersection( const budVec3& start, const budVec3& dir, float& scale ) const
{
	float d1, d2;
	
	d1 = Normal() * start + d;
	d2 = Normal() * dir;
	if( d2 == 0.0f )
	{
		return false;
	}
	scale = -( d1 / d2 );
	return true;
}

BUD_INLINE int budPlane::GetDimension() const
{
	return 4;
}

BUD_INLINE const budVec4& budPlane::ToVec4() const
{
	return *reinterpret_cast<const budVec4*>( &a );
}

BUD_INLINE budVec4& budPlane::ToVec4()
{
	return *reinterpret_cast<budVec4*>( &a );
}

BUD_INLINE const float* budPlane::ToFloatPtr() const
{
	return reinterpret_cast<const float*>( &a );
}

BUD_INLINE float* budPlane::ToFloatPtr()
{
	return reinterpret_cast<float*>( &a );
}

#endif /* !__MATH_PLANE_H__ */
