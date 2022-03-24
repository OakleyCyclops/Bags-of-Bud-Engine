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
#ifndef __SHADOWSHARED_H__
#define __SHADOWSHARED_H__

enum shadowVolumeState_t
{
	SHADOWVOLUME_DONE		= 0,
	SHADOWVOLUME_UNFINISHED	= 1
};

// this extra stretch should also make the "inside shadow volume test" valid for both eyes of a stereo view
const float INSIDE_SHADOW_VOLUME_EXTRA_STRETCH = 4.0f;	// in theory, should vary with FOV

#define TEMP_ROUND4( x )				( ( x + 3 ) & ~3 )	// round up to a multiple of 4 for SIMD
#define TEMP_CULLBITS( numVerts )		TEMP_ROUND4( numVerts )

bool R_ViewPotentiallyInsideInfiniteShadowVolume( const budBounds& occluderBounds, const budVec3& localLight, const budVec3& localView, const float znear );

bool R_LineIntersectsTriangleExpandedWithSphere( const budVec3& lineStart, const budVec3& lineEnd, const budVec3& lineDir, const float lineLength,
		const float sphereRadius, const budVec3& triVert0, const budVec3& triVert1, const budVec3& triVert2 );
bool R_ViewInsideShadowVolume( byte* cullBits, const idShadowVert* verts, int numVerts, const triIndex_t* indexes, int numIndexes,
							   const budVec3& localLightOrigin, const budVec3& localViewOrigin, const float zNear );

#endif // !__SHADOWSHARED_H__