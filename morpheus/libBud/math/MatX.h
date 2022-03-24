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

#ifndef __MATH_MATX_H__
#define __MATH_MATX_H__

/*
===============================================================================

budMatX - arbitrary sized dense real matrix

The matrix lives on 16 byte aligned and 16 byte padded memory.

NOTE: due to the temporary memory pool budMatX cannot be used by multiple threads.

===============================================================================
*/

#define MATX_MAX_TEMP		1024
#define MATX_QUAD( x )		( ( ( ( x ) + 3 ) & ~3 ) * sizeof( float ) )
#define MATX_CLEAREND()		int s = numRows * numColumns; while( s < ( ( s + 3 ) & ~3 ) ) { mat[s++] = 0.0f; }
#define MATX_ALLOCA( n )	( (float *) _alloca16( MATX_QUAD( n ) ) )
#define MATX_ALLOCA_CACHE_LINES( n )	( (float *) _alloca128( ( ( n ) * sizeof( float ) + CACHE_LINE_SIZE - 1 ) & ~ ( CACHE_LINE_SIZE - 1 ) ) )

#if defined(USE_INTRINSICS)
#define MATX_SIMD
#endif

class budMatX
{
public:
	BUD_INLINE					budMatX();
	BUD_INLINE					budMatX( const budMatX& other );
	BUD_INLINE					explicit budMatX( int rows, int columns );
	BUD_INLINE					explicit budMatX( int rows, int columns, float* src );
	BUD_INLINE					~budMatX();
	
	BUD_INLINE	void			Set( int rows, int columns, const float* src );
	BUD_INLINE	void			Set( const budMat3& m1, const budMat3& m2 );
	BUD_INLINE	void			Set( const budMat3& m1, const budMat3& m2, const budMat3& m3, const budMat3& m4 );
	
	BUD_INLINE	const float* 	operator[]( int index ) const;
	BUD_INLINE	float* 			operator[]( int index );
	BUD_INLINE	budMatX& 		operator=( const budMatX& a );
	BUD_INLINE	budMatX			operator*( const float a ) const;
	BUD_INLINE	budVecX			operator*( const budVecX& vec ) const;
	BUD_INLINE	budMatX			operator*( const budMatX& a ) const;
	BUD_INLINE	budMatX			operator+( const budMatX& a ) const;
	BUD_INLINE	budMatX			operator-( const budMatX& a ) const;
	BUD_INLINE	budMatX& 		operator*=( const float a );
	BUD_INLINE	budMatX& 		operator*=( const budMatX& a );
	BUD_INLINE	budMatX& 		operator+=( const budMatX& a );
	BUD_INLINE	budMatX& 		operator-=( const budMatX& a );
	
	friend BUD_INLINE	budMatX	operator*( const float a, const budMatX& m );
	friend BUD_INLINE	budVecX	operator*( const budVecX& vec, const budMatX& m );
	friend BUD_INLINE	budVecX& operator*=( budVecX& vec, const budMatX& m );
	
	BUD_INLINE	bool			Compare( const budMatX& a ) const;									// exact compare, no epsilon
	BUD_INLINE	bool			Compare( const budMatX& a, const float epsilon ) const;				// compare with epsilon
	BUD_INLINE	bool			operator==( const budMatX& a ) const;								// exact compare, no epsilon
	BUD_INLINE	bool			operator!=( const budMatX& a ) const;								// exact compare, no epsilon
	
	BUD_INLINE	void			SetSize( int rows, int columns );									// set the number of rows/columns
	void			ChangeSize( int rows, int columns, bool makeZero = false );		// change the size keeping data intact where possible
	BUD_INLINE	void			ChangeNumRows( int rows )
	{
		ChangeSize( rows, numColumns );	   // set the number of rows/columns
	}
	int				GetNumRows() const
	{
		return numRows;    // get the number of rows
	}
	int				GetNumColumns() const
	{
		return numColumns;    // get the number of columns
	}
	BUD_INLINE	void			SetData( int rows, int columns, float* data );						// set float array pointer
	BUD_INLINE	void			SetDataCacheLines( int rows, int columns, float* data, bool clear );// set float array pointer
	BUD_INLINE	void			Zero();																// clear matrix
	BUD_INLINE	void			Zero( int rows, int columns );										// set size and clear matrix
	BUD_INLINE	void			Identity();															// clear to identity matrix
	BUD_INLINE	void			Identity( int rows, int columns );									// set size and clear to identity matrix
	BUD_INLINE	void			Diag( const budVecX& v );											// create diagonal matrix from vector
	BUD_INLINE	void			Random( int seed, float l = 0.0f, float u = 1.0f );					// fill matrix with random values
	BUD_INLINE	void			Random( int rows, int columns, int seed, float l = 0.0f, float u = 1.0f );
	BUD_INLINE	void			Negate();															// (*this) = - (*this)
	BUD_INLINE	void			Clamp( float min, float max );										// clamp all values
	BUD_INLINE	budMatX& 		SwapRows( int r1, int r2 );											// swap rows
	BUD_INLINE	budMatX& 		SwapColumns( int r1, int r2 );										// swap columns
	BUD_INLINE	budMatX& 		SwapRowsColumns( int r1, int r2 );									// swap rows and columns
	budMatX& 		RemoveRow( int r );												// remove a row
	budMatX& 		RemoveColumn( int r );											// remove a column
	budMatX& 		RemoveRowColumn( int r );										// remove a row and column
	BUD_INLINE	void			ClearUpperTriangle();												// clear the upper triangle
	BUD_INLINE	void			ClearLowerTriangle();												// clear the lower triangle
	void			CopyLowerToUpperTriangle();											// copy the lower triangle to the upper triangle
	BUD_INLINE	void			SquareSubMatrix( const budMatX& m, int size );						// get square sub-matrix from 0,0 to size,size
	BUD_INLINE	float			MaxDifference( const budMatX& m ) const;								// return maximum element difference between this and m
	
	BUD_INLINE	bool			IsSquare() const
	{
		return ( numRows == numColumns );
	}
	BUD_INLINE	bool			IsZero( const float epsilon = MATRIX_EPSILON ) const;
	BUD_INLINE	bool			IsIdentity( const float epsilon = MATRIX_EPSILON ) const;
	BUD_INLINE	bool			IsDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	BUD_INLINE	bool			IsTriDiagonal( const float epsilon = MATRIX_EPSILON ) const;
	BUD_INLINE	bool			IsSymmetric( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsOrthogonal( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsOrthonormal( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsPMatrix( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsZMatrix( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsPositiveDefinite( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetricPositiveDefinite( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsPositiveSemiDefinite( const float epsilon = MATRIX_EPSILON ) const;
	bool			IsSymmetricPositiveSemiDefinite( const float epsilon = MATRIX_EPSILON ) const;
	
	BUD_INLINE	float			Trace() const;													// returns product of diagonal elements
	BUD_INLINE	float			Determinant() const;											// returns determinant of matrix
	BUD_INLINE	budMatX			Transpose() const;												// returns transpose
	BUD_INLINE	budMatX& 		TransposeSelf();												// transposes the matrix itself
	BUD_INLINE	void			Transpose( budMatX& dst ) const;								// stores transpose in 'dst'
	BUD_INLINE	budMatX			Inverse() const;												// returns the inverse ( m * m.Inverse() = identity )
	BUD_INLINE	bool			InverseSelf();													// returns false if determinant is zero
	BUD_INLINE	budMatX			InverseFast() const;											// returns the inverse ( m * m.Inverse() = identity )
	BUD_INLINE	bool			InverseFastSelf();												// returns false if determinant is zero
	BUD_INLINE	void			Inverse( budMatX& dst ) const;									// stores the inverse in 'dst' ( m * m.Inverse() = identity )
	
	bool			LowerTriangularInverse();									// in-place inversion, returns false if determinant is zero
	bool			UpperTriangularInverse();									// in-place inversion, returns false if determinant is zero
	
	BUD_INLINE	void			Subtract( const budMatX& a );									// (*this) -= a;
	
	BUD_INLINE	budVecX			Multiply( const budVecX& vec ) const;							// (*this) * vec
	BUD_INLINE	budVecX			TransposeMultiply( const budVecX& vec ) const;					// this->Transpose() * vec
	
	BUD_INLINE	budMatX			Multiply( const budMatX& a ) const;								// (*this) * a
	BUD_INLINE	budMatX			TransposeMultiply( const budMatX& a ) const;						// this->Transpose() * a
	
	BUD_INLINE	void			Multiply( budVecX& dst, const budVecX& vec ) const;				// dst = (*this) * vec
	BUD_INLINE	void			MultiplyAdd( budVecX& dst, const budVecX& vec ) const;			// dst += (*this) * vec
	BUD_INLINE	void			MultiplySub( budVecX& dst, const budVecX& vec ) const;			// dst -= (*this) * vec
	BUD_INLINE	void			TransposeMultiply( budVecX& dst, const budVecX& vec ) const;		// dst = this->Transpose() * vec
	BUD_INLINE	void			TransposeMultiplyAdd( budVecX& dst, const budVecX& vec ) const;	// dst += this->Transpose() * vec
	BUD_INLINE	void			TransposeMultiplySub( budVecX& dst, const budVecX& vec ) const;	// dst -= this->Transpose() * vec
	
	BUD_INLINE	void			Multiply( budMatX& dst, const budMatX& a ) const;					// dst = (*this) * a
	BUD_INLINE	void			TransposeMultiply( budMatX& dst, const budMatX& a ) const;		// dst = this->Transpose() * a
	
	BUD_INLINE	int				GetDimension() const;											// returns total number of values in matrix
	
	BUD_INLINE	const budVec6& 	SubVec6( int row ) const;										// interpret beginning of row as a const budVec6
	BUD_INLINE	budVec6& 		SubVec6( int row );												// interpret beginning of row as an budVec6
	BUD_INLINE	const budVecX	SubVecX( int row ) const;										// interpret complete row as a const budVecX
	BUD_INLINE	budVecX			SubVecX( int row );												// interpret complete row as an budVecX
	BUD_INLINE	const float* 	ToFloatPtr() const;												// pointer to const matrix float array
	BUD_INLINE	float* 			ToFloatPtr();													// pointer to matrix float array
	const char* 	ToString( int precision = 2 ) const;
	
	void			Update_RankOne( const budVecX& v, const budVecX& w, float alpha );
	void			Update_RankOneSymmetric( const budVecX& v, float alpha );
	void			Update_RowColumn( const budVecX& v, const budVecX& w, int r );
	void			Update_RowColumnSymmetric( const budVecX& v, int r );
	void			Update_Increment( const budVecX& v, const budVecX& w );
	void			Update_IncrementSymmetric( const budVecX& v );
	void			Update_Decrement( int r );
	
	bool			Inverse_GaussJordan();					// invert in-place with Gauss-Jordan elimination
	bool			Inverse_UpdateRankOne( const budVecX& v, const budVecX& w, float alpha );
	bool			Inverse_UpdateRowColumn( const budVecX& v, const budVecX& w, int r );
	bool			Inverse_UpdateIncrement( const budVecX& v, const budVecX& w );
	bool			Inverse_UpdateDecrement( const budVecX& v, const budVecX& w, int r );
	void			Inverse_Solve( budVecX& x, const budVecX& b ) const;
	
	bool			LU_Factor( int* index, float* det = NULL );		// factor in-place: L * U
	bool			LU_UpdateRankOne( const budVecX& v, const budVecX& w, float alpha, int* index );
	bool			LU_UpdateRowColumn( const budVecX& v, const budVecX& w, int r, int* index );
	bool			LU_UpdateIncrement( const budVecX& v, const budVecX& w, int* index );
	bool			LU_UpdateDecrement( const budVecX& v, const budVecX& w, const budVecX& u, int r, int* index );
	void			LU_Solve( budVecX& x, const budVecX& b, const int* index ) const;
	void			LU_Inverse( budMatX& inv, const int* index ) const;
	void			LU_UnpackFactors( budMatX& L, budMatX& U ) const;
	void			LU_MultiplyFactors( budMatX& m, const int* index ) const;
	
	bool			QR_Factor( budVecX& c, budVecX& d );				// factor in-place: Q * R
	bool			QR_UpdateRankOne( budMatX& R, const budVecX& v, const budVecX& w, float alpha );
	bool			QR_UpdateRowColumn( budMatX& R, const budVecX& v, const budVecX& w, int r );
	bool			QR_UpdateIncrement( budMatX& R, const budVecX& v, const budVecX& w );
	bool			QR_UpdateDecrement( budMatX& R, const budVecX& v, const budVecX& w, int r );
	void			QR_Solve( budVecX& x, const budVecX& b, const budVecX& c, const budVecX& d ) const;
	void			QR_Solve( budVecX& x, const budVecX& b, const budMatX& R ) const;
	void			QR_Inverse( budMatX& inv, const budVecX& c, const budVecX& d ) const;
	void			QR_UnpackFactors( budMatX& Q, budMatX& R, const budVecX& c, const budVecX& d ) const;
	void			QR_MultiplyFactors( budMatX& m, const budVecX& c, const budVecX& d ) const;
	
	bool			SVD_Factor( budVecX& w, budMatX& V );				// factor in-place: U * Diag(w) * V.Transpose()
	void			SVD_Solve( budVecX& x, const budVecX& b, const budVecX& w, const budMatX& V ) const;
	void			SVD_Inverse( budMatX& inv, const budVecX& w, const budMatX& V ) const;
	void			SVD_MultiplyFactors( budMatX& m, const budVecX& w, const budMatX& V ) const;
	
	bool			Cholesky_Factor();						// factor in-place: L * L.Transpose()
	bool			Cholesky_UpdateRankOne( const budVecX& v, float alpha, int offset = 0 );
	bool			Cholesky_UpdateRowColumn( const budVecX& v, int r );
	bool			Cholesky_UpdateIncrement( const budVecX& v );
	bool			Cholesky_UpdateDecrement( const budVecX& v, int r );
	void			Cholesky_Solve( budVecX& x, const budVecX& b ) const;
	void			Cholesky_Inverse( budMatX& inv ) const;
	void			Cholesky_MultiplyFactors( budMatX& m ) const;
	
	bool			LDLT_Factor();							// factor in-place: L * D * L.Transpose()
	bool			LDLT_UpdateRankOne( const budVecX& v, float alpha, int offset = 0 );
	bool			LDLT_UpdateRowColumn( const budVecX& v, int r );
	bool			LDLT_UpdateIncrement( const budVecX& v );
	bool			LDLT_UpdateDecrement( const budVecX& v, int r );
	void			LDLT_Solve( budVecX& x, const budVecX& b ) const;
	void			LDLT_Inverse( budMatX& inv ) const;
	void			LDLT_UnpackFactors( budMatX& L, budMatX& D ) const;
	void			LDLT_MultiplyFactors( budMatX& m ) const;
	
	void			TriDiagonal_ClearTriangles();
	bool			TriDiagonal_Solve( budVecX& x, const budVecX& b ) const;
	void			TriDiagonal_Inverse( budMatX& inv ) const;
	
	bool			Eigen_SolveSymmetricTriDiagonal( budVecX& eigenValues );
	bool			Eigen_SolveSymmetric( budVecX& eigenValues );
	bool			Eigen_Solve( budVecX& realEigenValues, budVecX& imaginaryEigenValues );
	void			Eigen_SortIncreasing( budVecX& eigenValues );
	void			Eigen_SortDecreasing( budVecX& eigenValues );
	
	static void		Test();
	
private:
	int				numRows;				// number of rows
	int				numColumns;				// number of columns
	int				alloced;				// floats allocated, if -1 then mat points to data set with SetData
	float* 			mat;					// memory the matrix is stored
	
	static float	temp[MATX_MAX_TEMP + 4];	// used to store intermediate results
	static float* 	tempPtr;				// pointer to 16 byte aligned temporary memory
	static int		tempIndex;				// index into memory pool, wraps around
	
private:
	void			SetTempSize( int rows, int columns );
	float			DeterminantGeneric() const;
	bool			InverseSelfGeneric();
	void			QR_Rotate( budMatX& R, int i, float a, float b );
	float			Pythag( float a, float b ) const;
	void			SVD_BiDiag( budVecX& w, budVecX& rv1, float& anorm );
	void			SVD_InitialWV( budVecX& w, budMatX& V, budVecX& rv1 );
	void			HouseholderReduction( budVecX& diag, budVecX& subd );
	bool			QL( budVecX& diag, budVecX& subd );
	void			HessenbergReduction( budMatX& H );
	void			ComplexDivision( float xr, float xi, float yr, float yi, float& cdivr, float& cdivi );
	bool			HessenbergToRealSchur( budMatX& H, budVecX& realEigenValues, budVecX& imaginaryEigenValues );
};

/*
========================
budMatX::budMatX
========================
*/
BUD_INLINE budMatX::budMatX()
{
	numRows = numColumns = alloced = 0;
	mat = NULL;
}

/*
========================
budMatX::~budMatX
========================
*/
BUD_INLINE budMatX::~budMatX()
{
	// if not temp memory
	if( mat != NULL && ( mat < budMatX::tempPtr || mat > budMatX::tempPtr + MATX_MAX_TEMP ) && alloced != -1 )
	{
		Mem_Free16( mat );
	}
}

/*
========================
budMatX::budMatX
========================
*/
BUD_INLINE budMatX::budMatX( int rows, int columns )
{
	numRows = numColumns = alloced = 0;
	mat = NULL;
	SetSize( rows, columns );
}

/*
========================
budMatX::budMatX
========================
*/
BUD_INLINE budMatX::budMatX( const budMatX& other )
{
	numRows = numColumns = alloced = 0;
	mat = NULL;
	Set( other.GetNumRows(), other.GetNumColumns(), other.ToFloatPtr() );
}

/*
========================
budMatX::budMatX
========================
*/
BUD_INLINE budMatX::budMatX( int rows, int columns, float* src )
{
	numRows = numColumns = alloced = 0;
	mat = NULL;
	SetData( rows, columns, src );
}

/*
========================
budMatX::Set
========================
*/
BUD_INLINE void budMatX::Set( int rows, int columns, const float* src )
{
	SetSize( rows, columns );
	memcpy( this->mat, src, rows * columns * sizeof( float ) );
}

/*
========================
budMatX::Set
========================
*/
BUD_INLINE void budMatX::Set( const budMat3& m1, const budMat3& m2 )
{
	SetSize( 3, 6 );
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			mat[( i + 0 ) * numColumns + ( j + 0 )] = m1[i][j];
			mat[( i + 0 ) * numColumns + ( j + 3 )] = m2[i][j];
		}
	}
}

/*
========================
budMatX::Set
========================
*/
BUD_INLINE void budMatX::Set( const budMat3& m1, const budMat3& m2, const budMat3& m3, const budMat3& m4 )
{
	SetSize( 6, 6 );
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			mat[( i + 0 ) * numColumns + ( j + 0 )] = m1[i][j];
			mat[( i + 0 ) * numColumns + ( j + 3 )] = m2[i][j];
			mat[( i + 3 ) * numColumns + ( j + 0 )] = m3[i][j];
			mat[( i + 3 ) * numColumns + ( j + 3 )] = m4[i][j];
		}
	}
}

/*
========================
budMatX::operator[]
========================
*/
BUD_INLINE const float* budMatX::operator[]( int index ) const
{
	assert( ( index >= 0 ) && ( index < numRows ) );
	return mat + index * numColumns;
}

/*
========================
budMatX::operator[]
========================
*/
BUD_INLINE float* budMatX::operator[]( int index )
{
	assert( ( index >= 0 ) && ( index < numRows ) );
	return mat + index * numColumns;
}

/*
========================
budMatX::operator=
========================
*/
BUD_INLINE budMatX& budMatX::operator=( const budMatX& a )
{
	SetSize( a.numRows, a.numColumns );
	int s = a.numRows * a.numColumns;
#ifdef MATX_SIMD
	for( int i = 0; i < s; i += 4 )
	{
		_mm_store_ps( mat + i, _mm_load_ps( a.mat + i ) );
	}
#else
	memcpy( mat, a.mat, s * sizeof( float ) );
#endif
	budMatX::tempIndex = 0;
	return *this;
}

/*
========================
budMatX::operator*
========================
*/
BUD_INLINE budMatX budMatX::operator*( const float a ) const
{
	budMatX m;
	
	m.SetTempSize( numRows, numColumns );
	int s = numRows * numColumns;
#ifdef MATX_SIMD
	__m128 va = _mm_load1_ps( & a );
	for( int i = 0; i < s; i += 4 )
	{
		_mm_store_ps( m.mat + i, _mm_mul_ps( _mm_load_ps( mat + i ), va ) );
	}
#else
	for( int i = 0; i < s; i++ )
	{
		m.mat[i] = mat[i] * a;
	}
#endif
	return m;
}

/*
========================
budMatX::operator*
========================
*/
BUD_INLINE budVecX budMatX::operator*( const budVecX& vec ) const
{
	assert( numColumns == vec.GetSize() );
	
	budVecX dst;
	dst.SetTempSize( numRows );
	Multiply( dst, vec );
	return dst;
}

/*
========================
budMatX::operator*
========================
*/
BUD_INLINE budMatX budMatX::operator*( const budMatX& a ) const
{
	assert( numColumns == a.numRows );
	
	budMatX dst;
	dst.SetTempSize( numRows, a.numColumns );
	Multiply( dst, a );
	return dst;
}

/*
========================
budMatX::operator+
========================
*/
BUD_INLINE budMatX budMatX::operator+( const budMatX& a ) const
{
	budMatX m;
	
	assert( numRows == a.numRows && numColumns == a.numColumns );
	m.SetTempSize( numRows, numColumns );
	int s = numRows * numColumns;
#ifdef MATX_SIMD
	for( int i = 0; i < s; i += 4 )
	{
		_mm_store_ps( m.mat + i, _mm_add_ps( _mm_load_ps( mat + i ), _mm_load_ps( a.mat + i ) ) );
	}
#else
	for( int i = 0; i < s; i++ )
	{
		m.mat[i] = mat[i] + a.mat[i];
	}
#endif
	return m;
}

/*
========================
budMatX::operator-
========================
*/
BUD_INLINE budMatX budMatX::operator-( const budMatX& a ) const
{
	budMatX m;
	
	assert( numRows == a.numRows && numColumns == a.numColumns );
	m.SetTempSize( numRows, numColumns );
	int s = numRows * numColumns;
#ifdef MATX_SIMD
	for( int i = 0; i < s; i += 4 )
	{
		_mm_store_ps( m.mat + i, _mm_sub_ps( _mm_load_ps( mat + i ), _mm_load_ps( a.mat + i ) ) );
	}
#else
	for( int i = 0; i < s; i++ )
	{
		m.mat[i] = mat[i] - a.mat[i];
	}
#endif
	return m;
}

/*
========================
budMatX::operator*=
========================
*/
BUD_INLINE budMatX& budMatX::operator*=( const float a )
{
	int s = numRows * numColumns;
#ifdef MATX_SIMD
	__m128 va = _mm_load1_ps( & a );
	for( int i = 0; i < s; i += 4 )
	{
		_mm_store_ps( mat + i, _mm_mul_ps( _mm_load_ps( mat + i ), va ) );
	}
#else
	for( int i = 0; i < s; i++ )
	{
		mat[i] *= a;
	}
#endif
	budMatX::tempIndex = 0;
	return *this;
}

/*
========================
budMatX::operator*=
========================
*/
BUD_INLINE budMatX& budMatX::operator*=( const budMatX& a )
{
	*this = *this * a;
	budMatX::tempIndex = 0;
	return *this;
}

/*
========================
budMatX::operator+=
========================
*/
BUD_INLINE budMatX& budMatX::operator+=( const budMatX& a )
{
	assert( numRows == a.numRows && numColumns == a.numColumns );
	int s = numRows * numColumns;
#ifdef MATX_SIMD
	for( int i = 0; i < s; i += 4 )
	{
		_mm_store_ps( mat + i, _mm_add_ps( _mm_load_ps( mat + i ), _mm_load_ps( a.mat + i ) ) );
	}
#else
	for( int i = 0; i < s; i++ )
	{
		mat[i] += a.mat[i];
	}
#endif
	budMatX::tempIndex = 0;
	return *this;
}

/*
========================
budMatX::operator-=
========================
*/
BUD_INLINE budMatX& budMatX::operator-=( const budMatX& a )
{
	assert( numRows == a.numRows && numColumns == a.numColumns );
	int s = numRows * numColumns;
#ifdef MATX_SIMD
	for( int i = 0; i < s; i += 4 )
	{
		_mm_store_ps( mat + i, _mm_sub_ps( _mm_load_ps( mat + i ), _mm_load_ps( a.mat + i ) ) );
	}
#else
	for( int i = 0; i < s; i++ )
	{
		mat[i] -= a.mat[i];
	}
#endif
	budMatX::tempIndex = 0;
	return *this;
}

/*
========================
operator*
========================
*/
BUD_INLINE budMatX operator*( const float a, budMatX const& m )
{
	return m * a;
}

/*
========================
operator*
========================
*/
BUD_INLINE budVecX operator*( const budVecX& vec, const budMatX& m )
{
	return m * vec;
}

/*
========================
operator*=
========================
*/
BUD_INLINE budVecX& operator*=( budVecX& vec, const budMatX& m )
{
	vec = m * vec;
	return vec;
}

/*
========================
budMatX::Compare
========================
*/
BUD_INLINE bool budMatX::Compare( const budMatX& a ) const
{
	assert( numRows == a.numRows && numColumns == a.numColumns );
	
	int s = numRows * numColumns;
	for( int i = 0; i < s; i++ )
	{
		if( mat[i] != a.mat[i] )
		{
			return false;
		}
	}
	return true;
}

/*
========================
budMatX::Compare
========================
*/
BUD_INLINE bool budMatX::Compare( const budMatX& a, const float epsilon ) const
{
	assert( numRows == a.numRows && numColumns == a.numColumns );
	
	int s = numRows * numColumns;
	for( int i = 0; i < s; i++ )
	{
		if( budMath::Fabs( mat[i] - a.mat[i] ) > epsilon )
		{
			return false;
		}
	}
	return true;
}

/*
========================
budMatX::operator==
========================
*/
BUD_INLINE bool budMatX::operator==( const budMatX& a ) const
{
	return Compare( a );
}

/*
========================
budMatX::operator!=
========================
*/
BUD_INLINE bool budMatX::operator!=( const budMatX& a ) const
{
	return !Compare( a );
}

/*
========================
budMatX::SetSize
========================
*/
BUD_INLINE void budMatX::SetSize( int rows, int columns )
{
	if( rows != numRows || columns != numColumns || mat == NULL )
	{
		assert( mat < budMatX::tempPtr || mat > budMatX::tempPtr + MATX_MAX_TEMP );
		int alloc = ( rows * columns + 3 ) & ~3;
		if( alloc > alloced && alloced != -1 )
		{
			if( mat != NULL )
			{
				Mem_Free16( mat );
			}
			mat = ( float* ) Mem_Alloc16( alloc * sizeof( float ), TAG_MATH );
			alloced = alloc;
		}
		numRows = rows;
		numColumns = columns;
		MATX_CLEAREND();
	}
}

/*
========================
budMatX::SetTempSize
========================
*/
BUD_INLINE void budMatX::SetTempSize( int rows, int columns )
{
	int newSize;
	
	newSize = ( rows * columns + 3 ) & ~3;
	assert( newSize < MATX_MAX_TEMP );
	if( budMatX::tempIndex + newSize > MATX_MAX_TEMP )
	{
		budMatX::tempIndex = 0;
	}
	mat = budMatX::tempPtr + budMatX::tempIndex;
	budMatX::tempIndex += newSize;
	alloced = newSize;
	numRows = rows;
	numColumns = columns;
	MATX_CLEAREND();
}

/*
========================
budMatX::SetData
========================
*/
BUD_INLINE void budMatX::SetData( int rows, int columns, float* data )
{
	assert( mat < budMatX::tempPtr || mat > budMatX::tempPtr + MATX_MAX_TEMP );
	if( mat != NULL && alloced != -1 )
	{
		Mem_Free16( mat );
	}
	// RB: changed UINT_PTR to uintptr_t
	assert( ( ( ( uintptr_t ) data ) & 15 ) == 0 ); // data must be 16 byte aligned
	// RB end
	mat = data;
	alloced = -1;
	numRows = rows;
	numColumns = columns;
	MATX_CLEAREND();
}

/*
========================
budMatX::SetDataCacheLines
========================
*/
BUD_INLINE void budMatX::SetDataCacheLines( int rows, int columns, float* data, bool clear )
{
	if( mat != NULL && alloced != -1 )
	{
		Mem_Free( mat );
	}
	
	// RB: changed UINT_PTR to uintptr_t
	assert( ( ( ( uintptr_t ) data ) & 127 ) == 0 ); // data must be 128 byte aligned
	// RB end
	
	mat = data;
	alloced = -1;
	numRows = rows;
	numColumns = columns;
	
	if( clear )
	{
		int size = numRows * numColumns * sizeof( float );
		for( int i = 0; i < size; i += CACHE_LINE_SIZE )
		{
			ZeroCacheLine( mat, i );
		}
	}
	else
	{
		MATX_CLEAREND();
	}
}

/*
========================
budMatX::Zero
========================
*/
BUD_INLINE void budMatX::Zero()
{
	int s = numRows * numColumns;
#ifdef MATX_SIMD
	for( int i = 0; i < s; i += 4 )
	{
		_mm_store_ps( mat + i, _mm_setzero_ps() );
	}
#else
	memset( mat, 0, numRows * numColumns * sizeof( float ) );
#endif
}

/*
========================
budMatX::Zero
========================
*/
BUD_INLINE void budMatX::Zero( int rows, int columns )
{
	SetSize( rows, columns );
	Zero();
}

/*
========================
budMatX::Identity
========================
*/
BUD_INLINE void budMatX::Identity()
{
	assert( numRows == numColumns );
	Zero();
	for( int i = 0; i < numRows; i++ )
	{
		mat[i * numColumns + i] = 1.0f;
	}
}

/*
========================
budMatX::Identity
========================
*/
BUD_INLINE void budMatX::Identity( int rows, int columns )
{
	assert( rows == columns );
	SetSize( rows, columns );
	budMatX::Identity();
}

/*
========================
budMatX::Diag
========================
*/
BUD_INLINE void budMatX::Diag( const budVecX& v )
{
	Zero( v.GetSize(), v.GetSize() );
	for( int i = 0; i < v.GetSize(); i++ )
	{
		mat[i * numColumns + i] = v[i];
	}
}

/*
========================
budMatX::Random
========================
*/
BUD_INLINE void budMatX::Random( int seed, float l, float u )
{
	idRandom rnd( seed );
	
	float c = u - l;
	int s = numRows * numColumns;
	for( int i = 0; i < s; i++ )
	{
		mat[i] = l + rnd.RandomFloat() * c;
	}
}

/*
========================
budMatX::Random
========================
*/
BUD_INLINE void budMatX::Random( int rows, int columns, int seed, float l, float u )
{
	idRandom rnd( seed );
	
	SetSize( rows, columns );
	float c = u - l;
	int s = numRows * numColumns;
	for( int i = 0; i < s; i++ )
	{
		mat[i] = l + rnd.RandomFloat() * c;
	}
}

/*
========================
budMatX::Negate
========================
*/
BUD_INLINE void budMatX::Negate()
{
	int s = numRows * numColumns;
#ifdef MATX_SIMD
	ALIGN16( const unsigned int signBit[4] ) = { IEEE_FLT_SIGN_MASK, IEEE_FLT_SIGN_MASK, IEEE_FLT_SIGN_MASK, IEEE_FLT_SIGN_MASK };
	for( int i = 0; i < s; i += 4 )
	{
		_mm_store_ps( mat + i, _mm_xor_ps( _mm_load_ps( mat + i ), ( __m128& ) signBit[0] ) );
	}
#else
	for( int i = 0; i < s; i++ )
	{
		mat[i] = -mat[i];
	}
#endif
}

/*
========================
budMatX::Clamp
========================
*/
BUD_INLINE void budMatX::Clamp( float min, float max )
{
	int s = numRows * numColumns;
	for( int i = 0; i < s; i++ )
	{
		if( mat[i] < min )
		{
			mat[i] = min;
		}
		else if( mat[i] > max )
		{
			mat[i] = max;
		}
	}
}

/*
========================
budMatX::SwapRows
========================
*/
BUD_INLINE budMatX& budMatX::SwapRows( int r1, int r2 )
{
	float* ptr1 = mat + r1 * numColumns;
	float* ptr2 = mat + r2 * numColumns;
	for( int i = 0; i < numColumns; i++ )
	{
		SwapValues( ptr1[i], ptr2[i] );
	}
	return *this;
}

/*
========================
budMatX::SwapColumns
========================
*/
BUD_INLINE budMatX& budMatX::SwapColumns( int r1, int r2 )
{
	float* ptr = mat;
	for( int i = 0; i < numRows; i++, ptr += numColumns )
	{
		SwapValues( ptr[r1], ptr[r2] );
	}
	return *this;
}

/*
========================
budMatX::SwapRowsColumns
========================
*/
BUD_INLINE budMatX& budMatX::SwapRowsColumns( int r1, int r2 )
{
	SwapRows( r1, r2 );
	SwapColumns( r1, r2 );
	return *this;
}

/*
========================
budMatX::ClearUpperTriangle
========================
*/
BUD_INLINE void budMatX::ClearUpperTriangle()
{
	assert( numRows == numColumns );
	for( int i = numRows - 2; i >= 0; i-- )
	{
		memset( mat + i * numColumns + i + 1, 0, ( numColumns - 1 - i ) * sizeof( float ) );
	}
}

/*
========================
budMatX::ClearLowerTriangle
========================
*/
BUD_INLINE void budMatX::ClearLowerTriangle()
{
	assert( numRows == numColumns );
	for( int i = 1; i < numRows; i++ )
	{
		memset( mat + i * numColumns, 0, i * sizeof( float ) );
	}
}

/*
========================
budMatX::SquareSubMatrix
========================
*/
BUD_INLINE void budMatX::SquareSubMatrix( const budMatX& m, int size )
{
	assert( size <= m.numRows && size <= m.numColumns );
	SetSize( size, size );
	for( int i = 0; i < size; i++ )
	{
		memcpy( mat + i * numColumns, m.mat + i * m.numColumns, size * sizeof( float ) );
	}
}

/*
========================
budMatX::MaxDifference
========================
*/
BUD_INLINE float budMatX::MaxDifference( const budMatX& m ) const
{
	assert( numRows == m.numRows && numColumns == m.numColumns );
	
	float maxDiff = -1.0f;
	for( int i = 0; i < numRows; i++ )
	{
		for( int j = 0; j < numColumns; j++ )
		{
			float diff = budMath::Fabs( mat[ i * numColumns + j ] - m[i][j] );
			if( maxDiff < 0.0f || diff > maxDiff )
			{
				maxDiff = diff;
			}
		}
	}
	return maxDiff;
}

/*
========================
budMatX::IsZero
========================
*/
BUD_INLINE bool budMatX::IsZero( const float epsilon ) const
{
	// returns true if (*this) == Zero
	for( int i = 0; i < numRows; i++ )
	{
		for( int j = 0; j < numColumns; j++ )
		{
			if( budMath::Fabs( mat[i * numColumns + j] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

/*
========================
budMatX::IsIdentity
========================
*/
BUD_INLINE bool budMatX::IsIdentity( const float epsilon ) const
{
	// returns true if (*this) == Identity
	assert( numRows == numColumns );
	for( int i = 0; i < numRows; i++ )
	{
		for( int j = 0; j < numColumns; j++ )
		{
			if( budMath::Fabs( mat[i * numColumns + j] - ( float )( i == j ) ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

/*
========================
budMatX::IsDiagonal
========================
*/
BUD_INLINE bool budMatX::IsDiagonal( const float epsilon ) const
{
	// returns true if all elements are zero except for the elements on the diagonal
	assert( numRows == numColumns );
	for( int i = 0; i < numRows; i++ )
	{
		for( int j = 0; j < numColumns; j++ )
		{
			if( i != j && budMath::Fabs( mat[i * numColumns + j] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

/*
========================
budMatX::IsTriDiagonal
========================
*/
BUD_INLINE bool budMatX::IsTriDiagonal( const float epsilon ) const
{
	// returns true if all elements are zero except for the elements on the diagonal plus or minus one column
	
	if( numRows != numColumns )
	{
		return false;
	}
	for( int i = 0; i < numRows - 2; i++ )
	{
		for( int j = i + 2; j < numColumns; j++ )
		{
			if( budMath::Fabs( ( *this )[i][j] ) > epsilon )
			{
				return false;
			}
			if( budMath::Fabs( ( *this )[j][i] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

/*
========================
budMatX::IsSymmetric
========================
*/
BUD_INLINE bool budMatX::IsSymmetric( const float epsilon ) const
{
	// (*this)[i][j] == (*this)[j][i]
	if( numRows != numColumns )
	{
		return false;
	}
	for( int i = 0; i < numRows; i++ )
	{
		for( int j = 0; j < numColumns; j++ )
		{
			if( budMath::Fabs( mat[ i * numColumns + j ] - mat[ j * numColumns + i ] ) > epsilon )
			{
				return false;
			}
		}
	}
	return true;
}

/*
========================
budMatX::Trace
========================
*/
BUD_INLINE float budMatX::Trace() const
{
	float trace = 0.0f;
	
	assert( numRows == numColumns );
	
	// sum of elements on the diagonal
	for( int i = 0; i < numRows; i++ )
	{
		trace += mat[i * numRows + i];
	}
	return trace;
}

/*
========================
budMatX::Determinant
========================
*/
BUD_INLINE float budMatX::Determinant() const
{

	assert( numRows == numColumns );
	
	switch( numRows )
	{
		case 1:
			return mat[0];
		case 2:
			return reinterpret_cast<const budMat2*>( mat )->Determinant();
		case 3:
			return reinterpret_cast<const budMat3*>( mat )->Determinant();
		case 4:
			return reinterpret_cast<const budMat4*>( mat )->Determinant();
		case 5:
			return reinterpret_cast<const budMat5*>( mat )->Determinant();
		case 6:
			return reinterpret_cast<const budMat6*>( mat )->Determinant();
		default:
			return DeterminantGeneric();
	}
}

/*
========================
budMatX::Transpose
========================
*/
BUD_INLINE budMatX budMatX::Transpose() const
{
	budMatX transpose;
	
	transpose.SetTempSize( numColumns, numRows );
	
	for( int i = 0; i < numRows; i++ )
	{
		for( int j = 0; j < numColumns; j++ )
		{
			transpose.mat[j * transpose.numColumns + i] = mat[i * numColumns + j];
		}
	}
	
	return transpose;
}

/*
========================
budMatX::TransposeSelf
========================
*/
BUD_INLINE budMatX& budMatX::TransposeSelf()
{
	*this = Transpose();
	return *this;
}

/*
========================
budMatX::Transpose
========================
*/
BUD_INLINE void budMatX::Transpose( budMatX& dst ) const
{
	dst = Transpose();
}

/*
========================
budMatX::Inverse
========================
*/
BUD_INLINE budMatX budMatX::Inverse() const
{
	budMatX invMat;
	
	invMat.SetTempSize( numRows, numColumns );
	memcpy( invMat.mat, mat, numRows * numColumns * sizeof( float ) );
	verify( invMat.InverseSelf() );
	return invMat;
}

/*
========================
budMatX::InverseSelf
========================
*/
BUD_INLINE bool budMatX::InverseSelf()
{

	assert( numRows == numColumns );
	
	switch( numRows )
	{
		case 1:
			if( budMath::Fabs( mat[0] ) < MATRIX_INVERSE_EPSILON )
			{
				return false;
			}
			mat[0] = 1.0f / mat[0];
			return true;
		case 2:
			return reinterpret_cast<budMat2*>( mat )->InverseSelf();
		case 3:
			return reinterpret_cast<budMat3*>( mat )->InverseSelf();
		case 4:
			return reinterpret_cast<budMat4*>( mat )->InverseSelf();
		case 5:
			return reinterpret_cast<budMat5*>( mat )->InverseSelf();
		case 6:
			return reinterpret_cast<budMat6*>( mat )->InverseSelf();
		default:
			return InverseSelfGeneric();
	}
}

/*
========================
budMatX::InverseFast
========================
*/
BUD_INLINE budMatX budMatX::InverseFast() const
{
	budMatX invMat;
	
	invMat.SetTempSize( numRows, numColumns );
	memcpy( invMat.mat, mat, numRows * numColumns * sizeof( float ) );
	verify( invMat.InverseFastSelf() );
	return invMat;
}

/*
========================
budMatX::InverseFastSelf
========================
*/
BUD_INLINE bool budMatX::InverseFastSelf()
{

	assert( numRows == numColumns );
	
	switch( numRows )
	{
		case 1:
			if( budMath::Fabs( mat[0] ) < MATRIX_INVERSE_EPSILON )
			{
				return false;
			}
			mat[0] = 1.0f / mat[0];
			return true;
		case 2:
			return reinterpret_cast<budMat2*>( mat )->InverseFastSelf();
		case 3:
			return reinterpret_cast<budMat3*>( mat )->InverseFastSelf();
		case 4:
			return reinterpret_cast<budMat4*>( mat )->InverseFastSelf();
		case 5:
			return reinterpret_cast<budMat5*>( mat )->InverseFastSelf();
		case 6:
			return reinterpret_cast<budMat6*>( mat )->InverseFastSelf();
		default:
			return InverseSelfGeneric();
	}
}

/*
========================
budMatX::Inverse
========================
*/
BUD_INLINE void budMatX::Inverse( budMatX& dst ) const
{
	dst = InverseFast();
}

/*
========================
budMatX::Subtract
========================
*/
BUD_INLINE void budMatX::Subtract( const budMatX& a )
{
	( *this ) -= a;
}

/*
========================
budMatX::Multiply
========================
*/
BUD_INLINE budVecX budMatX::Multiply( const budVecX& vec ) const
{
	assert( numColumns == vec.GetSize() );
	
	budVecX dst;
	dst.SetTempSize( numRows );
	Multiply( dst, vec );
	return dst;
}

/*
========================
budMatX::Multiply
========================
*/
BUD_INLINE budMatX budMatX::Multiply( const budMatX& a ) const
{
	assert( numColumns == a.numRows );
	
	budMatX dst;
	dst.SetTempSize( numRows, a.numColumns );
	Multiply( dst, a );
	return dst;
}

/*
========================
budMatX::TransposeMultiply
========================
*/
BUD_INLINE budVecX budMatX::TransposeMultiply( const budVecX& vec ) const
{
	assert( numRows == vec.GetSize() );
	
	budVecX dst;
	dst.SetTempSize( numColumns );
	TransposeMultiply( dst, vec );
	return dst;
}

/*
========================
budMatX::TransposeMultiply
========================
*/
BUD_INLINE budMatX budMatX::TransposeMultiply( const budMatX& a ) const
{
	assert( numRows == a.numRows );
	
	budMatX dst;
	dst.SetTempSize( numColumns, a.numColumns );
	TransposeMultiply( dst, a );
	return dst;
}

/*
========================
budMatX::Multiply
========================
*/
BUD_INLINE void budMatX::Multiply( budVecX& dst, const budVecX& vec ) const
{
	dst.SetSize( numRows );
	const float* mPtr = mat;
	const float* vPtr = vec.ToFloatPtr();
	float* dstPtr = dst.ToFloatPtr();
	float* temp = ( float* )_alloca16( numRows * sizeof( float ) );
	for( int i = 0; i < numRows; i++ )
	{
		float sum = mPtr[0] * vPtr[0];
		for( int j = 1; j < numColumns; j++ )
		{
			sum += mPtr[j] * vPtr[j];
		}
		temp[i] = sum;
		mPtr += numColumns;
	}
	for( int i = 0; i < numRows; i++ )
	{
		dstPtr[i] = temp[i];
	}
}

/*
========================
budMatX::MultiplyAdd
========================
*/
BUD_INLINE void budMatX::MultiplyAdd( budVecX& dst, const budVecX& vec ) const
{
	assert( dst.GetSize() == numRows );
	const float* mPtr = mat;
	const float* vPtr = vec.ToFloatPtr();
	float* dstPtr = dst.ToFloatPtr();
	float* temp = ( float* )_alloca16( numRows * sizeof( float ) );
	for( int i = 0; i < numRows; i++ )
	{
		float sum = mPtr[0] * vPtr[0];
		for( int j = 1; j < numColumns; j++ )
		{
			sum += mPtr[j] * vPtr[j];
		}
		temp[i] = dstPtr[i] + sum;
		mPtr += numColumns;
	}
	for( int i = 0; i < numRows; i++ )
	{
		dstPtr[i] = temp[i];
	}
}

/*
========================
budMatX::MultiplySub
========================
*/
BUD_INLINE void budMatX::MultiplySub( budVecX& dst, const budVecX& vec ) const
{
	assert( dst.GetSize() == numRows );
	const float* mPtr = mat;
	const float* vPtr = vec.ToFloatPtr();
	float* dstPtr = dst.ToFloatPtr();
	float* temp = ( float* )_alloca16( numRows * sizeof( float ) );
	for( int i = 0; i < numRows; i++ )
	{
		float sum = mPtr[0] * vPtr[0];
		for( int j = 1; j < numColumns; j++ )
		{
			sum += mPtr[j] * vPtr[j];
		}
		temp[i] = dstPtr[i] - sum;
		mPtr += numColumns;
	}
	for( int i = 0; i < numRows; i++ )
	{
		dstPtr[i] = temp[i];
	}
}

/*
========================
budMatX::TransposeMultiply
========================
*/
BUD_INLINE void budMatX::TransposeMultiply( budVecX& dst, const budVecX& vec ) const
{
	dst.SetSize( numColumns );
	const float* vPtr = vec.ToFloatPtr();
	float* dstPtr = dst.ToFloatPtr();
	float* temp = ( float* )_alloca16( numColumns * sizeof( float ) );
	for( int i = 0; i < numColumns; i++ )
	{
		const float* mPtr = mat + i;
		float sum = mPtr[0] * vPtr[0];
		for( int j = 1; j < numRows; j++ )
		{
			mPtr += numColumns;
			sum += mPtr[0] * vPtr[j];
		}
		temp[i] = sum;
	}
	for( int i = 0; i < numColumns; i++ )
	{
		dstPtr[i] = temp[i];
	}
}

/*
========================
budMatX::TransposeMultiplyAdd
========================
*/
BUD_INLINE void budMatX::TransposeMultiplyAdd( budVecX& dst, const budVecX& vec ) const
{
	assert( dst.GetSize() == numColumns );
	const float* vPtr = vec.ToFloatPtr();
	float* dstPtr = dst.ToFloatPtr();
	float* temp = ( float* )_alloca16( numColumns * sizeof( float ) );
	for( int i = 0; i < numColumns; i++ )
	{
		const float* mPtr = mat + i;
		float sum = mPtr[0] * vPtr[0];
		for( int j = 1; j < numRows; j++ )
		{
			mPtr += numColumns;
			sum += mPtr[0] * vPtr[j];
		}
		temp[i] = dstPtr[i] + sum;
	}
	for( int i = 0; i < numColumns; i++ )
	{
		dstPtr[i] = temp[i];
	}
}

/*
========================
budMatX::TransposeMultiplySub
========================
*/
BUD_INLINE void budMatX::TransposeMultiplySub( budVecX& dst, const budVecX& vec ) const
{
	assert( dst.GetSize() == numColumns );
	const float* vPtr = vec.ToFloatPtr();
	float* dstPtr = dst.ToFloatPtr();
	float* temp = ( float* )_alloca16( numColumns * sizeof( float ) );
	for( int i = 0; i < numColumns; i++ )
	{
		const float* mPtr = mat + i;
		float sum = mPtr[0] * vPtr[0];
		for( int j = 1; j < numRows; j++ )
		{
			mPtr += numColumns;
			sum += mPtr[0] * vPtr[j];
		}
		temp[i] = dstPtr[i] - sum;
	}
	for( int i = 0; i < numColumns; i++ )
	{
		dstPtr[i] = temp[i];
	}
}

/*
========================
budMatX::Multiply
========================
*/
BUD_INLINE void budMatX::Multiply( budMatX& dst, const budMatX& a ) const
{
	assert( numColumns == a.numRows );
	assert( &dst != &a && &dst != this );
	
	dst.SetSize( numRows, a.numColumns );
	float* dstPtr = dst.ToFloatPtr();
	const float* m1Ptr = ToFloatPtr();
	int k = numRows;
	int l = a.GetNumColumns();
	for( int i = 0; i < k; i++ )
	{
		for( int j = 0; j < l; j++ )
		{
			const float* m2Ptr = a.ToFloatPtr() + j;
			float sum = m1Ptr[0] * m2Ptr[0];
			for( int n = 1; n < numColumns; n++ )
			{
				m2Ptr += l;
				sum += m1Ptr[n] * m2Ptr[0];
			}
			*dstPtr++ = sum;
		}
		m1Ptr += numColumns;
	}
}

/*
========================
budMatX::TransposeMultiply
========================
*/
BUD_INLINE void budMatX::TransposeMultiply( budMatX& dst, const budMatX& a ) const
{
	assert( numRows == a.numRows );
	assert( &dst != &a && &dst != this );
	
	dst.SetSize( numColumns, a.numColumns );
	float* dstPtr = dst.ToFloatPtr();
	int k = numColumns;
	int l = a.numColumns;
	for( int i = 0; i < k; i++ )
	{
		for( int j = 0; j < l; j++ )
		{
			const float* m1Ptr = ToFloatPtr() + i;
			const float* m2Ptr = a.ToFloatPtr() + j;
			float sum = m1Ptr[0] * m2Ptr[0];
			for( int n = 1; n < numRows; n++ )
			{
				m1Ptr += numColumns;
				m2Ptr += a.numColumns;
				sum += m1Ptr[0] * m2Ptr[0];
			}
			*dstPtr++ = sum;
		}
	}
}

/*
========================
budMatX::GetDimension
========================
*/
BUD_INLINE int budMatX::GetDimension() const
{
	return numRows * numColumns;
}

/*
========================
budMatX::SubVec6
========================
*/
BUD_INLINE const budVec6& budMatX::SubVec6( int row ) const
{
	assert( numColumns >= 6 && row >= 0 && row < numRows );
	return *reinterpret_cast<const budVec6*>( mat + row * numColumns );
}

/*
========================
budMatX::SubVec6
========================
*/
BUD_INLINE budVec6& budMatX::SubVec6( int row )
{
	assert( numColumns >= 6 && row >= 0 && row < numRows );
	return *reinterpret_cast<budVec6*>( mat + row * numColumns );
}

/*
========================
budMatX::SubVecX
========================
*/
BUD_INLINE const budVecX budMatX::SubVecX( int row ) const
{
	budVecX v;
	assert( row >= 0 && row < numRows );
	v.SetData( numColumns, mat + row * numColumns );
	return v;
}

/*
========================
budMatX::SubVecX
========================
*/
BUD_INLINE budVecX budMatX::SubVecX( int row )
{
	budVecX v;
	assert( row >= 0 && row < numRows );
	v.SetData( numColumns, mat + row * numColumns );
	return v;
}

/*
========================
budMatX::ToFloatPtr
========================
*/
BUD_INLINE const float* budMatX::ToFloatPtr() const
{
	return mat;
}

/*
========================
budMatX::ToFloatPtr
========================
*/
BUD_INLINE float* budMatX::ToFloatPtr()
{
	return mat;
}

#endif // !__MATH_MATRIXX_H__
