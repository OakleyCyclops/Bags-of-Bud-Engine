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
#ifndef __BITMSG_H__
#define __BITMSG_H__

/*
================================================
budBitMsg operates on a sequence of individual bits. It handles byte ordering and
avoids alignment errors. It allows concurrent writing and reading. The data set with Init
is never free-d.
================================================
*/
class budBitMsg
{
public:
	budBitMsg()
	{
		InitWrite( NULL, 0 );
	}
	budBitMsg( byte* data, int length )
	{
		InitWrite( data, length );
	}
	budBitMsg( const byte* data, int length )
	{
		InitRead( data, length );
	}
	
	// both read & write
	void			InitWrite( byte* data, int length );
	
	// read only
	void			InitRead( const byte* data, int length );
	
	// get data for writing
	byte* 			GetWriteData();
	
	// get data for reading
	const byte* 	GetReadData() const;
	
	// get the maximum message size
	int				GetMaxSize() const;
	
	// generate error if not set and message is overflowed
	void			SetAllowOverflow( bool set );
	
	// returns true if the message was overflowed
	bool			IsOverflowed() const;
	
	// size of the message in bytes
	int				GetSize() const;
	
	// set the message size
	void			SetSize( int size );
	
	// get current write bit
	int				GetWriteBit() const;
	
	// set current write bit
	void			SetWriteBit( int bit );
	
	// returns number of bits written
	int				GetNumBitsWritten() const;
	
	// space left in bytes for writing
	int				GetRemainingSpace() const;
	
	// space left in bits for writing
	int				GetRemainingWriteBits() const;
	
	//------------------------
	// Write State
	//------------------------
	
	// save the write state
	void			SaveWriteState( int& s, int& b, uint64& t ) const;
	
	// restore the write state
	void			RestoreWriteState( int s, int b, uint64 t );
	
	//------------------------
	// Reading
	//------------------------
	
	// bytes read so far
	int				GetReadCount() const;
	
	// set the number of bytes and bits read
	void			SetReadCount( int bytes );
	
	// get current read bit
	int				GetReadBit() const;
	
	// set current read bit
	void			SetReadBit( int bit );
	
	// returns number of bits read
	int				GetNumBitsRead() const;
	
	// number of bytes left to read
	int				GetRemainingData() const;
	
	// number of bits left to read
	int				GetRemainingReadBits() const;
	
	// save the read state
	void			SaveReadState( int& c, int& b ) const;
	
	// restore the read state
	void			RestoreReadState( int c, int b );
	
	//------------------------
	// Writing
	//------------------------
	
	// begin writing
	void			BeginWriting();
	
	// write up to the next byte boundary
	void			WriteByteAlign();
	
	// write the specified number of bits
	void			WriteBits( int value, int numBits );
	
	void			WriteBool( bool c );
	void			WriteChar( int8 c );
	void			WriteByte( uint8 c );
	void			WriteShort( int16 c );
	void			WriteUShort( uint16 c );
	void			WriteLong( int32 c );
	void			WriteLongLong( int64 c );
	void			WriteFloat( float f );
	void			WriteFloat( float f, int exponentBits, int mantissaBits );
	void			WriteAngle8( float f );
	void			WriteAngle16( float f );
	void			WriteDir( const budVec3& dir, int numBits );
	void			WriteString( const char* s, int maxLength = -1, bool make7Bit = true );
	void			WriteData( const void* data, int length );
	void			WriteNetadr( const netadr_t adr );
	
	void			WriteUNorm8( float f )
	{
		WriteByte( budMath::Ftob( f * 255.0f ) );
	}
	void			WriteUNorm16( float f )
	{
		WriteUShort( budMath::Ftoi( f * 65535.0f ) );
	}
	void			WriteNorm16( float f )
	{
		WriteShort( budMath::Ftoi( f * 32767.0f ) );
	}
	
	void			WriteDeltaChar( int8 oldValue, int8 newValue )
	{
		WriteByte( newValue - oldValue );
	}
	void			WriteDeltaByte( uint8 oldValue, uint8 newValue )
	{
		WriteByte( newValue - oldValue );
	}
	void			WriteDeltaShort( int16 oldValue, int16 newValue )
	{
		WriteUShort( newValue - oldValue );
	}
	void			WriteDeltaUShort( uint16 oldValue, uint16 newValue )
	{
		WriteUShort( newValue - oldValue );
	}
	void			WriteDeltaLong( int32 oldValue, int32 newValue )
	{
		WriteLong( newValue - oldValue );
	}
	void			WriteDeltaFloat( float oldValue, float newValue )
	{
		WriteFloat( newValue - oldValue );
	}
	void			WriteDeltaFloat( float oldValue, float newValue, int exponentBits, int mantissaBits )
	{
		WriteFloat( newValue - oldValue, exponentBits, mantissaBits );
	}
	
	bool			WriteDeltaDict( const idDict& dict, const idDict* base );
	
	template< int _max_, int _numBits_ >
	void			WriteQuantizedFloat( float value );
	template< int _max_, int _numBits_ >
	void			WriteQuantizedUFloat( float value );		// Quantize a float to a variable number of bits (assumes unsigned, uses simple quantization)
	
	template< typename T >
	void			WriteVectorFloat( const T& v )
	{
		for( int i = 0; i < v.GetDimension(); i++ )
		{
			WriteFloat( v[i] );
		}
	}
	template< typename T >
	void			WriteVectorUNorm8( const T& v )
	{
		for( int i = 0; i < v.GetDimension(); i++ )
		{
			WriteUNorm8( v[i] );
		}
	}
	template< typename T >
	void			WriteVectorUNorm16( const T& v )
	{
		for( int i = 0; i < v.GetDimension(); i++ )
		{
			WriteUNorm16( v[i] );
		}
	}
	template< typename T >
	void			WriteVectorNorm16( const T& v )
	{
		for( int i = 0; i < v.GetDimension(); i++ )
		{
			WriteNorm16( v[i] );
		}
	}
	
	// Compress a vector to a variable number of bits (assumes signed, uses simple quantization)
	template< typename T, int _max_, int _numBits_  >
	void			WriteQuantizedVector( const T& v )
	{
		for( int i = 0; i < v.GetDimension(); i++ )
		{
			WriteQuantizedFloat< _max_, _numBits_ >( v[i] );
		}
	}
	
	// begin reading.
	void			BeginReading() const;
	
	// read up to the next byte boundary
	void			ReadByteAlign() const;
	
	// read the specified number of bits
	int				ReadBits( int numBits ) const;
	
	bool			ReadBool() const;
	int				ReadChar() const;
	int				ReadByte() const;
	int				ReadShort() const;
	int				ReadUShort() const;
	int				ReadLong() const;
	int64			ReadLongLong() const;
	float			ReadFloat() const;
	float			ReadFloat( int exponentBits, int mantissaBits ) const;
	float			ReadAngle8() const;
	float			ReadAngle16() const;
	budVec3			ReadDir( int numBits ) const;
	int				ReadString( char* buffer, int bufferSize ) const;
	int				ReadString( budStr& str ) const;
	int				ReadData( void* data, int length ) const;
	void			ReadNetadr( netadr_t* adr ) const;
	
	float			ReadUNorm8() const
	{
		return ReadByte() / 255.0f;
	}
	float			ReadUNorm16() const
	{
		return ReadUShort() / 65535.0f;
	}
	float			ReadNorm16() const
	{
		return ReadShort() / 32767.0f;
	}
	
	int8			ReadDeltaChar( int8 oldValue ) const
	{
		return oldValue + ReadByte();
	}
	uint8			ReadDeltaByte( uint8 oldValue ) const
	{
		return oldValue + ReadByte();
	}
	int16			ReadDeltaShort( int16 oldValue ) const
	{
		return oldValue + ReadUShort();
	}
	uint16			ReadDeltaUShort( uint16 oldValue ) const
	{
		return oldValue + ReadUShort();
	}
	int32			ReadDeltaLong( int32 oldValue ) const
	{
		return oldValue + ReadLong();
	}
	float			ReadDeltaFloat( float oldValue ) const
	{
		return oldValue + ReadFloat();
	}
	float			ReadDeltaFloat( float oldValue, int exponentBits, int mantissaBits ) const
	{
		return oldValue + ReadFloat( exponentBits, mantissaBits );
	}
	bool			ReadDeltaDict( idDict& dict, const idDict* base ) const;
	
	template< int _max_, int _numBits_ >
	float			ReadQuantizedFloat() const;
	template< int _max_, int _numBits_ >
	float			ReadQuantizedUFloat() const;
	
	template< typename T >
	void			ReadVectorFloat( T& v ) const
	{
		for( int i = 0; i < v.GetDimension(); i++ )
		{
			v[i] = ReadFloat();
		}
	}
	template< typename T >
	void			ReadVectorUNorm8( T& v ) const
	{
		for( int i = 0; i < v.GetDimension(); i++ )
		{
			v[i] = ReadUNorm8();
		}
	}
	template< typename T >
	void			ReadVectorUNorm16( T& v ) const
	{
		for( int i = 0; i < v.GetDimension(); i++ )
		{
			v[i] = ReadUNorm16();
		}
	}
	template< typename T >
	void			ReadVectorNorm16( T& v ) const
	{
		for( int i = 0; i < v.GetDimension(); i++ )
		{
			v[i] = ReadNorm16();
		}
	}
	template< typename T, int _max_, int _numBits_ >
	void			ReadQuantizedVector( T& v ) const
	{
		for( int i = 0; i < v.GetDimension(); i++ )
		{
			v[i] = ReadQuantizedFloat< _max_, _numBits_ >();
		}
	}
	
	static int		DirToBits( const budVec3& dir, int numBits );
	static budVec3	BitsToDir( int bits, int numBits );
	
	void			SetHasChanged( bool b )
	{
		hasChanged = b;
	}
	bool			HasChanged() const
	{
		return hasChanged;
	}
	
private:
	byte* 			writeData;		// pointer to data for writing
	const byte* 	readData;		// pointer to data for reading
	int				maxSize;		// maximum size of message in bytes
	int				curSize;		// current size of message in bytes
	mutable int		writeBit;		// number of bits written to the last written byte
	mutable int		readCount;		// number of bytes read so far
	mutable int		readBit;		// number of bits read from the last read byte
	bool			allowOverflow;	// if false, generate error when the message is overflowed
	bool			overflowed;		// set true if buffer size failed (with allowOverflow set)
	bool			hasChanged;		// Hack
	
	mutable uint64	tempValue;
	
private:
	bool			CheckOverflow( int numBits );
	byte* 			GetByteSpace( int length );
};

/*
========================
budBitMsg::InitWrite
========================
*/
BUD_INLINE void budBitMsg::InitWrite( byte* data, int length )
{
	writeData = data;
	readData = data;
	maxSize = length;
	curSize = 0;
	
	writeBit = 0;
	readCount = 0;
	readBit = 0;
	allowOverflow = false;
	overflowed = false;
	
	tempValue = 0;
}

/*
========================
budBitMsg::InitRead
========================
*/
BUD_INLINE void budBitMsg::InitRead( const byte* data, int length )
{
	writeData = NULL;
	readData = data;
	maxSize = length;
	curSize = length;
	
	writeBit = 0;
	readCount = 0;
	readBit = 0;
	allowOverflow = false;
	overflowed = false;
	
	tempValue = 0;
}

/*
========================
budBitMsg::GetWriteData
========================
*/
BUD_INLINE byte* budBitMsg::GetWriteData()
{
	return writeData;
}

/*
========================
budBitMsg::GetReadData
========================
*/
BUD_INLINE const byte* budBitMsg::GetReadData() const
{
	return readData;
}

/*
========================
budBitMsg::GetMaxSize
========================
*/
BUD_INLINE int budBitMsg::GetMaxSize() const
{
	return maxSize;
}

/*
========================
budBitMsg::SetAllowOverflow
========================
*/
BUD_INLINE void budBitMsg::SetAllowOverflow( bool set )
{
	allowOverflow = set;
}

/*
========================
budBitMsg::IsOverflowed
========================
*/
BUD_INLINE bool budBitMsg::IsOverflowed() const
{
	return overflowed;
}

/*
========================
budBitMsg::GetSize
========================
*/
BUD_INLINE int budBitMsg::GetSize() const
{
	return curSize + ( writeBit != 0 );
}

/*
========================
budBitMsg::SetSize
========================
*/
BUD_INLINE void budBitMsg::SetSize( int size )
{
	assert( writeBit == 0 );
	
	if( size > maxSize )
	{
		curSize = maxSize;
	}
	else
	{
		curSize = size;
	}
}

/*
========================
budBitMsg::GetWriteBit
========================
*/
BUD_INLINE int budBitMsg::GetWriteBit() const
{
	return writeBit;
}

/*
========================
budBitMsg::SetWriteBit
========================
*/
BUD_INLINE void budBitMsg::SetWriteBit( int bit )
{
	// see budBitMsg::WriteByteAlign
	assert( false );
	writeBit = bit & 7;
	if( writeBit )
	{
		writeData[curSize - 1] &= ( 1 << writeBit ) - 1;
	}
}

/*
========================
budBitMsg::GetNumBitsWritten
========================
*/
BUD_INLINE int budBitMsg::GetNumBitsWritten() const
{
	return ( curSize << 3 ) + writeBit;
}

/*
========================
budBitMsg::GetRemainingSpace
========================
*/
BUD_INLINE int budBitMsg::GetRemainingSpace() const
{
	return maxSize - GetSize();
}

/*
========================
budBitMsg::GetRemainingWriteBits
========================
*/
BUD_INLINE int budBitMsg::GetRemainingWriteBits() const
{
	return ( maxSize << 3 ) - GetNumBitsWritten();
}

/*
========================
budBitMsg::SaveWriteState
========================
*/
BUD_INLINE void budBitMsg::SaveWriteState( int& s, int& b, uint64& t ) const
{
	s = curSize;
	b = writeBit;
	t = tempValue;
}

/*
========================
budBitMsg::RestoreWriteState
========================
*/
BUD_INLINE void budBitMsg::RestoreWriteState( int s, int b, uint64 t )
{
	curSize = s;
	writeBit = b & 7;
	if( writeBit )
	{
		writeData[curSize] &= ( 1 << writeBit ) - 1;
	}
	tempValue = t;
}

/*
========================
budBitMsg::GetReadCount
========================
*/
BUD_INLINE int budBitMsg::GetReadCount() const
{
	return readCount;
}

/*
========================
budBitMsg::SetReadCount
========================
*/
BUD_INLINE void budBitMsg::SetReadCount( int bytes )
{
	readCount = bytes;
}

/*
========================
budBitMsg::GetReadBit
========================
*/
BUD_INLINE int budBitMsg::GetReadBit() const
{
	return readBit;
}

/*
========================
budBitMsg::SetReadBit
========================
*/
BUD_INLINE void budBitMsg::SetReadBit( int bit )
{
	readBit = bit & 7;
}

/*
========================
budBitMsg::GetNumBitsRead
========================
*/
BUD_INLINE int budBitMsg::GetNumBitsRead() const
{
	return ( ( readCount << 3 ) - ( ( 8 - readBit ) & 7 ) );
}

/*
========================
budBitMsg::GetRemainingData
========================
*/
BUD_INLINE int budBitMsg::GetRemainingData() const
{
	assert( writeBit == 0 );
	return curSize - readCount;
}

/*
========================
budBitMsg::GetRemainingReadBits
========================
*/
BUD_INLINE int budBitMsg::GetRemainingReadBits() const
{
	assert( writeBit == 0 );
	return ( curSize << 3 ) - GetNumBitsRead();
}

/*
========================
budBitMsg::SaveReadState
========================
*/
BUD_INLINE void budBitMsg::SaveReadState( int& c, int& b ) const
{
	assert( writeBit == 0 );
	c = readCount;
	b = readBit;
}

/*
========================
budBitMsg::RestoreReadState
========================
*/
BUD_INLINE void budBitMsg::RestoreReadState( int c, int b )
{
	assert( writeBit == 0 );
	readCount = c;
	readBit = b & 7;
}

/*
========================
budBitMsg::BeginWriting
========================
*/
BUD_INLINE void budBitMsg::BeginWriting()
{
	curSize = 0;
	overflowed = false;
	writeBit = 0;
	tempValue = 0;
}

/*
========================
budBitMsg::WriteByteAlign
========================
*/
BUD_INLINE void budBitMsg::WriteByteAlign()
{
	// it is important that no uninitialized data slips in the msg stream,
	// because we use memcmp to decide if entities have changed and wether we should transmit them
	// this function has the potential to leave uninitialized bits into the stream,
	// however budBitMsg::WriteBits is properly initializing the byte to 0 so hopefully we are still safe
	// adding this extra check just in case
	curSize += writeBit != 0;
	assert( writeBit == 0 || ( ( writeData[curSize - 1] >> writeBit ) == 0 ) ); // had to early out writeBit == 0 because when writeBit == 0 writeData[curSize - 1] may be the previous byte written and trigger false positives
	writeBit = 0;
	tempValue = 0;
}

/*
========================
budBitMsg::WriteBool
========================
*/
BUD_INLINE void budBitMsg::WriteBool( bool c )
{
	WriteBits( c, 1 );
}

/*
========================
budBitMsg::WriteChar
========================
*/
BUD_INLINE void budBitMsg::WriteChar( int8 c )
{
	WriteBits( c, -8 );
}

/*
========================
budBitMsg::WriteByte
========================
*/
BUD_INLINE void budBitMsg::WriteByte( uint8 c )
{
	WriteBits( c, 8 );
}

/*
========================
budBitMsg::WriteShort
========================
*/
BUD_INLINE void budBitMsg::WriteShort( int16 c )
{
	WriteBits( c, -16 );
}

/*
========================
budBitMsg::WriteUShort
========================
*/
BUD_INLINE void budBitMsg::WriteUShort( uint16 c )
{
	WriteBits( c, 16 );
}

/*
========================
budBitMsg::WriteLong
========================
*/
BUD_INLINE void budBitMsg::WriteLong( int32 c )
{
	WriteBits( c, 32 );
}

/*
========================
budBitMsg::WriteLongLong
========================
*/
BUD_INLINE void budBitMsg::WriteLongLong( int64 c )
{
	int a = c;
	int b = c >> 32;
	WriteBits( a, 32 );
	WriteBits( b, 32 );
}

/*
========================
budBitMsg::WriteFloat
========================
*/
BUD_INLINE void budBitMsg::WriteFloat( float f )
{
	WriteBits( *reinterpret_cast<int*>( &f ), 32 );
}

/*
========================
budBitMsg::WriteFloat
========================
*/
BUD_INLINE void budBitMsg::WriteFloat( float f, int exponentBits, int mantissaBits )
{
	int bits = budMath::FloatToBits( f, exponentBits, mantissaBits );
	WriteBits( bits, 1 + exponentBits + mantissaBits );
}

/*
========================
budBitMsg::WriteAngle8
========================
*/
BUD_INLINE void budBitMsg::WriteAngle8( float f )
{
	WriteByte( ANGLE2BYTE( f ) );
}

/*
========================
budBitMsg::WriteAngle16
========================
*/
BUD_INLINE void budBitMsg::WriteAngle16( float f )
{
	WriteShort( ANGLE2SHORT( f ) );
}

/*
========================
budBitMsg::WriteDir
========================
*/
BUD_INLINE void budBitMsg::WriteDir( const budVec3& dir, int numBits )
{
	WriteBits( DirToBits( dir, numBits ), numBits );
}

/*
========================
budBitMsg::BeginReading
========================
*/
BUD_INLINE void budBitMsg::BeginReading() const
{
	readCount = 0;
	readBit = 0;
	
	writeBit = 0;
	tempValue = 0;
}

/*
========================
budBitMsg::ReadByteAlign
========================
*/
BUD_INLINE void budBitMsg::ReadByteAlign() const
{
	readBit = 0;
}

/*
========================
budBitMsg::ReadBool
========================
*/
BUD_INLINE bool budBitMsg::ReadBool() const
{
	return ( ReadBits( 1 ) == 1 ) ? true : false;
}

/*
========================
budBitMsg::ReadChar
========================
*/
BUD_INLINE int budBitMsg::ReadChar() const
{
	return ( signed char )ReadBits( -8 );
}

/*
========================
budBitMsg::ReadByte
========================
*/
BUD_INLINE int budBitMsg::ReadByte() const
{
	return ( unsigned char )ReadBits( 8 );
}

/*
========================
budBitMsg::ReadShort
========================
*/
BUD_INLINE int budBitMsg::ReadShort() const
{
	return ( short )ReadBits( -16 );
}

/*
========================
budBitMsg::ReadUShort
========================
*/
BUD_INLINE int budBitMsg::ReadUShort() const
{
	return ( unsigned short )ReadBits( 16 );
}

/*
========================
budBitMsg::ReadLong
========================
*/
BUD_INLINE int budBitMsg::ReadLong() const
{
	return ReadBits( 32 );
}

/*
========================
budBitMsg::ReadLongLong
========================
*/
BUD_INLINE int64 budBitMsg::ReadLongLong() const
{
	int64 a = ReadBits( 32 );
	int64 b = ReadBits( 32 );
	int64 c = ( 0x00000000ffffffff & a ) | ( b << 32 );
	return c;
}

/*
========================
budBitMsg::ReadFloat
========================
*/
BUD_INLINE float budBitMsg::ReadFloat() const
{
	float value;
	*reinterpret_cast<int*>( &value ) = ReadBits( 32 );
	return value;
}

/*
========================
budBitMsg::ReadFloat
========================
*/
BUD_INLINE float budBitMsg::ReadFloat( int exponentBits, int mantissaBits ) const
{
	int bits = ReadBits( 1 + exponentBits + mantissaBits );
	return budMath::BitsToFloat( bits, exponentBits, mantissaBits );
}

/*
========================
budBitMsg::ReadAngle8
========================
*/
BUD_INLINE float budBitMsg::ReadAngle8() const
{
	return BYTE2ANGLE( ReadByte() );
}

/*
========================
budBitMsg::ReadAngle16
========================
*/
BUD_INLINE float budBitMsg::ReadAngle16() const
{
	return SHORT2ANGLE( ReadShort() );
}

/*
========================
budBitMsg::ReadDir
========================
*/
BUD_INLINE budVec3 budBitMsg::ReadDir( int numBits ) const
{
	return BitsToDir( ReadBits( numBits ), numBits );
}

/*
========================
budBitMsg::WriteQuantizedFloat
========================
*/
template< int _max_, int _numBits_ >
BUD_INLINE void budBitMsg::WriteQuantizedFloat( float value )
{
	enum { storeMax = ( 1 << ( _numBits_ - 1 ) ) - 1 };
	if( _max_ > storeMax )
	{
		// Scaling down (scale should be < 1)
		const float scale = ( float )storeMax / ( float )_max_;
		WriteBits( budMath::ClampInt( -storeMax, storeMax, budMath::Ftoi( value * scale ) ), -_numBits_ );
	}
	else
	{
		// Scaling up (scale should be >= 1) (Preserve whole numbers when possible)
		enum { scale = storeMax / _max_ };
		WriteBits( budMath::ClampInt( -storeMax, storeMax, budMath::Ftoi( value * scale ) ), -_numBits_ );
	}
}

/*
========================
budBitMsg::WriteQuantizedUFloat
========================
*/
template< int _max_, int _numBits_ >
BUD_INLINE void budBitMsg::WriteQuantizedUFloat( float value )
{
	enum { storeMax = ( 1 << _numBits_ ) - 1 };
	if( _max_ > storeMax )
	{
		// Scaling down (scale should be < 1)
		const float scale = ( float )storeMax / ( float )_max_;
		WriteBits( budMath::ClampInt( 0, storeMax, budMath::Ftoi( value * scale ) ), _numBits_ );
	}
	else
	{
		// Scaling up (scale should be >= 1) (Preserve whole numbers when possible)
		enum { scale = storeMax / _max_ };
		WriteBits( budMath::ClampInt( 0, storeMax, budMath::Ftoi( value * scale ) ), _numBits_ );
	}
}

/*
========================
budBitMsg::ReadQuantizedFloat
========================
*/
template< int _max_, int _numBits_ >
BUD_INLINE float budBitMsg::ReadQuantizedFloat() const
{
	enum { storeMax = ( 1 << ( _numBits_ - 1 ) ) - 1 };
	if( _max_ > storeMax )
	{
		// Scaling down (scale should be < 1)
		const float invScale = ( float )_max_ / ( float )storeMax;
		return ( float )ReadBits( -_numBits_ ) * invScale;
	}
	else
	{
		// Scaling up (scale should be >= 1) (Preserve whole numbers when possible)
		// Scale will be a whole number.
		// We use a float to get rid of (potential divide by zero) which is handled above, but the compiler is dumb
		const float scale = storeMax / _max_;
		const float invScale = 1.0f / scale;
		return ( float )ReadBits( -_numBits_ ) * invScale;
	}
}

/*
========================
budBitMsg::ReadQuantizedUFloat
========================
*/
template< int _max_, int _numBits_ >
float budBitMsg::ReadQuantizedUFloat() const
{
	enum { storeMax = ( 1 << _numBits_ ) - 1 };
	if( _max_ > storeMax )
	{
		// Scaling down (scale should be < 1)
		const float invScale = ( float )_max_ / ( float )storeMax;
		return ( float )ReadBits( _numBits_ ) * invScale;
	}
	else
	{
		// Scaling up (scale should be >= 1) (Preserve whole numbers when possible)
		// Scale will be a whole number.
		// We use a float to get rid of (potential divide by zero) which is handled above, but the compiler is dumb
		const float scale = storeMax / _max_;
		const float invScale = 1.0f / scale;
		return ( float )ReadBits( _numBits_ ) * invScale;
	}
}

/*
================
WriteFloatArray
Writes all the values from the array to the bit message.
================
*/
template< class _arrayType_ >
void WriteFloatArray( budBitMsg& message, const _arrayType_ & sourceArray )
{
	for( int i = 0; i < idTupleSize< _arrayType_ >::value; ++i )
	{
		message.WriteFloat( sourceArray[i] );
	}
}

/*
================
WriteFloatArrayDelta
Writes _num_ values from the array to the bit message.
================
*/
template< class _arrayType_ >
void WriteDeltaFloatArray( budBitMsg& message, const _arrayType_ & oldArray, const _arrayType_ & newArray )
{
	for( int i = 0; i < idTupleSize< _arrayType_ >::value; ++i )
	{
		message.WriteDeltaFloat( oldArray[i], newArray[i] );
	}
}

/*
================
ReadFloatArray
Reads _num_ values from the array to the bit message.
================
*/
template< class _arrayType_ >
_arrayType_ ReadFloatArray( const budBitMsg& message )
{
	_arrayType_ result;
	
	for( int i = 0; i < idTupleSize< _arrayType_ >::value; ++i )
	{
		result[i] = message.ReadFloat();
	}
	
	return result;
}

/*
================
ReadDeltaFloatArray
Reads _num_ values from the array to the bit message.
================
*/
template< class _arrayType_ >
_arrayType_ ReadDeltaFloatArray( const budBitMsg& message, const _arrayType_ & oldArray )
{
	_arrayType_ result;
	
	for( int i = 0; i < idTupleSize< _arrayType_ >::value; ++i )
	{
		result[i] = message.ReadDeltaFloat( oldArray[i] );
	}
	
	return result;
}

#endif /* !__BITMSG_H__ */
