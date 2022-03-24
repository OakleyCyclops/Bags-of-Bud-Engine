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

#include "AAS_local.h"
#include "../Game_local.h"		// for cvars and debug drawing


/*
============
budAASLocal::DrawCone
============
*/
void budAASLocal::DrawCone( const budVec3& origin, const budVec3& dir, float radius, const budVec4& color ) const
{
	int i;
	budMat3 axis;
	budVec3 center, top, p, lastp;
	
	axis[2] = dir;
	axis[2].NormalVectors( axis[0], axis[1] );
	axis[1] = -axis[1];
	
	center = origin + dir;
	top = center + dir * ( 3.0f * radius );
	lastp = center + radius * axis[1];
	
	for( i = 20; i <= 360; i += 20 )
	{
		p = center + sin( DEG2RAD( i ) ) * radius * axis[0] + cos( DEG2RAD( i ) ) * radius * axis[1];
		gameRenderWorld->DebugLine( color, lastp, p, 0 );
		gameRenderWorld->DebugLine( color, p, top, 0 );
		lastp = p;
	}
}

/*
============
budAASLocal::DrawReachability
============
*/
void budAASLocal::DrawReachability( const idReachability* reach ) const
{
	gameRenderWorld->DebugArrow( colorCyan, reach->start, reach->end, 2 );
	
	if( gameLocal.GetLocalPlayer() )
	{
		gameRenderWorld->DrawText( va( "%d", reach->edgeNum ), ( reach->start + reach->end ) * 0.5f, 0.1f, colorWhite, gameLocal.GetLocalPlayer()->viewAxis );
	}
	
	switch( reach->travelType )
	{
		case TFL_WALK:
		{
			//const idReachability_Walk *walk = static_cast<const idReachability_Walk *>(reach);
			break;
		}
		default:
		{
			break;
		}
	}
}

/*
============
budAASLocal::DrawEdge
============
*/
void budAASLocal::DrawEdge( int edgeNum, bool arrow ) const
{
	const aasEdge_t* edge;
	budVec4* color;
	
	if( !file )
	{
		return;
	}
	
	edge = &file->GetEdge( edgeNum );
	color = &colorRed;
	if( arrow )
	{
		gameRenderWorld->DebugArrow( *color, file->GetVertex( edge->vertexNum[0] ), file->GetVertex( edge->vertexNum[1] ), 1 );
	}
	else
	{
		gameRenderWorld->DebugLine( *color, file->GetVertex( edge->vertexNum[0] ), file->GetVertex( edge->vertexNum[1] ) );
	}
	
	if( gameLocal.GetLocalPlayer() )
	{
		gameRenderWorld->DrawText( va( "%d", edgeNum ), ( file->GetVertex( edge->vertexNum[0] ) + file->GetVertex( edge->vertexNum[1] ) ) * 0.5f + budVec3( 0, 0, 4 ), 0.1f, colorRed, gameLocal.GetLocalPlayer()->viewAxis );
	}
}

/*
============
budAASLocal::DrawFace
============
*/
void budAASLocal::DrawFace( int faceNum, bool side ) const
{
	int i, j, numEdges, firstEdge;
	const aasFace_t* face;
	budVec3 mid, end;
	
	if( !file )
	{
		return;
	}
	
	face = &file->GetFace( faceNum );
	numEdges = face->numEdges;
	firstEdge = face->firstEdge;
	
	mid = vec3_origin;
	for( i = 0; i < numEdges; i++ )
	{
		DrawEdge( abs( file->GetEdgeIndex( firstEdge + i ) ), ( face->flags & FACE_FLOOR ) != 0 );
		j = file->GetEdgeIndex( firstEdge + i );
		mid += file->GetVertex( file->GetEdge( abs( j ) ).vertexNum[ j < 0 ] );
	}
	
	mid /= numEdges;
	if( side )
	{
		end = mid - 5.0f * file->GetPlane( file->GetFace( faceNum ).planeNum ).Normal();
	}
	else
	{
		end = mid + 5.0f * file->GetPlane( file->GetFace( faceNum ).planeNum ).Normal();
	}
	gameRenderWorld->DebugArrow( colorGreen, mid, end, 1 );
}

/*
============
budAASLocal::DrawArea
============
*/
void budAASLocal::DrawArea( int areaNum ) const
{
	int i, numFaces, firstFace;
	const aasArea_t* area;
	idReachability* reach;
	
	if( !file )
	{
		return;
	}
	
	area = &file->GetArea( areaNum );
	numFaces = area->numFaces;
	firstFace = area->firstFace;
	
	for( i = 0; i < numFaces; i++ )
	{
		DrawFace( abs( file->GetFaceIndex( firstFace + i ) ), file->GetFaceIndex( firstFace + i ) < 0 );
	}
	
	for( reach = area->reach; reach; reach = reach->next )
	{
		DrawReachability( reach );
	}
}

/*
============
budAASLocal::DefaultSearchBounds
============
*/
const budBounds& budAASLocal::DefaultSearchBounds() const
{
	return file->GetSettings().boundingBoxes[0];
}

/*
============
budAASLocal::ShowArea
============
*/
void budAASLocal::ShowArea( const budVec3& origin ) const
{
	static int lastAreaNum;
	int areaNum;
	const aasArea_t* area;
	budVec3 org;
	
	areaNum = PointReachableAreaNum( origin, DefaultSearchBounds(), ( AREA_REACHABLE_WALK | AREA_REACHABLE_FLY ) );
	org = origin;
	PushPointIntoAreaNum( areaNum, org );
	
	if( aas_goalArea.GetInteger() )
	{
		int travelTime;
		idReachability* reach;
		
		RouteToGoalArea( areaNum, org, aas_goalArea.GetInteger(), TFL_WALK | TFL_AIR, travelTime, &reach );
		gameLocal.Printf( "\rtt = %4d", travelTime );
		if( reach )
		{
			gameLocal.Printf( " to area %4d", reach->toAreaNum );
			DrawArea( reach->toAreaNum );
		}
	}
	
	if( areaNum != lastAreaNum )
	{
		area = &file->GetArea( areaNum );
		gameLocal.Printf( "area %d: ", areaNum );
		if( area->flags & AREA_LEDGE )
		{
			gameLocal.Printf( "AREA_LEDGE " );
		}
		if( area->flags & AREA_REACHABLE_WALK )
		{
			gameLocal.Printf( "AREA_REACHABLE_WALK " );
		}
		if( area->flags & AREA_REACHABLE_FLY )
		{
			gameLocal.Printf( "AREA_REACHABLE_FLY " );
		}
		if( area->contents & AREACONTENTS_CLUSTERPORTAL )
		{
			gameLocal.Printf( "AREACONTENTS_CLUSTERPORTAL " );
		}
		if( area->contents & AREACONTENTS_OBSTACLE )
		{
			gameLocal.Printf( "AREACONTENTS_OBSTACLE " );
		}
		gameLocal.Printf( "\n" );
		lastAreaNum = areaNum;
	}
	
	if( org != origin )
	{
		budBounds bnds = file->GetSettings().boundingBoxes[ 0 ];
		bnds[ 1 ].z = bnds[ 0 ].z;
		gameRenderWorld->DebugBounds( colorYellow, bnds, org );
	}
	
	DrawArea( areaNum );
}

/*
============
budAASLocal::ShowWalkPath
============
*/
void budAASLocal::ShowWalkPath( const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin ) const
{
	int i, areaNum, curAreaNum, travelTime;
	idReachability* reach;
	budVec3 org, areaCenter;
	aasPath_t path;
	
	if( !file )
	{
		return;
	}
	
	org = origin;
	areaNum = PointReachableAreaNum( org, DefaultSearchBounds(), AREA_REACHABLE_WALK );
	PushPointIntoAreaNum( areaNum, org );
	curAreaNum = areaNum;
	
	for( i = 0; i < 100; i++ )
	{
	
		if( !RouteToGoalArea( curAreaNum, org, goalAreaNum, TFL_WALK | TFL_AIR, travelTime, &reach ) )
		{
			break;
		}
		
		if( !reach )
		{
			break;
		}
		
		gameRenderWorld->DebugArrow( colorGreen, org, reach->start, 2 );
		DrawReachability( reach );
		
		if( reach->toAreaNum == goalAreaNum )
		{
			break;
		}
		
		curAreaNum = reach->toAreaNum;
		org = reach->end;
	}
	
	if( WalkPathToGoal( path, areaNum, origin, goalAreaNum, goalOrigin, TFL_WALK | TFL_AIR ) )
	{
		gameRenderWorld->DebugArrow( colorBlue, origin, path.moveGoal, 2 );
	}
}

/*
============
budAASLocal::ShowFlyPath
============
*/
void budAASLocal::ShowFlyPath( const budVec3& origin, int goalAreaNum, const budVec3& goalOrigin ) const
{
	int i, areaNum, curAreaNum, travelTime;
	idReachability* reach;
	budVec3 org, areaCenter;
	aasPath_t path;
	
	if( !file )
	{
		return;
	}
	
	org = origin;
	areaNum = PointReachableAreaNum( org, DefaultSearchBounds(), AREA_REACHABLE_FLY );
	PushPointIntoAreaNum( areaNum, org );
	curAreaNum = areaNum;
	
	for( i = 0; i < 100; i++ )
	{
	
		if( !RouteToGoalArea( curAreaNum, org, goalAreaNum, TFL_WALK | TFL_FLY | TFL_AIR, travelTime, &reach ) )
		{
			break;
		}
		
		if( !reach )
		{
			break;
		}
		
		gameRenderWorld->DebugArrow( colorPurple, org, reach->start, 2 );
		DrawReachability( reach );
		
		if( reach->toAreaNum == goalAreaNum )
		{
			break;
		}
		
		curAreaNum = reach->toAreaNum;
		org = reach->end;
	}
	
	if( FlyPathToGoal( path, areaNum, origin, goalAreaNum, goalOrigin, TFL_WALK | TFL_FLY | TFL_AIR ) )
	{
		gameRenderWorld->DebugArrow( colorBlue, origin, path.moveGoal, 2 );
	}
}

/*
============
budAASLocal::ShowWallEdges
============
*/
void budAASLocal::ShowWallEdges( const budVec3& origin ) const
{
	int i, areaNum, numEdges, edges[1024];
	budVec3 start, end;
	idPlayer* player;
	
	player = gameLocal.GetLocalPlayer();
	if( !player )
	{
		return;
	}
	
	areaNum = PointReachableAreaNum( origin, DefaultSearchBounds(), ( AREA_REACHABLE_WALK | AREA_REACHABLE_FLY ) );
	numEdges = GetWallEdges( areaNum, budBounds( origin ).Expand( 256.0f ), TFL_WALK, edges, 1024 );
	for( i = 0; i < numEdges; i++ )
	{
		GetEdge( edges[i], start, end );
		gameRenderWorld->DebugLine( colorRed, start, end );
		gameRenderWorld->DrawText( va( "%d", edges[i] ), ( start + end ) * 0.5f, 0.1f, colorWhite, player->viewAxis );
	}
}

/*
============
budAASLocal::ShowHideArea
============
*/
void budAASLocal::ShowHideArea( const budVec3& origin, int targetAreaNum ) const
{
	int areaNum, numObstacles;
	budVec3 target;
	aasGoal_t goal;
	aasObstacle_t obstacles[10];
	
	areaNum = PointReachableAreaNum( origin, DefaultSearchBounds(), ( AREA_REACHABLE_WALK | AREA_REACHABLE_FLY ) );
	target = AreaCenter( targetAreaNum );
	
	// consider the target an obstacle
	obstacles[0].absBounds = budBounds( target ).Expand( 16 );
	numObstacles = 1;
	
	DrawCone( target, budVec3( 0, 0, 1 ), 16.0f, colorYellow );
	
	budAASFindCover findCover( target );
	if( FindNearestGoal( goal, areaNum, origin, target, TFL_WALK | TFL_AIR, obstacles, numObstacles, findCover ) )
	{
		DrawArea( goal.areaNum );
		ShowWalkPath( origin, goal.areaNum, goal.origin );
		DrawCone( goal.origin, budVec3( 0, 0, 1 ), 16.0f, colorWhite );
	}
}

/*
============
budAASLocal::PullPlayer
============
*/
bool budAASLocal::PullPlayer( const budVec3& origin, int toAreaNum ) const
{
	int areaNum;
	budVec3 areaCenter, dir, vel;
	budAngles delta;
	aasPath_t path;
	idPlayer* player;
	
	player = gameLocal.GetLocalPlayer();
	if( !player )
	{
		return true;
	}
	
	idPhysics* physics = player->GetPhysics();
	if( !physics )
	{
		return true;
	}
	
	if( !toAreaNum )
	{
		return false;
	}
	
	areaNum = PointReachableAreaNum( origin, DefaultSearchBounds(), ( AREA_REACHABLE_WALK | AREA_REACHABLE_FLY ) );
	areaCenter = AreaCenter( toAreaNum );
	if( player->GetPhysics()->GetAbsBounds().Expand( 8 ).ContainsPoint( areaCenter ) )
	{
		return false;
	}
	if( WalkPathToGoal( path, areaNum, origin, toAreaNum, areaCenter, TFL_WALK | TFL_AIR ) )
	{
		dir = path.moveGoal - origin;
		dir[2] *= 0.5f;
		dir.Normalize();
		delta = dir.ToAngles() - player->cmdAngles - player->GetDeltaViewAngles();
		delta.Normalize180();
		player->SetDeltaViewAngles( player->GetDeltaViewAngles() + delta * 0.1f );
		dir[2] = 0.0f;
		dir.Normalize();
		dir *= 100.0f;
		vel = physics->GetLinearVelocity();
		dir[2] = vel[2];
		physics->SetLinearVelocity( dir );
		return true;
	}
	else
	{
		return false;
	}
}

/*
============
budAASLocal::RandomPullPlayer
============
*/
void budAASLocal::RandomPullPlayer( const budVec3& origin ) const
{
	int rnd, i, n;
	
	if( !PullPlayer( origin, aas_pullPlayer.GetInteger() ) )
	{
	
		rnd = gameLocal.random.RandomFloat() * file->GetNumAreas();
		
		for( i = 0; i < file->GetNumAreas(); i++ )
		{
			n = ( rnd + i ) % file->GetNumAreas();
			if( file->GetArea( n ).flags & ( AREA_REACHABLE_WALK | AREA_REACHABLE_FLY ) )
			{
				aas_pullPlayer.SetInteger( n );
			}
		}
	}
	else
	{
		ShowWalkPath( origin, aas_pullPlayer.GetInteger(), AreaCenter( aas_pullPlayer.GetInteger() ) );
	}
}

/*
============
budAASLocal::ShowPushIntoArea
============
*/
void budAASLocal::ShowPushIntoArea( const budVec3& origin ) const
{
	int areaNum;
	budVec3 target;
	
	target = origin;
	areaNum = PointReachableAreaNum( target, DefaultSearchBounds(), ( AREA_REACHABLE_WALK | AREA_REACHABLE_FLY ) );
	PushPointIntoAreaNum( areaNum, target );
	gameRenderWorld->DebugArrow( colorGreen, origin, target, 1 );
}

/*
============
budAASLocal::Test
============
*/
void budAASLocal::Test( const budVec3& origin )
{

	if( !file )
	{
		return;
	}
	
	if( aas_randomPullPlayer.GetBool() )
	{
		RandomPullPlayer( origin );
	}
	if( ( aas_pullPlayer.GetInteger() > 0 ) && ( aas_pullPlayer.GetInteger() < file->GetNumAreas() ) )
	{
		ShowWalkPath( origin, aas_pullPlayer.GetInteger(), AreaCenter( aas_pullPlayer.GetInteger() ) );
		PullPlayer( origin, aas_pullPlayer.GetInteger() );
	}
	if( ( aas_showPath.GetInteger() > 0 ) && ( aas_showPath.GetInteger() < file->GetNumAreas() ) )
	{
		ShowWalkPath( origin, aas_showPath.GetInteger(), AreaCenter( aas_showPath.GetInteger() ) );
	}
	if( ( aas_showFlyPath.GetInteger() > 0 ) && ( aas_showFlyPath.GetInteger() < file->GetNumAreas() ) )
	{
		ShowFlyPath( origin, aas_showFlyPath.GetInteger(), AreaCenter( aas_showFlyPath.GetInteger() ) );
	}
	if( ( aas_showHideArea.GetInteger() > 0 ) && ( aas_showHideArea.GetInteger() < file->GetNumAreas() ) )
	{
		ShowHideArea( origin, aas_showHideArea.GetInteger() );
	}
	if( aas_showAreas.GetBool() )
	{
		ShowArea( origin );
	}
	if( aas_showWallEdges.GetBool() )
	{
		ShowWallEdges( origin );
	}
	if( aas_showPushIntoArea.GetBool() )
	{
		ShowPushIntoArea( origin );
	}
}
