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

#include "corePCH.hpp"
#pragma hdrstop

/*
================================================================================================
Contains external code for building ZipFiles.
================================================================================================
*/

#include "coreZip.hpp"


// #undef STDC

budCVar zip_verbosity( "zip_verbosity", "0", CVAR_BOOL, "1 = verbose logging when building zip files" );
#define	DEFAULT_COMPRESSION_LEVEL	(5)	/* 1 == Compress faster, 9 == Compress better */
#define DEFAULT_WRITEBUFFERSIZE (16384)

/*
 * DG: all the zip implementation has been updated to minizip 1.1 and is in framework/minizip/zip.cpp
 *     In the D3 BFG implementation there were some things specific to Doom3:
 *     * FILE was replaced by budFile, thus
 *       - fseek(), fread(), fwrite(), ftell() were replaced by budFile functions
 *       - fopen() was replaced by fileSystem->OpenExplicitFileWrite()
 *       - fclose() was replaced by fileSystem->CloseFile()
 *
 *     As this isn't important for the code to work I haven't done these changes, but using the
 *     Doom3 specific functions could be quite easily done by using zipOpen2() with an appropriate
 *     zlib_filefunc_def.
 *
 * TODO: Doom3 should already support zip64 for unzipping, maybe it would make sense to use the
 *       corresponding functions here as well.. but then we can't use budFile, because it only
 *       supports files up to 2GB (Length() and Tell() return ints)
 */

/*
========================
idZipBuilder::AddFileFilters
========================
*/
void idZipBuilder::AddFileFilters( const char* filters )
{
#if 0
	budStrList exts;
	budStrListBreakupString( exts, filters, "|" );
	if( ( exts.Num() > 0 ) && ( exts[ exts.Num() - 1 ] == "" ) )
	{
		exts.RemoveIndex( exts.Num() - 1 );
	}
	filterExts.Append( exts );
#endif
}

/*
========================
idZipBuilder::AddUncompressedFileFilters
========================
*/
void idZipBuilder::AddUncompressedFileFilters( const char* filters )
{
#if 0
	budStrList exts;
	budStrListBreakupString( exts, filters, "|" );
	if( ( exts.Num() > 0 ) && ( exts[ exts.Num() - 1 ] == "" ) )
	{
		exts.RemoveIndex( exts.Num() - 1 );
	}
	uncompressedFilterExts.Append( exts );
#endif
}

/*
========================
idZipBuilder::Build

builds a zip file of all the files in the specified folder, overwriting if necessary
========================
*/
bool idZipBuilder::Build( const char* zipPath, const char* folder, bool cleanFolder )
{
	zipFileName = zipPath;
	sourceFolderName = folder;
	
	if( !CreateZipFile( false ) )
	{
		// don't clean the folder if the zip fails
		return false;
	}
	
	if( cleanFolder )
	{
		CleanSourceFolder();
	}
	return true;
}

/*
========================
idZipBuilder::Update

updates a zip file with the files in the specified folder
========================
*/
bool idZipBuilder::Update( const char* zipPath, const char* folder, bool cleanFolder )
{
	// if this file doesn't exist, just build it
	if( fileSystem->GetTimestamp( zipPath ) == FILE_NOT_FOUND_TIMESTAMP )
	{
		return Build( zipPath, folder, cleanFolder );
	}
	zipFileName = zipPath;
	sourceFolderName = folder;
	
	if( !CreateZipFile( true ) )
	{
		// don't clean the folder if the zip fails
		return false;
	}
	
	if( cleanFolder )
	{
		CleanSourceFolder();
	}
	return true;
}

/*
========================
idZipBuilder::GetFileTime
========================
*/
bool idZipBuilder::GetFileTime( const budStr& filename, unsigned long* dostime ) const
{
	// RB: FIXME
#if defined(_WIN32)
	{
		FILETIME filetime;
		WIN32_FIND_DATA fileData;
		HANDLE			findHandle = FindFirstFile( filename.c_str(), &fileData );
		if( findHandle != INVALID_HANDLE_VALUE )
		{
			FileTimeToLocalFileTime( &( fileData.ftLastWriteTime ), &filetime );
			FileTimeToDosDateTime( &filetime, ( ( LPWORD )dostime ) + 1, ( ( LPWORD )dostime ) + 0 );
			FindClose( findHandle );
			return true;
		}
		FindClose( findHandle );
	}
#endif
	// RB end
	
	return false;
}

/*
========================
idZipBuilder::IsFiltered
========================
*/
bool idZipBuilder::IsFiltered( const budStr& filename ) const
{
	if( filterExts.Num() == 0 && uncompressedFilterExts.Num() == 0 )
	{
		return false;
	}
	for( int j = 0; j < filterExts.Num(); j++ )
	{
		budStr fileExt = budStr( "." + filterExts[j] );
		if( filename.Right( fileExt.Length() ).Icmp( fileExt ) == 0 )
		{
			return false;
		}
	}
	for( int j = 0; j < uncompressedFilterExts.Num(); j++ )
	{
		budStr fileExt = budStr( "." + uncompressedFilterExts[j] );
		if( filename.Right( fileExt.Length() ).Icmp( fileExt ) == 0 )
		{
			return false;
		}
	}
	return true;
}

/*
========================
idZipBuilder::IsUncompressed
========================
*/
bool idZipBuilder::IsUncompressed( const budStr& filename ) const
{
	if( uncompressedFilterExts.Num() == 0 )
	{
		return false;
	}
	for( int j = 0; j < uncompressedFilterExts.Num(); j++ )
	{
		budStr fileExt = budStr( "." + uncompressedFilterExts[j] );
		if( filename.Right( fileExt.Length() ).Icmp( fileExt ) == 0 )
		{
			return true;
		}
	}
	return false;
}

/*
========================
idZipBuilder::CreateZipFile
========================
*/
bool idZipBuilder::CreateZipFile( bool appendFiles )
{
#if 0
//#ifdef ID_PC
	if( zipFileName.IsEmpty() || sourceFolderName.IsEmpty() )
	{
		libBud::Warning( "[%s] - invalid parameters!", __FUNCTION__ );
		return false;
	}
	
	// need to clear the filesystem's zip cache before we can open and write
	//fileSystem->ClearZipCache();
	
	libBud::Printf( "Building zip file: '%s'\n", zipFileName.c_str() );
	
	sourceFolderName.StripTrailing( "\\" );
	sourceFolderName.StripTrailing( "/" );
	
#if 0
	// attempt to check the file out
	if( !Sys_IsFileWritable( zipFileName ) )
	{
		if( ( libBud::sourceControl == NULL ) || !libBud::sourceControl->CheckOut( zipFileName ) )
		{
			libBud::Warning( "READONLY zip file couldn't be checked out: %s", zipFileName.c_str() );
		}
		else
		{
			libBud::Printf( "Checked out: %s\n", zipFileName.c_str() );
		}
	}
#endif
	
	// if not appending, set the file size to zero to "create it from scratch"
	if( !appendFiles )
	{
		libBud::PrintfIf( zip_verbosity.GetBool(), "Overwriting zip file: '%s'\n", zipFileName.c_str() );
		budFile* zipFile = fileSystem->OpenExplicitFileWrite( zipFileName );
		if( zipFile != NULL )
		{
			delete zipFile;
			zipFile = NULL;
		}
	}
	else
	{
		libBud::PrintfIf( zip_verbosity.GetBool(), "Appending to zip file: '%s'\n", zipFileName.c_str() );
	}
	
	// enumerate the files to zip up in the source folder
	budStrStatic< MAX_OSPATH > relPath;
	relPath =
		fileSystem->OSPathToRelativePath( sourceFolderName );
	budFileList* files = fileSystem->ListFilesTree( relPath, "*.*" );
	
	// check to make sure that at least one file will be added to the package
	int atLeastOneFilteredFile = false;
	for( int i = 0; i < files->GetNumFiles(); i++ )
	{
		budStr filename = files->GetFile( i );
		
		if( !IsFiltered( filename ) )
		{
			atLeastOneFilteredFile = true;
			break;
		}
	}
	if( !atLeastOneFilteredFile )
	{
		// although we didn't actually update/create a zip file, it's because no files would be added anyway, which would result in a corrupted zip
		libBud::Printf( "Skipping zip creation/modification, no additional changes need to be made...\n" );
		return true;
	}
	
	// open the zip file
	zipFile zf = zipOpen( zipFileName, appendFiles ? APPEND_STATUS_ADDINZIP : 0 );
	if( zf == NULL )
	{
		libBud::Warning( "[%s] - error opening file '%s'!", __FUNCTION__, zipFileName.c_str() );
		return false;
	}
	
	// add the files to the zip file
	for( int i = 0; i < files->GetNumFiles(); i++ )
	{
	
		// add each file to the zip file
		zip_fileinfo zi;
		memset( &zi, 0, sizeof( zip_fileinfo ) );
		
		budStr filename = files->GetFile( i );
		
		if( IsFiltered( filename ) )
		{
			libBud::PrintfIf( zip_verbosity.GetBool(), "...Skipping: '%s'\n", filename.c_str() );
			continue;
		}
		
		budStr filenameInZip = filename;
		filenameInZip.Strip( relPath );
		filenameInZip.StripLeading( "/" );
		
		budStrStatic< MAX_OSPATH > ospath;
		ospath = fileSystem->RelativePathToOSPath( filename );
		GetFileTime( ospath, &zi.dosDate );
		
		libBud::PrintfIf( zip_verbosity.GetBool(), "...Adding: '%s' ", filenameInZip.c_str() );
		
		int compressionMethod = Z_DEFLATED;
		if( IsUncompressed( filenameInZip ) )
		{
			compressionMethod = 0;
		}
		
		int errcode = zipOpenNewFileInZip3( zf, filenameInZip, &zi, NULL, 0, NULL, 0, NULL /* comment*/,
											compressionMethod,	DEFAULT_COMPRESSION_LEVEL, 0, -MAX_WBITS, DEF_MEM_LEVEL,
											Z_DEFAULT_STRATEGY, NULL /*password*/, 0 /*fileCRC*/ );
											
		if( errcode != ZIP_OK )
		{
			libBud::Warning( "Error opening file in zipfile!" );
			continue;
		}
		else
		{
			// open the source file
			budFile_Permanent src( filename, ospath, FS_READ );
			if( !src.IsOpen() )
			{
				libBud::Warning( "Error opening source file!" );
				continue;
			}
			
			// copy the file data into the zip file
			idTempArray<byte> buffer( DEFAULT_WRITEBUFFERSIZE );
			size_t total = 0;
			while( size_t bytesRead = src.Read( buffer.Ptr(), buffer.Size() ) )
			{
				if( bytesRead > 0 )
				{
					errcode = zipWriteInFileInZip( zf, buffer.Ptr(), ( unsigned int )bytesRead );
					if( errcode != ZIP_OK )
					{
						libBud::Warning( "Error writing to zipfile (%i bytes)!", bytesRead );
						continue;
					}
				}
				total += bytesRead;
			}
			assert( total == ( size_t )src.Length() );
		}
		
		errcode = zipCloseFileInZip( zf );
		if( errcode != ZIP_OK )
		{
			libBud::Warning( "Error zipping source file!" );
			continue;
		}
		libBud::PrintfIf( zip_verbosity.GetBool(), "\n" );
	}
	
	// free the file list
	if( files != NULL )
	{
		fileSystem->FreeFileList( files );
	}
	
	// close the zip file
	int closeError = zipClose( zf, NULL );
	if( closeError != ZIP_OK )
	{
		libBud::Warning( "[%s] - error closing file '%s'!", __FUNCTION__, zipFileName.c_str() );
		return false;
	}
	
	libBud::Printf( "Done.\n" );
	
	return true;
#else
	
	return false;
#endif
	
}

/*
========================
idZipBuilder::CreateZipFileFromFileList
========================
*/
bool idZipBuilder::CreateZipFileFromFileList( const char* name, const budList< budFile_Memory* >& srcFiles )
{
	zipFileName = name;
	return CreateZipFileFromFiles( srcFiles );
}
/*
========================
idZipBuilder::CreateZipFileFromFiles
========================
*/
bool idZipBuilder::CreateZipFileFromFiles( const budList< budFile_Memory* >& srcFiles )
{
	if( zipFileName.IsEmpty() )
	{
		libBud::Warning( "[%s] - invalid parameters!", __FUNCTION__ );
		return false;
	}
	
	// need to clear the filesystem's zip cache before we can open and write
	//fileSystem->ClearZipCache();
	
	libBud::Printf( "Building zip file: '%s'\n", zipFileName.c_str() );
	
	// do not allow overwrite as this should be a tempfile attempt to check the file out
	if( !Sys_IsFileWritable( zipFileName ) )
	{
		libBud::PrintfIf( zip_verbosity.GetBool(), "File %s not writable, cannot proceed.\n", zipFileName.c_str() );
		return false;
	}
	
	// open the zip file
	zipFile zf = zipOpen( zipFileName, 0 );
	if( zf == NULL )
	{
		libBud::Warning( "[%s] - error opening file '%s'!", __FUNCTION__, zipFileName.c_str() );
		return false;
	}
	
	// add the files to the zip file
	for( int i = 0; i < srcFiles.Num(); i++ )
	{
	
		// add each file to the zip file
		zip_fileinfo zi;
		memset( &zi, 0, sizeof( zip_fileinfo ) );
		
		budFile_Memory* src = srcFiles[i];
		src->MakeReadOnly();
		
		libBud::PrintfIf( zip_verbosity.GetBool(), "...Adding: '%s' ", src->GetName() );
		
		int compressionMethod = Z_DEFLATED;
		if( IsUncompressed( src->GetName() ) )
		{
			compressionMethod = 0;
		}
		
		int errcode = zipOpenNewFileInZip3( zf, src->GetName(), &zi, NULL, 0, NULL, 0, NULL /* comment*/,
											compressionMethod,	DEFAULT_COMPRESSION_LEVEL, 0, -MAX_WBITS, DEF_MEM_LEVEL,
											Z_DEFAULT_STRATEGY, NULL /*password*/, 0 /*fileCRC*/ );
											
		if( errcode != ZIP_OK )
		{
			libBud::Warning( "Error opening file in zipfile!" );
			continue;
		}
		else
		{
			// copy the file data into the zip file
			idTempArray<byte> buffer( DEFAULT_WRITEBUFFERSIZE );
			size_t total = 0;
			while( size_t bytesRead = src->Read( buffer.Ptr(), buffer.Size() ) )
			{
				if( bytesRead > 0 )
				{
					errcode = zipWriteInFileInZip( zf, buffer.Ptr(), ( unsigned int )bytesRead );
					if( errcode != ZIP_OK )
					{
						libBud::Warning( "Error writing to zipfile (%" PRIuSIZE " bytes)!", bytesRead );
						continue;
					}
				}
				total += bytesRead;
			}
			assert( total == ( size_t )src->Length() );
		}
		
		errcode = zipCloseFileInZip( zf );
		if( errcode != ZIP_OK )
		{
			libBud::Warning( "Error zipping source file!" );
			continue;
		}
		libBud::PrintfIf( zip_verbosity.GetBool(), "\n" );
	}
	
	// close the zip file
	int closeError = zipClose( zf, zipFileName );
	if( closeError != ZIP_OK )
	{
		libBud::Warning( "[%s] - error closing file '%s'!", __FUNCTION__, zipFileName.c_str() );
		return false;
	}
	
	libBud::PrintfIf( zip_verbosity.GetBool(), "Done.\n" );
	
	return true;
}

/*
========================
idZipBuilder::CleanSourceFolder

this folder is assumed to be a path under FSPATH_BASE
========================
*/
zipFile idZipBuilder::CreateZipFile( const char* name )
{
	libBud::Printf( "Creating zip file: '%s'\n", name );
	
	// do not allow overwrite as this should be a tempfile attempt to check the file out
	if( !Sys_IsFileWritable( name ) )
	{
		libBud::PrintfIf( zip_verbosity.GetBool(), "File %s not writable, cannot proceed.\n", name );
		return NULL;
	}
	
	// open the zip file
	zipFile zf = zipOpen( name, 0 );
	if( zf == NULL )
	{
		libBud::Warning( "[%s] - error opening file '%s'!", __FUNCTION__, name );
	}
	return zf;
}

/*
========================
idZipBuilder::CleanSourceFolder

this folder is assumed to be a path under FSPATH_BASE
========================
*/
bool idZipBuilder::AddFile( zipFile zf, budFile_Memory* src, bool deleteFile )
{
	// add each file to the zip file
	zip_fileinfo zi;
	memset( &zi, 0, sizeof( zip_fileinfo ) );
	
	
	src->MakeReadOnly();
	
	libBud::PrintfIf( zip_verbosity.GetBool(), "...Adding: '%s' ", src->GetName() );
	
	int compressionMethod = Z_DEFLATED;
	if( IsUncompressed( src->GetName() ) )
	{
		compressionMethod = Z_NO_COMPRESSION;
	}
	
	int errcode = zipOpenNewFileInZip3( zf, src->GetName(), &zi, NULL, 0, NULL, 0, NULL /* comment*/,
										compressionMethod,	DEFAULT_COMPRESSION_LEVEL, 0, -MAX_WBITS, DEF_MEM_LEVEL,
										Z_DEFAULT_STRATEGY, NULL /*password*/, 0 /*fileCRC*/ );
										
	if( errcode != ZIP_OK )
	{
		libBud::Warning( "Error opening file in zipfile!" );
		if( deleteFile )
		{
			src->Clear( true );
			delete src;
		}
		return false;
	}
	else
	{
		// copy the file data into the zip file
		idTempArray<byte> buffer( DEFAULT_WRITEBUFFERSIZE );
		size_t total = 0;
		while( size_t bytesRead = src->Read( buffer.Ptr(), buffer.Size() ) )
		{
			if( bytesRead > 0 )
			{
				errcode = zipWriteInFileInZip( zf, buffer.Ptr(), ( unsigned int )bytesRead );
				if( errcode != ZIP_OK )
				{
					libBud::Warning( "Error writing to zipfile (%" PRIuSIZE " bytes)!", bytesRead );
					continue;
				}
			}
			total += bytesRead;
		}
		assert( total == ( size_t )src->Length() );
	}
	
	errcode = zipCloseFileInZip( zf );
	if( errcode != ZIP_OK )
	{
		libBud::Warning( "Error zipping source file!" );
		if( deleteFile )
		{
			src->Clear( true );
			delete src;
		}
		return false;
	}
	libBud::PrintfIf( zip_verbosity.GetBool(), "\n" );
	if( deleteFile )
	{
		src->Clear( true );
		delete src;
	}
	return true;
}

/*
========================
idZipBuilder::CleanSourceFolder

this folder is assumed to be a path under FSPATH_BASE
========================
*/
void idZipBuilder::CloseZipFile( zipFile zf )
{
	// close the zip file
	int closeError = zipClose( zf, zipFileName );
	if( closeError != ZIP_OK )
	{
		libBud::Warning( "[%s] - error closing file '%s'!", __FUNCTION__, zipFileName.c_str() );
	}
	libBud::PrintfIf( zip_verbosity.GetBool(), "Done.\n" );
}
/*
========================
idZipBuilder::CleanSourceFolder

this folder is assumed to be a path under FSPATH_BASE
========================
*/
void idZipBuilder::CleanSourceFolder()
{
#if 0
//#ifdef ID_PC_WIN
	budStrList deletedFiles;
	
	// make sure this is a valid path, we don't want to go nuking
	// some user path  or something else unintentionally
	budStr ospath = sourceFolderName;
	ospath.SlashesToBackSlashes();
	ospath.ToLower();
	
	char relPath[MAX_OSPATH];
	fileSystem->OSPathToRelativePath( ospath, relPath, MAX_OSPATH );
	
	// get the game's base path
	budStr basePath = fileSystem->GetBasePathStr( FSPATH_BASE );
	basePath.AppendPath( BASE_GAMEDIR );
	basePath.AppendPath( "maps" );
	basePath.SlashesToBackSlashes();
	basePath.ToLower();
	// path must be off of our base path, ospath can't have .map on the end, and
	// do some additional sanity checks
	if( ( ospath.Find( basePath ) == 0 ) && ( ospath.Right( 4 ) != ".map" ) &&
			( ospath != "c:\\" ) && ( ospath.Length() > basePath.Length() ) )
	{
		// get the files in the current directory
		budFileList* files = fileSystem->ListFilesTree( relPath, "*.*" );
		if( files->GetNumFiles() && zip_verbosity.GetBool() )
		{
			libBud::Printf( "Deleting files in '%s'...\n", relPath );
		}
		for( int i = 0; i < files->GetNumFiles(); i++ )
		{
			if( IsFiltered( files->GetFile( i ) ) )
			{
				continue;
			}
			// nuke 'em
			if( zip_verbosity.GetBool() )
			{
				libBud::Printf( "\t...%s\n", files->GetFile( i ) );
			}
			fileSystem->RemoveFile( files->GetFile( i ) );
			
			char ospath2[MAX_OSPATH];
			fileSystem->RelativePathToOSPath( files->GetFile( i ), ospath2, MAX_OSPATH );
			deletedFiles.Append( ospath2 );
		}
		fileSystem->FreeFileList( files );
		fileSystem->RemoveDir( relPath );
	}
	else
	{
		libBud::Printf( "Warning: idZipBuilder::CleanSourceFolder - Non-standard path: '%s'!\n", ospath.c_str() );
		return;
	}
	
	// figure out which deleted files need to be removed from source control, and then remove those files
	budStrList filesToRemoveFromSourceControl;
	for( int i = 0; i < deletedFiles.Num(); i++ )
	{
		scFileStatus_t fileStatus = libBud::sourceControl->GetFileStatus( deletedFiles[ i ] );
		if( SCF_IS_IN_SOURCE_CONTROL( fileStatus ) )
		{
			filesToRemoveFromSourceControl.Append( deletedFiles[ i ] );
		}
	}
	if( filesToRemoveFromSourceControl.Num() > 0 )
	{
		libBud::sourceControl->Delete( filesToRemoveFromSourceControl );
	}
	
#endif
}

/*
========================
idZipBuilder::BuildMapFolderZip
========================
*/
const char* ZIP_FILE_EXTENSION = "pk4";
bool idZipBuilder::BuildMapFolderZip( const char* mapFileName )
{
	budStr zipFileName = mapFileName;
	zipFileName.SetFileExtension( ZIP_FILE_EXTENSION );
	budStr pathToZip = mapFileName;
	pathToZip.StripFileExtension();
	idZipBuilder zip;
	zip.AddFileFilters( "bcm|bmodel|proc|" );
	zip.AddUncompressedFileFilters( "genmodel|sbcm|tbcm|" );
	bool success = zip.Build( zipFileName, pathToZip, true );
	// even if the zip build failed we want to clear the source folder so no contributing files are left around
	if( !success )
	{
		zip.CleanSourceFolder();
	}
	return success;
}

/*
========================
idZipBuilder::UpdateMapFolderZip
========================
*/
bool idZipBuilder::UpdateMapFolderZip( const char* mapFileName )
{
	budStr zipFileName = mapFileName;
	zipFileName.SetFileExtension( ZIP_FILE_EXTENSION );
	budStr pathToZip = mapFileName;
	pathToZip.StripFileExtension();
	idZipBuilder zip;
	zip.AddFileFilters( "bcm|bmodel|proc|" );
	zip.AddUncompressedFileFilters( "genmodel|sbcm|tbcm|" );
	bool success = zip.Update( zipFileName, pathToZip, true );
	// even if the zip build failed we want to clear the source folder so no contributing files are left around
	if( !success )
	{
		zip.CleanSourceFolder();
	}
	return success;
}

/*
========================
idZipBuilder::CombineFiles
========================
*/
budFile_Memory* idZipBuilder::CombineFiles( const budList< budFile_Memory* >& srcFiles )
{
	budFile_Memory* destFile = NULL;
	
#if 0
//#ifdef ID_PC

	// create a new temp file so we can zip into it without refactoring the zip routines
	char ospath[MAX_OSPATH];
	const char* tempName = "temp.tmp";
	fileSystem->RelativePathToOSPath( tempName, ospath, MAX_OSPATH, FSPATH_SAVE );
	fileSystem->RemoveFile( ospath );
	
	// combine src files into dest filename just specified
	idZipBuilder zip;
	zip.zipFileName = ospath;
	bool ret = zip.CreateZipFileFromFiles( srcFiles );
	
	// read the temp file created into a memory file to return
	if( ret )
	{
		destFile = new budFile_Memory();
		
		if( !destFile->Load( tempName, ospath ) )
		{
			assert( false && "couldn't read the combined file" );
			delete destFile;
			destFile = NULL;
		}
		
		// delete the temp file
		fileSystem->RemoveFile( ospath );
		
		// make the new file readable
		destFile->MakeReadOnly();
	}
	
#endif
	
	return destFile;
}

CONSOLE_COMMAND( testZipBuilderCombineFiles, "test routine for memory zip file building", 0 )
{
#if 0
	budList< budFile_Memory* > list;
	const char* 	testString = "test";
	int				numFiles = 2;
	
	if( args.Argc() > 2 )
	{
		libBud::Printf( "usage: testZipBuilderExtractFiles [numFiles]\n" );
		return;
	}
	
	for( int arg = 1; arg < args.Argc(); arg++ )
	{
		numFiles = atoi( args.Argv( arg ) );
	}
	
	// allocate all the test files
	for( int i = 0; i < numFiles; i++ )
	{
		budFile_Memory* file = new budFile_Memory( va( "%s%d.txt", testString, i + 1 ) );
		file->MakeWritable();
		budStr str = va( "%s%d", testString, i + 1 );
		file->WriteString( str );
		list.Append( file );
	}
	
	// combine the files into a single memory file
	idZipBuilder zip;
	budFile_Memory* file = zip.CombineFiles( list );
	if( file != NULL )
	{
		file->MakeReadOnly();
		
		char ospath[MAX_OSPATH];
		const char* tempName = "temp.zip";
		fileSystem->RelativePathToOSPath( tempName, ospath, MAX_OSPATH, FSPATH_SAVE );
		
		// remove previous file if it exists
		fileSystem->RemoveFile( ospath );
		
		if( file->Save( tempName, ospath ) )
		{
			libBud::PrintfIf( zip_verbosity.GetBool(), va( "File written: %s.\n", ospath ) );
		}
		else
		{
			libBud::Error( "Could not save the file." );
		}
		
		delete file;
	}
	
	list.DeleteContents();
#endif
	// Now look at the temp.zip, unzip it to see if it works
}

/*
========================
idZipBuilder::ExtractFiles
========================
*/
bool idZipBuilder::ExtractFiles( budFile_Memory*& srcFile, budList< budFile_Memory* >& destFiles )
{
	bool ret = false;
	
#if 0
//#ifdef ID_PC

	destFiles.Clear();
	
	// write the memory file to temp storage so we can unzip it without refactoring the unzip routines
	char ospath[MAX_OSPATH];
	const char* tempName = "temp.tmp";
	fileSystem->RelativePathToOSPath( tempName, ospath, MAX_OSPATH, FSPATH_SAVE );
	ret = srcFile->Save( tempName, ospath );
	assert( ret && "couldn't create temp file" );
	
	if( ret )
	{
	
		libBud::PrintfIf( zip_verbosity.GetBool(), "Opening archive %s:\n", ospath );
		unzFile zip = unzOpen( ospath );
		
		int numFiles = 0;
		int result = unzGoToFirstFile( zip );
		while( result == UNZ_OK )
		{
			numFiles++;
			unz_file_info curFileInfo;
			char fileName[MAX_OSPATH];
			unzGetCurrentFileInfo( zip, &curFileInfo, fileName, MAX_OSPATH, NULL, 0, NULL, 0 );
			
			libBud::PrintfIf( zip_verbosity.GetBool(), "%d: %s, size: %d \\ %d\n", numFiles, fileName, curFileInfo.compressed_size, curFileInfo.uncompressed_size );
			
			// create a buffer big enough to hold the entire uncompressed file
			void* buff = Mem_Alloc( curFileInfo.uncompressed_size, TAG_TEMP );
			result = unzOpenCurrentFile( zip );
			if( result == UNZ_OK )
			{
				result = unzReadCurrentFile( zip, buff, curFileInfo.uncompressed_size );
				unzCloseCurrentFile( zip );
			}
			
			// create the new memory file
			budFile_Memory* outFile = new budFile_Memory( fileName );
			outFile->SetReadOnlyData( ( const char* )buff, curFileInfo.uncompressed_size );
			
			destFiles.Append( outFile );
			
			result = unzGoToNextFile( zip );
		}
		
		// close it so we can delete the zip file and create a new one
		unzClose( zip );
		
		// delete the temp zipfile
		fileSystem->RemoveFile( ospath );
	}
	
#endif
	
	return ret;
}

CONSOLE_COMMAND( testZipBuilderExtractFiles, "test routine for memory zip file extraction", 0 )
{
#if 0
	budList< budFile_Memory* > list;
	budFile_Memory* zipfile;
	const char* 	testString = "test";
	int				numFiles = 2;
	bool			overallSuccess = true;
	bool			success = true;
	
	if( args.Argc() > 2 )
	{
		libBud::Printf( "usage: testZipBuilderExtractFiles [numFiles]\n" );
		return;
	}
	
	for( int arg = 1; arg < args.Argc(); arg++ )
	{
		numFiles = atoi( args.Argv( arg ) );
	}
	
	// create a temp.zip file with string files
	{
		// allocate all the test files
		for( int i = 0; i < numFiles; i++ )
		{
			budFile_Memory* file = new budFile_Memory( va( "%s%d.txt", testString, i + 1 ) );
			file->MakeWritable();
			budStr str = va( "%s%d", testString, i + 1 );
			file->WriteString( str );
			list.Append( file );
		}
		
		// combine the files into a single memory file
		idZipBuilder zip;
		zipfile = zip.CombineFiles( list );
		
		success = ( zipfile != NULL );
		overallSuccess &= success;
		libBud::Printf( "Zip file created: %s\n", success ? "^2PASS" : "^1FAIL" );
		
		// destroy all the test files
		list.DeleteContents();
	}
	
	// unzip the file into separate memory files
	if( overallSuccess )
	{
	
		// extract all the test files using the single zip file from above
		idZipBuilder zip;
		if( !zip.ExtractFiles( zipfile, list ) )
		{
			libBud::Error( "Could not extract files." );
		}
		
		success = ( list.Num() == numFiles );
		overallSuccess &= success;
		libBud::Printf( "Number of files: %s\n", success ? "^2PASS" : "^1FAIL" );
		
		for( int i = 0; i < list.Num(); i++ )
		{
			budStr str;
			budFile_Memory* file = list[i];
			file->MakeReadOnly();
			file->ReadString( str );
			
			budStr filename = va( "%s%d.txt", testString, i + 1 );
			budStr contents = va( "%s%d", testString, i + 1 );
			
			// test the filename
			bool nameSuccess = ( file->GetName() == filename );
			overallSuccess &= nameSuccess;
			
			// test the string
			bool contentSuccess = ( str == contents );
			overallSuccess &= contentSuccess;
			
			libBud::Printf( "Extraction of file, %s: %s^0, contents check: %s\n", filename.c_str(), nameSuccess ? "^2PASS" : "^1FAIL", contentSuccess ? "^2PASS" : "^1FAIL" );
		}
		
		list.DeleteContents();
	}
	
	if( zipfile != NULL )
	{
		delete zipfile;
	}
	
	libBud::Printf( "[%s] overall tests: %s\n", __FUNCTION__, overallSuccess ? "^2PASS" : "^1FAIL" );
#endif
}
