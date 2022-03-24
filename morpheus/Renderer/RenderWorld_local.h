/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2014-2016 Robert Beckebans
Copyright (C) 2014-2016 Kot in Action Creative Artel

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

#ifndef __RENDERWORLDLOCAL_H__
#define __RENDERWORLDLOCAL_H__

#include "BoundsTrack.h"

// assume any lightDef or entityDef index above this is an internal error
const int LUDICROUS_INDEX	= 10000;


typedef struct portal_s
{
	int						intoArea;		// area this portal leads to
	idWinding* 				w;				// winding points have counter clockwise ordering seen this area
	budPlane					plane;			// view must be on the positive side of the plane to cross
	struct portal_s* 		next;			// next portal of the area
	struct doublePortal_s* 	doublePortal;
} portal_t;


typedef struct doublePortal_s
{
	struct portal_s*			portals[2];
	int						blockingBits;	// PS_BLOCK_VIEW, PS_BLOCK_AIR, etc, set by doors that shut them off
	
	// A portal will be considered closed if it is past the
	// fog-out point in a fog volume.  We only support a single
	// fog volume over each portal.
	budRenderLightLocal* 		fogLight;
	struct doublePortal_s* 	nextFoggedPortal;
} doublePortal_t;


typedef struct portalArea_s
{
	int				areaNum;
	int				connectedAreaNum[NUM_PORTAL_ATTRIBUTES];	// if two areas have matching connectedAreaNum, they are
	// not separated by a portal with the apropriate PS_BLOCK_* blockingBits
	int				viewCount;		// set by R_FindViewLightsAndEntities
	portal_t* 		portals;		// never changes after load
	areaReference_t	entityRefs;		// head/tail of doubly linked list, may change
	areaReference_t	lightRefs;		// head/tail of doubly linked list, may change
} portalArea_t;


static const int	CHILDREN_HAVE_MULTIPLE_AREAS = -2;
static const int	AREANUM_SOLID = -1;
typedef struct
{
	budPlane			plane;
	int				children[2];		// negative numbers are (-1 - areaNumber), 0 = solid
	int				commonChildrenArea;	// if all children are either solid or a single area,
	// this is the area number, else CHILDREN_HAVE_MULTIPLE_AREAS
} areaNode_t;

struct reusableDecal_t
{
	qhandle_t				entityHandle;
	int						lastStartTime;
	budRenderModelDecal* 	decals;
};

struct reusableOverlay_t
{
	qhandle_t				entityHandle;
	int						lastStartTime;
	budRenderModelOverlay* 	overlays;
};

struct portalStack_t;

class budRenderWorldLocal : public budRenderWorld
{
public:
	budRenderWorldLocal();
	virtual					~budRenderWorldLocal();
	
	virtual	bool			InitFromMap( const char* mapName );
	virtual void			ResetLocalRenderModels();				// Fixes a crash when switching between expansion packs in Doom3:BFG
	
	virtual	qhandle_t		AddEntityDef( const renderEntity_t* re );
	virtual	void			UpdateEntityDef( qhandle_t entityHandle, const renderEntity_t* re );
	virtual	void			FreeEntityDef( qhandle_t entityHandle );
	virtual const renderEntity_t* GetRenderEntity( qhandle_t entityHandle ) const;
	
	virtual	qhandle_t		AddLightDef( const renderLight_t* rlight );
	virtual	void			UpdateLightDef( qhandle_t lightHandle, const renderLight_t* rlight );
	virtual	void			FreeLightDef( qhandle_t lightHandle );
	virtual const renderLight_t* GetRenderLight( qhandle_t lightHandle ) const;
	
	virtual bool			CheckAreaForPortalSky( int areaNum );
	
	virtual	void			GenerateAllInteractions();
	virtual void			RegenerateWorld();
	
	virtual void			ProjectDecalOntoWorld( const budFixedWinding& winding, const budVec3& projectionOrigin, const bool parallel, const float fadeDepth, const budMaterial* material, const int startTime );
	virtual void			ProjectDecal( qhandle_t entityHandle, const budFixedWinding& winding, const budVec3& projectionOrigin, const bool parallel, const float fadeDepth, const budMaterial* material, const int startTime );
	virtual void			ProjectOverlay( qhandle_t entityHandle, const budPlane localTextureAxis[2], const budMaterial* material, const int startTime );
	virtual void			RemoveDecals( qhandle_t entityHandle );
	
	virtual void			SetRenderView( const renderView_t* renderView );
	virtual	void			RenderScene( const renderView_t* renderView );
	
	virtual	int				NumAreas() const;
	virtual int				PointInArea( const budVec3& point ) const;
	virtual int				BoundsInAreas( const budBounds& bounds, int* areas, int maxAreas ) const;
	virtual	int				NumPortalsInArea( int areaNum );
	virtual exitPortal_t	GetPortal( int areaNum, int portalNum );
	
	virtual	guiPoint_t		GuiTrace( qhandle_t entityHandle, const budVec3 start, const budVec3 end ) const;
	virtual bool			ModelTrace( modelTrace_t& trace, qhandle_t entityHandle, const budVec3& start, const budVec3& end, const float radius ) const;
	virtual bool			Trace( modelTrace_t& trace, const budVec3& start, const budVec3& end, const float radius, bool skipDynamic = true, bool skipPlayer = false ) const;
	virtual bool			FastWorldTrace( modelTrace_t& trace, const budVec3& start, const budVec3& end ) const;
	
	virtual void			DebugClearLines( int time );
	virtual void			DebugLine( const budVec4& color, const budVec3& start, const budVec3& end, const int lifetime = 0, const bool depthTest = false );
	virtual void			DebugArrow( const budVec4& color, const budVec3& start, const budVec3& end, int size, const int lifetime = 0 );
	virtual void			DebugWinding( const budVec4& color, const idWinding& w, const budVec3& origin, const budMat3& axis, const int lifetime = 0, const bool depthTest = false );
	virtual void			DebugCircle( const budVec4& color, const budVec3& origin, const budVec3& dir, const float radius, const int numSteps, const int lifetime = 0, const bool depthTest = false );
	virtual void			DebugSphere( const budVec4& color, const budSphere& sphere, const int lifetime = 0, bool depthTest = false );
	virtual void			DebugBounds( const budVec4& color, const budBounds& bounds, const budVec3& org = vec3_origin, const int lifetime = 0 );
	virtual void			DebugBox( const budVec4& color, const budBox& box, const int lifetime = 0 );
	virtual void			DebugCone( const budVec4& color, const budVec3& apex, const budVec3& dir, float radius1, float radius2, const int lifetime = 0 );
	virtual void			DebugScreenRect( const budVec4& color, const budScreenRect& rect, const viewDef_t* viewDef, const int lifetime = 0 );
	virtual void			DebugAxis( const budVec3& origin, const budMat3& axis );
	
	virtual void			DebugClearPolygons( int time );
	virtual void			DebugPolygon( const budVec4& color, const idWinding& winding, const int lifeTime = 0, const bool depthTest = false );
	
	virtual void			DrawText( const char* text, const budVec3& origin, float scale, const budVec4& color, const budMat3& viewAxis, const int align = 1, const int lifetime = 0, bool depthTest = false );
	
	//-----------------------
	
	budStr					mapName;				// ie: maps/tim_dm2.proc, written to demoFile
	ID_TIME_T				mapTimeStamp;			// for fast reloads of the same level
	
	areaNode_t* 			areaNodes;
	int						numAreaNodes;
	
	portalArea_t* 			portalAreas;
	int						numPortalAreas;
	int						connectedAreaNum;		// incremented every time a door portal state changes
	
	budScreenRect* 			areaScreenRect;
	
	doublePortal_t* 		doublePortals;
	int						numInterAreaPortals;
	
	budList<budRenderModel*, TAG_MODEL>	localModels;
	
	budList<budRenderEntityLocal*, TAG_ENTITY>	entityDefs;
	budList<budRenderLightLocal*, TAG_LIGHT>		lightDefs;
	
	idBlockAlloc<areaReference_t, 1024> areaReferenceAllocator;
	idBlockAlloc<budInteraction, 256>	interactionAllocator;
	
#ifdef ID_PC
	static const int MAX_DECAL_SURFACES = 32;
#else
	static const int MAX_DECAL_SURFACES = 16;
#endif
	budArray<reusableDecal_t, MAX_DECAL_SURFACES>	decals;
	budArray<reusableOverlay_t, MAX_DECAL_SURFACES>	overlays;
	
	// all light / entity interactions are referenced here for fast lookup without
	// having to crawl the doubly linked lists.  EnntityDefs are sequential for better
	// cache access, because the table is accessed by light in budRenderWorldLocal::CreateLightDefInteractions()
	// Growing this table is time consuming, so we add a pad value to the number
	// of entityDefs and lightDefs
	budInteraction** 		interactionTable;
	int						interactionTableWidth;		// entityDefs
	int						interactionTableHeight;		// lightDefs
	
	bool					generateAllInteractionsCalled;
	
	//-----------------------
	// RenderWorld_load.cpp
	
	budRenderModel* 			ParseModel( budLexer* src, const char* mapName, ID_TIME_T mapTimeStamp, budFile* fileOut );
	budRenderModel* 			ParseShadowModel( budLexer* src, budFile* fileOut );
	void					SetupAreaRefs();
	void					ParseInterAreaPortals( budLexer* src, budFile* fileOut );
	void					ParseNodes( budLexer* src, budFile* fileOut );
	int						CommonChildrenArea_r( areaNode_t* node );
	void					FreeWorld();
	void					ClearWorld();
	void					FreeDefs();
	void					TouchWorldModels();
	void					AddWorldModelEntities();
	void					ClearPortalStates();
	void					ReadBinaryAreaPortals( budFile* file );
	void					ReadBinaryNodes( budFile* file );
	budRenderModel* 			ReadBinaryModel( budFile* file );
	budRenderModel* 			ReadBinaryShadowModel( budFile* file );
	
	//--------------------------
	// RenderWorld_portals.cpp
	
	bool					CullEntityByPortals( const budRenderEntityLocal* entity, const portalStack_t* ps );
	void					AddAreaViewEntities( int areaNum, const portalStack_t* ps );
	bool					CullLightByPortals( const budRenderLightLocal* light, const portalStack_t* ps );
	void					AddAreaViewLights( int areaNum, const portalStack_t* ps );
	void					AddAreaToView( int areaNum, const portalStack_t* ps );
	budScreenRect			ScreenRectFromWinding( const idWinding* w, const viewEntity_t* space );
	bool					PortalIsFoggedOut( const portal_t* p );
	void					FloodViewThroughArea_r( const budVec3& origin, int areaNum, const portalStack_t* ps );
	void					FlowViewThroughPortals( const budVec3& origin, int numPlanes, const budPlane* planes );
	void					BuildConnectedAreas_r( int areaNum );
	void					BuildConnectedAreas();
	void					FindViewLightsAndEntities();
	
	void					FloodLightThroughArea_r( budRenderLightLocal* light, int areaNum, const portalStack_t* ps );
	void					FlowLightThroughPortals( budRenderLightLocal* light );
	
	int						NumPortals() const;
	qhandle_t				FindPortal( const budBounds& b ) const;
	void					SetPortalState( qhandle_t portal, int blockingBits );
	int						GetPortalState( qhandle_t portal );
	bool					AreasAreConnected( int areaNum1, int areaNum2, portalConnection_t connection ) const;
	void					FloodConnectedAreas( portalArea_t* area, int portalAttributeIndex );
	budScreenRect& 			GetAreaScreenRect( int areaNum ) const
	{
		return areaScreenRect[areaNum];
	}
	
	//--------------------------
	// RenderWorld_demo.cpp
	
	void					StartWritingDemo( budDemoFile* demo );
	void					StopWritingDemo();
	bool					ProcessDemoCommand( budDemoFile* readDemo, renderView_t* demoRenderView, int* demoTimeOffset );
	
	void					WriteLoadMap();
	void					WriteRenderView( const renderView_t* renderView );
	void					WriteVisibleDefs( const viewDef_t* viewDef );
	void					WriteFreeDecal( budDemoFile* f, qhandle_t handle );
	void					WriteFreeOverlay( budDemoFile* f, qhandle_t handle );
	void					WriteFreeLight( qhandle_t handle );
	void					WriteFreeEntity( qhandle_t handle );
	void					WriteRenderDecal( budDemoFile* f, qhandle_t handle );
	void					WriteRenderOverlay( budDemoFile* f, qhandle_t handle );
	void					WriteRenderLight( budDemoFile* f, qhandle_t handle, const renderLight_t* light );
	void					WriteRenderEntity( budDemoFile* f, budRenderEntityLocal* entity );
	void					ReadRenderEntity();
	void					ReadRenderLight();
	
	
	//--------------------------
	// RenderWorld.cpp
	
	void					ResizeInteractionTable();
	
	void					AddEntityRefToArea( budRenderEntityLocal* def, portalArea_t* area );
	void					AddLightRefToArea( budRenderLightLocal* light, portalArea_t* area );
	
	void					RecurseProcBSP_r( modelTrace_t* results, int parentNodeNum, int nodeNum, float p1f, float p2f, const budVec3& p1, const budVec3& p2 ) const;
	void					BoundsInAreas_r( int nodeNum, const budBounds& bounds, int* areas, int* numAreas, int maxAreas ) const;
	
	float					DrawTextLength( const char* text, float scale, int len = 0 );
	
	void					FreeInteractions();
	
	void					PushFrustumIntoTree_r( budRenderEntityLocal* def, budRenderLightLocal* light, const frustumCorners_t& corners, int nodeNum );
	void					PushFrustumIntoTree( budRenderEntityLocal* def, budRenderLightLocal* light, const budRenderMatrix& frustumTransform, const budBounds& frustumBounds );
	
	budRenderModelDecal* 	AllocDecal( qhandle_t newEntityHandle, int startTime );
	budRenderModelOverlay* 	AllocOverlay( qhandle_t newEntityHandle, int startTime );
	
	//-------------------------------
	// tr_light.c
	void					CreateLightDefInteractions( budRenderLightLocal* const ldef, const int renderViewID );
};

// if an entity / light combination has been evaluated and found to not genrate any surfaces or shadows,
// the constant INTERACTION_EMPTY will be stored in the interaction table, int contrasts to NULL, which
// means that the combination has not yet been tested for having surfaces.
static budInteraction* const INTERACTION_EMPTY = ( budInteraction* )1;

void R_ListRenderLightDefs_f( const budCmdArgs& args );
void R_ListRenderEntityDefs_f( const budCmdArgs& args );

#endif /* !__RENDERWORLDLOCAL_H__ */
