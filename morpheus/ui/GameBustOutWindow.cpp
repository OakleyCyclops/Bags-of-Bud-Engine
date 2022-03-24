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

#include "../renderer/Image.h"

#include "DeviceContext.h"
#include "Window.h"
#include "UserInterfaceLocal.h"
#include "GameBustOutWindow.h"

#define BALL_RADIUS		12.f
#define BALL_SPEED		250.f
#define BALL_MAXSPEED	450.f

#define S_UNIQUE_CHANNEL	6

/*
*****************************************************************************
* BOEntity
****************************************************************************
*/
BOEntity::BOEntity( budGameBustOutWindow* _game )
{
	game = _game;
	visible = true;
	
	materialName = "";
	material = NULL;
	width = height = 8;
	color = colorWhite;
	powerup = POWERUP_NONE;
	
	position.Zero();
	velocity.Zero();
	
	removed = false;
	fadeOut = 0;
}

BOEntity::~BOEntity()
{
}

/*
======================
BOEntity::WriteToSaveGame
======================
*/
void BOEntity::WriteToSaveGame( budFile* savefile )
{

	savefile->Write( &visible, sizeof( visible ) );
	
	game->WriteSaveGameString( materialName, savefile );
	
	savefile->Write( &width, sizeof( width ) );
	savefile->Write( &height, sizeof( height ) );
	
	savefile->Write( &color, sizeof( color ) );
	savefile->Write( &position, sizeof( position ) );
	savefile->Write( &velocity, sizeof( velocity ) );
	
	savefile->Write( &powerup, sizeof( powerup ) );
	savefile->Write( &removed, sizeof( removed ) );
	savefile->Write( &fadeOut, sizeof( fadeOut ) );
}

/*
======================
BOEntity::ReadFromSaveGame
======================
*/
void BOEntity::ReadFromSaveGame( budFile* savefile, budGameBustOutWindow* _game )
{
	game = _game;
	
	savefile->Read( &visible, sizeof( visible ) );
	
	game->ReadSaveGameString( materialName, savefile );
	SetMaterial( materialName );
	
	savefile->Read( &width, sizeof( width ) );
	savefile->Read( &height, sizeof( height ) );
	
	savefile->Read( &color, sizeof( color ) );
	savefile->Read( &position, sizeof( position ) );
	savefile->Read( &velocity, sizeof( velocity ) );
	
	savefile->Read( &powerup, sizeof( powerup ) );
	savefile->Read( &removed, sizeof( removed ) );
	savefile->Read( &fadeOut, sizeof( fadeOut ) );
}

/*
======================
BOEntity::SetMaterial
======================
*/
void BOEntity::SetMaterial( const char* name )
{
	materialName = name;
	material = declManager->FindMaterial( name );
	material->SetSort( SS_GUI );
}

/*
======================
BOEntity::SetSize
======================
*/
void BOEntity::SetSize( float _width, float _height )
{
	width = _width;
	height = _height;
}

/*
======================
BOEntity::SetVisible
======================
*/
void BOEntity::SetColor( float r, float g, float b, float a )
{
	color.x = r;
	color.y = g;
	color.z = b;
	color.w = a;
}

/*
======================
BOEntity::SetVisible
======================
*/
void BOEntity::SetVisible( bool isVisible )
{
	visible = isVisible;
}

/*
======================
BOEntity::Update
======================
*/
void BOEntity::Update( float timeslice, int guiTime )
{

	if( !visible )
	{
		return;
	}
	
	// Move the entity
	position += velocity * timeslice;
	
	// Fade out the ent
	if( fadeOut )
	{
		color.w -= timeslice * 2.5;
		
		if( color.w <= 0.f )
		{
			color.w = 0.f;
			removed = true;
		}
	}
}

/*
======================
BOEntity::Draw
======================
*/
void BOEntity::Draw()
{
	if( visible )
	{
		dc->DrawMaterialRotated( position.x, position.y, width, height, material, color, 1.0f, 1.0f, DEG2RAD( 0.f ) );
	}
}

/*
*****************************************************************************
* BOBrick
****************************************************************************
*/
BOBrick::BOBrick()
{
	ent = NULL;
	x = y = width = height = 0;
	powerup = POWERUP_NONE;
	isBroken = false;
}

BOBrick::BOBrick( BOEntity* _ent, float _x, float _y, float _width, float _height )
{
	ent = _ent;
	x = _x;
	y = _y;
	width = _width;
	height = _height;
	powerup = POWERUP_NONE;
	
	isBroken = false;
	
	ent->position.x = x;
	ent->position.y = y;
	ent->SetSize( width, height );
	ent->SetMaterial( "game/bustout/brick" );
	
	ent->game->entities.Append( ent );
}

BOBrick::~BOBrick()
{
}

/*
======================
BOBrick::WriteToSaveGame
======================
*/
void BOBrick::WriteToSaveGame( budFile* savefile )
{
	savefile->Write( &x, sizeof( x ) );
	savefile->Write( &y, sizeof( y ) );
	savefile->Write( &width, sizeof( width ) );
	savefile->Write( &height, sizeof( height ) );
	
	savefile->Write( &powerup, sizeof( powerup ) );
	savefile->Write( &isBroken, sizeof( isBroken ) );
	
	int index = ent->game->entities.FindIndex( ent );
	savefile->Write( &index, sizeof( index ) );
}

/*
======================
BOBrick::ReadFromSaveGame
======================
*/
void BOBrick::ReadFromSaveGame( budFile* savefile, budGameBustOutWindow* game )
{
	savefile->Read( &x, sizeof( x ) );
	savefile->Read( &y, sizeof( y ) );
	savefile->Read( &width, sizeof( width ) );
	savefile->Read( &height, sizeof( height ) );
	
	savefile->Read( &powerup, sizeof( powerup ) );
	savefile->Read( &isBroken, sizeof( isBroken ) );
	
	int index;
	savefile->Read( &index, sizeof( index ) );
	ent = game->entities[index];
}

/*
======================
BOBrick::SetColor
======================
*/
void BOBrick::SetColor( budVec4 bcolor )
{
	ent->SetColor( bcolor.x, bcolor.y, bcolor.z, bcolor.w );
}

/*
======================
BOBrick::checkCollision
======================
*/
collideDir_t BOBrick::checkCollision( budVec2 pos, budVec2 vel )
{
	budVec2	ptA, ptB;
	float	dist;
	
	collideDir_t	result = COLLIDE_NONE;
	
	if( isBroken )
	{
		return result;
	}
	
	// Check for collision with each edge
	budVec2 vec;
	
	// Bottom
	ptA.x = x;
	ptA.y = y + height;
	
	ptB.x = x + width;
	ptB.y = y + height;
	
	if( vel.y < 0 && pos.y > ptA.y )
	{
		if( pos.x > ptA.x && pos.x < ptB.x )
		{
			dist = pos.y - ptA.y;
			
			if( dist < BALL_RADIUS )
			{
				result = COLLIDE_DOWN;
			}
		}
		else
		{
			if( pos.x <= ptA.x )
			{
				vec = pos - ptA;
			}
			else
			{
				vec = pos - ptB;
			}
			
			if( ( budMath::Fabs( vec.y ) > budMath::Fabs( vec.x ) ) && ( vec.LengthFast() < BALL_RADIUS ) )
			{
				result = COLLIDE_DOWN;
			}
		}
	}
	
	if( result == COLLIDE_NONE )
	{
		// Top
		ptA.y = y;
		ptB.y = y;
		
		if( vel.y > 0 && pos.y < ptA.y )
		{
			if( pos.x > ptA.x && pos.x < ptB.x )
			{
				dist = ptA.y - pos.y;
				
				if( dist < BALL_RADIUS )
				{
					result = COLLIDE_UP;
				}
			}
			else
			{
				if( pos.x <= ptA.x )
				{
					vec = pos - ptA;
				}
				else
				{
					vec = pos - ptB;
				}
				
				if( ( budMath::Fabs( vec.y ) > budMath::Fabs( vec.x ) ) && ( vec.LengthFast() < BALL_RADIUS ) )
				{
					result = COLLIDE_UP;
				}
			}
		}
		
		if( result == COLLIDE_NONE )
		{
			// Left side
			ptA.x = x;
			ptA.y = y;
			
			ptB.x = x;
			ptB.y = y + height;
			
			if( vel.x > 0 && pos.x < ptA.x )
			{
				if( pos.y > ptA.y && pos.y < ptB.y )
				{
					dist = ptA.x - pos.x;
					
					if( dist < BALL_RADIUS )
					{
						result = COLLIDE_LEFT;
					}
				}
				else
				{
					if( pos.y <= ptA.y )
					{
						vec = pos - ptA;
					}
					else
					{
						vec = pos - ptB;
					}
					
					if( ( budMath::Fabs( vec.x ) >= budMath::Fabs( vec.y ) ) && ( vec.LengthFast() < BALL_RADIUS ) )
					{
						result = COLLIDE_LEFT;
					}
				}
			}
			
			if( result == COLLIDE_NONE )
			{
				// Right side
				ptA.x = x + width;
				ptB.x = x + width;
				
				if( vel.x < 0 && pos.x > ptA.x )
				{
					if( pos.y > ptA.y && pos.y < ptB.y )
					{
						dist = pos.x - ptA.x;
						
						if( dist < BALL_RADIUS )
						{
							result = COLLIDE_LEFT;
						}
					}
					else
					{
						if( pos.y <= ptA.y )
						{
							vec = pos - ptA;
						}
						else
						{
							vec = pos - ptB;
						}
						
						if( ( budMath::Fabs( vec.x ) >= budMath::Fabs( vec.y ) ) && ( vec.LengthFast() < BALL_RADIUS ) )
						{
							result = COLLIDE_LEFT;
						}
					}
				}
				
			}
		}
	}
	
	return result;
}

/*
*****************************************************************************
* budGameBustOutWindow
****************************************************************************
*/
budGameBustOutWindow::budGameBustOutWindow( budUserInterfaceLocal* g ) : idWindow( g )
{
	gui = g;
	CommonInit();
}

budGameBustOutWindow::~budGameBustOutWindow()
{
	entities.DeleteContents( true );
	
	Mem_Free( levelBoardData );
}

/*
=============================
budGameBustOutWindow::WriteToSaveGame
=============================
*/
void budGameBustOutWindow::WriteToSaveGame( budFile* savefile )
{
	idWindow::WriteToSaveGame( savefile );
	
	gamerunning.WriteToSaveGame( savefile );
	onFire.WriteToSaveGame( savefile );
	onContinue.WriteToSaveGame( savefile );
	onNewGame.WriteToSaveGame( savefile );
	onNewLevel.WriteToSaveGame( savefile );
	
	savefile->Write( &timeSlice, sizeof( timeSlice ) );
	savefile->Write( &gameOver, sizeof( gameOver ) );
	savefile->Write( &numLevels, sizeof( numLevels ) );
	
	// Board Data is loaded when GUI is loaded, don't need to save
	
	savefile->Write( &numBricks, sizeof( numBricks ) );
	savefile->Write( &currentLevel, sizeof( currentLevel ) );
	
	savefile->Write( &updateScore, sizeof( updateScore ) );
	savefile->Write( &gameScore, sizeof( gameScore ) );
	savefile->Write( &nextBallScore, sizeof( nextBallScore ) );
	
	savefile->Write( &bigPaddleTime, sizeof( bigPaddleTime ) );
	savefile->Write( &paddleVelocity, sizeof( paddleVelocity ) );
	
	savefile->Write( &ballSpeed, sizeof( ballSpeed ) );
	savefile->Write( &ballsRemaining, sizeof( ballsRemaining ) );
	savefile->Write( &ballsInPlay, sizeof( ballsInPlay ) );
	savefile->Write( &ballHitCeiling, sizeof( ballHitCeiling ) );
	
	// Write Entities
	int i;
	int numberOfEnts = entities.Num();
	savefile->Write( &numberOfEnts, sizeof( numberOfEnts ) );
	for( i = 0; i < numberOfEnts; i++ )
	{
		entities[i]->WriteToSaveGame( savefile );
	}
	
	// Write Balls
	numberOfEnts = balls.Num();
	savefile->Write( &numberOfEnts, sizeof( numberOfEnts ) );
	for( i = 0; i < numberOfEnts; i++ )
	{
		int ballIndex = entities.FindIndex( balls[i] );
		savefile->Write( &ballIndex, sizeof( ballIndex ) );
	}
	
	// Write Powerups
	numberOfEnts = powerUps.Num();
	savefile->Write( &numberOfEnts, sizeof( numberOfEnts ) );
	for( i = 0; i < numberOfEnts; i++ )
	{
		int powerIndex = entities.FindIndex( powerUps[i] );
		savefile->Write( &powerIndex, sizeof( powerIndex ) );
	}
	
	// Write paddle
	paddle->WriteToSaveGame( savefile );
	
	// Write Bricks
	int row;
	for( row = 0; row < BOARD_ROWS; row++ )
	{
		numberOfEnts = board[row].Num();
		savefile->Write( &numberOfEnts, sizeof( numberOfEnts ) );
		for( i = 0; i < numberOfEnts; i++ )
		{
			board[row][i]->WriteToSaveGame( savefile );
		}
	}
}

/*
=============================
budGameBustOutWindow::ReadFromSaveGame
=============================
*/
void budGameBustOutWindow::ReadFromSaveGame( budFile* savefile )
{
	idWindow::ReadFromSaveGame( savefile );
	
	// Clear out existing paddle and entities from GUI load
	delete paddle;
	entities.DeleteContents( true );
	
	gamerunning.ReadFromSaveGame( savefile );
	onFire.ReadFromSaveGame( savefile );
	onContinue.ReadFromSaveGame( savefile );
	onNewGame.ReadFromSaveGame( savefile );
	onNewLevel.ReadFromSaveGame( savefile );
	
	savefile->Read( &timeSlice, sizeof( timeSlice ) );
	savefile->Read( &gameOver, sizeof( gameOver ) );
	savefile->Read( &numLevels, sizeof( numLevels ) );
	
	// Board Data is loaded when GUI is loaded, don't need to save
	
	savefile->Read( &numBricks, sizeof( numBricks ) );
	savefile->Read( &currentLevel, sizeof( currentLevel ) );
	
	savefile->Read( &updateScore, sizeof( updateScore ) );
	savefile->Read( &gameScore, sizeof( gameScore ) );
	savefile->Read( &nextBallScore, sizeof( nextBallScore ) );
	
	savefile->Read( &bigPaddleTime, sizeof( bigPaddleTime ) );
	savefile->Read( &paddleVelocity, sizeof( paddleVelocity ) );
	
	savefile->Read( &ballSpeed, sizeof( ballSpeed ) );
	savefile->Read( &ballsRemaining, sizeof( ballsRemaining ) );
	savefile->Read( &ballsInPlay, sizeof( ballsInPlay ) );
	savefile->Read( &ballHitCeiling, sizeof( ballHitCeiling ) );
	
	int i;
	int numberOfEnts;
	
	// Read entities
	savefile->Read( &numberOfEnts, sizeof( numberOfEnts ) );
	for( i = 0; i < numberOfEnts; i++ )
	{
		BOEntity* ent;
		
		ent = new( TAG_OLD_UI ) BOEntity( this );
		ent->ReadFromSaveGame( savefile, this );
		entities.Append( ent );
	}
	
	// Read balls
	savefile->Read( &numberOfEnts, sizeof( numberOfEnts ) );
	for( i = 0; i < numberOfEnts; i++ )
	{
		int ballIndex;
		savefile->Read( &ballIndex, sizeof( ballIndex ) );
		balls.Append( entities[ballIndex] );
	}
	
	// Read powerups
	savefile->Read( &numberOfEnts, sizeof( numberOfEnts ) );
	for( i = 0; i < numberOfEnts; i++ )
	{
		int powerIndex;
		savefile->Read( &powerIndex, sizeof( powerIndex ) );
		balls.Append( entities[powerIndex] );
	}
	
	// Read paddle
	paddle = new( TAG_OLD_UI ) BOBrick();
	paddle->ReadFromSaveGame( savefile, this );
	
	// Read board
	int row;
	for( row = 0; row < BOARD_ROWS; row++ )
	{
		savefile->Read( &numberOfEnts, sizeof( numberOfEnts ) );
		for( i = 0; i < numberOfEnts; i++ )
		{
			BOBrick* brick = new( TAG_OLD_UI ) BOBrick();
			brick->ReadFromSaveGame( savefile, this );
			board[row].Append( brick );
		}
	}
}

/*
=============================
budGameBustOutWindow::ResetGameState
=============================
*/
void budGameBustOutWindow::ResetGameState()
{
	gamerunning = false;
	gameOver = false;
	onFire = false;
	onContinue = false;
	onNewGame = false;
	onNewLevel = false;
	
	// Game moves forward 16 milliseconds every frame
	timeSlice = 0.016f;
	ballsRemaining = 3;
	ballSpeed = BALL_SPEED;
	ballsInPlay = 0;
	updateScore = false;
	numBricks = 0;
	currentLevel = 1;
	gameScore = 0;
	bigPaddleTime = 0;
	nextBallScore = gameScore + 10000;
	
	ClearBoard();
}

/*
=============================
budGameBustOutWindow::CommonInit
=============================
*/
void budGameBustOutWindow::CommonInit()
{
	BOEntity* ent;
	
	// Precache images
	declManager->FindMaterial( "game/bustout/ball" );
	declManager->FindMaterial( "game/bustout/doublepaddle" );
	declManager->FindMaterial( "game/bustout/powerup_bigpaddle" );
	declManager->FindMaterial( "game/bustout/powerup_multiball" );
	declManager->FindMaterial( "game/bustout/brick" );
	
	// Precache sounds
	declManager->FindSound( "arcade_ballbounce" );
	declManager->FindSound( "arcade_brickhit" );
	declManager->FindSound( "arcade_missedball" );
	declManager->FindSound( "arcade_sadsound" );
	declManager->FindSound( "arcade_extraball" );
	declManager->FindSound( "arcade_powerup" );
	
	ResetGameState();
	
	numLevels = 0;
	boardDataLoaded = false;
	levelBoardData = NULL;
	
	// Create Paddle
	ent = new( TAG_OLD_UI ) BOEntity( this );
	paddle = new( TAG_OLD_UI ) BOBrick( ent, 260.f, 440.f, 96.f, 24.f );
	paddle->ent->SetMaterial( "game/bustout/paddle" );
}

/*
=============================
budGameBustOutWindow::HandleEvent
=============================
*/
const char* budGameBustOutWindow::HandleEvent( const sysEvent_t* event, bool* updateVisuals )
{
	int key = event->evValue;
	
	// need to call this to allow proper focus and capturing on embedded children
	const char* ret = idWindow::HandleEvent( event, updateVisuals );
	
	if( event->evType == SE_KEY )
	{
	
		if( !event->evValue2 )
		{
			return ret;
		}
		if( key == K_MOUSE1 )
		{
			// Mouse was clicked
			if( ballsInPlay == 0 )
			{
				BOEntity* ball = CreateNewBall();
				
				ball->SetVisible( true );
				ball->position.x = paddle->ent->position.x + 48.f;
				ball->position.y = 430.f;
				
				ball->velocity.x = ballSpeed;
				ball->velocity.y = -ballSpeed * 2.f;
				ball->velocity.NormalizeFast();
				ball->velocity *= ballSpeed;
			}
		}
		else
		{
			return ret;
		}
	}
	
	return ret;
}

/*
=============================
budGameBustOutWindow::ParseInternalVar
=============================
*/
bool budGameBustOutWindow::ParseInternalVar( const char* _name, budTokenParser* src )
{
	if( budStr::Icmp( _name, "gamerunning" ) == 0 )
	{
		gamerunning = src->ParseBool();
		return true;
	}
	if( budStr::Icmp( _name, "onFire" ) == 0 )
	{
		onFire = src->ParseBool();
		return true;
	}
	if( budStr::Icmp( _name, "onContinue" ) == 0 )
	{
		onContinue = src->ParseBool();
		return true;
	}
	if( budStr::Icmp( _name, "onNewGame" ) == 0 )
	{
		onNewGame = src->ParseBool();
		return true;
	}
	if( budStr::Icmp( _name, "onNewLevel" ) == 0 )
	{
		onNewLevel = src->ParseBool();
		return true;
	}
	if( budStr::Icmp( _name, "numLevels" ) == 0 )
	{
		numLevels = src->ParseInt();
		
		// Load all the level images
		LoadBoardFiles();
		return true;
	}
	
	return idWindow::ParseInternalVar( _name, src );
}

/*
=============================
budGameBustOutWindow::GetWinVarByName
=============================
*/
idWinVar* budGameBustOutWindow::GetWinVarByName( const char* _name, bool winLookup, drawWin_t** owner )
{
	idWinVar* retVar = NULL;
	
	if( budStr::Icmp( _name, "gamerunning" ) == 0 )
	{
		retVar = &gamerunning;
	}
	else 	if( budStr::Icmp( _name, "onFire" ) == 0 )
	{
		retVar = &onFire;
	}
	else 	if( budStr::Icmp( _name, "onContinue" ) == 0 )
	{
		retVar = &onContinue;
	}
	else 	if( budStr::Icmp( _name, "onNewGame" ) == 0 )
	{
		retVar = &onNewGame;
	}
	else 	if( budStr::Icmp( _name, "onNewLevel" ) == 0 )
	{
		retVar = &onNewLevel;
	}
	
	if( retVar )
	{
		return retVar;
	}
	
	return idWindow::GetWinVarByName( _name, winLookup, owner );
}

/*
=============================
budGameBustOutWindow::PostParse
=============================
*/
void budGameBustOutWindow::PostParse()
{
	idWindow::PostParse();
}

/*
=============================
budGameBustOutWindow::Draw
=============================
*/
void budGameBustOutWindow::Draw( int time, float x, float y )
{
	int i;
	
	//Update the game every frame before drawing
	UpdateGame();
	
	for( i = entities.Num() - 1; i >= 0; i-- )
	{
		entities[i]->Draw();
	}
}

/*
=============================
budGameBustOutWindow::Activate
=============================
*/
const char* budGameBustOutWindow::Activate( bool activate )
{
	return "";
}


/*
=============================
budGameBustOutWindow::UpdateScore
=============================
*/
void budGameBustOutWindow::UpdateScore()
{

	if( gameOver )
	{
		gui->HandleNamedEvent( "GameOver" );
		return;
	}
	
	// Check for level progression
	if( numBricks == 0 )
	{
		ClearBalls();
		
		gui->HandleNamedEvent( "levelComplete" );
	}
	
	// Check for new ball score
	if( gameScore >= nextBallScore )
	{
		ballsRemaining++;
		gui->HandleNamedEvent( "extraBall" );
		
		// Play sound
		common->SW()->PlayShaderDirectly( "arcade_extraball", S_UNIQUE_CHANNEL );
		
		nextBallScore = gameScore + 10000;
	}
	
	gui->SetStateString( "player_score", va( "%i", gameScore ) );
	gui->SetStateString( "balls_remaining", va( "%i", ballsRemaining ) );
	gui->SetStateString( "current_level", va( "%i", currentLevel ) );
	gui->SetStateString( "next_ball_score", va( "%i", nextBallScore ) );
}

/*
=============================
budGameBustOutWindow::ClearBoard
=============================
*/
void budGameBustOutWindow::ClearBoard()
{
	int i, j;
	
	ClearPowerups();
	
	ballHitCeiling = false;
	
	for( i = 0; i < BOARD_ROWS; i++ )
	{
		for( j = 0; j < board[i].Num(); j++ )
		{
		
			BOBrick* brick = board[i][j];
			brick->ent->removed = true;
		}
		
		board[i].DeleteContents( true );
	}
}

/*
=============================
budGameBustOutWindow::ClearPowerups
=============================
*/
void budGameBustOutWindow::ClearPowerups()
{
	while( powerUps.Num() )
	{
		powerUps[0]->removed = true;
		powerUps.RemoveIndex( 0 );
	}
}

/*
=============================
budGameBustOutWindow::ClearBalls
=============================
*/
void budGameBustOutWindow::ClearBalls()
{
	while( balls.Num() )
	{
		balls[0]->removed = true;
		balls.RemoveIndex( 0 );
	}
	
	ballsInPlay = 0;
}

/*
=============================
budGameBustOutWindow::LoadBoardFiles
=============================
*/
void budGameBustOutWindow::LoadBoardFiles()
{
	int i;
	int w, h;
	ID_TIME_T time;
	int boardSize;
	byte* currentBoard;
	
	if( boardDataLoaded )
	{
		return;
	}
	
	boardSize = 9 * 12 * 4;
	levelBoardData = ( byte* )Mem_Alloc( boardSize * numLevels, TAG_CRAP );
	
	currentBoard = levelBoardData;
	
	for( i = 0; i < numLevels; i++ )
	{
		byte* pic;
		budStr	name = "guis/assets/bustout/level";
		name += ( i + 1 );
		name += ".tga";
		
		R_LoadImage( name, &pic, &w, &h, &time, false );
		
		if( pic != NULL )
		{
			if( w != 9 || h != 12 )
			{
				common->DWarning( "Hell Bust-Out level image not correct dimensions! (%d x %d)", w, h );
			}
			
			memcpy( currentBoard, pic, boardSize );
			Mem_Free( pic );
		}
		
		currentBoard += boardSize;
	}
	
	boardDataLoaded = true;
}

/*
=============================
budGameBustOutWindow::SetCurrentBoard
=============================
*/
void budGameBustOutWindow::SetCurrentBoard()
{
	int i, j;
	int realLevel = ( ( currentLevel - 1 ) % numLevels );
	int boardSize;
	byte* currentBoard;
	float	bx = 11.f;
	float	by = 24.f;
	float	stepx = 619.f / 9.f;
	float	stepy = ( 256 / 12.f );
	
	boardSize = 9 * 12 * 4;
	currentBoard = levelBoardData + ( realLevel * boardSize );
	
	for( j = 0; j < BOARD_ROWS; j++ )
	{
		bx = 11.f;
		
		for( i = 0; i < 9; i++ )
		{
			int pixelindex = ( j * 9 * 4 ) + ( i * 4 );
			
			if( currentBoard[pixelindex + 3] )
			{
				budVec4 bcolor;
				float pType = 0.f;
				
				BOEntity* bent = new( TAG_OLD_UI ) BOEntity( this );
				BOBrick* brick = new( TAG_OLD_UI ) BOBrick( bent, bx, by, stepx, stepy );
				
				bcolor.x = currentBoard[pixelindex + 0] / 255.f;
				bcolor.y = currentBoard[pixelindex + 1] / 255.f;
				bcolor.z = currentBoard[pixelindex + 2] / 255.f;
				bcolor.w = 1.f;
				brick->SetColor( bcolor );
				
				pType = currentBoard[pixelindex + 3] / 255.f;
				if( pType > 0.f && pType < 1.f )
				{
					if( pType < 0.5f )
					{
						brick->powerup = POWERUP_BIGPADDLE;
					}
					else
					{
						brick->powerup = POWERUP_MULTIBALL;
					}
				}
				
				board[j].Append( brick );
				numBricks++;
			}
			
			bx += stepx;
		}
		
		by += stepy;
	}
}

/*
=============================
budGameBustOutWindow::CreateNewBall
=============================
*/
BOEntity* budGameBustOutWindow::CreateNewBall()
{
	BOEntity* ball;
	
	ball = new( TAG_OLD_UI ) BOEntity( this );
	ball->position.x = 300.f;
	ball->position.y = 416.f;
	ball->SetMaterial( "game/bustout/ball" );
	ball->SetSize( BALL_RADIUS * 2.f, BALL_RADIUS * 2.f );
	ball->SetVisible( false );
	
	ballsInPlay++;
	
	balls.Append( ball );
	entities.Append( ball );
	
	return ball;
}

/*
=============================
budGameBustOutWindow::CreatePowerup
=============================
*/
BOEntity* budGameBustOutWindow::CreatePowerup( BOBrick* brick )
{
	BOEntity* powerEnt = new( TAG_OLD_UI ) BOEntity( this );
	
	powerEnt->position.x = brick->x;
	powerEnt->position.y = brick->y;
	powerEnt->velocity.x = 0.f;
	powerEnt->velocity.y = 64.f;
	
	powerEnt->powerup = brick->powerup;
	
	switch( powerEnt->powerup )
	{
		case POWERUP_BIGPADDLE:
			powerEnt->SetMaterial( "game/bustout/powerup_bigpaddle" );
			break;
		case POWERUP_MULTIBALL:
			powerEnt->SetMaterial( "game/bustout/powerup_multiball" );
			break;
		default:
			powerEnt->SetMaterial( "textures/common/nodraw" );
			break;
	}
	
	powerEnt->SetSize( 619 / 9, 256 / 12 );
	powerEnt->SetVisible( true );
	
	powerUps.Append( powerEnt );
	entities.Append( powerEnt );
	
	return powerEnt;
}

/*
=============================
budGameBustOutWindow::UpdatePowerups
=============================
*/
void budGameBustOutWindow::UpdatePowerups()
{
	budVec2 pos;
	
	for( int i = 0; i < powerUps.Num(); i++ )
	{
		BOEntity* pUp = powerUps[i];
		
		// Check for powerup falling below screen
		if( pUp->position.y > 480 )
		{
		
			powerUps.RemoveIndex( i );
			pUp->removed = true;
			continue;
		}
		
		// Check for the paddle catching a powerup
		pos.x = pUp->position.x + ( pUp->width / 2 );
		pos.y = pUp->position.y + ( pUp->height / 2 );
		
		collideDir_t collision = paddle->checkCollision( pos, pUp->velocity );
		if( collision != COLLIDE_NONE )
		{
			BOEntity* ball;
			
			// Give the powerup to the player
			switch( pUp->powerup )
			{
				case POWERUP_BIGPADDLE:
					bigPaddleTime = gui->GetTime() + 15000;
					break;
				case POWERUP_MULTIBALL:
					// Create 2 new balls in the spot of the existing ball
					for( int b = 0; b < 2; b++ )
					{
						ball = CreateNewBall();
						ball->position = balls[0]->position;
						ball->velocity = balls[0]->velocity;
						
						if( b == 0 )
						{
							ball->velocity.x -= 35.f;
						}
						else
						{
							ball->velocity.x += 35.f;
						}
						ball->velocity.NormalizeFast();
						ball->velocity *= ballSpeed;
						
						ball->SetVisible( true );
					}
					break;
				default:
					break;
			}
			
			// Play the sound
			common->SW()->PlayShaderDirectly( "arcade_powerup", S_UNIQUE_CHANNEL );
			
			// Remove it
			powerUps.RemoveIndex( i );
			pUp->removed = true;
		}
	}
}

/*
=============================
budGameBustOutWindow::UpdatePaddle
=============================
*/
void budGameBustOutWindow::UpdatePaddle()
{
	budVec2 cursorPos;
	float  oldPos = paddle->x;
	
	cursorPos.x = gui->CursorX();
	cursorPos.y = gui->CursorY();
	
	if( bigPaddleTime > gui->GetTime() )
	{
		paddle->x = cursorPos.x - 80.f;
		paddle->width = 160;
		paddle->ent->width = 160;
		paddle->ent->SetMaterial( "game/bustout/doublepaddle" );
	}
	else
	{
		paddle->x = cursorPos.x - 48.f;
		paddle->width = 96;
		paddle->ent->width = 96;
		paddle->ent->SetMaterial( "game/bustout/paddle" );
	}
	paddle->ent->position.x = paddle->x;
	
	paddleVelocity = ( paddle->x - oldPos );
}

/*
=============================
budGameBustOutWindow::UpdateBall
=============================
*/
void budGameBustOutWindow::UpdateBall()
{
	int ballnum, i, j;
	bool playSoundBounce = false;
	bool playSoundBrick = false;
	static int bounceChannel = 1;
	
	if( ballsInPlay == 0 )
	{
		return;
	}
	
	for( ballnum = 0; ballnum < balls.Num(); ballnum++ )
	{
		BOEntity* ball = balls[ballnum];
		
		// Check for ball going below screen, lost ball
		if( ball->position.y > 480.f )
		{
			ball->removed = true;
			continue;
		}
		
		// Check world collision
		if( ball->position.y < 20 && ball->velocity.y < 0 )
		{
			ball->velocity.y = -ball->velocity.y;
			
			// Increase ball speed when it hits ceiling
			if( !ballHitCeiling )
			{
				ballSpeed *= 1.25f;
				ballHitCeiling = true;
			}
			playSoundBounce = true;
		}
		
		if( ball->position.x > 608 && ball->velocity.x > 0 )
		{
			ball->velocity.x = -ball->velocity.x;
			playSoundBounce = true;
		}
		else if( ball->position.x < 8 && ball->velocity.x < 0 )
		{
			ball->velocity.x = -ball->velocity.x;
			playSoundBounce = true;
		}
		
		// Check for Paddle collision
		budVec2 ballCenter = ball->position + budVec2( BALL_RADIUS, BALL_RADIUS );
		collideDir_t collision = paddle->checkCollision( ballCenter, ball->velocity );
		
		if( collision == COLLIDE_UP )
		{
			if( ball->velocity.y > 0 )
			{
				budVec2	paddleVec( paddleVelocity * 2, 0 );
				float	centerX;
				
				if( bigPaddleTime > gui->GetTime() )
				{
					centerX = paddle->x + 80.f;
				}
				else
				{
					centerX = paddle->x + 48.f;
				}
				
				ball->velocity.y = -ball->velocity.y;
				
				paddleVec.x += ( ball->position.x - centerX ) * 2;
				
				ball->velocity += paddleVec;
				ball->velocity.NormalizeFast();
				ball->velocity *= ballSpeed;
				
				playSoundBounce = true;
			}
		}
		else if( collision == COLLIDE_LEFT || collision == COLLIDE_RIGHT )
		{
			if( ball->velocity.y > 0 )
			{
				ball->velocity.x = -ball->velocity.x;
				playSoundBounce = true;
			}
		}
		
		collision = COLLIDE_NONE;
		
		// Check for collision with bricks
		for( i = 0; i < BOARD_ROWS; i++ )
		{
			int num = board[i].Num();
			
			for( j = 0; j < num; j++ )
			{
				BOBrick* brick = ( board[i] )[j];
				
				collision = brick->checkCollision( ballCenter, ball->velocity );
				if( collision )
				{
					// Now break the brick if there was a collision
					brick->isBroken = true;
					brick->ent->fadeOut = true;
					
					if( brick->powerup > POWERUP_NONE )
					{
						verify( CreatePowerup( brick ) != NULL );
					}
					
					numBricks--;
					gameScore += 100;
					updateScore = true;
					
					// Go ahead an forcibly remove the last brick, no fade
					if( numBricks == 0 )
					{
						brick->ent->removed = true;
					}
					board[i].Remove( brick );
					break;
				}
			}
			
			if( collision )
			{
				playSoundBrick = true;
				break;
			}
		}
		
		if( collision == COLLIDE_DOWN || collision == COLLIDE_UP )
		{
			ball->velocity.y *= -1;
		}
		else if( collision == COLLIDE_LEFT || collision == COLLIDE_RIGHT )
		{
			ball->velocity.x *= -1;
		}
		
		if( playSoundBounce )
		{
			common->SW()->PlayShaderDirectly( "arcade_ballbounce", bounceChannel );
		}
		else if( playSoundBrick )
		{
			common->SW()->PlayShaderDirectly( "arcade_brickhit", bounceChannel );
		}
		
		if( playSoundBounce || playSoundBrick )
		{
			bounceChannel++;
			if( bounceChannel == 4 )
			{
				bounceChannel = 1;
			}
		}
	}
	
	// Check to see if any balls were removed from play
	for( ballnum = 0; ballnum < balls.Num(); ballnum++ )
	{
		if( balls[ballnum]->removed )
		{
			ballsInPlay--;
			balls.RemoveIndex( ballnum );
		}
	}
	
	// If all the balls were removed, update the game accordingly
	if( ballsInPlay == 0 )
	{
		if( ballsRemaining == 0 )
		{
			gameOver = true;
			
			// Game Over sound
			common->SW()->PlayShaderDirectly( "arcade_sadsound", S_UNIQUE_CHANNEL );
		}
		else
		{
			ballsRemaining--;
			
			// Ball was lost, but game is not over
			common->SW()->PlayShaderDirectly( "arcade_missedball", S_UNIQUE_CHANNEL );
		}
		
		ClearPowerups();
		updateScore = true;
	}
}

/*
=============================
budGameBustOutWindow::UpdateGame
=============================
*/
void budGameBustOutWindow::UpdateGame()
{
	int i;
	
	if( onNewGame )
	{
		ResetGameState();
		
		// Create Board
		SetCurrentBoard();
		
		gamerunning = true;
	}
	if( onContinue )
	{
		gameOver = false;
		ballsRemaining = 3;
		
		onContinue = false;
	}
	if( onNewLevel )
	{
		currentLevel++;
		
		ClearBoard();
		SetCurrentBoard();
		
		ballSpeed = BALL_SPEED * ( 1.f + ( ( float )currentLevel / 5.f ) );
		if( ballSpeed > BALL_MAXSPEED )
		{
			ballSpeed = BALL_MAXSPEED;
		}
		updateScore = true;
		onNewLevel = false;
	}
	
	if( gamerunning == true )
	{
	
		UpdatePaddle();
		UpdateBall();
		UpdatePowerups();
		
		for( i = 0; i < entities.Num(); i++ )
		{
			entities[i]->Update( timeSlice, gui->GetTime() );
		}
		
		// Delete entities that need to be deleted
		for( i = entities.Num() - 1; i >= 0; i-- )
		{
			if( entities[i]->removed )
			{
				BOEntity* ent = entities[i];
				delete ent;
				entities.RemoveIndex( i );
			}
		}
		
		if( updateScore )
		{
			UpdateScore();
			updateScore = false;
		}
	}
}