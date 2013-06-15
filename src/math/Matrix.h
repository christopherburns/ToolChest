/* ************************************************************************* *\

                  INTEL CORPORATION PROPRIETARY INFORMATION
     This software is supplied under the terms of a license agreement or
     nondisclosure agreement with Intel Corporation and may not be copied
     or disclosed except in accordance with the terms of that agreement.
     Copyright (C) 2011 Intel Corporation. All Rights Reserved.

     Author: Burns, Christopher A.

\* ************************************************************************* */

#ifndef MATRIX_H
#define MATRIX_H

#pragma once

////////////////////////////////////////////////////////////////////////////////
/// Generic Matrix Class: m[column][row]
////////////////////////////////////////////////////////////////////////////////


namespace Burns
{

template <class T, uint32 ROWS, uint32 COLS> class Matrix
{
public:
	//////////////////
	// Constructors //
	//////////////////

	FINLINE Matrix() { }
	FINLINE explicit Matrix(const T& v) { setDiagonal(v); }
	FINLINE explicit Matrix(const T* v)
   { for (Index i = 0; i < ROWS*COLS; i++) unroll()[i] = v[i]; }

	FINLINE explicit Matrix(const Vector<T, ROWS*COLS>& v) { unroll() = v; }

	template <class T1>	FINLINE explicit Matrix(const Matrix<T1, ROWS, COLS>& v)
   { unroll() = Vector<T, ROWS*COLS>(v.unroll()); }


	///////////////
	// Accessors //
	///////////////

	FINLINE const Vector<T,ROWS>& col(Index i) const { return c[i]; }
	FINLINE       Vector<T,ROWS>& col(Index i)       { return c[i]; }
	FINLINE const Vector<T,COLS>  row(Index i) const
   {
      T r[COLS];
      for (Index j = 0; j < COLS; ++j) r[j] = c[j][i];
      return Vector<T, COLS>(r);
   }

	FINLINE void setCol(Index i, const Vector<T,ROWS>& v) { c[i] = v; }
	FINLINE void setRow(Index i, const Vector<T,COLS>& v) { for (Index j = 0; j < COLS; j++) c[j][i] = v[j]; }
	FINLINE void setCol(Index i, const T& v) { for (Index j = 0; j < ROWS; j++) c[i][j] = v; }
	FINLINE void setRow(Index i, const T& v) { for (Index j = 0; j < COLS; j++) c[j][i] = v; }
	FINLINE void setAll(const T& v) { for (Index i=0; i<ROWS*COLS; i++) unroll()[i] = v; }
	FINLINE void setDiagonal(const T& v)
   {
      for (Index i = 0; i < COLS; i++)
         for (Index j = 0; j < ROWS; j++)
            c[i][j] = (i == j ? v : ZERO<T>());
   }

	FINLINE const Vector<T,ROWS*COLS>& unroll() const { return *(const Vector<T,ROWS*COLS>*)this; }
	FINLINE       Vector<T,ROWS*COLS>& unroll()       { return *(Vector<T,ROWS*COLS>*)this; }

	FINLINE const Vector<T,ROWS>& operator[](const Index col) const { ASSERT(col >= 0 && col < COLS); return c[col]; }
	FINLINE       Vector<T,ROWS>& operator[](const Index col)       { ASSERT(col >= 0 && col < COLS); return c[col]; }

   INLINE String toString() const
   {
      String s;
      for (Index i = 0; i < ROWS; ++i) s += row(i).toString() + "\n";
      return s;
   }

private:

   /// Column major order:   c[column][row];
   Vector<Vector<T, ROWS>, COLS> c;
};


///////////////////////////////////////////////////////////////////////////
//                             Mathematics                               //
///////////////////////////////////////////////////////////////////////////

/// See Mathematics.h for complete interface specification
template <class T, uint32 ROWS, uint32 COLS> struct MATHEMATICS<Matrix<T, ROWS, COLS> >
{
private:
   typedef Matrix<T, ROWS, COLS> Mat;

public:
   static FINLINE Mat zero() { return Mat(ZERO<T>()); }
   static FINLINE Mat one()  { return Mat(ONE<T>()); }
   static FINLINE Mat inf()  { Mat m; m.setAll(INF<T>()); return m; }
   static FINLINE Mat _inf() { Mat m; m.setAll(_INF<T>()); return m; }

   /// Min and Max
   static FINLINE Mat min(const Mat a, const Mat b)
   { return Mat(MIN(a.unroll(), b.unroll())); }
   static FINLINE Mat max(const Mat a, const Mat b)
   { return Mat(MAX(a.unroll(), b.unroll())); }

   /// Unary Functions
   static FINLINE const Mat abs      (const Mat& a) { return Mat(ABS  (a.unroll())); }
   static FINLINE const Mat rcp      (const Mat& a) { return Mat(RCP  (a.unroll())); }
   static FINLINE const Mat rcpSqrt  (const Mat& a) { return Mat(RCP_SQRT(a.unroll())); }
   static FINLINE const Mat sqrt     (const Mat& a) { return Mat(SQRT (a.unroll())); }
};

////////////////////////////////////////////////////////////////////////////////
/// Unary Operators
////////////////////////////////////////////////////////////////////////////////

template <class T, uint32 ROWS, uint32 COLS> FINLINE const Matrix<T,ROWS,COLS> operator+(const Matrix<T,ROWS,COLS>& a) { return Matrix<T,ROWS,COLS>(+a.unroll()); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE const Matrix<T,ROWS,COLS> operator-(const Matrix<T,ROWS,COLS>& a) { return Matrix<T,ROWS,COLS>(-a.unroll()); }

////////////////////////////////////////////////////////////////////////////////
/// Binary Operators
////////////////////////////////////////////////////////////////////////////////

template <class T, uint32 ROWS, uint32 COLS> FINLINE const Matrix<T,ROWS,COLS> operator+(const Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { return Matrix<T,ROWS,COLS>(a.unroll() + b.unroll()); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE const Matrix<T,ROWS,COLS> operator-(const Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { return Matrix<T,ROWS,COLS>(a.unroll() - b.unroll()); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE const Matrix<T,ROWS,COLS> operator*(const T& a,               const Matrix<T,ROWS,COLS>& b) { return Matrix<T,ROWS,COLS>(a * b.unroll()); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE const Matrix<T,ROWS,COLS> operator*(const Matrix<T,ROWS,COLS>& a, const T& b           ) { return Matrix<T,ROWS,COLS>(a.unroll() * b); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE const Matrix<T,ROWS,COLS> mul(const Matrix<T,ROWS,COLS>& a      , const Matrix<T,ROWS,COLS>& b) { return Matrix<T,ROWS,COLS>(a.unroll() * b.unroll()); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE const Matrix<T,ROWS,COLS> div(const Matrix<T,ROWS,COLS>& a      , const Matrix<T,ROWS,COLS>& b) { return Matrix<T,ROWS,COLS>(a.unroll() / b.unroll()); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE const Matrix<T,ROWS,COLS> operator/(const Matrix<T,ROWS,COLS>& a, const T& b           ) { return Matrix<T,ROWS,COLS>(a.unroll() / b); }

//template <class T, uint32 ROWS, uint32 COLS> FINLINE const Matrix<T,ROWS,COLS> min(const Matrix<T,ROWS,COLS> a, const Matrix<T,ROWS,COLS> b) { return Matrix<T,ROWS,COLS>(min(a.unroll(),b.unroll())); }
//template <class T, uint32 ROWS, uint32 COLS> FINLINE const Matrix<T,ROWS,COLS> max(const Matrix<T,ROWS,COLS> a, const Matrix<T,ROWS,COLS> b) { return Matrix<T,ROWS,COLS>(max(a.unroll(),b.unroll())); }

//////////////////////////
// Assignment Operators //
//////////////////////////

template <class T, uint32 ROWS, uint32 COLS> FINLINE Matrix<T,ROWS,COLS>& operator+=(Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { a.unroll() += b.unroll(); return a; }
template <class T, uint32 ROWS, uint32 COLS> FINLINE Matrix<T,ROWS,COLS>& operator-=(Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { a.unroll() -= b.unroll(); return a; }
template <class T, uint32 ROWS, uint32 COLS> FINLINE Matrix<T,ROWS,COLS>& operator*=(Matrix<T,ROWS,COLS>& a, const T& b) { a.unroll() *= b; return a; }
template <class T, uint32 ROWS, uint32 COLS> FINLINE Matrix<T,ROWS,COLS>& operator/=(Matrix<T,ROWS,COLS>& a, const T& b) { a.unroll() /= b; return a; }

/////////////////////////
// Reduction Operators //
/////////////////////////

template <class T, uint32 ROWS, uint32 COLS> FINLINE const T reduce_add(const Matrix<T,ROWS,COLS>& a) { return reduce_add(a.unroll()); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE const T reduce_mul(const Matrix<T,ROWS,COLS>& a) { return reduce_mul(a.unroll()); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE const T reduce_min(const Matrix<T,ROWS,COLS>& a) { return reduce_min(a.unroll()); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE const T reduce_max(const Matrix<T,ROWS,COLS>& a) { return reduce_max(a.unroll()); }

//////////////////////////
// Comparison Operators //
//////////////////////////

template <class T, uint32 ROWS, uint32 COLS> FINLINE bool operator ==(const Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { return a.unroll() == b.unroll(); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE bool operator !=(const Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { return a.unroll() != b.unroll(); }
template <class T, uint32 ROWS, uint32 COLS> FINLINE bool operator  <(const Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { return a.unroll()  < b.unroll(); }

////////////////////
// Linear Algebra //
////////////////////

/* transpose Matrix */
template <class T, uint32 ROWS, uint32 COLS>
FINLINE const Matrix<T,COLS,ROWS> transpose(const Matrix<T,ROWS,COLS>& m)
{ Matrix<T,COLS,ROWS> mt; for (int i = 0; i < ROWS; i++) mt[i] = m.row(i); return mt; }

/* Matrix with column vector multiplication */
template <class T, uint32 ROWS, uint32 COLS>
FINLINE const Vector<T,ROWS> operator*(const Matrix<T,ROWS,COLS>& m, const Vector<T,COLS>& v)
{ Vector<T,ROWS> r = v[0] * m[0]; for (int i = 1; i < COLS; i++)	r += v[i] * m[i];	return r; }

/* row vector with Matrix multiplication */
template <class T, uint32 ROWS, uint32 COLS>
FINLINE const Vector<T,COLS> operator*(const Vector<T,ROWS>& v, const Matrix<T,ROWS,COLS>& m)
{ Vector<T,COLS> r; for (int i = 0; i < COLS; i++) r[i] = Vector<T,ROWS>::dot(v, m[i]); return r; }

/* Matrix Matrix multiplication */
template <class T, uint32 ROWS, uint32 COLS1, uint32 COLS2>
FINLINE const Matrix<T,ROWS,COLS2> operator*(const Matrix<T,ROWS,COLS1>& m1, const Matrix<T,COLS1,COLS2>& m2)
{
	Matrix<T,ROWS,COLS2> r;
	for (Index j = 0; j < COLS2; j++)
		for (Index i = 0; i < ROWS; i++)
         r[j][i] = Vector<T,COLS1>::dot(m1.row(i), m2.col(j));
	return r;
}

/* Determinant through Gauss elimination.*/
template <class T, uint32 N>
FINLINE T det(const Matrix<T,N,N>& m)
{
	Matrix<T,N,N> mi(m);
	T determinant = ONE<T>();

	for (Index k = 0; k < N; k++)
	{
		determinant *= mi[k][k];

		for (Index i = 0; i < N; i++) if (i != k)
      {
			mi[i][k] *= -RCP(mi[k][k]);
			mi[k][i] *= +RCP(mi[k][k]);
		}

		for (Index i = 0; i < N; i++) if (i != k)
			for (Index j = 0; j < N; j++) if (j != k)
				mi[i][j] += mi[i][k] * mi[k][j] * mi[k][k];

		mi[k][k] = RCP(mi[k][k]);
	}
	return determinant;
}

/* Matrix inversion through Gauss elimination. */
template <class T, uint32 N>
const Matrix<T,N,N> gaussInvert(const Matrix<T,N,N>& m)
{
	Matrix<T,N,N> mi(m);

	for (Index k = 0; k < N; k++)
	{
		/* if the Matrix has no inverse, continue to simplify
			 constrained regression implementations */
		if (ABS(mi[k][k]) < T(1.0e-15))
      {
			mi[k][k] = ZERO<T>();
			continue;
		}

		for (Index i = 0; i < N; i++) if (i != k)
      {
			mi[i][k] *= -RCP(mi[k][k]);
			mi[k][i] *= +RCP(mi[k][k]);
		}

		for (Index i = 0; i < N; i++) if (i != k)
		  for (Index j = 0; j < N; j++) if (j != k)
			  mi[i][j] += mi[i][k] * mi[k][j] * mi[k][k];

		mi[k][k] = RCP(mi[k][k]);
	}
	return mi;
}

template <class T, uint32 N>
FINLINE const Matrix<T,N,N> invert(const Matrix<T,N,N>& m)
{ return gaussInvert(m); }

///////////////////////////////
// 2x2 Matrix specialization //
///////////////////////////////

template <class T>
FINLINE T det(const Matrix<T,2,2>& m)
{ return m[0][0]*m[1][1] - m[0][1]*m[1][0]; }

template <class T>
FINLINE const Matrix<T,2,2> invert(const Matrix<T,2,2>& m)
{
	Matrix<T,2,2> mi;
	T di = RCP(det(m));
	mi[0][0] =  di * m[1][1];
	mi[0][1] = -di * m[0][1];
	mi[1][0] = -di * m[1][0];
	mi[1][1] =  di * m[0][0];
	return mi;
}

///////////////////////////////
// 3x3 Matrix specialization //
///////////////////////////////

template <class T> FINLINE T det(const Matrix<T,3,3>& m)
{ return Vector<T, 3>::dot(m[0], cross(m[1], m[2])); }

template <class T>
FINLINE const Matrix<T,3,3> invert(const Matrix<T,3,3>& m)
{
	Matrix<T,3,3> mi;
	mi.setRow(0, cross(m[1],m[2]));
	mi.setRow(1, cross(m[2],m[0]));
	mi.setRow(2, cross(m[0],m[1]));
	mi *= RCP(det(m));
	return mi;
}

template <class T> FINLINE const Vector<T,3> xfmPoint (const Matrix<T,3,3>& m, const Vector<T,3>& v) { return m*v; }
template <class T> FINLINE const Vector<T,3> xfmVector(const Matrix<T,3,3>& m, const Vector<T,3>& v) { return m*v; }
template <class T> FINLINE const Vector<T,3> xfmNormal(const Matrix<T,3,3>& m, const Vector<T,3>& v) { return v*invert(m); }

template <class T> FINLINE const Vector<T,3> xfmPoint (const Matrix<T,3,4>& m, const Vector<T,3>& v) { return ((const Matrix<T,3,3>&)m).xfmPoint(v) + m[3]; }
template <class T> FINLINE const Vector<T,3> xfmVector(const Matrix<T,3,4>& m, const Vector<T,3>& v) { return ((const Matrix<T,3,3>&)m).xfmVector(v); }
template <class T> FINLINE const Vector<T,3> xfmNormal(const Matrix<T,3,4>& m, const Vector<T,3>& v) { return ((const Matrix<T,3,3>&)m).xfmNormal(v); }

//////////////////////
// Output Operators //
//////////////////////

template <class T, uint32 ROWS, uint32 COLS>
FINLINE std::ostream& operator << (std::ostream& s, const Matrix<T,ROWS,COLS>& m)
{
	s << "[";
	for (int i = 0; i < ROWS-1; i++) s << m.row(i) << std::endl;
	s << m.row(ROWS-1) << "]";
	return s;
}

typedef Matrix<float,2,2> Matrix2f; /// a Matrix of 2x2 floats
typedef Matrix<float,3,3> Matrix3f; /// a Matrix of 3x3 floats
typedef Matrix<float,4,4> Matrix4f; /// a Matrix of 4x4 floats


}; // namespace Burns


#endif // MATRIX_H