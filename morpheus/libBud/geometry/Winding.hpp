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

#ifndef __WINDING_H__
#define __WINDING_H__

/*
===============================================================================

	A winding is an arbitrary convex polygon defined by an array of points.

===============================================================================
*/

class idWinding
{

public:
	idWinding();
	explicit idWinding( const int n );								// allocate for n points
	explicit idWinding( const budVec3* verts, const int n );			// winding from points
	explicit idWinding( const budVec3& normal, const float dist );	// base winding for plane
	explicit idWinding( const budPlane& plane );						// base winding for plane
	explicit idWinding( const idWinding& winding );
	virtual			~idWinding();
	
	idWinding& 		operator=( const idWinding& winding );
	const budVec5& 	operator[]( const int index ) const;
	budVec5& 		operator[]( const int index );
	
	// add a point to the end of the winding point array
	idWinding& 		operator+=( const budVec3& v );
	idWinding& 		operator+=( const budVec5& v );
	void			AddPoint( const budVec3& v );
	void			AddPoint( const budVec5& v );
	
	// number of points on winding
	int				GetNumPoints() const;
	void			SetNumPoints( int n );
	virtual void	Clear();
	
	// huge winding for plane, the points go counter clockwise when facing the front of the plane
	void			BaseForPlane( const budVec3& normal, const float dist );
	void			BaseForPlane( const budPlane& plane );
	
	// splits the winding into a front and back winding, the winding itself stays unchanged
	// returns a SIDE_?
	int				Split( const budPlane& plane, const float epsilon, idWinding** front, idWinding** back ) const;
	// returns the winding fragment at the front of the clipping plane,
	// if there is nothing at the front the winding itself is destroyed and NULL is returned
	idWinding* 		Clip( const budPlane& plane, const float epsilon = ON_EPSILON, const bool keepOn = false );
	// cuts off the part at the back side of the plane, returns true if some part was at the front
	// if there is nothing at the front the number of points is set to zero
	bool			ClipInPlace( const budPlane& plane, const float epsilon = ON_EPSILON, const bool keepOn = false );
	
	// returns a copy of the winding
	idWinding* 		Copy() const;
	idWinding* 		Reverse() const;
	void			ReverseSelf();
	void			RemoveEqualPoints( const float epsilon = ON_EPSILON );
	void			RemoveColinearPoints( const budVec3& normal, const float epsilon = ON_EPSILON );
	void			RemovePoint( int point );
	void			InsertPoint( const budVec5& point, int spot );
	bool			InsertPointIfOnEdge( const budVec5& point, const budPlane& plane, const float epsilon = ON_EPSILON );
	bool			InsertPointIfOnEdge( const budVec3& point, const budPlane& plane, const float epsilon = ON_EPSILON );
	// add a winding to the convex hull
	void			AddToConvexHull( const idWinding* winding, const budVec3& normal, const float epsilon = ON_EPSILON );
	// add a point to the convex hull
	void			AddToConvexHull( const budVec3& point, const budVec3& normal, const float epsilon = ON_EPSILON );
	// tries to merge 'this' with the given winding, returns NULL if merge fails, both 'this' and 'w' stay intact
	// 'keep' tells if the contacting points should stay even if they create colinear edges
	idWinding* 		TryMerge( const idWinding& w, const budVec3& normal, int keep = false ) const;
	// check whether the winding is valid or not
	bool			Check( bool print = true ) const;
	
	float			GetArea() const;
	budVec3			GetCenter() const;
	float			GetRadius( const budVec3& center ) const;
	void			GetPlane( budVec3& normal, float& dist ) const;
	void			GetPlane( budPlane& plane ) const;
	void			GetBounds( budBounds& bounds ) const;
	
	bool			IsTiny() const;
	bool			IsHuge() const;	// base winding for a plane is typically huge
	void			Print() const;
	
	float			PlaneDistance( const budPlane& plane ) const;
	int				PlaneSide( const budPlane& plane, const float epsilon = ON_EPSILON ) const;
	
	bool			PlanesConcave( const idWinding& w2, const budVec3& normal1, const budVec3& normal2, float dist1, float dist2 ) const;
	
	bool			PointInside( const budVec3& normal, const budVec3& point, const float epsilon ) const;
	// returns true if the line or ray intersects the winding
	bool			LineIntersection( const budPlane& windingPlane, const budVec3& start, const budVec3& end, bool backFaceCull = false ) const;
	// intersection point is start + dir * scale
	bool			RayIntersection( const budPlane& windingPlane, const budVec3& start, const budVec3& dir, float& scale, bool backFaceCull = false ) const;
	
	static float	TriangleArea( const budVec3& a, const budVec3& b, const budVec3& c );
	
protected:
	int				numPoints;				// number of points
	budVec5* 		p;						// pointer to point data
	int				allocedSize;
	
	bool			EnsureAlloced( int n, bool keep = false );
	virtual bool	ReAllocate( int n, bool keep = false );
};

BUD_INLINE idWinding::idWinding()
{
	numPoints = allocedSize = 0;
	p = NULL;
}

BUD_INLINE idWinding::idWinding( int n )
{
	numPoints = allocedSize = 0;
	p = NULL;
	EnsureAlloced( n );
}

BUD_INLINE idWinding::idWinding( const budVec3* verts, const int n )
{
	int i;
	
	numPoints = allocedSize = 0;
	p = NULL;
	if( !EnsureAlloced( n ) )
	{
		numPoints = 0;
		return;
	}
	for( i = 0; i < n; i++ )
	{
		p[i].ToVec3() = verts[i];
		p[i].s = p[i].t = 0.0f;
	}
	numPoints = n;
}

BUD_INLINE idWinding::idWinding( const budVec3& normal, const float dist )
{
	numPoints = allocedSize = 0;
	p = NULL;
	BaseForPlane( normal, dist );
}

BUD_INLINE idWinding::idWinding( const budPlane& plane )
{
	numPoints = allocedSize = 0;
	p = NULL;
	BaseForPlane( plane );
}

BUD_INLINE idWinding::idWinding( const idWinding& winding )
{
	int i;
	if( !EnsureAlloced( winding.GetNumPoints() ) )
	{
		numPoints = 0;
		return;
	}
	for( i = 0; i < winding.GetNumPoints(); i++ )
	{
		p[i] = winding[i];
	}
	numPoints = winding.GetNumPoints();
}

BUD_INLINE idWinding::~idWinding()
{
	delete[] p;
	p = NULL;
}

BUD_INLINE idWinding& idWinding::operator=( const idWinding& winding )
{
	int i;
	
	if( !EnsureAlloced( winding.numPoints ) )
	{
		numPoints = 0;
		return *this;
	}
	for( i = 0; i < winding.numPoints; i++ )
	{
		p[i] = winding.p[i];
	}
	numPoints = winding.numPoints;
	return *this;
}

BUD_INLINE const budVec5& idWinding::operator[]( const int index ) const
{
	//assert( index >= 0 && index < numPoints );
	return p[ index ];
}

BUD_INLINE budVec5& idWinding::operator[]( const int index )
{
	//assert( index >= 0 && index < numPoints );
	return p[ index ];
}

BUD_INLINE idWinding& idWinding::operator+=( const budVec3& v )
{
	AddPoint( v );
	return *this;
}

BUD_INLINE idWinding& idWinding::operator+=( const budVec5& v )
{
	AddPoint( v );
	return *this;
}

BUD_INLINE void idWinding::AddPoint( const budVec3& v )
{
	if( !EnsureAlloced( numPoints + 1, true ) )
	{
		return;
	}
	p[numPoints] = v;
	numPoints++;
}

BUD_INLINE void idWinding::AddPoint( const budVec5& v )
{
	if( !EnsureAlloced( numPoints + 1, true ) )
	{
		return;
	}
	p[numPoints] = v;
	numPoints++;
}

BUD_INLINE int idWinding::GetNumPoints() const
{
	return numPoints;
}

BUD_INLINE void idWinding::SetNumPoints( int n )
{
	if( !EnsureAlloced( n, true ) )
	{
		return;
	}
	numPoints = n;
}

BUD_INLINE void idWinding::Clear()
{
	numPoints = 0;
	delete[] p;
	p = NULL;
}

BUD_INLINE void idWinding::BaseForPlane( const budPlane& plane )
{
	BaseForPlane( plane.Normal(), plane.Dist() );
}

BUD_INLINE bool idWinding::EnsureAlloced( int n, bool keep )
{
	if( n > allocedSize )
	{
		return ReAllocate( n, keep );
	}
	return true;
}


/*
===============================================================================

	budFixedWinding is a fixed buffer size winding not using
	memory allocations.

	When an operation would overflow the fixed buffer a warning
	is printed and the operation is safely cancelled.

===============================================================================
*/

#define	MAX_POINTS_ON_WINDING	64

class budFixedWinding : public idWinding
{

public:
	budFixedWinding();
	explicit budFixedWinding( const int n );
	explicit budFixedWinding( const budVec3* verts, const int n );
	explicit budFixedWinding( const budVec3& normal, const float dist );
	explicit budFixedWinding( const budPlane& plane );
	explicit budFixedWinding( const idWinding& winding );
	explicit budFixedWinding( const budFixedWinding& winding );
	virtual			~budFixedWinding();
	
	budFixedWinding& operator=( const idWinding& winding );
	
	virtual void	Clear();
	
	// splits the winding in a back and front part, 'this' becomes the front part
	// returns a SIDE_?
	int				Split( budFixedWinding* back, const budPlane& plane, const float epsilon = ON_EPSILON );
	
protected:
	budVec5			data[MAX_POINTS_ON_WINDING];	// point data
	
	virtual bool	ReAllocate( int n, bool keep = false );
};

BUD_INLINE budFixedWinding::budFixedWinding()
{
	numPoints = 0;
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
}

BUD_INLINE budFixedWinding::budFixedWinding( int n )
{
	numPoints = 0;
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
}

BUD_INLINE budFixedWinding::budFixedWinding( const budVec3* verts, const int n )
{
	int i;
	
	numPoints = 0;
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
	if( !EnsureAlloced( n ) )
	{
		numPoints = 0;
		return;
	}
	for( i = 0; i < n; i++ )
	{
		p[i].ToVec3() = verts[i];
		p[i].s = p[i].t = 0;
	}
	numPoints = n;
}

BUD_INLINE budFixedWinding::budFixedWinding( const budVec3& normal, const float dist )
{
	numPoints = 0;
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
	BaseForPlane( normal, dist );
}

BUD_INLINE budFixedWinding::budFixedWinding( const budPlane& plane )
{
	numPoints = 0;
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
	BaseForPlane( plane );
}

BUD_INLINE budFixedWinding::budFixedWinding( const idWinding& winding )
{
	int i;
	
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
	if( !EnsureAlloced( winding.GetNumPoints() ) )
	{
		numPoints = 0;
		return;
	}
	for( i = 0; i < winding.GetNumPoints(); i++ )
	{
		p[i] = winding[i];
	}
	numPoints = winding.GetNumPoints();
}

BUD_INLINE budFixedWinding::budFixedWinding( const budFixedWinding& winding )
{
	int i;
	
	p = data;
	allocedSize = MAX_POINTS_ON_WINDING;
	if( !EnsureAlloced( winding.GetNumPoints() ) )
	{
		numPoints = 0;
		return;
	}
	for( i = 0; i < winding.GetNumPoints(); i++ )
	{
		p[i] = winding[i];
	}
	numPoints = winding.GetNumPoints();
}

BUD_INLINE budFixedWinding::~budFixedWinding()
{
	p = NULL;	// otherwise it tries to free the fixed buffer
}

BUD_INLINE budFixedWinding& budFixedWinding::operator=( const idWinding& winding )
{
	int i;
	
	if( !EnsureAlloced( winding.GetNumPoints() ) )
	{
		numPoints = 0;
		return *this;
	}
	for( i = 0; i < winding.GetNumPoints(); i++ )
	{
		p[i] = winding[i];
	}
	numPoints = winding.GetNumPoints();
	return *this;
}

BUD_INLINE void budFixedWinding::Clear()
{
	numPoints = 0;
}
#endif	/* !__WINDING_H__ */
