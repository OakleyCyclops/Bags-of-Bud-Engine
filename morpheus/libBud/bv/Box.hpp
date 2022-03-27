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

#ifndef __BV_BOX_H__
#define __BV_BOX_H__

/*
===============================================================================

	Oriented Bounding Box

===============================================================================
*/

class budBox
{
public:
	budBox();
	explicit budBox( const budVec3& center, const budVec3& extents, const budMat3& axis );
	explicit budBox( const budVec3& point );
	explicit budBox( const budBounds& bounds );
	explicit budBox( const budBounds& bounds, const budVec3& origin, const budMat3& axis );
	
	budBox			operator+( const budVec3& t ) const;				// returns translated box
	budBox& 			operator+=( const budVec3& t );					// translate the box
	budBox			operator*( const budMat3& r ) const;				// returns rotated box
	budBox& 			operator*=( const budMat3& r );					// rotate the box
	budBox			operator+( const budBox& a ) const;
	budBox& 			operator+=( const budBox& a );
	budBox			operator-( const budBox& a ) const;
	budBox& 			operator-=( const budBox& a );
	
	bool			Compare( const budBox& a ) const;						// exact compare, no epsilon
	bool			Compare( const budBox& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const budBox& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const budBox& a ) const;						// exact compare, no epsilon
	
	void			Clear();									// inside out box
	void			Zero();									// single point at origin
	
	const budVec3& 	GetCenter() const;						// returns center of the box
	const budVec3& 	GetExtents() const;						// returns extents of the box
	const budMat3& 	GetAxis() const;							// returns the axis of the box
	float			GetVolume() const;						// returns the volume of the box
	bool			IsCleared() const;						// returns true if box are inside out
	
	bool			AddPoint( const budVec3& v );					// add the point, returns true if the box expanded
	bool			AddBox( const budBox& a );						// add the box, returns true if the box expanded
	budBox			Expand( const float d ) const;					// return box expanded in all directions with the given value
	budBox& 			ExpandSelf( const float d );					// expand box in all directions with the given value
	budBox			Translate( const budVec3& translation ) const;	// return translated box
	budBox& 			TranslateSelf( const budVec3& translation );		// translate this box
	budBox			Rotate( const budMat3& rotation ) const;			// return rotated box
	budBox& 			RotateSelf( const budMat3& rotation );			// rotate this box
	
	float			PlaneDistance( const budPlane& plane ) const;
	int				PlaneSide( const budPlane& plane, const float epsilon = ON_EPSILON ) const;
	
	bool			ContainsPoint( const budVec3& p ) const;			// includes touching
	bool			IntersectsBox( const budBox& a ) const;			// includes touching
	bool			LineIntersection( const budVec3& start, const budVec3& end ) const;
	// intersection points are (start + dir * scale1) and (start + dir * scale2)
	bool			RayIntersection( const budVec3& start, const budVec3& dir, float& scale1, float& scale2 ) const;
	
	// tight box for a collection of points
	void			FromPoints( const budVec3* points, const int numPoints );
	// most tight box for a translation
	void			FromPointTranslation( const budVec3& point, const budVec3& translation );
	void			FromBoxTranslation( const budBox& box, const budVec3& translation );
	// most tight box for a rotation
	void			FromPointRotation( const budVec3& point, const budRotation& rotation );
	void			FromBoxRotation( const budBox& box, const budRotation& rotation );
	
	void			ToPoints( budVec3 points[8] ) const;
	budSphere		ToSphere() const;
	
	// calculates the projection of this box onto the given axis
	void			AxisProjection( const budVec3& dir, float& min, float& max ) const;
	void			AxisProjection( const budMat3& ax, budBounds& bounds ) const;
	
	// calculates the silhouette of the box
	int				GetProjectionSilhouetteVerts( const budVec3& projectionOrigin, budVec3 silVerts[6] ) const;
	int				GetParallelProjectionSilhouetteVerts( const budVec3& projectionDir, budVec3 silVerts[6] ) const;
	
private:
	budVec3			center;
	budVec3			extents;
	budMat3			axis;
};

extern budBox	box_zero;

BUD_INLINE budBox::budBox()
{
}

BUD_INLINE budBox::budBox( const budVec3& center, const budVec3& extents, const budMat3& axis )
{
	this->center = center;
	this->extents = extents;
	this->axis = axis;
}

BUD_INLINE budBox::budBox( const budVec3& point )
{
	this->center = point;
	this->extents.Zero();
	this->axis.Identity();
}

BUD_INLINE budBox::budBox( const budBounds& bounds )
{
	this->center = ( bounds[0] + bounds[1] ) * 0.5f;
	this->extents = bounds[1] - this->center;
	this->axis.Identity();
}

BUD_INLINE budBox::budBox( const budBounds& bounds, const budVec3& origin, const budMat3& axis )
{
	this->center = ( bounds[0] + bounds[1] ) * 0.5f;
	this->extents = bounds[1] - this->center;
	this->center = origin + this->center * axis;
	this->axis = axis;
}

BUD_INLINE budBox budBox::operator+( const budVec3& t ) const
{
	return budBox( center + t, extents, axis );
}

BUD_INLINE budBox& budBox::operator+=( const budVec3& t )
{
	center += t;
	return *this;
}

BUD_INLINE budBox budBox::operator*( const budMat3& r ) const
{
	return budBox( center * r, extents, axis * r );
}

BUD_INLINE budBox& budBox::operator*=( const budMat3& r )
{
	center *= r;
	axis *= r;
	return *this;
}

BUD_INLINE budBox budBox::operator+( const budBox& a ) const
{
	budBox newBox;
	newBox = *this;
	newBox.AddBox( a );
	return newBox;
}

BUD_INLINE budBox& budBox::operator+=( const budBox& a )
{
	budBox::AddBox( a );
	return *this;
}

BUD_INLINE budBox budBox::operator-( const budBox& a ) const
{
	return budBox( center, extents - a.extents, axis );
}

BUD_INLINE budBox& budBox::operator-=( const budBox& a )
{
	extents -= a.extents;
	return *this;
}

BUD_INLINE bool budBox::Compare( const budBox& a ) const
{
	return ( center.Compare( a.center ) && extents.Compare( a.extents ) && axis.Compare( a.axis ) );
}

BUD_INLINE bool budBox::Compare( const budBox& a, const float epsilon ) const
{
	return ( center.Compare( a.center, epsilon ) && extents.Compare( a.extents, epsilon ) && axis.Compare( a.axis, epsilon ) );
}

BUD_INLINE bool budBox::operator==( const budBox& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budBox::operator!=( const budBox& a ) const
{
	return !Compare( a );
}

BUD_INLINE void budBox::Clear()
{
	center.Zero();
	extents[0] = extents[1] = extents[2] = -budMath::INFINITY;
	axis.Identity();
}

BUD_INLINE void budBox::Zero()
{
	center.Zero();
	extents.Zero();
	axis.Identity();
}

BUD_INLINE const budVec3& budBox::GetCenter() const
{
	return center;
}

BUD_INLINE const budVec3& budBox::GetExtents() const
{
	return extents;
}

BUD_INLINE const budMat3& budBox::GetAxis() const
{
	return axis;
}

BUD_INLINE float budBox::GetVolume() const
{
	return ( extents * 2.0f ).LengthSqr();
}

BUD_INLINE bool budBox::IsCleared() const
{
	return extents[0] < 0.0f;
}

BUD_INLINE budBox budBox::Expand( const float d ) const
{
	return budBox( center, extents + budVec3( d, d, d ), axis );
}

BUD_INLINE budBox& budBox::ExpandSelf( const float d )
{
	extents[0] += d;
	extents[1] += d;
	extents[2] += d;
	return *this;
}

BUD_INLINE budBox budBox::Translate( const budVec3& translation ) const
{
	return budBox( center + translation, extents, axis );
}

BUD_INLINE budBox& budBox::TranslateSelf( const budVec3& translation )
{
	center += translation;
	return *this;
}

BUD_INLINE budBox budBox::Rotate( const budMat3& rotation ) const
{
	return budBox( center * rotation, extents, axis * rotation );
}

BUD_INLINE budBox& budBox::RotateSelf( const budMat3& rotation )
{
	center *= rotation;
	axis *= rotation;
	return *this;
}

BUD_INLINE bool budBox::ContainsPoint( const budVec3& p ) const
{
	budVec3 lp = p - center;
	if( budMath::Fabs( lp * axis[0] ) > extents[0] ||
			budMath::Fabs( lp * axis[1] ) > extents[1] ||
			budMath::Fabs( lp * axis[2] ) > extents[2] )
	{
		return false;
	}
	return true;
}

BUD_INLINE budSphere budBox::ToSphere() const
{
	return budSphere( center, extents.Length() );
}

BUD_INLINE void budBox::AxisProjection( const budVec3& dir, float& min, float& max ) const
{
	float d1 = dir * center;
	float d2 = budMath::Fabs( extents[0] * ( dir * axis[0] ) ) +
			   budMath::Fabs( extents[1] * ( dir * axis[1] ) ) +
			   budMath::Fabs( extents[2] * ( dir * axis[2] ) );
	min = d1 - d2;
	max = d1 + d2;
}

BUD_INLINE void budBox::AxisProjection( const budMat3& ax, budBounds& bounds ) const
{
	for( int i = 0; i < 3; i++ )
	{
		float d1 = ax[i] * center;
		float d2 = budMath::Fabs( extents[0] * ( ax[i] * axis[0] ) ) +
				   budMath::Fabs( extents[1] * ( ax[i] * axis[1] ) ) +
				   budMath::Fabs( extents[2] * ( ax[i] * axis[2] ) );
		bounds[0][i] = d1 - d2;
		bounds[1][i] = d1 + d2;
	}
}

#endif /* !__BV_BOX_H__ */
