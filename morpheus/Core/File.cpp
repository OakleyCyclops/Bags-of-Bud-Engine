/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2012 Robert Beckebans

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

#include "PCH.hpp"
#pragma hdrstop

#include "Unzip.h"


/*
=================
FS_WriteFloatString
=================
*/
int FS_WriteFloatString( char* buf, const char* fmt, va_list argPtr )
{
	// DG: replaced long with int for 64bit compatibility in the whole function
	int i;
	unsigned int u;
	double f;
	char* str;
	int index;
	budStr tmp, format;
	
	index = 0;
	
	while( *fmt )
	{
		switch( *fmt )
		{
			case '%':
				format = "";
				format += *fmt++;
				while( ( *fmt >= '0' && *fmt <= '9' ) ||
						*fmt == '.' || *fmt == '-' || *fmt == '+' || *fmt == '#' )
				{
					format += *fmt++;
				}
				format += *fmt;
				switch( *fmt )
				{
					case 'f':
					case 'e':
					case 'E':
					case 'g':
					case 'G':
						f = va_arg( argPtr, double );
						if( format.Length() <= 2 )
						{
							// high precision floating point number without trailing zeros
							sprintf( tmp, "%1.10f", f );
							tmp.StripTrailing( '0' );
							tmp.StripTrailing( '.' );
							index += sprintf( buf + index, "%s", tmp.c_str() );
						}
						else
						{
							index += sprintf( buf + index, format.c_str(), f );
						}
						break;
					case 'd':
					case 'i':
						i = va_arg( argPtr, int );
						index += sprintf( buf + index, format.c_str(), i );
						break;
					case 'u':
						u = va_arg( argPtr, unsigned int );
						index += sprintf( buf + index, format.c_str(), u );
						break;
					case 'o':
						u = va_arg( argPtr, unsigned int );
						index += sprintf( buf + index, format.c_str(), u );
						break;
					case 'x':
						u = va_arg( argPtr, unsigned int );
						index += sprintf( buf + index, format.c_str(), u );
						break;
					case 'X':
						u = va_arg( argPtr, unsigned int );
						index += sprintf( buf + index, format.c_str(), u );
						break;
					case 'c':
						i = va_arg( argPtr, int );
						index += sprintf( buf + index, format.c_str(), ( char ) i );
						break;
					case 's':
						str = va_arg( argPtr, char* );
						index += sprintf( buf + index, format.c_str(), str );
						break;
					case '%':
						index += sprintf( buf + index, "%s", format.c_str() );
						break;
					default:
						common->Error( "FS_WriteFloatString: invalid format %s", format.c_str() );
						break;
				}
				fmt++;
				break;
			case '\\':
				fmt++;
				switch( *fmt )
				{
					case 't':
						index += sprintf( buf + index, "\t" );
						break;
					case 'v':
						index += sprintf( buf + index, "\v" );
						break;
					case 'n':
						index += sprintf( buf + index, "\n" );
						break;
					case '\\':
						index += sprintf( buf + index, "\\" );
						break;
					default:
						common->Error( "FS_WriteFloatString: unknown escape character \'%c\'", *fmt );
						break;
				}
				fmt++;
				break;
			default:
				index += sprintf( buf + index, "%c", *fmt );
				fmt++;
				break;
		}
	}
	
	return index;
	// DG end
}

/*
=================================================================================

budFile

=================================================================================
*/

/*
=================
budFile::GetName
=================
*/
const char* budFile::GetName() const
{
	return "";
}

/*
=================
budFile::GetFullPath
=================
*/
const char* budFile::GetFullPath() const
{
	return "";
}

/*
=================
budFile::Read
=================
*/
int budFile::Read( void* buffer, int len )
{
	common->FatalError( "budFile::Read: cannot read from budFile" );
	return 0;
}

/*
=================
budFile::Write
=================
*/
int budFile::Write( const void* buffer, int len )
{
	common->FatalError( "budFile::Write: cannot write to budFile" );
	return 0;
}

/*
=================
budFile::Length
=================
*/
int budFile::Length() const
{
	return 0;
}

/*
=================
budFile::Timestamp
=================
*/
ID_TIME_T budFile::Timestamp() const
{
	return 0;
}

/*
=================
budFile::Tell
=================
*/
int budFile::Tell() const
{
	return 0;
}

/*
=================
budFile::ForceFlush
=================
*/
void budFile::ForceFlush()
{
}

/*
=================
budFile::Flush
=================
*/
void budFile::Flush()
{
}

/*
=================
budFile::Seek
=================
*/
int budFile::Seek( long offset, fsOrigin_t origin )
{
	return -1;
}

/*
=================
budFile::Rewind
=================
*/
void budFile::Rewind()
{
	Seek( 0, FS_SEEK_SET );
}

/*
=================
budFile::Printf
=================
*/
int budFile::Printf( const char* fmt, ... )
{
	char buf[MAX_PRINT_MSG];
	int length;
	va_list argptr;
	
	va_start( argptr, fmt );
	length = budStr::vsnPrintf( buf, MAX_PRINT_MSG - 1, fmt, argptr );
	va_end( argptr );
	
	// so notepad formats the lines correctly
	budStr	work( buf );
	work.Replace( "\n", "\r\n" );
	
	return Write( work.c_str(), work.Length() );
}

/*
=================
budFile::VPrintf
=================
*/
int budFile::VPrintf( const char* fmt, va_list args )
{
	char buf[MAX_PRINT_MSG];
	int length;
	
	length = budStr::vsnPrintf( buf, MAX_PRINT_MSG - 1, fmt, args );
	return Write( buf, length );
}

/*
=================
budFile::WriteFloatString
=================
*/
int budFile::WriteFloatString( const char* fmt, ... )
{
	char buf[MAX_PRINT_MSG];
	int len;
	va_list argPtr;
	
	va_start( argPtr, fmt );
	len = FS_WriteFloatString( buf, fmt, argPtr );
	va_end( argPtr );
	
	return Write( buf, len );
}

/*
 =================
 budFile::ReadInt
 =================
 */
int budFile::ReadInt( int& value )
{
	int result = Read( &value, sizeof( value ) );
	value = LittleLong( value );
	return result;
}

/*
 =================
 budFile::ReadUnsignedInt
 =================
 */
int budFile::ReadUnsignedInt( unsigned int& value )
{
	int result = Read( &value, sizeof( value ) );
	value = LittleLong( value );
	return result;
}

/*
 =================
 budFile::ReadShort
 =================
 */
int budFile::ReadShort( short& value )
{
	int result = Read( &value, sizeof( value ) );
	value = LittleShort( value );
	return result;
}

/*
 =================
 budFile::ReadUnsignedShort
 =================
 */
int budFile::ReadUnsignedShort( unsigned short& value )
{
	int result = Read( &value, sizeof( value ) );
	value = LittleShort( value );
	return result;
}

/*
 =================
 budFile::ReadChar
 =================
 */
int budFile::ReadChar( char& value )
{
	return Read( &value, sizeof( value ) );
}

/*
 =================
 budFile::ReadUnsignedChar
 =================
 */
int budFile::ReadUnsignedChar( unsigned char& value )
{
	return Read( &value, sizeof( value ) );
}

/*
 =================
 budFile::ReadFloat
 =================
 */
int budFile::ReadFloat( float& value )
{
	int result = Read( &value, sizeof( value ) );
	value = LittleFloat( value );
	return result;
}

/*
 =================
 budFile::ReadBool
 =================
 */
int budFile::ReadBool( bool& value )
{
	unsigned char c;
	int result = ReadUnsignedChar( c );
	value = c ? true : false;
	return result;
}

/*
 =================
 budFile::ReadString
 =================
 */
int budFile::ReadString( budStr& string )
{
	int len;
	int result = 0;
	
	ReadInt( len );
	if( len >= 0 )
	{
		string.Fill( ' ', len );
		result = Read( &string[ 0 ], len );
	}
	return result;
}

/*
 =================
 budFile::ReadVec2
 =================
 */
int budFile::ReadVec2( budVec2& vec )
{
	int result = Read( &vec, sizeof( vec ) );
	LittleRevBytes( &vec, sizeof( float ), sizeof( vec ) / sizeof( float ) );
	return result;
}

/*
 =================
 budFile::ReadVec3
 =================
 */
int budFile::ReadVec3( budVec3& vec )
{
	int result = Read( &vec, sizeof( vec ) );
	LittleRevBytes( &vec, sizeof( float ), sizeof( vec ) / sizeof( float ) );
	return result;
}

/*
 =================
 budFile::ReadVec4
 =================
 */
int budFile::ReadVec4( budVec4& vec )
{
	int result = Read( &vec, sizeof( vec ) );
	LittleRevBytes( &vec, sizeof( float ), sizeof( vec ) / sizeof( float ) );
	return result;
}

/*
 =================
 budFile::ReadVec6
 =================
 */
int budFile::ReadVec6( budVec6& vec )
{
	int result = Read( &vec, sizeof( vec ) );
	LittleRevBytes( &vec, sizeof( float ), sizeof( vec ) / sizeof( float ) );
	return result;
}

/*
 =================
 budFile::ReadMat3
 =================
 */
int budFile::ReadMat3( budMat3& mat )
{
	int result = Read( &mat, sizeof( mat ) );
	LittleRevBytes( &mat, sizeof( float ), sizeof( mat ) / sizeof( float ) );
	return result;
}

/*
 =================
 budFile::WriteInt
 =================
 */
int budFile::WriteInt( const int value )
{
	int v = LittleLong( value );
	return Write( &v, sizeof( v ) );
}

/*
 =================
 budFile::WriteUnsignedInt
 =================
 */
int budFile::WriteUnsignedInt( const unsigned int value )
{
	unsigned int v = LittleLong( value );
	return Write( &v, sizeof( v ) );
}

/*
 =================
 budFile::WriteShort
 =================
 */
int budFile::WriteShort( const short value )
{
	short v = LittleShort( value );
	return Write( &v, sizeof( v ) );
}

/*
 =================
 budFile::WriteUnsignedShort
 =================
 */
int budFile::WriteUnsignedShort( const unsigned short value )
{
	unsigned short v = LittleShort( value );
	return Write( &v, sizeof( v ) );
}

/*
 =================
 budFile::WriteChar
 =================
 */
int budFile::WriteChar( const char value )
{
	return Write( &value, sizeof( value ) );
}

/*
 =================
 budFile::WriteUnsignedChar
 =================
 */
int budFile::WriteUnsignedChar( const unsigned char value )
{
	return Write( &value, sizeof( value ) );
}

/*
 =================
 budFile::WriteFloat
 =================
 */
int budFile::WriteFloat( const float value )
{
	float v = LittleFloat( value );
	return Write( &v, sizeof( v ) );
}

/*
 =================
 budFile::WriteBool
 =================
 */
int budFile::WriteBool( const bool value )
{
	unsigned char c = value;
	return WriteUnsignedChar( c );
}

/*
 =================
 budFile::WriteString
 =================
 */
int budFile::WriteString( const char* value )
{
	int len = strlen( value );
	WriteInt( len );
	return Write( value, len );
}

/*
 =================
 budFile::WriteVec2
 =================
 */
int budFile::WriteVec2( const budVec2& vec )
{
	budVec2 v = vec;
	LittleRevBytes( &v, sizeof( float ), sizeof( v ) / sizeof( float ) );
	return Write( &v, sizeof( v ) );
}

/*
 =================
 budFile::WriteVec3
 =================
 */
int budFile::WriteVec3( const budVec3& vec )
{
	budVec3 v = vec;
	LittleRevBytes( &v, sizeof( float ), sizeof( v ) / sizeof( float ) );
	return Write( &v, sizeof( v ) );
}

/*
 =================
 budFile::WriteVec4
 =================
 */
int budFile::WriteVec4( const budVec4& vec )
{
	budVec4 v = vec;
	LittleRevBytes( &v, sizeof( float ), sizeof( v ) / sizeof( float ) );
	return Write( &v, sizeof( v ) );
}

/*
 =================
 budFile::WriteVec6
 =================
 */
int budFile::WriteVec6( const budVec6& vec )
{
	budVec6 v = vec;
	LittleRevBytes( &v, sizeof( float ), sizeof( v ) / sizeof( float ) );
	return Write( &v, sizeof( v ) );
}

/*
 =================
 budFile::WriteMat3
 =================
 */
int budFile::WriteMat3( const budMat3& mat )
{
	budMat3 v = mat;
	LittleRevBytes( &v, sizeof( float ), sizeof( v ) / sizeof( float ) );
	return Write( &v, sizeof( v ) );
}

/*
=================================================================================

budFile_Memory

=================================================================================
*/


/*
=================
budFile_Memory::budFile_Memory
=================
*/
budFile_Memory::budFile_Memory()
{
	name = "*unknown*";
	maxSize = 0;
	fileSize = 0;
	allocated = 0;
	granularity = 16384;
	
	mode = ( 1 << FS_WRITE );
	filePtr = NULL;
	curPtr = NULL;
}

/*
=================
budFile_Memory::budFile_Memory
=================
*/
budFile_Memory::budFile_Memory( const char* name )
{
	this->name = name;
	maxSize = 0;
	fileSize = 0;
	allocated = 0;
	granularity = 16384;
	
	mode = ( 1 << FS_WRITE );
	filePtr = NULL;
	curPtr = NULL;
}

/*
=================
budFile_Memory::budFile_Memory
=================
*/
budFile_Memory::budFile_Memory( const char* name, char* data, int length )
{
	this->name = name;
	maxSize = length;
	fileSize = 0;
	allocated = length;
	granularity = 16384;
	
	mode = ( 1 << FS_WRITE );
	filePtr = data;
	curPtr = data;
}

/*
=================
budFile_Memory::budFile_Memory
=================
*/
budFile_Memory::budFile_Memory( const char* name, const char* data, int length )
{
	this->name = name;
	maxSize = 0;
	fileSize = length;
	allocated = 0;
	granularity = 16384;
	
	mode = ( 1 << FS_READ );
	filePtr = const_cast<char*>( data );
	curPtr = const_cast<char*>( data );
}

/*
=================
budFile_Memory::TakeDataOwnership

this also makes the file read only
=================
*/
void budFile_Memory::TakeDataOwnership()
{
	if( filePtr != NULL && fileSize > 0 )
	{
		maxSize = 0;
		mode = ( 1 << FS_READ );
		allocated = fileSize;
	}
}

/*
=================
budFile_Memory::~budFile_Memory
=================
*/
budFile_Memory::~budFile_Memory()
{
	if( filePtr && allocated > 0 && maxSize == 0 )
	{
		Mem_Free( filePtr );
	}
}

/*
=================
budFile_Memory::Read
=================
*/
int budFile_Memory::Read( void* buffer, int len )
{

	if( !( mode & ( 1 << FS_READ ) ) )
	{
		common->FatalError( "budFile_Memory::Read: %s not opened in read mode", name.c_str() );
		return 0;
	}
	
	if( curPtr + len > filePtr + fileSize )
	{
		len = filePtr + fileSize - curPtr;
	}
	memcpy( buffer, curPtr, len );
	curPtr += len;
	return len;
}

budCVar memcpyImpl( "memcpyImpl", "0", 0, "Which implementation of memcpy to use for budFile_Memory::Write() [0/1 - standard (1 eliminates branch misprediction), 2 - auto-vectorized]" );
void* memcpy2( void* __restrict b, const void* __restrict a, size_t n )
{
	char* s1 = ( char* )b;
	const char* s2 = ( const char* )a;
	for( ; 0 < n; --n )
	{
		*s1++ = *s2++;
	}
	return b;
}

/*
=================
budFile_Memory::Write
=================
*/
budHashTableT< int, int > histogram;
CONSOLE_COMMAND( outputHistogram, "", 0 )
{
	for( int i = 0; i < histogram.Num(); i++ )
	{
		int key;
		histogram.GetIndexKey( i, key );
		int* value = histogram.GetIndex( i );
		
		libBud::Printf( "%d\t%d\n", key, *value );
	}
}

CONSOLE_COMMAND( clearHistogram, "", 0 )
{
	histogram.Clear();
}

int budFile_Memory::Write( const void* buffer, int len )
{
	if( len == 0 )
	{
		// ~4% falls into this case for some reason...
		return 0;
	}
	
	if( !( mode & ( 1 << FS_WRITE ) ) )
	{
		common->FatalError( "budFile_Memory::Write: %s not opened in write mode", name.c_str() );
		return 0;
	}
	
	int alloc = curPtr + len + 1 - filePtr - allocated; // need room for len+1
	if( alloc > 0 )
	{
		if( maxSize != 0 )
		{
			common->Error( "budFile_Memory::Write: exceeded maximum size %" PRIuSIZE "", maxSize );
			return 0;
		}
		int extra = granularity * ( 1 + alloc / granularity );
		char* newPtr = ( char* ) Mem_Alloc( allocated + extra, TAG_budFile );
		if( allocated )
		{
			memcpy( newPtr, filePtr, allocated );
		}
		allocated += extra;
		curPtr = newPtr + ( curPtr - filePtr );
		if( filePtr )
		{
			Mem_Free( filePtr );
		}
		filePtr = newPtr;
	}
	
	//memcpy( curPtr, buffer, len );
	memcpy2( curPtr, buffer, len );
	
#if 0
	if( memcpyImpl.GetInteger() == 0 )
	{
		memcpy( curPtr, buffer, len );
	}
	else if( memcpyImpl.GetInteger() == 1 )
	{
		memcpy( curPtr, buffer, len );
	}
	else if( memcpyImpl.GetInteger() == 2 )
	{
		memcpy2( curPtr, buffer, len );
	}
#endif
	
#if 0
	int* value;
	if( histogram.Get( len, &value ) && value != NULL )
	{
		( *value )++;
	}
	else
	{
		histogram.Set( len, 1 );
	}
#endif
	
	curPtr += len;
	fileSize += len;
	filePtr[ fileSize ] = 0; // len + 1
	return len;
}

/*
=================
budFile_Memory::Length
=================
*/
int budFile_Memory::Length() const
{
	return fileSize;
}

/*
========================
budFile_Memory::SetLength
========================
*/
void budFile_Memory::SetLength( size_t len )
{
	PreAllocate( len );
	fileSize = len;
}

/*
========================
budFile_Memory::PreAllocate
========================
*/
void budFile_Memory::PreAllocate( size_t len )
{
	if( len > allocated )
	{
		if( maxSize != 0 )
		{
			libBud::Error( "budFile_Memory::SetLength: exceeded maximum size %" PRIuSIZE "", maxSize );
		}
		char* newPtr = ( char* )Mem_Alloc( len, TAG_budFile );
		if( allocated > 0 )
		{
			memcpy( newPtr, filePtr, allocated );
		}
		allocated = len;
		curPtr = newPtr + ( curPtr - filePtr );
		if( filePtr != NULL )
		{
			Mem_Free( filePtr );
		}
		filePtr = newPtr;
	}
}

/*
=================
budFile_Memory::Timestamp
=================
*/
ID_TIME_T budFile_Memory::Timestamp() const
{
	return 0;
}

/*
=================
budFile_Memory::Tell
=================
*/
int budFile_Memory::Tell() const
{
	return ( curPtr - filePtr );
}

/*
=================
budFile_Memory::ForceFlush
=================
*/
void budFile_Memory::ForceFlush()
{
}

/*
=================
budFile_Memory::Flush
=================
*/
void budFile_Memory::Flush()
{
}

/*
=================
budFile_Memory::Seek

  returns zero on success and -1 on failure
=================
*/
int budFile_Memory::Seek( long offset, fsOrigin_t origin )
{

	switch( origin )
	{
		case FS_SEEK_CUR:
		{
			curPtr += offset;
			break;
		}
		case FS_SEEK_END:
		{
			curPtr = filePtr + fileSize - offset;
			break;
		}
		case FS_SEEK_SET:
		{
			curPtr = filePtr + offset;
			break;
		}
		default:
		{
			common->FatalError( "budFile_Memory::Seek: bad origin for %s\n", name.c_str() );
			return -1;
		}
	}
	if( curPtr < filePtr )
	{
		curPtr = filePtr;
		return -1;
	}
	if( curPtr > filePtr + fileSize )
	{
		curPtr = filePtr + fileSize;
		return -1;
	}
	return 0;
}

/*
========================
budFile_Memory::SetMaxLength
========================
*/
void budFile_Memory::SetMaxLength( size_t len )
{
	size_t oldLength = fileSize;
	
	SetLength( len );
	
	maxSize = len;
	fileSize = oldLength;
}

/*
=================
budFile_Memory::MakeReadOnly
=================
*/
void budFile_Memory::MakeReadOnly()
{
	mode = ( 1 << FS_READ );
	Rewind();
}

/*
========================
budFile_Memory::MakeWritable
========================
*/
void budFile_Memory::MakeWritable()
{
	mode = ( 1 << FS_WRITE );
	Rewind();
}

/*
=================
budFile_Memory::Clear
=================
*/
void budFile_Memory::Clear( bool freeMemory )
{
	fileSize = 0;
	granularity = 16384;
	if( freeMemory )
	{
		allocated = 0;
		Mem_Free( filePtr );
		filePtr = NULL;
		curPtr = NULL;
	}
	else
	{
		curPtr = filePtr;
	}
}

/*
=================
budFile_Memory::SetData
=================
*/
void budFile_Memory::SetData( const char* data, int length )
{
	maxSize = 0;
	fileSize = length;
	allocated = 0;
	granularity = 16384;
	
	mode = ( 1 << FS_READ );
	filePtr = const_cast<char*>( data );
	curPtr = const_cast<char*>( data );
}

/*
========================
budFile_Memory::TruncateData
========================
*/
void budFile_Memory::TruncateData( size_t len )
{
	if( len > allocated )
	{
		libBud::Error( "budFile_Memory::TruncateData: len (%" PRIuSIZE ") exceeded allocated size (%" PRIuSIZE ")", len, allocated );
	}
	else
	{
		fileSize = len;
	}
}

/*
=================================================================================

budFile_BitMsg

=================================================================================
*/

/*
=================
budFile_BitMsg::budFile_BitMsg
=================
*/
budFile_BitMsg::budFile_BitMsg( budBitMsg& msg )
{
	name = "*unknown*";
	mode = ( 1 << FS_WRITE );
	this->msg = &msg;
}

/*
=================
budFile_BitMsg::budFile_BitMsg
=================
*/
budFile_BitMsg::budFile_BitMsg( const budBitMsg& msg )
{
	name = "*unknown*";
	mode = ( 1 << FS_READ );
	this->msg = const_cast<budBitMsg*>( &msg );
}

/*
=================
budFile_BitMsg::~budFile_BitMsg
=================
*/
budFile_BitMsg::~budFile_BitMsg()
{
}

/*
=================
budFile_BitMsg::Read
=================
*/
int budFile_BitMsg::Read( void* buffer, int len )
{

	if( !( mode & ( 1 << FS_READ ) ) )
	{
		common->FatalError( "budFile_BitMsg::Read: %s not opened in read mode", name.c_str() );
		return 0;
	}
	
	return msg->ReadData( buffer, len );
}

/*
=================
budFile_BitMsg::Write
=================
*/
int budFile_BitMsg::Write( const void* buffer, int len )
{

	if( !( mode & ( 1 << FS_WRITE ) ) )
	{
		common->FatalError( "budFile_Memory::Write: %s not opened in write mode", name.c_str() );
		return 0;
	}
	
	msg->WriteData( buffer, len );
	return len;
}

/*
=================
budFile_BitMsg::Length
=================
*/
int budFile_BitMsg::Length() const
{
	return msg->GetSize();
}

/*
=================
budFile_BitMsg::Timestamp
=================
*/
ID_TIME_T budFile_BitMsg::Timestamp() const
{
	return 0;
}

/*
=================
budFile_BitMsg::Tell
=================
*/
int budFile_BitMsg::Tell() const
{
	if( mode == FS_READ )
	{
		return msg->GetReadCount();
	}
	else
	{
		return msg->GetSize();
	}
}

/*
=================
budFile_BitMsg::ForceFlush
=================
*/
void budFile_BitMsg::ForceFlush()
{
}

/*
=================
budFile_BitMsg::Flush
=================
*/
void budFile_BitMsg::Flush()
{
}

/*
=================
budFile_BitMsg::Seek

  returns zero on success and -1 on failure
=================
*/
int budFile_BitMsg::Seek( long offset, fsOrigin_t origin )
{
	return -1;
}


/*
=================================================================================

budFile_Permanent

=================================================================================
*/

/*
=================
budFile_Permanent::budFile_Permanent
=================
*/
budFile_Permanent::budFile_Permanent()
{
	name = "invalid";
	o = NULL;
	mode = 0;
	fileSize = 0;
	handleSync = false;
}

/*
=================
budFile_Permanent::~budFile_Permanent
=================
*/
budFile_Permanent::~budFile_Permanent()
{
	if( o )
	{
		// RB begin
#if defined(_WIN32)
		CloseHandle( o );
#else
		fclose( o );
#endif
		// RB end
	}
}

/*
=================
budFile_Permanent::Read

Properly handles partial reads
=================
*/
int budFile_Permanent::Read( void* buffer, int len )
{
	int		block, remaining;
	int		read;
	byte* 	buf;
	int		tries;
	
	if( !( mode & ( 1 << FS_READ ) ) )
	{
		common->FatalError( "budFile_Permanent::Read: %s not opened in read mode", name.c_str() );
		return 0;
	}
	
	if( !o )
	{
		return 0;
	}
	
	buf = ( byte* )buffer;
	
	remaining = len;
	tries = 0;
	while( remaining )
	{
		block = remaining;
		
		// RB begin
#if defined(_WIN32)
		DWORD bytesRead;
		if( !ReadFile( o, buf, block, &bytesRead, NULL ) )
		{
			libBud::Warning( "budFile_Permanent::Read failed with %d from %s", GetLastError(), name.c_str() );
		}
		read = bytesRead;
#else
		read = fread( buf, 1, block, o );
#endif
		// RB end
		
		if( read == 0 )
		{
			// we might have been trying to read from a CD, which
			// sometimes returns a 0 read on windows
			if( !tries )
			{
				tries = 1;
			}
			else
			{
				return len - remaining;
			}
		}
		
		if( read == -1 )
		{
			common->FatalError( "budFile_Permanent::Read: -1 bytes read from %s", name.c_str() );
		}
		
		remaining -= read;
		buf += read;
	}
	return len;
}

/*
=================
budFile_Permanent::Write

Properly handles partial writes
=================
*/
int budFile_Permanent::Write( const void* buffer, int len )
{
	int		block, remaining;
	int		written;
	byte* 	buf;
	int		tries;
	
	if( !( mode & ( 1 << FS_WRITE ) ) )
	{
		common->FatalError( "budFile_Permanent::Write: %s not opened in write mode", name.c_str() );
		return 0;
	}
	
	if( !o )
	{
		return 0;
	}
	
	buf = ( byte* )buffer;
	
	remaining = len;
	tries = 0;
	while( remaining )
	{
		block = remaining;
		
		// RB begin
#if defined(_WIN32)
		DWORD bytesWritten;
		WriteFile( o, buf, block, &bytesWritten, NULL );
		written = bytesWritten;
#else
		written = fwrite( buf, 1, block, o );
#endif
		// RB end
		
		if( written == 0 )
		{
			if( !tries )
			{
				tries = 1;
			}
			else
			{
				common->Printf( "budFile_Permanent::Write: 0 bytes written to %s\n", name.c_str() );
				return 0;
			}
		}
		
		if( written == -1 )
		{
			common->Printf( "budFile_Permanent::Write: -1 bytes written to %s\n", name.c_str() );
			return 0;
		}
		
		remaining -= written;
		buf += written;
		fileSize += written;
	}
	if( handleSync )
	{
		Flush();
	}
	return len;
}

/*
=================
budFile_Permanent::ForceFlush
=================
*/
void budFile_Permanent::ForceFlush()
{
	// RB begin
#if defined(_WIN32)
	FlushFileBuffers( o );
#else
	setvbuf( o, NULL, _IONBF, 0 );
#endif
	// RB end
}

/*
=================
budFile_Permanent::Flush
=================
*/
void budFile_Permanent::Flush()
{
	// RB begin
#if defined(_WIN32)
	FlushFileBuffers( o );
#else
	fflush( o );
#endif
	// RB end
}

/*
=================
budFile_Permanent::Tell
=================
*/
int budFile_Permanent::Tell() const
{
	// RB begin
#if defined(_WIN32)
	return SetFilePointer( o, 0, NULL, FILE_CURRENT );
#else
	return ftell( o );
#endif
	// RB end
}

/*
================
budFile_Permanent::Length
================
*/
int budFile_Permanent::Length() const
{
	return fileSize;
}

/*
================
budFile_Permanent::Timestamp
================
*/
ID_TIME_T budFile_Permanent::Timestamp() const
{
	ID_TIME_T ts = Sys_FileTimeStamp( o );
	return ts;
}

/*
=================
budFile_Permanent::Seek

  returns zero on success and -1 on failure
=================
*/
int budFile_Permanent::Seek( long offset, fsOrigin_t origin )
{
	// RB begin
#if defined(_WIN32)
	int retVal = INVALID_SET_FILE_POINTER;
	switch( origin )
	{
		case FS_SEEK_CUR:
			retVal = SetFilePointer( o, offset, NULL, FILE_CURRENT );
			break;
		case FS_SEEK_END:
			retVal = SetFilePointer( o, offset, NULL, FILE_END );
			break;
		case FS_SEEK_SET:
			retVal = SetFilePointer( o, offset, NULL, FILE_BEGIN );
			break;
	}
	return ( retVal == INVALID_SET_FILE_POINTER ) ? -1 : 0;
#else
	int _origin;
	
	switch( origin )
	{
		case FS_SEEK_CUR:
		{
			_origin = SEEK_CUR;
			break;
		}
		case FS_SEEK_END:
		{
			_origin = SEEK_END;
			break;
		}
		case FS_SEEK_SET:
		{
			_origin = SEEK_SET;
			break;
		}
		default:
		{
			_origin = SEEK_CUR;
			common->FatalError( "budFile_Permanent::Seek: bad origin for %s\n", name.c_str() );
			break;
		}
	}
	
	return fseek( o, offset, _origin );
#endif
	// RB end
}

#if 1
/*
=================================================================================

budFile_Cached

=================================================================================
*/

/*
=================
budFile_Cached::budFile_Cached
=================
*/
budFile_Cached::budFile_Cached() : budFile_Permanent()
{
	internalFilePos = 0;
	bufferedStartOffset = 0;
	bufferedEndOffset = 0;
	buffered = NULL;
}

/*
=================
budFile_Cached::~budFile_Cached
=================
*/
budFile_Cached::~budFile_Cached()
{
	Mem_Free( buffered );
}

/*
=================
budFile_ReadBuffered::BufferData

Buffer a section of the file
=================
*/
void budFile_Cached::CacheData( uint64 offset, uint64 length )
{
	Mem_Free( buffered );
	bufferedStartOffset = offset;
	bufferedEndOffset = offset + length;
	buffered = ( byte* )Mem_Alloc( length, TAG_RESOURCE );
	if( buffered == NULL )
	{
		return;
	}
	int internalFilePos = budFile_Permanent::Tell();
	budFile_Permanent::Seek( offset, FS_SEEK_SET );
	budFile_Permanent::Read( buffered, length );
	budFile_Permanent::Seek( internalFilePos, FS_SEEK_SET );
}

/*
=================
budFile_ReadBuffered::Read

=================
*/
int budFile_Cached::Read( void* buffer, int len )
{
	if( internalFilePos >= bufferedStartOffset && internalFilePos + len < bufferedEndOffset )
	{
		// this is in the buffer
		memcpy( buffer, ( void* )&buffered[ internalFilePos - bufferedStartOffset ], len );
		internalFilePos += len;
		return len;
	}
	int read = budFile_Permanent::Read( buffer, len );
	if( read != -1 )
	{
		internalFilePos += ( int64 )read;
	}
	return read;
}



/*
=================
budFile_Cached::Tell
=================
*/
int budFile_Cached::Tell() const
{
	return internalFilePos;
}

/*
=================
budFile_Cached::Seek

  returns zero on success and -1 on failure
=================
*/
int budFile_Cached::Seek( long offset, fsOrigin_t origin )
{
	if( origin == FS_SEEK_SET && offset >= bufferedStartOffset && offset < bufferedEndOffset )
	{
		// don't do anything to the actual file ptr, just update or internal position
		internalFilePos = offset;
		return 0;
	}
	
	int retVal = budFile_Permanent::Seek( offset, origin );
	internalFilePos = budFile_Permanent::Tell();
	return retVal;
}
#endif

/*
=================================================================================

budFile_InZip

=================================================================================
*/

/*
=================
budFile_InZip::budFile_InZip
=================
*/
budFile_InZip::budFile_InZip()
{
	name = "invalid";
	zipFilePos = 0;
	fileSize = 0;
	memset( &z, 0, sizeof( z ) );
}

/*
=================
budFile_InZip::~budFile_InZip
=================
*/
budFile_InZip::~budFile_InZip()
{
	unzCloseCurrentFile( z );
	unzClose( z );
}

/*
=================
budFile_InZip::Read

Properly handles partial reads
=================
*/
int budFile_InZip::Read( void* buffer, int len )
{
	int l = unzReadCurrentFile( z, buffer, len );
	return l;
}

/*
=================
budFile_InZip::Write
=================
*/
int budFile_InZip::Write( const void* buffer, int len )
{
	common->FatalError( "budFile_InZip::Write: cannot write to the zipped file %s", name.c_str() );
	return 0;
}

/*
=================
budFile_InZip::ForceFlush
=================
*/
void budFile_InZip::ForceFlush()
{
	common->FatalError( "budFile_InZip::ForceFlush: cannot flush the zipped file %s", name.c_str() );
}

/*
=================
budFile_InZip::Flush
=================
*/
void budFile_InZip::Flush()
{
	common->FatalError( "budFile_InZip::Flush: cannot flush the zipped file %s", name.c_str() );
}

/*
=================
budFile_InZip::Tell
=================
*/
int budFile_InZip::Tell() const
{
	// DG: make sure the value fits into an int
	// it's a long after all, and there'S also unztell64 that returns ZPOS64_T
	// OTOH budFile in general seems to assume file-length <= INT_MAX so it may be ok..
	z_off_t ret = unztell( z );
	assert( ret <= INT_MAX );
	return ret;
	// DG end
}

/*
================
budFile_InZip::Length
================
*/
int budFile_InZip::Length() const
{
	return fileSize;
}

/*
================
budFile_InZip::Timestamp
================
*/
ID_TIME_T budFile_InZip::Timestamp() const
{
	return 0;
}

/*
=================
budFile_InZip::Seek

  returns zero on success and -1 on failure
=================
*/
#define ZIP_SEEK_BUF_SIZE	(1<<15)

int budFile_InZip::Seek( long offset, fsOrigin_t origin )
{
	int res, i;
	char* buf;
	
	switch( origin )
	{
		case FS_SEEK_END:
		{
			offset = fileSize - offset;
		}
		// FALLTHROUGH
		case FS_SEEK_SET:
		{
			// set the file position in the zip file (also sets the current file info)
			// DG use standard unzip.h function instead of custom one (not needed anymore with minizip 1.1)
			unzSetOffset64( z, zipFilePos );
			unzOpenCurrentFile( z );
			if( offset <= 0 )
			{
				return 0;
			}
		}
		// FALLTHROUGH
		case FS_SEEK_CUR:
		{
			buf = ( char* ) _alloca16( ZIP_SEEK_BUF_SIZE );
			for( i = 0; i < ( offset - ZIP_SEEK_BUF_SIZE ); i += ZIP_SEEK_BUF_SIZE )
			{
				res = unzReadCurrentFile( z, buf, ZIP_SEEK_BUF_SIZE );
				if( res < ZIP_SEEK_BUF_SIZE )
				{
					return -1;
				}
			}
			res = i + unzReadCurrentFile( z, buf, offset - i );
			return ( res == offset ) ? 0 : -1;
		}
		default:
		{
			common->FatalError( "budFile_InZip::Seek: bad origin for %s\n", name.c_str() );
			break;
		}
	}
	return -1;
}

#if 1

/*
=================================================================================

budFile_InnerResource

=================================================================================
*/

/*
=================
budFile_InnerResource::budFile_InnerResource
=================
*/
budFile_InnerResource::budFile_InnerResource( const char* _name, budFile* rezFile, int _offset, int _len )
{
	name = _name;
	offset = _offset;
	length = _len;
	resourceFile = rezFile;
	internalFilePos = 0;
	resourceBuffer = NULL;
}

/*
=================
budFile_InnerResource::~budFile_InnerResource
=================
*/
budFile_InnerResource::~budFile_InnerResource()
{
	if( resourceBuffer != NULL )
	{
		fileSystem->FreeResourceBuffer();
	}
}

/*
=================
budFile_InnerResource::Read

Properly handles partial reads
=================
*/
int budFile_InnerResource::Read( void* buffer, int len )
{
	if( resourceFile == NULL )
	{
		return 0;
	}
	
	if( internalFilePos + len > length )
	{
		len = length - internalFilePos;
	}
	
	int read = 0; //fileSystem->ReadFromBGL( resourceFile, (byte*)buffer, offset + internalFilePos, len );
	
	if( read != len )
	{
		if( resourceBuffer != NULL )
		{
			memcpy( buffer, &resourceBuffer[ internalFilePos ], len );
			read = len;
		}
		else
		{
			read = fileSystem->ReadFromBGL( resourceFile, buffer, offset + internalFilePos, len );
		}
	}
	
	internalFilePos += read;
	
	return read;
}

/*
=================
budFile_InnerResource::Tell
=================
*/
int budFile_InnerResource::Tell() const
{
	return internalFilePos;
}


/*
=================
budFile_InnerResource::Seek

  returns zero on success and -1 on failure
=================
*/

int budFile_InnerResource::Seek( long offset, fsOrigin_t origin )
{
	switch( origin )
	{
		case FS_SEEK_END:
		{
			internalFilePos = length - offset - 1;
			return 0;
		}
		case FS_SEEK_SET:
		{
			internalFilePos = offset;
			if( internalFilePos >= 0 && internalFilePos < length )
			{
				return 0;
			}
			return -1;
		}
		case FS_SEEK_CUR:
		{
			internalFilePos += offset;
			if( internalFilePos >= 0 && internalFilePos < length )
			{
				return 0;
			}
			return -1;
		}
		default:
		{
			common->FatalError( "budFile_InnerResource::Seek: bad origin for %s\n", name.c_str() );
			break;
		}
	}
	return -1;
}
#endif

/*
================================================================================================

budFileLocal

================================================================================================
*/

/*
========================
budFileLocal::~budFileLocal

Destructor that will destroy (close) the managed file when this wrapper class goes out of scope.
========================
*/
budFileLocal::~budFileLocal()
{
	if( file != NULL )
	{
		delete file;
		file = NULL;
	}
}

static const char* testEndianNessFilename = "temp.bin";
struct testEndianNess_t
{
	testEndianNess_t()
	{
		a = 0x12345678;
		b = 0x12345678;
		c = 3.0f;
		d = -4.0f;
		e = "test";
		f = budVec3( 1.0f, 2.0f, -3.0f );
		g = false;
		h = true;
		for( int index = 0; index < sizeof( i ); index++ )
		{
			i[index] = 0x37;
		}
	}
	bool operator==( testEndianNess_t& test ) const
	{
		return a == test.a &&
			   b == test.b &&
			   c == test.c &&
			   d == test.d &&
			   e == test.e &&
			   f == test.f &&
			   g == test.g &&
			   h == test.h &&
			   ( memcmp( i, test.i, sizeof( i ) ) == 0 );
	}
	int				a;
	unsigned int	b;
	float			c;
	float			d;
	budStr			e;
	budVec3			f;
	bool			g;
	bool			h;
	byte			i[10];
};
CONSOLE_COMMAND( testEndianNessWrite, "Tests the read/write compatibility between platforms", 0 )
{
	budFileLocal file( fileSystem->OpenFileWrite( testEndianNessFilename ) );
	if( file == NULL )
	{
		libBud::Printf( "Couldn't open the %s testfile.\n", testEndianNessFilename );
		return;
	}
	
	testEndianNess_t testData;
	
	file->WriteBig( testData.a );
	file->WriteBig( testData.b );
	file->WriteFloat( testData.c );
	file->WriteFloat( testData.d );
	file->WriteString( testData.e );
	file->WriteVec3( testData.f );
	file->WriteBig( testData.g );
	file->WriteBig( testData.h );
	file->Write( testData.i, sizeof( testData.i ) / sizeof( testData.i[0] ) );
}

CONSOLE_COMMAND( testEndianNessRead, "Tests the read/write compatibility between platforms", 0 )
{
	budFileLocal file( fileSystem->OpenFileRead( testEndianNessFilename ) );
	if( file == NULL )
	{
		libBud::Printf( "Couldn't find the %s testfile.\n", testEndianNessFilename );
		return;
	}
	
	testEndianNess_t srcData;
	testEndianNess_t testData;
	
	memset( &testData, 0, sizeof( testData ) );
	
	file->ReadBig( testData.a );
	file->ReadBig( testData.b );
	file->ReadFloat( testData.c );
	file->ReadFloat( testData.d );
	file->ReadString( testData.e );
	file->ReadVec3( testData.f );
	file->ReadBig( testData.g );
	file->ReadBig( testData.h );
	file->Read( testData.i, sizeof( testData.i ) / sizeof( testData.i[0] ) );
	
	assert( srcData == testData );
}

CONSOLE_COMMAND( testEndianNessReset, "Tests the read/write compatibility between platforms", 0 )
{
	fileSystem->RemoveFile( testEndianNessFilename );
}
