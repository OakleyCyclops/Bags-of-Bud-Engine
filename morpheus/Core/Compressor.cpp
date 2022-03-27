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
=================================================================================

	budCompressor_None

=================================================================================
*/

class budCompressor_None : public budCompressor
{
public:
	budCompressor_None();
	
	void			Init( budFile* f, bool compress, int wordLength );
	void			FinishCompress();
	float			GetCompressionRatio() const;
	
	const char* 	GetName();
	const char* 	GetFullPath();
	int				Read( void* outData, int outLength );
	int				Write( const void* inData, int inLength );
	int				Length();
	ID_TIME_T			Timestamp();
	int				Tell();
	void			ForceFlush();
	void			Flush();
	int				Seek( long offset, fsOrigin_t origin );
	
protected:
	budFile* 		file;
	bool			compress;
};

/*
================
budCompressor_None::budCompressor_None
================
*/
budCompressor_None::budCompressor_None()
{
	file = NULL;
	compress = true;
}

/*
================
budCompressor_None::Init
================
*/
void budCompressor_None::Init( budFile* f, bool compress, int wordLength )
{
	this->file = f;
	this->compress = compress;
}

/*
================
budCompressor_None::FinishCompress
================
*/
void budCompressor_None::FinishCompress()
{
}

/*
================
budCompressor_None::GetCompressionRatio
================
*/
float budCompressor_None::GetCompressionRatio() const
{
	return 0.0f;
}

/*
================
budCompressor_None::GetName
================
*/
const char* budCompressor_None::GetName()
{
	if( file )
	{
		return file->GetName();
	}
	else
	{
		return "";
	}
}

/*
================
budCompressor_None::GetFullPath
================
*/
const char* budCompressor_None::GetFullPath()
{
	if( file )
	{
		return file->GetFullPath();
	}
	else
	{
		return "";
	}
}

/*
================
budCompressor_None::Write
================
*/
int budCompressor_None::Write( const void* inData, int inLength )
{
	if( compress == false || inLength <= 0 )
	{
		return 0;
	}
	return file->Write( inData, inLength );
}

/*
================
budCompressor_None::Read
================
*/
int budCompressor_None::Read( void* outData, int outLength )
{
	if( compress == true || outLength <= 0 )
	{
		return 0;
	}
	return file->Read( outData, outLength );
}

/*
================
budCompressor_None::Length
================
*/
int budCompressor_None::Length()
{
	if( file )
	{
		return file->Length();
	}
	else
	{
		return 0;
	}
}

/*
================
budCompressor_None::Timestamp
================
*/
ID_TIME_T budCompressor_None::Timestamp()
{
	if( file )
	{
		return file->Timestamp();
	}
	else
	{
		return 0;
	}
}

/*
================
budCompressor_None::Tell
================
*/
int budCompressor_None::Tell()
{
	if( file )
	{
		return file->Tell();
	}
	else
	{
		return 0;
	}
}

/*
================
budCompressor_None::ForceFlush
================
*/
void budCompressor_None::ForceFlush()
{
	if( file )
	{
		file->ForceFlush();
	}
}

/*
================
budCompressor_None::Flush
================
*/
void budCompressor_None::Flush()
{
	if( file )
	{
		file->ForceFlush();
	}
}

/*
================
budCompressor_None::Seek
================
*/
int budCompressor_None::Seek( long offset, fsOrigin_t origin )
{
	common->Error( "cannot seek on budCompressor" );
	return -1;
}


/*
=================================================================================

	budCompressor_BitStream

	Base class for bit stream compression.

=================================================================================
*/

class budCompressor_BitStream : public budCompressor_None
{
public:
	budCompressor_BitStream() {}
	
	void			Init( budFile* f, bool compress, int wordLength );
	void			FinishCompress();
	float			GetCompressionRatio() const;
	
	int				Write( const void* inData, int inLength );
	int				Read( void* outData, int outLength );
	
protected:
	byte			buffer[65536];
	int				wordLength;
	
	int				readTotalBytes;
	int				readLength;
	int				readByte;
	int				readBit;
	const byte* 	readData;
	
	int				writeTotalBytes;
	int				writeLength;
	int				writeByte;
	int				writeBit;
	byte* 			writeData;
	
protected:
	void			InitCompress( const void* inData, const int inLength );
	void			InitDecompress( void* outData, int outLength );
	void			WriteBits( int value, int numBits );
	int				ReadBits( int numBits );
	void			UnreadBits( int numBits );
	int				Compare( const byte* src1, int bitPtr1, const byte* src2, int bitPtr2, int maxBits ) const;
};

/*
================
budCompressor_BitStream::Init
================
*/
void budCompressor_BitStream::Init( budFile* f, bool compress, int wordLength )
{

	assert( wordLength >= 1 && wordLength <= 32 );
	
	this->file = f;
	this->compress = compress;
	this->wordLength = wordLength;
	
	readTotalBytes = 0;
	readLength = 0;
	readByte = 0;
	readBit = 0;
	readData = NULL;
	
	writeTotalBytes = 0;
	writeLength = 0;
	writeByte = 0;
	writeBit = 0;
	writeData = NULL;
}

/*
================
budCompressor_BitStream::InitCompress
================
*/
BUD_INLINE void budCompressor_BitStream::InitCompress( const void* inData, const int inLength )
{

	readLength = inLength;
	readByte = 0;
	readBit = 0;
	readData = ( const byte* ) inData;
	
	if( !writeLength )
	{
		writeLength = sizeof( buffer );
		writeByte = 0;
		writeBit = 0;
		writeData = buffer;
	}
}

/*
================
budCompressor_BitStream::InitDecompress
================
*/
BUD_INLINE void budCompressor_BitStream::InitDecompress( void* outData, int outLength )
{

	if( !readLength )
	{
		readLength = file->Read( buffer, sizeof( buffer ) );
		readByte = 0;
		readBit = 0;
		readData = buffer;
	}
	
	writeLength = outLength;
	writeByte = 0;
	writeBit = 0;
	writeData = ( byte* ) outData;
}

/*
================
budCompressor_BitStream::WriteBits
================
*/
void budCompressor_BitStream::WriteBits( int value, int numBits )
{
	int put, fraction;
	
	// Short circuit for writing single bytes at a time
	if( writeBit == 0 && numBits == 8 && writeByte < writeLength )
	{
		writeByte++;
		writeTotalBytes++;
		writeData[writeByte - 1] = value;
		return;
	}
	
	
	while( numBits )
	{
		if( writeBit == 0 )
		{
			if( writeByte >= writeLength )
			{
				if( writeData == buffer )
				{
					file->Write( buffer, writeByte );
					writeByte = 0;
				}
				else
				{
					put = numBits;
					writeBit = put & 7;
					writeByte += ( put >> 3 ) + ( writeBit != 0 );
					writeTotalBytes += ( put >> 3 ) + ( writeBit != 0 );
					return;
				}
			}
			writeData[writeByte] = 0;
			writeByte++;
			writeTotalBytes++;
		}
		put = 8 - writeBit;
		if( put > numBits )
		{
			put = numBits;
		}
		fraction = value & ( ( 1 << put ) - 1 );
		writeData[writeByte - 1] |= fraction << writeBit;
		numBits -= put;
		value >>= put;
		writeBit = ( writeBit + put ) & 7;
	}
}

/*
================
budCompressor_BitStream::ReadBits
================
*/
int budCompressor_BitStream::ReadBits( int numBits )
{
	int value, valueBits, get, fraction;
	
	value = 0;
	valueBits = 0;
	
	// Short circuit for reading single bytes at a time
	if( readBit == 0 && numBits == 8 && readByte < readLength )
	{
		readByte++;
		readTotalBytes++;
		return readData[readByte - 1];
	}
	
	while( valueBits < numBits )
	{
		if( readBit == 0 )
		{
			if( readByte >= readLength )
			{
				if( readData == buffer )
				{
					readLength = file->Read( buffer, sizeof( buffer ) );
					readByte = 0;
				}
				else
				{
					get = numBits - valueBits;
					readBit = get & 7;
					readByte += ( get >> 3 ) + ( readBit != 0 );
					readTotalBytes += ( get >> 3 ) + ( readBit != 0 );
					return value;
				}
			}
			readByte++;
			readTotalBytes++;
		}
		get = 8 - readBit;
		if( get > ( numBits - valueBits ) )
		{
			get = ( numBits - valueBits );
		}
		fraction = readData[readByte - 1];
		fraction >>= readBit;
		fraction &= ( 1 << get ) - 1;
		value |= fraction << valueBits;
		valueBits += get;
		readBit = ( readBit + get ) & 7;
	}
	
	return value;
}

/*
================
budCompressor_BitStream::UnreadBits
================
*/
void budCompressor_BitStream::UnreadBits( int numBits )
{
	readByte -= ( numBits >> 3 );
	readTotalBytes -= ( numBits >> 3 );
	if( readBit == 0 )
	{
		readBit = 8 - ( numBits & 7 );
	}
	else
	{
		readBit -= numBits & 7;
		if( readBit <= 0 )
		{
			readByte--;
			readTotalBytes--;
			readBit = ( readBit + 8 ) & 7;
		}
	}
	if( readByte < 0 )
	{
		readByte = 0;
		readBit = 0;
	}
}

/*
================
budCompressor_BitStream::Compare
================
*/
int budCompressor_BitStream::Compare( const byte* src1, int bitPtr1, const byte* src2, int bitPtr2, int maxBits ) const
{
	int i;
	
	// If the two bit pointers are aligned then we can use a faster comparison
	if( ( bitPtr1 & 7 ) == ( bitPtr2 & 7 ) && maxBits > 16 )
	{
		const byte* p1 = &src1[bitPtr1 >> 3];
		const byte* p2 = &src2[bitPtr2 >> 3];
		
		int bits = 0;
		
		int bitsRemain = maxBits;
		
		// Compare the first couple bits (if any)
		if( bitPtr1 & 7 )
		{
			for( i = ( bitPtr1 & 7 ); i < 8; i++, bits++ )
			{
				if( ( ( *p1 >> i ) ^ ( *p2 >> i ) ) & 1 )
				{
					return bits;
				}
				bitsRemain--;
			}
			p1++;
			p2++;
		}
		
		int remain = bitsRemain >> 3;
		
		// Compare the middle bytes as ints
		while( remain >= 4 && ( *( const int* )p1 == *( const int* )p2 ) )
		{
			p1 += 4;
			p2 += 4;
			remain -= 4;
			bits += 32;
		}
		
		// Compare the remaining bytes
		while( remain > 0 && ( *p1 == *p2 ) )
		{
			p1++;
			p2++;
			remain--;
			bits += 8;
		}
		
		// Compare the last couple of bits (if any)
		int finalBits = 8;
		if( remain == 0 )
		{
			finalBits = ( bitsRemain & 7 );
		}
		for( i = 0; i < finalBits; i++, bits++ )
		{
			if( ( ( *p1 >> i ) ^ ( *p2 >> i ) ) & 1 )
			{
				return bits;
			}
		}
		
		assert( bits == maxBits );
		return bits;
	}
	else
	{
		for( i = 0; i < maxBits; i++ )
		{
			if( ( ( src1[bitPtr1 >> 3] >> ( bitPtr1 & 7 ) ) ^ ( src2[bitPtr2 >> 3] >> ( bitPtr2 & 7 ) ) ) & 1 )
			{
				break;
			}
			bitPtr1++;
			bitPtr2++;
		}
		return i;
	}
}

/*
================
budCompressor_BitStream::Write
================
*/
int budCompressor_BitStream::Write( const void* inData, int inLength )
{
	int i;
	
	if( compress == false || inLength <= 0 )
	{
		return 0;
	}
	
	InitCompress( inData, inLength );
	
	for( i = 0; i < inLength; i++ )
	{
		WriteBits( ReadBits( 8 ), 8 );
	}
	return i;
}

/*
================
budCompressor_BitStream::FinishCompress
================
*/
void budCompressor_BitStream::FinishCompress()
{
	if( compress == false )
	{
		return;
	}
	
	if( writeByte )
	{
		file->Write( buffer, writeByte );
	}
	writeLength = 0;
	writeByte = 0;
	writeBit = 0;
}

/*
================
budCompressor_BitStream::Read
================
*/
int budCompressor_BitStream::Read( void* outData, int outLength )
{
	int i;
	
	if( compress == true || outLength <= 0 )
	{
		return 0;
	}
	
	InitDecompress( outData, outLength );
	
	for( i = 0; i < outLength && readLength >= 0; i++ )
	{
		WriteBits( ReadBits( 8 ), 8 );
	}
	return i;
}

/*
================
budCompressor_BitStream::GetCompressionRatio
================
*/
float budCompressor_BitStream::GetCompressionRatio() const
{
	if( compress )
	{
		return ( readTotalBytes - writeTotalBytes ) * 100.0f / readTotalBytes;
	}
	else
	{
		return ( writeTotalBytes - readTotalBytes ) * 100.0f / writeTotalBytes;
	}
}


/*
=================================================================================

	budCompressor_RunLength

	The following algorithm implements run length compression with an arbitrary
	word size.

=================================================================================
*/

class budCompressor_RunLength : public budCompressor_BitStream
{
public:
	budCompressor_RunLength() {}
	
	void			Init( budFile* f, bool compress, int wordLength );
	
	int				Write( const void* inData, int inLength );
	int				Read( void* outData, int outLength );
	
private:
	int				runLengthCode;
};

/*
================
budCompressor_RunLength::Init
================
*/
void budCompressor_RunLength::Init( budFile* f, bool compress, int wordLength )
{
	budCompressor_BitStream::Init( f, compress, wordLength );
	runLengthCode = ( 1 << wordLength ) - 1;
}

/*
================
budCompressor_RunLength::Write
================
*/
int budCompressor_RunLength::Write( const void* inData, int inLength )
{
	int bits, nextBits, count;
	
	if( compress == false || inLength <= 0 )
	{
		return 0;
	}
	
	InitCompress( inData, inLength );
	
	while( readByte <= readLength )
	{
		count = 1;
		bits = ReadBits( wordLength );
		for( nextBits = ReadBits( wordLength ); nextBits == bits; nextBits = ReadBits( wordLength ) )
		{
			count++;
			if( count >= ( 1 << wordLength ) )
			{
				if( count >= ( 1 << wordLength ) + 3 || bits == runLengthCode )
				{
					break;
				}
			}
		}
		if( nextBits != bits )
		{
			UnreadBits( wordLength );
		}
		if( count > 3 || bits == runLengthCode )
		{
			WriteBits( runLengthCode, wordLength );
			WriteBits( bits, wordLength );
			if( bits != runLengthCode )
			{
				count -= 3;
			}
			WriteBits( count - 1, wordLength );
		}
		else
		{
			while( count-- )
			{
				WriteBits( bits, wordLength );
			}
		}
	}
	
	return inLength;
}

/*
================
budCompressor_RunLength::Read
================
*/
int budCompressor_RunLength::Read( void* outData, int outLength )
{
	int bits, count;
	
	if( compress == true || outLength <= 0 )
	{
		return 0;
	}
	
	InitDecompress( outData, outLength );
	
	while( writeByte <= writeLength && readLength >= 0 )
	{
		bits = ReadBits( wordLength );
		if( bits == runLengthCode )
		{
			bits = ReadBits( wordLength );
			count = ReadBits( wordLength ) + 1;
			if( bits != runLengthCode )
			{
				count += 3;
			}
			while( count-- )
			{
				WriteBits( bits, wordLength );
			}
		}
		else
		{
			WriteBits( bits, wordLength );
		}
	}
	
	return writeByte;
}


/*
=================================================================================

	budCompressor_RunLength_ZeroBased

	The following algorithm implements run length compression with an arbitrary
	word size for data with a lot of zero bits.

=================================================================================
*/

class budCompressor_RunLength_ZeroBased : public budCompressor_BitStream
{
public:
	budCompressor_RunLength_ZeroBased() {}
	
	int				Write( const void* inData, int inLength );
	int				Read( void* outData, int outLength );
	
private:
};

/*
================
budCompressor_RunLength_ZeroBased::Write
================
*/
int budCompressor_RunLength_ZeroBased::Write( const void* inData, int inLength )
{
	int bits, count;
	
	if( compress == false || inLength <= 0 )
	{
		return 0;
	}
	
	InitCompress( inData, inLength );
	
	while( readByte <= readLength )
	{
		count = 0;
		for( bits = ReadBits( wordLength ); bits == 0 && count < ( 1 << wordLength ); bits = ReadBits( wordLength ) )
		{
			count++;
		}
		if( count )
		{
			WriteBits( 0, wordLength );
			WriteBits( count - 1, wordLength );
			UnreadBits( wordLength );
		}
		else
		{
			WriteBits( bits, wordLength );
		}
	}
	
	return inLength;
}

/*
================
budCompressor_RunLength_ZeroBased::Read
================
*/
int budCompressor_RunLength_ZeroBased::Read( void* outData, int outLength )
{
	int bits, count;
	
	if( compress == true || outLength <= 0 )
	{
		return 0;
	}
	
	InitDecompress( outData, outLength );
	
	while( writeByte <= writeLength && readLength >= 0 )
	{
		bits = ReadBits( wordLength );
		if( bits == 0 )
		{
			count = ReadBits( wordLength ) + 1;
			while( count-- > 0 )
			{
				WriteBits( 0, wordLength );
			}
		}
		else
		{
			WriteBits( bits, wordLength );
		}
	}
	
	return writeByte;
}


/*
=================================================================================

	budCompressor_Huffman

	The following algorithm is based on the adaptive Huffman algorithm described
	in Sayood's Data Compression book. The ranks are not actually stored, but
	implicitly defined by the location of a node within a doubly-linked list

=================================================================================
*/

const int HMAX			= 256;				// Maximum symbol
const int NYT			= HMAX;				// NYT = Not Yet Transmitted
const int INTERNAL_NODE	= HMAX + 1;			// internal node

typedef struct nodetype
{
	struct nodetype* left, *right, *parent; // tree structure
	struct nodetype* next, *prev;			// doubly-linked list
	struct nodetype** head;					// highest ranked node in block
	int				weight;
	int				symbol;
} huffmanNode_t;

class budCompressor_Huffman : public budCompressor_None
{
public:
	budCompressor_Huffman() {}
	
	void			Init( budFile* f, bool compress, int wordLength );
	void			FinishCompress();
	float			GetCompressionRatio() const;
	
	int				Write( const void* inData, int inLength );
	int				Read( void* outData, int outLength );
	
private:
	byte			seq[65536];
	int				bloc;
	int				blocMax;
	int				blocIn;
	int				blocNode;
	int				blocPtrs;
	
	int				compressedSize;
	int				unCompressedSize;
	
	huffmanNode_t* 	tree;
	huffmanNode_t* 	lhead;
	huffmanNode_t* 	ltail;
	huffmanNode_t* 	loc[HMAX + 1];
	huffmanNode_t** freelist;
	
	huffmanNode_t	nodeList[768];
	huffmanNode_t* 	nodePtrs[768];
	
private:
	void			AddRef( byte ch );
	int				Receive( huffmanNode_t* node, int* ch );
	void			Transmit( int ch, byte* fout );
	void			PutBit( int bit, byte* fout, int* offset );
	int				GetBit( byte* fout, int* offset );
	
	void			Add_bit( char bit, byte* fout );
	int				Get_bit();
	huffmanNode_t** Get_ppnode();
	void			Free_ppnode( huffmanNode_t** ppnode );
	void			Swap( huffmanNode_t* node1, huffmanNode_t* node2 );
	void			Swaplist( huffmanNode_t* node1, huffmanNode_t* node2 );
	void			Increment( huffmanNode_t* node );
	void			Send( huffmanNode_t* node, huffmanNode_t* child, byte* fout );
};

/*
================
budCompressor_Huffman::Init
================
*/
void budCompressor_Huffman::Init( budFile* f, bool compress, int wordLength )
{
	int i;
	
	this->file = f;
	this->compress = compress;
	bloc = 0;
	blocMax = 0;
	blocIn = 0;
	blocNode = 0;
	blocPtrs = 0;
	compressedSize = 0;
	unCompressedSize = 0;
	
	tree = NULL;
	lhead = NULL;
	ltail = NULL;
	for( i = 0; i < ( HMAX + 1 ); i++ )
	{
		loc[i] = NULL;
	}
	freelist = NULL;
	
	for( i = 0; i < 768; i++ )
	{
		memset( &nodeList[i], 0, sizeof( huffmanNode_t ) );
		nodePtrs[i] = NULL;
	}
	
	if( compress )
	{
		// Add the NYT (not yet transmitted) node into the tree/list
		tree = lhead = loc[NYT] = &nodeList[blocNode++];
		tree->symbol = NYT;
		tree->weight = 0;
		lhead->next = lhead->prev = NULL;
		tree->parent = tree->left = tree->right = NULL;
	}
	else
	{
		// Initialize the tree & list with the NYT node
		tree = lhead = ltail = loc[NYT] = &nodeList[blocNode++];
		tree->symbol = NYT;
		tree->weight = 0;
		lhead->next = lhead->prev = NULL;
		tree->parent = tree->left = tree->right = NULL;
	}
}

/*
================
budCompressor_Huffman::PutBit
================
*/
void budCompressor_Huffman::PutBit( int bit, byte* fout, int* offset )
{
	bloc = *offset;
	if( ( bloc & 7 ) == 0 )
	{
		fout[( bloc >> 3 )] = 0;
	}
	fout[( bloc >> 3 )] |= bit << ( bloc & 7 );
	bloc++;
	*offset = bloc;
}

/*
================
budCompressor_Huffman::GetBit
================
*/
int budCompressor_Huffman::GetBit( byte* fin, int* offset )
{
	int t;
	bloc = *offset;
	t = ( fin[( bloc >> 3 )] >> ( bloc & 7 ) ) & 0x1;
	bloc++;
	*offset = bloc;
	return t;
}

/*
================
budCompressor_Huffman::Add_bit

  Add a bit to the output file (buffered)
================
*/
void budCompressor_Huffman::Add_bit( char bit, byte* fout )
{
	if( ( bloc & 7 ) == 0 )
	{
		fout[( bloc >> 3 )] = 0;
	}
	fout[( bloc >> 3 )] |= bit << ( bloc & 7 );
	bloc++;
}

/*
================
budCompressor_Huffman::Get_bit

  Get one bit from the input file (buffered)
================
*/
int budCompressor_Huffman::Get_bit()
{
	int t;
	int wh = bloc >> 3;
	int whb = wh >> 16;
	if( whb != blocIn )
	{
		blocMax += file->Read( seq, sizeof( seq ) );
		blocIn++;
	}
	wh &= 0xffff;
	t = ( seq[wh] >> ( bloc & 7 ) ) & 0x1;
	bloc++;
	return t;
}

/*
================
budCompressor_Huffman::Get_ppnode
================
*/
huffmanNode_t** budCompressor_Huffman::Get_ppnode()
{
	huffmanNode_t** tppnode;
	if( !freelist )
	{
		return &nodePtrs[blocPtrs++];
	}
	else
	{
		tppnode = freelist;
		freelist = ( huffmanNode_t** )*tppnode;
		return tppnode;
	}
}

/*
================
budCompressor_Huffman::Free_ppnode
================
*/
void budCompressor_Huffman::Free_ppnode( huffmanNode_t** ppnode )
{
	*ppnode = ( huffmanNode_t* )freelist;
	freelist = ppnode;
}

/*
================
budCompressor_Huffman::Swap

  Swap the location of the given two nodes in the tree.
================
*/
void budCompressor_Huffman::Swap( huffmanNode_t* node1, huffmanNode_t* node2 )
{
	huffmanNode_t* par1, *par2;
	
	par1 = node1->parent;
	par2 = node2->parent;
	
	if( par1 )
	{
		if( par1->left == node1 )
		{
			par1->left = node2;
		}
		else
		{
			par1->right = node2;
		}
	}
	else
	{
		tree = node2;
	}
	
	if( par2 )
	{
		if( par2->left == node2 )
		{
			par2->left = node1;
		}
		else
		{
			par2->right = node1;
		}
	}
	else
	{
		tree = node1;
	}
	
	node1->parent = par2;
	node2->parent = par1;
}

/*
================
budCompressor_Huffman::Swaplist

  Swap the given two nodes in the linked list (update ranks)
================
*/
void budCompressor_Huffman::Swaplist( huffmanNode_t* node1, huffmanNode_t* node2 )
{
	huffmanNode_t* par1;
	
	par1 = node1->next;
	node1->next = node2->next;
	node2->next = par1;
	
	par1 = node1->prev;
	node1->prev = node2->prev;
	node2->prev = par1;
	
	if( node1->next == node1 )
	{
		node1->next = node2;
	}
	if( node2->next == node2 )
	{
		node2->next = node1;
	}
	if( node1->next )
	{
		node1->next->prev = node1;
	}
	if( node2->next )
	{
		node2->next->prev = node2;
	}
	if( node1->prev )
	{
		node1->prev->next = node1;
	}
	if( node2->prev )
	{
		node2->prev->next = node2;
	}
}

/*
================
budCompressor_Huffman::Increment
================
*/
void budCompressor_Huffman::Increment( huffmanNode_t* node )
{
	huffmanNode_t* lnode;
	
	if( !node )
	{
		return;
	}
	
	if( node->next != NULL && node->next->weight == node->weight )
	{
		lnode = *node->head;
		if( lnode != node->parent )
		{
			Swap( lnode, node );
		}
		Swaplist( lnode, node );
	}
	if( node->prev && node->prev->weight == node->weight )
	{
		*node->head = node->prev;
	}
	else
	{
		*node->head = NULL;
		Free_ppnode( node->head );
	}
	node->weight++;
	if( node->next && node->next->weight == node->weight )
	{
		node->head = node->next->head;
	}
	else
	{
		node->head = Get_ppnode();
		*node->head = node;
	}
	if( node->parent )
	{
		Increment( node->parent );
		if( node->prev == node->parent )
		{
			Swaplist( node, node->parent );
			if( *node->head == node )
			{
				*node->head = node->parent;
			}
		}
	}
}

/*
================
budCompressor_Huffman::AddRef
================
*/
void budCompressor_Huffman::AddRef( byte ch )
{
	huffmanNode_t* tnode, *tnode2;
	if( loc[ch] == NULL )    /* if this is the first transmission of this node */
	{
		tnode = &nodeList[blocNode++];
		tnode2 = &nodeList[blocNode++];
		
		tnode2->symbol = INTERNAL_NODE;
		tnode2->weight = 1;
		tnode2->next = lhead->next;
		if( lhead->next )
		{
			lhead->next->prev = tnode2;
			if( lhead->next->weight == 1 )
			{
				tnode2->head = lhead->next->head;
			}
			else
			{
				tnode2->head = Get_ppnode();
				*tnode2->head = tnode2;
			}
		}
		else
		{
			tnode2->head = Get_ppnode();
			*tnode2->head = tnode2;
		}
		lhead->next = tnode2;
		tnode2->prev = lhead;
		
		tnode->symbol = ch;
		tnode->weight = 1;
		tnode->next = lhead->next;
		if( lhead->next )
		{
			lhead->next->prev = tnode;
			if( lhead->next->weight == 1 )
			{
				tnode->head = lhead->next->head;
			}
			else
			{
				/* this should never happen */
				tnode->head = Get_ppnode();
				*tnode->head = tnode2;
			}
		}
		else
		{
			/* this should never happen */
			tnode->head = Get_ppnode();
			*tnode->head = tnode;
		}
		lhead->next = tnode;
		tnode->prev = lhead;
		tnode->left = tnode->right = NULL;
		
		if( lhead->parent )
		{
			if( lhead->parent->left == lhead )    /* lhead is guaranteed to by the NYT */
			{
				lhead->parent->left = tnode2;
			}
			else
			{
				lhead->parent->right = tnode2;
			}
		}
		else
		{
			tree = tnode2;
		}
		
		tnode2->right = tnode;
		tnode2->left = lhead;
		
		tnode2->parent = lhead->parent;
		lhead->parent = tnode->parent = tnode2;
		
		loc[ch] = tnode;
		
		Increment( tnode2->parent );
	}
	else
	{
		Increment( loc[ch] );
	}
}

/*
================
budCompressor_Huffman::Receive

  Get a symbol.
================
*/
int budCompressor_Huffman::Receive( huffmanNode_t* node, int* ch )
{
	while( node && node->symbol == INTERNAL_NODE )
	{
		if( Get_bit() )
		{
			node = node->right;
		}
		else
		{
			node = node->left;
		}
	}
	if( !node )
	{
		return 0;
	}
	return ( *ch = node->symbol );
}

/*
================
budCompressor_Huffman::Send

  Send the prefix code for this node.
================
*/
void budCompressor_Huffman::Send( huffmanNode_t* node, huffmanNode_t* child, byte* fout )
{
	if( node->parent )
	{
		Send( node->parent, node, fout );
	}
	if( child )
	{
		if( node->right == child )
		{
			Add_bit( 1, fout );
		}
		else
		{
			Add_bit( 0, fout );
		}
	}
}

/*
================
budCompressor_Huffman::Transmit

  Send a symbol.
================
*/
void budCompressor_Huffman::Transmit( int ch, byte* fout )
{
	int i;
	if( loc[ch] == NULL )
	{
		/* huffmanNode_t hasn't been transmitted, send a NYT, then the symbol */
		Transmit( NYT, fout );
		for( i = 7; i >= 0; i-- )
		{
			Add_bit( ( char )( ( ch >> i ) & 0x1 ), fout );
		}
	}
	else
	{
		Send( loc[ch], NULL, fout );
	}
}

/*
================
budCompressor_Huffman::Write
================
*/
int budCompressor_Huffman::Write( const void* inData, int inLength )
{
	int i, ch;
	
	if( compress == false || inLength <= 0 )
	{
		return 0;
	}
	
	for( i = 0; i < inLength; i++ )
	{
		ch = ( ( const byte* )inData )[i];
		Transmit( ch, seq );				/* Transmit symbol */
		AddRef( ( byte )ch );					/* Do update */
		int b = ( bloc >> 3 );
		if( b > 32768 )
		{
			file->Write( seq, b );
			seq[0] = seq[b];
			bloc &= 7;
			compressedSize += b;
		}
	}
	
	unCompressedSize += i;
	return i;
}

/*
================
budCompressor_Huffman::FinishCompress
================
*/
void budCompressor_Huffman::FinishCompress()
{

	if( compress == false )
	{
		return;
	}
	
	bloc += 7;
	int str = ( bloc >> 3 );
	if( str )
	{
		file->Write( seq, str );
		compressedSize += str;
	}
}

/*
================
budCompressor_Huffman::Read
================
*/
int budCompressor_Huffman::Read( void* outData, int outLength )
{
	int i, j, ch;
	
	if( compress == true || outLength <= 0 )
	{
		return 0;
	}
	
	if( bloc == 0 )
	{
		blocMax = file->Read( seq, sizeof( seq ) );
		blocIn = 0;
	}
	
	for( i = 0; i < outLength; i++ )
	{
		ch = 0;
		// don't overflow reading from the file
		if( ( bloc >> 3 ) > blocMax )
		{
			break;
		}
		Receive( tree, &ch );				/* Get a character */
		if( ch == NYT )  					/* We got a NYT, get the symbol associated with it */
		{
			ch = 0;
			for( j = 0; j < 8; j++ )
			{
				ch = ( ch << 1 ) + Get_bit();
			}
		}
		
		( ( byte* )outData )[i] = ch;			/* Write symbol */
		AddRef( ( byte ) ch );				/* Increment node */
	}
	
	compressedSize = bloc >> 3;
	unCompressedSize += i;
	return i;
}

/*
================
budCompressor_Huffman::GetCompressionRatio
================
*/
float budCompressor_Huffman::GetCompressionRatio() const
{
	return ( unCompressedSize - compressedSize ) * 100.0f / unCompressedSize;
}


/*
=================================================================================

	budCompressor_Arithmetic

	The following algorithm is based on the Arithmetic Coding methods described
	by Mark Nelson. The probability table is implicitly stored.

=================================================================================
*/

const int AC_WORD_LENGTH	= 8;
const int AC_NUM_BITS		= 16;
const int AC_MSB_SHIFT		= 15;
const int AC_MSB2_SHIFT		= 14;
const int AC_MSB_MASK		= 0x8000;
const int AC_MSB2_MASK		= 0x4000;
const int AC_HIGH_INIT		= 0xffff;
const int AC_LOW_INIT		= 0x0000;

class budCompressor_Arithmetic : public budCompressor_BitStream
{
public:
	budCompressor_Arithmetic() {}
	
	void			Init( budFile* f, bool compress, int wordLength );
	void			FinishCompress();
	
	int				Write( const void* inData, int inLength );
	int				Read( void* outData, int outLength );
	
private:
	typedef struct acProbs_s
	{
		unsigned int	low;
		unsigned int	high;
	} acProbs_t;
	
	typedef struct acSymbol_s
	{
		unsigned int	low;
		unsigned int	high;
		int				position;
	} acSymbol_t;
	
	acProbs_t		probabilities[1 << AC_WORD_LENGTH];
	
	int				symbolBuffer;
	int				symbolBit;
	
	unsigned short	low;
	unsigned short	high;
	unsigned short	code;
	unsigned int	underflowBits;
	unsigned int	scale;
	
private:
	void			InitProbabilities();
	void			UpdateProbabilities( acSymbol_t* symbol );
	int				ProbabilityForCount( unsigned int count );
	
	void			CharToSymbol( byte c, acSymbol_t* symbol );
	void			EncodeSymbol( acSymbol_t* symbol );
	
	int				SymbolFromCount( unsigned int count, acSymbol_t* symbol );
	int				GetCurrentCount();
	void			RemoveSymbolFromStream( acSymbol_t* symbol );
	
	void			PutBit( int bit );
	int				GetBit();
	
	void			WriteOverflowBits();
};

/*
================
budCompressor_Arithmetic::Init
================
*/
void budCompressor_Arithmetic::Init( budFile* f, bool compress, int wordLength )
{
	budCompressor_BitStream::Init( f, compress, wordLength );
	
	symbolBuffer	= 0;
	symbolBit		= 0;
}

/*
================
budCompressor_Arithmetic::InitProbabilities
================
*/
void budCompressor_Arithmetic::InitProbabilities()
{
	high			= AC_HIGH_INIT;
	low				= AC_LOW_INIT;
	underflowBits	= 0;
	code			= 0;
	
	for( int i = 0; i < ( 1 << AC_WORD_LENGTH ); i++ )
	{
		probabilities[ i ].low = i;
		probabilities[ i ].high = i + 1;
	}
	
	scale = ( 1 << AC_WORD_LENGTH );
}

/*
================
budCompressor_Arithmetic::UpdateProbabilities
================
*/
void budCompressor_Arithmetic::UpdateProbabilities( acSymbol_t* symbol )
{
	int i, x;
	
	x = symbol->position;
	
	probabilities[ x ].high++;
	
	for( i = x + 1; i < ( 1 << AC_WORD_LENGTH ); i++ )
	{
		probabilities[ i ].low++;
		probabilities[ i ].high++;
	}
	
	scale++;
}

/*
================
budCompressor_Arithmetic::GetCurrentCount
================
*/
int budCompressor_Arithmetic::GetCurrentCount()
{
	// DG: use int instead of long for 64bit compatibility
	return ( unsigned int )( ( ( ( ( int ) code - low ) + 1 ) * scale - 1 ) / ( ( ( int ) high - low ) + 1 ) );
	// DG end
}

/*
================
budCompressor_Arithmetic::ProbabilityForCount
================
*/
int budCompressor_Arithmetic::ProbabilityForCount( unsigned int count )
{
#if 1

	int len, mid, offset, res;
	
	len = ( 1 << AC_WORD_LENGTH );
	mid = len;
	offset = 0;
	res = 0;
	while( mid > 0 )
	{
		mid = len >> 1;
		if( count >= probabilities[offset + mid].high )
		{
			offset += mid;
			len -= mid;
			res = 1;
		}
		else if( count < probabilities[offset + mid].low )
		{
			len -= mid;
			res = 0;
		}
		else
		{
			return offset + mid;
		}
	}
	return offset + res;
	
#else
	
	int j;
	
	for( j = 0; j < ( 1 << AC_WORD_LENGTH ); j++ )
	{
		if( count >= probabilities[ j ].low && count < probabilities[ j ].high )
		{
			return j;
		}
	}
	
	assert( false );
	
	return 0;
	
#endif
}

/*
================
budCompressor_Arithmetic::SymbolFromCount
================
*/
int budCompressor_Arithmetic::SymbolFromCount( unsigned int count, acSymbol_t* symbol )
{
	int p = ProbabilityForCount( count );
	symbol->low = probabilities[ p ].low;
	symbol->high = probabilities[ p ].high;
	symbol->position = p;
	return p;
}

/*
================
budCompressor_Arithmetic::RemoveSymbolFromStream
================
*/
void budCompressor_Arithmetic::RemoveSymbolFromStream( acSymbol_t* symbol )
{
	// DG: use int instead of long for 64bit compatibility
	int range;
	
	range	= ( int )( high - low ) + 1;
	// DG end
	high	= low + ( unsigned short )( ( range * symbol->high ) / scale - 1 );
	low		= low + ( unsigned short )( ( range * symbol->low ) / scale );
	
	while( true )
	{
	
		if( ( high & AC_MSB_MASK ) == ( low & AC_MSB_MASK ) )
		{
		
		}
		else if( ( low & AC_MSB2_MASK ) == AC_MSB2_MASK && ( high & AC_MSB2_MASK ) == 0 )
		{
			code	^= AC_MSB2_MASK;
			low		&= AC_MSB2_MASK - 1;
			high	|= AC_MSB2_MASK;
		}
		else
		{
			UpdateProbabilities( symbol );
			return;
		}
		
		low <<= 1;
		high <<= 1;
		high |= 1;
		code <<= 1;
		code |= ReadBits( 1 );
	}
}

/*
================
budCompressor_Arithmetic::GetBit
================
*/
int budCompressor_Arithmetic::GetBit()
{
	int getbit;
	
	if( symbolBit <= 0 )
	{
		// read a new symbol out
		acSymbol_t symbol;
		symbolBuffer = SymbolFromCount( GetCurrentCount(), &symbol );
		RemoveSymbolFromStream( &symbol );
		symbolBit = AC_WORD_LENGTH;
	}
	
	getbit = ( symbolBuffer >> ( AC_WORD_LENGTH - symbolBit ) ) & 1;
	symbolBit--;
	
	return getbit;
}

/*
================
budCompressor_Arithmetic::EncodeSymbol
================
*/
void budCompressor_Arithmetic::EncodeSymbol( acSymbol_t* symbol )
{
	unsigned int range;
	
	// rescale high and low for the new symbol.
	range	= ( high - low ) + 1;
	high	= low + ( unsigned short )( ( range * symbol->high ) / scale - 1 );
	low		= low + ( unsigned short )( ( range * symbol->low ) / scale );
	
	while( true )
	{
		if( ( high & AC_MSB_MASK ) == ( low & AC_MSB_MASK ) )
		{
			// the high digits of low and high have converged, and can be written to the stream
			WriteBits( high >> AC_MSB_SHIFT, 1 );
			
			while( underflowBits > 0 )
			{
			
				WriteBits( ~high >> AC_MSB_SHIFT, 1 );
				
				underflowBits--;
			}
		}
		else if( ( low & AC_MSB2_MASK ) && !( high & AC_MSB2_MASK ) )
		{
			// underflow is in danger of happening, 2nd digits are converging but 1st digits don't match
			underflowBits	+= 1;
			low				&= AC_MSB2_MASK - 1;
			high			|= AC_MSB2_MASK;
		}
		else
		{
			UpdateProbabilities( symbol );
			return;
		}
		
		low <<= 1;
		high <<= 1;
		high |=	1;
	}
}

/*
================
budCompressor_Arithmetic::CharToSymbol
================
*/
void budCompressor_Arithmetic::CharToSymbol( byte c, acSymbol_t* symbol )
{
	symbol->low			= probabilities[ c ].low;
	symbol->high		= probabilities[ c ].high;
	symbol->position	= c;
}

/*
================
budCompressor_Arithmetic::PutBit
================
*/
void budCompressor_Arithmetic::PutBit( int putbit )
{
	symbolBuffer |= ( putbit & 1 ) << symbolBit;
	symbolBit++;
	
	if( symbolBit >= AC_WORD_LENGTH )
	{
		acSymbol_t symbol;
		
		CharToSymbol( symbolBuffer, &symbol );
		EncodeSymbol( &symbol );
		
		symbolBit = 0;
		symbolBuffer = 0;
	}
}

/*
================
budCompressor_Arithmetic::WriteOverflowBits
================
*/
void budCompressor_Arithmetic::WriteOverflowBits()
{

	WriteBits( low >> AC_MSB2_SHIFT, 1 );
	
	underflowBits++;
	while( underflowBits-- > 0 )
	{
		WriteBits( ~low >> AC_MSB2_SHIFT, 1 );
	}
}

/*
================
budCompressor_Arithmetic::Write
================
*/
int budCompressor_Arithmetic::Write( const void* inData, int inLength )
{
	int i, j;
	
	if( compress == false || inLength <= 0 )
	{
		return 0;
	}
	
	InitCompress( inData, inLength );
	
	for( i = 0; i < inLength; i++ )
	{
		if( ( readTotalBytes & ( ( 1 << 14 ) - 1 ) ) == 0 )
		{
			if( readTotalBytes )
			{
				WriteOverflowBits();
				WriteBits( 0, 15 );
				while( writeBit )
				{
					WriteBits( 0, 1 );
				}
				WriteBits( 255, 8 );
			}
			InitProbabilities();
		}
		for( j = 0; j < 8; j++ )
		{
			PutBit( ReadBits( 1 ) );
		}
	}
	
	return inLength;
}

/*
================
budCompressor_Arithmetic::FinishCompress
================
*/
void budCompressor_Arithmetic::FinishCompress()
{
	if( compress == false )
	{
		return;
	}
	
	WriteOverflowBits();
	
	budCompressor_BitStream::FinishCompress();
}

/*
================
budCompressor_Arithmetic::Read
================
*/
int budCompressor_Arithmetic::Read( void* outData, int outLength )
{
	int i, j;
	
	if( compress == true || outLength <= 0 )
	{
		return 0;
	}
	
	InitDecompress( outData, outLength );
	
	for( i = 0; i < outLength && readLength >= 0; i++ )
	{
		if( ( writeTotalBytes & ( ( 1 << 14 ) - 1 ) ) == 0 )
		{
			if( writeTotalBytes )
			{
				while( readBit )
				{
					ReadBits( 1 );
				}
				while( ReadBits( 8 ) == 0 && readLength > 0 )
				{
				}
			}
			InitProbabilities();
			for( j = 0; j < AC_NUM_BITS; j++ )
			{
				code <<= 1;
				code |= ReadBits( 1 );
			}
		}
		for( j = 0; j < 8; j++ )
		{
			WriteBits( GetBit(), 1 );
		}
	}
	
	return i;
}


/*
=================================================================================

	budCompressor_LZSS

	In 1977 Abraham Lempel and Jacob Ziv presented a dictionary based scheme for
	text compression called LZ77. For any new text LZ77 outputs an offset/length
	pair to previously seen text and the next new byte after the previously seen
	text.

	In 1982 James Storer and Thomas Szymanski presented a modification on the work
	of Lempel and Ziv called LZSS. LZ77 always outputs an offset/length pair, even
	if a match is only one byte long. An offset/length pair usually takes more than
	a single byte to store and the compression is not optimal for small match sizes.
	LZSS uses a bit flag which tells whether the following data is a literal (byte)
	or an offset/length pair.

	The following algorithm is an implementation of LZSS with arbitrary word size.

=================================================================================
*/

const int LZSS_BLOCK_SIZE		= 65535;
const int LZSS_HASH_BITS		= 10;
const int LZSS_HASH_SIZE		= ( 1 << LZSS_HASH_BITS );
const int LZSS_HASH_MASK		= ( 1 << LZSS_HASH_BITS ) - 1;
const int LZSS_OFFSET_BITS		= 11;
const int LZSS_LENGTH_BITS		= 5;

class budCompressor_LZSS : public budCompressor_BitStream
{
public:
	budCompressor_LZSS() {}
	
	void			Init( budFile* f, bool compress, int wordLength );
	void			FinishCompress();
	
	int				Write( const void* inData, int inLength );
	int				Read( void* outData, int outLength );
	
protected:
	int				offsetBits;
	int				lengthBits;
	int				minMatchWords;
	
	byte			block[LZSS_BLOCK_SIZE];
	int				blockSize;
	int				blockIndex;
	
	int				hashTable[LZSS_HASH_SIZE];
	int				hashNext[LZSS_BLOCK_SIZE * 8];
	
protected:
	bool			FindMatch( int startWord, int startValue, int& wordOffset, int& numWords );
	void			AddToHash( int index, int hash );
	int				GetWordFromBlock( int wordOffset ) const;
	virtual void	CompressBlock();
	virtual void	DecompressBlock();
};

/*
================
budCompressor_LZSS::Init
================
*/
void budCompressor_LZSS::Init( budFile* f, bool compress, int wordLength )
{
	budCompressor_BitStream::Init( f, compress, wordLength );
	
	offsetBits = LZSS_OFFSET_BITS;
	lengthBits = LZSS_LENGTH_BITS;
	
	minMatchWords = ( offsetBits + lengthBits + wordLength ) / wordLength;
	blockSize = 0;
	blockIndex = 0;
}

/*
================
budCompressor_LZSS::FindMatch
================
*/
bool budCompressor_LZSS::FindMatch( int startWord, int startValue, int& wordOffset, int& numWords )
{
	int i, n, hash, bottom, maxBits;
	
	wordOffset = startWord;
	numWords = minMatchWords - 1;
	
	bottom = Max( 0, startWord - ( ( 1 << offsetBits ) - 1 ) );
	maxBits = ( blockSize << 3 ) - startWord * wordLength;
	
	hash = startValue & LZSS_HASH_MASK;
	for( i = hashTable[hash]; i >= bottom; i = hashNext[i] )
	{
		n = Compare( block, i * wordLength, block, startWord * wordLength, Min( maxBits, ( startWord - i ) * wordLength ) );
		if( n > numWords * wordLength )
		{
			numWords = n / wordLength;
			wordOffset = i;
			if( numWords > ( ( 1 << lengthBits ) - 1 + minMatchWords ) - 1 )
			{
				numWords = ( ( 1 << lengthBits ) - 1 + minMatchWords ) - 1;
				break;
			}
		}
	}
	
	return ( numWords >= minMatchWords );
}

/*
================
budCompressor_LZSS::AddToHash
================
*/
void budCompressor_LZSS::AddToHash( int index, int hash )
{
	hashNext[index] = hashTable[hash];
	hashTable[hash] = index;
}

/*
================
budCompressor_LZSS::GetWordFromBlock
================
*/
int budCompressor_LZSS::GetWordFromBlock( int wordOffset ) const
{
	int blockBit, blockByte, value, valueBits, get, fraction;
	
	blockBit = ( wordOffset * wordLength ) & 7;
	blockByte = ( wordOffset * wordLength ) >> 3;
	if( blockBit != 0 )
	{
		blockByte++;
	}
	
	value = 0;
	valueBits = 0;
	
	while( valueBits < wordLength )
	{
		if( blockBit == 0 )
		{
			if( blockByte >= LZSS_BLOCK_SIZE )
			{
				return value;
			}
			blockByte++;
		}
		get = 8 - blockBit;
		if( get > ( wordLength - valueBits ) )
		{
			get = ( wordLength - valueBits );
		}
		fraction = block[blockByte - 1];
		fraction >>= blockBit;
		fraction &= ( 1 << get ) - 1;
		value |= fraction << valueBits;
		valueBits += get;
		blockBit = ( blockBit + get ) & 7;
	}
	
	return value;
}

/*
================
budCompressor_LZSS::CompressBlock
================
*/
void budCompressor_LZSS::CompressBlock()
{
	int i, startWord, startValue, wordOffset, numWords;
	
	InitCompress( block, blockSize );
	
	memset( hashTable, -1, sizeof( hashTable ) );
	memset( hashNext, -1, sizeof( hashNext ) );
	
	startWord = 0;
	while( readByte < readLength )
	{
		startValue = ReadBits( wordLength );
		if( FindMatch( startWord, startValue, wordOffset, numWords ) )
		{
			WriteBits( 1, 1 );
			WriteBits( startWord - wordOffset, offsetBits );
			WriteBits( numWords - minMatchWords, lengthBits );
			UnreadBits( wordLength );
			for( i = 0; i < numWords; i++ )
			{
				startValue = ReadBits( wordLength );
				AddToHash( startWord, startValue & LZSS_HASH_MASK );
				startWord++;
			}
		}
		else
		{
			WriteBits( 0, 1 );
			WriteBits( startValue, wordLength );
			AddToHash( startWord, startValue & LZSS_HASH_MASK );
			startWord++;
		}
	}
	
	blockSize = 0;
}

/*
================
budCompressor_LZSS::DecompressBlock
================
*/
void budCompressor_LZSS::DecompressBlock()
{
	int i, offset, startWord, numWords;
	
	InitDecompress( block, LZSS_BLOCK_SIZE );
	
	startWord = 0;
	while( writeByte < writeLength && readLength >= 0 )
	{
		if( ReadBits( 1 ) )
		{
			offset = startWord - ReadBits( offsetBits );
			numWords = ReadBits( lengthBits ) + minMatchWords;
			for( i = 0; i < numWords; i++ )
			{
				WriteBits( GetWordFromBlock( offset + i ), wordLength );
				startWord++;
			}
		}
		else
		{
			WriteBits( ReadBits( wordLength ), wordLength );
			startWord++;
		}
	}
	
	blockSize = Min( writeByte, LZSS_BLOCK_SIZE );
}

/*
================
budCompressor_LZSS::Write
================
*/
int budCompressor_LZSS::Write( const void* inData, int inLength )
{
	int i, n;
	
	if( compress == false || inLength <= 0 )
	{
		return 0;
	}
	
	for( n = i = 0; i < inLength; i += n )
	{
		n = LZSS_BLOCK_SIZE - blockSize;
		if( inLength - i >= n )
		{
			memcpy( block + blockSize, ( ( const byte* )inData ) + i, n );
			blockSize = LZSS_BLOCK_SIZE;
			CompressBlock();
			blockSize = 0;
		}
		else
		{
			memcpy( block + blockSize, ( ( const byte* )inData ) + i, inLength - i );
			n = inLength - i;
			blockSize += n;
		}
	}
	
	return inLength;
}

/*
================
budCompressor_LZSS::FinishCompress
================
*/
void budCompressor_LZSS::FinishCompress()
{
	if( compress == false )
	{
		return;
	}
	if( blockSize )
	{
		CompressBlock();
	}
	budCompressor_BitStream::FinishCompress();
}

/*
================
budCompressor_LZSS::Read
================
*/
int budCompressor_LZSS::Read( void* outData, int outLength )
{
	int i, n;
	
	if( compress == true || outLength <= 0 )
	{
		return 0;
	}
	
	if( !blockSize )
	{
		DecompressBlock();
	}
	
	for( n = i = 0; i < outLength; i += n )
	{
		if( !blockSize )
		{
			return i;
		}
		n = blockSize - blockIndex;
		if( outLength - i >= n )
		{
			memcpy( ( ( byte* )outData ) + i, block + blockIndex, n );
			DecompressBlock();
			blockIndex = 0;
		}
		else
		{
			memcpy( ( ( byte* )outData ) + i, block + blockIndex, outLength - i );
			n = outLength - i;
			blockIndex += n;
		}
	}
	
	return outLength;
}

/*
=================================================================================

	budCompressor_LZSS_WordAligned

	Outputs word aligned compressed data.

=================================================================================
*/

class budCompressor_LZSS_WordAligned : public budCompressor_LZSS
{
public:
	budCompressor_LZSS_WordAligned() {}
	
	void			Init( budFile* f, bool compress, int wordLength );
private:
	virtual void	CompressBlock();
	virtual void	DecompressBlock();
};

/*
================
budCompressor_LZSS_WordAligned::Init
================
*/
void budCompressor_LZSS_WordAligned::Init( budFile* f, bool compress, int wordLength )
{
	budCompressor_LZSS::Init( f, compress, wordLength );
	
	offsetBits = 2 * wordLength;
	lengthBits = wordLength;
	
	minMatchWords = ( offsetBits + lengthBits + wordLength ) / wordLength;
	blockSize = 0;
	blockIndex = 0;
}

/*
================
budCompressor_LZSS_WordAligned::CompressBlock
================
*/
void budCompressor_LZSS_WordAligned::CompressBlock()
{
	int i, startWord, startValue, wordOffset, numWords;
	
	InitCompress( block, blockSize );
	
	memset( hashTable, -1, sizeof( hashTable ) );
	memset( hashNext, -1, sizeof( hashNext ) );
	
	startWord = 0;
	while( readByte < readLength )
	{
		startValue = ReadBits( wordLength );
		if( FindMatch( startWord, startValue, wordOffset, numWords ) )
		{
			WriteBits( numWords - ( minMatchWords - 1 ), lengthBits );
			WriteBits( startWord - wordOffset, offsetBits );
			UnreadBits( wordLength );
			for( i = 0; i < numWords; i++ )
			{
				startValue = ReadBits( wordLength );
				AddToHash( startWord, startValue & LZSS_HASH_MASK );
				startWord++;
			}
		}
		else
		{
			WriteBits( 0, lengthBits );
			WriteBits( startValue, wordLength );
			AddToHash( startWord, startValue & LZSS_HASH_MASK );
			startWord++;
		}
	}
	
	blockSize = 0;
}

/*
================
budCompressor_LZSS_WordAligned::DecompressBlock
================
*/
void budCompressor_LZSS_WordAligned::DecompressBlock()
{
	int i, offset, startWord, numWords;
	
	InitDecompress( block, LZSS_BLOCK_SIZE );
	
	startWord = 0;
	while( writeByte < writeLength && readLength >= 0 )
	{
		numWords = ReadBits( lengthBits );
		if( numWords )
		{
			numWords += ( minMatchWords - 1 );
			offset = startWord - ReadBits( offsetBits );
			for( i = 0; i < numWords; i++ )
			{
				WriteBits( GetWordFromBlock( offset + i ), wordLength );
				startWord++;
			}
		}
		else
		{
			WriteBits( ReadBits( wordLength ), wordLength );
			startWord++;
		}
	}
	
	blockSize = Min( writeByte, LZSS_BLOCK_SIZE );
}

/*
=================================================================================

	budCompressor_LZW

	http://www.unisys.com/about__unisys/lzw
	http://www.dogma.net/markn/articles/lzw/lzw.htm
	http://www.cs.cf.ac.uk/Dave/Multimedia/node214.html
	http://www.cs.duke.edu/csed/curious/compression/lzw.html
	http://oldwww.rasip.fer.hr/research/compress/algorithms/fund/lz/lzw.html

	This is the same compression scheme used by GIF with the exception that
	the EOI and clear codes are not explicitly stored.  Instead EOI happens
	when the input stream runs dry and CC happens when the table gets to big.

	This is a derivation of LZ78, but the dictionary starts with all single
	character values so only code words are output.  It is similar in theory
	to LZ77, but instead of using the previous X bytes as a lookup table, a table
	is built as the stream is read.  The	compressor and decompressor use the
	same formula, so the tables should be exactly alike.  The only catch is the
	decompressor is always one step behind the compressor and may get a code not
	yet in the table.  In this case, it is easy to determine what the next code
	is going to be (it will be the previous string plus the first byte of the
	previous string).

	The dictionary can be any size, but 12 bits seems to produce best results for
	most sample data.  The code size is variable.  It starts with the minimum
	number of bits required to store the dictionary and automatically increases
	as the dictionary gets bigger (it starts at 9 bits and grows to 10 bits when
	item 512 is added, 11 bits when 1024 is added, etc...) once the the dictionary
	is filled (4096 items for a 12 bit dictionary), the whole thing is cleared and
	the process starts over again.

	The compressor increases the bit size after it adds the item, while the
	decompressor does before it adds the item.  The difference is subtle, but
	it's because decompressor being one step behind.  Otherwise, the decompressor
	would read 512 with only 9 bits.

	If "Hello" is in the dictionary, then "Hell", "Hel", "He" and "H" will be too.
	We use this to our advantage by storing the index of the previous code, and
	the value of the last character.  This means when we traverse through the
	dictionary, we get the characters in reverse.

	Dictionary entries 0-255 are always going to have the values 0-255

=================================================================================
*/

class budCompressor_LZW : public budCompressor_BitStream
{
public:
	budCompressor_LZW() {}
	
	void			Init( budFile* f, bool compress, int wordLength );
	void			FinishCompress();
	
	int				Write( const void* inData, int inLength );
	int				Read( void* outData, int outLength );
	
protected:
	int				AddToDict( int w, int k );
	int				Lookup( int w, int k );
	
	bool			BumpBits();
	
	int				WriteChain( int code );
	void			DecompressBlock();
	
	static const int LZW_BLOCK_SIZE = 32767;
	static const int LZW_START_BITS = 9;
	static const int LZW_FIRST_CODE = ( 1 << ( LZW_START_BITS - 1 ) );
	static const int LZW_DICT_BITS = 12;
	static const int LZW_DICT_SIZE = 1 << LZW_DICT_BITS;
	
	// Dictionary data
	struct
	{
		int k;
		int w;
	}				dictionary[LZW_DICT_SIZE];
	budHashIndex		index;
	
	int				nextCode;
	int				codeBits;
	
	// Block data
	byte			block[LZW_BLOCK_SIZE];
	int				blockSize;
	int				blockIndex;
	
	// Used by the compressor
	int				w;
	
	// Used by the decompressor
	int				oldCode;
};

/*
================
budCompressor_LZW::Init
================
*/
void budCompressor_LZW::Init( budFile* f, bool compress, int wordLength )
{
	budCompressor_BitStream::Init( f, compress, wordLength );
	
	for( int i = 0; i < LZW_FIRST_CODE; i++ )
	{
		dictionary[i].k = i;
		dictionary[i].w = -1;
	}
	index.Clear();
	
	nextCode = LZW_FIRST_CODE;
	codeBits = LZW_START_BITS;
	
	blockSize = 0;
	blockIndex = 0;
	
	w = -1;
	oldCode = -1;
}

/*
================
budCompressor_LZW::Read
================
*/
int budCompressor_LZW::Read( void* outData, int outLength )
{
	int i, n;
	
	if( compress == true || outLength <= 0 )
	{
		return 0;
	}
	
	if( !blockSize )
	{
		DecompressBlock();
	}
	
	for( n = i = 0; i < outLength; i += n )
	{
		if( !blockSize )
		{
			return i;
		}
		n = blockSize - blockIndex;
		if( outLength - i >= n )
		{
			memcpy( ( ( byte* )outData ) + i, block + blockIndex, n );
			DecompressBlock();
			blockIndex = 0;
		}
		else
		{
			memcpy( ( ( byte* )outData ) + i, block + blockIndex, outLength - i );
			n = outLength - i;
			blockIndex += n;
		}
	}
	
	return outLength;
}

/*
================
budCompressor_LZW::Lookup
================
*/
int budCompressor_LZW::Lookup( int w, int k )
{
	int j;
	
	if( w == -1 )
	{
		return k;
	}
	else
	{
		for( j = index.First( w ^ k ); j >= 0 ; j = index.Next( j ) )
		{
			if( dictionary[ j ].k == k && dictionary[ j ].w == w )
			{
				return j;
			}
		}
	}
	
	return -1;
}

/*
================
budCompressor_LZW::AddToDict
================
*/
int budCompressor_LZW::AddToDict( int w, int k )
{
	dictionary[ nextCode ].k = k;
	dictionary[ nextCode ].w = w;
	index.Add( w ^ k, nextCode );
	return nextCode++;
}

/*
================
budCompressor_LZW::BumpBits

Possibly increments codeBits
Returns true if the dictionary was cleared
================
*/
bool budCompressor_LZW::BumpBits()
{
	if( nextCode == ( 1 << codeBits ) )
	{
		codeBits ++;
		if( codeBits > LZW_DICT_BITS )
		{
			nextCode = LZW_FIRST_CODE;
			codeBits = LZW_START_BITS;
			index.Clear();
			return true;
		}
	}
	return false;
}

/*
================
budCompressor_LZW::FinishCompress
================
*/
void budCompressor_LZW::FinishCompress()
{
	WriteBits( w, codeBits );
	budCompressor_BitStream::FinishCompress();
}

/*
================
budCompressor_LZW::Write
================
*/
int budCompressor_LZW::Write( const void* inData, int inLength )
{
	int i;
	
	InitCompress( inData, inLength );
	
	for( i = 0; i < inLength; i++ )
	{
		int k = ReadBits( 8 );
		
		int code = Lookup( w, k );
		if( code >= 0 )
		{
			w = code;
		}
		else
		{
			WriteBits( w, codeBits );
			if( !BumpBits() )
			{
				AddToDict( w, k );
			}
			w = k;
		}
	}
	
	return inLength;
}


/*
================
budCompressor_LZW::WriteChain
The chain is stored backwards, so we have to write it to a buffer then output the buffer in reverse
================
*/
int budCompressor_LZW::WriteChain( int code )
{
	byte chain[LZW_DICT_SIZE];
	int firstChar = 0;
	int i = 0;
	do
	{
		assert( i < LZW_DICT_SIZE - 1 && code >= 0 );
		chain[i++] = dictionary[code].k;
		code = dictionary[code].w;
	}
	while( code >= 0 );
	firstChar = chain[--i];
	for( ; i >= 0; i-- )
	{
		WriteBits( chain[i], 8 );
	}
	return firstChar;
}

/*
================
budCompressor_LZW::DecompressBlock
================
*/
void budCompressor_LZW::DecompressBlock()
{
	int code, firstChar;
	
	InitDecompress( block, LZW_BLOCK_SIZE );
	
	while( writeByte < writeLength - LZW_DICT_SIZE && readLength > 0 )
	{
		assert( codeBits <= LZW_DICT_BITS );
		
		code = ReadBits( codeBits );
		if( readLength == 0 )
		{
			break;
		}
		
		if( oldCode == -1 )
		{
			assert( code < 256 );
			WriteBits( code, 8 );
			oldCode = code;
			firstChar = code;
			continue;
		}
		
		if( code >= nextCode )
		{
			assert( code == nextCode );
			firstChar = WriteChain( oldCode );
			WriteBits( firstChar, 8 );
		}
		else
		{
			firstChar = WriteChain( code );
		}
		AddToDict( oldCode, firstChar );
		if( BumpBits() )
		{
			oldCode = -1;
		}
		else
		{
			oldCode = code;
		}
	}
	
	blockSize = Min( writeByte, LZW_BLOCK_SIZE );
}

/*
=================================================================================

	budCompressor

=================================================================================
*/

/*
================
budCompressor::AllocNoCompression
================
*/
budCompressor* budCompressor::AllocNoCompression()
{
	return new( TAG_BUDFILE ) budCompressor_None();
}

/*
================
budCompressor::AllocBitStream
================
*/
budCompressor* budCompressor::AllocBitStream()
{
	return new( TAG_BUDFILE ) budCompressor_BitStream();
}

/*
================
budCompressor::AllocRunLength
================
*/
budCompressor* budCompressor::AllocRunLength()
{
	return new( TAG_BUDFILE ) budCompressor_RunLength();
}

/*
================
budCompressor::AllocRunLength_ZeroBased
================
*/
budCompressor* budCompressor::AllocRunLength_ZeroBased()
{
	return new( TAG_BUDFILE ) budCompressor_RunLength_ZeroBased();
}

/*
================
budCompressor::AllocHuffman
================
*/
budCompressor* budCompressor::AllocHuffman()
{
	return new( TAG_BUDFILE ) budCompressor_Huffman();
}

/*
================
budCompressor::AllocArithmetic
================
*/
budCompressor* budCompressor::AllocArithmetic()
{
	return new( TAG_BUDFILE ) budCompressor_Arithmetic();
}

/*
================
budCompressor::AllocLZSS
================
*/
budCompressor* budCompressor::AllocLZSS()
{
	return new( TAG_BUDFILE ) budCompressor_LZSS();
}

/*
================
budCompressor::AllocLZSS_WordAligned
================
*/
budCompressor* budCompressor::AllocLZSS_WordAligned()
{
	return new( TAG_BUDFILE ) budCompressor_LZSS_WordAligned();
}

/*
================
budCompressor::AllocLZW
================
*/
budCompressor* budCompressor::AllocLZW()
{
	return new( TAG_BUDFILE ) budCompressor_LZW();
}
