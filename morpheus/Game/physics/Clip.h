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

#ifndef __CLIP_H__
#define __CLIP_H__

/*
===============================================================================

  Handles collision detection with the world and between physics objects.

===============================================================================
*/

#define CLIPMODEL_ID_TO_JOINT_HANDLE( id )	( ( id ) >= 0 ? INVALID_JOINT : ((jointHandle_t) ( -1 - id )) )
#define JOINT_HANDLE_TO_CLIPMODEL_ID( id )	( -1 - id )

class idClip;
class budClipModel;
class idEntity;

//===============================================================
//
//	budClipModel
//
//===============================================================

class budClipModel
{

	friend class idClip;
	
public:
	budClipModel();
	explicit budClipModel( const char* name );
	explicit budClipModel( const budTraceModel& trm );
	explicit budClipModel( const budTraceModel& trm, bool persistantThroughSave );
	explicit budClipModel( const int renderModelHandle );
	explicit budClipModel( const budClipModel* model );
	~budClipModel();
	
	bool					LoadModel( const char* name );
	void					LoadModel( const budTraceModel& trm, bool persistantThroughSave = true );
	void					LoadModel( const int renderModelHandle );
	
	void					Save( idSaveGame* savefile ) const;
	void					Restore( idRestoreGame* savefile );
	
	void					Link( idClip& clp );				// must have been linked with an entity and id before
	void					Link( idClip& clp, idEntity* ent, int newId, const budVec3& newOrigin, const budMat3& newAxis, int renderModelHandle = -1 );
	void					Unlink();						// unlink from sectors
	void					SetPosition( const budVec3& newOrigin, const budMat3& newAxis );	// unlinks the clip model
	void					Translate( const budVec3& translation );							// unlinks the clip model
	void					Rotate( const budRotation& rotation );							// unlinks the clip model
	void					Enable();						// enable for clipping
	void					Disable();					// keep linked but disable for clipping
	void					SetMaterial( const budMaterial* m );
	const budMaterial* 		GetMaterial() const;
	void					SetContents( int newContents );		// override contents
	int						GetContents() const;
	void					SetEntity( idEntity* newEntity );
	idEntity* 				GetEntity() const;
	void					SetId( int newId );
	int						GetId() const;
	void					SetOwner( idEntity* newOwner );
	idEntity* 				GetOwner() const;
	const budBounds& 		GetBounds() const;
	const budBounds& 		GetAbsBounds() const;
	const budVec3& 			GetOrigin() const;
	const budMat3& 			GetAxis() const;
	bool					IsTraceModel() const;			// returns true if this is a trace model
	bool					IsRenderModel() const;		// returns true if this is a render model
	bool					IsLinked() const;				// returns true if the clip model is linked
	bool					IsEnabled() const;			// returns true if enabled for collision detection
	bool					IsEqual( const budTraceModel& trm ) const;
	cmHandle_t				Handle() const;				// returns handle used to collide vs this model
	const budTraceModel* 	GetTraceModel() const;
	void					GetMassProperties( const float density, float& mass, budVec3& centerOfMass, budMat3& inertiaTensor ) const;
	
	static cmHandle_t		CheckModel( const char* name );
	static void				ClearTraceModelCache();
	static int				TraceModelCacheSize();
	
	static void				SaveTraceModels( idSaveGame* savefile );
	static void				RestoreTraceModels( idRestoreGame* savefile );
	
private:
	bool					enabled;				// true if this clip model is used for clipping
	idEntity* 				entity;					// entity using this clip model
	int						id;						// id for entities that use multiple clip models
	idEntity* 				owner;					// owner of the entity that owns this clip model
	budVec3					origin;					// origin of clip model
	budMat3					axis;					// orientation of clip model
	budBounds				bounds;					// bounds
	budBounds				absBounds;				// absolute bounds
	const budMaterial* 		material;				// material for trace models
	int						contents;				// all contents ored together
	cmHandle_t				collisionModelHandle;	// handle to collision model
	int						traceModelIndex;		// trace model used for collision detection
	int						renderModelHandle;		// render model def handle
	
	struct clipLink_s* 		clipLinks;				// links into sectors
	int						touchCount;
	
	void					Init();			// initialize
	void					Link_r( struct clipSector_s* node );
	
	static int				AllocTraceModel( const budTraceModel& trm, bool persistantThroughSaves = true );
	static void				FreeTraceModel( int traceModelIndex );
	static budTraceModel* 	GetCachedTraceModel( int traceModelIndex );
	static int				GetTraceModelHashKey( const budTraceModel& trm );
	static struct trmCache_s* 		GetTraceModelEntry( int traceModelIndex );
};


BUD_INLINE void budClipModel::Translate( const budVec3& translation )
{
	Unlink();
	origin += translation;
}

BUD_INLINE void budClipModel::Rotate( const budRotation& rotation )
{
	Unlink();
	origin *= rotation;
	axis *= rotation.ToMat3();
}

BUD_INLINE void budClipModel::Enable()
{
	enabled = true;
}

BUD_INLINE void budClipModel::Disable()
{
	enabled = false;
}

BUD_INLINE void budClipModel::SetMaterial( const budMaterial* m )
{
	material = m;
}

BUD_INLINE const budMaterial* budClipModel::GetMaterial() const
{
	return material;
}

BUD_INLINE void budClipModel::SetContents( int newContents )
{
	contents = newContents;
}

BUD_INLINE int budClipModel::GetContents() const
{
	return contents;
}

BUD_INLINE void budClipModel::SetEntity( idEntity* newEntity )
{
	entity = newEntity;
}

BUD_INLINE idEntity* budClipModel::GetEntity() const
{
	return entity;
}

BUD_INLINE void budClipModel::SetId( int newId )
{
	id = newId;
}

BUD_INLINE int budClipModel::GetId() const
{
	return id;
}

BUD_INLINE void budClipModel::SetOwner( idEntity* newOwner )
{
	owner = newOwner;
}

BUD_INLINE idEntity* budClipModel::GetOwner() const
{
	return owner;
}

BUD_INLINE const budBounds& budClipModel::GetBounds() const
{
	return bounds;
}

BUD_INLINE const budBounds& budClipModel::GetAbsBounds() const
{
	return absBounds;
}

BUD_INLINE const budVec3& budClipModel::GetOrigin() const
{
	return origin;
}

BUD_INLINE const budMat3& budClipModel::GetAxis() const
{
	return axis;
}

BUD_INLINE bool budClipModel::IsRenderModel() const
{
	return ( renderModelHandle != -1 );
}

BUD_INLINE bool budClipModel::IsTraceModel() const
{
	return ( traceModelIndex != -1 );
}

BUD_INLINE bool budClipModel::IsLinked() const
{
	return ( clipLinks != NULL );
}

BUD_INLINE bool budClipModel::IsEnabled() const
{
	return enabled;
}

BUD_INLINE bool budClipModel::IsEqual( const budTraceModel& trm ) const
{
	return ( traceModelIndex != -1 && *GetCachedTraceModel( traceModelIndex ) == trm );
}

BUD_INLINE const budTraceModel* budClipModel::GetTraceModel() const
{
	if( !IsTraceModel() )
	{
		return NULL;
	}
	return budClipModel::GetCachedTraceModel( traceModelIndex );
}


//===============================================================
//
//	idClip
//
//===============================================================

class idClip
{

	friend class budClipModel;
	
public:
	idClip();
	
	void					Init();
	void					Shutdown();
	
	// clip versus the rest of the world
	bool					Translation( trace_t& results, const budVec3& start, const budVec3& end,
										 const budClipModel* mdl, const budMat3& trmAxis, int contentMask, const idEntity* passEntity );
	bool					Rotation( trace_t& results, const budVec3& start, const budRotation& rotation,
									  const budClipModel* mdl, const budMat3& trmAxis, int contentMask, const idEntity* passEntity );
	bool					Motion( trace_t& results, const budVec3& start, const budVec3& end, const budRotation& rotation,
									const budClipModel* mdl, const budMat3& trmAxis, int contentMask, const idEntity* passEntity );
	int						Contacts( contactInfo_t* contacts, const int maxContacts, const budVec3& start, const budVec6& dir, const float depth,
									  const budClipModel* mdl, const budMat3& trmAxis, int contentMask, const idEntity* passEntity );
	int						Contents( const budVec3& start,
									  const budClipModel* mdl, const budMat3& trmAxis, int contentMask, const idEntity* passEntity );
									  
	// special case translations versus the rest of the world
	bool					TracePoint( trace_t& results, const budVec3& start, const budVec3& end,
										int contentMask, const idEntity* passEntity );
	bool					TraceBounds( trace_t& results, const budVec3& start, const budVec3& end, const budBounds& bounds,
										 int contentMask, const idEntity* passEntity );
										 
	// clip versus a specific model
	void					TranslationModel( trace_t& results, const budVec3& start, const budVec3& end,
			const budClipModel* mdl, const budMat3& trmAxis, int contentMask,
			cmHandle_t model, const budVec3& modelOrigin, const budMat3& modelAxis );
	void					RotationModel( trace_t& results, const budVec3& start, const budRotation& rotation,
										   const budClipModel* mdl, const budMat3& trmAxis, int contentMask,
										   cmHandle_t model, const budVec3& modelOrigin, const budMat3& modelAxis );
	int						ContactsModel( contactInfo_t* contacts, const int maxContacts, const budVec3& start, const budVec6& dir, const float depth,
										   const budClipModel* mdl, const budMat3& trmAxis, int contentMask,
										   cmHandle_t model, const budVec3& modelOrigin, const budMat3& modelAxis );
	int						ContentsModel( const budVec3& start,
										   const budClipModel* mdl, const budMat3& trmAxis, int contentMask,
										   cmHandle_t model, const budVec3& modelOrigin, const budMat3& modelAxis );
										   
	// clip versus all entities but not the world
	void					TranslationEntities( trace_t& results, const budVec3& start, const budVec3& end,
			const budClipModel* mdl, const budMat3& trmAxis, int contentMask, const idEntity* passEntity );
			
	// get a contact feature
	bool					GetModelContactFeature( const contactInfo_t& contact, const budClipModel* clipModel, budFixedWinding& winding ) const;
	
	// get entities/clip models within or touching the given bounds
	int						EntitiesTouchingBounds( const budBounds& bounds, int contentMask, idEntity** entityList, int maxCount ) const;
	int						ClipModelsTouchingBounds( const budBounds& bounds, int contentMask, budClipModel** clipModelList, int maxCount ) const;
	
	const budBounds& 		GetWorldBounds() const;
	budClipModel* 			DefaultClipModel();
	
	// stats and debug drawing
	void					PrintStatistics();
	void					DrawClipModels( const budVec3& eye, const float radius, const idEntity* passEntity );
	bool					DrawModelContactFeature( const contactInfo_t& contact, const budClipModel* clipModel, int lifetime ) const;
	
private:
	int						numClipSectors;
	struct clipSector_s* 	clipSectors;
	budBounds				worldBounds;
	budClipModel				temporaryClipModel;
	budClipModel				defaultClipModel;
	mutable int				touchCount;
	// statistics
	int						numTranslations;
	int						numRotations;
	int						numMotions;
	int						numRenderModelTraces;
	int						numContents;
	int						numContacts;
	
private:
	struct clipSector_s* 	CreateClipSectors_r( const int depth, const budBounds& bounds, budVec3& maxSector );
	void					ClipModelsTouchingBounds_r( const struct clipSector_s* node, struct listParms_s& parms ) const;
	const budTraceModel* 	TraceModelForClipModel( const budClipModel* mdl ) const;
	int						GetTraceClipModels( const budBounds& bounds, int contentMask, const idEntity* passEntity, budClipModel** clipModelList ) const;
	void					TraceRenderModel( trace_t& trace, const budVec3& start, const budVec3& end, const float radius, const budMat3& axis, budClipModel* touch ) const;
};


BUD_INLINE bool idClip::TracePoint( trace_t& results, const budVec3& start, const budVec3& end, int contentMask, const idEntity* passEntity )
{
	Translation( results, start, end, NULL, mat3_identity, contentMask, passEntity );
	return ( results.fraction < 1.0f );
}

BUD_INLINE bool idClip::TraceBounds( trace_t& results, const budVec3& start, const budVec3& end, const budBounds& bounds, int contentMask, const idEntity* passEntity )
{
	temporaryClipModel.LoadModel( budTraceModel( bounds ) );
	Translation( results, start, end, &temporaryClipModel, mat3_identity, contentMask, passEntity );
	return ( results.fraction < 1.0f );
}

BUD_INLINE const budBounds& idClip::GetWorldBounds() const
{
	return worldBounds;
}

BUD_INLINE budClipModel* idClip::DefaultClipModel()
{
	return &defaultClipModel;
}

#endif /* !__CLIP_H__ */
