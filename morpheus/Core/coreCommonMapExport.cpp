/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2015 Robert Beckebans

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

#ifdef HAHAHAHA

#include "corePCH.hpp"
#pragma hdrstop
#include "../Renderer/Image.h"

class OBJExporter
{
public:
	struct OBJFace
	{
		const budMaterial*			material;
		budList<budDrawVert>			verts;
		budList<triIndex_t>			indexes;
	};
	
	struct OBJObject
	{
		budStr						name;
		budList<OBJFace>				faces;
	};
	
	struct OBJGroup
	{
		budStr						name;
		budList<OBJObject>			objects;
	};
	
	budList<OBJGroup>				groups;
	budList< const budMaterial* >		materials;
	
	void	ConvertBrushToOBJ( OBJGroup& group, const idMapBrush* mapBrush, int entityNum, int primitiveNum, const budMat4& transform );
	void	ConvertPatchToOBJ( OBJGroup& group, const idMapPatch* patch, int entityNum, int primitiveNum, const budMat4& transform );
	void	ConvertMeshToOBJ( OBJGroup& group, const MapPolygonMesh* mesh, int entityNum, int primitiveNum, const budMat4& transform );
	
	void	Write( const char* relativePath, const char* basePath = "fs_basepath" );
};

void OBJExporter::Write( const char* relativePath, const char* basePath )
{
	budStrStatic< MAX_OSPATH > convertedFileName = relativePath;
	
	convertedFileName.SetFileExtension( ".obj" );
	budFileLocal objFile( fileSystem->OpenFileWrite( convertedFileName, basePath ) );
	
	convertedFileName.SetFileExtension( ".mtl" );
	budFileLocal mtlFile( fileSystem->OpenFileWrite( convertedFileName, basePath ) );
	
	int totalVerts = 0;
	
	for( int g = 0; g < groups.Num(); g++ )
	{
		const OBJGroup& group = groups[g];
		
		objFile->Printf( "g %s\n", group.name.c_str() );
		
		for( int o = 0; o < group.objects.Num(); o++ )
		{
			const OBJObject& geometry = group.objects[o];
			
			//objFile->Printf( "g %s\n", group.name.c_str() );
			//objFile->Printf( "o %s\n", geometry.name.c_str() );
			
			for( int i = 0; i < geometry.faces.Num(); i++ )
			{
				const OBJFace& face = geometry.faces[i];
				
				for( int j = 0; j < face.verts.Num(); j++ )
				{
					const budVec3& v = face.verts[j].xyz;
					
					objFile->Printf( "v %1.6f %1.6f %1.6f\n", v.x, v.y, v.z );
				}
				
				for( int j = 0; j < face.verts.Num(); j++ )
				{
					const budVec2& vST = face.verts[j].GetTexCoord();
					
					objFile->Printf( "vt %1.6f %1.6f\n", vST.x, vST.y );
				}
				
				for( int j = 0; j < face.verts.Num(); j++ )
				{
					const budVec3& n = face.verts[j].GetNormal();
					
					objFile->Printf( "vn %1.6f %1.6f %1.6f\n", n.x, n.y, n.z );
				}
				
				//objFile->Printf( "g %s\n", group.name.c_str() );
				//objFile->Printf( "o %s\n", geometry.name.c_str() );
				objFile->Printf( "usemtl %s\n", face.material->GetName() );
				
				objFile->Printf( "f " );
				//for( int j = 0; j < face.indexes.Num(); j++ )
				
				// flip order for OBJ
				for( int j = face.indexes.Num() - 1; j >= 0; j-- )
				{
					objFile->Printf( "%i/%i/%i ",
									 face.indexes[j] + 1 + totalVerts,
									 face.indexes[j] + 1 + totalVerts,
									 face.indexes[j] + 1 + totalVerts );
				}
				
				objFile->Printf( "\n\n" );
			}
			
			for( int i = 0; i < geometry.faces.Num(); i++ )
			{
				const OBJFace& face = geometry.faces[i];
				totalVerts += face.verts.Num();
			}
		}
	}
	
	for( int i = 0; i < materials.Num(); i++ )
	{
		const budMaterial* material = materials[i];
		
		mtlFile->Printf( "newmtl %s\n", material->GetName() );
		
		if( material->GetFastPathDiffuseImage() )
		{
			budStr path = material->GetFastPathDiffuseImage()->GetName();
			path.SlashesToBackSlashes();
			path.DefaultFileExtension( ".tga" );
			
			mtlFile->Printf( "\tmap_Kd //..\\..\\..\\%s\n", path.c_str() );
		}
		else if( material->GetEditorImage() )
		{
			budStr path = material->GetEditorImage()->GetName();
			path.SlashesToBackSlashes();
			path.DefaultFileExtension( ".tga" );
			
			mtlFile->Printf( "\tmap_Kd //..\\..\\..\\%s\n", path.c_str() );
		}
		
		
		mtlFile->Printf( "\n" );
	}
}


void OBJExporter::ConvertBrushToOBJ( OBJGroup& group, const idMapBrush* mapBrush, int entityNum, int primitiveNum, const budMat4& transform )
{
	OBJExporter::OBJObject& geometry = group.objects.Alloc();
	
	geometry.name.Format( "Primitive.%i", primitiveNum );
	
	// fix degenerate planes
	budPlane* planes = ( budPlane* ) _alloca16( mapBrush->GetNumSides() * sizeof( planes[0] ) );
	for( int i = 0; i < mapBrush->GetNumSides(); i++ )
	{
		planes[i] = mapBrush->GetSide( i )->GetPlane();
		planes[i].FixDegeneracies( DEGENERATE_DIST_EPSILON );
	}
	
	//budFixedWinding w;
	budList<budFixedWinding> planeWindings;
	budBounds bounds;
	bounds.Clear();
	
	int numVerts = 0;
	int numIndexes = 0;
	
	bool badBrush = false;
	
	
	
	for( int i = 0; i < mapBrush->GetNumSides(); i++ )
	{
		idMapBrushSide* mapSide = mapBrush->GetSide( i );
		
		const budMaterial* material = declManager->FindMaterial( mapSide->GetMaterial() );
		//contents |= ( material->GetContentFlags() & CONTENTS_REMOVE_UTIL );
		materials.AddUnique( material );
		
		// chop base plane by other brush sides
		budFixedWinding& w = planeWindings.Alloc();
		w.BaseForPlane( -planes[i] );
		
		if( !w.GetNumPoints() )
		{
			common->Printf( "Entity %i, Brush %i: base winding has no points\n", entityNum, primitiveNum );
			badBrush = true;
		}
		
		for( int j = 0; j < mapBrush->GetNumSides() && w.GetNumPoints(); j++ )
		{
			if( i == j )
			{
				continue;
			}
			
			
			if( !w.ClipInPlace( -planes[j], 0 ) )
			{
				// no intersection
				//badBrush = true;
				common->Printf( "Entity %i, Brush %i: no intersection with other brush plane\n", entityNum, primitiveNum );
				//break;
			}
		}
		
		if( w.GetNumPoints() <= 2 )
		{
			continue;
		}
		
		for( int j = 0; j < w.GetNumPoints(); j++ )
		{
			const budVec3& v = w[j].ToVec3();
			bounds.AddPoint( v );
		}
	}
	
	// allocate the surface
	
	// copy the data from the windings and build polygons
	for( int i = 0; i < mapBrush->GetNumSides(); i++ )
	{
		idMapBrushSide* mapSide = mapBrush->GetSide( i );
		
		budFixedWinding& w = planeWindings[i];
		if( !w.GetNumPoints() )
		{
			continue;
		}
		
		OBJExporter::OBJFace& face = geometry.faces.Alloc();
		
		face.material = declManager->FindMaterial( mapSide->GetMaterial() );
		
		for( int j = 0; j < w.GetNumPoints(); j++ )
		{
			budDrawVert& dv = face.verts.Alloc();
			
			const budVec3& xyz = w[j].ToVec3();
			
			dv.xyz = ( transform * budVec4( xyz.x, xyz.y, xyz.z, 1 ) ).ToVec3();
			
			// calculate texture s/t from brush primitive texture matrix
			budVec4 texVec[2];
			mapSide->GetTextureVectors( texVec );
			
			budVec2 st;
			st.x = ( xyz * texVec[0].ToVec3() ) + texVec[0][3];
			st.y = ( xyz * texVec[1].ToVec3() ) + texVec[1][3];
			
			// flip y
			st.y = 1.0f - st.y;
			
			dv.SetTexCoord( st );
			
			// copy normal
			dv.SetNormal( transform * mapSide->GetPlane().Normal() );
			
			//if( dv->GetNormal().Length() < 0.9 || dv->GetNormal().Length() > 1.1 )
			//{
			//	common->Error( "Bad normal in TriListForSide" );
			//}
		}
		
#if 0
		// triangulate
		for( int j = 1; j < w.GetNumPoints() - 1; j++ )
		{
			face.indexes.Append( numVerts );
			face.indexes.Append( numVerts + j );
			face.indexes.Append( numVerts + j + 1 );
		}
#else
		// export n-gon
		
		//for( int j = 0; j < w.GetNumPoints(); j++ )
		
		// reverse order, so normal does not point inwards
		for( int j = w.GetNumPoints() - 1; j >= 0; j-- )
		{
			face.indexes.Append( numVerts + j );
		}
#endif
		
		numVerts += w.GetNumPoints();
	}
}


void OBJExporter::ConvertPatchToOBJ( OBJGroup& group, const idMapPatch* patch, int entityNum, int primitiveNum, const budMat4& transform )
{
	OBJExporter::OBJObject& geometry = group.objects.Alloc();
	
	geometry.name.Format( "Primitive.%i", primitiveNum );
	
	idSurface_Patch* cp = new idSurface_Patch( *patch );
	
	if( patch->GetExplicitlySubdivided() )
	{
		cp->SubdivideExplicit( patch->GetHorzSubdivisions(), patch->GetVertSubdivisions(), true );
	}
	else
	{
		cp->Subdivide( DEFAULT_CURVE_MAX_ERROR, DEFAULT_CURVE_MAX_ERROR, DEFAULT_CURVE_MAX_LENGTH, true );
	}
	
	const budMaterial* material = declManager->FindMaterial( patch->GetMaterial() );
	materials.AddUnique( material );
	
	for( int i = 0; i < cp->GetNumIndexes(); i += 3 )
	{
		OBJExporter::OBJFace& face = geometry.faces.Alloc();
		face.material = material;
		
		budDrawVert& dv0 = face.verts.Alloc();
		budDrawVert& dv1 = face.verts.Alloc();
		budDrawVert& dv2 = face.verts.Alloc();
		
		dv0 = ( *cp )[cp->GetIndexes()[i + 1]];
		dv1 = ( *cp )[cp->GetIndexes()[i + 2]];
		dv2 = ( *cp )[cp->GetIndexes()[i + 0]];
		
		dv0.xyz = ( transform * budVec4( dv0.xyz.x, dv0.xyz.y, dv0.xyz.z, 1 ) ).ToVec3();
		dv1.xyz = ( transform * budVec4( dv1.xyz.x, dv1.xyz.y, dv1.xyz.z, 1 ) ).ToVec3();
		dv2.xyz = ( transform * budVec4( dv2.xyz.x, dv2.xyz.y, dv2.xyz.z, 1 ) ).ToVec3();
		
		//face.indexes.Append( cp->GetIndexes()[i + 0] );
		//face.indexes.Append( cp->GetIndexes()[i + 1] );
		//face.indexes.Append( cp->GetIndexes()[i + 2] );
		
		face.indexes.Append( i + 0 );
		face.indexes.Append( i + 1 );
		face.indexes.Append( i + 2 );
	}
	
	delete cp;
}

void OBJExporter::ConvertMeshToOBJ( OBJGroup& group, const MapPolygonMesh* mesh, int entityNum, int primitiveNum, const budMat4& transform )
{
	OBJExporter::OBJObject& geometry = group.objects.Alloc();
	
	geometry.name.Format( "Primitive.%i", primitiveNum );
	
	const budList<budDrawVert>& verts = mesh->GetDrawVerts();
	
	int numVerts = 0;
	
	for( int i = 0; i < mesh->GetNumPolygons(); i++ )
	{
		const MapPolygon& poly = mesh->GetFace( i );
		
		const budMaterial* material = declManager->FindMaterial( poly.GetMaterial() );
		materials.AddUnique( material );
		
		OBJExporter::OBJFace& face = geometry.faces.Alloc();
		face.material = material;
		
		const budList<int>& indexes = poly.GetIndexes();
		
		for( int j = 0; j < verts.Num(); j++ )
		{
			budDrawVert& dv = face.verts.Alloc();
			
			dv = verts[j];
			
			dv.xyz = ( transform * budVec4( dv.xyz.x, dv.xyz.y, dv.xyz.z, 1 ) ).ToVec3();
		}
		
#if 0
		//for( int j = 0; j < indexes.Num(); j++ )
		for( int j = 1; j < indexes.Num() - 1; j++ )
		{
			int index = indexes[j];
			
			//face.indexes.Append( j );
			
			face.indexes.Append( numVerts + j + 1 );
			face.indexes.Append( numVerts + j );
			face.indexes.Append( numVerts );
		}
#else
		for( int j = 0; j < indexes.Num(); j++ )
		{
			int index = indexes[j];
		
			face.indexes.Append( numVerts + index );
		}
#endif
		
		numVerts += verts.Num();
	}
}


CONSOLE_COMMAND( exportMapToOBJ, "Convert .map file to .obj/.mtl ", budCmdSystem::ArgCompletion_MapName )
{
	common->SetRefreshOnPrint( true );
	
	if( args.Argc() != 2 )
	{
		common->Printf( "Usage: exportMapToOBJ <map>\n" );
		return;
	}
	
	budStr filename = args.Argv( 1 );
	if( !filename.Length() )
	{
		return;
	}
	filename.StripFileExtension();
	
	budStr mapName;
	sprintf( mapName, "maps/%s.map", filename.c_str() );
	
	budMapFile map;
	if( map.Parse( mapName, false, false ) )
	{
		OBJExporter exporter;
		
		int count = map.GetNumEntities();
		for( int j = 0; j < count; j++ )
		{
			idMapEntity* ent = map.GetEntity( j );
			if( ent )
			{
				budStr classname = ent->epairs.GetString( "classname" );
				
				budVec3 origin;
				origin.Zero();
				
				budMat3 rot;
				rot.Identity();
				
				if( ent->GetNumPrimitives() )
				{
					OBJExporter::OBJGroup& group = exporter.groups.Alloc();
					
					if( classname == "worldspawn" )
					{
						group.name = "BSP";
					}
					else
					{
						group.name = ent->epairs.GetString( "name" );
						
						origin = ent->epairs.GetVector( "origin", "0 0 0" );
						
						if( !ent->epairs.GetMatrix( "rotation", "1 0 0 0 1 0 0 0 1", rot ) )
						{
							float angle = ent->epairs.GetFloat( "angle" );
							if( angle != 0.0f )
							{
								rot = budAngles( 0.0f, angle, 0.0f ).ToMat3();
							}
							else
							{
								rot.Identity();
							}
						}
					}
					
					budMat4 transform( rot, origin );
					
					for( int i = 0; i < ent->GetNumPrimitives(); i++ )
					{
						idMapPrimitive*	mapPrim;
						
						mapPrim = ent->GetPrimitive( i );
						if( mapPrim->GetType() == idMapPrimitive::TYPE_BRUSH )
						{
							exporter.ConvertBrushToOBJ( group, static_cast<idMapBrush*>( mapPrim ), j, i, transform );
							continue;
						}
						
						if( mapPrim->GetType() == idMapPrimitive::TYPE_PATCH )
						{
							exporter.ConvertPatchToOBJ( group, static_cast<idMapPatch*>( mapPrim ), j, i, transform );
							continue;
						}
						
						if( mapPrim->GetType() == idMapPrimitive::TYPE_MESH )
						{
							exporter.ConvertMeshToOBJ( group, static_cast<MapPolygonMesh*>( mapPrim ), j, i, transform );
							continue;
						}
					}
				}
				
				//Hack: for info_location
				/*
				bool hasLocation = false;
				
				budStrList* list;
				listHash.Get( classname, &list );
				if( list )
				{
					for( int k = 0; k < list->Num(); k++ )
					{
						budStr val = ent->epairs.GetString( ( *list )[k], "" );
				
						if( val.Length() && classname == "info_location" && ( *list )[k] == "location" )
						{
							hasLocation = true;
						}
				
						if( val.Length() && TestMapVal( val ) )
						{
				
							if( !hasLocation || ( *list )[k] == "location" )
							{
								//Localize it!!!
								strCount++;
								ent->epairs.Set( ( *list )[k], langDict.AddString( val ) );
							}
						}
					}
				}
				
				*/
			}
		}
		
		budStrStatic< MAX_OSPATH > canonical = mapName;
		canonical.ToLower();
		
		budStrStatic< MAX_OSPATH > extension;
		canonical.ExtractFileExtension( extension );
		
		budStrStatic< MAX_OSPATH > convertedFileName;
		
		//convertedFileName = "converted/";
		convertedFileName = canonical;
		
		exporter.Write( convertedFileName );
	}
	
	common->SetRefreshOnPrint( false );
}


















CONSOLE_COMMAND( convertMap, "Convert .map file to new map format with polygons instead of brushes ", budCmdSystem::ArgCompletion_MapNameNoJson )
{
	common->SetRefreshOnPrint( true );
	
	if( args.Argc() != 2 )
	{
		common->Printf( "Usage: convertMap <map>\n" );
		return;
	}
	
	budStr filename = args.Argv( 1 );
	if( !filename.Length() )
	{
		return;
	}
	filename.StripFileExtension();
	
	budStr mapName;
	sprintf( mapName, "maps/%s.map", filename.c_str() );
	
	budMapFile map;
	if( map.Parse( mapName, true, false ) )
	{
		map.ConvertToPolygonMeshFormat();
		
		budStrStatic< MAX_OSPATH > canonical = mapName;
		canonical.ToLower();
		
		budStrStatic< MAX_OSPATH > extension;
		canonical.StripFileExtension();
		
		budStrStatic< MAX_OSPATH > convertedFileName;
		
		convertedFileName = canonical;
		convertedFileName += "_converted";
		
		map.Write( convertedFileName, ".map" );
	}
	
	common->SetRefreshOnPrint( false );
}


CONSOLE_COMMAND( convertMapToJSON, "Convert .map file to new map format with polygons instead of brushes ", budCmdSystem::ArgCompletion_MapNameNoJson )
{
	common->SetRefreshOnPrint( true );
	
	if( args.Argc() != 2 )
	{
		common->Printf( "Usage: convertMapToJSON <map>\n" );
		return;
	}
	
	budStr filename = args.Argv( 1 );
	if( !filename.Length() )
	{
		return;
	}
	filename.StripFileExtension();
	
	budStr mapName;
	sprintf( mapName, "maps/%s.map", filename.c_str() );
	
	budMapFile map;
	if( map.Parse( mapName, true, false ) )
	{
		map.ConvertToPolygonMeshFormat();
		
		budStrStatic< MAX_OSPATH > canonical = mapName;
		canonical.ToLower();
		
		budStrStatic< MAX_OSPATH > extension;
		canonical.StripFileExtension();
		
		budStrStatic< MAX_OSPATH > convertedFileName;
		
		convertedFileName = canonical;
		//convertedFileName += "_converted";
		
		map.WriteJSON( convertedFileName, ".json" );
	}
	
	common->SetRefreshOnPrint( false );
}

#endif