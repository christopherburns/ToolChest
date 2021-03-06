#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include "Bits.h"
#include "Mathematics.h"
#include "Mask.h"

namespace Mathematics
{

template <class T, int N> class Vector;
template <class T> class WriteMaskedVector;


///////////////////////////////////////////////////////////////////////////
//                               TypeInfo                                //
///////////////////////////////////////////////////////////////////////////

template <class T, int N> class TypeInfo<Vector<T, N> >
{
public:
   static const bool Integral = TypeInfo<T>::Integral;
   static const bool Unsigned = TypeInfo<T>::Unsigned;
   static const int  Rank = 1;
   static const int  Width = N;

   typedef T ElementType;
   typedef Mask<typename TypeInfo<T>::Mask, N> Mask;
};


///////////////////////////////////////////////////////////////////////////
//                         Blending and Masking                          //
///////////////////////////////////////////////////////////////////////////


template <class T> inline T& Update(const bool mask, T& lhs, const T& rhs)
{ if (mask) lhs = rhs; return lhs; }

template <class T> inline const T Blend(const bool mask, const T& tValue, const T& fValue)
{ return mask ? tValue : fValue; }

template <class T> class WriteMaskedVector
{
public:
   typedef typename TypeInfo<T>::Mask Mask;
   friend inline T& Update(const bool mask, T& lhs, const T& rhs);

private:
   T& _value;
   const Mask& _mask;

public:
   inline WriteMaskedVector(const WriteMaskedVector&);
   inline WriteMaskedVector(T& value, const Mask& m) : _value(value), _mask(m) {}

   inline WriteMaskedVector& operator = (const WriteMaskedVector&);
   inline T& operator = (const T& i) { return Update(_mask, _value, i); }
};


template <class T, int N> class Vector
{
   ///////////////////////////////////////////////////////////////////////////
   //                           Public Interface                            //
   ///////////////////////////////////////////////////////////////////////////

public:

   /// Define our mask type
   typedef typename TypeInfo<Vector>::Mask Mask;

   /// Constructors
   inline Vector();                       ///< NO initialization
   inline explicit Vector(const T& t);    ///< Initialize all elements from scalar
   inline explicit Vector(const T t[N]);  ///< Initialize from memory location

   /// Conversion from different base type
   template <class S> explicit inline Vector(const Vector<S, N>& v);

   /// Constructors for common partial specializations
   inline Vector(const T& t0, const T& t1);                           // N == 2
   inline Vector(const T& t0, const T& t1, const T& t2);              // N == 3
   inline Vector(const T& t0, const T& t1, const T& t2, const T& t3); // N == 4
   inline Vector(
      const T& f0,  const T& f1,  const T& f2,  const T& f3,
      const T& f4,  const T& f5,  const T& f6,  const T& f7);
   inline Vector(
      const T& f0,  const T& f1,  const T& f2,  const T& f3,
      const T& f4,  const T& f5,  const T& f6,  const T& f7,
      const T& f8,  const T& f9,  const T& f10, const T& f11,
      const T& f12, const T& f13, const T& f14, const T& f15);

   // Expansion pseudo-constructors
   inline static Vector Expansion2x(const Vector<T, N/2>& t0, const Vector<T, N/2>& t1);
   inline static Vector Expansion4x(const Vector<T, N/4>& t0, const Vector<T, N/4>& t1, const Vector<T, N/4>& t2, const Vector<T, N/4>& t3);


   /// Dynamic Element Accessors
   inline const T& operator [] (int i) const;
   inline T& operator [] (int i);

   /// Static element accessors
   template <int I> inline const T& Get() const { return (I < WA) ? (_a.template Get<I>()) : (_b.template Get<I-WA>()); }
   template <int I> inline T& Get()             { return (I < WA) ? (_a.template Get<I>()) : (_b.template Get<I-WA>()); }
   template <int I> inline void Set(const T& v) { if     (I < WA) _a.Set<I>(v); else _b.Set<I-WA>(v); }

   /// Convenience static element accessors for Vector 2, 3
   inline const T& x() const { /*STATIC_assert(N >= 1);*/ return Get<0>(); }
   inline T& x()             { /*STATIC_assert(N >= 1);*/ return Get<0>(); }
   inline const T& y() const { /*STATIC_assert(N >= 2);*/ return Get<1>(); }
   inline T& y()             { /*STATIC_assert(N >= 2);*/ return Get<1>(); }
   inline const T& z() const { /*STATIC_assert(N >= 3);*/ return Get<2>(); }
   inline T& z()             { /*STATIC_assert(N >= 3);*/ return Get<2>(); }
   inline const T& w() const { /*STATIC_assert(N >= 4);*/ return Get<3>(); }
   inline T& w()             { /*STATIC_assert(N >= 4);*/ return Get<3>(); }

   inline const Vector<T, 2> xy()  const { /*STATIC_assert(N >= 2);*/ return Vector<T, 2>(x(), y()); }
   inline const Vector<T, 3> xyz() const { /*STATIC_assert(N >= 3);*/ return Vector<T, 3>(x(), y(), z()); }


   std::string ToString(int prec) const;
   std::string ToString() const;

   /// Insertion and extraction operators for C++ I/O Streams
   friend inline std::ostream& operator << (std::ostream& stream, const Vector& v)
   { stream << v.ToString(); return stream; }
   friend inline std::istream& operator >> (std::istream& stream, Vector& v)
   {
      char c;
      stream >> c;   // '['
      v._a.internalStreamIn(stream);
      stream >> c;   // ','
      v._b.internalStreamIn(stream);
      stream >> c;   // ']'
      return stream;
   }
   inline std::istream& internalStreamIn(std::istream& stream);



   //////////////////////
   // Loads and Stores //
   //////////////////////

   template <class U> inline static Vector Load(U * address);
   template <class U> inline static Vector LoadBroadcast(U * address);
   template <class U> inline static Vector Gather(U * address, const Vector<int32_t, N>& offsets);
   template <class U> inline static Vector Gather(U * address, const Vector<int32_t, N>& offsets, const Mask& mask);

   template <class U> inline void Store(U * address) const;
   template <class U> inline void StoreOne(U * address) const;
   template <class U> inline void Scatter(U * address, const Vector<int32_t, N>& offsets) const;
   template <class U> inline void Scatter(U * address, const Vector<int32_t, N>& offsets, const Mask& mask) const;


   ////////////////////////////
   // Mathematical Operators //
   ////////////////////////////

   /// Unary minus
   inline Vector operator - () const                                       { return Vector<T, N>(-_a, -_b); }

   /// Binary arithmetical operators
   inline friend Vector operator + (const Vector& a, const Vector& b)      { return Vector(VectorA(a._a + b._a), VectorB(a._b + b._b)); }
   inline friend Vector operator - (const Vector& a, const Vector& b)      { return Vector(VectorA(a._a - b._a), VectorB(a._b - b._b)); }
   inline friend Vector operator * (const Vector& a, const Vector& b)      { return Vector(VectorA(a._a * b._a), VectorB(a._b * b._b)); }
   inline friend Vector operator / (const Vector& a, const Vector& b)      { return Vector(VectorA(a._a / b._a), VectorB(a._b / b._b)); }
   inline friend Vector operator + (const Vector& a, const T& b)           { return Vector(VectorA(a._a + b), VectorB(a._b + b)); }
   inline friend Vector operator - (const Vector& a, const T& b)           { return Vector(VectorA(a._a - b), VectorB(a._b - b)); }
   inline friend Vector operator * (const Vector& a, const T& b)           { return Vector(VectorA(a._a * b), VectorB(a._b * b)); }
   inline friend Vector operator / (const Vector& a, const T& b)           { return Vector(VectorA(a._a / b), VectorB(a._b / b)); }
   inline friend Vector operator + (const T& a, const Vector& b)           { return Vector(VectorA(a + b._a), VectorB(a + b._b)); }
   inline friend Vector operator - (const T& a, const Vector& b)           { return Vector(VectorA(a - b._a), VectorB(a - b._b)); }
   inline friend Vector operator * (const T& a, const Vector& b)           { return Vector(VectorA(a * b._a), VectorB(a * b._b)); }
   inline friend Vector operator / (const T& a, const Vector& b)           { return Vector(VectorA(a / b._a), VectorB(a / b._b)); }


   /// Accumulation operators
   inline Vector& operator += (const Vector& b)                            { return *this = *this + b; }
   inline Vector& operator -= (const Vector& b)                            { return *this = *this - b; }
   inline Vector& operator *= (const Vector& b)                            { return *this = *this * b; }
   inline Vector& operator /= (const Vector& b)                            { return *this = *this / b; }
   inline Vector& operator += (const T& b)                                 { return *this = *this + b; }
   inline Vector& operator -= (const T& b)                                 { return *this = *this - b; }
   inline Vector& operator *= (const T& b)                                 { return *this = *this * b; }
   inline Vector& operator /= (const T& b)                                 { return *this = *this / b; }


   /////////////////////////////
   // Integer-Only Arithmetic //
   /////////////////////////////

   /// Bit shifting
   inline friend Vector operator >> (const Vector& a, uint32_t bits)      { /*STATIC_assert(TypeInfo<T>::Integral);*/ return Vector(VectorA(a._a >> bits),    VectorB(a._b >> bits)); }
   inline friend Vector operator << (const Vector& a, uint32_t bits)      { /*STATIC_assert(TypeInfo<T>::Integral);*/ return Vector(VectorA(a._a << bits),    VectorB(a._b << bits)); }
   inline friend Vector operator << (const Vector& a, const Vector& bits) { /*STATIC_assert(TypeInfo<T>::Integral);*/ return Vector(VectorA(a._a << bits._a), VectorB(a._b << bits._b)); }
   inline friend Vector operator >> (const Vector& a, const Vector& bits) { /*STATIC_assert(TypeInfo<T>::Integral);*/ return Vector(VectorA(a._a >> bits._a), VectorB(a._b >> bits._b)); }


   /// Integer Modulus
   inline friend Vector operator %  (const Vector& a, const Vector& b)     { /*STATIC_assert(TypeInfo<T>::Integral);*/ return Vector(VectorA(a._a % b._a), VectorB(a._b % b._b)); }
   inline friend Vector operator %  (const Vector& a, const T& b)          { /*STATIC_assert(TypeInfo<T>::Integral);*/ return Vector(VectorA(a._a % b), VectorB(a._b % b)); }

   inline Vector& operator <<= (const int bits)                            { return *this = *this << bits; }
   inline Vector& operator >>= (const int bits)                            { return *this = *this >> bits; }
   inline Vector& operator <<= (const Vector& bits)                        { return *this = *this << bits; }
   inline Vector& operator >>= (const Vector& bits)                        { return *this = *this >> bits; }


   ///////////////////////
   // Bitwise Operators //
   ///////////////////////

   inline Vector& operator ~ () const                                     { /*STATIC_assert(TypeInfo<T>::Integral);*/ return Vector(~_a, ~_b); }
   inline friend Vector operator & (const Vector& a, const Vector& bits)  { /*STATIC_assert(TypeInfo<T>::Integral);*/ return Vector(VectorA(a._a & bits._a), VectorB(a._b & bits._b)); }
   inline friend Vector operator | (const Vector& a, const Vector& bits)  { /*STATIC_assert(TypeInfo<T>::Integral);*/ return Vector(VectorA(a._a | bits._a), VectorB(a._b | bits._b)); }
   inline friend Vector operator ^ (const Vector& a, const Vector& bits)  { /*STATIC_assert(TypeInfo<T>::Integral);*/ return Vector(VectorA(a._a ^ bits._a), VectorB(a._b ^ bits._b)); }

   inline Vector& operator &= (const Vector& b)                           { return *this = *this & b; }
   inline Vector& operator |= (const Vector& b)                           { return *this = *this | b; }
   inline Vector& operator ^= (const Vector& b)                           { return *this = *this ^ b; }

   ////////////////
   // Comparison //
   ////////////////

   // Lighten up the FUCKING SYNTAX just a little
   inline friend typename Vector::Mask operator == (const Vector& a, const Vector& b) { return typename Vector::Mask(a._a == b._a, a._b == b._b); }
   inline friend typename Vector::Mask operator != (const Vector& a, const Vector& b) { return typename Vector::Mask(a._a != b._a, a._b != b._b); }
   inline friend typename Vector::Mask operator <  (const Vector& a, const Vector& b) { return typename Vector::Mask(a._a < b._a, a._b < b._b); }
   inline friend typename Vector::Mask operator >  (const Vector& a, const Vector& b) { return typename Vector::Mask(a._a > b._a, a._b > b._b); }
   inline friend typename Vector::Mask operator <= (const Vector& a, const Vector& b) { return typename Vector::Mask(a._a <= b._a, a._b <= b._b); }
   inline friend typename Vector::Mask operator >= (const Vector& a, const Vector& b) { return typename Vector::Mask(a._a >= b._a, a._b >= b._b); }


   //////////////////////////
   // Blending and Masking //
   //////////////////////////

   /// Returns a vector blending the values from tValue where m is true with the
   /// values from fValue where m is false:
   ///     blend( (t,t,f,t), (1,2,3,4), (-1,-2,-3,-4) ) ==> (1,2,-3,4)
   friend inline const Vector Blend(const Mask& m, const Vector& tValue, const Vector& fValue)
   { return Vector(Blend(m._a, tValue._a, fValue._a), Blend(m._b, tValue._b, fValue._b)); }

   /// Update
   friend inline Vector& Update(const Mask& m, Vector& lhs, const Vector& rhs)
   { Update(m._a, lhs._a, rhs._a); Update(m._b, lhs._b, rhs._b); return lhs; }

   inline WriteMaskedVector<Vector> operator [] (const Mask& m)
   { return WriteMaskedVector<Vector>(*this, m); }


   //////////////////////////
   // Reduction Operations //
   //////////////////////////

   inline T ReduceSum() const;
   inline T ReduceProduct() const;
   inline T ReduceMin() const;
   inline T ReduceMax() const;


   //////////////////////////
   // Geometric Operations //
   //////////////////////////

   /// These are only good for non-integral data

   inline T Average() const;         ///< Reduce sum / N
   inline T Length() const;          ///< Euclidean length of the vector
   inline T LengthSquared() const;   ///< Returns the length squared (avoids sqrt)
   inline Vector Unit() const;       ///< Returns normalized copy (length == 1)

   static T inline Dot(const Vector& v0, const Vector& v1);  ///< Dot product
   //static Vector<T, N> cross(const Vector& v0, const Vector& v1); ///< Cross product


   ////////////////////////////
   // Functional Programming //
   ////////////////////////////

   template <typename Functor> inline Vector Map(const Functor& f) const;


   ///////////////////////////////////////////////////////////////////////////
   //                      Private Data And Methods                         //
   ///////////////////////////////////////////////////////////////////////////


private:
   static const int WA = (1 << ILOG2<N>::value) == N ? N/2 : (1 << ILOG2<N>::value);
   static const int WB = N - WA;

   typedef Vector<T, WA> VectorA;
   typedef Vector<T, WB> VectorB;

   VectorA _a;
   VectorB _b;

   /// Private constructor
   inline Vector(const VectorA& a, const VectorB& b) : _a(a), _b(b) {}

   /// Private string I/O, necessary to make use of recursion
   inline std::string internalToString(int prec) const
   { return _a.internalToString(prec) + ", " + _b.internalToString(prec); }
   inline std::string internalToString() const
   { return _a.internalToString() + ", " + _b.internalToString(); }

   /// Befriend the mathematics structure that corresponds with each kind of
   /// vector, and we also extend friendship to all other specializations of
   /// Vector, to enable recursive algorithms which use private implmentation
   /// functions (See internalToString() as an example).
   friend struct MATHEMATICS<Vector<T, N> >;
   friend struct BITS<Vector<T, N> >;
   template <class F, int M> friend class Vector;
};



///////////////////////////////////////////////////////////////////////////
//                    Generic Vector Implementation                      //
///////////////////////////////////////////////////////////////////////////


/// Constructors
template <class T, int N> inline Vector<T, N>::Vector() { }
template <class T, int N> inline Vector<T, N>::Vector(const T& t) : _a(t), _b(t) {}
template <class T, int N> inline Vector<T, N>::Vector(const T t[N]) : _a(t), _b(t+WA) {}

/// Base Type conversion constructor
template <class T, int N>  template <class S>
inline Vector<T, N>::Vector(const Vector<S, N>& v) : _a(v._a), _b(v._b) {}


/// Constructors for common partial specializations
template <class T, int N> inline
Vector<T, N>::Vector(const T& t0, const T& t1)
 : _a(t0), _b(t1) { /*STATIC_assert(N == 2);*/ }
template <class T, int N> inline
Vector<T, N>::Vector(const T& t0, const T& t1, const T& t2)
 : _a(t0, t1), _b(t2) { /*STATIC_assert(N == 3);*/ }
template <class T, int N> inline
Vector<T, N>::Vector(const T& t0, const T& t1, const T& t2, const T& t3)
 : _a(t0, t1), _b(t2, t3) { /*STATIC_assert(N == 4);*/ }
template <class T, int N> inline
Vector<T, N>::Vector(
   const T& t0,  const T& t1,  const T& t2,  const T& t3,
   const T& t4,  const T& t5,  const T& t6,  const T& t7)
 : _a(t0, t1, t2, t3), _b(t4, t5, t6, t7) { /*STATIC_assert(N == 8);*/ }
template <class T, int N> inline
Vector<T, N>::Vector(
   const T& t0,  const T& t1,  const T& t2,  const T& t3,
   const T& t4,  const T& t5,  const T& t6,  const T& t7,
   const T& t8,  const T& t9,  const T& t10, const T& t11,
   const T& t12, const T& t13, const T& t14, const T& t15)
 : _a(t0, t1, t2, t3, t4, t5, t6, t7), _b(t8, t9, t10, t11, t12, t13, t14, t15)
{ /*STATIC_assert(N == 16);*/ }


// Expansions
template <class T, int N> inline Vector<T, N>
Vector<T, N>::Expansion2x(
   const Vector<T, N/2>& t0, const Vector<T, N/2>& t1)
{ return Vector<T, N>(t0, t1); }
template <class T, int N> inline Vector<T, N>
Vector<T, N>::Expansion4x(
   const Vector<T, N/4>& t0, const Vector<T, N/4>& t1,
   const Vector<T, N/4>& t2, const Vector<T, N/4>& t3)
{ return Expansion2x(Vector<T, N/2>::Expansion2x(t0, t1), Vector<T, N/2>::Expansion2x(t2, t3)); }




/// Basic Accessors
template <class T, int N> inline const T&
Vector<T, N>::operator [] (int i) const
{ assert(i >= 0 && i < N); return (i < WA) ? (_a[i]) : (_b[i-WA]); }
template <class T, int N> inline T& Vector<T, N>::operator [] (int i)
{ assert(i >= 0 && i < N); return (i < WA) ? (_a[i]) : (_b[i-WA]); }

/// Conversion to std::string
template <class T, int N>
inline std::string Vector<T, N>::ToString(int prec) const
{ return std::string("[") + internalToString(prec) + "]"; }
template <class T, int N>
inline std::string Vector<T, N>::ToString() const
{ return std::string("[") + internalToString() + "]"; }

/// Insertion and extraction operators for C++ I/O Streams
/*template <class T, int N> inline
std::ostream& operator << (std::ostream& stream, const Vector<T, N>& v)
{ stream << tostd::string(); }*/

template <class T, int N> inline
std::istream& Vector<T, N>::internalStreamIn(std::istream& stream)
{
   char c;
   _a.internalStreamIn(stream);
   stream >> c;   // ','
   _b.internalStreamIn(stream);
   return stream;
}


//////////////////////
// Loads and Stores //
//////////////////////

/// We have a problem here.  When (T != int32_t, float) and (N == 4, 16), then
/// we get the SIMD specialization of Vector<int32_t, N> but the generalization
/// of Vector<T, N>.  Vector<int32_t, N> specilizations do not support the recursive
/// definition employed in the generalization, and fails to compile.
///
/// The only option, as I see it, is to define these iteratively using [],
/// rather than recursively.

template <class T, int N> template <class U> inline Vector<T, N> Vector<T, N>::Load(U * addr)
{
   Vector<T, N> v;
   v._a = Vector<T, N>::VectorA::Load(addr);
   v._b = Vector<T, N>::VectorB::Load(addr + Vector<T, N>::WA);
   return v;
}

template <class T, int N> template <class U> inline Vector<T, N> Vector<T, N>::LoadBroadcast(U * addr)
{
   Vector<T, N> v;
   v._a = Vector<T, N>::VectorA::LoadBroadcast(addr);
   v._b = Vector<T, N>::VectorB::LoadBroadcast(addr);
   return v;
}

template <class T, int N> template <class U> inline Vector<T, N>
Vector<T, N>::Gather(U * addr, const Vector<int32_t, N>& offsets)
{
   Vector<T, N> v;
   for (int i = 0; i < N; ++i) v[i] = (T)(*(addr + offsets[i]));
   return v;
}

template <class T, int N> template <class U> inline Vector<T, N>
Vector<T, N>::Gather(U * addr, const Vector<int32_t, N>& offsets, const Mask& mask)
{
   Vector<T, N> v;
   for (int i = 0; i < N; ++i)
      if (mask[i]) v[i] = (T)(*(addr + offsets[i]));
   return v;
}

template <class T, int N> template <class U> inline void Vector<T, N>::Store(U * addr) const
{ _a.Store(addr); _b.Store(addr + WA); }

template <class T, int N> template <class U> inline void Vector<T, N>::StoreOne(U * addr) const
{ _a.StoreOne(addr); }

template <class T, int N> template <class U> void inline
Vector<T, N>::Scatter(U * addr, const Vector<int32_t, N>& offsets) const
{
   for (int i = 0; i < N; ++i)
      *(addr + offsets[i]) = (U)(operator[](i));
}

template <class T, int N> template <class U> void inline
Vector<T, N>::Scatter(U * addr, const Vector<int32_t, N>& offsets, const Mask& mask) const
{
   for (int i = 0; i < N; ++i)
      if (mask[i])
         *(addr + offsets[i]) = (U)(operator[](i));
}



/////////////////////////////////////
// Reduction Operation Definitions //
/////////////////////////////////////

template <class T, int N> inline T Vector<T, N>::ReduceSum() const
{ return _a.ReduceSum() + _b.ReduceSum(); }

template <class T, int N> inline T Vector<T, N>::ReduceProduct() const
{ return _a.ReduceProduct() * _b.ReduceProduct(); }

template <class T, int N> inline T Vector<T, N>::ReduceMin() const
{ T m1 = _a.ReduceMin(), m2 = _b.ReduceMin(); return MIN(m1, m2); }

template <class T, int N> inline T Vector<T, N>::ReduceMax() const
{ T m1 = _a.ReduceMax(), m2 = _b.ReduceMax(); return MAX(m1, m2); }


//////////////////////////
// Geometric Operations //
//////////////////////////

/// These are only good for non-integral data

template <class T, int N> inline T Vector<T, N>::Average() const
{
   STATIC_assert(!TypeInfo<T>::Integral);
   return ReduceSum() * RCP((float) N);
}

template <class T, int N> inline T Vector<T, N>::Length() const
{ /*STATIC_assert(!TypeInfo<T>::Integral);*/ return SQRT(LengthSquared()); }

template <class T, int N> inline T Vector<T, N>::LengthSquared() const
{ /*STATIC_assert(!TypeInfo<T>::Integral);*/ return Dot(*this, *this); }

template <class T, int N> inline Vector<T, N> Vector<T, N>::Unit() const
{
   /*STATIC_assert(!TypeInfo<T>::Integral);*/
   return (*this) * RCP_SQRT(LengthSquared());
}

template <class T, int N> inline T
Vector<T, N>::Dot(const Vector<T, N>& v1, const Vector<T, N>& v2)
{ /*STATIC_assert(!TypeInfo<T>::Integral);*/ return (v2 * v1).ReduceSum(); }

/*template <class T, int N> inline Vector<T, N>
Vector<T, N>::cross(const Vector<T, N>& v1, const Vector<T, N>& v2)
{
   /// IMPLEMENT ME when needed, don't feel like writing/testing it now
   //assert(false);
   STATIC_assert(!TypeInfo<T>::Integral);
   STATIC_assert(N == 3);

   T x1 =  (v1.y()*v2.z() - v1.z()*v2.y());
   T y1 = -(v1.x()*v2.z() - v1.z()*v2.x());
   T z1 =  (v1.x()*v2.y() - v1.y()*v2.x());
   return Vector<T, N>(x1, y1, z1);
}*/

////////////////////////////////////////
// Functional Programming Definitions //
////////////////////////////////////////

template <class T, int N>
template <typename Functor> inline
Vector<T, N> Vector<T, N>::Map(const Functor& f) const
{ return Vector<T, N>(_a.Map(f), _b.Map(f)); }


/////////////////////////////////
// Special Vector3f Operations //
/////////////////////////////////

/// Special, for 3-vectors alone! (4-vectors implement as well, ignoring
/// the fourth component)
template <class T> inline
Vector<T, 3> Cross(Vector<T, 3> a, Vector<T, 3> b)
{
   return
      Vector<T, 3>( a.y()*b.z() - b.y()*a.z(),
                   -a.x()*b.z() + b.x()*a.z(),
                    a.x()*b.y() - b.x()*a.y());
}

/// Conversion between coordinate systems.
/// The Spherical Coordinate system is defined thusly, with y
/// being the vertical axis:
///
/// phi is our azimuthal angle (longitudinal)
/// theta is our altitude angle (latitudinal)
/// Assumes everything is in radians
///   theta in [0, PI]
///   phi in [0, 2*PI]
///
///   x = R sin(theta) cos(phi)
///   z = R sin(theta) sin(phi)
///   y = R sin(theta)
///
///   R     = _position.length() // take the positive root
///   phi = atan2f(z/x)
///   theta = acos(y / R) - PI/2
///
template <class T> inline
Vector<T, 3> ToSphericalCoordinates(const Vector<T, 3>& s)
{
   STATIC_assert(!TypeInfo<T>::Integral);
   float R = s.length();
   return Vector<T, 3>(
      R,
      PI<T>() - ATAN2(s.z(), s.x()),
      PI<T>() - ACOS(s.y() / R));
}

template <class T> inline
Vector<T, 3> ToRectangularCoordinates(const Vector<T, 3>& s)
{
   STATIC_assert(!TypeInfo<T>::Integral);
   // Assuming x = R, y = phi, z = theta
   return Vector<T, 3>(
     s.x() * SIN(PI<T>()-s.z()) * COS(-s.y()-PI<T>()),
     s.x() * COS(PI<T>()-s.z()),
     s.x() * SIN(PI<T>()-s.z()) * SIN(-s.y()-PI<T>()));
}

// Color space conversions
template <class T> inline
Vector<T, 3> ToHSV(const Vector<T, 3>& rgb)
{
   STATIC_assert(!TypeInfo<T>::Integral);
   // Compute the hue
   float max = MAX(rgb.x(), MAX(rgb.y(), rgb.z()));
   float min = MIN(rgb.x(), MIN(rgb.y(), rgb.z()));

   float hue;
   if (max == min) hue = 0;
   else if (max == rgb.x() && rgb.y() >= rgb.z()) hue = 60 * (rgb.y() - rgb.z()) / (max - min);
   else if (max == rgb.x() && rgb.y() <  rgb.z()) hue = 60 * (rgb.y() - rgb.z()) / (max - min) + 360;
   else if (max == rgb.y())                       hue = 60 * (rgb.z() - rgb.x()) / (max - min) + 120;
   else if (max == rgb.z())                       hue = 60 * (rgb.x() - rgb.y()) / (max - min) + 240;

   float saturation = (max == 0.0f ? 0.0f : 1.0f - min/max);
   float value = max;
   hue /= 360;

   return Vector<T, 3>(hue, saturation, value);
}

template <class T> inline
Vector<T, 3> ToRGB(const Vector<T, 3>& hsv)
{
   STATIC_assert(!TypeInfo<T>::Integral);
   // pull h into range [0, 360)
   float xHat = hsv.x();
   if (hsv.x() < 0.0f) xHat = 0.0f;
   if (hsv.x() > 1.0f) xHat = 1.0f;
   xHat *= 360.0f;

   float r, g, b;

   if (xHat < 120.0f)
   {
      r = (120.0f - xHat) / 60.0f;
      g = xHat / 60.0f;
      b = 0.0f;
   }
   else if (xHat < 240.0f)
   {
      r = 0.0f;
      g = (240.0f - xHat) / 60.0f;
      b = (xHat - 120.0f) / 60.0f;
   }
   else
   {
      r = (xHat - 240.0f) / 60.0f;
      g = 0.0f;
      b = (360.0f - xHat) / 60.0f;
   }

   r = MIN(r, 1.0f);
   g = MIN(g, 1.0f);
   b = MIN(b, 1.0f);

   Vector<T, 3> c((1.0f - hsv.y() + hsv.y()*r) * hsv.z(),
                  (1.0f - hsv.y() + hsv.y()*g) * hsv.z(),
                  (1.0f - hsv.y() + hsv.y()*b) * hsv.z());
   return c;
}


//////////////////////
// Matrix Transpose //
//////////////////////

/// This is most useful when SOA data needs to be converted to AOS layout, for
/// output to disk, or printing, etc. at the end of computation
template <class T, int N, int M> inline Vector<Vector<T, N>, M> Transpose(
   const Vector<Vector<T, M>, N>& m)
{
   /// Probably very inefficient, but eh, it seems to work.
   Vector<Vector<T, N>, M> tm;
   for (int i = 0; i < M; ++i)
      for (int j = 0; j < N; ++j)
         tm[i][j] = m[j][i];

   return tm;
}


///////////////////////////////////////////////////////////////////////////
//                             Mathematics                               //
///////////////////////////////////////////////////////////////////////////

/// See Mathematics.h for complete interface specification
template <class T, int N> struct MATHEMATICS<Vector<T, N> >
{
private:
   typedef Vector<T, N> Vec;  ///< For shorthand, syntactical convenience

public:
   #define PREFIX static inline Vec
   PREFIX zero()                { return Vec(ZERO<T>()); }
   PREFIX one()                 { return Vec(ONE<T>()); }
   PREFIX minusOne()            { return Vec(MINUS_ONE<T>()); }
   PREFIX inf()                 { return Vec(INF<T>()); }
   PREFIX _inf()                { return Vec(_INF<T>()); }
   PREFIX maxv()                { return Vec(MAXV<T>()); }
   PREFIX e()                   { return Vec(E<T>()); }
   PREFIX pi()                  { return Vec(PI<T>()); }
   PREFIX rcpPi()               { return Vec(RCP_PI<T>()); }
   PREFIX phi()                 { return Vec(PHI<T>()); }
   PREFIX sqrt2()               { return Vec(SQRT2<T>()); }

   PREFIX cbrt(Vec x)           { return Vec(CBRT(x._a),  CBRT(x._b)); }
   PREFIX sqrt(Vec x)           { return Vec(SQRT(x._a),  SQRT(x._b)); }
   PREFIX rcpSqrt(Vec x)        { return Vec(RCP_SQRT(x._a), RCP_SQRT(x._b)); }
   PREFIX rcp(Vec x)            { return Vec(RCP(x._a),   RCP(x._b)); }
   PREFIX cos(Vec x)            { return Vec(COS(x._a),   COS(x._b)); }
   PREFIX sin(Vec x)            { return Vec(SIN(x._a),   SIN(x._b)); }
   PREFIX tan(Vec x)            { return Vec(TAN(x._a),   TAN(x._b)); }
   PREFIX pow(Vec x, Vec y)     { return Vec(POW(x._a, y._a),   POW(x._b, y._b)); }
   PREFIX exp(Vec x)            { return Vec(EXP(x._a),   EXP(x._b)); }
   PREFIX log(Vec x)            { return Vec(LOG(x._a),   LOG(x._b)); }
   PREFIX log2(Vec x)           { return Vec(LOG2(x._a),  LOG2(x._b)); }
   PREFIX log10(Vec x)          { return Vec(LOG10(x._a), LOG10(x._b)); }
   PREFIX abs(Vec x)            { return Vec(ABS(x._a),   ABS(x._b)); }
   PREFIX acos(Vec x)           { return Vec(ACOS(x._a),  ACOS(x._b)); }
   PREFIX asin(Vec x)           { return Vec(ASIN(x._a),  ASIN(x._b)); }
   PREFIX atan2(Vec x, Vec y)   { return Vec(ATAN2(x._a, y._a), ATAN2(x._b,y._b)); }
   PREFIX floor(Vec x)          { return Vec(FLOOR(x._a),   FLOOR(x._b)); }
   PREFIX ceil(Vec x)           { return Vec(CEILING(x._a), CEILING(x._b)); }
   PREFIX clampz(Vec x, Vec u)  { return Vec(CLAMPZ(x._a, u._a), CLAMPZ(x._b, u._b)); }
   PREFIX sign(Vec x)           { return Vec(SIGN(x._a), SIGN(x._b)); }

   PREFIX min(Vec x, Vec y)
   { return Vec(Blend(x._a < y._a, x._a, y._a), Blend(x._b < y._b, x._b, y._b)); }
   PREFIX max(Vec x, Vec y)
   { return Vec(Blend(x._a > y._a, x._a, y._a), Blend(x._b > y._b, x._b, y._b)); }

   PREFIX min3(Vec x, Vec y, Vec z)  { return min(x, min(y, z)); }
   PREFIX max3(Vec x, Vec y, Vec z)  { return max(x, max(y, z)); }
   #undef PREFIX
};


///////////////////////////////////////////////////////////////////////////
//                           Bit Manipulation                            //
///////////////////////////////////////////////////////////////////////////

template <class T, int N> struct BITS<Vector<T, N> >
{
private:
   typedef Vector<T, N> Vec;  ///< For shorthand, syntactical convenience

public:

   /// Bit rotations
   static inline Vec rotr(Vec a, int b) { return Vec(ROTR(a._a, b), ROTR(a._b, b)); }
   static inline Vec rotl(Vec a, int b) { return Vec(ROTL(a._a, b), ROTL(a._b, b)); }

   /// Bit interleaves
   static inline Vec interleave11(Vec a, Vec b)
   { return Vec(INTERLEAVE11(a._a, b._a), INTERLEAVE11(a._b, b._b)); }
   static inline Vec interleave21(Vec a, Vec b)
   { return Vec(INTERLEAVE21(a._a, b._a), INTERLEAVE21(a._b, b._b)); }

   /// Counts '1' bits in an integer
   //static inline Vec bitCount(T a);
};



///////////////////////////////////////////////////////////////////////////
//                  Base Case Vector<1> Implementation                   //
///////////////////////////////////////////////////////////////////////////

/// Base case implementation, for N == 1
template <typename T> class Vector<T, 1>
{
private:
   T _t;

   /// Private string I/O, necessary to make use of recursion
   inline std::string internalToString()         const { return std::to_string(_t); }
   //inline std::string internalToString(int prec) const { return std::to_string(_t); }

public:

   /// Define our native mask type
   typedef typename TypeInfo<Vector<T, 1> >::Mask Mask;

   /// Make some friends
   friend struct MATHEMATICS<Vector<T, 1> >;
   friend struct BITS<Vector<T, 1> >;
   template <class U, int M> friend class Vector;

   /// Constructors
   inline Vector()                                  {}
   inline explicit Vector(const T& t)    : _t(t)    {}
   inline explicit Vector(const T t[1])  : _t(t[0]) {}

   template <class S> inline explicit Vector(const Vector<S, 1>& v) : _t(T(v._t)) {}

   ///////////////
   // Accessors //
   ///////////////

   /// Dynamic element accessors
   inline const T& operator [] (int i) const { assert(i == 0); return _t; }
   inline T& operator [] (int i)             { assert(i == 0); return _t; }

   /// Static element accessors, no references
   template <int I> inline const T& Get() const    { return _t; }
   template <int I> inline T& Get()                { return _t; }
   template <int I> inline void Set(const T& v)    { _t = v; }


   /// std::string I/O representation
   //inline std::string ToString(int prec) const { return std::string("[") + std::to_string(_t) + "]"; }
   inline std::string ToString() const { return std::string("[") + std::to_string(_t) + "]"; }

   /// Insertion and extraction operators for C++ I/O Streams
   friend inline std::ostream& operator << (std::ostream& stream, const Vector& v)
   { stream << v.ToString(); return stream; }
   friend inline std::istream& operator >> (std::istream& stream, Vector& v)
   {
      char c;
      stream >> c >> v._t >> c;
      return stream;
   }

   inline std::istream& internalStreamIn(std::istream& stream)
   { stream >> _t; return stream; }


   /// Base type conversion operator(s)
   inline operator const T&() const { return _t; }
   inline operator T&() { return _t; }


   //////////////////////
   // Loads and Stores //
   //////////////////////

   /// All loads and stores are assumed to be aligned, address alignment will be
   /// asserted. Gather and Scatter operations are supported in base+offset
   /// format.

   template <class U> inline static Vector Load(U * addr)          { return Vector((T)(*addr)); }
   template <class U> inline static Vector LoadBroadcast(U * addr) { return Vector((T)(*addr)); }
   template <class U> inline static Vector Gather(U * addr, const Vector<int32_t, 1>& offsets)
   { return Vector((T)(*(addr + offsets._t))); }

   template <class U> inline void Store(U * addr)    const { (*addr) = (U)_t; }
   template <class U> inline void StoreOne(U * addr) const { (*addr) = (U)_t; }
   template <class U> inline void Scatter(U * addr,  const Vector<int32_t, 1>& offsets) const
   { *(addr + offsets._t) = (U)_t; }



   ////////////////////////////
   // Mathematical Operators //
   ////////////////////////////

   /// Unary reduceMinus
   inline Vector operator - () const { return Vector(-_t); }

   /// Binary arithmetical operators
   #define PREFIX friend inline Vector
   PREFIX operator + (const Vector& a, const Vector& b) { return Vector(a._t + b._t); }
   PREFIX operator - (const Vector& a, const Vector& b) { return Vector(a._t - b._t); }
   PREFIX operator * (const Vector& a, const Vector& b) { return Vector(a._t * b._t); }
   PREFIX operator / (const Vector& a, const Vector& b) { return Vector(a._t / b._t); }
   #undef PREFIX


   /// Accumulation operators
   inline Vector& operator += (const Vector& b) { _t += b._t; return *this; }
   inline Vector& operator -= (const Vector& b) { _t -= b._t; return *this; }
   inline Vector& operator *= (const Vector& b) { _t *= b._t; return *this; }
   inline Vector& operator /= (const Vector& b) { _t /= b._t; return *this; }


   /// Note: Mathematical functions (sin, pow, etc.) and common bit-manipulation
   /// functions (rotl, interleave, bitscan, etc.) are implemented as global
   /// functions via math/Mathematics.h and math/Bits.h


   /////////////////////////////
   // Integer-Only Arithmetic //
   /////////////////////////////

   //inline friend Vector operator >> (const Vector& a, uint32_t bits)         { /*STATIC_assert(TypeInfo<T>::Integral);*/ return a._t >> bits; }
   //inline friend Vector operator << (const Vector& a, uint32_t bits)         { /*STATIC_assert(TypeInfo<T>::Integral);*/ return a._t << bits; }
   inline friend Vector operator << (const Vector& a, const Vector& bits)  { /*STATIC_assert(TypeInfo<T>::Integral); return Vector(a._t << bits._t);*/ }
   inline friend Vector operator >> (const Vector& a, const Vector& bits)  { /*STATIC_assert(TypeInfo<T>::Integral); return Vector(a._t >> bits._t);*/ }

   /// Integer Modulus
   inline friend Vector operator %  (const Vector& a, const Vector& b)     { /*STATIC_assert(TypeInfo<T>::Integral); return Vector(a._t % b._t);*/ }
   //inline friend Vector operator %  (const Vector& a, const T& b)          { /*STATIC_assert(TypeInfo<T>::Integral);*/ return a._t % b; }

   //inline Vector& operator <<= (const int bits)                            { return *this = *this << bits; }
   //inline Vector& operator >>= (const int bits)                            { return *this = *this >> bits; }
   inline Vector& operator <<= (const Vector& bits)                        { return *this = *this << bits; }
   inline Vector& operator >>= (const Vector& bits)                        { return *this = *this >> bits; }


   ///////////////////////
   // Bitwise Operators //
   ///////////////////////

   inline Vector operator ~ () const                                      { /*STATIC_assert(TypeInfo<T>::Integral); return Vector(~_t);*/ }
   inline friend Vector operator & (const Vector& a, const Vector& bits)  { /*STATIC_assert(TypeInfo<T>::Integral); return Vector(a._t & bits._t);*/ }
   inline friend Vector operator | (const Vector& a, const Vector& bits)  { /*STATIC_assert(TypeInfo<T>::Integral); return Vector(a._t | bits._t);*/ }
   inline friend Vector operator ^ (const Vector& a, const Vector& bits)  { /*STATIC_assert(TypeInfo<T>::Integral); return Vector(a._t ^ bits._t);*/ }

   inline Vector& operator &= (const Vector& b)                           { return *this = *this & b; }
   inline Vector& operator |= (const Vector& b)                           { return *this = *this | b; }
   inline Vector& operator ^= (const Vector& b)                           { return *this = *this ^ b; }


   ////////////////
   // Comparison //
   ////////////////

   friend inline const Mask operator == (const Vector& a, const Vector& b)
   { return a._t == b._t; }
   friend inline const Mask operator != (const Vector& a, const Vector& b)
   { return a._t != b._t; }
   friend inline const Mask operator <  (const Vector& a, const Vector& b)
   { return a._t <  b._t; }
   friend inline const Mask operator >  (const Vector& a, const Vector& b)
   { return a._t >  b._t; }
   friend inline const Mask operator <= (const Vector& a, const Vector& b)
   { return a._t <= b._t; }
   friend inline const Mask operator >= (const Vector& a, const Vector& b)
   { return a._t >= b._t; }


   //////////////////////////
   // Blending and Masking //
   //////////////////////////

   /// Blend (calls blend(T::Mask, T, T)
   friend inline const Vector Blend(const Mask& m, const Vector& tValue, const Vector& fValue)
   { return Vector(Blend(m._t, tValue._t, fValue._t)); }

   /// Update
   friend inline Vector& Update(const Mask& m, Vector& lhs, const Vector& rhs)
   { update(m._t, lhs._t, rhs._t); return lhs; }

   inline WriteMaskedVector<Vector> operator [] (const Mask& m)
   { return WriteMaskedVector<Vector>(*this, m); }


   //////////////////////////
   // Reduction Operations //
   //////////////////////////

   inline T ReduceSum() const     { return _t; }
   inline T ReduceProduct() const { return _t; }
   inline T ReduceMin() const     { return _t; }
   inline T ReduceMax() const     { return _t; }


   //////////////////////////
   // Geometric Operations //
   //////////////////////////

   /// These are only good for non-integral data
   /// No dot product or cross product for a 1-vector

   inline T Average() const { return _t; }                   ///< Reduce sum / N
   inline T Length() const  { return _t; }                   ///< Euclidean length of the vector
   inline T LengthSquared() const { return _t*_t; }          ///< Returns the length squared (avoids sqrt)
   inline Vector Unit() const { return Vector<T, 1>(ONE<T>()); }  ///< Returns normalized copy (length == 1)



   ////////////////////////////
   // Functional Programming //
   ////////////////////////////

   template <typename Functor>
   inline Vector Map(const Functor& f) const { return f(_t); }
};


/// Mathematics. See Mathematics.h for complete interface specification
template <typename T> struct MATHEMATICS<Vector<T, 1> >
{
private:
   typedef Vector<T, 1> Vec1;

public:

   static inline Vec1 zero()                 { return Vec1(ZERO<T>()); }
   static inline Vec1 one()                  { return Vec1(ONE<T>()); }
   static inline Vec1 minusOne()             { return Vec1(MINUS_ONE<T>()); }
   static inline Vec1 inf()                  { return Vec1(INF<T>()); }
   static inline Vec1 _inf()                 { return Vec1(_INF<T>()); }
   static inline Vec1 e()                    { return Vec1(E<T>()); }
   static inline Vec1 pi()                   { return Vec1(PI<T>()); }
   static inline Vec1 rcpPi()                { return Vec1(RCP_PI<T>()); }
   static inline Vec1 phi()                  { return Vec1(PHI<T>()); }
   static inline Vec1 sqrt2()                { return Vec1(SQRT2<T>()); }

   static inline Vec1 cbrt(Vec1 x)           { return Vec1(CBRT(x._t)); }
   static inline Vec1 sqrt(Vec1 x)           { return Vec1(SQRT(x._t)); }
   static inline Vec1 rcpSqrt(Vec1 x)        { return Vec1(RCP_SQRT(x._t)); }
   static inline Vec1 rcp(Vec1 x)            { return Vec1(RCP(x._t)); }
   static inline Vec1 cos(Vec1 x)            { return Vec1(COS(x._t)); }
   static inline Vec1 sin(Vec1 x)            { return Vec1(SIN(x._t)); }
   static inline Vec1 tan(Vec1 x)            { return Vec1(TAN(x._t)); }
   static inline Vec1 pow(Vec1 x, Vec1 y)    { return Vec1(POW(x._t, y._t)); }
   static inline Vec1 exp(Vec1 x)            { return Vec1(EXP(x._t)); }
   static inline Vec1 log(Vec1 x)            { return Vec1(LOG(x._t)); }
   static inline Vec1 log2(Vec1 x)           { return Vec1(LOG2(x._t)); }
   static inline Vec1 log10(Vec1 x)          { return Vec1(LOG10(x._t)); }
   static inline Vec1 abs(Vec1 x)            { return Vec1(ABS(x._t)); }
   static inline Vec1 acos(Vec1 x)           { return Vec1(ACOS(x._t)); }
   static inline Vec1 asin(Vec1 x)           { return Vec1(ASIN(x._t)); }
   static inline Vec1 atan2(Vec1 x, Vec1 y)  { return Vec1(ATAN2(x._t, y._t)); }
   static inline Vec1 floor(Vec1 x)          { return Vec1(FLOOR(x._t)); }
   static inline Vec1 ceil(Vec1 x)           { return Vec1(CEILING(x._t)); }
   static inline Vec1 clampz(Vec1 x, Vec1 u) { return Vec1(CLAMPZ(x._t, u._t)); }
   static inline Vec1 sign(Vec1 x)           { return Vec1(SIGN(x._t)); }

   static inline Vec1 min(Vec1 x, Vec1 y)
   { return (x._t < y._t ? Vec1(x._t) : Vec1(y._t)); }
   static inline Vec1 max(Vec1 x, Vec1 y)
   { return (x._t > y._t ? Vec1(x._t) : Vec1(y._t)); }

   static inline Vec1 min3(Vec1 x, Vec1 y, Vec1 z)  { return min(x, min(y, z)); }
   static inline Vec1 max3(Vec1 x, Vec1 y, Vec1 z)  { return max(x, max(y, z)); }
};


///////////////////////////////////////////////////////////////////////////
//                           Bit Manipulation                            //
///////////////////////////////////////////////////////////////////////////

template <typename T> struct BITS<Vector<T, 1> >
{
private:
   typedef Vector<T, 1> Vec;  ///< For shorthand, syntactical convenience

public:

   /// Bit rotations
   static inline Vec rotr(Vec a, int b) { return Vec(ROTR(a._t, b)); }
   static inline Vec rotl(Vec a, int b) { return Vec(ROTL(a._t, b)); }

   /// Bit interleaves
   static inline Vec interleave11(Vec a, Vec b)
   { return Vec(INTERLEAVE11(a._t, b._t)); }
   static inline Vec interleave21(Vec a, Vec b)
   { return Vec(INTERLEAVE21(a._t, b._t)); }

   /// Counts '1' bits in an integer
   //static inline Vec bitCount(Vec a);
};

}; // namespace Mathematics

#endif // MATH_VECTOR_H


