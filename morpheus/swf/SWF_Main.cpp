/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2013-2015 Robert Beckebans

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
#include "../renderer/DXT//DXTCodec.h"

#pragma warning(disable: 4355) // 'this' : used in base member initializer list

CVar swf_loadBinary( "swf_loadBinary", "1", CVAR_INTEGER, "used to set whether to load binary swf from generated" );
// RB begin
CVar postLoadExportFlashAtlas( "postLoadExportFlashAtlas", "0", CVAR_INTEGER, "" );
CVar postLoadExportFlashToSWF( "postLoadExportFlashToSWF", "0", CVAR_INTEGER, "" );
CVar postLoadExportFlashToJSON( "postLoadExportFlashToJSON", "0", CVAR_INTEGER, "" );
// RB end

int budSWF::mouseX = -1;
int budSWF::mouseY = -1;
bool budSWF::isMouseInClientArea = false;

extern CVar in_useJoystick;



/*
===================
budSWF::budSWF
===================
*/
budSWF::budSWF( const char* filename_, budSoundWorld* soundWorld_ )
{

	atlasMaterial = NULL;
	
	swfScale = 1.0f;
	scaleToVirtual.Set( 1.0f, 1.0f );
	
	random.SetSeed( Sys_Milliseconds() );
	
	guiSolid = declManager->FindMaterial( "guiSolid" );
	guiCursor_arrow = declManager->FindMaterial( "ui/assets/guicursor_arrow" );
	guiCursor_hand = declManager->FindMaterial( "ui/assets/guicursor_hand" );
	white = declManager->FindMaterial( "_white" );
	
	// RB:
	debugFont = renderSystem->RegisterFont( "Arial Narrow" );
	
	tooltipButtonImage.Append( keyButtonImages_t( "<JOY1>", "guis/assets/hud/controller/xb360/a", "guis/assets/hud/controller/ps3/cross", 37, 37, 0 ) );
	tooltipButtonImage.Append( keyButtonImages_t( "<JOY2>", "guis/assets/hud/controller/xb360/b", "guis/assets/hud/controller/ps3/circle", 37, 37, 0 ) );
	tooltipButtonImage.Append( keyButtonImages_t( "<JOY3>", "guis/assets/hud/controller/xb360/x", "guis/assets/hud/controller/ps3/square", 37, 37, 0 ) );
	tooltipButtonImage.Append( keyButtonImages_t( "<JOY4>", "guis/assets/hud/controller/xb360/y", "guis/assets/hud/controller/ps3/triangle", 37, 37, 0 ) );
	tooltipButtonImage.Append( keyButtonImages_t( "<JOY_TRIGGER2>", "guis/assets/hud/controller/xb360/rt", "guis/assets/hud/controller/ps3/r2", 64, 52, 0 ) );
	tooltipButtonImage.Append( keyButtonImages_t( "<JOY_TRIGGER1>", "guis/assets/hud/controller/xb360/lt", "guis/assets/hud/controller/ps3/l2", 64, 52, 0 ) );
	tooltipButtonImage.Append( keyButtonImages_t( "<JOY5>", "guis/assets/hud/controller/xb360/lb", "guis/assets/hud/controller/ps3/l1", 52, 32, 0 ) );
	tooltipButtonImage.Append( keyButtonImages_t( "<JOY6>", "guis/assets/hud/controller/xb360/rb", "guis/assets/hud/controller/ps3/r1", 52, 32, 0 ) );
	tooltipButtonImage.Append( keyButtonImages_t( "<MOUSE1>", "guis/assets/hud/controller/mouse1", "", 64, 52, 0 ) );
	tooltipButtonImage.Append( keyButtonImages_t( "<MOUSE2>", "guis/assets/hud/controller/mouse2", "", 64, 52, 0 ) );
	tooltipButtonImage.Append( keyButtonImages_t( "<MOUSE3>", "guis/assets/hud/controller/mouse3", "", 64, 52, 0 ) );
	
	for( int index = 0; index < tooltipButtonImage.Num(); index++ )
	{
		if( ( tooltipButtonImage[index].xbImage != NULL ) && ( tooltipButtonImage[index].xbImage[0] != '\0' ) )
		{
			declManager->FindMaterial( tooltipButtonImage[index].xbImage );
		}
		if( ( tooltipButtonImage[index].psImage != NULL ) && ( tooltipButtonImage[index].psImage[0] != '\0' ) )
		{
			declManager->FindMaterial( tooltipButtonImage[index].psImage );
		}
	}
	
	frameWidth = 0;
	frameHeight = 0;
	frameRate = 0;
	lastRenderTime = 0;
	
	isActive = false;
	inhibitControl = false;
	useInhibtControl = true;
	
	crop = false;
	blackbars = false;
	paused = false;
	hasHitObject = false;
	
	useMouse = true;
	mouseEnabled = false;
	renderBorder = 0;
	mouseObject = NULL;
	hoverObject = NULL;
	soundWorld = NULL;
	forceNonPCPlatform = false;
	
	if( String::Cmpn( filename_, "swf/", 4 ) != 0 )
	{
		// if it doesn't already have swf/ in front of it, add it
		filename = "swf/";
		filename += filename_;
	}
	else
	{
		filename = filename_;
	}
	filename.ToLower();
	filename.BackSlashesToSlashes();
	filename.SetFileExtension( ".swf" );
	
	timestamp = fileSystem->GetTimestamp( filename );
	
	mainsprite = new( TAG_SWF ) budSWFSprite( this );
	mainspriteInstance = NULL;
	
	String binaryFileName = "generated/";
	binaryFileName += filename;
	binaryFileName.SetFileExtension( ".bswf" );
	
	// RB: add JSON alternative
	String jsonFileName = filename;
	jsonFileName.SetFileExtension( ".json" );
	ID_TIME_T jsonSourceTime = fileSystem->GetTimestamp( jsonFileName );
	
	bool loadedFromJSON = false;
	if( swf_loadBinary.GetBool() )
	{
		if( timestamp == FILE_NOT_FOUND_TIMESTAMP )
		{
			timestamp = jsonSourceTime;
		}
		
		if( !LoadBinary( binaryFileName, timestamp ) )
		{
			if( LoadJSON( jsonFileName ) )
			{
				loadedFromJSON = true;
				
				WriteBinary( binaryFileName );
			}
			else if( LoadSWF( filename ) )
			{
				WriteBinary( binaryFileName );
			}
		}
	}
	else
	{
		if( LoadJSON( jsonFileName ) )
		{
			loadedFromJSON = true;
		}
		else
		{
			LoadSWF( filename );
		}
	}
	
	if( postLoadExportFlashToSWF.GetBool() )
	{
		String jsonFileName = "exported/";
		jsonFileName += filename;
		jsonFileName.SetFileExtension( ".json" );
		
		WriteJSON( jsonFileName );
	}
	
	String atlasFileName = binaryFileName;
	atlasFileName.SetFileExtension( ".tga" );
	atlasMaterial = declManager->FindMaterial( atlasFileName );
	
	byte* atlasExportImageRGBA = NULL;
	int atlasExportImageWidth = 0;
	int atlasExportImageHeight = 0;
	
	if( /*!loadedFromJSON &&*/ ( postLoadExportFlashToJSON.GetBool() || postLoadExportFlashAtlas.GetBool() || postLoadExportFlashToSWF.GetBool() ) )
	{
		StringStatic< MAX_OSPATH > generatedName = atlasFileName;
		generatedName.StripFileExtension();
		budImage::GetGeneratedName( generatedName, TD_DEFAULT, CF_2D );
		
		budBinaryImage im( generatedName );
		ID_TIME_T binaryFileTime = im.LoadFromGeneratedFile( FILE_NOT_FOUND_TIMESTAMP );
		
		if( binaryFileTime != FILE_NOT_FOUND_TIMESTAMP )
		{
			const bimageFile_t& imgHeader = im.GetFileHeader();
			const bimageImage_t& img = im.GetImageHeader( 0 );
			
			const byte* data = im.GetImageData( 0 );
			
			//( img.level, 0, 0, img.destZ, img.width, img.height, data );
			
			TempArray<byte> rgba( img.width * img.height * 4 );
			memset( rgba.Ptr(), 255, rgba.Size() );
			
			if( imgHeader.format == FMT_DXT1 )
			{
				idDxtDecoder dxt;
				dxt.DecompressImageDXT1( data, rgba.Ptr(), img.width, img.height );
			}
			else if( imgHeader.format == FMT_DXT5 )
			{
				idDxtDecoder dxt;
				
				if( imgHeader.colorFormat == CFM_NORMAL_DXT5 )
				{
					dxt.DecompressNormalMapDXT5( data, rgba.Ptr(), img.width, img.height );
				}
				else if( imgHeader.colorFormat == CFM_YCOCG_DXT5 )
				{
					dxt.DecompressYCoCgDXT5( data, rgba.Ptr(), img.width, img.height );
				}
				else
				{
				
					dxt.DecompressImageDXT5( data, rgba.Ptr(), img.width, img.height );
				}
			}
			else if( imgHeader.format == FMT_LUM8 || imgHeader.format == FMT_INT8 )
			{
				// LUM8 and INT8 just read the red channel
				byte* pic = rgba.Ptr();
				for( int i = 0; i < img.dataSize; i++ )
				{
					pic[ i * 4 ] = data[ i ];
				}
			}
			else if( imgHeader.format == FMT_ALPHA )
			{
				// ALPHA reads the alpha channel
				byte* pic = rgba.Ptr();
				for( int i = 0; i < img.dataSize; i++ )
				{
					pic[ i * 4 + 3 ] = data[ i ];
				}
			}
			else if( imgHeader.format == FMT_L8A8 )
			{
				// L8A8 reads the alpha and red channels
				byte* pic = rgba.Ptr();
				for( int i = 0; i < img.dataSize / 2; i++ )
				{
					pic[ i * 4 + 0 ] = data[ i * 2 + 0 ];
					pic[ i * 4 + 3 ] = data[ i * 2 + 1 ];
				}
			}
			else if( imgHeader.format == FMT_RGB565 )
			{
				// FIXME
				/*
				byte* pic = rgba.Ptr();
				for( int i = 0; i < img.dataSize / 2; i++ )
				{
					unsigned short color = ( ( pic[ i * 4 + 0 ] >> 3 ) << 11 ) | ( ( pic[ i * 4 + 1 ] >> 2 ) << 5 ) | ( pic[ i * 4 + 2 ] >> 3 );
					img.data[ i * 2 + 0 ] = ( color >> 8 ) & 0xFF;
					img.data[ i * 2 + 1 ] = color & 0xFF;
				}
				*/
			}
			else
			{
				byte* pic = rgba.Ptr();
				for( int i = 0; i < img.dataSize; i++ )
				{
					pic[ i ] = data[ i ];
				}
			}
			
			String atlasFileNameExport = atlasFileName;
			atlasFileNameExport.Replace( "generated/", "exported/" );
			atlasFileNameExport.SetFileExtension( ".png" );
			
			R_WritePNG( atlasFileNameExport, rgba.Ptr(), 4, img.width, img.height, true, "fs_basepath" );
			
			if( postLoadExportFlashToSWF.GetBool() )
			{
				atlasExportImageWidth = img.width;
				atlasExportImageHeight = img.height;
				atlasExportImageRGBA = ( byte* ) Mem_Alloc( rgba.Size(), TAG_TEMP );
				memcpy( atlasExportImageRGBA, rgba.Ptr(), rgba.Size() );
			}
		}
	}
	
	if( postLoadExportFlashToSWF.GetBool() )
	{
		String swfFileName = "exported/";
		swfFileName += filename;
		swfFileName.SetFileExtension( ".swf" );
		
		WriteSWF( swfFileName, atlasExportImageRGBA, atlasExportImageWidth, atlasExportImageHeight );
	}
	
	if( atlasExportImageRGBA != NULL )
	{
		Mem_Free( atlasExportImageRGBA );
		atlasExportImageRGBA = NULL;
	}
	// RB end
	
	globals = budSWFScriptObject::Alloc();
	globals->Set( "_global", globals );
	
	globals->Set( "Object", &scriptFunction_Object );
	
	mainspriteInstance = spriteInstanceAllocator.Alloc();
	mainspriteInstance->Init( mainsprite, NULL, 0 );
	
	shortcutKeys = budSWFScriptObject::Alloc();
	scriptFunction_shortcutKeys_clear.Bind( this );
	scriptFunction_shortcutKeys_clear.Call( shortcutKeys, budSWFParmList() );
	globals->Set( "shortcutKeys", shortcutKeys );
	
	globals->Set( "deactivate", scriptFunction_deactivate.Bind( this ) );
	globals->Set( "inhibitControl", scriptFunction_inhibitControl.Bind( this ) );
	globals->Set( "useInhibit", scriptFunction_useInhibit.Bind( this ) );
	globals->Set( "precacheSound", scriptFunction_precacheSound.Bind( this ) );
	globals->Set( "playSound", scriptFunction_playSound.Bind( this ) );
	globals->Set( "stopSounds", scriptFunction_stopSounds.Bind( this ) );
	globals->Set( "getPlatform", scriptFunction_getPlatform.Bind( this ) );
	globals->Set( "getTruePlatform", scriptFunction_getTruePlatform.Bind( this ) );
	globals->Set( "getLocalString", scriptFunction_getLocalString.Bind( this ) );
	globals->Set( "swapPS3Buttons", scriptFunction_swapPS3Buttons.Bind( this ) );
	globals->Set( "_root", mainspriteInstance->scriptObject );
	globals->Set( "strReplace", scriptFunction_strReplace.Bind( this ) );
	globals->Set( "getCVarInteger", scriptFunction_getCVarInteger.Bind( this ) );
	globals->Set( "setCVarInteger", scriptFunction_setCVarInteger.Bind( this ) );
	
	globals->Set( "acos", scriptFunction_acos.Bind( this ) );
	globals->Set( "cos", scriptFunction_cos.Bind( this ) );
	globals->Set( "sin", scriptFunction_sin.Bind( this ) );
	globals->Set( "round", scriptFunction_round.Bind( this ) );
	globals->Set( "pow", scriptFunction_pow.Bind( this ) );
	globals->Set( "sqrt", scriptFunction_sqrt.Bind( this ) );
	globals->Set( "abs", scriptFunction_abs.Bind( this ) );
	globals->Set( "rand", scriptFunction_rand.Bind( this ) );
	globals->Set( "floor", scriptFunction_floor.Bind( this ) );
	globals->Set( "ceil", scriptFunction_ceil.Bind( this ) );
	globals->Set( "toUpper", scriptFunction_toUpper.Bind( this ) );
	
	globals->SetNative( "platform", swfScriptVar_platform.Bind( &scriptFunction_getPlatform ) );
	globals->SetNative( "blackbars", swfScriptVar_blackbars.Bind( this ) );
	globals->SetNative( "cropToHeight", swfScriptVar_crop.Bind( this ) );
	globals->SetNative( "cropToFit", swfScriptVar_crop.Bind( this ) );
	globals->SetNative( "crop", swfScriptVar_crop.Bind( this ) );
	
	// Do this to touch any external references (like sounds)
	// But disable script warnings because many globals won't have been created yet
	extern CVar swf_debug;
	int debug = swf_debug.GetInteger();
	swf_debug.SetInteger( 0 );
	
	mainspriteInstance->Run();
	mainspriteInstance->RunActions();
	mainspriteInstance->RunTo( 0 );
	
	swf_debug.SetInteger( debug );
	
	if( mouseX == -1 )
	{
		mouseX = ( frameWidth / 2 );
	}
	
	if( mouseY == -1 )
	{
		mouseY = ( frameHeight / 2 );
	}
	
	soundWorld = soundWorld_;
}

/*
===================
budSWF::~budSWF
===================
*/
budSWF::~budSWF()
{
	spriteInstanceAllocator.Free( mainspriteInstance );
	delete mainsprite;
	
	for( int i = 0 ; i < dictionary.Num() ; i++ )
	{
		if( dictionary[i].sprite )
		{
			delete dictionary[i].sprite;
			dictionary[i].sprite = NULL;
		}
		if( dictionary[i].shape )
		{
			delete dictionary[i].shape;
			dictionary[i].shape = NULL;
		}
		if( dictionary[i].font )
		{
			delete dictionary[i].font;
			dictionary[i].font = NULL;
		}
		if( dictionary[i].text )
		{
			delete dictionary[i].text;
			dictionary[i].text = NULL;
		}
		if( dictionary[i].edittext )
		{
			delete dictionary[i].edittext;
			dictionary[i].edittext = NULL;
		}
	}
	
	globals->Clear();
	tooltipButtonImage.Clear();
	globals->Release();
	
	shortcutKeys->Clear();
	shortcutKeys->Release();
}

/*
===================
budSWF::Activate
when a SWF is deactivated, it rewinds the timeline back to the start
===================
*/
void budSWF::Activate( bool b )
{
	if( !isActive && b )
	{
		inhibitControl = false;
		lastRenderTime = Sys_Milliseconds();
		
		mainspriteInstance->FreeDisplayList();
		mainspriteInstance->Play();
		mainspriteInstance->Run();
		mainspriteInstance->RunActions();
	}
	isActive = b;
}

/*
===================
budSWF::InhibitControl
===================
*/
bool budSWF::InhibitControl()
{
	if( !IsLoaded() || !IsActive() )
	{
		return false;
	}
	return ( inhibitControl && useInhibtControl );
}

/*
===================
budSWF::PlaySound
===================
*/
int budSWF::PlaySound( const char* sound, int channel, bool blocking )
{
	if( !IsActive() )
	{
		return -1;
	}
	if( soundWorld != NULL )
	{
		return soundWorld->PlayShaderDirectly( sound, channel );
	}
	else
	{
		libBud::Warning( "No playing sound world on soundSystem in swf play sound!" );
		return -1;
	}
}

/*
===================
budSWF::PlaySound
===================
*/
void budSWF::StopSound( int channel )
{
	if( soundWorld != NULL )
	{
		soundWorld->PlayShaderDirectly( NULL, channel );
	}
	else
	{
		libBud::Warning( "No playing sound world on soundSystem in swf play sound!" );
	}
}

/*
===================
budSWF::budSWFScriptFunction_inhibitControl::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_inhibitControl::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	pThis->inhibitControl = parms[0].ToBool();
	return budSWFScriptVar();
}

/*
===================
budSWF::budSWFScriptFunction_inhibitControl::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_useInhibit::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	pThis->useInhibtControl = parms[0].ToBool();
	return budSWFScriptVar();
}

/*
===================
budSWF::budSWFScriptFunction_deactivate::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_deactivate::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	pThis->Activate( false );
	return budSWFScriptVar();
}

/*
===================
budSWF::budSWFScriptFunction_precacheSound::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_precacheSound::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	const idSoundShader* soundShader = declManager->FindSound( parms[0].ToString(), true );
	return soundShader->GetName();
}

/*
===================
budSWF::budSWFScriptFunction_playSound::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_playSound::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	int channel = SCHANNEL_ANY;
	// specific channel passed in
	if( parms.Num() > 1 )
	{
		channel = parms[1].ToInteger();
	}
	
	pThis->PlaySound( parms[0].ToString(), channel );
	
	return budSWFScriptVar();
}

/*
===================
budSWF::budSWFScriptFunction_stopSounds::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_stopSounds::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{

	int channel = SCHANNEL_ANY;
	if( parms.Num() == 1 )
	{
		channel = parms[0].ToInteger();
	}
	
	pThis->StopSound( channel );
	
	return budSWFScriptVar();
}

/*
========================
budSWFScriptFunction_GetPlatform::Call
========================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_getPlatform::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	return pThis->GetPlatform();
}

/*
========================
budSWFScriptFunction_GetPlatform::Call
========================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_getTruePlatform::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{

	return 2;
}


/*
========================
budSWFScriptFunction_GetPlatform::Call
========================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_strReplace::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{

	if( parms.Num() != 3 )
	{
		return "";
	}
	
	String str = parms[0].ToString();
	String repString = parms[1].ToString();
	String val = parms[2].ToString();
	str.Replace( repString, val );
	
	return str;
}

/*
========================
budSWFScriptFunction_GetPlatform::Call
========================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_getLocalString::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{

	if( parms.Num() == 0 )
	{
		return budSWFScriptVar();
	}
	
	String val = budLocalization::GetString( parms[0].ToString() );
	return val;
}

/*
========================
budSWF::UseCircleForAccept
========================
*/
bool budSWF::UseCircleForAccept()
{
	return false;
}

/*
========================
budSWF::GetPlatform
========================
*/
int	budSWF::GetPlatform()
{


	if( in_useJoystick.GetBool() || forceNonPCPlatform )
	{
		forceNonPCPlatform = false;
		return 0;
	}
	
	return 2;
}

/*
========================
budSWFScriptFunction_swapPS3Buttons::Call
========================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_swapPS3Buttons::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	return pThis->UseCircleForAccept();
}

/*
========================
budSWFScriptFunction_getCVarInteger::Call
========================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_getCVarInteger::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	return cvarSystem->GetCVarInteger( parms[0].ToString() );
}

/*
========================
budSWFScriptFunction_setCVarInteger::Call
========================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_setCVarInteger::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	cvarSystem->SetCVarInteger( parms[0].ToString(), parms[1].ToInteger() );
	return budSWFScriptVar();
}

/*
===================
budSWF::budSWFScriptFunction_acos::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_acos::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	if( parms.Num() != 1 )
	{
		return budSWFScriptVar();
	}
	return Math::ACos( parms[0].ToFloat() );
}

/*
===================
budSWF::budSWFScriptFunction_cos::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_cos::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	if( parms.Num() != 1 )
	{
		return budSWFScriptVar();
	}
	return Math::Cos( parms[0].ToFloat() );
}

/*
===================
budSWF::budSWFScriptFunction_sin::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_sin::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	if( parms.Num() != 1 )
	{
		return budSWFScriptVar();
	}
	return ( Math::Sin( parms[0].ToFloat() ) );
}

/*
===================
budSWF::budSWFScriptFunction_round::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_round::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	if( parms.Num() != 1 )
	{
		return budSWFScriptVar();
	}
	int value = Math::Ftoi( parms[0].ToFloat() + 0.5f );
	return value;
}

/*
===================
budSWF::budSWFScriptFunction_pow::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_pow::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	if( parms.Num() != 2 )
	{
		return budSWFScriptVar();
	}
	
	float value = parms[0].ToFloat();
	float power = parms[1].ToFloat();
	return ( Math::Pow( value, power ) );
}

/*
===================
budSWF::budSWFScriptFunction_pow::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_sqrt::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	if( parms.Num() != 1 )
	{
		return budSWFScriptVar();
	}
	
	float value = parms[0].ToFloat();
	return ( Math::Sqrt( value ) );
}

/*
===================
budSWF::budSWFScriptFunction_abs::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_abs::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	if( parms.Num() != 1 )
	{
		return budSWFScriptVar();
	}
	
	float value = Math::Fabs( parms[0].ToFloat() );
	return value;
}

/*
===================
budSWF::budSWFScriptFunction_rand::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_rand::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	float min = 0.0f;
	float max = 1.0f;
	switch( parms.Num() )
	{
		case 0:
			break;
		case 1:
			max = parms[0].ToFloat();
			break;
		default:
			min = parms[0].ToFloat();
			max = parms[1].ToFloat();
			break;
	}
	return min + pThis->GetRandom().RandomFloat() * ( max - min );
}

/*
========================
budSWFScriptFunction_floor::Call
========================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_floor::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	if( parms.Num() != 1 || !parms[0].IsNumeric() )
	{
		libBud::Warning( "Invalid parameters specified for floor" );
		return budSWFScriptVar();
	}
	
	float num = parms[0].ToFloat();
	
	return budSWFScriptVar( Math::Floor( num ) );
}

/*
========================
budSWFScriptFunction_ceil::Call
========================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_ceil::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	if( parms.Num() != 1 || !parms[0].IsNumeric() )
	{
		libBud::Warning( "Invalid parameters specified for ceil" );
		return budSWFScriptVar();
	}
	
	float num = parms[0].ToFloat();
	
	return budSWFScriptVar( Math::Ceil( num ) );
}

/*
========================
budSWFScriptFunction_toUpper::Call
========================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_toUpper::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	if( parms.Num() != 1 || !parms[0].IsString() )
	{
		libBud::Warning( "Invalid parameters specified for toUpper" );
		return budSWFScriptVar();
	}
	
	String val = budLocalization::GetString( parms[0].ToString() );
	val.ToUpper();
	return val;
}

/*
===================
budSWF::budSWFScriptFunction_shortcutKeys_clear::Call
===================
*/
budSWFScriptVar budSWF::budSWFScriptFunction_shortcutKeys_clear::Call( budSWFScriptObject* thisObject, const budSWFParmList& parms )
{
	budSWFScriptObject* object = pThis->shortcutKeys;
	object->Clear();
	object->Set( "clear", this );
	object->Set( "JOY1", "ENTER" );
	object->Set( "JOY2", "BACKSPACE" );
	object->Set( "JOY3", "START" );
	object->Set( "JOY5", "LB" );
	object->Set( "JOY6", "RB" );
	object->Set( "JOY9", "START" );
	object->Set( "JOY10", "BACKSPACE" );
	object->Set( "JOY_DPAD_UP", "UP" );
	object->Set( "JOY_DPAD_DOWN", "DOWN" );
	object->Set( "JOY_DPAD_LEFT", "LEFT" );
	object->Set( "JOY_DPAD_RIGHT", "RIGHT" );
	object->Set( "JOY_STICK1_UP", "STICK1_UP" );
	object->Set( "JOY_STICK1_DOWN", "STICK1_DOWN" );
	object->Set( "JOY_STICK1_LEFT", "STICK1_LEFT" );
	object->Set( "JOY_STICK1_RIGHT", "STICK1_RIGHT" );
	object->Set( "JOY_STICK2_UP", "STICK2_UP" );
	object->Set( "JOY_STICK2_DOWN", "STICK2_DOWN" );
	object->Set( "JOY_STICK2_LEFT", "STICK2_LEFT" );
	object->Set( "JOY_STICK2_RIGHT", "STICK2_RIGHT" );
	object->Set( "KP_ENTER", "ENTER" );
	object->Set( "MWHEELDOWN", "MWHEEL_DOWN" );
	object->Set( "MWHEELUP", "MWHEEL_UP" );
	object->Set( "K_TAB", "TAB" );
	
	
	// FIXME: I'm an RTARD and didn't realize the keys all have "ARROW" after them
	object->Set( "LEFTARROW", "LEFT" );
	object->Set( "RIGHTARROW", "RIGHT" );
	object->Set( "UPARROW", "UP" );
	object->Set( "DOWNARROW", "DOWN" );
	
	
	return budSWFScriptVar();
}

budSWFScriptVar budSWF::budSWFScriptNativeVar_blackbars::Get( budSWFScriptObject* object )
{
	return pThis->blackbars;
}

void budSWF::budSWFScriptNativeVar_blackbars::Set( budSWFScriptObject* object, const budSWFScriptVar& value )
{
	pThis->blackbars = value.ToBool();
}

budSWFScriptVar budSWF::budSWFScriptNativeVar_crop::Get( budSWFScriptObject* object )
{
	return pThis->crop;
}

void budSWF::budSWFScriptNativeVar_crop::Set( budSWFScriptObject* object, const budSWFScriptVar& value )
{
	pThis->crop = value.ToBool();
}
