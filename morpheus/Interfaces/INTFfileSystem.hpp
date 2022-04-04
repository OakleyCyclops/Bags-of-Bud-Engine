
class budFile
{
public:
	virtual					~budFile() {};
	// Get the name of the file.
	virtual const char* 	GetName() const;
	// Get the full file path.
	virtual const char* 	GetFullPath() const;
	// Read data from the file to the buffer.
	virtual int				Read( void* buffer, int len );
	// Write data from the buffer to the file.
	virtual int				Write( const void* buffer, int len );
	// Returns the length of the file.
	virtual int				Length() const;
	// Return a time value for reload operations.
	virtual ID_TIME_T		Timestamp() const;
	// Returns offset in file.
	virtual int				Tell() const;
	// Forces flush on files being writting to.
	virtual void			ForceFlush();
	// Causes any buffered data to be written to the file.
	virtual void			Flush();
	// Seek on a file.
	virtual int				Seek( long offset, fsOrigin_t origin );
	// Go back to the beginning of the file.
	virtual void			Rewind();
	// Like fprintf.
	virtual int				Printf( VERIFY_FORMAT_STRING const char* fmt, ... );
	// Like fprintf but with argument pointer
	virtual int				VPrintf( const char* fmt, va_list arg );
	// Write a string with high precision floating point numbers to the file.
	virtual int				WriteFloatString( VERIFY_FORMAT_STRING const char* fmt, ... );
	
	template<class type> BUD_INLINE size_t ReadBig( type& c )
	{
		size_t r = Read( &c, sizeof( c ) );
		idSwap::Big( c );
		return r;
	}
	
	template<class type> BUD_INLINE size_t ReadBigArray( type* c, int count )
	{
		size_t r = Read( c, sizeof( c[0] ) * count );
		idSwap::BigArray( c, count );
		return r;
	}
	
	template<class type> BUD_INLINE size_t WriteBig( const type& c )
	{
		type b = c;
		idSwap::Big( b );
		return Write( &b, sizeof( b ) );
	}
	
	template<class type> BUD_INLINE size_t WriteBigArray( const type* c, int count )
	{
		size_t r = 0;
		for( int i = 0; i < count; i++ )
		{
			r += WriteBig( c[i] );
		}
		return r;
	}
};

class budFileSystem
{
    public:
        virtual void			Init() = 0;
        virtual void			Restart() = 0;
        virtual void			Shutdown( bool reloading ) = 0;
        virtual bool			IsInitialized() const = 0;

        // Lists files with the given extension in the given directory.
        // Directory should not have either a leading or trailing '/'
        // The returned files will not include any directories or '/' unless fullRelativePath is set.
        // The extension must include a leading dot and may not contain wildcards.
        // If extension is "/", only subdirectories will be returned.

        virtual budFileList* 	ListFiles( const char* relativePath, const char* extension, bool sort = false, bool fullRelativePath = false, const char* gamedir = NULL ) = 0;
        
        // Lists files in the given directory and all subdirectories with the given extension.
        // Directory should not have either a leading or trailing '/'
        // The returned files include a full relative path.
        // The extension must include a leading dot and may not contain wildcards.
        virtual budFileList* 	ListFilesTree( const char* relativePath, const char* extension, bool sort = false, const char* gamedir = NULL ) = 0;
        // Frees the given file list.
        virtual void			FreeFileList( budFileList* fileList ) = 0;
        // Converts a relative path to a full OS path.
        virtual const char* 	OSPathToRelativePath( const char* OSPath ) = 0;
        // Converts a full OS path to a relative path.
        virtual const char* 	RelativePathToOSPath( const char* relativePath, const char* basePath = "fs_basepath" ) = 0;
        // Builds a full OS path from the given components.
        virtual const char* 	BuildOSPath( const char* base, const char* game, const char* relativePath ) = 0;
        virtual const char* 	BuildOSPath( const char* base, const char* relativePath ) = 0;
        // Creates the given OS path for as far as it doesn't exist already.
        virtual void			CreateOSPath( const char* OSPath ) = 0;
        // Reads a complete file.
        // Returns the length of the file, or -1 on failure.
        // A null buffer will just return the file length without loading.
        // A null timestamp will be ignored.
        // As a quick check for existance. -1 length == not present.
        // A 0 byte will always be appended at the end, so string ops are safe.
        // The buffer should be considered read-only, because it may be cached for other uses.
        virtual int				ReadFile( const char* relativePath, void** buffer, ID_TIME_T* timestamp = NULL ) = 0;
        // Frees the memory allocated by ReadFile.
        virtual void			FreeFile( void* buffer ) = 0;
        // Writes a complete file, will create any needed subdirectories.
        // Returns the length of the file, or -1 on failure.
        virtual int				WriteFile( const char* relativePath, const void* buffer, int size, const char* basePath = "fs_savepath" ) = 0;
        // Removes the given file.
        virtual void			RemoveFile( const char* relativePath ) = 0;
        // Removes the specified directory.
        virtual	bool			RemoveDir( const char* relativePath ) = 0;
        // Renames a file, taken from idTech5 (minus the fsPath_t)
        virtual bool			RenameFile( const char* relativePath, const char* newName, const char* basePath = "fs_savepath" ) = 0;
        // Opens a file for reading.
        virtual budFile* 		OpenFileRead( const char* relativePath, bool allowCopyFiles = true, const char* gamedir = NULL ) = 0;
        // Opens a file for reading, reads the file completely in memory and returns an budFile_Memory obj.
        virtual budFile* 		OpenFileReadMemory( const char* relativePath, bool allowCopyFiles = true, const char* gamedir = NULL ) = 0;
        // Opens a file for writing, will create any needed subdirectories.
        virtual budFile* 		OpenFileWrite( const char* relativePath, const char* basePath = "fs_savepath" ) = 0;
        // Opens a file for writing at the end.
        virtual budFile* 		OpenFileAppend( const char* filename, bool sync = false, const char* basePath = "fs_basepath" ) = 0;
        // Opens a file for reading, writing, or appending depending on the value of mode.
        virtual budFile* 		OpenFileByMode( const char* relativePath, fsMode_t mode ) = 0;
        // Opens a file for reading from a full OS path.
        virtual budFile* 		OpenExplicitFileRead( const char* OSPath ) = 0;
        // Opens a file for writing to a full OS path.
        virtual budFile* 		OpenExplicitFileWrite( const char* OSPath ) = 0;
        // opens a zip container
        virtual budFile_Cached* 		OpenExplicitPakFile( const char* OSPath ) = 0;
        // Closes a file.
        virtual void			CloseFile( budFile* f ) = 0;
        // look for a dynamic module
        virtual void			FindDLL( const char* basename, char dllPath[ MAX_OSPATH ] ) = 0;
        
        // don't use for large copies - allocates a single memory block for the copy
        virtual void			CopyFile( const char* fromOSPath, const char* toOSPath ) = 0;
        
        // look for a file in the loaded paks or the addon paks
        // if the file is found in addons, FS's internal structures are ready for a reloadEngine
        virtual findFile_t		FindFile( const char* path ) = 0;
        
        // ignore case and seperator char distinctions
        virtual bool			FilenameCompare( const char* s1, const char* s2 ) const = 0;
        
        // Returns length of file, -1 if no file exists
        virtual int				GetFileLength( const char* relativePath ) = 0;
        
        virtual sysFolder_t		IsFolder( const char* relativePath, const char* basePath = "fs_basepath" ) = 0;
        
};