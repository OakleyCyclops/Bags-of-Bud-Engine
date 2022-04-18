/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "PCH.hpp"
#pragma hdrstop

#include "AASBuild_local.h"

#define BFL_PATCH		0x1000

//===============================================================
//
//	budAASBuild
//
//===============================================================

/*
============
budAASBuild::budAASBuild
============
*/
budAASBuild::budAASBuild()
{
	file = NULL;
	procNodes = NULL;
	numProcNodes = 0;
	numGravitationalSubdivisions = 0;
	numMergedLeafNodes = 0;
	numLedgeSubdivisions = 0;
	ledgeMap = NULL;
}

/*
============
budAASBuild::~budAASBuild
============
*/
budAASBuild::~budAASBuild()
{
	Shutdown();
}

/*
================
budAASBuild::Shutdown
================
*/
void budAASBuild::Shutdown()
{
	aasSettings = NULL;
	if( file )
	{
		delete file;
		file = NULL;
	}
	DeleteProcBSP();
	numGravitationalSubdivisions = 0;
	numMergedLeafNodes = 0;
	numLedgeSubdivisions = 0;
	ledgeList.Clear();
	if( ledgeMap )
	{
		delete ledgeMap;
		ledgeMap = NULL;
	}
}

/*
================
budAASBuild::ParseProcNodes
================
*/
void budAASBuild::ParseProcNodes( budLexer* src )
{
	int i;
	
	src->ExpectTokenString( "{" );
	
	budAASBuild::numProcNodes = src->ParseInt();
	if( budAASBuild::numProcNodes < 0 )
	{
		src->Error( "budAASBuild::ParseProcNodes: bad numProcNodes" );
	}
	budAASBuild::procNodes = ( aasProcNode_t* )Mem_ClearedAlloc( budAASBuild::numProcNodes * sizeof( aasProcNode_t ), TAG_TOOLS );
	
	for( i = 0; i < budAASBuild::numProcNodes; i++ )
	{
		aasProcNode_t* node;
		
		node = &( budAASBuild::procNodes[i] );
		
		src->Parse1DMatrix( 4, node->plane.ToFloatPtr() );
		node->children[0] = src->ParseInt();
		node->children[1] = src->ParseInt();
	}
	
	src->ExpectTokenString( "}" );
}

/*
================
budAASBuild::LoadProcBSP
================
*/
bool budAASBuild::LoadProcBSP( const char* name, ID_TIME_T minFileTime )
{
	String fileName;
	budToken token;
	budLexer* src;
	
	// load it
	fileName = name;
	fileName.SetFileExtension( PROC_FILE_EXT );
	src = new budLexer( fileName, LEXFL_NOSTRINGCONCAT | LEXFL_NODOLLARPRECOMPILE );
	if( !src->IsLoaded() )
	{
		common->Warning( "budAASBuild::LoadProcBSP: couldn't load %s", fileName.c_str() );
		delete src;
		return false;
	}
	
	// if the file is too old
	if( src->GetFileTime() < minFileTime )
	{
		delete src;
		return false;
	}
	
	if( !src->ReadToken( &token ) || token.Icmp( PROC_FILE_ID ) )
	{
		common->Warning( "budAASBuild::LoadProcBSP: bad id '%s' instead of '%s'", token.c_str(), PROC_FILE_ID );
		delete src;
		return false;
	}
	
	// parse the file
	while( 1 )
	{
		if( !src->ReadToken( &token ) )
		{
			break;
		}
		
		if( token == "model" )
		{
			src->SkipBracedSection();
			continue;
		}
		
		if( token == "shadowModel" )
		{
			src->SkipBracedSection();
			continue;
		}
		
		if( token == "interAreaPortals" )
		{
			src->SkipBracedSection();
			continue;
		}
		
		if( token == "nodes" )
		{
			budAASBuild::ParseProcNodes( src );
			break;
		}
		
		src->Error( "budAASBuild::LoadProcBSP: bad token \"%s\"", token.c_str() );
	}
	
	delete src;
	
	return true;
}

/*
============
budAASBuild::DeleteProcBSP
============
*/
void budAASBuild::DeleteProcBSP()
{
	if( procNodes )
	{
		Mem_Free( procNodes );
		procNodes = NULL;
	}
	numProcNodes = 0;
}

/*
============
budAASBuild::ChoppedAwayByProcBSP
============
*/
bool budAASBuild::ChoppedAwayByProcBSP( int nodeNum, budFixedWinding* w, const Vector3& normal, const Vector3& origin, const float radius )
{
	int res;
	budFixedWinding back;
	aasProcNode_t* node;
	float dist;
	
	do
	{
		node = budAASBuild::procNodes + nodeNum;
		dist = node->plane.Normal() * origin + node->plane[3];
		if( dist > radius )
		{
			res = SIDE_FRONT;
		}
		else if( dist < -radius )
		{
			res = SIDE_BACK;
		}
		else
		{
			res = w->Split( &back, node->plane, ON_EPSILON );
		}
		if( res == SIDE_FRONT )
		{
			nodeNum = node->children[0];
		}
		else if( res == SIDE_BACK )
		{
			nodeNum = node->children[1];
		}
		else if( res == SIDE_ON )
		{
			// continue with the side the winding faces
			if( node->plane.Normal() * normal > 0.0f )
			{
				nodeNum = node->children[0];
			}
			else
			{
				nodeNum = node->children[1];
			}
		}
		else
		{
			// if either node is not solid
			if( node->children[0] < 0 || node->children[1] < 0 )
			{
				return false;
			}
			// only recurse if the node is not solid
			if( node->children[1] > 0 )
			{
				if( !budAASBuild::ChoppedAwayByProcBSP( node->children[1], &back, normal, origin, radius ) )
				{
					return false;
				}
			}
			nodeNum = node->children[0];
		}
	}
	while( nodeNum > 0 );
	if( nodeNum < 0 )
	{
		return false;
	}
	return true;
}

/*
============
budAASBuild::ClipBrushSidesWithProcBSP
============
*/
void budAASBuild::ClipBrushSidesWithProcBSP( idBrushList& brushList )
{
	int i, clippedSides;
	idBrush* brush;
	budFixedWinding neww;
	budBounds bounds;
	float radius;
	Vector3 origin;
	
	// if the .proc file has no BSP tree
	if( budAASBuild::procNodes == NULL )
	{
		return;
	}
	
	clippedSides = 0;
	for( brush = brushList.Head(); brush; brush = brush->Next() )
	{
		for( i = 0; i < brush->GetNumSides(); i++ )
		{
		
			if( !brush->GetSide( i )->GetWinding() )
			{
				continue;
			}
			
			// make a local copy of the winding
			neww = *brush->GetSide( i )->GetWinding();
			neww.GetBounds( bounds );
			origin = ( bounds[1] - bounds[0] ) * 0.5f;
			radius = origin.Length() + ON_EPSILON;
			origin = bounds[0] + origin;
			
			if( ChoppedAwayByProcBSP( 0, &neww, brush->GetSide( i )->GetPlane().Normal(), origin, radius ) )
			{
				brush->GetSide( i )->SetFlag( SFL_USED_SPLITTER );
				clippedSides++;
			}
		}
	}
	
	common->Printf( "%6d brush sides clipped\n", clippedSides );
}

/*
============
budAASBuild::ContentsForAAS
============
*/
int budAASBuild::ContentsForAAS( int contents )
{
	int c;
	
	if( contents & ( CONTENTS_SOLID | CONTENTS_AAS_SOLID | CONTENTS_MONSTERCLIP ) )
	{
		return AREACONTENTS_SOLID;
	}
	c = 0;
	if( contents & CONTENTS_WATER )
	{
		c |= AREACONTENTS_WATER;
	}
	if( contents & CONTENTS_AREAPORTAL )
	{
		c |= AREACONTENTS_CLUSTERPORTAL;
	}
	if( contents & CONTENTS_AAS_OBSTACLE )
	{
		c |= AREACONTENTS_OBSTACLE;
	}
	return c;
}

/*
============
budAASBuild::AddBrushForMapBrush
============
*/
idBrushList budAASBuild::AddBrushesForMapBrush( const idMapBrush* mapBrush, const Vector3& origin, const Matrix3& axis, int entityNum, int primitiveNum, idBrushList brushList )
{
	int contents, i;
	idMapBrushSide* mapSide;
	const budMaterial* mat;
	List<idBrushSide*> sideList;
	idBrush* brush;
	budPlane plane;
	
	contents = 0;
	for( i = 0; i < mapBrush->GetNumSides(); i++ )
	{
		mapSide = mapBrush->GetSide( i );
		mat = declManager->FindMaterial( mapSide->GetMaterial() );
		contents |= mat->GetContentFlags();
		plane = mapSide->GetPlane();
		plane.FixDegeneracies( DEGENERATE_DIST_EPSILON );
		sideList.Append( new idBrushSide( plane, -1 ) );
	}
	
	contents = ContentsForAAS( contents );
	if( !contents )
	{
		for( i = 0; i < sideList.Num(); i++ )
		{
			delete sideList[i];
		}
		return brushList;
	}
	
	brush = new idBrush();
	brush->SetContents( contents );
	
	if( !brush->FromSides( sideList ) )
	{
		common->Warning( "brush primitive %d on entity %d is degenerate", primitiveNum, entityNum );
		delete brush;
		return brushList;
	}
	
	brush->SetEntityNum( entityNum );
	brush->SetPrimitiveNum( primitiveNum );
	brush->Transform( origin, axis );
	brushList.AddToTail( brush );
	
	return brushList;
}

/*
============
budAASBuild::AddBrushesForPatch
============
*/
idBrushList budAASBuild::AddBrushesForMapPatch( const idMapPatch* mapPatch, const Vector3& origin, const Matrix3& axis, int entityNum, int primitiveNum, idBrushList brushList )
{
	int i, j, contents, validBrushes;
	float dot;
	int v1, v2, v3, v4;
	budFixedWinding w;
	budPlane plane;
	Vector3 d1, d2;
	idBrush* brush;
	idSurface_Patch mesh;
	const budMaterial* mat;
	
	mat = declManager->FindMaterial( mapPatch->GetMaterial() );
	contents = ContentsForAAS( mat->GetContentFlags() );
	
	if( !contents )
	{
		return brushList;
	}
	
	mesh = idSurface_Patch( *mapPatch );
	
	// if the patch has an explicit number of subdivisions use it to avoid cracks
	if( mapPatch->GetExplicitlySubdivided() )
	{
		mesh.SubdivideExplicit( mapPatch->GetHorzSubdivisions(), mapPatch->GetVertSubdivisions(), false, true );
	}
	else
	{
		mesh.Subdivide( DEFAULT_CURVE_MAX_ERROR_CD, DEFAULT_CURVE_MAX_ERROR_CD, DEFAULT_CURVE_MAX_LENGTH_CD, false );
	}
	
	validBrushes = 0;
	
	for( i = 0; i < mesh.GetWidth() - 1; i++ )
	{
		for( j = 0; j < mesh.GetHeight() - 1; j++ )
		{
		
			v1 = j * mesh.GetWidth() + i;
			v2 = v1 + 1;
			v3 = v1 + mesh.GetWidth() + 1;
			v4 = v1 + mesh.GetWidth();
			
			d1 = mesh[v2].xyz - mesh[v1].xyz;
			d2 = mesh[v3].xyz - mesh[v1].xyz;
			plane.SetNormal( d1.Cross( d2 ) );
			if( plane.Normalize() != 0.0f )
			{
				plane.FitThroughPoint( mesh[v1].xyz );
				dot = plane.Distance( mesh[v4].xyz );
				// if we can turn it into a quad
				if( Math::Fabs( dot ) < 0.1f )
				{
					w.Clear();
					w += mesh[v1].xyz;
					w += mesh[v2].xyz;
					w += mesh[v3].xyz;
					w += mesh[v4].xyz;
					
					brush = new idBrush();
					brush->SetContents( contents );
					if( brush->FromWinding( w, plane ) )
					{
						brush->SetEntityNum( entityNum );
						brush->SetPrimitiveNum( primitiveNum );
						brush->SetFlag( BFL_PATCH );
						brush->Transform( origin, axis );
						brushList.AddToTail( brush );
						validBrushes++;
					}
					else
					{
						delete brush;
					}
					continue;
				}
				else
				{
					// create one of the triangles
					w.Clear();
					w += mesh[v1].xyz;
					w += mesh[v2].xyz;
					w += mesh[v3].xyz;
					
					brush = new idBrush();
					brush->SetContents( contents );
					if( brush->FromWinding( w, plane ) )
					{
						brush->SetEntityNum( entityNum );
						brush->SetPrimitiveNum( primitiveNum );
						brush->SetFlag( BFL_PATCH );
						brush->Transform( origin, axis );
						brushList.AddToTail( brush );
						validBrushes++;
					}
					else
					{
						delete brush;
					}
				}
			}
			// create the other triangle
			d1 = mesh[v3].xyz - mesh[v1].xyz;
			d2 = mesh[v4].xyz - mesh[v1].xyz;
			plane.SetNormal( d1.Cross( d2 ) );
			if( plane.Normalize() != 0.0f )
			{
				plane.FitThroughPoint( mesh[v1].xyz );
				
				w.Clear();
				w += mesh[v1].xyz;
				w += mesh[v3].xyz;
				w += mesh[v4].xyz;
				
				brush = new idBrush();
				brush->SetContents( contents );
				if( brush->FromWinding( w, plane ) )
				{
					brush->SetEntityNum( entityNum );
					brush->SetPrimitiveNum( primitiveNum );
					brush->SetFlag( BFL_PATCH );
					brush->Transform( origin, axis );
					brushList.AddToTail( brush );
					validBrushes++;
				}
				else
				{
					delete brush;
				}
			}
		}
	}
	
	if( !validBrushes )
	{
		common->Warning( "patch primitive %d on entity %d is completely degenerate", primitiveNum, entityNum );
	}
	
	return brushList;
}

/*
============
budAASBuild::AddBrushesForMapEntity
============
*/
idBrushList budAASBuild::AddBrushesForMapEntity( const idMapEntity* mapEnt, int entityNum, idBrushList brushList )
{
	int i;
	Vector3 origin;
	Matrix3 axis;
	
	if( mapEnt->GetNumPrimitives() < 1 )
	{
		return brushList;
	}
	
	mapEnt->epairs.GetVector( "origin", "0 0 0", origin );
	if( !mapEnt->epairs.GetMatrix( "rotation", "1 0 0 0 1 0 0 0 1", axis ) )
	{
		float angle = mapEnt->epairs.GetFloat( "angle" );
		if( angle != 0.0f )
		{
			axis = Angles( 0.0f, angle, 0.0f ).ToMat3();
		}
		else
		{
			axis.Identity();
		}
	}
	
	for( i = 0; i < mapEnt->GetNumPrimitives(); i++ )
	{
		idMapPrimitive*	mapPrim;
		
		mapPrim = mapEnt->GetPrimitive( i );
		if( mapPrim->GetType() == idMapPrimitive::TYPE_BRUSH )
		{
			brushList = AddBrushesForMapBrush( static_cast<idMapBrush*>( mapPrim ), origin, axis, entityNum, i, brushList );
			continue;
		}
		if( mapPrim->GetType() == idMapPrimitive::TYPE_PATCH )
		{
			if( aasSettings->usePatches )
			{
				brushList = AddBrushesForMapPatch( static_cast<idMapPatch*>( mapPrim ), origin, axis, entityNum, i, brushList );
			}
			continue;
		}
	}
	
	return brushList;
}

/*
============
budAASBuild::AddBrushesForMapFile
============
*/
idBrushList budAASBuild::AddBrushesForMapFile( const budMapFile* mapFile, idBrushList brushList )
{
	int i;
	
	common->Printf( "[Brush Load]\n" );
	
	brushList = AddBrushesForMapEntity( mapFile->GetEntity( 0 ), 0, brushList );
	
	for( i = 1; i < mapFile->GetNumEntities(); i++ )
	{
		const char* classname = mapFile->GetEntity( i )->epairs.GetString( "classname" );
		
		if( String::Icmp( classname, "func_aas_obstacle" ) == 0 )
		{
			brushList = AddBrushesForMapEntity( mapFile->GetEntity( i ), i, brushList );
		}
	}
	
	common->Printf( "%6d brushes\n", brushList.Num() );
	
	return brushList;
}

/*
============
budAASBuild::CheckForEntities
============
*/
bool budAASBuild::CheckForEntities( const budMapFile* mapFile, StringList& entityClassNames ) const
{
	int		i;
	String	classname;
	
	com_editors |= EDITOR_AAS;
	
	for( i = 0; i < mapFile->GetNumEntities(); i++ )
	{
		if( !mapFile->GetEntity( i )->epairs.GetString( "classname", "", classname ) )
		{
			continue;
		}
		
		if( aasSettings->ValidEntity( classname ) )
		{
			entityClassNames.AddUnique( classname );
		}
	}
	
	com_editors &= ~EDITOR_AAS;
	
	return ( entityClassNames.Num() != 0 );
}

/*
============
MergeAllowed
============
*/
bool MergeAllowed( idBrush* b1, idBrush* b2 )
{
	return ( b1->GetContents() == b2->GetContents() && !( ( b1->GetFlags() | b2->GetFlags() ) & BFL_PATCH ) );
}

/*
============
ExpandedChopAllowed
============
*/
bool ExpandedChopAllowed( idBrush* b1, idBrush* b2 )
{
	return ( b1->GetContents() == b2->GetContents() );
}

/*
============
ExpandedMergeAllowed
============
*/
bool ExpandedMergeAllowed( idBrush* b1, idBrush* b2 )
{
	return ( b1->GetContents() == b2->GetContents() );
}

/*
============
budAASBuild::ChangeMultipleBoundingBoxContents
============
*/
void budAASBuild::ChangeMultipleBoundingBoxContents_r( idBrushBSPNode* node, int mask )
{
	while( node )
	{
		if( !( node->GetContents() & mask ) )
		{
			node->SetContents( node->GetContents() & ~AREACONTENTS_SOLID );
		}
		ChangeMultipleBoundingBoxContents_r( node->GetChild( 0 ), mask );
		node = node->GetChild( 1 );
	}
}

/*
============
budAASBuild::Build
============
*/
bool budAASBuild::Build( const String& fileName, const budAASSettings* settings )
{
	int i, bit, mask, startTime;
	budMapFile* mapFile;
	idBrushList brushList;
	List<idBrushList*> expandedBrushes;
	idBrush* b;
	idBrushBSP bsp;
	String name;
	budAASReach reach;
	budAASCluster cluster;
	StringList entityClassNames;
	
	startTime = Sys_Milliseconds();
	
	Shutdown();
	
	aasSettings = settings;
	
	name = fileName;
	name.SetFileExtension( "map" );
	
	mapFile = new budMapFile;
	if( !mapFile->Parse( name ) )
	{
		delete mapFile;
		common->Error( "Couldn't load map file: '%s'", name.c_str() );
		return false;
	}
	
	// check if this map has any entities that use this AAS file
	if( !CheckForEntities( mapFile, entityClassNames ) )
	{
		delete mapFile;
		common->Printf( "no entities in map that use %s\n", settings->fileExtension.c_str() );
		return true;
	}
	
	// load map file brushes
	brushList = AddBrushesForMapFile( mapFile, brushList );
	
	// if empty map
	if( brushList.Num() == 0 )
	{
		delete mapFile;
		common->Error( "%s is empty", name.c_str() );
		return false;
	}
	
	// merge as many brushes as possible before expansion
	brushList.Merge( MergeAllowed );
	
	// if there is a .proc file newer than the .map file
	if( LoadProcBSP( fileName, mapFile->GetFileTime() ) )
	{
		ClipBrushSidesWithProcBSP( brushList );
		DeleteProcBSP();
	}
	
	// make copies of the brush list
	expandedBrushes.Append( &brushList );
	for( i = 1; i < aasSettings->numBoundingBoxes; i++ )
	{
		expandedBrushes.Append( brushList.Copy() );
	}
	
	// expand brushes for the axial bounding boxes
	mask = AREACONTENTS_SOLID;
	for( i = 0; i < expandedBrushes.Num(); i++ )
	{
		for( b = expandedBrushes[i]->Head(); b; b = b->Next() )
		{
			b->ExpandForAxialBox( aasSettings->boundingBoxes[i] );
			bit = 1 << ( i + AREACONTENTS_BBOX_BIT );
			mask |= bit;
			b->SetContents( b->GetContents() | bit );
		}
	}
	
	// move all brushes back into the original list
	for( i = 1; i < aasSettings->numBoundingBoxes; i++ )
	{
		brushList.AddToTail( *expandedBrushes[i] );
		delete expandedBrushes[i];
	}
	
	if( aasSettings->writeBrushMap )
	{
		bsp.WriteBrushMap( fileName, "_" + aasSettings->fileExtension, AREACONTENTS_SOLID );
	}
	
	// build BSP tree from brushes
	bsp.Build( brushList, AREACONTENTS_SOLID, ExpandedChopAllowed, ExpandedMergeAllowed );
	
	// only solid nodes with all bits set for all bounding boxes need to stay solid
	ChangeMultipleBoundingBoxContents_r( bsp.GetRootNode(), mask );
	
	// portalize the bsp tree
	bsp.Portalize();
	
	// remove subspaces not reachable by entities
	if( !bsp.RemoveOutside( mapFile, AREACONTENTS_SOLID, entityClassNames ) )
	{
		bsp.LeakFile( name );
		delete mapFile;
		common->Printf( "%s has no outside", name.c_str() );
		return false;
	}
	
	// gravitational subdivision
	GravitationalSubdivision( bsp );
	
	// merge portals where possible
	bsp.MergePortals( AREACONTENTS_SOLID );
	
	// melt portal windings
	bsp.MeltPortals( AREACONTENTS_SOLID );
	
	if( aasSettings->writeBrushMap )
	{
		WriteLedgeMap( fileName, "_" + aasSettings->fileExtension + "_ledge" );
	}
	
	// ledge subdivisions
	LedgeSubdivision( bsp );
	
	// merge leaf nodes
	MergeLeafNodes( bsp );
	
	// merge portals where possible
	bsp.MergePortals( AREACONTENTS_SOLID );
	
	// melt portal windings
	bsp.MeltPortals( AREACONTENTS_SOLID );
	
	// store the file from the bsp tree
	StoreFile( bsp );
	file->settings = *aasSettings;
	
	// calculate reachability
	reach.Build( mapFile, file );
	
	// build clusters
	cluster.Build( file );
	
	// optimize the file
	if( !aasSettings->noOptimize )
	{
		file->Optimize();
	}
	
	// write the file
	name.SetFileExtension( aasSettings->fileExtension );
	file->Write( name, mapFile->GetGeometryCRC() );
	
	// delete the map file
	delete mapFile;
	
	common->Printf( "%6d seconds to create AAS\n", ( Sys_Milliseconds() - startTime ) / 1000 );
	
	return true;
}

/*
============
budAASBuild::BuildReachability
============
*/
bool budAASBuild::BuildReachability( const String& fileName, const budAASSettings* settings )
{
	int startTime;
	budMapFile* mapFile;
	String name;
	budAASReach reach;
	budAASCluster cluster;
	
	startTime = Sys_Milliseconds();
	
	aasSettings = settings;
	
	name = fileName;
	name.SetFileExtension( "map" );
	
	mapFile = new budMapFile;
	if( !mapFile->Parse( name ) )
	{
		delete mapFile;
		common->Error( "Couldn't load map file: '%s'", name.c_str() );
		return false;
	}
	
	file = new budAASFileLocal();
	
	name.SetFileExtension( aasSettings->fileExtension );
	if( !file->Load( name, 0 ) )
	{
		delete mapFile;
		common->Error( "Couldn't load AAS file: '%s'", name.c_str() );
		return false;
	}
	
	file->settings = *aasSettings;
	
	// calculate reachability
	reach.Build( mapFile, file );
	
	// build clusters
	cluster.Build( file );
	
	// write the file
	file->Write( name, mapFile->GetGeometryCRC() );
	
	// delete the map file
	delete mapFile;
	
	common->Printf( "%6d seconds to calculate reachability\n", ( Sys_Milliseconds() - startTime ) / 1000 );
	
	return true;
}

/*
============
ParseOptions
============
*/
int ParseOptions( const CmdArgs& args, budAASSettings& settings )
{
	int i;
	String str;
	
	for( i = 1; i < args.Argc(); i++ )
	{
	
		str = args.Argv( i );
		str.StripLeading( '-' );
		
		if( str.Icmp( "usePatches" ) == 0 )
		{
			settings.usePatches = true;
			common->Printf( "usePatches = true\n" );
		}
		else if( str.Icmp( "writeBrushMap" ) == 0 )
		{
			settings.writeBrushMap = true;
			common->Printf( "writeBrushMap = true\n" );
		}
		else if( str.Icmp( "playerFlood" ) == 0 )
		{
			settings.playerFlood = true;
			common->Printf( "playerFlood = true\n" );
		}
		else if( str.Icmp( "noOptimize" ) == 0 )
		{
			settings.noOptimize = true;
			common->Printf( "noOptimize = true\n" );
		}
	}
	return args.Argc() - 1;
}

/*
============
RunAAS_f
============
*/
void RunAAS_f( const CmdArgs& args )
{
	int i;
	budAASBuild aas;
	budAASSettings settings;
	String mapName;
	
	if( args.Argc() <= 1 )
	{
		common->Printf( "runAAS [options] <mapfile>\n"
						"options:\n"
						"  -usePatches        = use bezier patches for collision detection.\n"
						"  -writeBrushMap     = write a brush map with the AAS geometry.\n"
						"  -playerFlood       = use player spawn points as valid AAS positions.\n" );
		return;
	}
	
	common->ClearWarnings( "compiling AAS" );
	
	common->SetRefreshOnPrint( true );
	
	// get the aas settings definitions
	const Dict* dict = gameEdit->FindEntityDefDict( "aas_types", false );
	if( !dict )
	{
		common->Error( "Unable to find entityDef for 'aas_types'" );
	}
	
	const idKeyValue* kv = dict->MatchPrefix( "type" );
	while( kv != NULL )
	{
		const Dict* settingsDict = gameEdit->FindEntityDefDict( kv->GetValue(), false );
		if( !settingsDict )
		{
			common->Warning( "Unable to find '%s' in def/aas.def", kv->GetValue().c_str() );
		}
		else
		{
			settings.FromDict( kv->GetValue(), settingsDict );
			i = ParseOptions( args, settings );
			mapName = args.Argv( i );
			mapName.BackSlashesToSlashes();
			if( mapName.Icmpn( "maps/", 4 ) != 0 )
			{
				mapName = "maps/" + mapName;
			}
			aas.Build( mapName, &settings );
		}
		
		kv = dict->MatchPrefix( "type", kv );
		if( kv )
		{
			common->Printf( "=======================================================\n" );
		}
	}
	common->SetRefreshOnPrint( false );
	common->PrintWarnings();
}

/*
============
RunAASDir_f
============
*/
void RunAASDir_f( const CmdArgs& args )
{
	int i;
	budAASBuild aas;
	budAASSettings settings;
	budFileList* mapFiles;
	
	if( args.Argc() <= 1 )
	{
		common->Printf( "runAASDir <folder>\n" );
		return;
	}
	
	common->ClearWarnings( "compiling AAS" );
	
	common->SetRefreshOnPrint( true );
	
	// get the aas settings definitions
	const Dict* dict = gameEdit->FindEntityDefDict( "aas_types", false );
	if( !dict )
	{
		common->Error( "Unable to find entityDef for 'aas_types'" );
	}
	
	// scan for .map files
	mapFiles = fileSystem->ListFiles( String( "maps/" ) + args.Argv( 1 ), ".map" );
	
	// create AAS files for all the .map files
	for( i = 0; i < mapFiles->GetNumFiles(); i++ )
	{
		if( i )
		{
			common->Printf( "=======================================================\n" );
		}
		
		const idKeyValue* kv = dict->MatchPrefix( "type" );
		while( kv != NULL )
		{
			const Dict* settingsDict = gameEdit->FindEntityDefDict( kv->GetValue(), false );
			if( !settingsDict )
			{
				common->Warning( "Unable to find '%s' in def/aas.def", kv->GetValue().c_str() );
			}
			else
			{
				settings.FromDict( kv->GetValue(), settingsDict );
				aas.Build( String( "maps/" ) + args.Argv( 1 ) + "/" + mapFiles->GetFile( i ), &settings );
			}
			
			kv = dict->MatchPrefix( "type", kv );
			if( kv )
			{
				common->Printf( "=======================================================\n" );
			}
		}
	}
	
	fileSystem->FreeFileList( mapFiles );
	
	common->SetRefreshOnPrint( false );
	common->PrintWarnings();
}

/*
============
RunReach_f
============
*/
void RunReach_f( const CmdArgs& args )
{
	int i;
	budAASBuild aas;
	budAASSettings settings;
	
	if( args.Argc() <= 1 )
	{
		common->Printf( "runReach [options] <mapfile>\n" );
		return;
	}
	
	common->ClearWarnings( "calculating AAS reachability" );
	
	common->SetRefreshOnPrint( true );
	
	// get the aas settings definitions
	const Dict* dict = gameEdit->FindEntityDefDict( "aas_types", false );
	if( !dict )
	{
		common->Error( "Unable to find entityDef for 'aas_types'" );
	}
	
	const idKeyValue* kv = dict->MatchPrefix( "type" );
	while( kv != NULL )
	{
		const Dict* settingsDict = gameEdit->FindEntityDefDict( kv->GetValue(), false );
		if( !settingsDict )
		{
			common->Warning( "Unable to find '%s' in def/aas.def", kv->GetValue().c_str() );
		}
		else
		{
			settings.FromDict( kv->GetValue(), settingsDict );
			i = ParseOptions( args, settings );
			aas.BuildReachability( String( "maps/" ) + args.Argv( i ), &settings );
		}
		
		kv = dict->MatchPrefix( "type", kv );
		if( kv )
		{
			common->Printf( "=======================================================\n" );
		}
	}
	
	common->SetRefreshOnPrint( false );
	common->PrintWarnings();
}
