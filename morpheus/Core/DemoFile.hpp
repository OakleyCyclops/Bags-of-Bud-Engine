/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2014-2016 Robert Beckebans
Copyright (C) 2014-2016 Kot in Action Creative Artel

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

#ifndef __DEMOFILE_H__
#define __DEMOFILE_H__

/*
===============================================================================

	Demo file

===============================================================================
*/

typedef enum
{
	DS_FINISHED,
	DS_RENDER,
	DS_SOUND,
	DS_GAME,
	DS_VERSION
} demoSystem_t;

class budDemoFile : public budFile
{
public:
	budDemoFile();
	~budDemoFile();
	
	const char* 	GetName()
	{
		return ( f ? f->GetName() : "" );
	}
	const char* 	GetFullPath()
	{
		return ( f ? f->GetFullPath() : "" );
	}
	
	void			SetLog( bool b, const char* p );
	void			Log( const char* p );
	bool			OpenForReading( const char* fileName );
	bool			OpenForWriting( const char* fileName );
	void			Close();
	
	const char* 	ReadHashString();
	void			WriteHashString( const char* str );
	
	void			ReadDict( idDict& dict );
	void			WriteDict( const idDict& dict );
	
	int				Read( void* buffer, int len );
	int				Write( const void* buffer, int len );
	
private:
	static idCompressor* AllocCompressor( int type );
	
	bool			writing;
	byte* 			fileImage;
	budFile* 		f;
	idCompressor* 	compressor;
	
	budList<budStr*>	demoStrings;
	budFile* 		fLog;
	bool			log;
	budStr			logStr;
	
	static budCVar	com_logDemos;
	static budCVar	com_compressDemos;
	static budCVar	com_preloadDemos;
};

#endif /* !__DEMOFILE_H__ */
