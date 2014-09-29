#ifndef MATRIX_H
#define MATRIX_H

///////////////////////////////////////////
/// Generic Matrix Class: m[Column][row] //
///////////////////////////////////////////

namespace Mathematics
{

	template <class T, uint32_t ROWS, uint32_t COLS> class Matrix
	{
	public:
		//////////////////
		// Constructors //
		//////////////////

		inline Matrix() { }
		inline explicit Matrix(const T& v) { SetDiagonal(v); }
		inline explicit Matrix(const T* v)
	   { for (int i = 0; i < ROWS*COLS; i++) Unroll()[i] = v[i]; }

		inline explicit Matrix(const Vector<T, ROWS*COLS>& v) { Unroll() = v; }

		template <class T1>	inline explicit Matrix(const Matrix<T1, ROWS, COLS>& v)
	   { Unroll() = Vector<T, ROWS*COLS>(v.Unroll()); }


		///////////////
		// Accessors //
		///////////////

		inline const Vector<T,ROWS>& Col(int i) const { return c[i]; }
		inline       Vector<T,ROWS>& Col(int i)       { return c[i]; }
		inline const Vector<T,COLS>  Row(int i) const
	   {
	      T r[COLS];
	      for (int j = 0; j < COLS; ++j) r[j] = c[j][i];
	      return Vector<T, COLS>(r);
	   }

		inline void SetCol(int i, const Vector<T,ROWS>& v) { c[i] = v; }
		inline void SetRow(int i, const Vector<T,COLS>& v) { for (int j = 0; j < COLS; j++) c[j][i] = v[j]; }
		inline void SetCol(int i, const T& v) { for (int j = 0; j < ROWS; j++) c[i][j] = v; }
		inline void SetRow(int i, const T& v) { for (int j = 0; j < COLS; j++) c[j][i] = v; }
		inline void SetAll(const T& v) { for (int i=0; i<ROWS*COLS; i++) Unroll()[i] = v; }
		inline void SetDiagonal(const T& v)
	   {
	      for (int i = 0; i < COLS; i++)
	         for (int j = 0; j < ROWS; j++)
	            c[i][j] = (i == j ? v : ZERO<T>());
	   }

		inline const Vector<T,ROWS*COLS>& Unroll() const { return *(const Vector<T,ROWS*COLS>*)this; }
		inline       Vector<T,ROWS*COLS>& Unroll()       { return *(Vector<T,ROWS*COLS>*)this; }

		inline const Vector<T,ROWS>& operator[](const int Col) const { assert(Col >= 0 && Col < COLS); return c[Col]; }
		inline       Vector<T,ROWS>& operator[](const int Col)       { assert(Col >= 0 && Col < COLS); return c[Col]; }

	   inline std::string ToString() const
	   {
	      std::string s;
	      for (int i = 0; i < ROWS; ++i) s += Row(i).ToString() + "\n";
	      return s;
	   }

	private:

	   /// Column major order:   c[Column][row];
	   Vector<Vector<T, ROWS>, COLS> c;
	};


	///////////////////////////////////////////////////////////////////////////
	//                             Mathematics                               //
	///////////////////////////////////////////////////////////////////////////

	/// See Mathematics.h for complete interface specification
	template <class T, uint32_t ROWS, uint32_t COLS> struct MATHEMATICS<Matrix<T, ROWS, COLS> >
	{
	private:
	   typedef Matrix<T, ROWS, COLS> Mat;

	public:
	   static inline Mat zero() { return Mat(ZERO<T>()); }
	   static inline Mat one()  { return Mat(ONE<T>()); }
	   static inline Mat inf()  { Mat m; m.SetAll(INF<T>()); return m; }
	   static inline Mat _inf() { Mat m; m.SetAll(_INF<T>()); return m; }

	   /// Min and Max
	   static inline Mat min(const Mat a, const Mat b)
	   { return Mat(MIN(a.Unroll(), b.Unroll())); }
	   static inline Mat max(const Mat a, const Mat b)
	   { return Mat(MAX(a.Unroll(), b.Unroll())); }

	   /// Unary Functions
	   static inline const Mat abs      (const Mat& a) { return Mat(ABS  (a.Unroll())); }
	   static inline const Mat rcp      (const Mat& a) { return Mat(RCP  (a.Unroll())); }
	   static inline const Mat rcpSqrt  (const Mat& a) { return Mat(RCP_SQRT(a.Unroll())); }
	   static inline const Mat sqrt     (const Mat& a) { return Mat(SQRT (a.Unroll())); }
	};

	////////////////////////////////////////////////////////////////////////////////
	/// Unary Operators
	////////////////////////////////////////////////////////////////////////////////

	template <class T, uint32_t ROWS, uint32_t COLS> inline const Matrix<T,ROWS,COLS> operator+(const Matrix<T,ROWS,COLS>& a) { return Matrix<T,ROWS,COLS>(+a.Unroll()); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline const Matrix<T,ROWS,COLS> operator-(const Matrix<T,ROWS,COLS>& a) { return Matrix<T,ROWS,COLS>(-a.Unroll()); }

	////////////////////////////////////////////////////////////////////////////////
	/// Binary Operators
	////////////////////////////////////////////////////////////////////////////////

	template <class T, uint32_t ROWS, uint32_t COLS> inline const Matrix<T,ROWS,COLS> operator+(const Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { return Matrix<T,ROWS,COLS>(a.Unroll() + b.Unroll()); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline const Matrix<T,ROWS,COLS> operator-(const Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { return Matrix<T,ROWS,COLS>(a.Unroll() - b.Unroll()); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline const Matrix<T,ROWS,COLS> operator*(const T& a,               const Matrix<T,ROWS,COLS>& b) { return Matrix<T,ROWS,COLS>(a * b.Unroll()); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline const Matrix<T,ROWS,COLS> operator*(const Matrix<T,ROWS,COLS>& a, const T& b           ) { return Matrix<T,ROWS,COLS>(a.Unroll() * b); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline const Matrix<T,ROWS,COLS> mul(const Matrix<T,ROWS,COLS>& a      , const Matrix<T,ROWS,COLS>& b) { return Matrix<T,ROWS,COLS>(a.Unroll() * b.Unroll()); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline const Matrix<T,ROWS,COLS> div(const Matrix<T,ROWS,COLS>& a      , const Matrix<T,ROWS,COLS>& b) { return Matrix<T,ROWS,COLS>(a.Unroll() / b.Unroll()); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline const Matrix<T,ROWS,COLS> operator/(const Matrix<T,ROWS,COLS>& a, const T& b           ) { return Matrix<T,ROWS,COLS>(a.Unroll() / b); }

	//template <class T, uint32_t ROWS, uint32_t COLS> inline const Matrix<T,ROWS,COLS> min(const Matrix<T,ROWS,COLS> a, const Matrix<T,ROWS,COLS> b) { return Matrix<T,ROWS,COLS>(min(a.Unroll(),b.Unroll())); }
	//template <class T, uint32_t ROWS, uint32_t COLS> inline const Matrix<T,ROWS,COLS> max(const Matrix<T,ROWS,COLS> a, const Matrix<T,ROWS,COLS> b) { return Matrix<T,ROWS,COLS>(max(a.Unroll(),b.Unroll())); }

	//////////////////////////
	// Assignment Operators //
	//////////////////////////

	template <class T, uint32_t ROWS, uint32_t COLS> inline Matrix<T,ROWS,COLS>& operator+=(Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { a.Unroll() += b.Unroll(); return a; }
	template <class T, uint32_t ROWS, uint32_t COLS> inline Matrix<T,ROWS,COLS>& operator-=(Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { a.Unroll() -= b.Unroll(); return a; }
	template <class T, uint32_t ROWS, uint32_t COLS> inline Matrix<T,ROWS,COLS>& operator*=(Matrix<T,ROWS,COLS>& a, const T& b) { a.Unroll() *= b; return a; }
	template <class T, uint32_t ROWS, uint32_t COLS> inline Matrix<T,ROWS,COLS>& operator/=(Matrix<T,ROWS,COLS>& a, const T& b) { a.Unroll() /= b; return a; }

	/////////////////////////
	// Reduction Operators //
	/////////////////////////

	template <class T, uint32_t ROWS, uint32_t COLS> inline const T reduce_add(const Matrix<T,ROWS,COLS>& a) { return reduce_add(a.Unroll()); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline const T reduce_mul(const Matrix<T,ROWS,COLS>& a) { return reduce_mul(a.Unroll()); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline const T reduce_min(const Matrix<T,ROWS,COLS>& a) { return reduce_min(a.Unroll()); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline const T reduce_max(const Matrix<T,ROWS,COLS>& a) { return reduce_max(a.Unroll()); }

	//////////////////////////
	// Comparison Operators //
	//////////////////////////

	template <class T, uint32_t ROWS, uint32_t COLS> inline bool operator ==(const Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { return a.Unroll() == b.Unroll(); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline bool operator !=(const Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { return a.Unroll() != b.Unroll(); }
	template <class T, uint32_t ROWS, uint32_t COLS> inline bool operator  <(const Matrix<T,ROWS,COLS>& a, const Matrix<T,ROWS,COLS>& b) { return a.Unroll()  < b.Unroll(); }

	////////////////////
	// Linear Algebra //
	////////////////////

	/* transpose Matrix */
	template <class T, uint32_t ROWS, uint32_t COLS>
	inline const Matrix<T,COLS,ROWS> Transpose(const Matrix<T,ROWS,COLS>& m)
	{ Matrix<T,COLS,ROWS> mt; for (int i = 0; i < ROWS; i++) mt[i] = m.Row(i); return mt; }

	/* Matrix with Column vector multiplication */
	template <class T, uint32_t ROWS, uint32_t COLS>
	inline const Vector<T,ROWS> operator*(const Matrix<T,ROWS,COLS>& m, const Vector<T,COLS>& v)
	{ Vector<T,ROWS> r = v[0] * m[0]; for (int i = 1; i < COLS; i++)	r += v[i] * m[i];	return r; }

	/* row vector with Matrix multiplication */
	template <class T, uint32_t ROWS, uint32_t COLS>
	inline const Vector<T,COLS> operator*(const Vector<T,ROWS>& v, const Matrix<T,ROWS,COLS>& m)
	{ Vector<T,COLS> r; for (int i = 0; i < COLS; i++) r[i] = Vector<T,ROWS>::Dot(v, m[i]); return r; }

	/* Matrix Matrix multiplication */
	template <class T, uint32_t ROWS, uint32_t COLS1, uint32_t COLS2>
	inline const Matrix<T,ROWS,COLS2> operator*(const Matrix<T,ROWS,COLS1>& m1, const Matrix<T,COLS1,COLS2>& m2)
	{
		Matrix<T,ROWS,COLS2> r;
		for (int j = 0; j < COLS2; j++)
			for (int i = 0; i < ROWS; i++)
	         r[j][i] = Vector<T,COLS1>::Dot(m1.Row(i), m2.Col(j));
		return r;
	}

	/* Determinant through Gauss elimination.*/
	template <class T, uint32_t N>
	inline T Det(const Matrix<T,N,N>& m)
	{
		Matrix<T,N,N> mi(m);
		T determinant = ONE<T>();

		for (int k = 0; k < N; k++)
		{
			determinant *= mi[k][k];

			for (int i = 0; i < N; i++) if (i != k)
	      {
				mi[i][k] *= -RCP(mi[k][k]);
				mi[k][i] *= +RCP(mi[k][k]);
			}

			for (int i = 0; i < N; i++) if (i != k)
				for (int j = 0; j < N; j++) if (j != k)
					mi[i][j] += mi[i][k] * mi[k][j] * mi[k][k];

			mi[k][k] = RCP(mi[k][k]);
		}
		return determinant;
	}

	/* Matrix inversion through Gauss elimination. */
	template <class T, uint32_t N>
	const Matrix<T,N,N> GaussInvert(const Matrix<T,N,N>& m)
	{
		Matrix<T,N,N> mi(m);

		for (int k = 0; k < N; k++)
		{
			/* if the Matrix has no inverse, continue to simplify
				 constrained regression implementations */
			if (ABS(mi[k][k]) < T(1.0e-15))
	      {
				mi[k][k] = ZERO<T>();
				continue;
			}

			for (int i = 0; i < N; i++) if (i != k)
	      {
				mi[i][k] *= -RCP(mi[k][k]);
				mi[k][i] *= +RCP(mi[k][k]);
			}

			for (int i = 0; i < N; i++) if (i != k)
			   for (int j = 0; j < N; j++) if (j != k)
				   mi[i][j] += mi[i][k] * mi[k][j] * mi[k][k];

			mi[k][k] = RCP(mi[k][k]);
		}
		return mi;
	}

	template <class T, uint32_t N>
	inline const Matrix<T,N,N> Invert(const Matrix<T,N,N>& m)
	{ return GaussInvert(m); }

	///////////////////////////////
	// 2x2 Matrix specialization //
	///////////////////////////////

	template <class T>
	inline T Det(const Matrix<T,2,2>& m)
	{ return m[0][0]*m[1][1] - m[0][1]*m[1][0]; }

	template <class T>
	inline const Matrix<T,2,2> Invert(const Matrix<T,2,2>& m)
	{
		Matrix<T,2,2> mi;
		T di = RCP(Det(m));
		mi[0][0] =  di * m[1][1];
		mi[0][1] = -di * m[0][1];
		mi[1][0] = -di * m[1][0];
		mi[1][1] =  di * m[0][0];
		return mi;
	}

	///////////////////////////////
	// 3x3 Matrix specialization //
	///////////////////////////////

	template <class T> inline T Det(const Matrix<T,3,3>& m)
	{ return Vector<T, 3>::Dot(m[0], Cross(m[1], m[2])); }

	template <class T>
	inline const Matrix<T,3,3> Invert(const Matrix<T,3,3>& m)
	{
		Matrix<T,3,3> mi;
		mi.SetRow(0, Cross(m[1],m[2]));
		mi.SetRow(1, Cross(m[2],m[0]));
		mi.SetRow(2, Cross(m[0],m[1]));
		mi *= RCP(Det(m));
		return mi;
	}

	template <class T> inline const Vector<T,3> XfmPoint (const Matrix<T,3,3>& m, const Vector<T,3>& v) { return m*v; }
	template <class T> inline const Vector<T,3> XfmVector(const Matrix<T,3,3>& m, const Vector<T,3>& v) { return m*v; }
	template <class T> inline const Vector<T,3> XfmNormal(const Matrix<T,3,3>& m, const Vector<T,3>& v) { return v*invert(m); }

	template <class T> inline const Vector<T,3> XfmPoint (const Matrix<T,3,4>& m, const Vector<T,3>& v) { return ((const Matrix<T,3,3>&)m).xfmPoint(v) + m[3]; }
	template <class T> inline const Vector<T,3> XfmVector(const Matrix<T,3,4>& m, const Vector<T,3>& v) { return ((const Matrix<T,3,3>&)m).xfmVector(v); }
	template <class T> inline const Vector<T,3> XfmNormal(const Matrix<T,3,4>& m, const Vector<T,3>& v) { return ((const Matrix<T,3,3>&)m).xfmNormal(v); }

	//////////////////////
	// Output Operators //
	//////////////////////

	template <class T, uint32_t ROWS, uint32_t COLS>
	inline std::ostream& operator << (std::ostream& s, const Matrix<T,ROWS,COLS>& m)
	{
		s << "[";
		for (int i = 0; i < ROWS-1; i++) s << m.Row(i) << std::endl;
		s << m.Row(ROWS-1) << "]";
		return s;
	}

	typedef Matrix<float,2,2> Matrix2f; /// a Matrix of 2x2 floats
	typedef Matrix<float,3,3> Matrix3f; /// a Matrix of 3x3 floats
	typedef Matrix<float,4,4> Matrix4f; /// a Matrix of 4x4 floats
}; // namespace Mathematics


#endif // MATRIX_H