/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2013-2015 Robert Beckebans

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

/*
===============================================================================

	Trace model vs. polygonal model collision detection.

===============================================================================
*/

#include "CollisionModel.h"

#define MIN_NODE_SIZE						64.0f
#define MAX_NODE_POLYGONS					128
#define CM_MAX_POLYGON_EDGES				64
#define CIRCLE_APPROXIMATION_LENGTH			64.0f

#define	MAX_SUBMODELS						2048
#define	TRACE_MODEL_HANDLE					MAX_SUBMODELS

#define VERTEX_HASH_BOXSIZE					(1<<6)	// must be power of 2
#define VERTEX_HASH_SIZE					(VERTEX_HASH_BOXSIZE*VERTEX_HASH_BOXSIZE)
#define EDGE_HASH_SIZE						(1<<14)

#define NODE_BLOCK_SIZE_SMALL				8
#define NODE_BLOCK_SIZE_LARGE				256
#define REFERENCE_BLOCK_SIZE_SMALL			8
#define REFERENCE_BLOCK_SIZE_LARGE			256

#define MAX_WINDING_LIST					128		// quite a few are generated at times
#define INTEGRAL_EPSILON					0.01f
#define VERTEX_EPSILON						0.1f
#define CHOP_EPSILON						0.1f


typedef struct cm_windingList_s
{
	int					numWindings;			// number of windings
	budFixedWinding		w[MAX_WINDING_LIST];	// windings
	Vector3				normal;					// normal for all windings
	budBounds			bounds;					// bounds of all windings in list
	Vector3				origin;					// origin for radius
	float				radius;					// radius relative to origin for all windings
	int					contents;				// winding surface contents
	int					primitiveNum;			// number of primitive the windings came from
} cm_windingList_t;

/*
===============================================================================

Collision model

===============================================================================
*/

typedef struct cm_vertex_s
{
	Vector3					p;					// vertex point
	int						checkcount;			// for multi-check avoidance
	// DG: use int instead of long for 64bit compatibility
	unsigned int			side;				// each bit tells at which side this vertex passes one of the trace model edges
	unsigned int			sideSet;			// each bit tells if sidedness for the trace model edge has been calculated yet
	// DG end
} cm_vertex_t;

typedef struct cm_edge_s
{
	int						checkcount;			// for multi-check avoidance
	unsigned short			internal;			// a trace model can never collide with internal edges
	unsigned short			numUsers;			// number of polygons using this edge
	// DG: use int instead of long for 64bit compatibility
	unsigned int			side;				// each bit tells at which side of this edge one of the trace model vertices passes
	unsigned int			sideSet;			// each bit tells if sidedness for the trace model vertex has been calculated yet
	// DG end
	int						vertexNum[2];		// start and end point of edge
	Vector3					normal;				// edge normal
} cm_edge_t;

typedef struct cm_polygonBlock_s
{
	int						bytesRemaining;
	byte* 					next;
} cm_polygonBlock_t;

typedef struct cm_polygon_s
{
	budBounds				bounds;				// polygon bounds
	int						checkcount;			// for multi-check avoidance
	int						contents;			// contents behind polygon
	const budMaterial* 		material;			// material
	budPlane					plane;				// polygon plane
	int						numEdges;			// number of edges
	int						edges[1];			// variable sized, indexes into cm_edge_t list
} cm_polygon_t;

typedef struct cm_polygonRef_s
{
	cm_polygon_t* 			p;					// pointer to polygon
	struct cm_polygonRef_s* next;				// next polygon in chain
} cm_polygonRef_t;

typedef struct cm_polygonRefBlock_s
{
	cm_polygonRef_t* 		nextRef;			// next polygon reference in block
	struct cm_polygonRefBlock_s* next;			// next block with polygon references
} cm_polygonRefBlock_t;

typedef struct cm_brushBlock_s
{
	int						bytesRemaining;
	byte* 					next;
} cm_brushBlock_t;

typedef struct cm_brush_s
{
	cm_brush_s()
	{
		checkcount = 0;
		contents = 0;
		material = NULL;
		primitiveNum = 0;
		numPlanes = 0;
	}
	int						checkcount;			// for multi-check avoidance
	budBounds				bounds;				// brush bounds
	int						contents;			// contents of brush
	const budMaterial* 		material;			// material
	int						primitiveNum;		// number of brush primitive
	int						numPlanes;			// number of bounding planes
	budPlane					planes[1];			// variable sized
} cm_brush_t;

typedef struct cm_brushRef_s
{
	cm_brush_t* 			b;					// pointer to brush
	struct cm_brushRef_s* 	next;				// next brush in chain
} cm_brushRef_t;

typedef struct cm_brushRefBlock_s
{
	cm_brushRef_t* 			nextRef;			// next brush reference in block
	struct cm_brushRefBlock_s* next;			// next block with brush references
} cm_brushRefBlock_t;

typedef struct cm_node_s
{
	int						planeType;			// node axial plane type
	float					planeDist;			// node plane distance
	cm_polygonRef_t* 		polygons;			// polygons in node
	cm_brushRef_t* 			brushes;			// brushes in node
	struct cm_node_s* 		parent;				// parent of this node
	struct cm_node_s* 		children[2];		// node children
} cm_node_t;

typedef struct cm_nodeBlock_s
{
	cm_node_t* 				nextNode;			// next node in block
	struct cm_nodeBlock_s* next;				// next block with nodes
} cm_nodeBlock_t;

typedef struct cm_model_s
{
	String					name;				// model name
	budBounds				bounds;				// model bounds
	int						contents;			// all contents of the model ored together
	bool					isConvex;			// set if model is convex
	// model geometry
	int						maxVertices;		// size of vertex array
	int						numVertices;		// number of vertices
	cm_vertex_t* 			vertices;			// array with all vertices used by the model
	int						maxEdges;			// size of edge array
	int						numEdges;			// number of edges
	cm_edge_t* 				edges;				// array with all edges used by the model
	cm_node_t* 				node;				// first node of spatial subdivision
	// blocks with allocated memory
	cm_nodeBlock_t* 		nodeBlocks;			// list with blocks of nodes
	cm_polygonRefBlock_t* 	polygonRefBlocks;	// list with blocks of polygon references
	cm_brushRefBlock_t* 	brushRefBlocks;		// list with blocks of brush references
	cm_polygonBlock_t* 		polygonBlock;		// memory block with all polygons
	cm_brushBlock_t* 		brushBlock;			// memory block with all brushes
	// statistics
	int						numPolygons;
	int						polygonMemory;
	int						numBrushes;
	int						brushMemory;
	int						numNodes;
	int						numBrushRefs;
	int						numPolygonRefs;
	int						numInternalEdges;
	int						numSharpEdges;
	int						numRemovedPolys;
	int						numMergedPolys;
	int						usedMemory;
} cm_model_t;

/*
===============================================================================

Data used during collision detection calculations

===============================================================================
*/

typedef struct cm_trmVertex_s
{
	int used;										// true if this vertex is used for collision detection
	Vector3 p;										// vertex position
	Vector3 endp;									// end point of vertex after movement
	int polygonSide;								// side of polygon this vertex is on (rotational collision)
	idPluecker pl;									// pluecker coordinate for vertex movement
	Vector3 rotationOrigin;							// rotation origin for this vertex
	budBounds rotationBounds;						// rotation bounds for this vertex
} cm_trmVertex_t;

typedef struct cm_trmEdge_s
{
	int used;										// true when vertex is used for collision detection
	Vector3 start;									// start of edge
	Vector3 end;										// end of edge
	int vertexNum[2];								// indexes into cm_traceWork_t->vertices
	idPluecker pl;									// pluecker coordinate for edge
	Vector3 cross;									// (z,-y,x) of cross product between edge dir and movement dir
	budBounds rotationBounds;						// rotation bounds for this edge
	idPluecker plzaxis;								// pluecker coordinate for rotation about the z-axis
	unsigned short bitNum;							// vertex bit number
} cm_trmEdge_t;

typedef struct cm_trmPolygon_s
{
	int used;
	budPlane plane;									// polygon plane
	int numEdges;									// number of edges
	int edges[MAX_TRACEMODEL_POLYEDGES];			// index into cm_traceWork_t->edges
	budBounds rotationBounds;						// rotation bounds for this polygon
} cm_trmPolygon_t;

typedef struct cm_traceWork_s
{
	int numVerts;
	cm_trmVertex_t vertices[MAX_TRACEMODEL_VERTS];	// trm vertices
	int numEdges;
	cm_trmEdge_t edges[MAX_TRACEMODEL_EDGES + 1];		// trm edges
	int numPolys;
	cm_trmPolygon_t polys[MAX_TRACEMODEL_POLYS];	// trm polygons
	cm_model_t* model;								// model colliding with
	Vector3 start;									// start of trace
	Vector3 end;										// end of trace
	Vector3 dir;										// trace direction
	budBounds bounds;								// bounds of full trace
	budBounds size;									// bounds of transformed trm relative to start
	Vector3 extents;									// largest of abs(size[0]) and abs(size[1]) for BSP trace
	int contents;									// ignore polygons that do not have any of these contents flags
	trace_t trace;									// collision detection result
	
	bool rotation;									// true if calculating rotational collision
	bool pointTrace;								// true if only tracing a point
	bool positionTest;								// true if not tracing but doing a position test
	bool isConvex;									// true if the trace model is convex
	bool axisIntersectsTrm;							// true if the rotation axis intersects the trace model
	bool getContacts;								// true if retrieving contacts
	bool quickExit;									// set to quickly stop the collision detection calculations
	
	Vector3 origin;									// origin of rotation in model space
	Vector3 axis;									// rotation axis in model space
	Matrix3 matrix;									// rotates axis of rotation to the z-axis
	float angle;									// angle for rotational collision
	float maxTan;									// max tangent of half the positive angle used instead of fraction
	float radius;									// rotation radius of trm start
	Rotation modelVertexRotation;					// inverse rotation for model vertices
	
	contactInfo_t* contacts;						// array with contacts
	int maxContacts;								// max size of contact array
	int numContacts;								// number of contacts found
	
	budPlane heartPlane1;							// polygons should be near anough the trace heart planes
	float maxDistFromHeartPlane1;
	budPlane heartPlane2;
	float maxDistFromHeartPlane2;
	idPluecker polygonEdgePlueckerCache[CM_MAX_POLYGON_EDGES];
	idPluecker polygonVertexPlueckerCache[CM_MAX_POLYGON_EDGES];
	Vector3 polygonRotationOriginCache[CM_MAX_POLYGON_EDGES];
} cm_traceWork_t;

/*
===============================================================================

Collision Map

===============================================================================
*/

typedef struct cm_procNode_s
{
	budPlane plane;
	int children[2];				// negative numbers are (-1 - areaNumber), 0 = solid
} cm_procNode_t;

class budCollisionModelManagerLocal : public budCollisionModelManager
{
public:
	// load collision models from a map file
	void			LoadMap( const budMapFile* mapFile );
	// frees all the collision models
	void			FreeMap();
	
	void			Preload( const char* mapName );
	// get clip handle for model
	cmHandle_t		LoadModel( const char* modelName );
	// sets up a trace model for collision with other trace models
	cmHandle_t		SetupTrmModel( const budTraceModel& trm, const budMaterial* material );
	// create trace model from a collision model, returns true if succesfull
	bool			TrmFromModel( const char* modelName, budTraceModel& trm );
	
	// name of the model
	const char* 	GetModelName( cmHandle_t model ) const;
	// bounds of the model
	bool			GetModelBounds( cmHandle_t model, budBounds& bounds ) const;
	// all contents flags of brushes and polygons ored together
	bool			GetModelContents( cmHandle_t model, int& contents ) const;
	// get the vertex of a model
	bool			GetModelVertex( cmHandle_t model, int vertexNum, Vector3& vertex ) const;
	// get the edge of a model
	bool			GetModelEdge( cmHandle_t model, int edgeNum, Vector3& start, Vector3& end ) const;
	// get the polygon of a model
	bool			GetModelPolygon( cmHandle_t model, int polygonNum, budFixedWinding& winding ) const;
	
	// translates a trm and reports the first collision if any
	void			Translation( trace_t* results, const Vector3& start, const Vector3& end,
								 const budTraceModel* trm, const Matrix3& trmAxis, int contentMask,
								 cmHandle_t model, const Vector3& modelOrigin, const Matrix3& modelAxis );
	// rotates a trm and reports the first collision if any
	void			Rotation( trace_t* results, const Vector3& start, const Rotation& rotation,
							  const budTraceModel* trm, const Matrix3& trmAxis, int contentMask,
							  cmHandle_t model, const Vector3& modelOrigin, const Matrix3& modelAxis );
	// returns the contents the trm is stuck in or 0 if the trm is in free space
	int				Contents( const Vector3& start,
							  const budTraceModel* trm, const Matrix3& trmAxis, int contentMask,
							  cmHandle_t model, const Vector3& modelOrigin, const Matrix3& modelAxis );
	// stores all contact points of the trm with the model, returns the number of contacts
	int				Contacts( contactInfo_t* contacts, const int maxContacts, const Vector3& start, const Vector6& dir, const float depth,
							  const budTraceModel* trm, const Matrix3& trmAxis, int contentMask,
							  cmHandle_t model, const Vector3& modelOrigin, const Matrix3& modelAxis );
	// test collision detection
	void			DebugOutput( const Vector3& origin );
	// draw a model
	void			DrawModel( cmHandle_t model, const Vector3& origin, const Matrix3& axis,
							   const Vector3& viewOrigin, const float radius );
	// print model information, use -1 handle for accumulated model info
	void			ModelInfo( cmHandle_t model );
	// list all loaded models
	void			ListModels();
	// write a collision model file for the map entity
	bool			WriteCollisionModelForMapEntity( const idMapEntity* mapEnt, const char* filename, const bool testTraceModel = true );
	
private:			// CollisionMap_translate.cpp
	int				TranslateEdgeThroughEdge( Vector3& cross, idPluecker& l1, idPluecker& l2, float* fraction );
	void			TranslateTrmEdgeThroughPolygon( cm_traceWork_t* tw, cm_polygon_t* poly, cm_trmEdge_t* trmEdge );
	void			TranslateTrmVertexThroughPolygon( cm_traceWork_t* tw, cm_polygon_t* poly, cm_trmVertex_t* v, int bitNum );
	void			TranslatePointThroughPolygon( cm_traceWork_t* tw, cm_polygon_t* poly, cm_trmVertex_t* v );
	void			TranslateVertexThroughTrmPolygon( cm_traceWork_t* tw, cm_trmPolygon_t* trmpoly, cm_polygon_t* poly, cm_vertex_t* v, Vector3& endp, idPluecker& pl );
	bool			TranslateTrmThroughPolygon( cm_traceWork_t* tw, cm_polygon_t* p );
	void			SetupTranslationHeartPlanes( cm_traceWork_t* tw );
	void			SetupTrm( cm_traceWork_t* tw, const budTraceModel* trm );
	
private:			// CollisionMap_rotate.cpp
	int				CollisionBetweenEdgeBounds( cm_traceWork_t* tw, const Vector3& va, const Vector3& vb,
			const Vector3& vc, const Vector3& vd, float tanHalfAngle,
			Vector3& collisionPoint, Vector3& collisionNormal );
	int				RotateEdgeThroughEdge( cm_traceWork_t* tw, const idPluecker& pl1,
										   const Vector3& vc, const Vector3& vd,
										   const float minTan, float& tanHalfAngle );
	int				EdgeFurthestFromEdge( cm_traceWork_t* tw, const idPluecker& pl1,
										  const Vector3& vc, const Vector3& vd,
										  float& tanHalfAngle, float& dir );
	void			RotateTrmEdgeThroughPolygon( cm_traceWork_t* tw, cm_polygon_t* poly, cm_trmEdge_t* trmEdge );
	int				RotatePointThroughPlane( const cm_traceWork_t* tw, const Vector3& point, const budPlane& plane,
			const float angle, const float minTan, float& tanHalfAngle );
	int				PointFurthestFromPlane( const cm_traceWork_t* tw, const Vector3& point, const budPlane& plane,
											const float angle, float& tanHalfAngle, float& dir );
	int				RotatePointThroughEpsilonPlane( const cm_traceWork_t* tw, const Vector3& point, const Vector3& endPoint,
			const budPlane& plane, const float angle, const Vector3& origin,
			float& tanHalfAngle, Vector3& collisionPoint, Vector3& endDir );
	void			RotateTrmVertexThroughPolygon( cm_traceWork_t* tw, cm_polygon_t* poly, cm_trmVertex_t* v, int vertexNum );
	void			RotateVertexThroughTrmPolygon( cm_traceWork_t* tw, cm_trmPolygon_t* trmpoly, cm_polygon_t* poly,
			cm_vertex_t* v, Vector3& rotationOrigin );
	bool			RotateTrmThroughPolygon( cm_traceWork_t* tw, cm_polygon_t* p );
	void			BoundsForRotation( const Vector3& origin, const Vector3& axis, const Vector3& start, const Vector3& end, budBounds& bounds );
	void			Rotation180( trace_t* results, const Vector3& rorg, const Vector3& axis,
								 const float startAngle, const float endAngle, const Vector3& start,
								 const budTraceModel* trm, const Matrix3& trmAxis, int contentMask,
								 cmHandle_t model, const Vector3& origin, const Matrix3& modelAxis );
								 
private:			// CollisionMap_contents.cpp
	bool			TestTrmVertsInBrush( cm_traceWork_t* tw, cm_brush_t* b );
	bool			TestTrmInPolygon( cm_traceWork_t* tw, cm_polygon_t* p );
	cm_node_t* 		PointNode( const Vector3& p, cm_model_t* model );
	int				PointContents( const Vector3 p, cmHandle_t model );
	int				TransformedPointContents( const Vector3& p, cmHandle_t model, const Vector3& origin, const Matrix3& modelAxis );
	int				ContentsTrm( trace_t* results, const Vector3& start,
								 const budTraceModel* trm, const Matrix3& trmAxis, int contentMask,
								 cmHandle_t model, const Vector3& modelOrigin, const Matrix3& modelAxis );
								 
private:			// CollisionMap_trace.cpp
	void			TraceTrmThroughNode( cm_traceWork_t* tw, cm_node_t* node );
	void			TraceThroughAxialBSPTree_r( cm_traceWork_t* tw, cm_node_t* node, float p1f, float p2f, Vector3& p1, Vector3& p2 );
	void			TraceThroughModel( cm_traceWork_t* tw );
	void			RecurseProcBSP_r( trace_t* results, int parentNodeNum, int nodeNum, float p1f, float p2f, const Vector3& p1, const Vector3& p2 );
	
private:			// CollisionMap_load.cpp
	void			Clear();
	void			FreeTrmModelStructure();
	// model deallocation
	void			RemovePolygonReferences_r( cm_node_t* node, cm_polygon_t* p );
	void			RemoveBrushReferences_r( cm_node_t* node, cm_brush_t* b );
	void			FreeNode( cm_node_t* node );
	void			FreePolygonReference( cm_polygonRef_t* pref );
	void			FreeBrushReference( cm_brushRef_t* bref );
	void			FreePolygon( cm_model_t* model, cm_polygon_t* poly );
	void			FreeBrush( cm_model_t* model, cm_brush_t* brush );
	void			FreeTree_r( cm_model_t* model, cm_node_t* headNode, cm_node_t* node );
	void			FreeModel( cm_model_t* model );
	// merging polygons
	void			ReplacePolygons( cm_model_t* model, cm_node_t* node, cm_polygon_t* p1, cm_polygon_t* p2, cm_polygon_t* newp );
	cm_polygon_t* 	TryMergePolygons( cm_model_t* model, cm_polygon_t* p1, cm_polygon_t* p2 );
	bool			MergePolygonWithTreePolygons( cm_model_t* model, cm_node_t* node, cm_polygon_t* polygon );
	void			MergeTreePolygons( cm_model_t* model, cm_node_t* node );
	// finding internal edges
	bool			PointInsidePolygon( cm_model_t* model, cm_polygon_t* p, Vector3& v );
	void			FindInternalEdgesOnPolygon( cm_model_t* model, cm_polygon_t* p1, cm_polygon_t* p2 );
	void			FindInternalPolygonEdges( cm_model_t* model, cm_node_t* node, cm_polygon_t* polygon );
	void			FindInternalEdges( cm_model_t* model, cm_node_t* node );
	void			FindContainedEdges( cm_model_t* model, cm_polygon_t* p );
	// loading of proc BSP tree
	void			ParseProcNodes( budLexer* src );
	void			LoadProcBSP( const char* name );
	// removal of contained polygons
	int				R_ChoppedAwayByProcBSP( int nodeNum, budFixedWinding* w, const Vector3& normal, const Vector3& origin, const float radius );
	int				ChoppedAwayByProcBSP( const budFixedWinding& w, const budPlane& plane, int contents );
	void			ChopWindingListWithBrush( cm_windingList_t* list, cm_brush_t* b );
	void			R_ChopWindingListWithTreeBrushes( cm_windingList_t* list, cm_node_t* node );
	budFixedWinding* WindingOutsideBrushes( budFixedWinding* w, const budPlane& plane, int contents, int patch, cm_node_t* headNode );
	// creation of axial BSP tree
	cm_model_t* 	AllocModel();
	cm_node_t* 		AllocNode( cm_model_t* model, int blockSize );
	cm_polygonRef_t* AllocPolygonReference( cm_model_t* model, int blockSize );
	cm_brushRef_t* 	AllocBrushReference( cm_model_t* model, int blockSize );
	cm_polygon_t* 	AllocPolygon( cm_model_t* model, int numEdges );
	cm_brush_t* 	AllocBrush( cm_model_t* model, int numPlanes );
	void			AddPolygonToNode( cm_model_t* model, cm_node_t* node, cm_polygon_t* p );
	void			AddBrushToNode( cm_model_t* model, cm_node_t* node, cm_brush_t* b );
	void			SetupTrmModelStructure();
	void			R_FilterPolygonIntoTree( cm_model_t* model, cm_node_t* node, cm_polygonRef_t* pref, cm_polygon_t* p );
	void			R_FilterBrushIntoTree( cm_model_t* model, cm_node_t* node, cm_brushRef_t* pref, cm_brush_t* b );
	cm_node_t* 		R_CreateAxialBSPTree( cm_model_t* model, cm_node_t* node, const budBounds& bounds );
	cm_node_t* 		CreateAxialBSPTree( cm_model_t* model, cm_node_t* node );
	// creation of raw polygons
	void			SetupHash();
	void			ShutdownHash();
	void			ClearHash( budBounds& bounds );
	int				HashVec( const Vector3& vec );
	int				GetVertex( cm_model_t* model, const Vector3& v, int* vertexNum );
	int				GetEdge( cm_model_t* model, const Vector3& v1, const Vector3& v2, int* edgeNum, int v1num );
	void			CreatePolygon( cm_model_t* model, budFixedWinding* w, const budPlane& plane, const budMaterial* material, int primitiveNum );
	void			PolygonFromWinding( cm_model_t* model, budFixedWinding* w, const budPlane& plane, const budMaterial* material, int primitiveNum );
	void			CalculateEdgeNormals( cm_model_t* model, cm_node_t* node );
	void			CreatePatchPolygons( cm_model_t* model, idSurface_Patch& mesh, const budMaterial* material, int primitiveNum );
	void			ConvertPatch( cm_model_t* model, const idMapPatch* patch, int primitiveNum );
	void			ConvertBrushSides( cm_model_t* model, const idMapBrush* mapBrush, int primitiveNum );
	void			ConvertBrush( cm_model_t* model, const idMapBrush* mapBrush, int primitiveNum );
	// RB: support new .map format
	void			ConvertMesh( cm_model_t* model, const MapPolygonMesh* mesh, int primitiveNum );
	// RB end
	void			PrintModelInfo( const cm_model_t* model );
	void			AccumulateModelInfo( cm_model_t* model );
	void			RemapEdges( cm_node_t* node, int* edgeRemap );
	void			OptimizeArrays( cm_model_t* model );
	void			FinishModel( cm_model_t* model );
	void			BuildModels( const budMapFile* mapFile );
	cmHandle_t		FindModel( const char* name );
	cm_model_t* 	CollisionModelForMapEntity( const idMapEntity* mapEnt );	// brush/patch model from .map
	cm_model_t* 	LoadRenderModel( const char* fileName );					// ASE/LWO models
	cm_model_t* 	LoadBinaryModel( const char* fileName, ID_TIME_T sourceTimeStamp );
	cm_model_t* 	LoadBinaryModelFromFile( budFile* fileIn, ID_TIME_T sourceTimeStamp );
	void			WriteBinaryModel( cm_model_t* model, const char* fileName, ID_TIME_T sourceTimeStamp );
	void			WriteBinaryModelToFile( cm_model_t* model, budFile* fileOut, ID_TIME_T sourceTimeStamp );
	bool			TrmFromModel_r( budTraceModel& trm, cm_node_t* node );
	bool			TrmFromModel( const cm_model_t* model, budTraceModel& trm );
	
private:			// CollisionMap_files.cpp
	// writing
	void			WriteNodes( budFile* fp, cm_node_t* node );
	int				CountPolygonMemory( cm_node_t* node ) const;
	void			WritePolygons( budFile* fp, cm_node_t* node );
	int				CountBrushMemory( cm_node_t* node ) const;
	void			WriteBrushes( budFile* fp, cm_node_t* node );
	void			WriteCollisionModel( budFile* fp, cm_model_t* model );
	void			WriteCollisionModelsToFile( const char* filename, int firstModel, int lastModel, unsigned int mapFileCRC );
	// loading
	cm_node_t* 		ParseNodes( budLexer* src, cm_model_t* model, cm_node_t* parent );
	void			ParseVertices( budLexer* src, cm_model_t* model );
	void			ParseEdges( budLexer* src, cm_model_t* model );
	void			ParsePolygons( budLexer* src, cm_model_t* model );
	void			ParseBrushes( budLexer* src, cm_model_t* model );
	cm_model_t* 	ParseCollisionModel( budLexer* src );
	bool			LoadCollisionModelFile( const char* name, unsigned int mapFileCRC );
	
private:			// CollisionMap_debug
	int				ContentsFromString( const char* string ) const;
	const char* 	StringFromContents( const int contents ) const;
	void			DrawEdge( cm_model_t* model, int edgeNum, const Vector3& origin, const Matrix3& axis );
	void			DrawPolygon( cm_model_t* model, cm_polygon_t* p, const Vector3& origin, const Matrix3& axis,
								 const Vector3& viewOrigin );
	void			DrawNodePolygons( cm_model_t* model, cm_node_t* node, const Vector3& origin, const Matrix3& axis,
									  const Vector3& viewOrigin, const float radius );
									  
private:			// collision map data
	String			mapName;
	ID_TIME_T			mapFileTime;
	int				loaded;
	// for multi-check avoidance
	int				checkCount;
	// models
	int				maxModels;
	int				numModels;
	cm_model_t** 	models;
	// polygons and brush for trm model
	cm_polygonRef_t* trmPolygons[MAX_TRACEMODEL_POLYS];
	cm_brushRef_t* 	trmBrushes[1];
	const budMaterial* trmMaterial;
	// for data pruning
	int				numProcNodes;
	cm_procNode_t* 	procNodes;
	// for retrieving contact points
	bool			getContacts;
	contactInfo_t* 	contacts;
	int				maxContacts;
	int				numContacts;
};

// for debugging
extern CVar cm_debugCollision;
