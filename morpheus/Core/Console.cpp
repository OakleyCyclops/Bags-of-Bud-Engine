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
#include "corePCH.hpp"
#include "ConsoleHistory.hpp"
#include "Common_local.hpp"

#define	CON_TEXTSIZE			0x30000
#define	NUM_CON_TIMES			4
#define CONSOLE_FIRSTREPEAT		200
#define CONSOLE_REPEAT			100

#define	COMMAND_HISTORY			64

struct overlayText_t
{
	budStr			text;
	justify_t		justify;
	int				time;
};

// the console will query the cvar and command systems for
// command completion information

class budConsoleLocal : public budConsole
{
public:
	virtual	void		Init();
	virtual void		Shutdown();
	virtual	bool		ProcessEvent( const sysEvent_t* event, bool forceAccept );
	virtual	bool		Active();
	virtual	void		ClearNotifyLines();
	virtual void		Open();
	virtual	void		Close();
	virtual	void		Print( const char* text );
	// virtual	void		Draw( bool forceFullScreen );
	
	// virtual void		PrintOverlay( budOverlayHandle& handle, justify_t justify, const char* text, ... );
	
	// virtual budDebugGraph* 	CreateGraph( int numItems );
	// virtual void			DestroyGraph( budDebugGraph* graph );
	
	void				Dump( const char* toFile );
	void				Clear();

private:
	void				Resize();
	
	void				KeyDownEvent( int key );
	
	void				Linefeed();
	
	void				PageUp();
	void				PageDown();
	void				Top();
	void				Bottom();
	
	void				DrawInput();
	void				DrawNotify();
	void				DrawSolbudConsole( float frac );
	
	void				Scroll();
	void				SetDisplayFraction( float frac );
	void				UpdateDisplayFraction();
	
	void				DrawTextLeftAlign( float x, float& y, const char* text, ... );
	void				DrawTextRightAlign( float x, float& y, const char* text, ... );
	
	float				DrawFPS( float y );
	float				DrawMemoryUsage( float y );
	
	void				DrawOverlayText( float& leftY, float& rightY, float& centerY );
	void				DrawDebugGraphs();
	
	//============================
	
	// allow these constants to be adjusted for HMD
	int					LOCALSAFE_LEFT;
	int					LOCALSAFE_RIGHT;
	int					LOCALSAFE_TOP;
	int					LOCALSAFE_BOTTOM;
	int					LOCALSAFE_WIDTH;
	int					LOCALSAFE_HEIGHT;
	int					LINE_WIDTH;
	int					TOTAL_LINES;
	
	bool				keyCatching;
	
	short				text[CON_TEXTSIZE];
	int					current;		// line where next message will be printed
	int					x;				// offset in current line for next print
	int					display;		// bottom of console displays this line
	int					lastKeyEvent;	// time of last key event for scroll delay
	int					nextKeyEvent;	// keyboard repeat rate
	
	float				displayFrac;	// approaches finalFrac at con_speed
	float				finalFrac;		// 0.0 to 1.0 lines of console to display
	int					fracTime;		// time of last displayFrac update
	
	int					vislines;		// in scanlines
	
	int					times[NUM_CON_TIMES];	// cls.realtime time the line was generated
	// for transparent notify lines
	budVec4				color;
	
	// idEditField			historyEditLines[COMMAND_HISTORY];
	
	int					nextHistoryLine;// the last line in the history buffer, not masked
	int					historyLine;	// the line being displayed from history buffer
	// will be <= nextHistoryLine
	
	// idEditField			consoleField;
	
	budList< overlayText_t >	overlayText;
	// budList< budDebugGraph*> debugGraphs;
	
	int					lastVirtualScreenWidth;
	int					lastVirtualScreenHeight;
	
	static budCVar		con_speed;
	static budCVar		con_notifyTime;
	static budCVar		con_noPrint;
};

static budConsoleLocal localConsole;
budConsole* console = &localConsole;

budCVar budConsoleLocal::con_speed( "con_speed", "3", CVAR_SYSTEM, "speed at which the console moves up and down" );
budCVar budConsoleLocal::con_notifyTime( "con_notifyTime", "3", CVAR_SYSTEM, "time messages are displayed onscreen when console is pulled up" );
#ifdef DEBUG
budCVar budConsoleLocal::con_noPrint( "con_noPrint", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT, "print on the console but not onscreen when console is pulled up" );
#else
budCVar budConsoleLocal::con_noPrint( "con_noPrint", "1", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT, "print on the console but not onscreen when console is pulled up" );
#endif

/*
=============================================================================

	Misc stats

=============================================================================
*/

//=========================================================================

/*
==============
Con_Clear_f
==============
*/
static void Con_Clear_f( const budCmdArgs& args )
{
	localConsole.Clear();
}

/*
==============
Con_Dump_f
==============
*/
static void Con_Dump_f( const budCmdArgs& args )
{
	if( args.Argc() != 2 )
	{
		common->Printf( "usage: conDump <filename>\n" );
		return;
	}
	
	budStr fileName = args.Argv( 1 );
	fileName.DefaultFileExtension( ".txt" );
	
	common->Printf( "Dumped console text to %s.\n", fileName.c_str() );
	
	localConsole.Dump( fileName.c_str() );
}

/*
==============
budConsoleLocal::Init
==============
*/
void budConsoleLocal::Init()
{

	int		i;
	
	keyCatching = false;
	
	// LOCALSAFE_LEFT		= 32;
	// LOCALSAFE_RIGHT		= SCREEN_WIDTH - LOCALSAFE_LEFT;
	// LOCALSAFE_TOP		= 24;
	// LOCALSAFE_BOTTOM	= SCREEN_HEIGHT - LOCALSAFE_TOP;
	// LOCALSAFE_WIDTH		= LOCALSAFE_RIGHT - LOCALSAFE_LEFT;
	// LOCALSAFE_HEIGHT	= LOCALSAFE_BOTTOM - LOCALSAFE_TOP;
	
	// LINE_WIDTH = ( ( LOCALSAFE_WIDTH / SMALLCHAR_WIDTH ) - 2 );
	// TOTAL_LINES = ( CON_TEXTSIZE / LINE_WIDTH );
	
	// lastKeyEvent = -1;
	// nextKeyEvent = CONSOLE_FIRSTREPEAT;
	
	// consoleField.Clear();
	// consoleField.SetWidthInChars( LINE_WIDTH );
	
	// for( i = 0 ; i < COMMAND_HISTORY ; i++ )
	// {
	// 	historyEditLines[i].Clear();
	// 	historyEditLines[i].SetWidthInChars( LINE_WIDTH );
	// }
	
	cmdSystem->AddCommand( "clear", Con_Clear_f, CMD_FL_SYSTEM, "clears the console" );
	cmdSystem->AddCommand( "conDump", Con_Dump_f, CMD_FL_SYSTEM, "dumps the console text to a file" );
}

/*
==============
budConsoleLocal::Shutdown
==============
*/
void budConsoleLocal::Shutdown()
{
	cmdSystem->RemoveCommand( "clear" );
	cmdSystem->RemoveCommand( "conDump" );
}

/*
================
budConsoleLocal::Active
================
*/
bool	budConsoleLocal::Active()
{
	return keyCatching;
}

/*
================
budConsoleLocal::ClearNotifyLines
================
*/
void	budConsoleLocal::ClearNotifyLines()
{
	int		i;
	
	for( i = 0 ; i < NUM_CON_TIMES ; i++ )
	{
		times[i] = 0;
	}
}

/*
================
budConsoleLocal::Open
================
*/
void	budConsoleLocal::Open()
{
	if( keyCatching )
		return; // already open
		
	// consoleField.ClearAutoComplete();
	// consoleField.Clear();
	// keyCatching = true;
	// SetDisplayFraction( 0.5f );

}

/*
================
budConsoleLocal::Close
================
*/
void	budConsoleLocal::Close()
{
	keyCatching = false;
	SetDisplayFraction( 0 );
	displayFrac = 0;	// don't scroll to that point, go immediately
	ClearNotifyLines();
}

/*
================
budConsoleLocal::Clear
================
*/
void budConsoleLocal::Clear()
{
	int		i;
	
	for( i = 0 ; i < CON_TEXTSIZE ; i++ )
	{
		text[i] = ( budStr::ColorIndex( C_COLOR_CYAN ) << 8 ) | ' ';
	}
	
	Bottom();		// go to end
}

/*
================
budConsoleLocal::Dump

Save the console contents out to a file
================
*/
void budConsoleLocal::Dump( const char* fileName )
{
	int		l, x, i;
	short* 	line;
	budFile* f;
	char*	 buffer = ( char* )alloca( LINE_WIDTH + 3 );
	
	f = fileSystem->OpenFileWrite( fileName );
	if( !f )
	{
		common->Warning( "couldn't open %s", fileName );
		return;
	}
	
	// skip empty lines
	l = current - TOTAL_LINES + 1;
	if( l < 0 )
	{
		l = 0;
	}
	for( ; l <= current ; l++ )
	{
		line = text + ( l % TOTAL_LINES ) * LINE_WIDTH;
		for( x = 0; x < LINE_WIDTH; x++ )
			if( ( line[x] & 0xff ) > ' ' )
				break;
		if( x != LINE_WIDTH )
			break;
	}
	
	// write the remaining lines
	for( ; l <= current; l++ )
	{
		line = text + ( l % TOTAL_LINES ) * LINE_WIDTH;
		for( i = 0; i < LINE_WIDTH; i++ )
		{
			buffer[i] = line[i] & 0xff;
		}
		for( x = LINE_WIDTH - 1; x >= 0; x-- )
		{
			if( buffer[x] <= ' ' )
			{
				buffer[x] = 0;
			}
			else
			{
				break;
			}
		}
		buffer[x + 1] = '\r';
		buffer[x + 2] = '\n';
		buffer[x + 3] = 0;
		f->Write( buffer, strlen( buffer ) );
	}
	
	fileSystem->CloseFile( f );
}

/*
==============
budConsoleLocal::Resize
==============
*/
// void budConsoleLocal::Resize()
// {
// 	if( renderSystem->GetVirtualWidth() == lastVirtualScreenWidth && renderSystem->GetVirtualHeight() == lastVirtualScreenHeight )
// 		return;
		
// 	lastVirtualScreenWidth = renderSystem->GetVirtualWidth();
// 	lastVirtualScreenHeight = renderSystem->GetVirtualHeight();
// 	LOCALSAFE_RIGHT		= renderSystem->GetVirtualWidth() - LOCALSAFE_LEFT;
// 	LOCALSAFE_BOTTOM	= renderSystem->GetVirtualHeight() - LOCALSAFE_TOP;
// 	LOCALSAFE_WIDTH		= LOCALSAFE_RIGHT - LOCALSAFE_LEFT;
// 	LOCALSAFE_HEIGHT	= LOCALSAFE_BOTTOM - LOCALSAFE_TOP;
// }

/*
================
budConsoleLocal::PageUp
================
*/
void budConsoleLocal::PageUp()
{
	display -= 2;
	if( current - display >= TOTAL_LINES )
	{
		display = current - TOTAL_LINES + 1;
	}
}

/*
================
budConsoleLocal::PageDown
================
*/
void budConsoleLocal::PageDown()
{
	display += 2;
	if( display > current )
	{
		display = current;
	}
}

/*
================
budConsoleLocal::Top
================
*/
void budConsoleLocal::Top()
{
	display = 0;
}

/*
================
budConsoleLocal::Bottom
================
*/
void budConsoleLocal::Bottom()
{
	display = current;
}


/*
=============================================================================

CONSOLE LINE EDITING

==============================================================================
*/

/*
====================
KeyDownEvent

Handles history and console scrollback
====================
*/
void budConsoleLocal::KeyDownEvent( int key )
{

	// Execute F key bindings
	if( key >= K_F1 && key <= K_F12 )
	{
		idKeyInput::ExecKeyBinding( key );
		return;
	}
	
	// ctrl-L clears screen
	if( key == K_L && ( idKeyInput::IsDown( K_LCTRL ) || idKeyInput::IsDown( K_RCTRL ) ) )
	{
		Clear();
		return;
	}
	
	// command completion
	
	// command history (ctrl-p ctrl-n for unix style)
	
	if( ( key == K_UPARROW ) ||
			( key == K_P && ( idKeyInput::IsDown( K_LCTRL ) || idKeyInput::IsDown( K_RCTRL ) ) ) )
	{
		budStr hist = consoleHistory.RetrieveFromHistory( true );
		return;
	}
	
	if( ( key == K_DOWNARROW ) ||
			( key == K_N && ( idKeyInput::IsDown( K_LCTRL ) || idKeyInput::IsDown( K_RCTRL ) ) ) )
	{
		budStr hist = consoleHistory.RetrieveFromHistory( false );
		
		return;
	}
	
	// console scrolling
	if( key == K_PGUP )
	{
		PageUp();
		lastKeyEvent = eventLoop->Milliseconds();
		nextKeyEvent = CONSOLE_FIRSTREPEAT;
		return;
	}
	
	if( key == K_PGDN )
	{
		PageDown();
		lastKeyEvent = eventLoop->Milliseconds();
		nextKeyEvent = CONSOLE_FIRSTREPEAT;
		return;
	}
	
	if( key == K_MWHEELUP )
	{
		PageUp();
		return;
	}
	
	if( key == K_MWHEELDOWN )
	{
		PageDown();
		return;
	}
	
	// ctrl-home = top of console
	if( key == K_HOME && ( idKeyInput::IsDown( K_LCTRL ) || idKeyInput::IsDown( K_RCTRL ) ) )
	{
		Top();
		return;
	}
	
	// ctrl-end = bottom of console
	if( key == K_END && ( idKeyInput::IsDown( K_LCTRL ) || idKeyInput::IsDown( K_RCTRL ) ) )
	{
		Bottom();
		return;
	}
	
}

/*
==============
Scroll
deals with scrolling text because we don't have key repeat
==============
*/
void budConsoleLocal::Scroll( )
{
	if( lastKeyEvent == -1 || ( lastKeyEvent + 200 ) > eventLoop->Milliseconds() )
	{
		return;
	}
	// console scrolling
	if( idKeyInput::IsDown( K_PGUP ) )
	{
		PageUp();
		nextKeyEvent = CONSOLE_REPEAT;
		return;
	}
	
	if( idKeyInput::IsDown( K_PGDN ) )
	{
		PageDown();
		nextKeyEvent = CONSOLE_REPEAT;
		return;
	}
}

/*
==============
SetDisplayFraction

Causes the console to start opening the desired amount.
==============
*/
void budConsoleLocal::SetDisplayFraction( float frac )
{
	finalFrac = frac;
	fracTime = Sys_Milliseconds();
}

/*
==============
UpdateDisplayFraction

Scrolls the console up or down based on conspeed
==============
*/
void budConsoleLocal::UpdateDisplayFraction()
{
	if( con_speed.GetFloat() <= 0.1f )
	{
		fracTime = Sys_Milliseconds();
		displayFrac = finalFrac;
		return;
	}
	
	// scroll towards the destination height
	if( finalFrac < displayFrac )
	{
		displayFrac -= con_speed.GetFloat() * ( Sys_Milliseconds() - fracTime ) * 0.001f;
		if( finalFrac > displayFrac )
		{
			displayFrac = finalFrac;
		}
		fracTime = Sys_Milliseconds();
	}
	else if( finalFrac > displayFrac )
	{
		displayFrac += con_speed.GetFloat() * ( Sys_Milliseconds() - fracTime ) * 0.001f;
		if( finalFrac < displayFrac )
		{
			displayFrac = finalFrac;
		}
		fracTime = Sys_Milliseconds();
	}
}

/*
==============
ProcessEvent
==============
*/
bool	budConsoleLocal::ProcessEvent( const sysEvent_t* event, bool forceAccept )
{
	const bool consoleKey = event->evType == SE_KEY && event->evValue == K_GRAVE && com_allowConsole.GetBool();
	
	// we always catch the console key event
	if( !forceAccept && consoleKey )
	{
		// ignore up events
		if( event->evValue2 == 0 )
		{
			return true;
		}
		
		// a down event will toggle the destination lines
		if( keyCatching )
		{
			Close();
			Sys_GrabMouseCursor( true );
		}
		else
		{
			keyCatching = true;
			if( idKeyInput::IsDown( K_LSHIFT ) || idKeyInput::IsDown( K_RSHIFT ) )
			{
				// if the shift key is down, don't open the console as much
				SetDisplayFraction( 0.2f );
			}
			else
			{
				SetDisplayFraction( 0.5f );
			}
		}
		return true;
	}
	
	// if we aren't key catching, dump all the other events
	if( !forceAccept && !keyCatching )
	{
		return false;
	}
	
	if( event->evType == SE_KEY )
	{
		// ignore up key events
		if( event->evValue2 == 0 )
		{
			return true;
		}
		
		KeyDownEvent( event->evValue );
		return true;
	}
	
	// we don't handle things like mouse, joystick, and network packets
	return false;
}

/*
==============================================================================

PRINTING

==============================================================================
*/

/*
===============
Linefeed
===============
*/
void budConsoleLocal::Linefeed()
{
	int		i;
	
	// mark time for transparent overlay
	if( current >= 0 )
	{
		times[current % NUM_CON_TIMES] = Sys_Milliseconds();
	}
	
	x = 0;
	if( display == current )
	{
		display++;
	}
	current++;
	for( i = 0; i < LINE_WIDTH; i++ )
	{
		int offset = ( ( unsigned int )current % TOTAL_LINES ) * LINE_WIDTH + i;
		text[offset] = ( budStr::ColorIndex( C_COLOR_CYAN ) << 8 ) | ' ';
	}
}


/*
================
Print

Handles cursor positioning, line wrapping, etc
================
*/
void budConsoleLocal::Print( const char* txt )
{
	int		y;
	int		c, l;
	int		color;
	
	if( TOTAL_LINES == 0 )
	{
		// not yet initialized
		return;
	}
	
	color = budStr::ColorIndex( C_COLOR_CYAN );
	
	while( ( c = *( const unsigned char* )txt ) != 0 )
	{
		if( budStr::IsColor( txt ) )
		{
			if( *( txt + 1 ) == C_COLOR_DEFAULT )
			{
				color = budStr::ColorIndex( C_COLOR_CYAN );
			}
			else
			{
				color = budStr::ColorIndex( *( txt + 1 ) );
			}
			txt += 2;
			continue;
		}
		
		y = current % TOTAL_LINES;
		
		// if we are about to print a new word, check to see
		// if we should wrap to the new line
		if( c > ' ' && ( x == 0 || text[y * LINE_WIDTH + x - 1] <= ' ' ) )
		{
			// count word length
			for( l = 0 ; l < LINE_WIDTH ; l++ )
			{
				if( txt[l] <= ' ' )
				{
					break;
				}
			}
			
			// word wrap
			if( l != LINE_WIDTH && ( x + l >= LINE_WIDTH ) )
			{
				Linefeed();
			}
		}
		
		txt++;
		
		switch( c )
		{
			case '\n':
				Linefeed();
				break;
			case '\t':
				do
				{
					text[y * LINE_WIDTH + x] = ( color << 8 ) | ' ';
					x++;
					if( x >= LINE_WIDTH )
					{
						Linefeed();
						x = 0;
					}
				}
				while( x & 3 );
				break;
			case '\r':
				x = 0;
				break;
			default:	// display character and advance
				text[y * LINE_WIDTH + x] = ( color << 8 ) | c;
				x++;
				if( x >= LINE_WIDTH )
				{
					Linefeed();
					x = 0;
				}
				break;
		}
	}
	
	
	// mark time for transparent overlay
	if( current >= 0 )
	{
		times[current % NUM_CON_TIMES] = Sys_Milliseconds();
	}
}