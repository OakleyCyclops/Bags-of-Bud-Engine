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

#ifndef __SURFACE_H__
#define __SURFACE_H__

/*
===============================================================================

	Surface base class.

	A surface is tesselated to a triangle mesh with each edge shared by
	at most two triangles.

===============================================================================
*/

typedef struct surfaceEdge_s
{
	int						verts[2];	// edge vertices always with ( verts[0] < verts[1] )
	int						tris[2];	// edge triangles
} surfaceEdge_t;


class idSurface
{
public:
	idSurface();
	explicit idSurface( const idSurface& surf );
	explicit idSurface( const budDrawVert* verts, const int numVerts, const int* indexes, const int numIndexes );
	~idSurface();
	
	const budDrawVert& 		operator[]( const int index ) const;
	budDrawVert& 			operator[]( const int index );
	idSurface& 				operator+=( const idSurface& surf );
	
	int						GetNumIndexes() const
	{
		return indexes.Num();
	}
	const int* 				GetIndexes() const
	{
		return indexes.Ptr();
	}
	int						GetNumVertices() const
	{
		return verts.Num();
	}
	const budDrawVert* 		GetVertices() const
	{
		return verts.Ptr();
	}
	const int* 				GetEdgeIndexes() const
	{
		return edgeIndexes.Ptr();
	}
	const surfaceEdge_t* 	GetEdges() const
	{
		return edges.Ptr();
	}
	
	void					Clear();
	void					TranslateSelf( const budVec3& translation );
	void					RotateSelf( const budMat3& rotation );
	
	// splits the surface into a front and back surface, the surface itself stays unchanged
	// frontOnPlaneEdges and backOnPlaneEdges optionally store the indexes to the edges that lay on the split plane
	// returns a SIDE_?
	int						Split( const budPlane& plane, const float epsilon, idSurface** front, idSurface** back, int* frontOnPlaneEdges = NULL, int* backOnPlaneEdges = NULL ) const;
	// cuts off the part at the back side of the plane, returns true if some part was at the front
	// if there is nothing at the front the number of points is set to zero
	bool					ClipInPlace( const budPlane& plane, const float epsilon = ON_EPSILON, const bool keepOn = false );
	
	// returns true if each triangle can be reached from any other triangle by a traversal
	bool					IsConnected() const;
	// returns true if the surface is closed
	bool					IsClosed() const;
	// returns true if the surface is a convex hull
	bool					IsPolytope( const float epsilon = 0.1f ) const;
	
	float					PlaneDistance( const budPlane& plane ) const;
	int						PlaneSide( const budPlane& plane, const float epsilon = ON_EPSILON ) const;
	
	// returns true if the line intersects one of the surface triangles
	bool					LineIntersection( const budVec3& start, const budVec3& end, bool backFaceCull = false ) const;
	// intersection point is start + dir * scale
	bool					RayIntersection( const budVec3& start, const budVec3& dir, float& scale, bool backFaceCull = false ) const;
	
protected:
	budList<budDrawVert, TAG_libBud_LIST_SURFACE>		verts;			// vertices
	budList<int, TAG_libBud_LIST_SURFACE>				indexes;		// 3 references to vertices for each triangle
	budList<surfaceEdge_t, TAG_libBud_LIST_SURFACE>	edges;			// edges
	budList<int, TAG_libBud_LIST_SURFACE>				edgeIndexes;	// 3 references to edges for each triangle, may be negative for reversed edge
	
protected:
	void					GenerateEdgeIndexes();
	int						FindEdge( int v1, int v2 ) const;
};

/*
====================
idSurface::idSurface
====================
*/
BUD_INLINE idSurface::idSurface()
{
}

/*
=================
idSurface::idSurface
=================
*/
BUD_INLINE idSurface::idSurface( const budDrawVert* verts, const int numVerts, const int* indexes, const int numIndexes )
{
	assert( verts != NULL && indexes != NULL && numVerts > 0 && numIndexes > 0 );
	this->verts.SetNum( numVerts );
	memcpy( this->verts.Ptr(), verts, numVerts * sizeof( verts[0] ) );
	this->indexes.SetNum( numIndexes );
	memcpy( this->indexes.Ptr(), indexes, numIndexes * sizeof( indexes[0] ) );
	GenerateEdgeIndexes();
}

/*
====================
idSurface::idSurface
====================
*/
BUD_INLINE idSurface::idSurface( const idSurface& surf )
{
	this->verts = surf.verts;
	this->indexes = surf.indexes;
	this->edges = surf.edges;
	this->edgeIndexes = surf.edgeIndexes;
}

/*
====================
idSurface::~idSurface
====================
*/
BUD_INLINE idSurface::~idSurface()
{
}

/*
=================
idSurface::operator[]
=================
*/
BUD_INLINE const budDrawVert& idSurface::operator[]( const int index ) const
{
	return verts[ index ];
};

/*
=================
idSurface::operator[]
=================
*/
BUD_INLINE budDrawVert& idSurface::operator[]( const int index )
{
	return verts[ index ];
};

/*
=================
idSurface::operator+=
=================
*/
BUD_INLINE idSurface& idSurface::operator+=( const idSurface& surf )
{
	int i, m, n;
	n = verts.Num();
	m = indexes.Num();
	verts.Append( surf.verts );			// merge verts where possible ?
	indexes.Append( surf.indexes );
	for( i = m; i < indexes.Num(); i++ )
	{
		indexes[i] += n;
	}
	GenerateEdgeIndexes();
	return *this;
}

/*
=================
idSurface::Clear
=================
*/
BUD_INLINE void idSurface::Clear()
{
	verts.Clear();
	indexes.Clear();
	edges.Clear();
	edgeIndexes.Clear();
}

/*
=================
idSurface::TranslateSelf
=================
*/
BUD_INLINE void idSurface::TranslateSelf( const budVec3& translation )
{
	for( int i = 0; i < verts.Num(); i++ )
	{
		verts[i].xyz += translation;
	}
}

/*
=================
idSurface::RotateSelf
=================
*/
BUD_INLINE void idSurface::RotateSelf( const budMat3& rotation )
{
	for( int i = 0; i < verts.Num(); i++ )
	{
		verts[i].xyz *= rotation;
		verts[i].SetNormal( verts[i].GetNormal() * rotation );
		verts[i].SetTangent( verts[i].GetTangent() * rotation );
	}
}

#endif /* !__SURFACE_H__ */
