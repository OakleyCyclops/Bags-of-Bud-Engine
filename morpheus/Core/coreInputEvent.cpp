/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2012 dhewg (dhewm3)
Copyright (C) 2012 Robert Beckebans
Copyright (C) 2013 Daniel Gibson

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

#include "corePCH.hpp"
#pragma hdrstop

// DG: SDL.h somehow needs the following functions, so #undef those silly
//     "don't use" #defines from Str.h
#undef strncmp
#undef strcasecmp
#undef vsnprintf
// DG end

#include <SDL2/SDL.h>

#include "coreLocal.hpp"
#include "unix/unixPublic.hpp"

// DG: those are needed for moving/resizing windows
// extern budCVar r_windowX;
// extern budCVar r_windowY;
// extern budCVar r_windowWidth;
// extern budCVar r_windowHeight;
// DG end

const char* kbdNames[] =
{
	"english", "french", "german", "italian", "spanish", "turkish", "norwegian", NULL
};

budCVar in_keyboard( "in_keyboard", "english", CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_NOCHEAT, "keyboard layout", kbdNames, budCmdSystem::ArgCompletion_String<kbdNames> );

struct kbd_poll_t
{
	int key;
	bool state;
	
	kbd_poll_t()
	{
	}
	
	kbd_poll_t( int k, bool s )
	{
		key = k;
		state = s;
	}
};

struct mouse_poll_t
{
	int action;
	int value;
	
	mouse_poll_t()
	{
	}
	
	mouse_poll_t( int a, int v )
	{
		action = a;
		value = v;
	}
};

static budList<kbd_poll_t> kbd_polls;
static budList<mouse_poll_t> mouse_polls;

struct joystick_poll_t
{
	int action;
	int value;
	
	joystick_poll_t()
	{
	}
	
	joystick_poll_t( int a, int v )
	{
		action = a;
		value = v;
	}
};
static budList<joystick_poll_t> joystick_polls;
SDL_Joystick* joy = NULL;
int SDL_joystick_has_hat = 0;
bool buttonStates[K_LAST_KEY];	// For keeping track of button up/down events

#include "sdl2_scancode_mappings.hpp"

static int SDLScanCodeToKeyNum( SDL_Scancode sc )
{
	int idx = int( sc );
	assert( idx >= 0 && idx < SDL_NUM_SCANCODES );
	
	return scanCodeToKeyNum[idx];
}

static SDL_Scancode KeyNumToSDLScanCode( int keyNum )
{
	if( keyNum < K_JOY1 )
	{
		for( int i = 0; i < SDL_NUM_SCANCODES; ++i )
		{
			if( scanCodeToKeyNum[i] == keyNum )
			{
				return SDL_Scancode( i );
			}
		}
	}
	return SDL_SCANCODE_UNKNOWN;
}

// both strings are expected to have at most SDL_TEXTINPUTEVENT_TEXT_SIZE chars/ints (including terminating null)
static void ConvertUTF8toUTF32( const char* utf8str, int32* utf32buf )
{
	static SDL_iconv_t cd = SDL_iconv_t( -1 );
	
	if( cd == SDL_iconv_t( -1 ) )
	{
		const char* toFormat = "UTF-32LE"; // TODO: what does d3bfg expect on big endian machines?
		cd = SDL_iconv_open( toFormat, "UTF-8" );
		if( cd == SDL_iconv_t( -1 ) )
		{
			common->Warning( "Couldn't initialize SDL_iconv for UTF-8 to UTF-32!" ); // TODO: or error?
			return;
		}
	}
	
	size_t len = strlen( utf8str );
	
	size_t inbytesleft = len;
	size_t outbytesleft = 4 * SDL_TEXTINPUTEVENT_TEXT_SIZE; // *4 because utf-32 needs 4x as much space as utf-8
	char* outbuf = ( char* )utf32buf;
	size_t n = SDL_iconv( cd, &utf8str, &inbytesleft, &outbuf, &outbytesleft );
	
	if( n == size_t( -1 ) ) // some error occured during iconv
	{
		common->Warning( "Converting UTF-8 string \"%s\" from SDL_TEXTINPUT to UTF-32 failed!", utf8str );
		
		// clear utf32-buffer, just to be sure there's no garbage..
		memset( utf32buf, 0, SDL_TEXTINPUTEVENT_TEXT_SIZE * sizeof( int32 ) );
	}
	
	// reset cd so it can be used again
	SDL_iconv( cd, NULL, &inbytesleft, NULL, &outbytesleft );
	
}

static void PushConsoleEvent( const char* s )
{
	char* b;
	size_t len;
	
	len = strlen( s ) + 1;
	b = ( char* )Mem_Alloc( len, TAG_EVENTS );
	strcpy( b, s );
	
	SDL_Event event;
	
	event.type = SDL_USEREVENT;
	event.user.code = SE_CONSOLE;
	event.user.data1 = ( void* )len;
	event.user.data2 = b;
	
	SDL_PushEvent( &event );
}



/*
=================
Sys_InitInput
=================
*/
void Sys_InitInput()
{
	int numJoysticks, i;
	
	kbd_polls.SetGranularity( 64 );
	mouse_polls.SetGranularity( 64 );
	
	memset( buttonStates, 0, sizeof( buttonStates ) );
	
	in_keyboard.SetModified();
	
	// GameController
	if( SDL_Init( SDL_INIT_GAMECONTROLLER ) )
		common->Printf( "Sys_InitInput: SDL_INIT_GAMECONTROLLER error: %s\n", SDL_GetError() );
		
	SDL_GameController* controller = NULL;
	for( int i = 0; i < SDL_NumJoysticks(); ++i )
	{
		if( SDL_IsGameController( i ) )
		{
			controller = SDL_GameControllerOpen( i );
			if( controller )
			{
				common->Printf( "GameController %i name: %s\n", i, SDL_GameControllerName( controller ) );
				common->Printf( "GameController %i is mapped as \"%s\".\n", i, SDL_GameControllerMapping( controller ) );
			}
			else
			{
				common->Printf( "Could not open gamecontroller %i: %s\n", i, SDL_GetError() );
			}
			
		}
	}
	// WM0110: Initialise SDL Joystick
	common->Printf( "Sys_InitInput: Joystick subsystem init\n" );
	if( SDL_Init( SDL_INIT_JOYSTICK ) )
	{
		common->Printf( "Sys_InitInput: Joystic Init ERROR!\n" );
	}
	
	numJoysticks = SDL_NumJoysticks();
	common->Printf( "Sys_InitInput: Joystic - Found %i joysticks\n", numJoysticks );
	
	// Open first available joystick and use it
	if( SDL_NumJoysticks() > 0 )
	{
		joy = SDL_JoystickOpen( 0 );
	
		if( joy )
		{
			int num_hats;
	
			num_hats = SDL_JoystickNumHats( joy );
			common->Printf( "Opened Joystick number 0\n" );
			common->Printf( "Name: %s\n", SDL_JoystickName( joy ) );
			common->Printf( "Number of Axes: %d\n", SDL_JoystickNumAxes( joy ) );
			common->Printf( "Number of Buttons: %d\n", SDL_JoystickNumButtons( joy ) );
			common->Printf( "Number of Hats: %d\n", num_hats );
			common->Printf( "Number of Balls: %d\n", SDL_JoystickNumBalls( joy ) );
	
			SDL_joystick_has_hat = 0;
			if( num_hats )
			{
				SDL_joystick_has_hat = 1;
			}
		}
		else
		{
			joy = NULL;
			common->Printf( "Couldn't open Joystick 0\n" );
		}
	}
	else
	{
		joy = NULL;
	}
	// WM0110
}

/*
=================
Sys_ShutdownInput
=================
*/
void Sys_ShutdownInput()
{
	kbd_polls.Clear();
	mouse_polls.Clear();
	joystick_polls.Clear();
	
	memset( buttonStates, 0, sizeof( buttonStates ) );
	
	// Close any opened SDL Joystic
	if( joy )
	{
		common->Printf( "Sys_ShutdownInput: closing SDL joystick.\n" );
		SDL_JoystickClose( joy );
	}
	else
	{
		common->Printf( "Sys_ShutdownInput: SDL joystick not initialized. Nothing to close.\n" );
	}
}

/*
===========
Sys_InitScanTable
===========
*/
// Windows has its own version due to the tools
#ifndef _WIN32
void Sys_InitScanTable()
{
}
#endif

/*
===============
Sys_GetConsoleKey
===============
*/
unsigned char Sys_GetConsoleKey( bool shifted )
{
	static unsigned char keys[2] = { '`', '~' };
	
	if( in_keyboard.IsModified() )
	{
		budStr lang = in_keyboard.GetString();
		
		if( lang.Length() )
		{
			if( !lang.Icmp( "french" ) )
			{
				keys[0] = '<';
				keys[1] = '>';
			}
			else if( !lang.Icmp( "german" ) )
			{
				keys[0] = '^';
				keys[1] = 176; // °
			}
			else if( !lang.Icmp( "italian" ) )
			{
				keys[0] = '\\';
				keys[1] = '|';
			}
			else if( !lang.Icmp( "spanish" ) )
			{
				keys[0] = 186; // º
				keys[1] = 170; // ª
			}
			else if( !lang.Icmp( "turkish" ) )
			{
				keys[0] = '"';
				keys[1] = 233; // é
			}
			else if( !lang.Icmp( "norwegian" ) )
			{
				keys[0] = 124; // |
				keys[1] = 167; // §
			}
		}
		
		in_keyboard.ClearModified();
	}
	
	return shifted ? keys[1] : keys[0];
}

/*
===============
Sys_MapCharForKey
===============
*/
unsigned char Sys_MapCharForKey( int key )
{
	return key & 0xff;
}

/*
===============
Sys_GrabMouseCursor
===============
*/
void Sys_GrabMouseCursor( bool grabIt )
{
	int flags;
	
	if( grabIt )
	{
		// DG: disabling the cursor is now done once in GLimp_Init() because it should always be disabled
		flags = GRAB_ENABLE | GRAB_SETSTATE;
		// DG end
	}
	else
	{
		flags = GRAB_SETSTATE;
	}
	
	// GLimp_GrabInput( flags );
}

/*
================
Sys_GetEvent
================
*/
sysEvent_t Sys_GetEvent()
{
	sysEvent_t res = { };
	
	SDL_Event ev;
	int key;
	
	// when this is returned, it's assumed that there are no more events!
	static const sysEvent_t no_more_events = { SE_NONE, 0, 0, 0, NULL };
	
	// WM0110: previous state of joystick hat
	static int previous_hat_state = SDL_HAT_CENTERED;
	
	// utf-32 version of the textinput event
	static int32 uniStr[SDL_TEXTINPUTEVENT_TEXT_SIZE] = {0};
	static size_t uniStrPos = 0;
	
	if( uniStr[0] != 0 )
	{
		res.evType = SE_CHAR;
		res.evValue = uniStr[uniStrPos];
		
		++uniStrPos;
		
		if( !uniStr[uniStrPos] || uniStrPos == SDL_TEXTINPUTEVENT_TEXT_SIZE )
		{
			memset( uniStr, 0, sizeof( uniStr ) );
			uniStrPos = 0;
		}
		
		return res;
	}
	
	// DG: fake a "mousewheel not pressed anymore" event for SDL2
	// so scrolling in menus stops after one step
	static int mwheelRel = 0;
	if( mwheelRel )
	{
		res.evType = SE_KEY;
		res.evValue = mwheelRel;
		res.evValue2 = 0; // "not pressed anymore"
		mwheelRel = 0;
		return res;
	}
	// DG end
	
	static int32 uniChar = 0;
	
	if( uniChar )
	{
		res.evType = SE_CHAR;
		res.evValue = uniChar;
		
		uniChar = 0;
		
		return res;
	}
	
	// loop until there is an event we care about (will return then) or no more events
	while( SDL_PollEvent( &ev ) )
	{
		switch( ev.type )
		{
			case SDL_WINDOWEVENT:
				switch( ev.window.event )
				{
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					{
						// unset modifier, in case alt-tab was used to leave window and ALT is still set
						// as that can cause fullscreen-toggling when pressing enter...
						SDL_Keymod currentmod = SDL_GetModState();
						int newmod = KMOD_NONE;
						if( currentmod & KMOD_CAPS ) // preserve capslock
							newmod |= KMOD_CAPS;
							
						SDL_SetModState( ( SDL_Keymod )newmod );
						
						// DG: un-pause the game when focus is gained, that also re-grabs the input
						//     disabling the cursor is now done once in GLimp_Init() because it should always be disabled
						cvarSystem->SetCVarBool( "com_pause", false );
						// DG end
						break;
					}
					
					case SDL_WINDOWEVENT_FOCUS_LOST:
						// DG: pause the game when focus is lost, that also un-grabs the input
						cvarSystem->SetCVarBool( "com_pause", true );
						// DG end
						break;
						
					case SDL_WINDOWEVENT_LEAVE:
						// mouse has left the window
						res.evType = SE_MOUSE_LEAVE;
						return res;
						
					// DG: handle resizing and moving of window
					case SDL_WINDOWEVENT_RESIZED:
					{
						int w = ev.window.data1;
						int h = ev.window.data2;
						// r_windowWidth.SetInteger( w );
						// r_windowHeight.SetInteger( h );
						
						break;
					}
					
					case SDL_WINDOWEVENT_MOVED:
					{
						int x = ev.window.data1;
						int y = ev.window.data2;
						// r_windowX.SetInteger( x );
						// r_windowY.SetInteger( y );
						break;
					}
				}
				
				continue; // handle next event
			
			case SDL_KEYDOWN:
				if( ev.key.keysym.sym == SDLK_RETURN && ( ev.key.keysym.mod & KMOD_ALT ) > 0 )
				{
					// DG: go to fullscreen on current display, instead of always first display
					int fullscreen = 0;
					cvarSystem->SetCVarInteger( "r_fullscreen", fullscreen );
					// DG end
					PushConsoleEvent( "vid_restart" );
					continue; // handle next event
				}
				
				// DG: ctrl-g to un-grab mouse - yeah, left ctrl shoots, then just use right ctrl :)
				if( ev.key.keysym.sym == SDLK_g && ( ev.key.keysym.mod & KMOD_CTRL ) > 0 )
				{
					bool grab = cvarSystem->GetCVarBool( "in_nograb" );
					grab = !grab;
					cvarSystem->SetCVarBool( "in_nograb", grab );
					continue; // handle next event
				}
				// DG end
				
			// fall through
			case SDL_KEYUP:
			{
				bool isChar;
				
				// DG: special case for SDL_SCANCODE_GRAVE - the console key under Esc
				if( ev.key.keysym.scancode == SDL_SCANCODE_GRAVE )
				{
					key = K_GRAVE;
					uniChar = K_BACKSPACE; // bad hack to get empty console inputline..
				} // DG end, the original code is in the else case
				else
				{
					key = SDLScanCodeToKeyNum( ev.key.keysym.scancode );
					
					if( key == 0 )
					{
						// SDL2 has no ev.key.keysym.unicode anymore.. but the scancode should work well enough for console
						if( ev.type == SDL_KEYDOWN ) // FIXME: don't complain if this was an ASCII char and the console is open?
							common->Warning( "unmapped SDL key %d scancode %d", ev.key.keysym.sym, ev.key.keysym.scancode );
							
						continue; // just handle next event
					}

				}
				
				res.evType = SE_KEY;
				res.evValue = key;
				res.evValue2 = ev.key.state == SDL_PRESSED ? 1 : 0;
				
				kbd_polls.Append( kbd_poll_t( key, ev.key.state == SDL_PRESSED ) );
				
				if( key == K_BACKSPACE && ev.key.state == SDL_PRESSED )
					uniChar = key;
					
				return res;
			}
			case SDL_TEXTINPUT:
				if( ev.text.text[0] != '\0' )
				{
					// fill uniStr array for SE_CHAR events
					ConvertUTF8toUTF32( ev.text.text, uniStr );
					
					// return an event with the first/only char
					res.evType = SE_CHAR;
					res.evValue = uniStr[0];
					
					uniStrPos = 1;
					
					if( uniStr[1] == 0 )
					{
						// it's just this one character, clear uniStr
						uniStr[0] = 0;
						uniStrPos = 0;
					}
					return res;
				}
				
				continue; // just handle next event
				
			case SDL_MOUSEMOTION:
				// DG: return event with absolute mouse-coordinates when in menu
				// to fix cursor problems in windowed mode
				// DG end
				
				mouse_polls.Append( mouse_poll_t( M_DELTAX, ev.motion.xrel ) );
				mouse_polls.Append( mouse_poll_t( M_DELTAY, ev.motion.yrel ) );
				
				return res;
				
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			case SDL_FINGERMOTION:
				continue; // Avoid 'unknown event' spam when testing with touchpad by skipping this
				
			case SDL_MOUSEWHEEL:
				res.evType = SE_KEY;
				
				res.evValue = ( ev.wheel.y > 0 ) ? K_MWHEELUP : K_MWHEELDOWN;
				mouse_polls.Append( mouse_poll_t( M_DELTAZ, ev.wheel.y ) );
				
				res.evValue2 = 1; // for "pressed"
				
				// remember mousewheel direction to issue a "not pressed anymore" event
				mwheelRel = res.evValue;
				
				return res;
				
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				res.evType = SE_KEY;
				
				switch( ev.button.button )
				{
					case SDL_BUTTON_LEFT:
						res.evValue = K_MOUSE1;
						mouse_polls.Append( mouse_poll_t( M_ACTION1, ev.button.state == SDL_PRESSED ? 1 : 0 ) );
						break;
					case SDL_BUTTON_MIDDLE:
						res.evValue = K_MOUSE3;
						mouse_polls.Append( mouse_poll_t( M_ACTION3, ev.button.state == SDL_PRESSED ? 1 : 0 ) );
						break;
					case SDL_BUTTON_RIGHT:
						res.evValue = K_MOUSE2;
						mouse_polls.Append( mouse_poll_t( M_ACTION2, ev.button.state == SDL_PRESSED ? 1 : 0 ) );
						break;
						
					default:
						// handle X1 button and above
						if( ev.button.button <= 16 ) // d3bfg doesn't support more than 16 mouse buttons
						{
							int buttonIndex = ev.button.button - SDL_BUTTON_LEFT;
							res.evValue = K_MOUSE1 + buttonIndex;
							mouse_polls.Append( mouse_poll_t( M_ACTION1 + buttonIndex, ev.button.state == SDL_PRESSED ? 1 : 0 ) );
						}
						else // unsupported mouse button
						{
							continue; // just ignore
						}
				}
				
				res.evValue2 = ev.button.state == SDL_PRESSED ? 1 : 0;
				
				return res;
				
			// GameController
			case SDL_JOYAXISMOTION:
			case SDL_JOYHATMOTION:
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
			case SDL_JOYDEVICEADDED:
			case SDL_JOYDEVICEREMOVED:
				// Avoid 'unknown event' spam
				continue;
				
			case SDL_CONTROLLERAXISMOTION:
				res.evType = SE_JOYSTICK;
				res.evValue = J_AXIS_LEFT_X + ( ev.caxis.axis - SDL_CONTROLLER_AXIS_LEFTX );
				res.evValue2 = ev.caxis.value;
				
				joystick_polls.Append( joystick_poll_t( res.evValue, res.evValue2 ) );
				return res;
				
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
				static int controllerButtonRemap[][2] =
				{
					{K_JOY1, J_ACTION1},
					{K_JOY2, J_ACTION2},
					{K_JOY3, J_ACTION3},
					{K_JOY4, J_ACTION4},
					{K_JOY9, J_ACTION9},
					{K_JOY11, J_ACTION11},
					{K_JOY10, J_ACTION10},
					{K_JOY7, J_ACTION7},
					{K_JOY8, J_ACTION8},
					{K_JOY5, J_ACTION5},
					{K_JOY6, J_ACTION6},
					{K_JOY_DPAD_UP, J_DPAD_UP},
					{K_JOY_DPAD_DOWN, J_DPAD_DOWN},
					{K_JOY_DPAD_LEFT, J_DPAD_LEFT},
					{K_JOY_DPAD_RIGHT, J_DPAD_RIGHT},
				};
				joystick_polls.Append( joystick_poll_t( controllerButtonRemap[ev.cbutton.button][1], ev.cbutton.state == SDL_PRESSED ? 1 : 0 ) );
				
				res.evType = SE_KEY;
				res.evValue = controllerButtonRemap[ev.cbutton.button][0];
				res.evValue2 = ev.cbutton.state == SDL_PRESSED ? 1 : 0;
				
				joystick_polls.Append( joystick_poll_t( res.evValue, res.evValue2 ) );
				return res;
				
			case SDL_QUIT:
				PushConsoleEvent( "quit" );
				res = no_more_events; // don't handle next event, just quit.
				return res;
				
			case SDL_USEREVENT:
				switch( ev.user.code )
				{
					case SE_CONSOLE:
						res.evType = SE_CONSOLE;
						res.evPtrLength = ( intptr_t )ev.user.data1;
						res.evPtr = ev.user.data2;
						return res;
					default:
						common->Warning( "unknown user event %u", ev.user.code );
				}
				continue; // just handle next event
			default:
				common->Warning( "unknown event %u", ev.type );
				continue; // just handle next event
		}
	}
	
	res = no_more_events;
	return res;
}

/*
================
Sys_ClearEvents
================
*/
void Sys_ClearEvents()
{
	SDL_Event ev;
	
	while( SDL_PollEvent( &ev ) )
		;
		
	kbd_polls.SetNum( 0 );
	mouse_polls.SetNum( 0 );
}

/*
================
Sys_GenerateEvents
================
*/
void Sys_GenerateEvents()
{
	char* s = Posix_ConsoleInput();
	
	if( s )
		PushConsoleEvent( s );
		
	SDL_PumpEvents();
}

/*
================
Sys_PollKeyboardInputEvents
================
*/
int Sys_PollKeyboardInputEvents()
{
	return kbd_polls.Num();
}

/*
================
Sys_ReturnKeyboardInputEvent
================
*/
int Sys_ReturnKeyboardInputEvent( const int n, int& key, bool& state )
{
	if( n >= kbd_polls.Num() )
		return 0;
		
	key = kbd_polls[n].key;
	state = kbd_polls[n].state;
	return 1;
}

/*
================
Sys_EndKeyboardInputEvents
================
*/
void Sys_EndKeyboardInputEvents()
{
	kbd_polls.SetNum( 0 );
}

/*
================
Sys_PollMouseInputEvents
================
*/
int Sys_PollMouseInputEvents( int mouseEvents[MAX_MOUSE_EVENTS][2] )
{
	int numEvents = mouse_polls.Num();
	
	if( numEvents > MAX_MOUSE_EVENTS )
	{
		numEvents = MAX_MOUSE_EVENTS;
	}
	
	for( int i = 0; i < numEvents; i++ )
	{
		const mouse_poll_t& mp = mouse_polls[i];
		
		mouseEvents[i][0] = mp.action;
		mouseEvents[i][1] = mp.value;
	}
	
	mouse_polls.SetNum( 0 );
	
	return numEvents;
}

const char* Sys_GetKeyName( keyNum_t keynum )
{
	// unfortunately, in SDL1.2 there is no way to get the keycode for a scancode, so this doesn't work there.
	// so this is SDL2-only.
#if SDL_VERSION_ATLEAST(2, 0, 0)
	
	SDL_Scancode scancode = KeyNumToSDLScanCode( ( int )keynum );
	SDL_Keycode keycode = SDL_GetKeyFromScancode( scancode );
	
	const char* ret = SDL_GetKeyName( keycode );
	if( ret != NULL && ret[0] != '\0' )
	{
		return ret;
	}
#endif
	return NULL;
}

char* Sys_GetClipboardData()
{
	char* txt = SDL_GetClipboardText();
	
	if( txt == NULL )
	{
		return NULL;
	}
	else if( txt[0] == '\0' )
	{
		SDL_free( txt );
		return NULL;
	}
	
	char* ret = Mem_CopyString( txt );
	SDL_free( txt );
	return ret;
}

void Sys_SetClipboardData( const char* string )
{
	SDL_SetClipboardText( string );
}


//=====================================================================================
//	Joystick Input Handling
//=====================================================================================

void Sys_SetRumble( int device, int low, int hi )
{
	// TODO;
	// SDL 2.0 required (SDL Haptic subsystem)
}

int Sys_PollJoystickInputEvents( int deviceNum )
{
	int numEvents = joystick_polls.Num();
	
	return numEvents;
}

// This funcion called by void budUsercmdGenLocal::Joystick( int deviceNum ) in
// file UsercmdGen.cpp
// action - must have values belonging to enum sys_jEvents (sys_public.h)
// value - must be 1/0 for button or DPAD pressed/released
//         for joystick axes must be in the range (-32769, 32768)
//         for joystick trigger must be in the range (0, 32768)
int Sys_ReturnJoystickInputEvent( const int n, int& action, int& value )
{
	// Get last element of the list and copy into argument references
	const joystick_poll_t& mp = joystick_polls[n];
	action = mp.action;
	value = mp.value;
	
	return 1;
}

// This funcion called by void budUsercmdGenLocal::Joystick( int deviceNum ) in
// file UsercmdGen.cpp
void Sys_EndJoystickInputEvents()
{
	// Empty the joystick event container. This is called after
	// all joystick events have been read using Sys_ReturnJoystickInputEvent()
	joystick_polls.SetNum( 0 );
}

typedef struct
{
	keyNum_t		keynum;
	const char* 	name;
	const char* 	strId;	// localized string id
} keyname_t;

#define NAMEKEY( code, strId ) { K_##code, #code, strId }
#define NAMEKEY2( code ) { K_##code, #code, #code }

#define ALIASKEY( alias, code ) { K_##code, alias, "" }

// names not in this list can either be lowercase ascii, or '0xnn' hex sequences
keyname_t keynames[] =
{
	NAMEKEY( ESCAPE, "#str_07020" ),
	NAMEKEY2( 1 ),
	NAMEKEY2( 2 ),
	NAMEKEY2( 3 ),
	NAMEKEY2( 4 ),
	NAMEKEY2( 5 ),
	NAMEKEY2( 6 ),
	NAMEKEY2( 7 ),
	NAMEKEY2( 8 ),
	NAMEKEY2( 9 ),
	NAMEKEY2( 0 ),
	NAMEKEY( MINUS, "-" ),
	NAMEKEY( EQUALS, "=" ),
	NAMEKEY( BACKSPACE, "#str_07022" ),
	NAMEKEY( TAB, "#str_07018" ),
	NAMEKEY2( Q ),
	NAMEKEY2( W ),
	NAMEKEY2( E ),
	NAMEKEY2( R ),
	NAMEKEY2( T ),
	NAMEKEY2( Y ),
	NAMEKEY2( U ),
	NAMEKEY2( I ),
	NAMEKEY2( O ),
	NAMEKEY2( P ),
	NAMEKEY( LBRACKET, "[" ),
	NAMEKEY( RBRACKET, "]" ),
	NAMEKEY( ENTER, "#str_07019" ),
	NAMEKEY( LCTRL, "#str_07028" ),
	NAMEKEY2( A ),
	NAMEKEY2( S ),
	NAMEKEY2( D ),
	NAMEKEY2( F ),
	NAMEKEY2( G ),
	NAMEKEY2( H ),
	NAMEKEY2( J ),
	NAMEKEY2( K ),
	NAMEKEY2( L ),
	NAMEKEY( SEMICOLON, "#str_07129" ),
	NAMEKEY( APOSTROPHE, "#str_07130" ),
	NAMEKEY( GRAVE, "`" ),
	NAMEKEY( LSHIFT, "#str_07029" ),
	NAMEKEY( BACKSLASH, "\\" ),
	NAMEKEY2( Z ),
	NAMEKEY2( X ),
	NAMEKEY2( C ),
	NAMEKEY2( V ),
	NAMEKEY2( B ),
	NAMEKEY2( N ),
	NAMEKEY2( M ),
	NAMEKEY( COMMA, "," ),
	NAMEKEY( PERIOD, "." ),
	NAMEKEY( SLASH, "/" ),
	NAMEKEY( RSHIFT, "#str_bind_RSHIFT" ),
	NAMEKEY( KP_STAR, "#str_07126" ),
	NAMEKEY( LALT, "#str_07027" ),
	NAMEKEY( SPACE, "#str_07021" ),
	NAMEKEY( CAPSLOCK, "#str_07034" ),
	NAMEKEY( F1, "#str_07036" ),
	NAMEKEY( F2, "#str_07037" ),
	NAMEKEY( F3, "#str_07038" ),
	NAMEKEY( F4, "#str_07039" ),
	NAMEKEY( F5, "#str_07040" ),
	NAMEKEY( F6, "#str_07041" ),
	NAMEKEY( F7, "#str_07042" ),
	NAMEKEY( F8, "#str_07043" ),
	NAMEKEY( F9, "#str_07044" ),
	NAMEKEY( F10, "#str_07045" ),
	NAMEKEY( NUMLOCK, "#str_07125" ),
	NAMEKEY( SCROLL, "#str_07035" ),
	NAMEKEY( KP_7, "#str_07110" ),
	NAMEKEY( KP_8, "#str_07111" ),
	NAMEKEY( KP_9, "#str_07112" ),
	NAMEKEY( KP_MINUS, "#str_07123" ),
	NAMEKEY( KP_4, "#str_07113" ),
	NAMEKEY( KP_5, "#str_07114" ),
	NAMEKEY( KP_6, "#str_07115" ),
	NAMEKEY( KP_PLUS, "#str_07124" ),
	NAMEKEY( KP_1, "#str_07116" ),
	NAMEKEY( KP_2, "#str_07117" ),
	NAMEKEY( KP_3, "#str_07118" ),
	NAMEKEY( KP_0, "#str_07120" ),
	NAMEKEY( KP_DOT, "#str_07121" ),
	NAMEKEY( F11, "#str_07046" ),
	NAMEKEY( F12, "#str_07047" ),
	NAMEKEY2( F13 ),
	NAMEKEY2( F14 ),
	NAMEKEY2( F15 ),
	NAMEKEY2( KANA ),
	NAMEKEY2( CONVERT ),
	NAMEKEY2( NOCONVERT ),
	NAMEKEY2( YEN ),
	NAMEKEY( KP_EQUALS, "#str_07127" ),
	NAMEKEY2( CIRCUMFLEX ),
	NAMEKEY( AT, "@" ),
	NAMEKEY( COLON, ":" ),
	NAMEKEY( UNDERLINE, "_" ),
	NAMEKEY2( KANJI ),
	NAMEKEY2( STOP ),
	NAMEKEY2( AX ),
	NAMEKEY2( UNLABELED ),
	NAMEKEY( KP_ENTER, "#str_07119" ),
	NAMEKEY( RCTRL, "#str_bind_RCTRL" ),
	NAMEKEY( KP_COMMA, "," ),
	NAMEKEY( KP_SLASH, "#str_07122" ),
	NAMEKEY( PRINTSCREEN, "#str_07179" ),
	NAMEKEY( RALT, "#str_bind_RALT" ),
	NAMEKEY( PAUSE, "#str_07128" ),
	NAMEKEY( HOME, "#str_07052" ),
	NAMEKEY( UPARROW, "#str_07023" ),
	NAMEKEY( PGUP, "#str_07051" ),
	NAMEKEY( LEFTARROW, "#str_07025" ),
	NAMEKEY( RIGHTARROW, "#str_07026" ),
	NAMEKEY( END, "#str_07053" ),
	NAMEKEY( DOWNARROW, "#str_07024" ),
	NAMEKEY( PGDN, "#str_07050" ),
	NAMEKEY( INS, "#str_07048" ),
	NAMEKEY( DEL, "#str_07049" ),
	NAMEKEY( LWIN, "#str_07030" ),
	NAMEKEY( RWIN, "#str_07031" ),
	NAMEKEY( APPS, "#str_07032" ),
	NAMEKEY2( POWER ),
	NAMEKEY2( SLEEP ),
	
	// DG: adding names for keys from cegui/directinput I added in enum keyNum_t in sys_public.h
	//     (they're really valid directinput scancodes, they just haven't been handled before in d3bfg)
	NAMEKEY2( OEM_102 ),
	NAMEKEY2( ABNT_C1 ),
	NAMEKEY2( NEXTTRACK ),
	NAMEKEY2( MUTE ),
	NAMEKEY2( CALCULATOR ),
	NAMEKEY2( PLAYPAUSE ),
	NAMEKEY2( MEDIASTOP ),
	NAMEKEY2( VOLUMEDOWN ),
	NAMEKEY2( VOLUMEUP ),
	NAMEKEY2( WEBHOME ),
	NAMEKEY2( WAKE ),
	NAMEKEY2( WEBSEARCH ),
	NAMEKEY2( WEBFAVORITES ),
	NAMEKEY2( WEBREFRESH ),
	NAMEKEY2( WEBSTOP ),
	NAMEKEY2( WEBFORWARD ),
	NAMEKEY2( WEBBACK ),
	NAMEKEY2( MYCOMPUTER ),
	NAMEKEY2( MAIL ),
	NAMEKEY2( MEDIASELECT ),
	// DG end
	
	// --
	
	NAMEKEY( MOUSE1, "#str_07054" ),
	NAMEKEY( MOUSE2, "#str_07055" ),
	NAMEKEY( MOUSE3, "#str_07056" ),
	NAMEKEY( MOUSE4, "#str_07057" ),
	NAMEKEY( MOUSE5, "#str_07058" ),
	NAMEKEY( MOUSE6, "#str_07059" ),
	NAMEKEY( MOUSE7, "#str_07060" ),
	NAMEKEY( MOUSE8, "#str_07061" ),
	
	// DG: some more mouse buttons
	NAMEKEY2( MOUSE9 ),
	NAMEKEY2( MOUSE10 ),
	NAMEKEY2( MOUSE11 ),
	NAMEKEY2( MOUSE12 ),
	NAMEKEY2( MOUSE13 ),
	NAMEKEY2( MOUSE14 ),
	NAMEKEY2( MOUSE15 ),
	NAMEKEY2( MOUSE16 ),
	// DG end
	
	NAMEKEY( MWHEELDOWN, "#str_07132" ),
	NAMEKEY( MWHEELUP, "#str_07131" ),
	
	NAMEKEY( JOY1, "#str_07062" ),
	NAMEKEY( JOY2, "#str_07063" ),
	NAMEKEY( JOY3, "#str_07064" ),
	NAMEKEY( JOY4, "#str_07065" ),
	NAMEKEY( JOY5, "#str_07066" ),
	NAMEKEY( JOY6, "#str_07067" ),
	NAMEKEY( JOY7, "#str_07068" ),
	NAMEKEY( JOY8, "#str_07069" ),
	NAMEKEY( JOY9, "#str_07070" ),
	NAMEKEY( JOY10, "#str_07071" ),
	NAMEKEY( JOY11, "#str_07072" ),
	NAMEKEY( JOY12, "#str_07073" ),
	NAMEKEY( JOY13, "#str_07074" ),
	NAMEKEY( JOY14, "#str_07075" ),
	NAMEKEY( JOY15, "#str_07076" ),
	NAMEKEY( JOY16, "#str_07077" ),
	
	NAMEKEY2( JOY_DPAD_UP ),
	NAMEKEY2( JOY_DPAD_DOWN ),
	NAMEKEY2( JOY_DPAD_LEFT ),
	NAMEKEY2( JOY_DPAD_RIGHT ),
	
	NAMEKEY2( JOY_STICK1_UP ),
	NAMEKEY2( JOY_STICK1_DOWN ),
	NAMEKEY2( JOY_STICK1_LEFT ),
	NAMEKEY2( JOY_STICK1_RIGHT ),
	
	NAMEKEY2( JOY_STICK2_UP ),
	NAMEKEY2( JOY_STICK2_DOWN ),
	NAMEKEY2( JOY_STICK2_LEFT ),
	NAMEKEY2( JOY_STICK2_RIGHT ),
	
	NAMEKEY2( JOY_TRIGGER1 ),
	NAMEKEY2( JOY_TRIGGER2 ),
	
	//------------------------
	// Aliases to make it easier to bind or to support old configs
	//------------------------
	ALIASKEY( "ALT", LALT ),
	ALIASKEY( "RIGHTALT", RALT ),
	ALIASKEY( "CTRL", LCTRL ),
	ALIASKEY( "SHIFT", LSHIFT ),
	ALIASKEY( "MENU", APPS ),
	ALIASKEY( "COMMAND", LALT ),
	
	ALIASKEY( "KP_HOME", KP_7 ),
	ALIASKEY( "KP_UPARROW", KP_8 ),
	ALIASKEY( "KP_PGUP", KP_9 ),
	ALIASKEY( "KP_LEFTARROW", KP_4 ),
	ALIASKEY( "KP_RIGHTARROW", KP_6 ),
	ALIASKEY( "KP_END", KP_1 ),
	ALIASKEY( "KP_DOWNARROW", KP_2 ),
	ALIASKEY( "KP_PGDN", KP_3 ),
	ALIASKEY( "KP_INS", KP_0 ),
	ALIASKEY( "KP_DEL", KP_DOT ),
	ALIASKEY( "KP_NUMLOCK", NUMLOCK ),
	
	ALIASKEY( "-", MINUS ),
	ALIASKEY( "=", EQUALS ),
	ALIASKEY( "[", LBRACKET ),
	ALIASKEY( "]", RBRACKET ),
	ALIASKEY( "\\", BACKSLASH ),
	ALIASKEY( "/", SLASH ),
	ALIASKEY( ",", COMMA ),
	ALIASKEY( ".", PERIOD ),
	
	{K_NONE, NULL, NULL}
};

class idKey
{
public:
	idKey()
	{
		down = false;
		repeats = 0;
		usercmdAction = 0;
	}
	bool			down;
	int				repeats;		// if > 1, it is autorepeating
	budStr			binding;
	int				usercmdAction;	// for testing by the asyncronous usercmd generation
};

bool		key_overstrikeMode = false;
idKey* 		keys = NULL;


/*
===================
idKeyInput::ArgCompletion_KeyName
===================
*/
void idKeyInput::ArgCompletion_KeyName( const budCmdArgs& args, void( *callback )( const char* s ) )
{
	for( keyname_t* kn = keynames; kn->name; kn++ )
	{
		callback( va( "%s %s", args.Argv( 0 ), kn->name ) );
	}
}

/*
===================
idKeyInput::GetOverstrikeMode
===================
*/
bool idKeyInput::GetOverstrikeMode()
{
	return key_overstrikeMode;
}

/*
===================
idKeyInput::SetOverstrikeMode
===================
*/
void idKeyInput::SetOverstrikeMode( bool state )
{
	key_overstrikeMode = state;
}

/*
===================
idKeyInput::IsDown
===================
*/
bool idKeyInput::IsDown( int keynum )
{
	if( keynum == -1 )
	{
		return false;
	}
	
	return keys[keynum].down;
}

/*
========================
idKeyInput::StringToKeyNum
========================
*/
keyNum_t idKeyInput::StringToKeyNum( const char* str )
{

	if( !str || !str[0] )
	{
		return K_NONE;
	}
	
	// scan for a text match
	for( keyname_t* kn = keynames; kn->name; kn++ )
	{
		if( !budStr::Icmp( str, kn->name ) )
		{
			return kn->keynum;
		}
	}
	
	return K_NONE;
}

/*
========================
idKeyInput::KeyNumToString
========================
*/
const char* idKeyInput::KeyNumToString( keyNum_t keynum )
{
	// check for a key string
	for( keyname_t* kn = keynames; kn->name; kn++ )
	{
		if( keynum == kn->keynum )
		{
			return kn->name;
		}
	}
	return "?";
}


/*
========================
idKeyInput::LocalizedKeyName
========================
*/
const char* idKeyInput::LocalizedKeyName( keyNum_t keynum )
{
	// RB
#if defined(_WIN32)
	// DG TODO: move this into a win32 Sys_GetKeyName()
	if( keynum < K_JOY1 )
	{
		// On the PC, we want to turn the scan code in to a key label that matches the currently selected keyboard layout
		unsigned char keystate[256] = { 0 };
		WCHAR temp[5];
		
		int scancode = ( int )keynum;
		int vkey = MapVirtualKey( keynum, MAPVK_VSC_TO_VK_EX );
		int result = -1;
		while( result < 0 )
		{
			result = ToUnicode( vkey, scancode, keystate, temp, sizeof( temp ) / sizeof( temp[0] ), 0 );
		}
		if( result > 0 && temp[0] > ' ' && iswprint( temp[0] ) )
		{
			static budStr bindStr;
			bindStr.Empty();
			bindStr.AppendUTF8Char( temp[0] );
			return bindStr;
		}
	}
#else // DG: for !Windows I introduced Sys_GetKeyName() to get key label for current keyboard layout
	
	const char* ret = nullptr;
	
	if( keynum < K_JOY1 ) // only for keyboard keys, not joystick or mouse
	{
		ret = Sys_GetKeyName( keynum );
	}
	
	if( ret != NULL )
	{
		return ret;
	}
#endif
	
	// check for a key string
	for( keyname_t* kn = keynames; kn->name; kn++ )
	{
		if( keynum == kn->keynum )
		{
			return budLocalization::GetString( kn->strId );
		}
	}
	return "????";
	// RB/DG end
}

/*
===================
idKeyInput::SetBinding
===================
*/
void idKeyInput::SetBinding( int keynum, const char* binding )
{
	if( keynum == -1 )
	{
		return;
	}
	
	// Clear out all button states so we aren't stuck forever thinking this key is held down
	usercmdGen->Clear();
	
	// allocate memory for new binding
	keys[keynum].binding = binding;
	
	// find the action for the async command generation
	keys[keynum].usercmdAction = usercmdGen->CommandStringUsercmdData( binding );
	
	// consider this like modifying an archived cvar, so the
	// file write will be triggered at the next oportunity
	cvarSystem->SetModifiedFlags( CVAR_ARCHIVE );
}


/*
===================
idKeyInput::GetBinding
===================
*/
const char* idKeyInput::GetBinding( int keynum )
{
	if( keynum == -1 )
	{
		return "";
	}
	
	return keys[ keynum ].binding;
}

/*
===================
idKeyInput::GetUsercmdAction
===================
*/
int idKeyInput::GetUsercmdAction( int keynum )
{
	return keys[ keynum ].usercmdAction;
}

/*
===================
Key_Unbind_f
===================
*/
void Key_Unbind_f( const budCmdArgs& args )
{
	int		b;
	
	if( args.Argc() != 2 )
	{
		common->Printf( "unbind <key> : remove commands from a key\n" );
		return;
	}
	
	b = idKeyInput::StringToKeyNum( args.Argv( 1 ) );
	if( b == -1 )
	{
		// If it wasn't a key, it could be a command
		if( !idKeyInput::UnbindBinding( args.Argv( 1 ) ) )
		{
			common->Printf( "\"%s\" isn't a valid key\n", args.Argv( 1 ) );
		}
	}
	else
	{
		idKeyInput::SetBinding( b, "" );
	}
}

/*
===================
Key_Unbindall_f
===================
*/
void Key_Unbindall_f( const budCmdArgs& args )
{
	for( int i = 0; i < K_LAST_KEY; i++ )
	{
		idKeyInput::SetBinding( i, "" );
	}
}

/*
===================
Key_Bind_f
===================
*/
void Key_Bind_f( const budCmdArgs& args )
{
	int			i, c, b;
	char		cmd[MAX_STRING_CHARS];
	
	c = args.Argc();
	
	if( c < 2 )
	{
		common->Printf( "bind <key> [command] : attach a command to a key\n" );
		return;
	}
	b = idKeyInput::StringToKeyNum( args.Argv( 1 ) );
	if( b == -1 )
	{
		common->Printf( "\"%s\" isn't a valid key\n", args.Argv( 1 ) );
		return;
	}
	
	if( c == 2 )
	{
		if( keys[b].binding.Length() )
		{
			common->Printf( "\"%s\" = \"%s\"\n", args.Argv( 1 ), keys[b].binding.c_str() );
		}
		else
		{
			common->Printf( "\"%s\" is not bound\n", args.Argv( 1 ) );
		}
		return;
	}
	
	// copy the rest of the command line
	cmd[0] = 0;		// start out with a null string
	for( i = 2; i < c; i++ )
	{
		strcat( cmd, args.Argv( i ) );
		if( i != ( c - 1 ) )
		{
			strcat( cmd, " " );
		}
	}
	
	idKeyInput::SetBinding( b, cmd );
}

/*
============
Key_BindUnBindTwo_f

binds keynum to bindcommand and unbinds if there are already two binds on the key
============
*/
void Key_BindUnBindTwo_f( const budCmdArgs& args )
{
	int c = args.Argc();
	if( c < 3 )
	{
		common->Printf( "bindunbindtwo <keynum> [command]\n" );
		return;
	}
	int key = atoi( args.Argv( 1 ) );
	budStr bind = args.Argv( 2 );
	if( idKeyInput::NumBinds( bind ) >= 2 && !idKeyInput::KeyIsBoundTo( key, bind ) )
	{
		idKeyInput::UnbindBinding( bind );
	}
	idKeyInput::SetBinding( key, bind );
}



/*
============
idKeyInput::WriteBindings

Writes lines containing "bind key value"
============
*/
void idKeyInput::WriteBindings( budFile* f )
{
	f->Printf( "unbindall\n" );
	
	for( int i = 0; i < K_LAST_KEY; i++ )
	{
		if( keys[i].binding.Length() )
		{
			const char* name = KeyNumToString( ( keyNum_t )i );
			f->Printf( "bind \"%s\" \"%s\"\n", name, keys[i].binding.c_str() );
		}
	}
}

/*
============
Key_ListBinds_f
============
*/
void Key_ListBinds_f( const budCmdArgs& args )
{
	for( int i = 0; i < K_LAST_KEY; i++ )
	{
		if( keys[i].binding.Length() )
		{
			common->Printf( "%s \"%s\"\n", idKeyInput::KeyNumToString( ( keyNum_t )i ), keys[i].binding.c_str() );
		}
	}
}

/*
============
idKeyInput::KeysFromBinding
returns the localized name of the key for the binding
============
*/
const char* idKeyInput::KeysFromBinding( const char* bind )
{
	static char keyName[MAX_STRING_CHARS];
	keyName[0] = 0;
	
	if( bind && *bind )
	{
		for( int i = 0; i < K_LAST_KEY; i++ )
		{
			if( keys[i].binding.Icmp( bind ) == 0 )
			{
				if( keyName[0] != '\0' )
				{
					budStr::Append( keyName, sizeof( keyName ), budLocalization::GetString( "#str_07183" ) );
				}
				budStr::Append( keyName, sizeof( keyName ), LocalizedKeyName( ( keyNum_t )i ) );
			}
		}
	}
	if( keyName[0] == '\0' )
	{
		budStr::Copynz( keyName, budLocalization::GetString( "#str_07133" ), sizeof( keyName ) );
	}
	budStr::ToLower( keyName );
	return keyName;
}

/*
========================
idKeyInput::KeyBindingsFromBinding

return: bindings for keyboard mouse and gamepad
========================
*/
keyBindings_t idKeyInput::KeyBindingsFromBinding( const char* bind, bool firstOnly, bool localized )
{
	budStr keyboard;
	budStr mouse;
	budStr gamepad;
	
	if( bind && *bind )
	{
		for( int i = 0; i < K_LAST_KEY; i++ )
		{
			if( keys[i].binding.Icmp( bind ) == 0 )
			{
				if( i >= K_JOY1 && i <= K_JOY_DPAD_RIGHT )
				{
					const char* gamepadKey = "";
					if( localized )
					{
						gamepadKey = LocalizedKeyName( ( keyNum_t )i );
					}
					else
					{
						gamepadKey = KeyNumToString( ( keyNum_t )i );
					}
					if( budStr::Icmp( gamepadKey, "" ) != 0 )
					{
						if( !gamepad.IsEmpty() )
						{
							if( firstOnly )
							{
								continue;
							}
							gamepad.Append( ", " );
						}
						gamepad.Append( gamepadKey );
					}
				}
				else if( i >= K_MOUSE1 && i <= K_MWHEELUP )
				{
					const char* mouseKey = "";
					if( localized )
					{
						mouseKey = LocalizedKeyName( ( keyNum_t )i );
					}
					else
					{
						mouseKey = KeyNumToString( ( keyNum_t )i );
					}
					if( budStr::Icmp( mouseKey, "" ) != 0 )
					{
						if( !mouse.IsEmpty() )
						{
							if( firstOnly )
							{
								continue;
							}
							mouse.Append( ", " );
						}
						mouse.Append( mouseKey );
					}
				}
				else
				{
					const char* tmp = "";
					if( localized )
					{
						tmp = LocalizedKeyName( ( keyNum_t )i );
					}
					else
					{
						tmp = KeyNumToString( ( keyNum_t )i );
					}
					if( budStr::Icmp( tmp, "" ) != 0 && budStr::Icmp( tmp, keyboard ) != 0 )
					{
						if( !keyboard.IsEmpty() )
						{
							if( firstOnly )
							{
								continue;
							}
							keyboard.Append( ", " );
						}
						keyboard.Append( tmp );
					}
				}
			}
		}
	}
	
	keyBindings_t bindings;
	bindings.gamepad = gamepad;
	bindings.mouse = mouse;
	bindings.keyboard = keyboard;
	
	return bindings;
}

/*
============
idKeyInput::BindingFromKey
returns the binding for the localized name of the key
============
*/
const char* idKeyInput::BindingFromKey( const char* key )
{
	const int keyNum = idKeyInput::StringToKeyNum( key );
	if( keyNum < 0 || keyNum >= K_LAST_KEY )
	{
		return NULL;
	}
	return keys[keyNum].binding.c_str();
}

/*
============
idKeyInput::UnbindBinding
============
*/
bool idKeyInput::UnbindBinding( const char* binding )
{
	bool unbound = false;
	if( binding && *binding )
	{
		for( int i = 0; i < K_LAST_KEY; i++ )
		{
			if( keys[i].binding.Icmp( binding ) == 0 )
			{
				SetBinding( i, "" );
				unbound = true;
			}
		}
	}
	return unbound;
}

/*
============
idKeyInput::NumBinds
============
*/
int idKeyInput::NumBinds( const char* binding )
{
	int count = 0;
	
	if( binding && *binding )
	{
		for( int i = 0; i < K_LAST_KEY; i++ )
		{
			if( keys[i].binding.Icmp( binding ) == 0 )
			{
				count++;
			}
		}
	}
	return count;
}

/*
============
idKeyInput::KeyIsBountTo
============
*/
bool idKeyInput::KeyIsBoundTo( int keynum, const char* binding )
{
	if( keynum >= 0 && keynum < K_LAST_KEY )
	{
		return ( keys[keynum].binding.Icmp( binding ) == 0 );
	}
	return false;
}

/*
===================
idKeyInput::PreliminaryKeyEvent

Tracks global key up/down state
Called by the system for both key up and key down events
===================
*/
void idKeyInput::PreliminaryKeyEvent( int keynum, bool down )
{
	keys[keynum].down = down;
}

/*
=================
idKeyInput::ExecKeyBinding
=================
*/
bool idKeyInput::ExecKeyBinding( int keynum )
{
	// commands that are used by the async thread
	// don't add text
	if( keys[keynum].usercmdAction )
	{
		return false;
	}
	
	// send the bound action
	if( keys[keynum].binding.Length() )
	{
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, keys[keynum].binding.c_str() );
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "\n" );
	}
	return true;
}

/*
===================
idKeyInput::ClearStates
===================
*/
void idKeyInput::ClearStates()
{
	for( int i = 0; i < K_LAST_KEY; i++ )
	{
		if( keys[i].down )
		{
			PreliminaryKeyEvent( i, false );
		}
		keys[i].down = false;
	}
	
	// clear the usercommand states
	usercmdGen->Clear();
}

/*
===================
idKeyInput::Init
===================
*/
void idKeyInput::Init()
{

	keys = new( TAG_SYSTEM ) idKey[K_LAST_KEY];
	
	// register our functions
	cmdSystem->AddCommand( "bind", Key_Bind_f, CMD_FL_SYSTEM, "binds a command to a key", idKeyInput::ArgCompletion_KeyName );
	cmdSystem->AddCommand( "bindunbindtwo", Key_BindUnBindTwo_f, CMD_FL_SYSTEM, "binds a key but unbinds it first if there are more than two binds" );
	cmdSystem->AddCommand( "unbind", Key_Unbind_f, CMD_FL_SYSTEM, "unbinds any command from a key", idKeyInput::ArgCompletion_KeyName );
	cmdSystem->AddCommand( "unbindall", Key_Unbindall_f, CMD_FL_SYSTEM, "unbinds any commands from all keys" );
	cmdSystem->AddCommand( "listBinds", Key_ListBinds_f, CMD_FL_SYSTEM, "lists key bindings" );
}

/*
===================
idKeyInput::Shutdown
===================
*/
void idKeyInput::Shutdown()
{
	delete [] keys;
	keys = NULL;
}


/*
========================
Key_CovertHIDCode
Converts from a USB HID code to a K_ code
========================
*/
int Key_CovertHIDCode( int hid )
{
	if( hid >= 0 && hid <= 106 )
	{
		int table[] =
		{
			K_NONE, K_NONE, K_NONE, K_NONE,
			K_A, K_B, K_C, K_D, K_E, K_F, K_G, K_H, K_I, K_J, K_K, K_L, K_M, K_N, K_O, K_P, K_Q, K_R, K_S, K_T, K_U, K_V, K_W, K_X, K_Y, K_Z,
			K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_0,
			K_ENTER, K_ESCAPE, K_BACKSPACE, K_TAB, K_SPACE,
			K_MINUS, K_EQUALS, K_LBRACKET, K_RBRACKET, K_BACKSLASH, K_NONE, K_SEMICOLON, K_APOSTROPHE, K_GRAVE, K_COMMA, K_PERIOD, K_SLASH, K_CAPSLOCK,
			K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12,
			K_PRINTSCREEN, K_SCROLL, K_PAUSE, K_INS, K_HOME, K_PGUP, K_DEL, K_END, K_PGDN, K_RIGHTARROW, K_LEFTARROW, K_DOWNARROW, K_UPARROW,
			K_NUMLOCK, K_KP_SLASH, K_KP_STAR, K_KP_MINUS, K_KP_PLUS, K_KP_ENTER,
			K_KP_1, K_KP_2, K_KP_3, K_KP_4, K_KP_5, K_KP_6, K_KP_7, K_KP_8, K_KP_9, K_KP_0, K_KP_DOT,
			K_NONE, K_APPS, K_POWER, K_KP_EQUALS,
			K_F13, K_F14, K_F15
		};
		return table[hid];
	}
	if( hid >= 224 && hid <= 231 )
	{
		int table[] =
		{
			K_LCTRL, K_LSHIFT, K_LALT, K_LWIN,
			K_RCTRL, K_RSHIFT, K_RALT, K_RWIN
		};
		return table[hid - 224];
	}
	return K_NONE;
}