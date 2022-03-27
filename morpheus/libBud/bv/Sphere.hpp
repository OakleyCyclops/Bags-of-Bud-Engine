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

#ifndef __BV_SPHERE_H__
#define __BV_SPHERE_H__

/*
===============================================================================

	Sphere

===============================================================================
*/

class budSphere
{
public:
	budSphere();
	explicit budSphere( const budVec3& point );
	explicit budSphere( const budVec3& point, const float r );
	
	float			operator[]( const int index ) const;
	float& 			operator[]( const int index );
	budSphere		operator+( const budVec3& t ) const;				// returns tranlated sphere
	budSphere& 		operator+=( const budVec3& t );					// translate the sphere
	budSphere		operator+( const budSphere& s ) const;
	budSphere& 		operator+=( const budSphere& s );
	
	bool			Compare( const budSphere& a ) const;							// exact compare, no epsilon
	bool			Compare( const budSphere& a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const budSphere& a ) const;						// exact compare, no epsilon
	bool			operator!=(	const budSphere& a ) const;						// exact compare, no epsilon
	
	void			Clear();									// inside out sphere
	void			Zero();									// single point at origin
	void			SetOrigin( const budVec3& o );					// set origin of sphere
	void			SetRadius( const float r );						// set square radius
	
	const budVec3& 	GetOrigin() const;						// returns origin of sphere
	float			GetRadius() const;						// returns sphere radius
	bool			IsCleared() const;						// returns true if sphere is inside out
	
	bool			AddPoint( const budVec3& p );					// add the point, returns true if the sphere expanded
	bool			AddSphere( const budSphere& s );					// add the sphere, returns true if the sphere expanded
	budSphere		Expand( const float d ) const;					// return bounds expanded in all directions with the given value
	budSphere& 		ExpandSelf( const float d );					// expand bounds in all directions with the given value
	budSphere		Translate( const budVec3& translation ) const;
	budSphere& 		TranslateSelf( const budVec3& translation );
	
	float			PlaneDistance( const budPlane& plane ) const;
	int				PlaneSide( const budPlane& plane, const float epsilon = ON_EPSILON ) const;
	
	bool			ContainsPoint( const budVec3& p ) const;			// includes touching
	bool			IntersectsSphere( const budSphere& s ) const;	// includes touching
	bool			LineIntersection( const budVec3& start, const budVec3& end ) const;
	// intersection points are (start + dir * scale1) and (start + dir * scale2)
	bool			RayIntersection( const budVec3& start, const budVec3& dir, float& scale1, float& scale2 ) const;
	
	// Tight sphere for a point set.
	void			FromPoints( const budVec3* points, const int numPoints );
	// Most tight sphere for a translation.
	void			FromPointTranslation( const budVec3& point, const budVec3& translation );
	void			FromSphereTranslation( const budSphere& sphere, const budVec3& start, const budVec3& translation );
	// Most tight sphere for a rotation.
	void			FromPointRotation( const budVec3& point, const budRotation& rotation );
	void			FromSphereRotation( const budSphere& sphere, const budVec3& start, const budRotation& rotation );
	
	void			AxisProjection( const budVec3& dir, float& min, float& max ) const;
	
private:
	budVec3			origin;
	float			radius;
};

extern budSphere	sphere_zero;

BUD_INLINE budSphere::budSphere()
{
}

BUD_INLINE budSphere::budSphere( const budVec3& point )
{
	origin = point;
	radius = 0.0f;
}

BUD_INLINE budSphere::budSphere( const budVec3& point, const float r )
{
	origin = point;
	radius = r;
}

BUD_INLINE float budSphere::operator[]( const int index ) const
{
	return ( ( float* ) &origin )[index];
}

BUD_INLINE float& budSphere::operator[]( const int index )
{
	return ( ( float* ) &origin )[index];
}

BUD_INLINE budSphere budSphere::operator+( const budVec3& t ) const
{
	return budSphere( origin + t, radius );
}

BUD_INLINE budSphere& budSphere::operator+=( const budVec3& t )
{
	origin += t;
	return *this;
}

BUD_INLINE bool budSphere::Compare( const budSphere& a ) const
{
	return ( origin.Compare( a.origin ) && radius == a.radius );
}

BUD_INLINE bool budSphere::Compare( const budSphere& a, const float epsilon ) const
{
	return ( origin.Compare( a.origin, epsilon ) && budMath::Fabs( radius - a.radius ) <= epsilon );
}

BUD_INLINE bool budSphere::operator==( const budSphere& a ) const
{
	return Compare( a );
}

BUD_INLINE bool budSphere::operator!=( const budSphere& a ) const
{
	return !Compare( a );
}

BUD_INLINE void budSphere::Clear()
{
	origin.Zero();
	radius = -1.0f;
}

BUD_INLINE void budSphere::Zero()
{
	origin.Zero();
	radius = 0.0f;
}

BUD_INLINE void budSphere::SetOrigin( const budVec3& o )
{
	origin = o;
}

BUD_INLINE void budSphere::SetRadius( const float r )
{
	radius = r;
}

BUD_INLINE const budVec3& budSphere::GetOrigin() const
{
	return origin;
}

BUD_INLINE float budSphere::GetRadius() const
{
	return radius;
}

BUD_INLINE bool budSphere::IsCleared() const
{
	return ( radius < 0.0f );
}

BUD_INLINE bool budSphere::AddPoint( const budVec3& p )
{
	if( radius < 0.0f )
	{
		origin = p;
		radius = 0.0f;
		return true;
	}
	else
	{
		float r = ( p - origin ).LengthSqr();
		if( r > radius * radius )
		{
			r = budMath::Sqrt( r );
			origin += ( p - origin ) * 0.5f * ( 1.0f - radius / r );
			radius += 0.5f * ( r - radius );
			return true;
		}
		return false;
	}
}

BUD_INLINE bool budSphere::AddSphere( const budSphere& s )
{
	if( radius < 0.0f )
	{
		origin = s.origin;
		radius = s.radius;
		return true;
	}
	else
	{
		float r = ( s.origin - origin ).LengthSqr();
		if( r > ( radius + s.radius ) * ( radius + s.radius ) )
		{
			r = budMath::Sqrt( r );
			origin += ( s.origin - origin ) * 0.5f * ( 1.0f - radius / ( r + s.radius ) );
			radius += 0.5f * ( ( r + s.radius ) - radius );
			return true;
		}
		return false;
	}
}

BUD_INLINE budSphere budSphere::Expand( const float d ) const
{
	return budSphere( origin, radius + d );
}

BUD_INLINE budSphere& budSphere::ExpandSelf( const float d )
{
	radius += d;
	return *this;
}

BUD_INLINE budSphere budSphere::Translate( const budVec3& translation ) const
{
	return budSphere( origin + translation, radius );
}

BUD_INLINE budSphere& budSphere::TranslateSelf( const budVec3& translation )
{
	origin += translation;
	return *this;
}

BUD_INLINE bool budSphere::ContainsPoint( const budVec3& p ) const
{
	if( ( p - origin ).LengthSqr() > radius * radius )
	{
		return false;
	}
	return true;
}

BUD_INLINE bool budSphere::IntersectsSphere( const budSphere& s ) const
{
	float r = s.radius + radius;
	if( ( s.origin - origin ).LengthSqr() > r * r )
	{
		return false;
	}
	return true;
}

BUD_INLINE void budSphere::FromPointTranslation( const budVec3& point, const budVec3& translation )
{
	origin = point + 0.5f * translation;
	radius = budMath::Sqrt( 0.5f * translation.LengthSqr() );
}

BUD_INLINE void budSphere::FromSphereTranslation( const budSphere& sphere, const budVec3& start, const budVec3& translation )
{
	origin = start + sphere.origin + 0.5f * translation;
	radius = budMath::Sqrt( 0.5f * translation.LengthSqr() ) + sphere.radius;
}

BUD_INLINE void budSphere::FromPointRotation( const budVec3& point, const budRotation& rotation )
{
	budVec3 end = rotation * point;
	origin = ( point + end ) * 0.5f;
	radius = budMath::Sqrt( 0.5f * ( end - point ).LengthSqr() );
}

BUD_INLINE void budSphere::FromSphereRotation( const budSphere& sphere, const budVec3& start, const budRotation& rotation )
{
	budVec3 end = rotation * sphere.origin;
	origin = start + ( sphere.origin + end ) * 0.5f;
	radius = budMath::Sqrt( 0.5f * ( end - sphere.origin ).LengthSqr() ) + sphere.radius;
}

BUD_INLINE void budSphere::AxisProjection( const budVec3& dir, float& min, float& max ) const
{
	float d;
	d = dir * origin;
	min = d - radius;
	max = d + radius;
}

#endif /* !__BV_SPHERE_H__ */
