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

#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

/*
===============================================================================

	File System

	No stdio calls should be used by any part of the game, because of all sorts
	of directory and separator char issues. Throughout the game a forward slash
	should be used as a separator. The file system takes care of the conversion
	to an OS specific separator. The file system treats all file and directory
	names as case insensitive.

	The following cvars store paths used by the file system:

	"fs_basepath"		path to local install
	"fs_savepath"		path to config, save game, etc. files, read & write

	The base path for file saving can be set to "fs_savepath" or "fs_basepath".

===============================================================================
*/

void			Sys_Mkdir( const char* path );
bool			Sys_Rmdir( const char* path );
bool			Sys_IsFileWritable( const char* path );

enum sysFolder_t
{
	FOLDER_ERROR	= -1,
	FOLDER_NO		= 0,
	FOLDER_YES		= 1
};

// returns FOLDER_YES if the specified path is a folder
sysFolder_t		Sys_IsFolder( const char* path );

// use fs_debug to verbose Sys_ListFiles
// returns -1 if directory was not found (the list is cleared)
int				Sys_ListFiles( const char* directory, const char* extension, budList<class budStr>& list );

const char* 	Sys_EXEPath();
const char* 	Sys_CWD();

const char* 	Sys_LaunchPath();


static const ID_TIME_T	FILE_NOT_FOUND_TIMESTAMP	= ( ID_TIME_T ) - 1;
static const int		MAX_OSPATH					= 256;

// modes for OpenFileByMode
typedef enum
{
	FS_READ			= 0,
	FS_WRITE		= 1,
	FS_APPEND	= 2
} fsMode_t;

typedef enum
{
	FIND_NO,
	FIND_YES
} findFile_t;

// file list for directory listings
class budFileList
{
	friend class budFileSystemLocal;
public:
	const char* 			GetBasePath() const
	{
		return basePath;
	}
	int						GetNumFiles() const
	{
		return list.Num();
	}
	const char* 			GetFile( int index ) const
	{
		return list[index];
	}
	const budStrList& 		GetList() const
	{
		return list;
	}
	
private:
	budStr					basePath;
	budStrList				list;
};

#endif /* !__FILESYSTEM_H__ */
