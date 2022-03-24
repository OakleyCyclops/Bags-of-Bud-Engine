/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2013 Robert Beckebans

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

#ifndef __DRAWVERT_H__
#define __DRAWVERT_H__

// The hardware converts a byte to a float by division with 255 and in the
// vertex programs we convert the floating-point value in the range [0, 1]
// to the range [-1, 1] by multiplying with 2 and subtracting 1.
#define VERTEX_BYTE_TO_FLOAT( x )		( (x) * ( 2.0f / 255.0f ) - 1.0f )
#define VERTEX_FLOAT_TO_BYTE( x )		budMath::Ftob( ( (x) + 1.0f ) * ( 255.0f / 2.0f ) + 0.5f )

// The hardware converts a byte to a float by division with 255 and in the
// fragment programs we convert the floating-point value in the range [0, 1]
// to the range [-1, 1] by multiplying with 2 and subtracting 1.
// This is the conventional OpenGL mapping which specifies an exact
// representation for -1 and +1 but not 0. The DirectX 10 mapping is
// in the comments which specifies a non-linear mapping with an exact
// representation of -1, 0 and +1 but -1 is represented twice.
#define NORMALMAP_BYTE_TO_FLOAT( x )	VERTEX_BYTE_TO_FLOAT( x )	//( (x) - 128.0f ) * ( 1.0f / 127.0f )
#define NORMALMAP_FLOAT_TO_BYTE( x )	VERTEX_FLOAT_TO_BYTE( x )	//budMath::Ftob( 128.0f + 127.0f * (x) + 0.5f )

/*
================================================
halfFloat_t
================================================
*/
typedef unsigned short halfFloat_t;

// GPU half-float bit patterns
#define HF_MANTISSA(x)	(x&1023)
#define HF_EXP(x)		((x&32767)>>10)
#define HF_SIGN(x)		((x&32768)?-1:1)

/*
========================
F16toF32
========================
*/
BUD_INLINE float F16toF32( halfFloat_t x )
{
	int e = HF_EXP( x );
	int m = HF_MANTISSA( x );
	int s = HF_SIGN( x );
	
	if( 0 < e && e < 31 )
	{
		return s * powf( 2.0f, ( e - 15.0f ) ) * ( 1 + m / 1024.0f );
	}
	else if( m == 0 )
	{
		return s * 0.0f;
	}
	return s * powf( 2.0f, -14.0f ) * ( m / 1024.0f );
}

/*
========================
F32toF16
========================
*/
BUD_INLINE halfFloat_t F32toF16( float a )
{
	unsigned int f = *( unsigned* )( &a );
	unsigned int signbit  = ( f & 0x80000000 ) >> 16;
	int exponent = ( ( f & 0x7F800000 ) >> 23 ) - 112;
	unsigned int mantissa = ( f & 0x007FFFFF );
	
	if( exponent <= 0 )
	{
		return 0;
	}
	if( exponent > 30 )
	{
		return ( halfFloat_t )( signbit | 0x7BFF );
	}
	
	return ( halfFloat_t )( signbit | ( exponent << 10 ) | ( mantissa >> 13 ) );
}

/*
===============================================================================

	Draw Vertex.

===============================================================================
*/

class budDrawVert
{
	friend class idSwap;
	friend class idShadowVertSkinned;
	friend class budRenderModelStatic;
	
	friend void TransformVertsAndTangents( budDrawVert* targetVerts, const int numVerts, const budDrawVert* baseVerts, const budJointMat* joints );
	
public:
	budVec3				xyz;			// 12 bytes
private:
	// RB: don't let the old tools code mess with these values
	halfFloat_t			st[2];			// 4 bytes
	byte				normal[4];		// 4 bytes
	byte				tangent[4];		// 4 bytes -- [3] is texture polarity sign
public:
	byte				color[4];		// 4 bytes
	byte				color2[4];		// 4 bytes -- weights for skinning
	
	float				operator[]( const int index ) const;
	float& 				operator[]( const int index );
	
	void				Clear();
	
	const budVec3		GetNormal() const;
	const budVec3		GetNormalRaw() const;		// not re-normalized for renderbump
	
	// must be normalized already!
	void				SetNormal( float x, float y, float z );
	void				SetNormal( const budVec3& n );
	
	const budVec3		GetTangent() const;
	const budVec3		GetTangentRaw() const;		// not re-normalized for renderbump
	
	// must be normalized already!
	void				SetTangent( float x, float y, float z );
	void				SetTangent( const budVec3& t );
	
	// derived from normal, tangent, and tangent flag
	const budVec3 		GetBiTangent() const;
	const budVec3 		GetBiTangentRaw() const;	// not re-normalized for renderbump
	
	void				SetBiTangent( float x, float y, float z );
	BUD_INLINE void		SetBiTangent( const budVec3& t );
	
	float				GetBiTangentSign() const;
	byte				GetBiTangentSignBit() const;
	
	void				SetTexCoordNative( const halfFloat_t s, const halfFloat_t t );
	void				SetTexCoord( const budVec2& st );
	void				SetTexCoord( float s, float t );
	void				SetTexCoordS( float s );
	void				SetTexCoordT( float t );
	const budVec2		GetTexCoord() const;
	const float			GetTexCoordS() const;
	const float			GetTexCoordT() const;
	const halfFloat_t	GetTexCoordNativeS() const;
	const halfFloat_t	GetTexCoordNativeT() const;
	
	// either 1.0f or -1.0f
	BUD_INLINE void		SetBiTangentSign( float sign );
	BUD_INLINE void		SetBiTangentSignBit( byte bit );
	
	void				Lerp( const budDrawVert& a, const budDrawVert& b, const float f );
	void				LerpAll( const budDrawVert& a, const budDrawVert& b, const float f );
	
	void				SetColor( dword color );
	void				SetNativeOrderColor( dword color );
	dword				GetColor() const;
	
	void				SetColor2( dword color );
	void				SetNativeOrderColor2( dword color );
	void				ClearColor2();
	dword				GetColor2() const;
	
	static budDrawVert	GetSkinnedDrawVert( const budDrawVert& vert, const budJointMat* joints );
	static budVec3		GetSkinnedDrawVertPosition( const budDrawVert& vert, const budJointMat* joints );
};

#define DRAWVERT_SIZE				32
#define DRAWVERT_XYZ_OFFSET			(0*4)
#define DRAWVERT_ST_OFFSET			(3*4)
#define DRAWVERT_NORMAL_OFFSET		(4*4)
#define DRAWVERT_TANGENT_OFFSET		(5*4)
#define DRAWVERT_COLOR_OFFSET		(6*4)
#define DRAWVERT_COLOR2_OFFSET		(7*4)

// RB begin
assert_sizeof( budDrawVert, DRAWVERT_SIZE );
#if 0
assert_offsetof( budDrawVert, xyz,		DRAWVERT_XYZ_OFFSET );
assert_offsetof( budDrawVert, normal,	DRAWVERT_NORMAL_OFFSET );
assert_offsetof( budDrawVert, tangent,	DRAWVERT_TANGENT_OFFSET );
#endif
// RB end

/*
========================
VertexFloatToByte

Assumes input is in the range [-1, 1]
========================
*/
BUD_INLINE void VertexFloatToByte( const float& x, const float& y, const float& z, byte* bval )
{
	assert_4_byte_aligned( bval );	// for __stvebx
	
#if defined(USE_INTRINSICS)
	
	const __m128 vector_float_one			= { 1.0f, 1.0f, 1.0f, 1.0f };
	const __m128 vector_float_half			= { 0.5f, 0.5f, 0.5f, 0.5f };
	const __m128 vector_float_255_over_2	= { 255.0f / 2.0f, 255.0f / 2.0f, 255.0f / 2.0f, 255.0f / 2.0f };
	
	const __m128 xyz = _mm_unpacklo_ps( _mm_unpacklo_ps( _mm_load_ss( &x ), _mm_load_ss( &z ) ), _mm_load_ss( &y ) );
	const __m128 xyzScaled = _mm_madd_ps( _mm_add_ps( xyz, vector_float_one ), vector_float_255_over_2, vector_float_half );
	const __m128i xyzInt = _mm_cvtps_epi32( xyzScaled );
	const __m128i xyzShort = _mm_packs_epi32( xyzInt, xyzInt );
	const __m128i xyzChar = _mm_packus_epi16( xyzShort, xyzShort );
	const __m128i xyz16 = _mm_unpacklo_epi8( xyzChar, _mm_setzero_si128() );
	
	bval[0] = ( byte )_mm_extract_epi16( xyz16, 0 );	// cannot use _mm_extract_epi8 because it is an SSE4 instruction
	bval[1] = ( byte )_mm_extract_epi16( xyz16, 1 );
	bval[2] = ( byte )_mm_extract_epi16( xyz16, 2 );
	
#else
	
	bval[0] = VERTEX_FLOAT_TO_BYTE( x );
	bval[1] = VERTEX_FLOAT_TO_BYTE( y );
	bval[2] = VERTEX_FLOAT_TO_BYTE( z );
	
#endif
}

/*
========================
budDrawVert::operator[]
========================
*/
BUD_INLINE float budDrawVert::operator[]( const int index ) const
{
	assert( index >= 0 && index < 5 );
	return ( ( float* )( &xyz ) )[index];
}

/*
========================
budDrawVert::operator[]
========================
*/
BUD_INLINE float&	budDrawVert::operator[]( const int index )
{
	assert( index >= 0 && index < 5 );
	return ( ( float* )( &xyz ) )[index];
}

/*
========================
budDrawVert::Clear
========================
*/
BUD_INLINE void budDrawVert::Clear()
{
	*reinterpret_cast<dword*>( &this->xyz.x ) = 0;
	*reinterpret_cast<dword*>( &this->xyz.y ) = 0;
	*reinterpret_cast<dword*>( &this->xyz.z ) = 0;
	*reinterpret_cast<dword*>( this->st ) = 0;
	*reinterpret_cast<dword*>( this->normal ) = 0x00FF8080;	// x=0, y=0, z=1
	*reinterpret_cast<dword*>( this->tangent ) = 0xFF8080FF;	// x=1, y=0, z=0
	*reinterpret_cast<dword*>( this->color ) = 0;
	*reinterpret_cast<dword*>( this->color2 ) = 0;
}

/*
========================
budDrawVert::GetNormal
========================
*/
BUD_INLINE const budVec3 budDrawVert::GetNormal() const
{
	budVec3 n(	VERTEX_BYTE_TO_FLOAT( normal[0] ),
				VERTEX_BYTE_TO_FLOAT( normal[1] ),
				VERTEX_BYTE_TO_FLOAT( normal[2] ) );
	n.Normalize();	// after the normal has been compressed & uncompressed, it may not be normalized anymore
	return n;
}

/*
========================
budDrawVert::GetNormalRaw
========================
*/
BUD_INLINE const budVec3 budDrawVert::GetNormalRaw() const
{
	budVec3 n(	VERTEX_BYTE_TO_FLOAT( normal[0] ),
				VERTEX_BYTE_TO_FLOAT( normal[1] ),
				VERTEX_BYTE_TO_FLOAT( normal[2] ) );
	// don't re-normalize just like we do in the vertex programs
	return n;
}

/*
========================
budDrawVert::SetNormal
must be normalized already!
========================
*/
BUD_INLINE void budDrawVert::SetNormal( const budVec3& n )
{
	VertexFloatToByte( n.x, n.y, n.z, normal );
}

/*
========================
budDrawVert::SetNormal
========================
*/
BUD_INLINE void budDrawVert::SetNormal( float x, float y, float z )
{
	VertexFloatToByte( x, y, z, normal );
}

/*
========================
&budDrawVert::GetTangent
========================
*/
BUD_INLINE const budVec3 budDrawVert::GetTangent() const
{
	budVec3 t(	VERTEX_BYTE_TO_FLOAT( tangent[0] ),
				VERTEX_BYTE_TO_FLOAT( tangent[1] ),
				VERTEX_BYTE_TO_FLOAT( tangent[2] ) );
	t.Normalize();
	return t;
}

/*
========================
&budDrawVert::GetTangentRaw
========================
*/
BUD_INLINE const budVec3 budDrawVert::GetTangentRaw() const
{
	budVec3 t(	VERTEX_BYTE_TO_FLOAT( tangent[0] ),
				VERTEX_BYTE_TO_FLOAT( tangent[1] ),
				VERTEX_BYTE_TO_FLOAT( tangent[2] ) );
	// don't re-normalize just like we do in the vertex programs
	return t;
}

/*
========================
budDrawVert::SetTangent
========================
*/
BUD_INLINE void budDrawVert::SetTangent( float x, float y, float z )
{
	VertexFloatToByte( x, y, z, tangent );
}

/*
========================
budDrawVert::SetTangent
========================
*/
BUD_INLINE void budDrawVert::SetTangent( const budVec3& t )
{
	VertexFloatToByte( t.x, t.y, t.z, tangent );
}

/*
========================
budDrawVert::GetBiTangent
========================
*/
BUD_INLINE const budVec3 budDrawVert::GetBiTangent() const
{
	// derive from the normal, tangent, and bitangent direction flag
	budVec3 bitangent;
	bitangent.Cross( GetNormal(), GetTangent() );
	bitangent *= GetBiTangentSign();
	return bitangent;
}

/*
========================
budDrawVert::GetBiTangentRaw
========================
*/
BUD_INLINE const budVec3 budDrawVert::GetBiTangentRaw() const
{
	// derive from the normal, tangent, and bitangent direction flag
	// don't re-normalize just like we do in the vertex programs
	budVec3 bitangent;
	bitangent.Cross( GetNormalRaw(), GetTangentRaw() );
	bitangent *= GetBiTangentSign();
	return bitangent;
}

/*
========================
budDrawVert::SetBiTangent
========================
*/
BUD_INLINE void budDrawVert::SetBiTangent( float x, float y, float z )
{
	SetBiTangent( budVec3( x, y, z ) );
}

/*
========================
budDrawVert::SetBiTangent
========================
*/
BUD_INLINE void budDrawVert::SetBiTangent( const budVec3& t )
{
	budVec3 bitangent;
	bitangent.Cross( GetNormal(), GetTangent() );
	SetBiTangentSign( bitangent * t );
}

/*
========================
budDrawVert::GetBiTangentSign
========================
*/
BUD_INLINE float budDrawVert::GetBiTangentSign() const
{
	return ( tangent[3] < 128 ) ? -1.0f : 1.0f;
}

/*
========================
budDrawVert::GetBiTangentSignBit
========================
*/
BUD_INLINE byte budDrawVert::GetBiTangentSignBit() const
{
	return ( tangent[3] < 128 ) ? 1 : 0;
}

/*
========================
budDrawVert::SetBiTangentSign
========================
*/
BUD_INLINE void budDrawVert::SetBiTangentSign( float sign )
{
	tangent[3] = ( sign < 0.0f ) ? 0 : 255;
}

/*
========================
budDrawVert::SetBiTangentSignBit
========================
*/
BUD_INLINE void budDrawVert::SetBiTangentSignBit( byte sign )
{
	tangent[3] = sign ? 0 : 255;
}

/*
========================
budDrawVert::Lerp
========================
*/
BUD_INLINE void budDrawVert::Lerp( const budDrawVert& a, const budDrawVert& b, const float f )
{
	xyz = a.xyz + f * ( b.xyz - a.xyz );
	SetTexCoord( ::Lerp( a.GetTexCoord(), b.GetTexCoord(), f ) );
}

/*
========================
budDrawVert::LerpAll
========================
*/
BUD_INLINE void budDrawVert::LerpAll( const budDrawVert& a, const budDrawVert& b, const float f )
{
	xyz = ::Lerp( a.xyz, b.xyz, f );
	SetTexCoord( ::Lerp( a.GetTexCoord(), b.GetTexCoord(), f ) );
	
	budVec3 normal = ::Lerp( a.GetNormal(), b.GetNormal(), f );
	budVec3 tangent = ::Lerp( a.GetTangent(), b.GetTangent(), f );
	budVec3 bitangent = ::Lerp( a.GetBiTangent(), b.GetBiTangent(), f );
	normal.Normalize();
	tangent.Normalize();
	bitangent.Normalize();
	SetNormal( normal );
	SetTangent( tangent );
	SetBiTangent( bitangent );
	
	color[0] = ( byte )( a.color[0] + f * ( b.color[0] - a.color[0] ) );
	color[1] = ( byte )( a.color[1] + f * ( b.color[1] - a.color[1] ) );
	color[2] = ( byte )( a.color[2] + f * ( b.color[2] - a.color[2] ) );
	color[3] = ( byte )( a.color[3] + f * ( b.color[3] - a.color[3] ) );
	
	color2[0] = ( byte )( a.color2[0] + f * ( b.color2[0] - a.color2[0] ) );
	color2[1] = ( byte )( a.color2[1] + f * ( b.color2[1] - a.color2[1] ) );
	color2[2] = ( byte )( a.color2[2] + f * ( b.color2[2] - a.color2[2] ) );
	color2[3] = ( byte )( a.color2[3] + f * ( b.color2[3] - a.color2[3] ) );
}

/*
========================
budDrawVert::SetNativeOrderColor
========================
*/
BUD_INLINE void budDrawVert::SetNativeOrderColor( dword color )
{
	*reinterpret_cast<dword*>( this->color ) = color;
}

/*
========================
budDrawVert::SetColor
========================
*/
BUD_INLINE void budDrawVert::SetColor( dword color )
{
	*reinterpret_cast<dword*>( this->color ) = color;
}

/*
========================
budDrawVert::SetColor
========================
*/
BUD_INLINE dword budDrawVert::GetColor() const
{
	return *reinterpret_cast<const dword*>( this->color );
}

/*
========================
budDrawVert::SetTexCoordNative
========================
*/
BUD_INLINE void budDrawVert::SetTexCoordNative( const halfFloat_t s, const halfFloat_t t )
{
	st[0] = s;
	st[1] = t;
}

/*
========================
budDrawVert::SetTexCoord
========================
*/
BUD_INLINE void budDrawVert::SetTexCoord( const budVec2& st )
{
	SetTexCoordS( st.x );
	SetTexCoordT( st.y );
}

/*
========================
budDrawVert::SetTexCoord
========================
*/
BUD_INLINE void budDrawVert::SetTexCoord( float s, float t )
{
	SetTexCoordS( s );
	SetTexCoordT( t );
}

/*
========================
budDrawVert::SetTexCoordS
========================
*/
BUD_INLINE void budDrawVert::SetTexCoordS( float s )
{
	st[0] = F32toF16( s );
}

/*
========================
budDrawVert::SetTexCoordT
========================
*/
BUD_INLINE void budDrawVert::SetTexCoordT( float t )
{
	st[1] = F32toF16( t );
}

/*
========================
budDrawVert::GetTexCoord
========================
*/
BUD_INLINE const budVec2	budDrawVert::GetTexCoord() const
{
	return budVec2( F16toF32( st[0] ), F16toF32( st[1] ) );
}

/*
========================
budDrawVert::GetTexCoordT
========================
*/
BUD_INLINE const float budDrawVert::GetTexCoordS() const
{
	return F16toF32( st[0] );
}

/*
========================
budDrawVert::GetTexCoordS
========================
*/
BUD_INLINE const float budDrawVert::GetTexCoordT() const
{
	return F16toF32( st[1] );
}

/*
========================
budDrawVert::GetTexCoordNativeS
========================
*/
BUD_INLINE const halfFloat_t budDrawVert::GetTexCoordNativeS() const
{
	return st[0];
}

/*
========================
budDrawVert::GetTexCoordNativeT
========================
*/
BUD_INLINE const halfFloat_t budDrawVert::GetTexCoordNativeT() const
{
	return st[1];
}

/*
========================
budDrawVert::SetNativeOrderColor2
========================
*/
BUD_INLINE void budDrawVert::SetNativeOrderColor2( dword color2 )
{
	*reinterpret_cast<dword*>( this->color2 ) = color2;
}

/*
========================
budDrawVert::SetColor
========================
*/
BUD_INLINE void budDrawVert::SetColor2( dword color2 )
{
	*reinterpret_cast<dword*>( this->color2 ) = color2;
}

/*
========================
budDrawVert::ClearColor2
========================
*/
BUD_INLINE void budDrawVert::ClearColor2()
{
	*reinterpret_cast<dword*>( this->color2 ) = 0x80808080;
}

/*
========================
budDrawVert::GetColor2
========================
*/
BUD_INLINE dword budDrawVert::GetColor2() const
{
	return *reinterpret_cast<const dword*>( this->color2 );
}

/*
========================
WriteDrawVerts16

Use 16-byte in-order SIMD writes because the destVerts may live in write-combined memory
========================
*/
BUD_INLINE void WriteDrawVerts16( budDrawVert* destVerts, const budDrawVert* localVerts, int numVerts )
{
	assert_sizeof( budDrawVert, 32 );
	assert_16_byte_aligned( destVerts );
	assert_16_byte_aligned( localVerts );
	
#if defined(USE_INTRINSICS)
	
	for( int i = 0; i < numVerts; i++ )
	{
		__m128i v0 = _mm_load_si128( ( const __m128i* )( ( byte* )( localVerts + i ) +  0 ) );
		__m128i v1 = _mm_load_si128( ( const __m128i* )( ( byte* )( localVerts + i ) + 16 ) );
		_mm_stream_si128( ( __m128i* )( ( byte* )( destVerts + i ) +  0 ), v0 );
		_mm_stream_si128( ( __m128i* )( ( byte* )( destVerts + i ) + 16 ), v1 );
	}
	
#else
	
	memcpy( destVerts, localVerts, numVerts * sizeof( budDrawVert ) );
	
#endif
}

/*
=====================
budDrawVert::GetSkinnedDrawVert
=====================
*/
BUD_INLINE budDrawVert budDrawVert::GetSkinnedDrawVert( const budDrawVert& vert, const budJointMat* joints )
{
	if( joints == NULL )
	{
		return vert;
	}
	
	const budJointMat& j0 = joints[vert.color[0]];
	const budJointMat& j1 = joints[vert.color[1]];
	const budJointMat& j2 = joints[vert.color[2]];
	const budJointMat& j3 = joints[vert.color[3]];
	
	const float w0 = vert.color2[0] * ( 1.0f / 255.0f );
	const float w1 = vert.color2[1] * ( 1.0f / 255.0f );
	const float w2 = vert.color2[2] * ( 1.0f / 255.0f );
	const float w3 = vert.color2[3] * ( 1.0f / 255.0f );
	
	budJointMat accum;
	budJointMat::Mul( accum, j0, w0 );
	budJointMat::Mad( accum, j1, w1 );
	budJointMat::Mad( accum, j2, w2 );
	budJointMat::Mad( accum, j3, w3 );
	
	budDrawVert outVert;
	outVert.xyz = accum * budVec4( vert.xyz.x, vert.xyz.y, vert.xyz.z, 1.0f );
	outVert.SetTexCoordNative( vert.GetTexCoordNativeS(), vert.GetTexCoordNativeT() );
	outVert.SetNormal( accum * vert.GetNormal() );
	outVert.SetTangent( accum * vert.GetTangent() );
	outVert.tangent[3] = vert.tangent[3];
	for( int i = 0; i < 4; i++ )
	{
		outVert.color[i] = vert.color[i];
		outVert.color2[i] = vert.color2[i];
	}
	return outVert;
}

/*
=====================
budDrawVert::GetSkinnedDrawVertPosition
=====================
*/
BUD_INLINE budVec3 budDrawVert::GetSkinnedDrawVertPosition( const budDrawVert& vert, const budJointMat* joints )
{
	if( joints == NULL )
	{
		return vert.xyz;
	}
	
	const budJointMat& j0 = joints[vert.color[0]];
	const budJointMat& j1 = joints[vert.color[1]];
	const budJointMat& j2 = joints[vert.color[2]];
	const budJointMat& j3 = joints[vert.color[3]];
	
	const float w0 = vert.color2[0] * ( 1.0f / 255.0f );
	const float w1 = vert.color2[1] * ( 1.0f / 255.0f );
	const float w2 = vert.color2[2] * ( 1.0f / 255.0f );
	const float w3 = vert.color2[3] * ( 1.0f / 255.0f );
	
	budJointMat accum;
	budJointMat::Mul( accum, j0, w0 );
	budJointMat::Mad( accum, j1, w1 );
	budJointMat::Mad( accum, j2, w2 );
	budJointMat::Mad( accum, j3, w3 );
	
	return accum * budVec4( vert.xyz.x, vert.xyz.y, vert.xyz.z, 1.0f );
}

/*
===============================================================================
Shadow Vertex
===============================================================================
*/
class idShadowVert
{
public:
	budVec4			xyzw;
	
	void			Clear();
	static int		CreateShadowCache( idShadowVert* vertexCache, const budDrawVert* verts, const int numVerts );
};

#define SHADOWVERT_XYZW_OFFSET		(0)

assert_offsetof( idShadowVert, xyzw, SHADOWVERT_XYZW_OFFSET );

BUD_INLINE void idShadowVert::Clear()
{
	xyzw.Zero();
}

/*
===============================================================================
Skinned Shadow Vertex
===============================================================================
*/
class idShadowVertSkinned
{
public:
	budVec4			xyzw;
	byte			color[4];
	byte			color2[4];
	byte			pad[8];		// pad to multiple of 32-byte for glDrawElementsBaseVertex
	
	void			Clear();
	static int		CreateShadowCache( idShadowVertSkinned* vertexCache, const budDrawVert* verts, const int numVerts );
};

#define SHADOWVERTSKINNED_XYZW_OFFSET		(0)
#define SHADOWVERTSKINNED_COLOR_OFFSET		(16)
#define SHADOWVERTSKINNED_COLOR2_OFFSET		(20)

assert_offsetof( idShadowVertSkinned, xyzw, SHADOWVERTSKINNED_XYZW_OFFSET );
assert_offsetof( idShadowVertSkinned, color, SHADOWVERTSKINNED_COLOR_OFFSET );
assert_offsetof( idShadowVertSkinned, color2, SHADOWVERTSKINNED_COLOR2_OFFSET );

BUD_INLINE void idShadowVertSkinned::Clear()
{
	xyzw.Zero();
}

#endif /* !__DRAWVERT_H__ */
