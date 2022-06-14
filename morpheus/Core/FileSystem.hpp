#ifndef __FILE_SYSTEM_HPP__
#define __FILE_SYSTEM_HPP__

#ifdef BUDPOSIX
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/statvfs.h>
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

/*
=========
FileSystem
=========
*/
namespace FileSystem
{
    void Init(int argc, const char** argv);
    void Restart();
    void RegisterCVarsAndCmds();

    void SetupGameDirs(const char* dirName);

    bool ReadDistroTXT();
    void WriteDistroTXT();

    // Filetypes: 0 = read text, 1 = write text, 2 = read and write text
    int OpenTXT(const char* path, int fileType);
    int CloseTXT(const char* path);

    char** ReadTXT(const char* path);
    void WriteTXT(const char* path);

    // Not to be confused with converting paths to ones with a different prefix
    // These functions actually convert a path with either a VFS:// or OS:// prefix
    // To an actual path that can be used by the lower level OS functions
    // true = VFS://, false = OS://
    String& OSPath2VFSPath(const char* path, bool isInRootFolder);

    // Not to be confused with converting paths to ones with a different prefix
    // These functions actually convert a path with either a VFS:// or OS:// prefix
    // To an actual path that can be used by the lower level OS functions
    String& VFSPath2OSPath(String& str, const char* path);

    void LoadCFG(int cfgFile);
    void LoadDLL(const char* dllName);
    void UnloadDLL();

    const char* GetEXEDir();

    void Shutdown(bool restarting);

    void cmdRestart();

    inline const char* distroTXT = "Distro.txt";
    inline const char* gameTXT = "GameMeta.txt";

    inline CVar vfs_baseDirOverride = {"vfs_baseDirOverride", new char*(""), "Overrides the folder to search specified by DistroInfo.txt", CVAR_STRING | CVAR_CORE | CVAR_INIT};
    inline Cmd vfs_restart = {"vfs_restart", cmdRestart, "Restarts the filesystem", CMD_CORE};
};

// Prefix for paths that start in the same folder as the executable
#define VFS_PREFIX "VFS://"

// Prefix for paths that start in / on linux or C:\ on windows etc
#define OS_PREFIX "OS://"

#endif /* !__FILE_SYSTEM_HPP__ */