/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2012-2016 Robert Beckebans
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

#ifndef __TR_LOCAL_H__
#define __TR_LOCAL_H__

#include "PCH.hpp"

#include "GLState.h"
#include "ScreenRect.h"
#include "Image.h"
#include "Font.h"
#include "Framebuffer.h"



// maximum texture units
const int MAX_PROG_TEXTURE_PARMS	= 16;

const int FALLOFF_TEXTURE_SIZE		= 64;

const float	DEFAULT_FOG_DISTANCE	= 500.0f;

// picky to get the bilerp correct at terminator
const int FOG_ENTER_SIZE			= 64;
const float FOG_ENTER				= ( FOG_ENTER_SIZE + 1.0f ) / ( FOG_ENTER_SIZE * 2 );


enum demoCommand_t
{
	DC_BAD,
	DC_RENDERVIEW,
	DC_UPDATE_ENTITYDEF,
	DC_DELETE_ENTITYDEF,
	DC_UPDATE_LIGHTDEF,
	DC_DELETE_LIGHTDEF,
	DC_LOADMAP,
	DC_CROP_RENDER,
	DC_UNCROP_RENDER,
	DC_CAPTURE_RENDER,
	DC_END_FRAME,
	DC_DEFINE_MODEL,
	DC_SET_PORTAL_STATE,
	DC_UPDATE_SOUNDOCCLUSION,
	DC_GUI_MODEL,
	DC_UPDATE_DECAL,
	DC_DELETE_DECAL,
	DC_UPDATE_OVERLAY,
	DC_DELETE_OVERLAY,
	DC_CACHE_SKINS,
	DC_CACHE_PARTICLES,
	DC_CACHE_MATERIALS,
};

/*
==============================================================================

SURFACES

==============================================================================
*/

#include "ModelDecal.h"
#include "ModelOverlay.h"
#include "Interaction.h"

class budRenderWorldLocal;
struct viewEntity_t;
struct viewLight_t;

// drawSurf_t structures command the back end to render surfaces
// a given srfTriangles_t may be used with multiple viewEntity_t,
// as when viewed in a subview or multiple viewport render, or
// with multiple shaders when skinned, or, possibly with multiple
// lights, although currently each lighting interaction creates
// unique srfTriangles_t
// drawSurf_t are always allocated and freed every frame, they are never cached

struct drawSurf_t
{
	const srfTriangles_t* 	frontEndGeo;		// don't use on the back end, it may be updated by the front end!
	int						numIndexes;
	vertCacheHandle_t		indexCache;			// triIndex_t
	vertCacheHandle_t		ambientCache;		// budDrawVert
	vertCacheHandle_t		shadowCache;		// idShadowVert / idShadowVertSkinned
	vertCacheHandle_t		jointCache;			// budJointMat
	const viewEntity_t* 	space;
	const budMaterial* 		material;			// may be NULL for shadow volumes
	uint64					extraGLState;		// Extra GL state |'d with material->stage[].drawStateBits
	float					sort;				// material->sort, modified by gui / entity sort offsets
	const float* 				shaderRegisters;	// evaluated and adjusted for referenceShaders
	drawSurf_t* 			nextOnLight;		// viewLight chains
	drawSurf_t** 			linkChain;			// defer linking to lights to a serial section to avoid a mutex
	budScreenRect			scissorRect;		// for scissor clipping, local inside renderView viewport
	int						renderZFail;
	volatile shadowVolumeState_t shadowVolumeState;
};

// areas have references to hold all the lights and entities in them
struct areaReference_t
{
	areaReference_t* 		areaNext;				// chain in the area
	areaReference_t* 		areaPrev;
	areaReference_t* 		ownerNext;				// chain on either the entityDef or lightDef
	budRenderEntityLocal* 	entity;					// only one of entity / light will be non-NULL
	budRenderLightLocal* 	light;					// only one of entity / light will be non-NULL
	struct portalArea_s*		area;					// so owners can find all the areas they are in
};


// budRenderLight should become the new public interface replacing the qhandle_t to light defs in the budRenderWorld interface
class budRenderLight
{
public:
	virtual					~budRenderLight() {}
	
	virtual void			FreeRenderLight() = 0;
	virtual void			UpdateRenderLight( const renderLight_t* re, bool forceUpdate = false ) = 0;
	virtual void			GetRenderLight( renderLight_t* re ) = 0;
	virtual void			ForceUpdate() = 0;
	virtual int				GetIndex() = 0;
};


// budRenderEntity should become the new public interface replacing the qhandle_t to entity defs in the budRenderWorld interface
class budRenderEntity
{
public:
	virtual					~budRenderEntity() {}
	
	virtual void			FreeRenderEntity() = 0;
	virtual void			UpdateRenderEntity( const renderEntity_t* re, bool forceUpdate = false ) = 0;
	virtual void			GetRenderEntity( renderEntity_t* re ) = 0;
	virtual void			ForceUpdate() = 0;
	virtual int				GetIndex() = 0;
	
	// overlays are extra polygons that deform with animating models for blood and damage marks
	virtual void			ProjectOverlay( const budPlane localTextureAxis[2], const budMaterial* material ) = 0;
	virtual void			RemoveDecals() = 0;
};


class budRenderLightLocal : public budRenderLight
{
public:
	budRenderLightLocal();
	
	virtual void			FreeRenderLight();
	virtual void			UpdateRenderLight( const renderLight_t* re, bool forceUpdate = false );
	virtual void			GetRenderLight( renderLight_t* re );
	virtual void			ForceUpdate();
	virtual int				GetIndex();
	
	bool					LightCastsShadows() const
	{
		return parms.forceShadows || ( !parms.noShadows && lightShader->LightCastsShadows() );
	}
	
	renderLight_t			parms;					// specification
	
	bool					lightHasMoved;			// the light has changed its position since it was
	// first added, so the prelight model is not valid
	budRenderWorldLocal* 	world;
	int						index;					// in world lightdefs
	
	int						areaNum;				// if not -1, we may be able to cull all the light's
	// interactions if !viewDef->connectedAreas[areaNum]
	
	int						lastModifiedFrameNum;	// to determine if it is constantly changing,
	// and should go in the dynamic frame memory, or kept
	// in the cached memory
	bool					archived;				// for demo writing
	
	
	// derived information
	budPlane					lightProject[4];		// old style light projection where Z and W are flipped and projected lights lightProject[3] is divided by ( zNear + zFar )
	budRenderMatrix			baseLightProject;		// global xyz1 to projected light strq
	budRenderMatrix			inverseBaseLightProject;// transforms the zero-to-one cube to exactly cover the light in world space
	
	const budMaterial* 		lightShader;			// guaranteed to be valid, even if parms.shader isn't
	budImage* 				falloffImage;
	
	Vector3					globalLightOrigin;		// accounting for lightCenter and parallel
	budBounds				globalLightBounds;
	
	int						viewCount;				// if == tr.viewCount, the light is on the viewDef->viewLights list
	viewLight_t* 			viewLight;
	
	areaReference_t* 		references;				// each area the light is present in will have a lightRef
	budInteraction* 			firstInteraction;		// doubly linked list
	budInteraction* 			lastInteraction;
	
	struct doublePortal_s* 	foggedPortals;
};


class budRenderEntityLocal : public budRenderEntity
{
public:
	budRenderEntityLocal();
	
	virtual void			FreeRenderEntity();
	virtual void			UpdateRenderEntity( const renderEntity_t* re, bool forceUpdate = false );
	virtual void			GetRenderEntity( renderEntity_t* re );
	virtual void			ForceUpdate();
	virtual int				GetIndex();
	
	// overlays are extra polygons that deform with animating models for blood and damage marks
	virtual void			ProjectOverlay( const budPlane localTextureAxis[2], const budMaterial* material );
	virtual void			RemoveDecals();
	
	bool					IsDirectlyVisible() const;
	void					ReadFromDemoFile( class budDemoFile* f );
	void					WriteToDemoFile( class budDemoFile* f ) const;
	renderEntity_t			parms;
	
	float					modelMatrix[16];		// this is just a rearrangement of parms.axis and parms.origin
	budRenderMatrix			modelRenderMatrix;
	budRenderMatrix			inverseBaseModelProject;// transforms the unit cube to exactly cover the model in world space
	
	budRenderWorldLocal* 	world;
	int						index;					// in world entityDefs
	
	int						lastModifiedFrameNum;	// to determine if it is constantly changing,
	// and should go in the dynamic frame memory, or kept
	// in the cached memory
	bool					archived;				// for demo writing
	
	budRenderModel* 			dynamicModel;			// if parms.model->IsDynamicModel(), this is the generated data
	int						dynamicModelFrameCount;	// continuously animating dynamic models will recreate
	// dynamicModel if this doesn't == tr.viewCount
	budRenderModel* 			cachedDynamicModel;
	
	
	// the local bounds used to place entityRefs, either from parms for dynamic entities, or a model bounds
	budBounds				localReferenceBounds;
	
	// axis aligned bounding box in world space, derived from refernceBounds and
	// modelMatrix in R_CreateEntityRefs()
	budBounds				globalReferenceBounds;
	
	// a viewEntity_t is created whenever a budRenderEntityLocal is considered for inclusion
	// in a given view, even if it turns out to not be visible
	int						viewCount;				// if tr.viewCount == viewCount, viewEntity is valid,
	// but the entity may still be off screen
	viewEntity_t* 			viewEntity;				// in frame temporary memory
	
	budRenderModelDecal* 	decals;					// decals that have been projected on this model
	budRenderModelOverlay* 	overlays;				// blood overlays on animated models
	
	areaReference_t* 		entityRefs;				// chain of all references
	budInteraction* 			firstInteraction;		// doubly linked list
	budInteraction* 			lastInteraction;
	
	bool					needsPortalSky;
};

struct shadowOnlyEntity_t
{
	shadowOnlyEntity_t* 	next;
	budRenderEntityLocal*		edef;
};

// viewLights are allocated on the frame temporary stack memory
// a viewLight contains everything that the back end needs out of an budRenderLightLocal,
// which the front end may be modifying simultaniously if running in SMP mode.
// a viewLight may exist even without any surfaces, and may be relevent for fogging,
// but should never exist if its volume does not intersect the view frustum
struct viewLight_t
{
	viewLight_t* 			next;
	
	// back end should NOT reference the lightDef, because it can change when running SMP
	budRenderLightLocal* 	lightDef;
	
	// for scissor clipping, local inside renderView viewport
	// scissorRect.Empty() is true if the viewEntity_t was never actually
	// seen through any portals
	budScreenRect			scissorRect;
	
	// R_AddSingleLight() determined that the light isn't actually needed
	bool					removeFromList;
	
	// R_AddSingleLight builds this list of entities that need to be added
	// to the viewEntities list because they potentially cast shadows into
	// the view, even though the aren't directly visible
	shadowOnlyEntity_t* 	shadowOnlyViewEntities;
	
	enum interactionState_t
	{
		INTERACTION_UNCHECKED,
		INTERACTION_NO,
		INTERACTION_YES
	};
	byte* 					entityInteractionState;		// [numEntities]
	
	Vector3					globalLightOrigin;			// global light origin used by backend
	budPlane					lightProject[4];			// light project used by backend
	budPlane					fogPlane;					// fog plane for backend fog volume rendering
	// RB: added for shadow mapping
	budRenderMatrix			baseLightProject;			// global xyz1 to projected light strq
	bool					pointLight;					// otherwise a projection light (should probably invert the sense of this, because points are way more common)
	bool					parallel;					// lightCenter gives the direction to the light at infinity
	Vector3					lightCenter;				// offset the lighting direction for shading and
	int						shadowLOD;					// level of detail for shadowmap selection
	// RB end
	budRenderMatrix			inverseBaseLightProject;	// the matrix for deforming the 'zeroOneCubeModel' to exactly cover the light volume in world space
	const budMaterial* 		lightShader;				// light shader used by backend
	const float*				shaderRegisters;			// shader registers used by backend
	budImage* 				falloffImage;				// falloff image used by backend
	
	drawSurf_t* 			globalShadows;				// shadow everything
	drawSurf_t* 			localInteractions;			// don't get local shadows
	drawSurf_t* 			localShadows;				// don't shadow local surfaces
	drawSurf_t* 			globalInteractions;			// get shadows from everything
	drawSurf_t* 			translucentInteractions;	// translucent interactions don't get shadows
	
	// R_AddSingleLight will build a chain of parameters here to setup shadow volumes
	preLightShadowVolumeParms_t* 	preLightShadowVolumes;
};

// a viewEntity is created whenever a budRenderEntityLocal is considered for inclusion
// in the current view, but it may still turn out to be culled.
// viewEntity are allocated on the frame temporary stack memory
// a viewEntity contains everything that the back end needs out of a budRenderEntityLocal,
// which the front end may be modifying simultaneously if running in SMP mode.
// A single entityDef can generate multiple viewEntity_t in a single frame, as when seen in a mirror
struct viewEntity_t
{
	viewEntity_t* 			next;
	
	// back end should NOT reference the entityDef, because it can change when running SMP
	budRenderEntityLocal*		entityDef;
	
	// for scissor clipping, local inside renderView viewport
	// scissorRect.Empty() is true if the viewEntity_t was never actually
	// seen through any portals, but was created for shadow casting.
	// a viewEntity can have a non-empty scissorRect, meaning that an area
	// that it is in is visible, and still not be visible.
	budScreenRect			scissorRect;
	
	bool					isGuiSurface;			// force two sided and vertex colors regardless of material setting
	
	bool					skipMotionBlur;
	
	bool					weaponDepthHack;
	float					modelDepthHack;
	
	float					modelMatrix[16];		// local coords to global coords
	float					modelViewMatrix[16];	// local coords to eye coords
	
	budRenderMatrix			mvp;
	
	// parallelAddModels will build a chain of surfaces here that will need to
	// be linked to the lights or added to the drawsurf list in a serial code section
	drawSurf_t* 			drawSurfs;
	
	// R_AddSingleModel will build a chain of parameters here to setup shadow volumes
	staticShadowVolumeParms_t* 		staticShadowVolumes;
	dynamicShadowVolumeParms_t* 	dynamicShadowVolumes;
};


const int	MAX_CLIP_PLANES	= 1;				// we may expand this to six for some subview issues

// RB: added multiple subfrustums for cascaded shadow mapping
enum frustumPlanes_t
{
	FRUSTUM_PLANE_LEFT,
	FRUSTUM_PLANE_RIGHT,
	FRUSTUM_PLANE_BOTTOM,
	FRUSTUM_PLANE_TOP,
	FRUSTUM_PLANE_NEAR,
	FRUSTUM_PLANE_FAR,
	FRUSTUM_PLANES = 6,
	FRUSTUM_CLIPALL = 1 | 2 | 4 | 8 | 16 | 32
};

enum
{
	FRUSTUM_PRIMARY,
	FRUSTUM_CASCADE1,
	FRUSTUM_CASCADE2,
	FRUSTUM_CASCADE3,
	FRUSTUM_CASCADE4,
	FRUSTUM_CASCADE5,
	MAX_FRUSTUMS,
};

typedef budPlane frustum_t[FRUSTUM_PLANES];
// RB end

// viewDefs are allocated on the frame temporary stack memory
struct viewDef_t
{
	// specified in the call to DrawScene()
	renderView_t		renderView;
	
	float				projectionMatrix[16];
	budRenderMatrix		projectionRenderMatrix;	// tech5 version of projectionMatrix
	
	// RB begin
	float				unprojectionToCameraMatrix[16];
	budRenderMatrix		unprojectionToCameraRenderMatrix;
	
	float				unprojectionToWorldMatrix[16];
	budRenderMatrix		unprojectionToWorldRenderMatrix;
	// RB end
	
	viewEntity_t		worldSpace;
	
	budRenderWorldLocal* renderWorld;
	
	Vector3				initialViewAreaOrigin;
	// Used to find the portalArea that view flooding will take place from.
	// for a normal view, the initialViewOrigin will be renderView.viewOrg,
	// but a mirror may put the projection origin outside
	// of any valid area, or in an unconnected area of the map, so the view
	// area must be based on a point just off the surface of the mirror / subview.
	// It may be possible to get a failed portal pass if the plane of the
	// mirror intersects a portal, and the initialViewAreaOrigin is on
	// a different side than the renderView.viewOrg is.
	
	bool				isSubview;				// true if this view is not the main view
	bool				isMirror;				// the portal is a mirror, invert the face culling
	bool				isXraySubview;
	
	bool				isEditor;
	bool				is2Dgui;
	
	int					numClipPlanes;			// mirrors will often use a single clip plane
	budPlane				clipPlanes[MAX_CLIP_PLANES];		// in world space, the positive side
	// of the plane is the visible side
	budScreenRect		viewport;				// in real pixels and proper Y flip
	
	budScreenRect		scissor;
	// for scissor clipping, local inside renderView viewport
	// subviews may only be rendering part of the main view
	// these are real physical pixel values, possibly scaled and offset from the
	// renderView x/y/width/height
	
	viewDef_t* 			superView;				// never go into an infinite subview loop
	const drawSurf_t* 	subviewSurface;
	
	// drawSurfs are the visible surfaces of the viewEntities, sorted
	// by the material sort parameter
	drawSurf_t** 		drawSurfs;				// we don't use an List for this, because
	int					numDrawSurfs;			// it is allocated in frame temporary memory
	int					maxDrawSurfs;			// may be resized
	
	viewLight_t*			viewLights;			// chain of all viewLights effecting view
	viewEntity_t* 		viewEntitys;			// chain of all viewEntities effecting view, including off screen ones casting shadows
	// we use viewEntities as a check to see if a given view consists solely
	// of 2D rendering, which we can optimize in certain ways.  A 2D view will
	// not have any viewEntities
	
	// RB begin
	frustum_t			frustums[MAX_FRUSTUMS];					// positive sides face outward, [4] is the front clip plane
	float				frustumSplitDistances[MAX_FRUSTUMS];
	budRenderMatrix		frustumMVPs[MAX_FRUSTUMS];
	// RB end
	
	int					areaNum;				// -1 = not in a valid area
	
	// An array in frame temporary memory that lists if an area can be reached without
	// crossing a closed door.  This is used to avoid drawing interactions
	// when the light is behind a closed door.
	bool* 				connectedAreas;
};


// complex light / surface interactions are broken up into multiple passes of a
// simple interaction shader
struct drawInteraction_t
{
	const drawSurf_t* 	surf;
	
	budImage* 			bumpImage;
	budImage* 			diffuseImage;
	budImage* 			specularImage;
	
	Vector4				diffuseColor;	// may have a light color baked into it
	Vector4				specularColor;	// may have a light color baked into it
	stageVertexColor_t	vertexColor;	// applies to both diffuse and specular
	
	int					ambientLight;	// use tr.ambientNormalMap instead of normalization cube map
	
	// these are loaded into the vertex program
	Vector4				bumpMatrix[2];
	Vector4				diffuseMatrix[2];
	Vector4				specularMatrix[2];
};

/*
=============================================================

RENDERER BACK END COMMAND QUEUE

TR_CMDS

=============================================================
*/

enum renderCommand_t
{
	RC_NOP,
	RC_DRAW_VIEW_3D,	// may be at a reduced resolution, will be upsampled before 2D GUIs
	RC_DRAW_VIEW_GUI,	// not resolution scaled
	RC_SET_BUFFER,
	RC_COPY_RENDER,
	RC_POST_PROCESS,
};

struct emptyCommand_t
{
	renderCommand_t		commandId;
	renderCommand_t* 	next;
};

struct setBufferCommand_t
{
	renderCommand_t		commandId;
	renderCommand_t* 	next;
	int					buffer;
};

struct drawSurfsCommand_t
{
	renderCommand_t		commandId;
	renderCommand_t* 	next;
	viewDef_t* 			viewDef;
};

struct copyRenderCommand_t
{
	renderCommand_t		commandId;
	renderCommand_t* 	next;
	int					x;
	int					y;
	int					imageWidth;
	int					imageHeight;
	budImage*				image;
	int					cubeFace;					// when copying to a cubeMap
	bool				clearColorAfterCopy;
};

struct postProcessCommand_t
{
	renderCommand_t		commandId;
	renderCommand_t* 	next;
	viewDef_t* 			viewDef;
};

//=======================================================================

// this is the inital allocation for max number of drawsurfs
// in a given view, but it will automatically grow if needed
const int INITIAL_DRAWSURFS =		2048;

enum frameAllocType_t
{
	FRAME_ALLOC_VIEW_DEF,
	FRAME_ALLOC_VIEW_ENTITY,
	FRAME_ALLOC_VIEW_LIGHT,
	FRAME_ALLOC_SURFACE_TRIANGLES,
	FRAME_ALLOC_DRAW_SURFACE,
	FRAME_ALLOC_INTERACTION_STATE,
	FRAME_ALLOC_SHADOW_ONLY_ENTITY,
	FRAME_ALLOC_SHADOW_VOLUME_PARMS,
	FRAME_ALLOC_SHADER_REGISTER,
	FRAME_ALLOC_DRAW_SURFACE_POINTER,
	FRAME_ALLOC_DRAW_COMMAND,
	FRAME_ALLOC_UNKNOWN,
	FRAME_ALLOC_MAX
};

// all of the information needed by the back end must be
// contained in a budFrameData.  This entire structure is
// duplicated so the front and back end can run in parallel
// on an SMP machine.
class budFrameData
{
public:
	budSysInterlockedInteger	frameMemoryAllocated;
	budSysInterlockedInteger	frameMemoryUsed;
	byte* 					frameMemory;
	
	int						highWaterAllocated;	// max used on any frame
	int						highWaterUsed;
	
	// the currently building command list commands can be inserted
	// at the front if needed, as required for dynamically generated textures
	emptyCommand_t* 		cmdHead;	// may be of other command type based on commandId
	emptyCommand_t* 		cmdTail;
};

extern	budFrameData*	frameData;

//=======================================================================

void R_AddDrawViewCmd( viewDef_t* parms, bool guiOnly );
void R_AddDrawPostProcess( viewDef_t* parms );

void R_ReloadGuis_f( const CmdArgs& args );
void R_ListGuis_f( const CmdArgs& args );

void* R_GetCommandBuffer( int bytes );

// this allows a global override of all materials
bool R_GlobalShaderOverride( const budMaterial** shader );

// this does various checks before calling the budDeclSkin
const budMaterial* R_RemapShaderBySkin( const budMaterial* shader, const budDeclSkin* customSkin, const budMaterial* customShader );


//====================================================


/*
** performanceCounters_t
*/
struct performanceCounters_t
{
	int		c_box_cull_in;
	int		c_box_cull_out;
	int		c_createInteractions;	// number of calls to budInteraction::CreateInteraction
	int		c_createShadowVolumes;
	int		c_generateMd5;
	int		c_entityDefCallbacks;
	int		c_alloc;			// counts for R_StaticAllc/R_StaticFree
	int		c_free;
	int		c_visibleViewEntities;
	int		c_shadowViewEntities;
	int		c_viewLights;
	int		c_numViews;			// number of total views rendered
	int		c_deformedSurfaces;	// budMD5Mesh::GenerateSurface
	int		c_deformedVerts;	// budMD5Mesh::GenerateSurface
	int		c_deformedIndexes;	// budMD5Mesh::GenerateSurface
	int		c_tangentIndexes;	// R_DeriveTangents()
	int		c_entityUpdates;
	int		c_lightUpdates;
	int		c_entityReferences;
	int		c_lightReferences;
	int		c_guiSurfs;
	int		frontEndMicroSec;	// sum of time in all RE_RenderScene's in a frame
};

enum vertexLayoutType_t
{
	LAYOUT_UNKNOWN = 0,	// RB: TODO -1
	LAYOUT_DRAW_VERT,
	LAYOUT_DRAW_SHADOW_VERT,
	LAYOUT_DRAW_SHADOW_VERT_SKINNED,
	NUM_VERTEX_LAYOUTS
};

class budParallelJobList;

const int MAX_GUI_SURFACES	= 1024;		// default size of the drawSurfs list for guis, will
// be automatically expanded as needed

static const int MAX_RENDER_CROPS = 8;

#include "RenderBackend.h"

/*
** Most renderer globals are defined here.
** backend functions should never modify any of these fields,
** but may read fields that aren't dynamically modified
** by the frontend.
*/
class budRenderSystemLocal : public budRenderSystem
{
public:
	// external functions
	virtual void			Init();
	virtual void			Shutdown();
	virtual bool			IsInitialized() const
	{
		return bInitialized;
	}
	virtual void			ResetGuiModels();
	virtual void			InitOpenGL();
	virtual void			ShutdownOpenGL();
	virtual bool			IsOpenGLRunning() const;
	virtual bool			IsFullScreen() const;
	virtual stereo3DMode_t	GetStereo3DMode() const;
	virtual bool			HasQuadBufferSupport() const;
	virtual bool			IsStereoScopicRenderingSupported() const;
	virtual stereo3DMode_t	GetStereoScopicRenderingMode() const;
	virtual void			EnableStereoScopicRendering( const stereo3DMode_t mode ) const;
	virtual int				GetWidth() const;
	virtual int				GetHeight() const;
	virtual int				GetVirtualWidth() const;
	virtual int				GetVirtualHeight() const;
	virtual float			GetPixelAspect() const;
	virtual float			GetPhysicalScreenWidthInCentimeters() const;
	virtual budRenderWorld* 	AllocRenderWorld();
	virtual void			FreeRenderWorld( budRenderWorld* rw );
	virtual void			BeginLevelLoad();
	virtual void			EndLevelLoad();
	virtual void			LoadLevelImages();
	virtual void			Preload( const idPreloadManifest& manifest, const char* mapName );
	virtual void			BeginAutomaticBackgroundSwaps( autoRenderIconType_t icon = AUTORENDER_DEFAULTICON );
	virtual void			EndAutomaticBackgroundSwaps();
	virtual bool			AreAutomaticBackgroundSwapsRunning( autoRenderIconType_t* usingAlternateIcon = NULL ) const;
	
	virtual budFont* 		RegisterFont( const char* fontName );
	virtual void			ResetFonts();
	virtual void			PrintMemInfo( MemInfo_t* mi );
	
	virtual void			SetColor( const Vector4& color );
	virtual uint32			GetColor();
	virtual void			SetGLState( const uint64 glState ) ;
	virtual void			DrawFilled( const Vector4& color, float x, float y, float w, float h );
	virtual void			DrawStretchPic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, const budMaterial* material );
	virtual void			DrawStretchPic( const Vector4& topLeft, const Vector4& topRight, const Vector4& bottomRight, const Vector4& bottomLeft, const budMaterial* material );
	virtual void			DrawStretchTri( const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& t1, const Vector2& t2, const Vector2& t3, const budMaterial* material );
	virtual budDrawVert* 	AllocTris( int numVerts, const triIndex_t* indexes, int numIndexes, const budMaterial* material, const stereoDepthType_t stereoType = STEREO_DEPTH_TYPE_NONE );
	virtual void			DrawSmallChar( int x, int y, int ch );
	virtual void			DrawSmallStringExt( int x, int y, const char* string, const Vector4& setColor, bool forceColor );
	virtual void			DrawBigChar( int x, int y, int ch );
	virtual void			DrawBigStringExt( int x, int y, const char* string, const Vector4& setColor, bool forceColor );
	
	virtual void			WriteDemoPics();
	virtual void			WriteEndFrame();
	virtual void			DrawDemoPics();
	virtual const emptyCommand_t* 	SwapCommandBuffers( uint64* frontEndMicroSec, uint64* backEndMicroSec, uint64* shadowMicroSec, uint64* gpuMicroSec );
	
	virtual void			SwapCommandBuffers_FinishRendering( uint64* frontEndMicroSec, uint64* backEndMicroSec, uint64* shadowMicroSec, uint64* gpuMicroSec );
	virtual const emptyCommand_t* 	SwapCommandBuffers_FinishCommandBuffers();
	
	virtual void			RenderCommandBuffers( const emptyCommand_t* commandBuffers );
	virtual void			TakeScreenshot( int width, int height, const char* fileName, int downSample, renderView_t* ref, int exten );
	virtual void			CropRenderSize( int width, int height );
	virtual void			CaptureRenderToImage( const char* imageName, bool clearColorAfterCopy = false );
	virtual void			CaptureRenderToFile( const char* fileName, bool fixAlpha );
	virtual void			UnCrop();
	virtual bool			UploadImage( const char* imageName, const byte* data, int width, int height );
	
	void					PrintPerformanceCounters();
	
	void					SetInitialized()
	{
		bInitialized = true;
	}
	
public:
	// internal functions
	budRenderSystemLocal();
	~budRenderSystemLocal();
	
	void					UpdateStereo3DMode();
	
	void					Clear();
	void					GetCroppedViewport( budScreenRect* viewport );
	void					PerformResolutionScaling( int& newWidth, int& newHeight );
	int						GetFrameCount() const
	{
		return frameCount;
	};
	
	void					OnFrame();
	
public:
	// renderer globals
	bool					registered;		// cleared at shutdown, set at InitOpenGL
	
	bool					takingScreenshot;
	
	int						frameCount;		// incremented every frame
	int						viewCount;		// incremented every view (twice a scene if subviewed)
	// and every R_MarkFragments call
	
	float					frameShaderTime;	// shader time for all non-world 2D rendering
	
	Vector4					ambientLightVector;	// used for "ambient bump mapping"
	
	List<budRenderWorldLocal*>worlds;
	
	budRenderWorldLocal* 	primaryWorld;
	renderView_t			primaryRenderView;
	viewDef_t* 				primaryView;
	// many console commands need to know which world they should operate on
	
	const budMaterial* 		whiteMaterial;
	const budMaterial* 		charSetMaterial;
	const budMaterial* 		defaultPointLight;
	const budMaterial* 		defaultProjectedLight;
	const budMaterial* 		defaultMaterial;
	budImage* 				testImage;
	budCinematic* 			testVideo;
	int						testVideoStartTime;
	
	budImage* 				ambientCubeImage;	// hack for testing dependent ambient lighting
	
	viewDef_t* 				viewDef;
	
	performanceCounters_t	pc;					// performance counters
	
	viewEntity_t			identitySpace;		// can use if we don't know viewDef->worldSpace is valid
	
	budScreenRect			renderCrops[MAX_RENDER_CROPS];
	int						currentRenderCrop;
	
	// GUI drawing variables for surface creation
	int						guiRecursionLevel;		// to prevent infinite overruns
	uint32					currentColorNativeBytesOrder;
	uint64					currentGLState;
	class budGuiModel* 		guiModel;
	
	List<budFont*, TAG_FONT>		fonts;
	
	unsigned short			gammaTable[256];	// brightness / gamma modify this
	
	srfTriangles_t* 		unitSquareTriangles;
	srfTriangles_t* 		zeroOneCubeTriangles;
	srfTriangles_t* 		testImageTriangles;
	
	// these are allocated at buffer swap time, but
	// the back end should only use the ones in the backEnd stucture,
	// which are copied over from the frame that was just swapped.
	drawSurf_t				unitSquareSurface_;
	drawSurf_t				zeroOneCubeSurface_;
	drawSurf_t				testImageSurface_;
	
	budParallelJobList* 		frontEndJobList;
	
	budRenderBackend			backend;
	
	unsigned				timerQueryId;		// for GL_TIME_ELAPSED_EXT queries
	
private:
	bool					bInitialized;
};

extern budRenderSystemLocal	tr;
extern glconfig_t			glConfig;		// outside of TR since it shouldn't be cleared during ref re-init

//
// cvars
//
extern CVar r_windowX;
extern CVar r_windowY;
extern CVar r_windowWidth;
extern CVar r_windowHeight;

extern CVar r_debugContext;				// enable various levels of context debug
extern CVar r_glDriver;					// "opengl32", etc
extern CVar r_skipIntelWorkarounds;		// skip work arounds for Intel driver bugs
extern CVar r_vidMode;					// video mode number
extern CVar r_displayRefresh;				// optional display refresh rate option for vid mode
extern CVar r_fullscreen;					// 0 = windowed, 1 = full screen
extern CVar r_antiAliasing;				// anti aliasing mode, SMAA, TXAA, MSAA etc.

extern CVar r_znear;						// near Z clip plane

extern CVar r_swapInterval;				// changes wglSwapIntarval
extern CVar r_offsetFactor;				// polygon offset parameter
extern CVar r_offsetUnits;				// polygon offset parameter
extern CVar r_singleTriangle;				// only draw a single triangle per primitive
extern CVar r_logFile;					// number of frames to emit GL logs
extern CVar r_clear;						// force screen clear every frame
extern CVar r_subviewOnly;				// 1 = don't render main view, allowing subviews to be debugged
extern CVar r_lightScale;					// all light intensities are multiplied by this, which is normally 3
extern CVar r_flareSize;					// scale the flare deforms from the material def

extern CVar r_gamma;						// changes gamma tables
extern CVar r_brightness;					// changes gamma tables

extern CVar r_checkBounds;				// compare all surface bounds with precalculated ones
extern CVar r_maxAnisotropicFiltering;	// texture filtering parameter
extern CVar r_useTrilinearFiltering;		// Extra quality filtering
extern CVar r_lodBias;					// lod bias

extern CVar r_useLightPortalFlow;			// 1 = do a more precise area reference determination
extern CVar r_useShadowSurfaceScissor;	// 1 = scissor shadows by the scissor rect of the interaction surfaces
extern CVar r_useConstantMaterials;		// 1 = use pre-calculated material registers if possible
extern CVar r_useNodeCommonChildren;		// stop pushing reference bounds early when possible
extern CVar r_useSilRemap;				// 1 = consider verts with the same XYZ, but different ST the same for shadows
extern CVar r_useLightPortalCulling;		// 0 = none, 1 = box, 2 = exact clip of polyhedron faces, 3 MVP to plane culling
extern CVar r_useLightAreaCulling;		// 0 = off, 1 = on
extern CVar r_useLightScissors;			// 1 = use custom scissor rectangle for each light
extern CVar r_useEntityPortalCulling;		// 0 = none, 1 = box
extern CVar r_skipPrelightShadows;		// 1 = skip the dmap generated static shadow volumes
extern CVar r_useCachedDynamicModels;		// 1 = cache snapshots of dynamic models
extern CVar r_useScissor;					// 1 = scissor clip as portals and lights are processed
extern CVar r_usePortals;					// 1 = use portals to perform area culling, otherwise draw everything
extern CVar r_useStateCaching;			// avoid redundant state changes in GL_*() calls
extern CVar r_useEntityCallbacks;			// if 0, issue the callback immediately at update time, rather than defering
extern CVar r_lightAllBackFaces;			// light all the back faces, even when they would be shadowed
extern CVar r_useLightDepthBounds;		// use depth bounds test on lights to reduce both shadow and interaction fill
extern CVar r_useShadowDepthBounds;		// use depth bounds test on individual shadows to reduce shadow fill
// RB begin
extern CVar r_useShadowMapping;			// use shadow mapping instead of stencil shadows
extern CVar r_useHalfLambertLighting;		// use Half-Lambert lighting instead of classic Lambert
extern CVar r_useHDR;
extern CVar r_useSRGB;
extern CVar r_useSeamlessCubeMap;
// RB end

extern CVar r_skipStaticInteractions;		// skip interactions created at level load
extern CVar r_skipDynamicInteractions;	// skip interactions created after level load
extern CVar r_skipPostProcess;			// skip all post-process renderings
extern CVar r_skipSuppress;				// ignore the per-view suppressions
extern CVar r_skipInteractions;			// skip all light/surface interaction drawing
extern CVar r_skipFrontEnd;				// bypasses all front end work, but 2D gui rendering still draws
extern CVar r_skipBackEnd;				// don't draw anything
extern CVar r_skipCopyTexture;			// do all rendering, but don't actually copyTexSubImage2D
extern CVar r_skipRender;					// skip 3D rendering, but pass 2D
extern CVar r_skipRenderContext;			// NULL the rendering context during backend 3D rendering
extern CVar r_skipTranslucent;			// skip the translucent interaction rendering
extern CVar r_skipAmbient;				// bypasses all non-interaction drawing
extern CVar r_skipNewAmbient;				// bypasses all vertex/fragment program ambients
extern CVar r_skipBlendLights;			// skip all blend lights
extern CVar r_skipFogLights;				// skip all fog lights
extern CVar r_skipSubviews;				// 1 = don't render any mirrors / cameras / etc
extern CVar r_skipGuiShaders;				// 1 = don't render any gui elements on surfaces
extern CVar r_skipParticles;				// 1 = don't render any particles
extern CVar r_skipUpdates;				// 1 = don't accept any entity or light updates, making everything static
extern CVar r_skipDeforms;				// leave all deform materials in their original state
extern CVar r_skipDynamicTextures;		// don't dynamically create textures
extern CVar r_skipBump;					// uses a flat surface instead of the bump map
extern CVar r_skipSpecular;				// use black for specular
extern CVar r_skipDiffuse;				// use black for diffuse
extern CVar r_skipDecals;					// skip decal surfaces
extern CVar r_skipOverlays;				// skip overlay surfaces
extern CVar r_skipShadows;				// disable shadows

extern CVar r_ignoreGLErrors;

extern CVar r_screenFraction;				// for testing fill rate, the resolution of the entire screen can be changed
extern CVar r_showUnsmoothedTangents;		// highlight geometry rendered with unsmoothed tangents
extern CVar r_showSilhouette;				// highlight edges that are casting shadow planes
extern CVar r_showVertexColor;			// draws all triangles with the solid vertex color
extern CVar r_showUpdates;				// report entity and light updates and ref counts
extern CVar r_showDemo;					// report reads and writes to the demo file
extern CVar r_showDynamic;				// report stats on dynamic surface generation
extern CVar r_showIntensity;				// draw the screen colors based on intensity, red = 0, green = 128, blue = 255
extern CVar r_showTrace;					// show the intersection of an eye trace with the world
extern CVar r_showDepth;					// display the contents of the depth buffer and the depth range
extern CVar r_showTris;					// enables wireframe rendering of the world
extern CVar r_showSurfaceInfo;			// show surface material name under crosshair
extern CVar r_showNormals;				// draws wireframe normals
extern CVar r_showEdges;					// draw the sil edges
extern CVar r_showViewEntitys;			// displays the bounding boxes of all view models and optionally the index
extern CVar r_showTexturePolarity;		// shade triangles by texture area polarity
extern CVar r_showTangentSpace;			// shade triangles by tangent space
extern CVar r_showDominantTri;			// draw lines from vertexes to center of dominant triangles
extern CVar r_showTextureVectors;			// draw each triangles texture (tangent) vectors
extern CVar r_showLights;					// 1 = print light info, 2 = also draw volumes
extern CVar r_showLightCount;				// colors surfaces based on light count
extern CVar r_showShadows;				// visualize the stencil shadow volumes
extern CVar r_showLightScissors;			// show light scissor rectangles
extern CVar r_showMemory;					// print frame memory utilization
extern CVar r_showCull;					// report sphere and box culling stats
extern CVar r_showAddModel;				// report stats from tr_addModel
extern CVar r_showSurfaces;				// report surface/light/shadow counts
extern CVar r_showPrimitives;				// report vertex/index/draw counts
extern CVar r_showPortals;				// draw portal outlines in color based on passed / not passed
extern CVar r_showSkel;					// draw the skeleton when model animates
extern CVar r_showOverDraw;				// show overdraw
// RB begin
extern CVar r_showShadowMaps;
extern CVar r_showShadowMapLODs;
// RB end
extern CVar r_jointNameScale;				// size of joint names when r_showskel is set to 1
extern CVar r_jointNameOffset;			// offset of joint names when r_showskel is set to 1

extern CVar r_testGamma;					// draw a grid pattern to test gamma levels
extern CVar r_testGammaBias;				// draw a grid pattern to test gamma levels

extern CVar r_singleLight;				// suppress all but one light
extern CVar r_singleEntity;				// suppress all but one entity
extern CVar r_singleArea;					// only draw the portal area the view is actually in
extern CVar r_singleSurface;				// suppress all but one surface on each entity
extern CVar r_shadowPolygonOffset;		// bias value added to depth test for stencil shadow drawing
extern CVar r_shadowPolygonFactor;		// scale value for stencil shadow drawing

extern CVar r_jitter;						// randomly subpixel jitter the projection matrix
extern CVar r_orderIndexes;				// perform index reorganization to optimize vertex use

extern CVar r_debugLineDepthTest;			// perform depth test on debug lines
extern CVar r_debugLineWidth;				// width of debug lines
extern CVar r_debugArrowStep;				// step size of arrow cone line rotation in degrees
extern CVar r_debugPolygonFilled;

extern CVar r_materialOverride;			// override all materials

extern CVar r_debugRenderToTexture;

extern CVar stereoRender_enable;
extern CVar stereoRender_deGhost;			// subtract from opposite eye to reduce ghosting

extern CVar r_useGPUSkinning;

// RB begin
extern CVar r_shadowMapFrustumFOV;
extern CVar r_shadowMapSingleSide;
extern CVar r_shadowMapImageSize;
extern CVar r_shadowMapJitterScale;
extern CVar r_shadowMapBiasScale;
extern CVar r_shadowMapRandomizeJitter;
extern CVar r_shadowMapSamples;
extern CVar r_shadowMapSplits;
extern CVar r_shadowMapSplitWeight;
extern CVar r_shadowMapLodScale;
extern CVar r_shadowMapLodBias;
extern CVar r_shadowMapPolygonFactor;
extern CVar r_shadowMapPolygonOffset;
extern CVar r_shadowMapOccluderFacing;
extern CVar r_shadowMapRegularDepthBiasScale;
extern CVar r_shadowMapSunDepthBiasScale;

extern CVar r_hdrAutoExposure;
extern CVar r_hdrMinLuminance;
extern CVar r_hdrMaxLuminance;
extern CVar r_hdrKey;
extern CVar r_hdrContrastDynamicThreshold;
extern CVar r_hdrContrastStaticThreshold;
extern CVar r_hdrContrastOffset;
extern CVar r_hdrGlarePasses;
extern CVar r_hdrDebug;

extern CVar r_ldrContrastThreshold;
extern CVar r_ldrContrastOffset;

extern CVar r_useFilmicPostProcessEffects;
extern CVar r_forceAmbient;

extern CVar r_useSSGI;
extern CVar r_ssgiDebug;
extern CVar r_ssgiFiltering;

extern CVar r_useSSAO;
extern CVar r_ssaoDebug;
extern CVar r_ssaoFiltering;
extern CVar r_useHierarchicalDepthBuffer;

extern CVar r_exposure;
// RB end

/*
====================================================================

INITIALIZATION

====================================================================
*/

void R_SetNewMode( const bool fullInit );

void R_SetColorMappings();

void R_ScreenShot_f( const CmdArgs& args );

/*
====================================================================

IMPLEMENTATION SPECIFIC FUNCTIONS

====================================================================
*/

struct vidMode_t
{
	int width;
	int height;
	int displayHz;
	
	// RB begin
	vidMode_t()
	{
		width = 640;
		height = 480;
		displayHz = 60;
	}
	
	vidMode_t( int width, int height, int displayHz ) :
		width( width ), height( height ), displayHz( displayHz ) {}
	// RB end
	
	bool operator==( const vidMode_t& a )
	{
		return a.width == width && a.height == height && a.displayHz == displayHz;
	}
};

// the number of displays can be found by itterating this until it returns false
// displayNum is the 0 based value passed to EnumDisplayDevices(), you must add
// 1 to this to get an r_fullScreen value.
bool R_GetModeListForDisplay( const int displayNum, List<vidMode_t>& modeList );

struct glimpParms_t
{
	int			x;				// ignored in fullscreen
	int			y;				// ignored in fullscreen
	int			width;
	int			height;
	int			fullScreen;		// 0 = windowed, otherwise 1 based monitor number to go full screen on
	// -1 = borderless window for spanning multiple displays
	bool		stereo;
	int			displayHz;
	int			multiSamples;
};

// DG: R_GetModeListForDisplay is called before GLimp_Init(), but SDL needs SDL_Init() first.
// So add PreInit for platforms that need it, others can just stub it.
void		GLimp_PreInit();

// If the desired mode can't be set satisfactorily, false will be returned.
// If succesful, sets glConfig.nativeScreenWidth, glConfig.nativeScreenHeight, and glConfig.pixelAspect
// The renderer will then reset the glimpParms to "safe mode" of 640x480
// fullscreen and try again.  If that also fails, the error will be fatal.
bool		GLimp_Init( glimpParms_t parms );

// will set up gl up with the new parms
bool		GLimp_SetScreenParms( glimpParms_t parms );

// Destroys the rendering context, closes the window, resets the resolution,
// and resets the gamma ramps.
void		GLimp_Shutdown();

// Sets the hardware gamma ramps for gamma and brightness adjustment.
// These are now taken as 16 bit values, so we can take full advantage
// of dacs with >8 bits of precision
void		GLimp_SetGamma( unsigned short red[256],
							unsigned short green[256],
							unsigned short blue[256] );



/*
============================================================

RENDERWORLD_DEFS

============================================================
*/

void R_DeriveEntityData( budRenderEntityLocal* def );
void R_CreateEntityRefs( budRenderEntityLocal* def );
void R_FreeEntityDefDerivedData( budRenderEntityLocal* def, bool keepDecals, bool keepCachedDynamicModel );
void R_FreeEntityDefCachedDynamicModel( budRenderEntityLocal* def );
void R_FreeEntityDefDecals( budRenderEntityLocal* def );
void R_FreeEntityDefOverlay( budRenderEntityLocal* def );
void R_FreeEntityDefFadedDecals( budRenderEntityLocal* def, int time );

// RB: for dmap
void R_DeriveLightData( budRenderLightLocal* light );

// Called by the editor and dmap to operate on light volumes
void R_RenderLightFrustum( const renderLight_t& renderLight, budPlane lightFrustum[6] );

srfTriangles_t* R_PolytopeSurface( int numPlanes, const budPlane* planes, idWinding** windings );
// RB end
void R_CreateLightRefs( budRenderLightLocal* light );
void R_FreeLightDefDerivedData( budRenderLightLocal* light );

void R_FreeDerivedData();
void R_ReCreateWorldReferences();
void R_CheckForEntityDefsUsingModel( budRenderModel* model );
void R_ModulateLights_f( const CmdArgs& args );

/*
============================================================

RENDERWORLD_PORTALS

============================================================
*/

viewEntity_t* R_SetEntityDefViewEntity( budRenderEntityLocal* def );
viewLight_t* R_SetLightDefViewLight( budRenderLightLocal* def );

/*
====================================================================

TR_FRONTEND_MAIN

====================================================================
*/

void R_InitFrameData();
void R_ShutdownFrameData();
void R_ToggleSmpFrame();
void* R_FrameAlloc( int bytes, frameAllocType_t type = FRAME_ALLOC_UNKNOWN );
void* R_ClearedFrameAlloc( int bytes, frameAllocType_t type = FRAME_ALLOC_UNKNOWN );

void* R_StaticAlloc( int bytes, const memTag_t tag = TAG_RENDER_STATIC );		// just malloc with error checking
void* R_ClearedStaticAlloc( int bytes );	// with memset
void R_StaticFree( void* data );

void R_RenderView( viewDef_t* parms );
void R_RenderPostProcess( viewDef_t* parms );

/*
============================================================

TR_FRONTEND_ADDLIGHTS

============================================================
*/

void R_ShadowBounds( const budBounds& modelBounds, const budBounds& lightBounds, const Vector3& lightOrigin, budBounds& shadowBounds );

BUD_INLINE bool R_CullModelBoundsToLight( const budRenderLightLocal* light, const budBounds& localBounds, const budRenderMatrix& modelRenderMatrix )
{
	budRenderMatrix modelLightProject;
	budRenderMatrix::Multiply( light->baseLightProject, modelRenderMatrix, modelLightProject );
	return budRenderMatrix::CullBoundsToMVP( modelLightProject, localBounds, true );
}

void R_AddLights();
void R_OptimizeViewLightsList();

/*
============================================================

TR_FRONTEND_ADDMODELS

============================================================
*/

bool R_IssueEntityDefCallback( budRenderEntityLocal* def );
budRenderModel* R_EntityDefDynamicModel( budRenderEntityLocal* def );
void R_ClearEntityDefDynamicModel( budRenderEntityLocal* def );

void R_SetupDrawSurfShader( drawSurf_t* drawSurf, const budMaterial* shader, const renderEntity_t* renderEntity );
void R_SetupDrawSurfJoints( drawSurf_t* drawSurf, const srfTriangles_t* tri, const budMaterial* shader );
void R_LinkDrawSurfToView( drawSurf_t* drawSurf, viewDef_t* viewDef );

void R_AddModels();

/*
=============================================================

TR_FRONTEND_DEFORM

=============================================================
*/

drawSurf_t* R_DeformDrawSurf( drawSurf_t* drawSurf );

/*
=============================================================

TR_FRONTEND_GUISURF

=============================================================
*/

void R_SurfaceToTextureAxis( const srfTriangles_t* tri, Vector3& origin, Vector3 axis[3] );
void R_AddInGameGuis( const drawSurf_t* const drawSurfs[], const int numDrawSurfs );

/*
============================================================

TR_FRONTEND_SUBVIEW

============================================================
*/

bool R_PreciseCullSurface( const drawSurf_t* drawSurf, budBounds& ndcBounds );
bool R_GenerateSubViews( const drawSurf_t* const drawSurfs[], const int numDrawSurfs );

/*
============================================================

TR_TRISURF

============================================================
*/

srfTriangles_t* 	R_AllocStaticTriSurf();
void				R_AllocStaticTriSurfVerts( srfTriangles_t* tri, int numVerts );
void				R_AllocStaticTriSurfIndexes( srfTriangles_t* tri, int numIndexes );
void				R_AllocStaticTriSurfPreLightShadowVerts( srfTriangles_t* tri, int numVerts );
void				R_AllocStaticTriSurfSilIndexes( srfTriangles_t* tri, int numIndexes );
void				R_AllocStaticTriSurfDominantTris( srfTriangles_t* tri, int numVerts );
void				R_AllocStaticTriSurfSilEdges( srfTriangles_t* tri, int numSilEdges );
void				R_AllocStaticTriSurfMirroredVerts( srfTriangles_t* tri, int numMirroredVerts );
void				R_AllocStaticTriSurfDupVerts( srfTriangles_t* tri, int numDupVerts );

srfTriangles_t* 	R_CopyStaticTriSurf( const srfTriangles_t* tri );

void				R_ResizeStaticTriSurfVerts( srfTriangles_t* tri, int numVerts );
void				R_ResizeStaticTriSurfIndexes( srfTriangles_t* tri, int numIndexes );
void				R_ReferenceStaticTriSurfVerts( srfTriangles_t* tri, const srfTriangles_t* reference );
void				R_ReferenceStaticTriSurfIndexes( srfTriangles_t* tri, const srfTriangles_t* reference );

void				R_FreeStaticTriSurfSilIndexes( srfTriangles_t* tri );
void				R_FreeStaticTriSurf( srfTriangles_t* tri );
void				R_FreeStaticTriSurfVerts( srfTriangles_t* tri );
void				R_FreeStaticTriSurfVertexCaches( srfTriangles_t* tri );
int					R_TriSurfMemory( const srfTriangles_t* tri );

void				R_BoundTriSurf( srfTriangles_t* tri );
void				R_RemoveDuplicatedTriangles( srfTriangles_t* tri );
void				R_CreateSilIndexes( srfTriangles_t* tri );
void				R_RemoveDegenerateTriangles( srfTriangles_t* tri );
void				R_RemoveUnusedVerts( srfTriangles_t* tri );
void				R_RangeCheckIndexes( const srfTriangles_t* tri );
void				R_CreateVertexNormals( srfTriangles_t* tri );		// also called by dmap
void				R_CleanupTriangles( srfTriangles_t* tri, bool createNormals, bool identifySilEdges, bool useUnsmoothedTangents );
void				R_ReverseTriangles( srfTriangles_t* tri );

// Only deals with vertexes and indexes, not silhouettes, planes, etc.
// Does NOT perform a cleanup triangles, so there may be duplicated verts in the result.
srfTriangles_t* 	R_MergeSurfaceList( const srfTriangles_t** surfaces, int numSurfaces );
srfTriangles_t* 	R_MergeTriangles( const srfTriangles_t* tri1, const srfTriangles_t* tri2 );

// if the deformed verts have significant enough texture coordinate changes to reverse the texture
// polarity of a triangle, the tangents will be incorrect
void				R_DeriveTangents( srfTriangles_t* tri );

// copy data from a front-end srfTriangles_t to a back-end drawSurf_t
void				R_InitDrawSurfFromTri( drawSurf_t& ds, srfTriangles_t& tri );

// For static surfaces, the indexes, ambient, and shadow buffers can be pre-created at load
// time, rather than being re-created each frame in the frame temporary buffers.
void				R_CreateStaticBuffersForTri( srfTriangles_t& tri );

// deformable meshes precalculate as much as possible from a base frame, then generate
// complete srfTriangles_t from just a new set of vertexes
struct deformInfo_t
{
	int					numSourceVerts;
	
	// numOutputVerts may be smaller if the input had duplicated or degenerate triangles
	// it will often be larger if the input had mirrored texture seams that needed
	// to be busted for proper tangent spaces
	int					numOutputVerts;
	budDrawVert* 		verts;
	
	int					numIndexes;
	triIndex_t* 		indexes;
	
	triIndex_t* 		silIndexes;				// indexes changed to be the first vertex with same XYZ, ignoring normal and texcoords
	
	int					numMirroredVerts;		// this many verts at the end of the vert list are tangent mirrors
	int* 				mirroredVerts;			// tri->mirroredVerts[0] is the mirror of tri->numVerts - tri->numMirroredVerts + 0
	
	int					numDupVerts;			// number of duplicate vertexes
	int* 				dupVerts;				// pairs of the number of the first vertex and the number of the duplicate vertex
	
	int					numSilEdges;			// number of silhouette edges
	silEdge_t* 			silEdges;				// silhouette edges
	
	vertCacheHandle_t	staticIndexCache;		// GL_INDEX_TYPE
	vertCacheHandle_t	staticAmbientCache;		// budDrawVert
	vertCacheHandle_t	staticShadowCache;		// idShadowCacheSkinned
};


// if outputVertexes is not NULL, it will point to a newly allocated set of verts that includes the mirrored ones
deformInfo_t* 		R_BuildDeformInfo( int numVerts, const budDrawVert* verts, int numIndexes, const int* indexes,
									   bool useUnsmoothedTangents );
void				R_FreeDeformInfo( deformInfo_t* deformInfo );
int					R_DeformInfoMemoryUsed( deformInfo_t* deformInfo );

/*
=============================================================

TR_TRACE

=============================================================
*/

struct localTrace_t
{
	float		fraction;
	// only valid if fraction < 1.0
	Vector3		point;
	Vector3		normal;
	int			indexes[3];
};

localTrace_t R_LocalTrace( const Vector3& start, const Vector3& end, const float radius, const srfTriangles_t* tri );



/*
============================================================

TR_BACKEND_DRAW

============================================================
*/

void RB_SetMVP( const budRenderMatrix& mvp );

/*
=============================================================

TR_BACKEND_RENDERTOOLS

=============================================================
*/

float RB_DrawTextLength( const char* text, float scale, int len );
void RB_AddDebugText( const char* text, const Vector3& origin, float scale, const Vector4& color, const Matrix3& viewAxis, const int align, const int lifetime, const bool depthTest );
void RB_ClearDebugText( int time );
void RB_AddDebugLine( const Vector4& color, const Vector3& start, const Vector3& end, const int lifeTime, const bool depthTest );
void RB_ClearDebugLines( int time );
void RB_AddDebugPolygon( const Vector4& color, const idWinding& winding, const int lifeTime, const bool depthTest );
void RB_ClearDebugPolygons( int time );
void RB_DrawBounds( const budBounds& bounds );

void RB_ShutdownDebugTools();
void RB_SetVertexColorParms( stageVertexColor_t svc );

//=============================================

#include "ResolutionScale.h"
#include "RenderLog.h"
#include "jobs/ShadowShared.h"
#include "jobs/prelightshadowvolume/PreLightShadowVolume.h"
#include "jobs/staticshadowvolume/StaticShadowVolume.h"
#include "jobs/dynamicshadowvolume/DynamicShadowVolume.h"
#include "GLMatrix.h"



#include "BufferObject.h"
#include "RenderProgs.h"
#include "RenderWorld_local.h"
#include "GuiModel.h"
#include "VertexCache.h"

#endif /* !__TR_LOCAL_H__ */
