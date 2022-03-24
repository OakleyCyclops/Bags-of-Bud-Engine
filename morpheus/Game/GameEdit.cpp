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

#include "PCH.hpp"
#pragma hdrstop

#include "Game_local.h"


/*
===============================================================================

	Ingame cursor.

===============================================================================
*/

CLASS_DECLARATION( idEntity, idCursor3D )
END_CLASS

/*
===============
idCursor3D::idCursor3D
===============
*/
idCursor3D::idCursor3D()
{
	draggedPosition.Zero();
}

/*
===============
idCursor3D::~idCursor3D
===============
*/
idCursor3D::~idCursor3D()
{
}

/*
===============
idCursor3D::Spawn
===============
*/
void idCursor3D::Spawn()
{
}

/*
===============
idCursor3D::Present
===============
*/
void idCursor3D::Present()
{
	// don't present to the renderer if the entity hasn't changed
	if( !( thinkFlags & TH_UPDATEVISUALS ) )
	{
		return;
	}
	BecomeInactive( TH_UPDATEVISUALS );
	
	const budVec3& origin = GetPhysics()->GetOrigin();
	const budMat3& axis = GetPhysics()->GetAxis();
	gameRenderWorld->DebugArrow( colorYellow, origin + axis[1] * -5.0f + axis[2] * 5.0f, origin, 2 );
	gameRenderWorld->DebugArrow( colorRed, origin, draggedPosition, 2 );
}

/*
===============
idCursor3D::Think
===============
*/
void idCursor3D::Think()
{
	if( thinkFlags & TH_THINK )
	{
		drag.Evaluate( gameLocal.time );
	}
	Present();
}


/*
===============================================================================

	Allows entities to be dragged through the world with physics.

===============================================================================
*/

#define MAX_DRAG_TRACE_DISTANCE			2048.0f

/*
==============
idDragEntity::idDragEntity
==============
*/
idDragEntity::idDragEntity()
{
	cursor = NULL;
	Clear();
}

/*
==============
idDragEntity::~idDragEntity
==============
*/
idDragEntity::~idDragEntity()
{
	StopDrag();
	selected = NULL;
	delete cursor;
	cursor = NULL;
}


/*
==============
idDragEntity::Clear
==============
*/
void idDragEntity::Clear()
{
	dragEnt			= NULL;
	joint			= INVALID_JOINT;
	id				= 0;
	localEntityPoint.Zero();
	localPlayerPoint.Zero();
	bodyName.Clear();
	selected		= NULL;
}

/*
==============
idDragEntity::StopDrag
==============
*/
void idDragEntity::StopDrag()
{
	dragEnt = NULL;
	if( cursor )
	{
		cursor->BecomeInactive( TH_THINK );
	}
}

/*
==============
idDragEntity::Update
==============
*/
void idDragEntity::Update( idPlayer* player )
{
	budVec3 viewPoint, origin;
	budMat3 viewAxis, axis;
	trace_t trace;
	idEntity* newEnt = NULL;
	budAngles angles;
	jointHandle_t newJoint = INVALID_JOINT;
	budStr newBodyName;
	
	player->GetViewPos( viewPoint, viewAxis );
	
	// if no entity selected for dragging
	if( !dragEnt.GetEntity() )
	{
	
		if( player->usercmd.buttons & BUTTON_ATTACK )
		{
		
			gameLocal.clip.TracePoint( trace, viewPoint, viewPoint + viewAxis[0] * MAX_DRAG_TRACE_DISTANCE, ( CONTENTS_SOLID | CONTENTS_RENDERMODEL | CONTENTS_BODY ), player );
			if( trace.fraction < 1.0f )
			{
			
				newEnt = gameLocal.entities[ trace.c.entityNum ];
				if( newEnt )
				{
				
					if( newEnt->GetBindMaster() )
					{
						if( newEnt->GetBindJoint() )
						{
							trace.c.id = JOINT_HANDLE_TO_CLIPMODEL_ID( newEnt->GetBindJoint() );
						}
						else
						{
							trace.c.id = newEnt->GetBindBody();
						}
						newEnt = newEnt->GetBindMaster();
					}
					
					if( newEnt->IsType( budAFEntity_Base::Type ) && static_cast<budAFEntity_Base*>( newEnt )->IsActiveAF() )
					{
						budAFEntity_Base* af = static_cast<budAFEntity_Base*>( newEnt );
						
						// joint being dragged
						newJoint = CLIPMODEL_ID_TO_JOINT_HANDLE( trace.c.id );
						// get the body id from the trace model id which might be a joint handle
						trace.c.id = af->BodyForClipModelId( trace.c.id );
						// get the name of the body being dragged
						newBodyName = af->GetAFPhysics()->GetBody( trace.c.id )->GetName();
						
					}
					else if( !newEnt->IsType( idWorldspawn::Type ) )
					{
					
						if( trace.c.id < 0 )
						{
							newJoint = CLIPMODEL_ID_TO_JOINT_HANDLE( trace.c.id );
						}
						else
						{
							newJoint = INVALID_JOINT;
						}
						newBodyName = "";
						
					}
					else
					{
					
						newJoint = INVALID_JOINT;
						newEnt = NULL;
					}
				}
				if( newEnt )
				{
					dragEnt = newEnt;
					selected = newEnt;
					joint = newJoint;
					id = trace.c.id;
					bodyName = newBodyName;
					
					if( !cursor )
					{
						cursor = ( idCursor3D* )gameLocal.SpawnEntityType( idCursor3D::Type );
					}
					
					idPhysics* phys = dragEnt.GetEntity()->GetPhysics();
					localPlayerPoint = ( trace.c.point - viewPoint ) * viewAxis.Transpose();
					origin = phys->GetOrigin( id );
					axis = phys->GetAxis( id );
					localEntityPoint = ( trace.c.point - origin ) * axis.Transpose();
					
					cursor->drag.Init( g_dragDamping.GetFloat() );
					cursor->drag.SetPhysics( phys, id, localEntityPoint );
					cursor->Show();
					
					if( phys->IsType( idPhysics_AF::Type ) ||
							phys->IsType( idPhysics_RigidBody::Type ) ||
							phys->IsType( idPhysics_Monster::Type ) )
					{
						cursor->BecomeActive( TH_THINK );
					}
				}
			}
		}
	}
	
	// if there is an entity selected for dragging
	idEntity* drag = dragEnt.GetEntity();
	if( drag )
	{
	
		if( !( player->usercmd.buttons & BUTTON_ATTACK ) )
		{
			StopDrag();
			return;
		}
		
		cursor->SetOrigin( viewPoint + localPlayerPoint * viewAxis );
		cursor->SetAxis( viewAxis );
		
		cursor->drag.SetDragPosition( cursor->GetPhysics()->GetOrigin() );
		
		renderEntity_t* renderEntity = drag->GetRenderEntity();
		budAnimator* dragAnimator = drag->GetAnimator();
		
		if( joint != INVALID_JOINT && renderEntity != NULL && dragAnimator != NULL )
		{
			dragAnimator->GetJointTransform( joint, gameLocal.time, cursor->draggedPosition, axis );
			cursor->draggedPosition = renderEntity->origin + cursor->draggedPosition * renderEntity->axis;
			gameRenderWorld->DrawText( va( "%s\n%s\n%s, %s", drag->GetName(), drag->GetType()->classname, dragAnimator->GetJointName( joint ), bodyName.c_str() ), cursor->GetPhysics()->GetOrigin(), 0.1f, colorWhite, viewAxis, 1 );
		}
		else
		{
			cursor->draggedPosition = cursor->GetPhysics()->GetOrigin();
			gameRenderWorld->DrawText( va( "%s\n%s\n%s", drag->GetName(), drag->GetType()->classname, bodyName.c_str() ), cursor->GetPhysics()->GetOrigin(), 0.1f, colorWhite, viewAxis, 1 );
		}
	}
	
	// if there is a selected entity
	if( selected.GetEntity() && g_dragShowSelection.GetBool() )
	{
		// draw the bbox of the selected entity
		renderEntity_t* renderEntity = selected.GetEntity()->GetRenderEntity();
		if( renderEntity )
		{
			gameRenderWorld->DebugBox( colorYellow, budBox( renderEntity->bounds, renderEntity->origin, renderEntity->axis ) );
		}
	}
}

/*
==============
idDragEntity::SetSelected
==============
*/
void idDragEntity::SetSelected( idEntity* ent )
{
	selected = ent;
	StopDrag();
}

/*
==============
idDragEntity::DeleteSelected
==============
*/
void idDragEntity::DeleteSelected()
{
	delete selected.GetEntity();
	selected = NULL;
	StopDrag();
}

/*
==============
idDragEntity::BindSelected
==============
*/
void idDragEntity::BindSelected()
{
	int num, largestNum;
	budLexer lexer;
	budToken type, bodyName;
	budStr key, value, bindBodyName;
	const idKeyValue* kv;
	budAFEntity_Base* af;
	
	af = static_cast<budAFEntity_Base*>( dragEnt.GetEntity() );
	
	if( !af || !af->IsType( budAFEntity_Base::Type ) || !af->IsActiveAF() )
	{
		return;
	}
	
	bindBodyName = af->GetAFPhysics()->GetBody( id )->GetName();
	largestNum = 1;
	
	// parse all the bind constraints
	kv = af->spawnArgs.MatchPrefix( "bindConstraint ", NULL );
	while( kv )
	{
		key = kv->GetKey();
		key.Strip( "bindConstraint " );
		if( sscanf( key, "bind%d", &num ) )
		{
			if( num >= largestNum )
			{
				largestNum = num + 1;
			}
		}
		
		lexer.LoadMemory( kv->GetValue(), kv->GetValue().Length(), kv->GetKey() );
		lexer.ReadToken( &type );
		lexer.ReadToken( &bodyName );
		lexer.FreeSource();
		
		// if there already exists a bind constraint for this body
		if( bodyName.Icmp( bindBodyName ) == 0 )
		{
			// delete the bind constraint
			af->spawnArgs.Delete( kv->GetKey() );
			kv = NULL;
		}
		
		kv = af->spawnArgs.MatchPrefix( "bindConstraint ", kv );
	}
	
	sprintf( key, "bindConstraint bind%d", largestNum );
	sprintf( value, "ballAndSocket %s %s", bindBodyName.c_str(), af->GetAnimator()->GetJointName( joint ) );
	
	af->spawnArgs.Set( key, value );
	af->spawnArgs.Set( "bind", "worldspawn" );
	af->Bind( gameLocal.world, true );
}

/*
==============
idDragEntity::UnbindSelected
==============
*/
void idDragEntity::UnbindSelected()
{
	const idKeyValue* kv;
	budAFEntity_Base* af;
	
	af = static_cast<budAFEntity_Base*>( selected.GetEntity() );
	
	if( !af || !af->IsType( budAFEntity_Base::Type ) || !af->IsActiveAF() )
	{
		return;
	}
	
	// unbind the selected entity
	af->Unbind();
	
	// delete all the bind constraints
	kv = selected.GetEntity()->spawnArgs.MatchPrefix( "bindConstraint ", NULL );
	while( kv )
	{
		selected.GetEntity()->spawnArgs.Delete( kv->GetKey() );
		kv = selected.GetEntity()->spawnArgs.MatchPrefix( "bindConstraint ", NULL );
	}
	
	// delete any bind information
	af->spawnArgs.Delete( "bind" );
	af->spawnArgs.Delete( "bindToJoint" );
	af->spawnArgs.Delete( "bindToBody" );
}


/*
===============================================================================

	Handles ingame entity editing.

===============================================================================
*/

/*
==============
idEditEntities::idEditEntities
==============
*/
idEditEntities::idEditEntities()
{
	selectableEntityClasses.Clear();
	nextSelectTime = 0;
}

/*
=============
idEditEntities::SelectEntity
=============
*/
bool idEditEntities::SelectEntity( const budVec3& origin, const budVec3& dir, const idEntity* skip )
{
	budVec3		end;
	idEntity*	ent;
	
	if( !g_editEntityMode.GetInteger() || selectableEntityClasses.Num() == 0 )
	{
		return false;
	}
	
	if( gameLocal.time < nextSelectTime )
	{
		return true;
	}
	nextSelectTime = gameLocal.time + 300;
	
	end = origin + dir * 4096.0f;
	
	ent = NULL;
	for( int i = 0; i < selectableEntityClasses.Num(); i++ )
	{
		ent = gameLocal.FindTraceEntity( origin, end, *selectableEntityClasses[i].typeInfo, skip );
		if( ent )
		{
			break;
		}
	}
	if( ent )
	{
		ClearSelectedEntities();
		if( EntityIsSelectable( ent ) )
		{
			AddSelectedEntity( ent );
			gameLocal.Printf( "entity #%d: %s '%s'\n", ent->entityNumber, ent->GetClassname(), ent->name.c_str() );
			ent->ShowEditingDialog();
			return true;
		}
	}
	return false;
}

/*
=============
idEditEntities::AddSelectedEntity
=============
*/
void idEditEntities::AddSelectedEntity( idEntity* ent )
{
	ent->fl.selected = true;
	selectedEntities.AddUnique( ent );
}

/*
==============
idEditEntities::RemoveSelectedEntity
==============
*/
void idEditEntities::RemoveSelectedEntity( idEntity* ent )
{
	if( selectedEntities.Find( ent ) )
	{
		selectedEntities.Remove( ent );
	}
}

/*
=============
idEditEntities::ClearSelectedEntities
=============
*/
void idEditEntities::ClearSelectedEntities()
{
	int i, count;
	
	count = selectedEntities.Num();
	for( i = 0; i < count; i++ )
	{
		selectedEntities[i]->fl.selected = false;
	}
	selectedEntities.Clear();
}


/*
=============
idEditEntities::EntityIsSelectable
=============
*/
bool idEditEntities::EntityIsSelectable( idEntity* ent, budVec4* color, budStr* text )
{
	for( int i = 0; i < selectableEntityClasses.Num(); i++ )
	{
		if( ent->GetType() == selectableEntityClasses[i].typeInfo )
		{
			if( text )
			{
				*text = selectableEntityClasses[i].textKey;
			}
			if( color )
			{
				if( ent->fl.selected )
				{
					*color = colorRed;
				}
				else
				{
					switch( i )
					{
						case 1 :
							*color = colorYellow;
							break;
						case 2 :
							*color = colorBlue;
							break;
						default:
							*color = colorGreen;
					}
				}
			}
			return true;
		}
	}
	return false;
}

/*
=============
idEditEntities::DisplayEntities
=============
*/
void idEditEntities::DisplayEntities()
{
	idEntity* ent;
	
	if( !gameLocal.GetLocalPlayer() )
	{
		return;
	}
	
	selectableEntityClasses.Clear();
	selectedTypeInfo_t sit;
	
	switch( g_editEntityMode.GetInteger() )
	{
		case 1:
			sit.typeInfo = &idLight::Type;
			sit.textKey = "texture";
			selectableEntityClasses.Append( sit );
			break;
		case 2:
			sit.typeInfo = &idSound::Type;
			sit.textKey = "s_shader";
			selectableEntityClasses.Append( sit );
			sit.typeInfo = &idLight::Type;
			sit.textKey = "texture";
			selectableEntityClasses.Append( sit );
			break;
		case 3:
			sit.typeInfo = &budAFEntity_Base::Type;
			sit.textKey = "articulatedFigure";
			selectableEntityClasses.Append( sit );
			break;
		case 4:
			sit.typeInfo = &idFuncEmitter::Type;
			sit.textKey = "model";
			selectableEntityClasses.Append( sit );
			break;
		case 5:
			sit.typeInfo = &budAI::Type;
			sit.textKey = "name";
			selectableEntityClasses.Append( sit );
			break;
		case 6:
			sit.typeInfo = &idEntity::Type;
			sit.textKey = "name";
			selectableEntityClasses.Append( sit );
			break;
		case 7:
			sit.typeInfo = &idEntity::Type;
			sit.textKey = "model";
			selectableEntityClasses.Append( sit );
			break;
		default:
			return;
	}
	
	budBounds viewBounds( gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin() );
	budBounds viewTextBounds( gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin() );
	budMat3 axis = gameLocal.GetLocalPlayer()->viewAngles.ToMat3();
	
	viewBounds.ExpandSelf( 512 );
	viewTextBounds.ExpandSelf( 128 );
	
	budStr textKey;
	
	for( ent = gameLocal.spawnedEntities.Next(); ent != NULL; ent = ent->spawnNode.Next() )
	{
	
		budVec4 color;
		
		textKey = "";
		if( !EntityIsSelectable( ent, &color, &textKey ) )
		{
			continue;
		}
		
		bool drawArrows = false;
		if( ent->GetType() == &budAFEntity_Base::Type )
		{
			if( !static_cast<budAFEntity_Base*>( ent )->IsActiveAF() )
			{
				continue;
			}
		}
		else if( ent->GetType() == &idSound::Type )
		{
			if( ent->fl.selected )
			{
				drawArrows = true;
			}
			const idSoundShader* ss = declManager->FindSound( ent->spawnArgs.GetString( textKey ) );
			if( ss->HasDefaultSound() || ss->base->GetState() == DS_DEFAULTED )
			{
				color.Set( 1.0f, 0.0f, 1.0f, 1.0f );
			}
		}
		else if( ent->GetType() == &idFuncEmitter::Type )
		{
			if( ent->fl.selected )
			{
				drawArrows = true;
			}
		}
		
		if( !viewBounds.ContainsPoint( ent->GetPhysics()->GetOrigin() ) )
		{
			continue;
		}
		
		gameRenderWorld->DebugBounds( color, budBounds( ent->GetPhysics()->GetOrigin() ).Expand( 8 ) );
		if( drawArrows )
		{
			budVec3 start = ent->GetPhysics()->GetOrigin();
			budVec3 end = start + budVec3( 1, 0, 0 ) * 20.0f;
			gameRenderWorld->DebugArrow( colorWhite, start, end, 2 );
			gameRenderWorld->DrawText( "x+", end + budVec3( 4, 0, 0 ), 0.15f, colorWhite, axis );
			end = start + budVec3( 1, 0, 0 ) * -20.0f;
			gameRenderWorld->DebugArrow( colorWhite, start, end, 2 );
			gameRenderWorld->DrawText( "x-", end + budVec3( -4, 0, 0 ), 0.15f, colorWhite, axis );
			end = start + budVec3( 0, 1, 0 ) * +20.0f;
			gameRenderWorld->DebugArrow( colorGreen, start, end, 2 );
			gameRenderWorld->DrawText( "y+", end + budVec3( 0, 4, 0 ), 0.15f, colorWhite, axis );
			end = start + budVec3( 0, 1, 0 ) * -20.0f;
			gameRenderWorld->DebugArrow( colorGreen, start, end, 2 );
			gameRenderWorld->DrawText( "y-", end + budVec3( 0, -4, 0 ), 0.15f, colorWhite, axis );
			end = start + budVec3( 0, 0, 1 ) * +20.0f;
			gameRenderWorld->DebugArrow( colorBlue, start, end, 2 );
			gameRenderWorld->DrawText( "z+", end + budVec3( 0, 0, 4 ), 0.15f, colorWhite, axis );
			end = start + budVec3( 0, 0, 1 ) * -20.0f;
			gameRenderWorld->DebugArrow( colorBlue, start, end, 2 );
			gameRenderWorld->DrawText( "z-", end + budVec3( 0, 0, -4 ), 0.15f, colorWhite, axis );
		}
		
		if( textKey.Length() )
		{
			const char* text = ent->spawnArgs.GetString( textKey );
			if( viewTextBounds.ContainsPoint( ent->GetPhysics()->GetOrigin() ) )
			{
				gameRenderWorld->DrawText( text, ent->GetPhysics()->GetOrigin() + budVec3( 0, 0, 12 ), 0.25, colorWhite, axis, 1 );
			}
		}
	}
}


/*
===============================================================================

	budGameEdit

===============================================================================
*/

budGameEdit			gameEditLocal;
budGameEdit* 		gameEdit = &gameEditLocal;


/*
=============
budGameEdit::GetSelectedEntities
=============
*/
int budGameEdit::GetSelectedEntities( idEntity* list[], int max )
{
	int num = 0;
	idEntity* ent;
	
	for( ent = gameLocal.spawnedEntities.Next(); ent != NULL; ent = ent->spawnNode.Next() )
	{
		if( ent->fl.selected )
		{
			list[num++] = ent;
			if( num >= max )
			{
				break;
			}
		}
	}
	return num;
}

/*
=============
budGameEdit::TriggerSelected
=============
*/
void budGameEdit::TriggerSelected()
{
	idEntity* ent;
	for( ent = gameLocal.spawnedEntities.Next(); ent != NULL; ent = ent->spawnNode.Next() )
	{
		if( ent->fl.selected )
		{
			ent->ProcessEvent( &EV_Activate, gameLocal.GetLocalPlayer() );
		}
	}
}

/*
================
budGameEdit::ClearEntitySelection
================
*/
void budGameEdit::ClearEntitySelection()
{
	idEntity* ent;
	
	for( ent = gameLocal.spawnedEntities.Next(); ent != NULL; ent = ent->spawnNode.Next() )
	{
		ent->fl.selected = false;
	}
	gameLocal.editEntities->ClearSelectedEntities();
}

/*
================
budGameEdit::AddSelectedEntity
================
*/
void budGameEdit::AddSelectedEntity( idEntity* ent )
{
	if( ent )
	{
		gameLocal.editEntities->AddSelectedEntity( ent );
	}
}

/*
================
budGameEdit::FindEntityDefDict
================
*/
const idDict* budGameEdit::FindEntityDefDict( const char* name, bool makeDefault ) const
{
	return gameLocal.FindEntityDefDict( name, makeDefault );
}

/*
================
budGameEdit::SpawnEntityDef
================
*/
void budGameEdit::SpawnEntityDef( const idDict& args, idEntity** ent )
{
	gameLocal.SpawnEntityDef( args, ent );
}

/*
================
budGameEdit::FindEntity
================
*/
idEntity* budGameEdit::FindEntity( const char* name ) const
{
	return gameLocal.FindEntity( name );
}

/*
=============
budGameEdit::GetUniqueEntityName

generates a unique name for a given classname
=============
*/
const char* budGameEdit::GetUniqueEntityName( const char* classname ) const
{
	int			id;
	static char	name[1024];
	
	// can only have MAX_GENTITIES, so if we have a spot available, we're guaranteed to find one
	for( id = 0; id < MAX_GENTITIES; id++ )
	{
		budStr::snPrintf( name, sizeof( name ), "%s_%d", classname, id );
		if( !gameLocal.FindEntity( name ) )
		{
			return name;
		}
	}
	
	// id == MAX_GENTITIES + 1, which can't be in use if we get here
	budStr::snPrintf( name, sizeof( name ), "%s_%d", classname, id );
	return name;
}

/*
================
budGameEdit::EntityGetOrigin
================
*/
void  budGameEdit::EntityGetOrigin( idEntity* ent, budVec3& org ) const
{
	if( ent )
	{
		org = ent->GetPhysics()->GetOrigin();
	}
}

/*
================
budGameEdit::EntityGetAxis
================
*/
void budGameEdit::EntityGetAxis( idEntity* ent, budMat3& axis ) const
{
	if( ent )
	{
		axis = ent->GetPhysics()->GetAxis();
	}
}

/*
================
budGameEdit::EntitySetOrigin
================
*/
void budGameEdit::EntitySetOrigin( idEntity* ent, const budVec3& org )
{
	if( ent )
	{
		ent->SetOrigin( org );
	}
}

/*
================
budGameEdit::EntitySetAxis
================
*/
void budGameEdit::EntitySetAxis( idEntity* ent, const budMat3& axis )
{
	if( ent )
	{
		ent->SetAxis( axis );
	}
}

/*
================
budGameEdit::EntitySetColor
================
*/
void budGameEdit::EntitySetColor( idEntity* ent, const budVec3 color )
{
	if( ent )
	{
		ent->SetColor( color );
	}
}

/*
================
budGameEdit::EntityTranslate
================
*/
void budGameEdit::EntityTranslate( idEntity* ent, const budVec3& org )
{
	if( ent )
	{
		ent->GetPhysics()->Translate( org );
	}
}

/*
================
budGameEdit::EntityGetSpawnArgs
================
*/
const idDict* budGameEdit::EntityGetSpawnArgs( idEntity* ent ) const
{
	if( ent )
	{
		return &ent->spawnArgs;
	}
	return NULL;
}

/*
================
budGameEdit::EntityUpdateChangeableSpawnArgs
================
*/
void budGameEdit::EntityUpdateChangeableSpawnArgs( idEntity* ent, const idDict* dict )
{
	if( ent )
	{
		ent->UpdateChangeableSpawnArgs( dict );
	}
}

/*
================
budGameEdit::EntityChangeSpawnArgs
================
*/
void budGameEdit::EntityChangeSpawnArgs( idEntity* ent, const idDict* newArgs )
{
	if( ent )
	{
		for( int i = 0 ; i < newArgs->GetNumKeyVals() ; i ++ )
		{
			const idKeyValue* kv = newArgs->GetKeyVal( i );
			
			if( kv->GetValue().Length() > 0 )
			{
				ent->spawnArgs.Set( kv->GetKey() , kv->GetValue() );
			}
			else
			{
				ent->spawnArgs.Delete( kv->GetKey() );
			}
		}
	}
}

/*
================
budGameEdit::EntityUpdateVisuals
================
*/
void budGameEdit::EntityUpdateVisuals( idEntity* ent )
{
	if( ent )
	{
		ent->UpdateVisuals();
	}
}

/*
================
budGameEdit::EntitySetModel
================
*/
void budGameEdit::EntitySetModel( idEntity* ent, const char* val )
{
	if( ent )
	{
		ent->spawnArgs.Set( "model", val );
		ent->SetModel( val );
	}
}

/*
================
budGameEdit::EntityStopSound
================
*/
void budGameEdit::EntityStopSound( idEntity* ent )
{
	if( ent )
	{
		ent->StopSound( SND_CHANNEL_ANY, false );
	}
}

/*
================
budGameEdit::EntityDelete
================
*/
void budGameEdit::EntityDelete( idEntity* ent )
{
	delete ent;
}

/*
================
budGameEdit::PlayerIsValid
================
*/
bool budGameEdit::PlayerIsValid() const
{
	return ( gameLocal.GetLocalPlayer() != NULL );
}

/*
================
budGameEdit::PlayerGetOrigin
================
*/
void budGameEdit::PlayerGetOrigin( budVec3& org ) const
{
	org = gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin();
}

/*
================
budGameEdit::PlayerGetAxis
================
*/
void budGameEdit::PlayerGetAxis( budMat3& axis ) const
{
	axis = gameLocal.GetLocalPlayer()->GetPhysics()->GetAxis();
}

/*
================
budGameEdit::PlayerGetViewAngles
================
*/
void budGameEdit::PlayerGetViewAngles( budAngles& angles ) const
{
	angles = gameLocal.GetLocalPlayer()->viewAngles;
}

/*
================
budGameEdit::PlayerGetEyePosition
================
*/
void budGameEdit::PlayerGetEyePosition( budVec3& org ) const
{
	org = gameLocal.GetLocalPlayer()->GetEyePosition();
}


/*
================
budGameEdit::MapGetEntityDict
================
*/
const idDict* budGameEdit::MapGetEntityDict( const char* name ) const
{
	budMapFile* mapFile = gameLocal.GetLevelMap();
	if( mapFile && name && *name )
	{
		idMapEntity* mapent = mapFile->FindEntity( name );
		if( mapent )
		{
			return &mapent->epairs;
		}
	}
	return NULL;
}

/*
================
budGameEdit::MapSave
================
*/
void budGameEdit::MapSave( const char* path ) const
{
	budMapFile* mapFile = gameLocal.GetLevelMap();
	if( mapFile )
	{
		mapFile->Write( ( path ) ? path : mapFile->GetName(), ".map" );
	}
}

/*
================
budGameEdit::MapSetEntityKeyVal
================
*/
void budGameEdit::MapSetEntityKeyVal( const char* name, const char* key, const char* val ) const
{
	budMapFile* mapFile = gameLocal.GetLevelMap();
	if( mapFile && name && *name )
	{
		idMapEntity* mapent = mapFile->FindEntity( name );
		if( mapent )
		{
			mapent->epairs.Set( key, val );
		}
	}
}

/*
================
budGameEdit::MapCopyDictToEntity
================
*/
void budGameEdit::MapCopyDictToEntity( const char* name, const idDict* dict ) const
{
	budMapFile* mapFile = gameLocal.GetLevelMap();
	if( mapFile && name && *name )
	{
		idMapEntity* mapent = mapFile->FindEntity( name );
		if( mapent )
		{
			for( int i = 0; i < dict->GetNumKeyVals(); i++ )
			{
				const idKeyValue* kv = dict->GetKeyVal( i );
				const char* key = kv->GetKey();
				const char* val = kv->GetValue();
				mapent->epairs.Set( key, val );
			}
		}
	}
}



/*
================
budGameEdit::MapGetUniqueMatchingKeyVals
================
*/
int budGameEdit::MapGetUniqueMatchingKeyVals( const char* key, const char* list[], int max ) const
{
	budMapFile* mapFile = gameLocal.GetLevelMap();
	int count = 0;
	if( mapFile )
	{
		for( int i = 0; i < mapFile->GetNumEntities(); i++ )
		{
			idMapEntity* ent = mapFile->GetEntity( i );
			if( ent )
			{
				const char* k = ent->epairs.GetString( key );
				if( k != NULL && *k != '\0' && count < max )
				{
					list[count++] = k;
				}
			}
		}
	}
	return count;
}

/*
================
budGameEdit::MapAddEntity
================
*/
void budGameEdit::MapAddEntity( const idDict* dict ) const
{
	budMapFile* mapFile = gameLocal.GetLevelMap();
	if( mapFile )
	{
		idMapEntity* ent = new( TAG_GAME ) idMapEntity();
		ent->epairs = *dict;
		mapFile->AddEntity( ent );
	}
}

/*
================
budGameEdit::MapRemoveEntity
================
*/
void budGameEdit::MapRemoveEntity( const char* name ) const
{
	budMapFile* mapFile = gameLocal.GetLevelMap();
	if( mapFile )
	{
		idMapEntity* ent = mapFile->FindEntity( name );
		if( ent )
		{
			mapFile->RemoveEntity( ent );
		}
	}
}


/*
================
budGameEdit::MapGetEntitiesMatchignClassWithString
================
*/
int budGameEdit::MapGetEntitiesMatchingClassWithString( const char* classname, const char* match, const char* list[], const int max ) const
{
	budMapFile* mapFile = gameLocal.GetLevelMap();
	int count = 0;
	if( mapFile )
	{
		int entCount = mapFile->GetNumEntities();
		for( int i = 0 ; i < entCount; i++ )
		{
			idMapEntity* ent = mapFile->GetEntity( i );
			if( ent )
			{
				budStr work = ent->epairs.GetString( "classname" );
				if( work.Icmp( classname ) == 0 )
				{
					if( match && *match )
					{
						work = ent->epairs.GetString( "soundgroup" );
						if( count < max && work.Icmp( match ) == 0 )
						{
							list[count++] = ent->epairs.GetString( "name" );
						}
					}
					else if( count < max )
					{
						list[count++] = ent->epairs.GetString( "name" );
					}
				}
			}
		}
	}
	return count;
}


/*
================
budGameEdit::MapEntityTranslate
================
*/
void budGameEdit::MapEntityTranslate( const char* name, const budVec3& v ) const
{
	budMapFile* mapFile = gameLocal.GetLevelMap();
	if( mapFile && name && *name )
	{
		idMapEntity* mapent = mapFile->FindEntity( name );
		if( mapent )
		{
			budVec3 origin;
			mapent->epairs.GetVector( "origin", "", origin );
			origin += v;
			mapent->epairs.SetVector( "origin", origin );
		}
	}
}
