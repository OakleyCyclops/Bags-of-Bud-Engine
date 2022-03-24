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

#ifndef __BV_BOUNDS_H__
#define __BV_BOUNDS_H__

/*
===============================================================================

	Axis Aligned Bounding Box

===============================================================================
*/

class budBounds
{
public:
	budBounds();
	explicit budBounds( const budVec3& mins, const budVec3& maxs );
	explicit budBounds( const budVec3& point );
	
	const budVec3& 	operator[]( const int index ) const;
	budVec3& 		operator[]( const int index );
	budBounds		operator+( const budVec3& t ) const;				// returns translated bounds
	budBounds& 		operator+=( const budVec3& t );					// translate the bounds
	budBounds		operator*( const budMat3& r ) const;				// returns rotated bounds
	budBounds& 		operator*=( const budMat3& r );					// rotate the bounds
	budBounds		operator+( const budBounds& a ) const;
	budBounds& 		operator+=( const budBounds& a );
	budBounds		operator-( const budBounds& a ) const;
	budBounds& 		operator-=( const budBounds& a );
	
	bool			Compare( const budBounds& a ) const;							// exact compare, no epsilon
	bool			Compare( const budBounds& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const budBounds& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const budBounds& a ) const;						// exact compare, no epsilon
	
	void			Clear();									// inside out bounds
	void			Zero();									// single point at origin
	
	budVec3			GetCenter() const;						// returns center of bounds
	float			GetRadius() const;						// returns the radius relative to the bounds origin
	float			GetRadius( const budVec3& center ) const;		// returns the radius relative to the given center
	float			GetVolume() const;						// returns the volume of the bounds
	bool			IsCleared() const;						// returns true if bounds are inside out
	
	bool			AddPoint( const budVec3& v );					// add the point, returns true if the bounds expanded
	bool			AddBounds( const budBounds& a );					// add the bounds, returns true if the bounds expanded
	budBounds		Intersect( const budBounds& a ) const;			// return intersection of this bounds with the given bounds
	budBounds& 		IntersectSelf( const budBounds& a );				// intersect this bounds with the given bounds
	budBounds		Expand( const float d ) const;					// return bounds expanded in all directions with the given value
	budBounds& 		ExpandSelf( const float d );					// expand bounds in all directions with the given value
	budBounds		Translate( const budVec3& translation ) const;	// return translated bounds
	budBounds& 		TranslateSelf( const budVec3& translation );		// translate this bounds
	budBounds		Rotate( const budMat3& rotation ) const;			// return rotated bounds
	budBounds& 		RotateSelf( const budMat3& rotation );			// rotate this bounds
	
	float			PlaneDistance( const budPlane& plane ) const;
	int				PlaneSide( const budPlane& plane, const float epsilon = ON_EPSILON ) const;
	
	bool			ContainsPoint( const budVec3& p ) const;			// includes touching
	bool			IntersectsBounds( const budBounds& a ) const;	// includes touching
	bool			LineIntersection( const budVec3& start, const budVec3& end ) const;
	// intersection point is start + dir * scale
	bool			RayIntersection( const budVec3& start, const budVec3& dir, float& scale ) const;
	
	// most tight bounds for the given transformed bounds
	void			FromTransformedBounds( const budBounds& bounds, const budVec3& origin, const budMat3& axis );
	// most tight bounds for a point set
	void			FromPoints( const budVec3* points, const int numPoints );
	// most tight bounds for a translation
	void			FromPointTranslation( const budVec3& point, const budVec3& translation );
	void			FromBoundsTranslation( const budBounds& bounds, const budVec3& origin, const budMat3& axis, const budVec3& translation );
	// most tight bounds for a rotation
	void			FromPointRotation( const budVec3& point, const budRotation& rotation );
	void			FromBoundsRotation( const budBounds& bounds, const budVec3& origin, const budMat3& axis, const budRotation& rotation );
	
	void			ToPoints( budVec3 points[8] ) const;
	budSphere		ToSphere() const;
	
	void			AxisProjection( const budVec3& dir, float& min, float& max ) const;
	void			AxisProjection( const budVec3& origin, const budMat3& axis, const budVec3& dir, float& min, float& max ) const;
	
	int				GetDimension() const;
	
	const float* 	ToFloatPtr() const;
	float* 			ToFloatPtr();
	
private:
	budVec3			b[2];
};

extern budBounds	bounds_zero;
extern budBounds bounds_zeroOneCube;
extern budBounds bounds_unitCube;

BUD_INLINE budBounds::budBounds()
{
}

BUD_INLINE budBounds::budBounds( const budVec3& mins, const budVec3& maxs )
{
	b[0] = mins;
	b[1] = maxs;
}

BUD_INLINE budBounds::budBounds( const budVec3& point )
{
	b[0] = point;
	b[1] = point;
}

BUD_INLINE const budVec3& budBounds::operator[]( const int index ) const
{
	return b[index];
}

BUD_INLINE budVec3& budBounds::operator[]( const int index )
{
	return b[index];
}

BUD_INLINE budBounds budBounds::operator+( const budVec3& t ) const
{
	return budBounds( b[0] + t, b[1] + t );
}

BUD_INLINE budBounds& budBounds::operator+=( const budVec3& t )
{
	b[0] += t;
	b[1] += t;
	return *this;
}

BUD_INLINE budBounds budBounds::operator*( const budMat3& r ) const
{
	budBounds bounds;
	bounds.FromTransformedBounds( *this, vec3_origin, r );
	return bounds;
}

BUD_INLINE budBounds& budBounds::operator*=( const budMat3& r )
{
	this->FromTransformedBounds( *this, vec3_origin, r );
	return *this;
}

BUD_INLINE budBounds budBounds::operator+( const budBounds& a ) const
{
	budBounds newBounds;
	newBounds = *this;
	newBounds.AddBounds( a );
	return newBounds;
}

BUD_INLINE budBounds& budBounds::operator+=( const budBounds& a )
{
	budBounds::AddBounds( a );
	return *this;
}

BUD_INLINE budBounds budBounds::operator-( const budBounds& a ) const
{
	assert( b[1][0] - b[0][0] > a.b[1][0] - a.b[0][0] &&
			b[1][1] - b[0][1] > a.b[1][1] - a.b[0][1] &&
			b[1][2] - b[0][2] > a.b[1][2] - a.b[0][2] );
	return budBounds( budVec3( b[0][0] + a.b[1][0], b[0][1] + a.b[1][1], b[0][2] + a.b[1][2] ),
					 budVec3( b[1][0] + a.b[0][0], b[1][1] + a.b[0][1], b[1][2] + a.b[0][2] ) );
}

BUD_INLINE budBounds& budBounds::operator-=( const budBounds& a )
{
	assert( b[1][0] - b[0][0] > a.b[1][0] - a.b[0][0] &&
			b[1][1] - b[0][1] > a.b[1][1] - a.b[0][1] &&
			b[1][2] - b[0][2] > a.b[1][2] - a.b[0][2] );
	b[0] += a.b[1];
	b[1] += a.b[0];
	return *this;
}

BUD_INLINE bool budBounds::Compare( const budBounds& a ) const
{
	return ( b[0].Compare( a.b[0] ) && b[1].Compare( a.b[1] ) );
}

BUD_INLINE bool budBounds::Compare( const budBounds& a, const float epsilon ) const
{
	return ( b[0].Compare( a.b[0], epsilon ) && b[1].Compare( a.b[1], epsilon ) );
}

BUD_INLINE bool budBounds::operator==( const budBounds& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budBounds::operator!=( const budBounds& a ) const
{
	return !Compare( a );
}

BUD_INLINE void budBounds::Clear()
{
	b[0][0] = b[0][1] = b[0][2] = budMath::INFINITY;
	b[1][0] = b[1][1] = b[1][2] = -budMath::INFINITY;
}

BUD_INLINE void budBounds::Zero()
{
	b[0][0] = b[0][1] = b[0][2] =
							b[1][0] = b[1][1] = b[1][2] = 0;
}

BUD_INLINE budVec3 budBounds::GetCenter() const
{
	return budVec3( ( b[1][0] + b[0][0] ) * 0.5f, ( b[1][1] + b[0][1] ) * 0.5f, ( b[1][2] + b[0][2] ) * 0.5f );
}

BUD_INLINE float budBounds::GetVolume() const
{
	if( b[0][0] >= b[1][0] || b[0][1] >= b[1][1] || b[0][2] >= b[1][2] )
	{
		return 0.0f;
	}
	return ( ( b[1][0] - b[0][0] ) * ( b[1][1] - b[0][1] ) * ( b[1][2] - b[0][2] ) );
}

BUD_INLINE bool budBounds::IsCleared() const
{
	return b[0][0] > b[1][0];
}

BUD_INLINE bool budBounds::AddPoint( const budVec3& v )
{
	bool expanded = false;
	if( v[0] < b[0][0] )
	{
		b[0][0] = v[0];
		expanded = true;
	}
	if( v[0] > b[1][0] )
	{
		b[1][0] = v[0];
		expanded = true;
	}
	if( v[1] < b[0][1] )
	{
		b[0][1] = v[1];
		expanded = true;
	}
	if( v[1] > b[1][1] )
	{
		b[1][1] = v[1];
		expanded = true;
	}
	if( v[2] < b[0][2] )
	{
		b[0][2] = v[2];
		expanded = true;
	}
	if( v[2] > b[1][2] )
	{
		b[1][2] = v[2];
		expanded = true;
	}
	return expanded;
}

BUD_INLINE bool budBounds::AddBounds( const budBounds& a )
{
	bool expanded = false;
	if( a.b[0][0] < b[0][0] )
	{
		b[0][0] = a.b[0][0];
		expanded = true;
	}
	if( a.b[0][1] < b[0][1] )
	{
		b[0][1] = a.b[0][1];
		expanded = true;
	}
	if( a.b[0][2] < b[0][2] )
	{
		b[0][2] = a.b[0][2];
		expanded = true;
	}
	if( a.b[1][0] > b[1][0] )
	{
		b[1][0] = a.b[1][0];
		expanded = true;
	}
	if( a.b[1][1] > b[1][1] )
	{
		b[1][1] = a.b[1][1];
		expanded = true;
	}
	if( a.b[1][2] > b[1][2] )
	{
		b[1][2] = a.b[1][2];
		expanded = true;
	}
	return expanded;
}

BUD_INLINE budBounds budBounds::Intersect( const budBounds& a ) const
{
	budBounds n;
	n.b[0][0] = ( a.b[0][0] > b[0][0] ) ? a.b[0][0] : b[0][0];
	n.b[0][1] = ( a.b[0][1] > b[0][1] ) ? a.b[0][1] : b[0][1];
	n.b[0][2] = ( a.b[0][2] > b[0][2] ) ? a.b[0][2] : b[0][2];
	n.b[1][0] = ( a.b[1][0] < b[1][0] ) ? a.b[1][0] : b[1][0];
	n.b[1][1] = ( a.b[1][1] < b[1][1] ) ? a.b[1][1] : b[1][1];
	n.b[1][2] = ( a.b[1][2] < b[1][2] ) ? a.b[1][2] : b[1][2];
	return n;
}

BUD_INLINE budBounds& budBounds::IntersectSelf( const budBounds& a )
{
	if( a.b[0][0] > b[0][0] )
	{
		b[0][0] = a.b[0][0];
	}
	if( a.b[0][1] > b[0][1] )
	{
		b[0][1] = a.b[0][1];
	}
	if( a.b[0][2] > b[0][2] )
	{
		b[0][2] = a.b[0][2];
	}
	if( a.b[1][0] < b[1][0] )
	{
		b[1][0] = a.b[1][0];
	}
	if( a.b[1][1] < b[1][1] )
	{
		b[1][1] = a.b[1][1];
	}
	if( a.b[1][2] < b[1][2] )
	{
		b[1][2] = a.b[1][2];
	}
	return *this;
}

BUD_INLINE budBounds budBounds::Expand( const float d ) const
{
	return budBounds( budVec3( b[0][0] - d, b[0][1] - d, b[0][2] - d ),
					 budVec3( b[1][0] + d, b[1][1] + d, b[1][2] + d ) );
}

BUD_INLINE budBounds& budBounds::ExpandSelf( const float d )
{
	b[0][0] -= d;
	b[0][1] -= d;
	b[0][2] -= d;
	b[1][0] += d;
	b[1][1] += d;
	b[1][2] += d;
	return *this;
}

BUD_INLINE budBounds budBounds::Translate( const budVec3& translation ) const
{
	return budBounds( b[0] + translation, b[1] + translation );
}

BUD_INLINE budBounds& budBounds::TranslateSelf( const budVec3& translation )
{
	b[0] += translation;
	b[1] += translation;
	return *this;
}

BUD_INLINE budBounds budBounds::Rotate( const budMat3& rotation ) const
{
	budBounds bounds;
	bounds.FromTransformedBounds( *this, vec3_origin, rotation );
	return bounds;
}

BUD_INLINE budBounds& budBounds::RotateSelf( const budMat3& rotation )
{
	FromTransformedBounds( *this, vec3_origin, rotation );
	return *this;
}

BUD_INLINE bool budBounds::ContainsPoint( const budVec3& p ) const
{
	if( p[0] < b[0][0] || p[1] < b[0][1] || p[2] < b[0][2]
			|| p[0] > b[1][0] || p[1] > b[1][1] || p[2] > b[1][2] )
	{
		return false;
	}
	return true;
}

BUD_INLINE bool budBounds::IntersectsBounds( const budBounds& a ) const
{
	if( a.b[1][0] < b[0][0] || a.b[1][1] < b[0][1] || a.b[1][2] < b[0][2]
			|| a.b[0][0] > b[1][0] || a.b[0][1] > b[1][1] || a.b[0][2] > b[1][2] )
	{
		return false;
	}
	return true;
}

BUD_INLINE budSphere budBounds::ToSphere() const
{
	budSphere sphere;
	sphere.SetOrigin( ( b[0] + b[1] ) * 0.5f );
	sphere.SetRadius( ( b[1] - sphere.GetOrigin() ).Length() );
	return sphere;
}

BUD_INLINE void budBounds::AxisProjection( const budVec3& dir, float& min, float& max ) const
{
	float d1, d2;
	budVec3 center, extents;
	
	center = ( b[0] + b[1] ) * 0.5f;
	extents = b[1] - center;
	
	d1 = dir * center;
	d2 = budMath::Fabs( extents[0] * dir[0] ) +
		 budMath::Fabs( extents[1] * dir[1] ) +
		 budMath::Fabs( extents[2] * dir[2] );
		 
	min = d1 - d2;
	max = d1 + d2;
}

BUD_INLINE void budBounds::AxisProjection( const budVec3& origin, const budMat3& axis, const budVec3& dir, float& min, float& max ) const
{
	float d1, d2;
	budVec3 center, extents;
	
	center = ( b[0] + b[1] ) * 0.5f;
	extents = b[1] - center;
	center = origin + center * axis;
	
	d1 = dir * center;
	d2 = budMath::Fabs( extents[0] * ( dir * axis[0] ) ) +
		 budMath::Fabs( extents[1] * ( dir * axis[1] ) ) +
		 budMath::Fabs( extents[2] * ( dir * axis[2] ) );
		 
	min = d1 - d2;
	max = d1 + d2;
}

BUD_INLINE int budBounds::GetDimension() const
{
	return 6;
}

BUD_INLINE const float* budBounds::ToFloatPtr() const
{
	return &b[0].x;
}

BUD_INLINE float* budBounds::ToFloatPtr()
{
	return &b[0].x;
}

#endif /* !__BV_BOUNDS_H__ */
