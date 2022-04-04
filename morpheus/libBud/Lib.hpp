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

#ifndef __LIB_H__
#define __LIB_H__

#include <stddef.h>

/*
===============================================================================

	libBud contains stateless support classes and concrete types. Some classes
	do have static variables, but such variables are initialized once and
	read-only after initialization (they do not maintain a modifiable state).

	The interface pointers budSys, budCommon, budCVarSystem and budFileSystem
	should be set before using libBud. The pointers stored here should not
	be used by any part of the engine except for libBud.

===============================================================================
*/

class libBud
{
private:
	static bool					mainThreadInitialized;
	static ID_TLS				isMainThread;
	
public:
	static class budSys* 			sys;
	static class budCommon* 		common;
	static class budCVarSystem* 	cvarSystem;
	static class budFileSystem* 	fileSystem;
	static int						frameNumber;
	
	static void					Init();
	static void					ShutDown();
	
	// wrapper to budCommon functions
	static void       			Printf( VERIFY_FORMAT_STRING const char* fmt, ... ) ID_STATIC_ATTRIBUTE_PRINTF( 1, 2 );
	static void       			PrintfIf( const bool test, VERIFY_FORMAT_STRING const char* fmt, ... ) ID_STATIC_ATTRIBUTE_PRINTF( 2, 3 );
	NO_RETURN static void       Error( VERIFY_FORMAT_STRING const char* fmt, ... ) ID_STATIC_ATTRIBUTE_PRINTF( 1, 2 );
	NO_RETURN static void       FatalError( VERIFY_FORMAT_STRING const char* fmt, ... ) ID_STATIC_ATTRIBUTE_PRINTF( 1, 2 );
	static void       			Warning( VERIFY_FORMAT_STRING const char* fmt, ... ) ID_STATIC_ATTRIBUTE_PRINTF( 1, 2 );
	static void       			WarningIf( const bool test, VERIFY_FORMAT_STRING const char* fmt, ... ) ID_STATIC_ATTRIBUTE_PRINTF( 2, 3 );
	
	// the extra check for mainThreadInitialized is necessary for this to be accurate
	// when called by startup code that happens before libBud::Init
	static bool					IsMainThread()
	{
		return ( 0 == mainThreadInitialized ) || ( 1 == isMainThread );
	}
};


/*
===============================================================================

	Types and defines used throughout the engine.

===============================================================================
*/

typedef int						qhandle_t;

class budFile;
class budVec3;
class budVec4;

#ifndef NULL
#define NULL					((void *)0)
#endif

#ifndef BIT
#define BIT( num )				( 1ULL << ( num ) )
#endif

#define	MAX_STRING_CHARS		1024		// max length of a string
#define MAX_PRINT_MSG			16384		// buffer size for our various printf routines

// maximum world size
#define MAX_WORLD_COORD			( 128 * 1024 )
#define MIN_WORLD_COORD			( -128 * 1024 )
#define MAX_WORLD_SIZE			( MAX_WORLD_COORD - MIN_WORLD_COORD )

#define SIZE_KB( x )						( ( (x) + 1023 ) / 1024 )
#define SIZE_MB( x )						( ( ( SIZE_KB( x ) ) + 1023 ) / 1024 )
#define SIZE_GB( x )						( ( ( SIZE_MB( x ) ) + 1023 ) / 1024 )

// basic colors
extern	budVec4 colorBlack;
extern	budVec4 colorWhite;
extern	budVec4 colorRed;
extern	budVec4 colorGreen;
extern	budVec4 colorBlue;
extern	budVec4 colorYellow;
extern	budVec4 colorMagenta;
extern	budVec4 colorCyan;
extern	budVec4 colorOrange;
extern	budVec4 colorPurple;
extern	budVec4 colorPink;
extern	budVec4 colorBrown;
extern	budVec4 colorLtGrey;
extern	budVec4 colorMdGrey;
extern	budVec4 colorDkGrey;

// packs color floats in the range [0,1] into an integer
dword	PackColor( const budVec3& color );
void	UnpackColor( const dword color, budVec3& unpackedColor );
dword	PackColor( const budVec4& color );
void	UnpackColor( const dword color, budVec4& unpackedColor );

// little/big endian conversion
short	BigShort( short l );
short	LittleShort( short l );
int		BigLong( int l );
int		LittleLong( int l );
float	BigFloat( float l );
float	LittleFloat( float l );
void	BigRevBytes( void* bp, int elsize, int elcount );
void	LittleRevBytes( void* bp, int elsize, int elcount );
void	LittleBitField( void* bp, int elsize );
void	Swap_Init();

bool	Swap_IsBigEndian();

// for base64
void	SixtetsForInt( byte* out, int src );
int		IntForSixtets( byte* in );

/*
================================================
idException
================================================
*/
class idException
{
public:
	static const int MAX_ERROR_LEN = 2048;
	
	idException( const char* text = "" )
	{
		strncpy( error, text, MAX_ERROR_LEN );
	}
	
	// this really, really should be a const function, but it's referenced too many places to change right now
	const char* 	GetError()
	{
		return error;
	}
	
protected:
	// if GetError() were correctly const this would be named GetError(), too
	char* 		GetErrorBuffer()
	{
		return error;
	}
	int			GetErrorBufferSize()
	{
		return MAX_ERROR_LEN;
	}
	
private:
	friend class idFatalException;
	static char error[MAX_ERROR_LEN];
};

/*
================================================
idFatalException
================================================
*/
class idFatalException
{
public:
	static const int MAX_ERROR_LEN = 2048;
	
	idFatalException( const char* text = "" )
	{
		strncpy( idException::error, text, MAX_ERROR_LEN );
	}
	
	// this really, really should be a const function, but it's referenced too many places to change right now
	const char* 	GetError()
	{
		return idException::error;
	}
	
protected:
	// if GetError() were correctly const this would be named GetError(), too
	char* 		GetErrorBuffer()
	{
		return idException::error;
	}
	int			GetErrorBufferSize()
	{
		return MAX_ERROR_LEN;
	}
};

/*
================================================
idNetworkLoadException
================================================
*/
class idNetworkLoadException : public idException
{
public:
	idNetworkLoadException( const char* text = "" ) : idException( text ) { }
};

/*
===============================================================================

	libBud headers.

===============================================================================
*/

// memory management and arrays
#include "Heap.hpp"
#include "containers/Sort.hpp"
#include "containers/List.hpp"

// math
#include "math/Simd.hpp"
#include "math/Math.hpp"
#include "math/Random.hpp"
#include "math/Complex.hpp"
#include "math/Vector.hpp"
#include "math/VecX.hpp"
#include "math/VectorI.hpp"
#include "math/Matrix.hpp"
#include "math/MatX.hpp"
#include "math/Angles.hpp"
#include "math/Quat.hpp"
#include "math/Rotation.hpp"
#include "math/Plane.hpp"
#include "math/Pluecker.hpp"
#include "math/Polynomial.hpp"
#include "math/Extrapolate.hpp"
#include "math/Interpolate.hpp"
#include "math/Curve.hpp"
#include "math/Ode.hpp"
#include "math/Lcp.hpp"

// bounding volumes
#include "bv/Sphere.hpp"
#include "bv/Bounds.hpp"
#include "bv/Box.hpp"

// geometry
#include "geometry/RenderMatrix.hpp"
#include "geometry/JointTransform.hpp"
#include "geometry/DrawVert.hpp"
#include "geometry/Winding.hpp"
#include "geometry/Winding2D.hpp"
#include "geometry/Surface.hpp"
#include "geometry/Surface_Patch.hpp"
#include "geometry/Surface_Polytope.hpp"
#include "geometry/Surface_SweptSpline.hpp"
#include "geometry/TraceModel.hpp"

// text manipulation
#include "Str.hpp"
#include "StrStatic.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Base64.hpp"
#include "CmdArgs.hpp"

// containers
#include "containers/Array.hpp"
#include "containers/BTree.hpp"
#include "containers/BinSearch.hpp"
#include "containers/HashIndex.hpp"
#include "containers/HashTable.hpp"
#include "containers/StaticList.hpp"
#include "containers/LinkList.hpp"
#include "containers/Hierarchy.hpp"
#include "containers/Queue.hpp"
#include "containers/Stack.hpp"
#include "containers/StrList.hpp"
#include "containers/StrPool.hpp"
#include "containers/VectorSet.hpp"
#include "containers/PlaneSet.hpp"

// hashing
#include "hashing/CRC32.hpp"
#include "hashing/MD4.hpp"
#include "hashing/MD5.hpp"

// misc
#include "Dict.hpp"
#include "LangDict.hpp"
#include "DataQueue.hpp"
#include "BitMsg.hpp"
#include "MapFile.hpp"
#include "Timer.hpp"
#include "Thread.hpp"
#include "Swap.hpp"
#include "Callback.hpp"
#include "ParallelJobList.hpp"

// CVar and Cmd System
#include "Cmd.hpp"
#include "CmdArgs.hpp"
#include "CVar.hpp"

#include "SoftwareCache.hpp"

#endif	/* !__LIB_H__ */
