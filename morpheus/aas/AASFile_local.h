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

#ifndef __AASFILELOCAL_H__
#define __AASFILELOCAL_H__

/*
===============================================================================

	AAS File Local

===============================================================================
*/

class budAASFileLocal : public budAASFile
{
	friend class budAASBuild;
	friend class budAASReach;
	friend class budAASCluster;
public:
	budAASFileLocal();
	virtual 					~budAASFileLocal();
	
public:
	virtual budVec3				EdgeCenter( int edgeNum ) const;
	virtual budVec3				FaceCenter( int faceNum ) const;
	virtual budVec3				AreaCenter( int areaNum ) const;
	
	virtual budBounds			EdgeBounds( int edgeNum ) const;
	virtual budBounds			FaceBounds( int faceNum ) const;
	virtual budBounds			AreaBounds( int areaNum ) const;
	
	virtual int					PointAreaNum( const budVec3& origin ) const;
	virtual int					PointReachableAreaNum( const budVec3& origin, const budBounds& searchBounds, const int areaFlags, const int excludeTravelFlags ) const;
	virtual int					BoundsReachableAreaNum( const budBounds& bounds, const int areaFlags, const int excludeTravelFlags ) const;
	virtual void				PushPointIntoAreaNum( int areaNum, budVec3& point ) const;
	virtual bool				Trace( aasTrace_t& trace, const budVec3& start, const budVec3& end ) const;
	virtual void				PrintInfo() const;
	
public:
	bool						Load( const budStr& fileName, unsigned int mapFileCRC );
	bool						Write( const budStr& fileName, unsigned int mapFileCRC );
	
	int							MemorySize() const;
	void						ReportRoutingEfficiency() const;
	void						Optimize();
	void						LinkReversedReachability();
	void						FinishAreas();
	
	void						Clear();
	void						DeleteReachabilities();
	void						DeleteClusters();
	
private:
	bool						ParseIndex( budLexer& src, budList<aasIndex_t>& indexes );
	bool						ParsePlanes( budLexer& src );
	bool						ParseVertices( budLexer& src );
	bool						ParseEdges( budLexer& src );
	bool						ParseFaces( budLexer& src );
	bool						ParseReachabilities( budLexer& src, int areaNum );
	bool						ParseAreas( budLexer& src );
	bool						ParseNodes( budLexer& src );
	bool						ParsePortals( budLexer& src );
	bool						ParseClusters( budLexer& src );
	
private:
	int							BoundsReachableAreaNum_r( int nodeNum, const budBounds& bounds, const int areaFlags, const int excludeTravelFlags ) const;
	void						MaxTreeDepth_r( int nodeNum, int& depth, int& maxDepth ) const;
	int							MaxTreeDepth() const;
	int							AreaContentsTravelFlags( int areaNum ) const;
	budVec3						AreaReachableGoal( int areaNum ) const;
	int							NumReachabilities() const;
};

#endif /* !__AASFILELOCAL_H__ */
