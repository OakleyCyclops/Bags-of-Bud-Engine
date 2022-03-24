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

#ifndef __AAS_H__
#define __AAS_H__

/*
===============================================================================

	Area Awareness System

===============================================================================
*/

enum
{
	PATHTYPE_WALK,
	PATHTYPE_WALKOFFLEDGE,
	PATHTYPE_BARRIERJUMP,
	PATHTYPE_JUMP
};

typedef struct aasPath_s
{
	int							type;			// path type
	budVec3						moveGoal;		// point the AI should move towards
	int							moveAreaNum;	// number of the area the AI should move towards
	budVec3						secondaryGoal;	// secondary move goal for complex navigation
	const idReachability* 		reachability;	// reachability used for navigation
} aasPath_t;


typedef struct aasGoal_s
{
	int							areaNum;		// area the goal is in
	budVec3						origin;			// position of goal
} aasGoal_t;


typedef struct aasObstacle_s
{
	budBounds					absBounds;		// absolute bounds of obstacle
	budBounds					expAbsBounds;	// expanded absolute bounds of obstacle
} aasObstacle_t;

class budAASCallback
{
public:
	virtual						~budAASCallback() {};
	virtual	bool				TestArea( const class budAAS* aas, int areaNum ) = 0;
};

typedef int aasHandle_t;

class budAAS
{
public:
	static budAAS* 				Alloc();
	virtual						~budAAS() = 0;
	// Initialize for the given map.
	virtual bool				Init( const budStr& mapName, unsigned int mapFileCRC ) = 0;
	// Print AAS stats.
	virtual void				Stats() const = 0;
	// Test from the given origin.
	virtual void				Test( const budVec3& origin ) = 0;
	// Get the AAS settings.
	virtual const budAASSettings* GetSettings() const = 0;
	// Returns the number of the area the origin is in.
	virtual int					PointAreaNum( const budVec3& origin ) const = 0;
	// Returns the number of the nearest reachable area for the given point.
	virtual int					PointReachableAreaNum( const budVec3& origin, const budBounds& bounds, const int areaFlags ) const = 0;
	// Returns the number of the first reachable area in or touching the bounds.
	virtual int					BoundsReachableAreaNum( const budBounds& bounds, const int areaFlags ) const = 0;
	// Push the point into the area.
	virtual void				PushPointIntoAreaNum( int areaNum, budVec3& origin ) const = 0;
	// Returns a reachable point inside the given area.
	virtual budVec3				AreaCenter( int areaNum ) const = 0;
	// Returns the area flags.
	virtual int					AreaFlags( int areaNum ) const = 0;
	// Returns the travel flags for traveling through the area.
	virtual int					AreaTravelFlags( int areaNum ) const = 0;
	// Trace through the areas and report the first collision.
	virtual bool				Trace( aasTrace_t& trace, const budVec3& start, const budVec3& end ) const = 0;
	// Get a plane for a trace.
	virtual const budPlane& 		GetPlane( int planeNum ) const = 0;
	// Get wall edges.
	virtual int					GetWallEdges( int areaNum, const budBounds& bounds, int travelFlags, int* edges, int maxEdges ) const = 0;
	// Sort the wall edges to create continuous sequences of walls.
	virtual void				SortWallEdges( int* edges, int numEdges ) const = 0;
	// Get the vertex numbers for an edge.
	virtual void				GetEdgeVertexNumbers( int edgeNum, int verts[2] ) const = 0;
	// Get an edge.
	virtual void				GetEdge( int edgeNum, budVec3& start, budVec3& end ) const = 0;
	// Find all areas within or touching the bounds with the given contents and disable/enable them for routing.
	virtual bool				SetAreaState( const budBounds& bounds, const int areaContents, bool disabled ) = 0;
	// Add an obstacle to the routing system.
	virtual aasHandle_t			AddObstacle( const budBounds& bounds ) = 0;
	// Remove an obstacle from the routing system.
	virtual void				RemoveObstacle( const aasHandle_t handle ) = 0;
	// Remove all obstacles from the routing system.
	virtual void				RemoveAllObstacles() = 0;
	// Returns the travel time towards the goal area in 100th of a second.
	virtual int					TravelTimeToGoalArea( int areaNum, const budVec3& origin, int goalAreaNum, int travelFlags ) const = 0;
	// Get the travel time and first reachability to be used towards the goal, returns true if there is a path.
	virtual bool				RouteToGoalArea( int areaNum, const budVec3 origin, int goalAreaNum, int travelFlags, int& travelTime, idReachability** reach ) const = 0;
	// Creates a walk path towards the goal.
	virtual bool				WalkPathToGoal( aasPath_t& path, int areaNum, const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin, int travelFlags ) const = 0;
	// Returns true if one can walk along a straight line from the origin to the goal origin.
	virtual bool				WalkPathValid( int areaNum, const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin, int travelFlags, budVec3& endPos, int& endAreaNum ) const = 0;
	// Creates a fly path towards the goal.
	virtual bool				FlyPathToGoal( aasPath_t& path, int areaNum, const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin, int travelFlags ) const = 0;
	// Returns true if one can fly along a straight line from the origin to the goal origin.
	virtual bool				FlyPathValid( int areaNum, const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin, int travelFlags, budVec3& endPos, int& endAreaNum ) const = 0;
	// Show the walk path from the origin towards the area.
	virtual void				ShowWalkPath( const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin ) const = 0;
	// Show the fly path from the origin towards the area.
	virtual void				ShowFlyPath( const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin ) const = 0;
	// Find the nearest goal which satisfies the callback.
	virtual bool				FindNearestGoal( aasGoal_t& goal, int areaNum, const budVec3 origin, const budVec3& target, int travelFlags, aasObstacle_t* obstacles, int numObstacles, budAASCallback& callback ) const = 0;
};

#endif /* !__AAS_H__ */
