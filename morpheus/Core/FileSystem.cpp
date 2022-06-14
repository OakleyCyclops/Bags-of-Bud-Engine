#include "CorePCH.hpp"

/*
==================
FileSystem::Init
Where the real shit begins
==================
*/
void FileSystem::Init(int argc, const char** argv)
{
    ConsoleShell::Print("============ Initializing Virtual File System ============\n");
    RegisterCVarsAndCmds();

    if (!ReadDistroTXT())
    {
        WriteDistroTXT();
    }

}

/*
==================
Command functions
==================
*/
void FileSystem::cmdRestart()
{
    Shutdown(true);
}


/*
==================
FileSystem::RegisterCVarsAndCmds
==================
*/
void FileSystem::RegisterCVarsAndCmds()
{
    Console::Register(&vfs_baseDirOverride);
    Console::Register(&vfs_restart);
}

/*
==================
FileSystem::Shutdown
==================
*/
void FileSystem::Shutdown(bool restarting)
{

}

/*
==================
FileSystem::Restart
==================
*/
void FileSystem::Restart()
{

}

/*
==================
FileSystem::ReadDistroTXT
==================
*/
bool FileSystem::ReadDistroTXT()
{
    // Create a string with our path
    char* path;
    char** file;
    
    String txt = VFS_PREFIX;

    StringMethods::Append(&txt, distroTXT);

    file = ReadTXT(txt.Cstring);

    // If fopen is null then print a warning, this isn't considered an error because we'll write a new txt file as a fallback
    if (file == nullptr)
    {
        ConsoleShell::Warning("Can't find Distro.txt");
        return false;
    }

}

/*
==================
FileSystem::WriteDistroTXT
==================
*/
void FileSystem::WriteDistroTXT()
{

}

/*
==================
FileSystem::LoadCFG
==================
*/
void FileSystem::LoadCFG(int cfgFile)
{

}

/*
==================
FileSystem::ReadVFStxt
==================
*/
char** FileSystem::ReadTXT(const char* path)
{
    #ifdef BUDPOSIX

        // First time messing with any low level kernel calls
        // Unlike the standard library, i can straight up just get all the contents from a file without any loop nonsense

        int txtFile;

        char* buffer;
        String string;

        struct stat64 stat;

        txtFile = OpenTXT(path, 0);

        if (txtFile == NULL)
        {
            return nullptr;
        }

        fstat64(txtFile, &stat);

        buffer = (char*)mmap64(0, stat.st_size, PROT_READ, MAP_PRIVATE, txtFile, 0);

        unsigned int i = 0;
        unsigned int j = 0;
        unsigned int k = 0;

        char c = buffer[i];

        // Just in case
        if (buffer == (void*)-1)
        {
            return nullptr;
        }

        // First we iterate on how big we need the char** to be
        while (c != '\0')
        {
            c = buffer[i];

            if (c == '\n')
            {
                j++;
            }

            i++;
        }

        char* lines[j + 1];
        
        // Reset i, j, and c
        i = 0;
        j = 0;
        c = buffer[i];

        // Then until we come across a \n, we take all the chars before then and append them to a string

        while (c != '\0')
        {
            c = buffer[i];
            k++;

            if (c != '\n')
            {
                StringMethods::Append(&string, c);
            }
            
            else if (c == '\n')
            {
                lines[j] = new char[k];
                
                strcpy(lines[j], string.Cstring);
                StringMethods::Clear(&string);

                k = 0;
                j++;
            }

            i++;
        }

        return lines;


    #endif

}

/*
==================
FileSystem::OpenVFSfileTXT
==================
*/
int FileSystem::OpenTXT(const char* vfsPath, int fileType)
{
    int fp;

    String path;

    VFSPath2OSPath(path, vfsPath);

    if (path == "")
    {
        return NULL;
    }

    #ifdef BUDPOSIX
        switch (fileType)
        {
            // Read only
            case 0:
            {
                fp = open64(path.Cstring, O_RDONLY);

                
                if (fp < 0)
                {
                    return NULL;
                }

                else
                {
                    return fp;
                }
            }

            // Write only
            case 1:
            {
                fp = open64(path.Cstring, O_WRONLY);

                if (fp < 0)
                {
                    return NULL;
                }

                else
                {
                    return fp;
                }   
            }

            // Read and write
            case 2:
            {
                fp = open64(path.Cstring, O_RDWR);

                if (fp < 0)
                {
                    return NULL;
                }

                else
                {
                    return fp;
                }       
            }
        }
    #endif
}

/*
==================
FileSystem::OSPath2VFSPath
==================
*/
String& FileSystem::OSPath2VFSPath(const char* path, bool isInRootFolder)
{









}

/*
==================
FileSystem::VFSPath2OSPath
==================
*/
String& FileSystem::VFSPath2OSPath(String& realPath, const char* path)
{

    if (CharMethods::FindText(path, VFS_PREFIX, true, 0, (strlen(VFS_PREFIX))))
    {
        StringMethods::Append(&realPath, GetEXEDir());

        #ifdef BUDPOSIX
            StringMethods::Append(&realPath, '/');
        #endif

        #ifdef BUDWINDOWS
            StringMethods::Append(realPath, '\\');
        #endif
        
        String* file = new String;
        
        StringMethods::Append(file, path);
        StringMethods::StripLeading(file, VFS_PREFIX);

        StringMethods::Append(&realPath, file->Cstring);

        delete file;
        return realPath;
    }

    else if (CharMethods::FindText(path, OS_PREFIX, true, 0, (strlen(OS_PREFIX))))
    {
        String* file = new String;
        
        StringMethods::Append(file, path);
        StringMethods::StripLeading(file, OS_PREFIX);

        // I hate windows and it's backslash bullshit why cant you just be normal 
        #ifdef BUDWINDOWS
            StringMethods::BackSlashesToForwardSlashes(&file);
        #endif

        StringMethods::Append(&realPath, file->Cstring);

        delete file;
        return realPath;
    }

    else
    {
        return realPath;
    }
}

/*
==================
FileSystem::LoadDLL
==================
*/
void FileSystem::LoadDLL(const char* dllName)
{

}

/*
==================
FileSystem::UnloadDLL
==================
*/
void FileSystem::UnloadDLL()
{

}

/*
==================
FileSystem::GetEXEDir
==================
*/
const char* FileSystem::GetEXEDir()
{
    #ifdef BUDPOSIX
        char path[PATH_MAX] = {};
        const char* result;
        ssize_t count;

        count = readlink("/proc/self/exe", path, PATH_MAX);

        if (count != -1)
        {
            result = dirname(path);
        }

        return result;
    #endif

    #ifdef BUDWINDOWS

    #endif
}