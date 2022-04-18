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

#pragma hdrstop
#include "PCH.hpp"


#include "RenderCommon.h"
#include "Model_local.h"

/*

This is a simple dynamic model that just creates a stretched quad between
two points that faces the view, like a dynamic deform tube.

*/

static const char* beam_SnapshotName = "_beam_Snapshot_";

/*
===============
budRenderModelBeam::IsDynamicModel
===============
*/
dynamicModel_t budRenderModelBeam::IsDynamicModel() const
{
	return DM_CONTINUOUS;	// regenerate for every view
}

/*
===============
budRenderModelBeam::IsLoaded
===============
*/
bool budRenderModelBeam::IsLoaded() const
{
	return true;	// don't ever need to load
}

/*
===============
budRenderModelBeam::InstantiateDynamicModel
===============
*/
budRenderModel* budRenderModelBeam::InstantiateDynamicModel( const struct renderEntity_s* renderEntity, const viewDef_t* viewDef, budRenderModel* cachedModel )
{
	budRenderModelStatic* staticModel;
	srfTriangles_t* tri;
	modelSurface_t surf;
	
	if( cachedModel )
	{
		delete cachedModel;
		cachedModel = NULL;
	}
	
	if( renderEntity == NULL || viewDef == NULL )
	{
		delete cachedModel;
		return NULL;
	}
	
	if( cachedModel != NULL )
	{
	
		assert( dynamic_cast<budRenderModelStatic*>( cachedModel ) != NULL );
		assert( String::Icmp( cachedModel->Name(), beam_SnapshotName ) == 0 );
		
		staticModel = static_cast<budRenderModelStatic*>( cachedModel );
		surf = *staticModel->Surface( 0 );
		tri = surf.geometry;
		
	}
	else
	{
	
		staticModel = new( TAG_MODEL ) budRenderModelStatic;
		staticModel->InitEmpty( beam_SnapshotName );
		
		tri = R_AllocStaticTriSurf();
		R_AllocStaticTriSurfVerts( tri, 4 );
		R_AllocStaticTriSurfIndexes( tri, 6 );
		
		tri->verts[0].Clear();
		tri->verts[0].SetTexCoord( 0, 0 );
		
		tri->verts[1].Clear();
		tri->verts[1].SetTexCoord( 0, 1 );
		
		tri->verts[2].Clear();
		tri->verts[2].SetTexCoord( 1, 0 );
		
		tri->verts[3].Clear();
		tri->verts[3].SetTexCoord( 1, 1 );
		
		tri->indexes[0] = 0;
		tri->indexes[1] = 2;
		tri->indexes[2] = 1;
		tri->indexes[3] = 2;
		tri->indexes[4] = 3;
		tri->indexes[5] = 1;
		
		tri->numVerts = 4;
		tri->numIndexes = 6;
		
		surf.geometry = tri;
		surf.id = 0;
		surf.shader = tr.defaultMaterial;
		staticModel->AddSurface( surf );
	}
	
	Vector3	target = *reinterpret_cast<const Vector3*>( &renderEntity->shaderParms[SHADERPARM_BEAM_END_X] );
	
	// we need the view direction to project the minor axis of the tube
	// as the view changes
	Vector3	localView, localTarget;
	float	modelMatrix[16];
	R_AxisToModelMatrix( renderEntity->axis, renderEntity->origin, modelMatrix );
	R_GlobalPointToLocal( modelMatrix, viewDef->renderView.vieworg, localView );
	R_GlobalPointToLocal( modelMatrix, target, localTarget );
	
	Vector3	major = localTarget;
	Vector3	minor;
	
	Vector3	mid = 0.5f * localTarget;
	Vector3	dir = mid - localView;
	minor.Cross( major, dir );
	minor.Normalize();
	if( renderEntity->shaderParms[SHADERPARM_BEAM_WIDTH] != 0.0f )
	{
		minor *= renderEntity->shaderParms[SHADERPARM_BEAM_WIDTH] * 0.5f;
	}
	
	int red		= Math::Ftoi( renderEntity->shaderParms[SHADERPARM_RED] * 255.0f );
	int green	= Math::Ftoi( renderEntity->shaderParms[SHADERPARM_GREEN] * 255.0f );
	int blue	= Math::Ftoi( renderEntity->shaderParms[SHADERPARM_BLUE] * 255.0f );
	int alpha	= Math::Ftoi( renderEntity->shaderParms[SHADERPARM_ALPHA] * 255.0f );
	
	tri->verts[0].xyz = minor;
	tri->verts[0].color[0] = red;
	tri->verts[0].color[1] = green;
	tri->verts[0].color[2] = blue;
	tri->verts[0].color[3] = alpha;
	
	tri->verts[1].xyz = -minor;
	tri->verts[1].color[0] = red;
	tri->verts[1].color[1] = green;
	tri->verts[1].color[2] = blue;
	tri->verts[1].color[3] = alpha;
	
	tri->verts[2].xyz = localTarget + minor;
	tri->verts[2].color[0] = red;
	tri->verts[2].color[1] = green;
	tri->verts[2].color[2] = blue;
	tri->verts[2].color[3] = alpha;
	
	tri->verts[3].xyz = localTarget - minor;
	tri->verts[3].color[0] = red;
	tri->verts[3].color[1] = green;
	tri->verts[3].color[2] = blue;
	tri->verts[3].color[3] = alpha;
	
	R_BoundTriSurf( tri );
	
	staticModel->bounds = tri->bounds;
	
	return staticModel;
}

/*
===============
budRenderModelBeam::Bounds
===============
*/
budBounds budRenderModelBeam::Bounds( const struct renderEntity_s* renderEntity ) const
{
	budBounds	b;
	
	b.Zero();
	if( !renderEntity )
	{
		b.ExpandSelf( 8.0f );
	}
	else
	{
		Vector3	target = *reinterpret_cast<const Vector3*>( &renderEntity->shaderParms[SHADERPARM_BEAM_END_X] );
		Vector3	localTarget;
		float	modelMatrix[16];
		R_AxisToModelMatrix( renderEntity->axis, renderEntity->origin, modelMatrix );
		R_GlobalPointToLocal( modelMatrix, target, localTarget );
		
		b.AddPoint( localTarget );
		if( renderEntity->shaderParms[SHADERPARM_BEAM_WIDTH] != 0.0f )
		{
			b.ExpandSelf( renderEntity->shaderParms[SHADERPARM_BEAM_WIDTH] * 0.5f );
		}
	}
	return b;
}
