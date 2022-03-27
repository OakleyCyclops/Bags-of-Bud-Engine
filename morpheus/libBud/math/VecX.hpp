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

#ifndef __MATH_VECX_H__
#define __MATH_VECX_H__

/*
===============================================================================

budVecX - arbitrary sized vector

The vector lives on 16 byte aligned and 16 byte padded memory.

NOTE: due to the temporary memory pool budVecX cannot be used by multiple threads

===============================================================================
*/

#define VECX_MAX_TEMP		1024
#define VECX_QUAD( x )		( ( ( ( x ) + 3 ) & ~3 ) * sizeof( float ) )
#define VECX_CLEAREND()		int s = size; while( s < ( ( s + 3) & ~3 ) ) { p[s++] = 0.0f; }
#define VECX_ALLOCA( n )	( (float *) _alloca16( VECX_QUAD( n ) ) )

#if defined(USE_INTRINSICS)
#define VECX_SIMD
#endif

class budVecX
{
	friend class budMatX;
	
public:
	BUD_INLINE					budVecX();
	BUD_INLINE					explicit budVecX( int length );
	BUD_INLINE					explicit budVecX( int length, float* data );
	BUD_INLINE					~budVecX();
	
	BUD_INLINE	float			Get( int index ) const;
	BUD_INLINE	float& 			Get( int index );
	
	BUD_INLINE	float			operator[]( const int index ) const;
	BUD_INLINE	float& 			operator[]( const int index );
	BUD_INLINE	budVecX			operator-() const;
	BUD_INLINE	budVecX& 		operator=( const budVecX& a );
	BUD_INLINE	budVecX			operator*( const float a ) const;
	BUD_INLINE	budVecX			operator/( const float a ) const;
	BUD_INLINE	float			operator*( const budVecX& a ) const;
	BUD_INLINE	budVecX			operator-( const budVecX& a ) const;
	BUD_INLINE	budVecX			operator+( const budVecX& a ) const;
	BUD_INLINE	budVecX& 		operator*=( const float a );
	BUD_INLINE	budVecX& 		operator/=( const float a );
	BUD_INLINE	budVecX& 		operator+=( const budVecX& a );
	BUD_INLINE	budVecX& 		operator-=( const budVecX& a );
	
	friend BUD_INLINE	budVecX	operator*( const float a, const budVecX& b );
	
	BUD_INLINE	bool			Compare( const budVecX& a ) const;							// exact compare, no epsilon
	BUD_INLINE	bool			Compare( const budVecX& a, const float epsilon ) const;		// compare with epsilon
	BUD_INLINE	bool			operator==(	const budVecX& a ) const;						// exact compare, no epsilon
	BUD_INLINE	bool			operator!=(	const budVecX& a ) const;						// exact compare, no epsilon
	
	BUD_INLINE	void			SetSize( int size );
	BUD_INLINE	void			ChangeSize( int size, bool makeZero = false );
	BUD_INLINE	int				GetSize() const
	{
		return size;
	}
	BUD_INLINE	void			SetData( int length, float* data );
	BUD_INLINE	void			Zero();
	BUD_INLINE	void			Zero( int length );
	BUD_INLINE	void			Random( int seed, float l = 0.0f, float u = 1.0f );
	BUD_INLINE	void			Random( int length, int seed, float l = 0.0f, float u = 1.0f );
	BUD_INLINE	void			Negate();
	BUD_INLINE	void			Clamp( float min, float max );
	BUD_INLINE	budVecX& 		SwapElements( int e1, int e2 );
	
	BUD_INLINE	float			Length() const;
	BUD_INLINE	float			LengthSqr() const;
	BUD_INLINE	budVecX			Normalize() const;
	BUD_INLINE	float			NormalizeSelf();
	
	BUD_INLINE	int				GetDimension() const;
	
	BUD_INLINE	void			AddScaleAdd( const float scale, const budVecX& v0, const budVecX& v1 );
	
	BUD_INLINE	const budVec3& 	SubVec3( int index ) const;
	BUD_INLINE	budVec3& 		SubVec3( int index );
	BUD_INLINE	const budVec6& 	SubVec6( int index = 0 ) const;
	BUD_INLINE	budVec6& 		SubVec6( int index = 0 );
	BUD_INLINE	const float* 	ToFloatPtr() const;
	BUD_INLINE	float* 			ToFloatPtr();
	const char* 	ToString( int precision = 2 ) const;
	
private:
	int				size;					// size of the vector
	int				alloced;				// if -1 p points to data set with SetData
	float* 			p;						// memory the vector is stored
	
	static float	temp[VECX_MAX_TEMP + 4];	// used to store intermediate results
	static float* 	tempPtr;				// pointer to 16 byte aligned temporary memory
	static int		tempIndex;				// index into memory pool, wraps around
	
	BUD_INLINE void	SetTempSize( int size );
};


/*
========================
budVecX::budVecX
========================
*/
BUD_INLINE budVecX::budVecX()
{
	size = alloced = 0;
	p = NULL;
}

/*
========================
budVecX::budVecX
========================
*/
BUD_INLINE budVecX::budVecX( int length )
{
	size = alloced = 0;
	p = NULL;
	SetSize( length );
}

/*
========================
budVecX::budVecX
========================
*/
BUD_INLINE budVecX::budVecX( int length, float* data )
{
	size = alloced = 0;
	p = NULL;
	SetData( length, data );
}

/*
========================
budVecX::~budVecX
========================
*/
BUD_INLINE budVecX::~budVecX()
{
	// if not temp memory
	if( p && ( p < budVecX::tempPtr || p >= budVecX::tempPtr + VECX_MAX_TEMP ) && alloced != -1 )
	{
		Mem_Free16( p );
	}
}

/*
========================
budVecX::Get
========================
*/
BUD_INLINE float budVecX::Get( int index ) const
{
	assert( index >= 0 && index < size );
	return p[index];
}

/*
========================
budVecX::Get
========================
*/
BUD_INLINE float& budVecX::Get( int index )
{
	assert( index >= 0 && index < size );
	return p[index];
}

/*
========================
budVecX::operator[]
========================
*/
BUD_INLINE float budVecX::operator[]( int index ) const
{
	return Get( index );
}

/*
========================
budVecX::operator[]
========================
*/
BUD_INLINE float& budVecX::operator[]( int index )
{
	return Get( index );
}

/*
========================
budVecX::operator-
========================
*/
BUD_INLINE budVecX budVecX::operator-() const
{
	budVecX m;
	
	m.SetTempSize( size );
#ifdef VECX_SIMD
	ALIGN16( unsigned int signBit[4] ) = { IEEE_FLT_SIGN_MASK, IEEE_FLT_SIGN_MASK, IEEE_FLT_SIGN_MASK, IEEE_FLT_SIGN_MASK };
	for( int i = 0; i < size; i += 4 )
	{
		_mm_store_ps( m.p + i, _mm_xor_ps( _mm_load_ps( p + i ), ( __m128& ) signBit[0] ) );
	}
#else
	for( int i = 0; i < size; i++ )
	{
		m.p[i] = -p[i];
	}
#endif
	return m;
}

/*
========================
budVecX::operator=
========================
*/
BUD_INLINE budVecX& budVecX::operator=( const budVecX& a )
{
	SetSize( a.size );
#ifdef VECX_SIMD
	for( int i = 0; i < a.size; i += 4 )
	{
		_mm_store_ps( p + i, _mm_load_ps( a.p + i ) );
	}
#else
	memcpy( p, a.p, a.size * sizeof( float ) );
#endif
	budVecX::tempIndex = 0;
	return *this;
}

/*
========================
budVecX::operator+
========================
*/
BUD_INLINE budVecX budVecX::operator+( const budVecX& a ) const
{
	budVecX m;
	
	assert( size == a.size );
	m.SetTempSize( size );
#ifdef VECX_SIMD
	for( int i = 0; i < size; i += 4 )
	{
		_mm_store_ps( m.p + i, _mm_add_ps( _mm_load_ps( p + i ), _mm_load_ps( a.p + i ) ) );
	}
#else
	for( int i = 0; i < size; i++ )
	{
		m.p[i] = p[i] + a.p[i];
	}
#endif
	return m;
}

/*
========================
budVecX::operator-
========================
*/
BUD_INLINE budVecX budVecX::operator-( const budVecX& a ) const
{
	budVecX m;
	
	assert( size == a.size );
	m.SetTempSize( size );
#ifdef VECX_SIMD
	for( int i = 0; i < size; i += 4 )
	{
		_mm_store_ps( m.p + i, _mm_sub_ps( _mm_load_ps( p + i ), _mm_load_ps( a.p + i ) ) );
	}
#else
	for( int i = 0; i < size; i++ )
	{
		m.p[i] = p[i] - a.p[i];
	}
#endif
	return m;
}

/*
========================
budVecX::operator+=
========================
*/
BUD_INLINE budVecX& budVecX::operator+=( const budVecX& a )
{
	assert( size == a.size );
#ifdef VECX_SIMD
	for( int i = 0; i < size; i += 4 )
	{
		_mm_store_ps( p + i, _mm_add_ps( _mm_load_ps( p + i ), _mm_load_ps( a.p + i ) ) );
	}
#else
	for( int i = 0; i < size; i++ )
	{
		p[i] += a.p[i];
	}
#endif
	budVecX::tempIndex = 0;
	return *this;
}

/*
========================
budVecX::operator-=
========================
*/
BUD_INLINE budVecX& budVecX::operator-=( const budVecX& a )
{
	assert( size == a.size );
#ifdef VECX_SIMD
	for( int i = 0; i < size; i += 4 )
	{
		_mm_store_ps( p + i, _mm_sub_ps( _mm_load_ps( p + i ), _mm_load_ps( a.p + i ) ) );
	}
#else
	for( int i = 0; i < size; i++ )
	{
		p[i] -= a.p[i];
	}
#endif
	budVecX::tempIndex = 0;
	return *this;
}

/*
========================
budVecX::operator*
========================
*/
BUD_INLINE budVecX budVecX::operator*( const float a ) const
{
	budVecX m;
	
	m.SetTempSize( size );
#ifdef VECX_SIMD
	__m128 va = _mm_load1_ps( & a );
	for( int i = 0; i < size; i += 4 )
	{
		_mm_store_ps( m.p + i, _mm_mul_ps( _mm_load_ps( p + i ), va ) );
	}
#else
	for( int i = 0; i < size; i++ )
	{
		m.p[i] = p[i] * a;
	}
#endif
	return m;
}

/*
========================
budVecX::operator*=
========================
*/
BUD_INLINE budVecX& budVecX::operator*=( const float a )
{
#ifdef VECX_SIMD
	__m128 va = _mm_load1_ps( & a );
	for( int i = 0; i < size; i += 4 )
	{
		_mm_store_ps( p + i, _mm_mul_ps( _mm_load_ps( p + i ), va ) );
	}
#else
	for( int i = 0; i < size; i++ )
	{
		p[i] *= a;
	}
#endif
	return *this;
}

/*
========================
budVecX::operator/
========================
*/
BUD_INLINE budVecX budVecX::operator/( const float a ) const
{
	assert( fabs( a ) > budMath::FLT_SMALLEST_NON_DENORMAL );
	return ( *this ) * ( 1.0f / a );
}

/*
========================
budVecX::operator/=
========================
*/
BUD_INLINE budVecX& budVecX::operator/=( const float a )
{
	assert( fabs( a ) > budMath::FLT_SMALLEST_NON_DENORMAL );
	( *this ) *= ( 1.0f / a );
	return *this;
}

/*
========================
operator*
========================
*/
BUD_INLINE budVecX operator*( const float a, const budVecX& b )
{
	return b * a;
}

/*
========================
budVecX::operator*
========================
*/
BUD_INLINE float budVecX::operator*( const budVecX& a ) const
{
	assert( size == a.size );
	float sum = 0.0f;
	for( int i = 0; i < size; i++ )
	{
		sum += p[i] * a.p[i];
	}
	return sum;
}

/*
========================
budVecX::Compare
========================
*/
BUD_INLINE bool budVecX::Compare( const budVecX& a ) const
{
	assert( size == a.size );
	for( int i = 0; i < size; i++ )
	{
		if( p[i] != a.p[i] )
		{
			return false;
		}
	}
	return true;
}

/*
========================
budVecX::Compare
========================
*/
BUD_INLINE bool budVecX::Compare( const budVecX& a, const float epsilon ) const
{
	assert( size == a.size );
	for( int i = 0; i < size; i++ )
	{
		if( budMath::Fabs( p[i] - a.p[i] ) > epsilon )
		{
			return false;
		}
	}
	return true;
}

/*
========================
budVecX::operator==
========================
*/
BUD_INLINE bool budVecX::operator==( const budVecX& a ) const
{
	return Compare( a );
}

/*
========================
budVecX::operator!=
========================
*/
BUD_INLINE bool budVecX::operator!=( const budVecX& a ) const
{
	return !Compare( a );
}

/*
========================
budVecX::SetSize
========================
*/
BUD_INLINE void budVecX::SetSize( int newSize )
{
	//assert( p < budVecX::tempPtr || p > budVecX::tempPtr + VECX_MAX_TEMP );
	if( newSize != size || p == NULL )
	{
		int alloc = ( newSize + 3 ) & ~3;
		if( alloc > alloced && alloced != -1 )
		{
			if( p )
			{
				Mem_Free16( p );
			}
			p = ( float* ) Mem_Alloc16( alloc * sizeof( float ), TAG_MATH );
			alloced = alloc;
		}
		size = newSize;
		VECX_CLEAREND();
	}
}

/*
========================
budVecX::ChangeSize
========================
*/
BUD_INLINE void budVecX::ChangeSize( int newSize, bool makeZero )
{
	if( newSize != size )
	{
		int alloc = ( newSize + 3 ) & ~3;
		if( alloc > alloced && alloced != -1 )
		{
			float* oldVec = p;
			p = ( float* ) Mem_Alloc16( alloc * sizeof( float ), TAG_MATH );
			alloced = alloc;
			if( oldVec )
			{
				for( int i = 0; i < size; i++ )
				{
					p[i] = oldVec[i];
				}
				Mem_Free16( oldVec );
			}
			if( makeZero )
			{
				// zero any new elements
				for( int i = size; i < newSize; i++ )
				{
					p[i] = 0.0f;
				}
			}
		}
		size = newSize;
		VECX_CLEAREND();
	}
}

/*
========================
budVecX::SetTempSize
========================
*/
BUD_INLINE void budVecX::SetTempSize( int newSize )
{
	size = newSize;
	alloced = ( newSize + 3 ) & ~3;
	assert( alloced < VECX_MAX_TEMP );
	if( budVecX::tempIndex + alloced > VECX_MAX_TEMP )
	{
		budVecX::tempIndex = 0;
	}
	p = budVecX::tempPtr + budVecX::tempIndex;
	budVecX::tempIndex += alloced;
	VECX_CLEAREND();
}

/*
========================
budVecX::SetData
========================
*/
BUD_INLINE void budVecX::SetData( int length, float* data )
{
	if( p != NULL && ( p < budVecX::tempPtr || p >= budVecX::tempPtr + VECX_MAX_TEMP ) && alloced != -1 )
	{
		Mem_Free16( p );
	}
	assert_16_byte_aligned( data ); // data must be 16 byte aligned
	p = data;
	size = length;
	alloced = -1;
	VECX_CLEAREND();
}

/*
========================
budVecX::Zero
========================
*/
BUD_INLINE void budVecX::Zero()
{
#ifdef VECX_SIMD
	for( int i = 0; i < size; i += 4 )
	{
		_mm_store_ps( p + i, _mm_setzero_ps() );
	}
#else
	memset( p, 0, size * sizeof( float ) );
#endif
}

/*
========================
budVecX::Zero
========================
*/
BUD_INLINE void budVecX::Zero( int length )
{
	SetSize( length );
#ifdef VECX_SIMD
	for( int i = 0; i < length; i += 4 )
	{
		_mm_store_ps( p + i, _mm_setzero_ps() );
	}
#else
	memset( p, 0, length * sizeof( float ) );
#endif
}

/*
========================
budVecX::Random
========================
*/
BUD_INLINE void budVecX::Random( int seed, float l, float u )
{
	idRandom rnd( seed );
	
	float c = u - l;
	for( int i = 0; i < size; i++ )
	{
		p[i] = l + rnd.RandomFloat() * c;
	}
}

/*
========================
budVecX::Random
========================
*/
BUD_INLINE void budVecX::Random( int length, int seed, float l, float u )
{
	idRandom rnd( seed );
	
	SetSize( length );
	float c = u - l;
	for( int i = 0; i < size; i++ )
	{
		p[i] = l + rnd.RandomFloat() * c;
	}
}

/*
========================
budVecX::Negate
========================
*/
BUD_INLINE void budVecX::Negate()
{
#ifdef VECX_SIMD
	ALIGN16( const unsigned int signBit[4] ) = { IEEE_FLT_SIGN_MASK, IEEE_FLT_SIGN_MASK, IEEE_FLT_SIGN_MASK, IEEE_FLT_SIGN_MASK };
	for( int i = 0; i < size; i += 4 )
	{
		_mm_store_ps( p + i, _mm_xor_ps( _mm_load_ps( p + i ), ( __m128& ) signBit[0] ) );
	}
#else
	for( int i = 0; i < size; i++ )
	{
		p[i] = -p[i];
	}
#endif
}

/*
========================
budVecX::Clamp
========================
*/
BUD_INLINE void budVecX::Clamp( float min, float max )
{
	for( int i = 0; i < size; i++ )
	{
		if( p[i] < min )
		{
			p[i] = min;
		}
		else if( p[i] > max )
		{
			p[i] = max;
		}
	}
}

/*
========================
budVecX::SwapElements
========================
*/
BUD_INLINE budVecX& budVecX::SwapElements( int e1, int e2 )
{
	float tmp;
	tmp = p[e1];
	p[e1] = p[e2];
	p[e2] = tmp;
	return *this;
}

/*
========================
budVecX::Length
========================
*/
BUD_INLINE float budVecX::Length() const
{
	float sum = 0.0f;
	for( int i = 0; i < size; i++ )
	{
		sum += p[i] * p[i];
	}
	return budMath::Sqrt( sum );
}

/*
========================
budVecX::LengthSqr
========================
*/
BUD_INLINE float budVecX::LengthSqr() const
{
	float sum = 0.0f;
	for( int i = 0; i < size; i++ )
	{
		sum += p[i] * p[i];
	}
	return sum;
}

/*
========================
budVecX::Normalize
========================
*/
BUD_INLINE budVecX budVecX::Normalize() const
{
	budVecX m;
	
	m.SetTempSize( size );
	float sum = 0.0f;
	for( int i = 0; i < size; i++ )
	{
		sum += p[i] * p[i];
	}
	float invSqrt = budMath::InvSqrt( sum );
	for( int i = 0; i < size; i++ )
	{
		m.p[i] = p[i] * invSqrt;
	}
	return m;
}

/*
========================
budVecX::NormalizeSelf
========================
*/
BUD_INLINE float budVecX::NormalizeSelf()
{
	float sum = 0.0f;
	for( int i = 0; i < size; i++ )
	{
		sum += p[i] * p[i];
	}
	float invSqrt = budMath::InvSqrt( sum );
	for( int i = 0; i < size; i++ )
	{
		p[i] *= invSqrt;
	}
	return invSqrt * sum;
}

/*
========================
budVecX::GetDimension
========================
*/
BUD_INLINE int budVecX::GetDimension() const
{
	return size;
}

/*
========================
budVecX::SubVec3
========================
*/
BUD_INLINE budVec3& budVecX::SubVec3( int index )
{
	assert( index >= 0 && index * 3 + 3 <= size );
	return *reinterpret_cast<budVec3*>( p + index * 3 );
}

/*
========================
budVecX::SubVec3
========================
*/
BUD_INLINE const budVec3& budVecX::SubVec3( int index ) const
{
	assert( index >= 0 && index * 3 + 3 <= size );
	return *reinterpret_cast<const budVec3*>( p + index * 3 );
}

/*
========================
budVecX::SubVec6
========================
*/
BUD_INLINE budVec6& budVecX::SubVec6( int index )
{
	assert( index >= 0 && index * 6 + 6 <= size );
	return *reinterpret_cast<budVec6*>( p + index * 6 );
}

/*
========================
budVecX::SubVec6
========================
*/
BUD_INLINE const budVec6& budVecX::SubVec6( int index ) const
{
	assert( index >= 0 && index * 6 + 6 <= size );
	return *reinterpret_cast<const budVec6*>( p + index * 6 );
}

/*
========================
budVecX::ToFloatPtr
========================
*/
BUD_INLINE const float* budVecX::ToFloatPtr() const
{
	return p;
}

/*
========================
budVecX::ToFloatPtr
========================
*/
BUD_INLINE float* budVecX::ToFloatPtr()
{
	return p;
}

/*
========================
budVecX::AddScaleAdd
========================
*/
BUD_INLINE void budVecX::AddScaleAdd( const float scale, const budVecX& v0, const budVecX& v1 )
{
	assert( GetSize() == v0.GetSize() );
	assert( GetSize() == v1.GetSize() );
	
	const float* v0Ptr = v0.ToFloatPtr();
	const float* v1Ptr = v1.ToFloatPtr();
	float* dstPtr = ToFloatPtr();
	
	for( int i = 0; i < size; i++ )
	{
		dstPtr[i] += scale * ( v0Ptr[i] + v1Ptr[i] );
	}
}

#endif // !__MATH_VECTORX_H__
