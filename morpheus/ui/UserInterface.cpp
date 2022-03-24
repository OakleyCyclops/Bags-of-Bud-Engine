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

#include "ListGUILocal.h"
#include "DeviceContext.h"
#include "Window.h"
#include "UserInterfaceLocal.h"

extern budCVar r_skipGuiShaders;		// 1 = don't render any gui elements on surfaces

budUserInterfaceManagerLocal	uiManagerLocal;
budUserInterfaceManager* 	uiManager = &uiManagerLocal;

// These used to be in every window, but they all pointed at the same one in budUserInterfaceManagerLocal.
// Made a global so it can be switched out dynamically to test optimized versions.
idDeviceContext* dc;

budCVar g_useNewGuiCode(	"g_useNewGuiCode",	"1", CVAR_GAME | CVAR_INTEGER, "use optimized device context code, 2 = toggle on/off every frame" );

extern budCVar sys_lang;




/*
===============================================================================

	budUserInterfaceManagerLocal

===============================================================================
*/

void budUserInterfaceManagerLocal::Init()
{
	screenRect = idRectangle( 0, 0, 640, 480 );
	dcOld.Init();
	dcOptimized.Init();
	
	SetDrawingDC();
	
}

void budUserInterfaceManagerLocal::Shutdown()
{
	guis.DeleteContents( true );
	demoGuis.DeleteContents( true );
	dcOld.Shutdown();
	dcOptimized.Shutdown();
	mapParser.Clear();
}

void budUserInterfaceManagerLocal::SetDrawingDC()
{
	static int toggle;
	
	// to make it more obvious that there is a difference between the old and
	// new paths, toggle between them every frame if g_useNewGuiCode is set to 2
	toggle++;
	
	if( g_useNewGuiCode.GetInteger() == 1 ||
			( g_useNewGuiCode.GetInteger() == 2 && ( toggle & 1 ) ) )
	{
		dc = &dcOptimized;
	}
	else
	{
		dc = &dcOld;
	}
}

void budUserInterfaceManagerLocal::Touch( const char* name )
{
	budUserInterface* gui = Alloc();
	gui->InitFromFile( name );
//	delete gui;
}

void budUserInterfaceManagerLocal::WritePrecacheCommands( budFile* f )
{

	int c = guis.Num();
	for( int i = 0; i < c; i++ )
	{
		char	str[1024];
		sprintf( str, "touchGui %s\n", guis[i]->Name() );
		common->Printf( "%s", str );
		f->Printf( "%s", str );
	}
}

void budUserInterfaceManagerLocal::SetSize( float width, float height )
{
	dc->SetSize( width, height );
}

void budUserInterfaceManagerLocal::Preload( const char* mapName )
{
	if( mapName != NULL && mapName[ 0 ] != '\0' )
	{
		mapParser.LoadFromFile( va( "generated/guis/%s.bgui", mapName ) );
	}
}

void budUserInterfaceManagerLocal::BeginLevelLoad()
{
	for( int i = 0; i < guis.Num(); i++ )
	{
		guis[ i ]->ClearRefs();
	}
}

void budUserInterfaceManagerLocal::EndLevelLoad( const char* mapName )
{
	int c = guis.Num();
	for( int i = 0; i < c; i++ )
	{
		if( guis[i]->GetRefs() == 0 )
		{
			//common->Printf( "purging %s.\n", guis[i]->GetSourceFile() );
			
			// use this to make sure no materials still reference this gui
			bool remove = true;
			for( int j = 0; j < declManager->GetNumDecls( DECL_MATERIAL ); j++ )
			{
				const budMaterial* material = static_cast<const budMaterial*>( declManager->DeclByIndex( DECL_MATERIAL, j, false ) );
				if( material->GlobalGui() == guis[i] )
				{
					remove = false;
					break;
				}
			}
			if( remove )
			{
				delete guis[ i ];
				guis.RemoveIndex( i );
				i--;
				c--;
			}
		}
		common->UpdateLevelLoadPacifier();
	}
	if( cvarSystem->GetCVarBool( "fs_buildresources" ) && mapName != NULL && mapName[ 0 ] != '\0' )
	{
		mapParser.WriteToFile( va( "generated/guis/%s.bgui", mapName ) );
		budFile* f = fileSystem->OpenFileRead( va( "generated/guis/%s.bgui", mapName ) );
		delete f;
	}
	
	dcOld.Init();
	dcOptimized.Init();
}

void budUserInterfaceManagerLocal::Reload( bool all )
{
	ID_TIME_T ts;
	
	int c = guis.Num();
	for( int i = 0; i < c; i++ )
	{
		if( !all )
		{
			fileSystem->ReadFile( guis[i]->GetSourceFile(), NULL, &ts );
			if( ts <= guis[i]->GetTimeStamp() )
			{
				continue;
			}
		}
		
		guis[i]->InitFromFile( guis[i]->GetSourceFile() );
		common->Printf( "reloading %s.\n", guis[i]->GetSourceFile() );
	}
}

void budUserInterfaceManagerLocal::ListGuis() const
{
	int c = guis.Num();
	common->Printf( "\n   size   refs   name\n" );
	size_t total = 0;
	int copies = 0;
	int unique = 0;
	for( int i = 0; i < c; i++ )
	{
		budUserInterfaceLocal* gui = guis[i];
		size_t sz = gui->Size();
		bool isUnique = guis[i]->interactive;
		if( isUnique )
		{
			unique++;
		}
		else
		{
			copies++;
		}
		common->Printf( "%6.1fk %4i (%s) %s ( %i transitions )\n", sz / 1024.0f, guis[i]->GetRefs(), isUnique ? "unique" : "copy", guis[i]->GetSourceFile(), guis[i]->desktop->NumTransitions() );
		total += sz;
	}
	common->Printf( "===========\n  %i total Guis ( %i copies, %i unique ), %.2f total Mbytes", c, copies, unique, total / ( 1024.0f * 1024.0f ) );
}

bool budUserInterfaceManagerLocal::CheckGui( const char* qpath ) const
{
	budFile* file = fileSystem->OpenFileRead( qpath );
	if( file )
	{
		fileSystem->CloseFile( file );
		return true;
	}
	return false;
}

budUserInterface* budUserInterfaceManagerLocal::Alloc() const
{
	return new( TAG_OLD_UI ) budUserInterfaceLocal();
}

void budUserInterfaceManagerLocal::DeAlloc( budUserInterface* gui )
{
	if( gui )
	{
		int c = guis.Num();
		for( int i = 0; i < c; i++ )
		{
			if( guis[i] == gui )
			{
				delete guis[i];
				guis.RemoveIndex( i );
				return;
			}
		}
	}
}

budUserInterface* budUserInterfaceManagerLocal::FindGui( const char* qpath, bool autoLoad, bool needUnique, bool forceNOTUnique )
{
	int c = guis.Num();
	
	for( int i = 0; i < c; i++ )
	{
		budUserInterfaceLocal* gui = guis[i];
		if( gui == NULL )
		{
			continue;
		}
		
		if( !budStr::Icmp( gui->GetSourceFile(), qpath ) )
		{
			if( !forceNOTUnique && ( needUnique || guis[i]->IsInteractive() ) )
			{
				break;
			}
			// Reload the gui if it's been cleared
			if( guis[i]->GetRefs() == 0 )
			{
				guis[i]->InitFromFile( guis[i]->GetSourceFile() );
			}
			guis[i]->AddRef();
			return guis[i];
		}
	}
	
	if( autoLoad )
	{
		budUserInterface* gui = Alloc();
		if( gui->InitFromFile( qpath ) )
		{
			gui->SetUniqued( forceNOTUnique ? false : needUnique );
			return gui;
		}
		else
		{
			delete gui;
		}
	}
	return NULL;
}

budUserInterface* budUserInterfaceManagerLocal::FindDemoGui( const char* qpath )
{
	int c = demoGuis.Num();
	for( int i = 0; i < c; i++ )
	{
		if( !budStr::Icmp( demoGuis[i]->GetSourceFile(), qpath ) )
		{
			return demoGuis[i];
		}
	}
	return NULL;
}

budListGUI* 	budUserInterfaceManagerLocal::AllocListGUI() const
{
	return new( TAG_OLD_UI ) budListGUILocal();
}

void budUserInterfaceManagerLocal::FreeListGUI( budListGUI* listgui )
{
	delete listgui;
}

/*
===============================================================================

	budUserInterfaceLocal

===============================================================================
*/

budUserInterfaceLocal::budUserInterfaceLocal()
{
	cursorX = cursorY = 0.0;
	desktop = NULL;
	loading = false;
	active = false;
	interactive = false;
	uniqued = false;
	bindHandler = NULL;
	//so the reg eval in gui parsing doesn't get bogus values
	time = 0;
	refs = 1;
}

budUserInterfaceLocal::~budUserInterfaceLocal()
{
	delete desktop;
	desktop = NULL;
}

const char* budUserInterfaceLocal::Name() const
{
	return source;
}

const char* budUserInterfaceLocal::Comment() const
{
	if( desktop )
	{
		return desktop->GetComment();
	}
	return "";
}

bool budUserInterfaceLocal::IsInteractive() const
{
	return interactive;
}

bool budUserInterfaceLocal::InitFromFile( const char* qpath, bool rebuild, bool cache )
{

	if( !( qpath && *qpath ) )
	{
		// FIXME: Memory leak!!
		return false;
	}
	
	int sz = sizeof( idWindow );
	sz = sizeof( idSimpleWindow );
	loading = true;
	
	if( rebuild )
	{
		delete desktop;
		desktop = new( TAG_OLD_UI ) idWindow( this );
	}
	else if( desktop == NULL )
	{
		desktop = new( TAG_OLD_UI ) idWindow( this );
	}
	
	// First try loading the localized version
	// Then fall back to the english version
	for( int i = 0; i < 2; i++ )
	{
		source = qpath;
		budStr trySource = qpath;
		trySource.ToLower();
		trySource.BackSlashesToSlashes();
		if( i == 0 )
		{
			trySource.Replace( "guis/", va( "guis/%s/", sys_lang.GetString() ) );
		}
		fileSystem->ReadFile( trySource, NULL, &timeStamp );
		if( timeStamp != FILE_NOT_FOUND_TIMESTAMP )
		{
			source = trySource;
			break;
		}
	}
	state.Set( "text", "Test Text!" );
	
	budTokenParser& bsrc = uiManagerLocal.GetBinaryParser();
	if( !bsrc.IsLoaded() || !bsrc.StartParsing( source ) )
	{
		budParser src( LEXFL_NOFATALERRORS | LEXFL_NOSTRINGCONCAT | LEXFL_ALLOWMULTICHARLITERALS | LEXFL_ALLOWBACKSLASHSTRINGCONCAT );
		src.LoadFile( source );
		if( src.IsLoaded() )
		{
			bsrc.LoadFromParser( src, source );
			ID_TIME_T ts = fileSystem->GetTimestamp( source );
			bsrc.UpdateTimeStamp( ts );
		}
	}
	if( bsrc.IsLoaded() && bsrc.StartParsing( source ) )
	{
		budToken token;
		while( bsrc.ReadToken( &token ) )
		{
			if( budStr::Icmp( token, "windowDef" ) == 0 )
			{
				if( desktop->Parse( &bsrc, rebuild ) )
				{
					desktop->SetFlag( WIN_DESKTOP );
					desktop->FixupParms();
				}
				continue;
			}
		}
		state.Set( "name", qpath );	// don't use localized name
		bsrc.DoneParsing();
	}
	else
	{
		desktop->SetFlag( WIN_DESKTOP );
		desktop->name = "Desktop";
		desktop->text = va( "Invalid GUI: %s", qpath );
		desktop->rect = idRectangle( 0.0f, 0.0f, 640.0f, 480.0f );
		desktop->drawRect = desktop->rect;
		desktop->foreColor = budVec4( 1.0f, 1.0f, 1.0f, 1.0f );
		desktop->backColor = budVec4( 0.0f, 0.0f, 0.0f, 1.0f );
		desktop->SetupFromState();
		common->Warning( "Couldn't load gui: '%s'", source.c_str() );
	}
	interactive = desktop->Interactive();
	if( uiManagerLocal.guis.Find( this ) == NULL )
	{
		uiManagerLocal.guis.Append( this );
	}
	loading = false;
	return true;
}

const char* budUserInterfaceLocal::HandleEvent( const sysEvent_t* event, int _time, bool* updateVisuals )
{

	time = _time;
	
	if( bindHandler && event->evType == SE_KEY && event->evValue2 == 1 )
	{
		const char* ret = bindHandler->HandleEvent( event, updateVisuals );
		bindHandler = NULL;
		return ret;
	}
	
	if( event->evType == SE_MOUSE )
	{
		cursorX += event->evValue;
		cursorY += event->evValue2;
		
		if( cursorX < 0 )
		{
			cursorX = 0;
		}
		if( cursorY < 0 )
		{
			cursorY = 0;
		}
	}
	
	if( desktop )
	{
		return desktop->HandleEvent( event, updateVisuals );
	}
	
	return "";
}

void budUserInterfaceLocal::HandleNamedEvent( const char* eventName )
{
	desktop->RunNamedEvent( eventName );
}

void budUserInterfaceLocal::Redraw( int _time, bool hud )
{
	if( r_skipGuiShaders.GetInteger() > 5 )
	{
		return;
	}
	if( !loading && desktop )
	{
		time = _time;
		dc->PushClipRect( uiManagerLocal.screenRect );
		desktop->Redraw( 0, 0, hud );
		dc->PopClipRect();
	}
}

void budUserInterfaceLocal::DrawCursor()
{
	if( !desktop || desktop->GetFlags() & WIN_MENUGUI )
	{
		dc->DrawCursor( &cursorX, &cursorY, 32.0f );
	}
	else
	{
		dc->DrawCursor( &cursorX, &cursorY, 56.0f );
	}
}

const idDict& budUserInterfaceLocal::State() const
{
	return state;
}

void budUserInterfaceLocal::DeleteStateVar( const char* varName )
{
	state.Delete( varName );
}

void budUserInterfaceLocal::SetStateString( const char* varName, const char* value )
{
	state.Set( varName, value );
}

void budUserInterfaceLocal::SetStateBool( const char* varName, const bool value )
{
	state.SetBool( varName, value );
}

void budUserInterfaceLocal::SetStateInt( const char* varName, const int value )
{
	state.SetInt( varName, value );
}

void budUserInterfaceLocal::SetStateFloat( const char* varName, const float value )
{
	state.SetFloat( varName, value );
}

const char* budUserInterfaceLocal::GetStateString( const char* varName, const char* defaultString ) const
{
	return state.GetString( varName, defaultString );
}

bool budUserInterfaceLocal::GetStateBool( const char* varName, const char* defaultString ) const
{
	return state.GetBool( varName, defaultString );
}

int budUserInterfaceLocal::GetStateInt( const char* varName, const char* defaultString ) const
{
	return state.GetInt( varName, defaultString );
}

float budUserInterfaceLocal::GetStateFloat( const char* varName, const char* defaultString ) const
{
	return state.GetFloat( varName, defaultString );
}

void budUserInterfaceLocal::StateChanged( int _time, bool redraw )
{
	time = _time;
	if( desktop )
	{
		desktop->StateChanged( redraw );
	}
	if( state.GetBool( "noninteractive" ) )
	{
		interactive = false;
	}
	else
	{
		if( desktop )
		{
			interactive = desktop->Interactive();
		}
		else
		{
			interactive = false;
		}
	}
}

const char* budUserInterfaceLocal::Activate( bool activate, int _time )
{
	time = _time;
	active = activate;
	if( desktop )
	{
		activateStr = "";
		desktop->Activate( activate, activateStr );
		return activateStr;
	}
	return "";
}

void budUserInterfaceLocal::Trigger( int _time )
{
	time = _time;
	if( desktop )
	{
		desktop->Trigger();
	}
}

void budUserInterfaceLocal::ReadFromDemoFile( class budDemoFile* f )
{
	budStr work;
	f->ReadDict( state );
	source = state.GetString( "name" );
	
	if( desktop == NULL )
	{
		f->Log( "creating new gui\n" );
		desktop = new( TAG_OLD_UI ) idWindow( this );
		desktop->SetFlag( WIN_DESKTOP );
		desktop->ReadFromDemoFile( f );
	}
	else
	{
		f->Log( "re-using gui\n" );
		desktop->ReadFromDemoFile( f, false );
	}
	
	f->ReadFloat( cursorX );
	f->ReadFloat( cursorY );
	
	bool add = true;
	int c = uiManagerLocal.demoGuis.Num();
	for( int i = 0; i < c; i++ )
	{
		if( uiManagerLocal.demoGuis[i] == this )
		{
			add = false;
			break;
		}
	}
	
	if( add )
	{
		uiManagerLocal.demoGuis.Append( this );
	}
}

void budUserInterfaceLocal::WriteToDemoFile( class budDemoFile* f )
{
	budStr work;
	f->WriteDict( state );
	if( desktop )
	{
		desktop->WriteToDemoFile( f );
	}
	
	f->WriteFloat( cursorX );
	f->WriteFloat( cursorY );
}

bool budUserInterfaceLocal::WriteToSaveGame( budFile* savefile ) const
{
	int len;
	const idKeyValue* kv;
	const char* string;
	
	int num = state.GetNumKeyVals();
	savefile->Write( &num, sizeof( num ) );
	
	for( int i = 0; i < num; i++ )
	{
		kv = state.GetKeyVal( i );
		len = kv->GetKey().Length();
		string = kv->GetKey().c_str();
		savefile->Write( &len, sizeof( len ) );
		savefile->Write( string, len );
		
		len = kv->GetValue().Length();
		string = kv->GetValue().c_str();
		savefile->Write( &len, sizeof( len ) );
		savefile->Write( string, len );
	}
	
	savefile->Write( &active, sizeof( active ) );
	savefile->Write( &interactive, sizeof( interactive ) );
	savefile->Write( &uniqued, sizeof( uniqued ) );
	savefile->Write( &time, sizeof( time ) );
	len = activateStr.Length();
	savefile->Write( &len, sizeof( len ) );
	savefile->Write( activateStr.c_str(), len );
	len = pendingCmd.Length();
	savefile->Write( &len, sizeof( len ) );
	savefile->Write( pendingCmd.c_str(), len );
	len = returnCmd.Length();
	savefile->Write( &len, sizeof( len ) );
	savefile->Write( returnCmd.c_str(), len );
	
	savefile->Write( &cursorX, sizeof( cursorX ) );
	savefile->Write( &cursorY, sizeof( cursorY ) );
	
	desktop->WriteToSaveGame( savefile );
	
	return true;
}

bool budUserInterfaceLocal::ReadFromSaveGame( budFile* savefile )
{
	int num;
	int i, len;
	budStr key;
	budStr value;
	
	savefile->Read( &num, sizeof( num ) );
	
	state.Clear();
	for( i = 0; i < num; i++ )
	{
		savefile->Read( &len, sizeof( len ) );
		key.Fill( ' ', len );
		savefile->Read( &key[0], len );
		
		savefile->Read( &len, sizeof( len ) );
		value.Fill( ' ', len );
		savefile->Read( &value[0], len );
		
		state.Set( key, value );
	}
	
	savefile->Read( &active, sizeof( active ) );
	savefile->Read( &interactive, sizeof( interactive ) );
	savefile->Read( &uniqued, sizeof( uniqued ) );
	savefile->Read( &time, sizeof( time ) );
	
	savefile->Read( &len, sizeof( len ) );
	activateStr.Fill( ' ', len );
	savefile->Read( &activateStr[0], len );
	savefile->Read( &len, sizeof( len ) );
	pendingCmd.Fill( ' ', len );
	savefile->Read( &pendingCmd[0], len );
	savefile->Read( &len, sizeof( len ) );
	returnCmd.Fill( ' ', len );
	savefile->Read( &returnCmd[0], len );
	
	savefile->Read( &cursorX, sizeof( cursorX ) );
	savefile->Read( &cursorY, sizeof( cursorY ) );
	
	desktop->ReadFromSaveGame( savefile );
	
	return true;
}

size_t budUserInterfaceLocal::Size()
{
	size_t sz = sizeof( *this ) + state.Size() + source.Allocated();
	if( desktop )
	{
		sz += desktop->Size();
	}
	return sz;
}

void budUserInterfaceLocal::RecurseSetKeyBindingNames( idWindow* window )
{
	int i;
	idWinVar* v = window->GetWinVarByName( "bind" );
	if( v )
	{
		SetStateString( v->GetName(), idKeyInput::KeysFromBinding( v->GetName() ) );
	}
	i = 0;
	while( i < window->GetChildCount() )
	{
		idWindow* next = window->GetChild( i );
		if( next )
		{
			RecurseSetKeyBindingNames( next );
		}
		i++;
	}
}

/*
==============
budUserInterfaceLocal::SetKeyBindingNames
==============
*/
void budUserInterfaceLocal::SetKeyBindingNames()
{
	if( !desktop )
	{
		return;
	}
	// walk the windows
	RecurseSetKeyBindingNames( desktop );
}

/*
==============
budUserInterfaceLocal::SetCursor
==============
*/
void budUserInterfaceLocal::SetCursor( float x, float y )
{
	cursorX = x;
	cursorY = y;
}

