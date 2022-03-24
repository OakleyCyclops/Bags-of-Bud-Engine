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

#ifndef __AAS_LOCAL_H__
#define __AAS_LOCAL_H__

#include "AAS.h"
#include "../Pvs.h"


class idRoutingCache
{
	friend class budAASLocal;
	
public:
	idRoutingCache( int size );
	~idRoutingCache();
	
	int							Size() const;
	
private:
	int							type;					// portal or area cache
	int							size;					// size of cache
	int							cluster;				// cluster of the cache
	int							areaNum;				// area of the cache
	int							travelFlags;			// combinations of the travel flags
	idRoutingCache* 			next;					// next in list
	idRoutingCache* 			prev;					// previous in list
	idRoutingCache* 			time_next;				// next in time based list
	idRoutingCache* 			time_prev;				// previous in time based list
	unsigned short				startTravelTime;		// travel time to start with
	unsigned char* 				reachabilities;			// reachabilities used for routing
	unsigned short* 			travelTimes;			// travel time for every area
};


class idRoutingUpdate
{
	friend class budAASLocal;
	
private:
	int							cluster;				// cluster number of this update
	int							areaNum;				// area number of this update
	unsigned short				tmpTravelTime;			// temporary travel time
	unsigned short* 			areaTravelTimes;		// travel times within the area
	budVec3						start;					// start point into area
	idRoutingUpdate* 			next;					// next in list
	idRoutingUpdate* 			prev;					// prev in list
	bool						isInList;				// true if the update is in the list
};


class idRoutingObstacle
{
	friend class budAASLocal;
	idRoutingObstacle() { }
	
private:
	budBounds					bounds;					// obstacle bounds
	budList<int, TAG_AAS>					areas;					// areas the bounds are in
};


class budAASLocal : public budAAS
{
public:
	budAASLocal();
	virtual						~budAASLocal();
	virtual bool				Init( const budStr& mapName, unsigned int mapFileCRC );
	virtual void				Shutdown();
	virtual void				Stats() const;
	virtual void				Test( const budVec3& origin );
	virtual const budAASSettings* GetSettings() const;
	virtual int					PointAreaNum( const budVec3& origin ) const;
	virtual int					PointReachableAreaNum( const budVec3& origin, const budBounds& searchBounds, const int areaFlags ) const;
	virtual int					BoundsReachableAreaNum( const budBounds& bounds, const int areaFlags ) const;
	virtual void				PushPointIntoAreaNum( int areaNum, budVec3& origin ) const;
	virtual budVec3				AreaCenter( int areaNum ) const;
	virtual int					AreaFlags( int areaNum ) const;
	virtual int					AreaTravelFlags( int areaNum ) const;
	virtual bool				Trace( aasTrace_t& trace, const budVec3& start, const budVec3& end ) const;
	virtual const budPlane& 		GetPlane( int planeNum ) const;
	virtual int					GetWallEdges( int areaNum, const budBounds& bounds, int travelFlags, int* edges, int maxEdges ) const;
	virtual void				SortWallEdges( int* edges, int numEdges ) const;
	virtual void				GetEdgeVertexNumbers( int edgeNum, int verts[2] ) const;
	virtual void				GetEdge( int edgeNum, budVec3& start, budVec3& end ) const;
	virtual bool				SetAreaState( const budBounds& bounds, const int areaContents, bool disabled );
	virtual aasHandle_t			AddObstacle( const budBounds& bounds );
	virtual void				RemoveObstacle( const aasHandle_t handle );
	virtual void				RemoveAllObstacles();
	virtual int					TravelTimeToGoalArea( int areaNum, const budVec3& origin, int goalAreaNum, int travelFlags ) const;
	virtual bool				RouteToGoalArea( int areaNum, const budVec3 origin, int goalAreaNum, int travelFlags, int& travelTime, idReachability** reach ) const;
	virtual bool				WalkPathToGoal( aasPath_t& path, int areaNum, const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin, int travelFlags ) const;
	virtual bool				WalkPathValid( int areaNum, const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin, int travelFlags, budVec3& endPos, int& endAreaNum ) const;
	virtual bool				FlyPathToGoal( aasPath_t& path, int areaNum, const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin, int travelFlags ) const;
	virtual bool				FlyPathValid( int areaNum, const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin, int travelFlags, budVec3& endPos, int& endAreaNum ) const;
	virtual void				ShowWalkPath( const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin ) const;
	virtual void				ShowFlyPath( const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin ) const;
	virtual bool				FindNearestGoal( aasGoal_t& goal, int areaNum, const budVec3 origin, const budVec3& target, int travelFlags, aasObstacle_t* obstacles, int numObstacles, budAASCallback& callback ) const;
	
private:
	budAASFile* 					file;
	budStr						name;
	
private:	// routing data
	idRoutingCache***			areaCacheIndex;			// for each area in each cluster the travel times to all other areas in the cluster
	int							areaCacheIndexSize;		// number of area cache entries
	idRoutingCache** 			portalCacheIndex;		// for each area in the world the travel times from each portal
	int							portalCacheIndexSize;	// number of portal cache entries
	idRoutingUpdate* 			areaUpdate;				// memory used to update the area routing cache
	idRoutingUpdate* 			portalUpdate;			// memory used to update the portal routing cache
	unsigned short* 			goalAreaTravelTimes;	// travel times to goal areas
	unsigned short* 			areaTravelTimes;		// travel times through the areas
	int							numAreaTravelTimes;		// number of area travel times
	mutable idRoutingCache* 	cacheListStart;			// start of list with cache sorted from oldest to newest
	mutable idRoutingCache* 	cacheListEnd;			// end of list with cache sorted from oldest to newest
	mutable int					totalCacheMemory;		// total cache memory used
	budList<idRoutingObstacle*, TAG_AAS>	obstacleList;			// list with obstacles
	
private:	// routing
	bool						SetupRouting();
	void						ShutdownRouting();
	unsigned short				AreaTravelTime( int areaNum, const budVec3& start, const budVec3& end ) const;
	void						CalculateAreaTravelTimes();
	void						DeleteAreaTravelTimes();
	void						SetupRoutingCache();
	void						DeleteClusterCache( int clusterNum );
	void						DeletePortalCache();
	void						ShutdownRoutingCache();
	void						RoutingStats() const;
	void						LinkCache( idRoutingCache* cache ) const;
	void						UnlinkCache( idRoutingCache* cache ) const;
	void						DeleteOldestCache() const;
	idReachability* 			GetAreaReachability( int areaNum, int reachabilityNum ) const;
	int							ClusterAreaNum( int clusterNum, int areaNum ) const;
	void						UpdateAreaRoutingCache( idRoutingCache* areaCache ) const;
	idRoutingCache* 			GetAreaRoutingCache( int clusterNum, int areaNum, int travelFlags ) const;
	void						UpdatePortalRoutingCache( idRoutingCache* portalCache ) const;
	idRoutingCache* 			GetPortalRoutingCache( int clusterNum, int areaNum, int travelFlags ) const;
	void						RemoveRoutingCacheUsingArea( int areaNum );
	void						DisableArea( int areaNum );
	void						EnableArea( int areaNum );
	bool						SetAreaState_r( int nodeNum, const budBounds& bounds, const int areaContents, bool disabled );
	void						GetBoundsAreas_r( int nodeNum, const budBounds& bounds, budList<int>& areas ) const;
	void						SetObstacleState( const idRoutingObstacle* obstacle, bool enable );
	
private:	// pathing
	bool						EdgeSplitPoint( budVec3& split, int edgeNum, const budPlane& plane ) const;
	bool						FloorEdgeSplitPoint( budVec3& split, int areaNum, const budPlane& splitPlane, const budPlane& frontPlane, bool closest ) const;
	budVec3						SubSampleWalkPath( int areaNum, const budVec3& origin, const budVec3& start, const budVec3& end, int travelFlags, int& endAreaNum ) const;
	budVec3						SubSampleFlyPath( int areaNum, const budVec3& origin, const budVec3& start, const budVec3& end, int travelFlags, int& endAreaNum ) const;
	
private:	// debug
	const budBounds& 			DefaultSearchBounds() const;
	void						DrawCone( const budVec3& origin, const budVec3& dir, float radius, const budVec4& color ) const;
	void						DrawArea( int areaNum ) const;
	void						DrawFace( int faceNum, bool side ) const;
	void						DrawEdge( int edgeNum, bool arrow ) const;
	void						DrawReachability( const idReachability* reach ) const;
	void						ShowArea( const budVec3& origin ) const;
	void						ShowWallEdges( const budVec3& origin ) const;
	void						ShowHideArea( const budVec3& origin, int targerAreaNum ) const;
	bool						PullPlayer( const budVec3& origin, int toAreaNum ) const;
	void						RandomPullPlayer( const budVec3& origin ) const;
	void						ShowPushIntoArea( const budVec3& origin ) const;
};

#endif /* !__AAS_LOCAL_H__ */
