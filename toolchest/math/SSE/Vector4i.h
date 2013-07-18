#ifndef VECTOR4I_H
#define VECTOR4I_H

#include "../Vector.h"

#include "Mask4.h"


namespace ToolChest
{

///////////////////////////////////////////////////////////////////////////
//                               TypeInfo                                //
///////////////////////////////////////////////////////////////////////////


template <> class TypeInfo<Vector<int32, 4> >
{
public:
   static const bool Integral = TypeInfo<int32>::Integral;
   static const bool Unsigned = TypeInfo<int32>::Unsigned;
   static const int  Rank = 1;
   static const int  Width = 4;

   typedef int32 ElementType;
   typedef Mask<bool, 4> Mask;
};

template <> class Vector<int32, 4>
{
   ///////////////////////////////////////////////////////////////////////////
   //                           Public Interface                            //
   ///////////////////////////////////////////////////////////////////////////

public:

   /// Define our mask type
   typedef TypeInfo<Vector>::Mask Mask;

   /// Constructors
   inline Vector() {}
   inline explicit Vector(const int32& t) : _r(t) {}
   inline explicit Vector(const int32 t[4]) : _r(t[0], t[1], t[2], t[3]) { }
   inline Vector(int32 i0, int32 i1, int32 i2, int32 i3) : _r(i0, i1, i2, i3) {}

   /// Construction from Vector3f + float
   inline Vector(const Vector<int32, 3>& v, int32 w) : _r(v.x(), v.y(), v.z(), w) {}

   /// Conversion from arbitrary (but cast-able) base type
   template <class S> explicit Vector(const Vector<S, 4>& v) :
      _r((int32)v.x(), (int32)v.y(), (int32)v.z(), (int32)v.w()) {}
   template <> explicit Vector(const Vector<float, 4>& v);  /// Specialization

   /// Subscript accessors (introduces aliasing)
   inline const int32& operator [] (int i) const
   { assert(i < 4); return ((int32*)&_r)[i]; }
   inline int32& operator [] (int i) { assert(i < 4); return ((int32*)&_r)[i]; }

   /// Convenient static, non-aliased element accessors for Vector2, 3, 4
   inline const int32 x() const { return _r.i32Extract<0>(); }
   inline const int32 y() const { return _r.i32Extract<1>(); }
   inline const int32 z() const { return _r.i32Extract<2>(); }
   inline const int32 w() const { return _r.i32Extract<3>(); }

   inline const Vector<int32, 2> xy() const { return cast<Vector<int32, 2>>(*this); } //{ return Vector2i(x(), y()); }
   inline const Vector<int32, 3> xyz() const { return cast<Vector<int32, 3>>(*this); } //{ return Vector3i(x(), y(), z()); }

   template <int I> inline float get() const { return _r.i32Extract<I>(); }
   template <int I> inline void set(const int32& v) { _r = _r.i32Insert<I>(v); }

   /// Conversion to String (introduces aliasing)
   /// Conversion to String
   inline String toString(int prec = 3) const
   { return String("[") + internalToString(prec) + "]"; }


   /// Type cast operators, defined in SSE.h
   operator Vector<float, 4>() const;


   //////////////////////
   // Loads and Stores //
   //////////////////////

   /// All loads and stores are assumed to be aligned, address alignment will be
   /// asserted. Gather and Scatter operations are supported in base+offset
   /// format.

   template <class U> static Vector load(U * addr);
   template <class U> static Vector loadu(U * addr);
   template <class U> static Vector loadBroadcast(U * addr);
   template <class U> static Vector gather(U * addr, const Vector<int32, 4>& offsets);
   template <class U> static Vector gather(U * addr, const Vector<int32, 4>& offsets, const Mask& m);

   template <class U> void store(U * addr) const;
   template <class U> void storeOne(U * addr) const;
   template <class U> void scatter(U * addr, const Vector<int32, 4>& offsets) const;
   template <class U> void scatter(U * addr, const Vector<int32, 4>& offsets, const Mask& m) const;

   ////////////////////////////
   // Mathematical Operators //
   ////////////////////////////

   /// Unary minus
   inline Vector operator - () const                                       { return _r ^ SSERegister(0x80000000); }

   /// Binary arithmetical operators
   inline friend Vector operator + (const Vector& a, const Vector& b)      { return SSERegister::i32Add(a._r, b._r); }
   inline friend Vector operator - (const Vector& a, const Vector& b)      { return SSERegister::i32Sub(a._r, b._r); }
   inline friend Vector operator * (const Vector& a, const Vector& b)      { return SSERegister::i32Mul(a._r, b._r); }
   inline friend Vector operator / (const Vector& a, const Vector& b)      { return SSERegister::i32Div(a._r, b._r); }
   inline friend Vector operator + (const Vector& a, const int32& b)       { return a + Vector(b); }
   inline friend Vector operator - (const Vector& a, const int32& b)       { return a - Vector(b); }
   inline friend Vector operator * (const Vector& a, const int32& b)       { return a * Vector(b); }
   inline friend Vector operator / (const Vector& a, const int32& b)       { return a / Vector(b); }
   inline friend Vector operator + (const int32& a, const Vector& b)       { return b + a; }
   inline friend Vector operator - (const int32& a, const Vector& b)       { return Vector(a) - b; }
   inline friend Vector operator * (const int32& a, const Vector& b)       { return b * a; }
   inline friend Vector operator / (const int32& a, const Vector& b)       { return Vector(a) / b; }

   /// Accumulation operators
   inline Vector& operator += (const Vector& b)                            { return *this = *this + b; }
   inline Vector& operator -= (const Vector& b)                            { return *this = *this - b; }
   inline Vector& operator *= (const Vector& b)                            { return *this = *this * b; }
   inline Vector& operator /= (const Vector& b)                            { return *this = *this / b; }
   inline Vector& operator += (const int32& b)                             { return *this += Vector(b); }
   inline Vector& operator -= (const int32& b)                             { return *this -= Vector(b); }
   inline Vector& operator *= (const int32& b)                             { return *this *= Vector(b); }
   inline Vector& operator /= (const int32& b)                             { return *this /= Vector(b); }


   ////////////////////////
   // Bitshift Operators //
   ////////////////////////

   inline Vector operator << (const int bits) const                        { return _r.shiftLeft32((uint8)bits); }
   inline Vector operator >> (const int bits) const                        { return _r.shiftRightA32((uint8)bits); }
   inline Vector logicalRightShift(const int bits) const                   { return _r.shiftRightL32((uint8)bits); }
   inline Vector operator << (const Vector& bits) const;
   inline Vector operator >> (const Vector& bits) const;
   inline Vector logicalRightShift(const Vector& bits) const;

   /// Integer Modulus
   friend Vector operator %  (const Vector& a, const Vector& b);
   friend Vector operator %  (const Vector& a, const int32& b);
   //friend Vector operator %  (const int32& a, const Vector& b);

   inline Vector& operator <<= (const int bits)                            { return *this = *this << bits; }
   inline Vector& operator >>= (const int bits)                            { return *this = *this >> bits; }
   inline Vector& operator <<= (const Vector& bits)                        { return *this = *this << bits; }
   inline Vector& operator >>= (const Vector& bits)                        { return *this = *this >> bits; }


   ////////////////
   // Comparison //
   ////////////////

   inline friend Mask operator == (const Vector& a, const Vector& b)       { return SSERegister::i32Equality(a._r, b._r); }
   inline friend Mask operator != (const Vector& a, const Vector& b)       { return SSERegister::i32Inequality(a._r, b._r); }
   inline friend Mask operator <  (const Vector& a, const Vector& b)       { return SSERegister::i32LessThan(a._r, b._r); }
   inline friend Mask operator >  (const Vector& a, const Vector& b)       { return SSERegister::i32GreaterThan(a._r, b._r); }
   inline friend Mask operator <= (const Vector& a, const Vector& b)       { return SSERegister::i32LessThanOrEqual(a._r, b._r); }
   inline friend Mask operator >= (const Vector& a, const Vector& b)       { return SSERegister::i32GreaterThanOrEqual(a._r, b._r); }


   ////////////////////////
   // Bitwise Operations //
   ////////////////////////

   inline Vector operator ~ () const                                       { return ~_r; }
   inline friend Vector operator & (const Vector& a, const Vector& b)      { return a._r & b._r; }
   inline friend Vector operator | (const Vector& a, const Vector& b)      { return a._r | b._r; }
   inline friend Vector operator ^ (const Vector& a, const Vector& b)      { return a._r ^ b._r; }

   inline Vector& operator &= (const Vector& a)                            { _r &= a._r; return *this; }
   inline Vector& operator |= (const Vector& a)                            { _r |= a._r; return *this; }
   inline Vector& operator ^= (const Vector& a)                            { _r ^= a._r; return *this; }


   ////////////////////////////////////
   // Blending, Masking, and Shuffle //
   ////////////////////////////////////

   /// Returns a vector blending the values from tValue where m is true with the
   /// values from fValue where m is false:
   ///     blend( (t,t,f,t), (1,2,3,4), (-1,-2,-3,-4) ) ==> (1,2,-3,4)
   friend inline const Vector blend(const Mask& m, const Vector& tValue, const Vector& fValue)
   { return Vector(m._m.blend(tValue._r, fValue._r)); }

   /// Update
   friend inline Vector& update(const Mask& m, Vector& lhs, const Vector& rhs)
   { lhs._r = m._m.blend(rhs._r, lhs._r); return lhs; }

   /// Masking operator
   inline WriteMaskedVector<Vector> operator [] (const Mask& m)
   { return WriteMaskedVector<Vector>(*this, m); }

   /// Shuffle
   template<uint8 i0, uint8 i1, uint8 i2, uint8 i3> inline Vector shuffle() const
   { return _r.shuffle32<i0, i1, i2, i3>(); }


   //////////////////////////
   // Reduction Operations //
   //////////////////////////

   inline int32 reduceSum() const     { return _r.i32ReduceAdd().i32Extract<0>(); }
   inline int32 reduceProduct() const { return _r.i32ReduceMul().i32Extract<0>(); }
   inline int32 reduceMin() const     { return _r.i32ReduceMin().i32Extract<0>(); }
   inline int32 reduceMax() const     { return _r.i32ReduceMax().i32Extract<0>(); }


   ////////////////////////////
   // Functional Programming //
   ////////////////////////////

   template <typename Functor> inline Vector map(const Functor& f) const { return f(*this); }


   ///////////////////////////////////////////////////////////////////////////
   //                      Private Data And Methods                         //
   ///////////////////////////////////////////////////////////////////////////

public:
   SSERegister _r;

   /// Private constructors
   inline Vector(const __m128i reg) : _r(reg) {}
   inline Vector(const SSERegister& reg) : _r(reg) {}

   friend struct MATHEMATICS<Vector<int32, 4> >;
   friend struct BITS<Vector<int32, 4> >;

   /// Returns the register inside the object
   inline SSERegister getRegister() const { return _r; }

   /// Make friends with all generic Vector types
   template <class U, int M> friend class Vector;

   inline String internalToString(int prec = 3) const
   {
      return
         String((int32)((int32*)&_r)[0], prec) + ", " +
         String((int32)((int32*)&_r)[1], prec) + ", " +
         String((int32)((int32*)&_r)[2], prec) + ", " +
         String((int32)((int32*)&_r)[3], prec);
   }
};


/////////////////////////////////
// Vector Shifting Definitions //
/////////////////////////////////

/// SSE does not provide vector bit shift instructions (only shift a vector by
/// a single integer). This totally blows, and there's probably a faster way to
/// implement this using other SSE intrinsics, but for now this should be
/// functional.
///
/// If and when a more efficient implementation using SSE intrinsics is provided
/// we'll move these into the SSERegister class.

inline Vector<int32, 4> Vector<int32, 4>::operator << (const Vector<int32, 4>& bits) const
{
   /// This is the slowest possible way to do this
   Vector<int32, 4> r;
   r._r = r._r.i32Insert<0>(x() << bits.x());
   r._r = r._r.i32Insert<1>(y() << bits.y());
   r._r = r._r.i32Insert<2>(z() << bits.z());
   r._r = r._r.i32Insert<3>(w() << bits.w());

   return r;
}

inline Vector<int32, 4> Vector<int32, 4>::operator >> (const Vector<int32, 4>& bits) const
{
   /// This is the slowest possible way to do this
   Vector<int32, 4> r;
   r._r = r._r.i32Insert<0>(x() >> bits.x());
   r._r = r._r.i32Insert<1>(y() >> bits.y());
   r._r = r._r.i32Insert<2>(z() >> bits.z());
   r._r = r._r.i32Insert<3>(w() >> bits.w());
   return r;
}

inline Vector<int32, 4> Vector<int32, 4>::logicalRightShift(const Vector<int32, 4>& bits) const
{
   /// This is the slowest possible way to do this
   Vector<int32, 4> r;
   r._r = r._r.i32Insert<0>((uint32)x() >> bits.x());
   r._r = r._r.i32Insert<1>((uint32)y() >> bits.y());
   r._r = r._r.i32Insert<2>((uint32)z() >> bits.z());
   r._r = r._r.i32Insert<3>((uint32)w() >> bits.w());

   return r;
}


// SSE also fails to provide integer modulus operations.

inline Vector<int32, 4> operator % (
   const Vector<int32, 4>& a, const Vector<int32, 4>& b)
{
   Vector<int32, 4> r;
   r._r = r._r.i32Insert<0>(a.x() % b.x());
   r._r = r._r.i32Insert<1>(a.y() % b.y());
   r._r = r._r.i32Insert<2>(a.z() % b.z());
   r._r = r._r.i32Insert<3>(a.w() % b.w());

   return r;
}

inline Vector<int32, 4> operator % (
   const Vector<int32, 4>& a, const int32& b)
{
   Vector<int32, 4> r;
   r._r = r._r.i32Insert<0>(a.x() % b);
   r._r = r._r.i32Insert<1>(a.y() % b);
   r._r = r._r.i32Insert<2>(a.z() % b);
   r._r = r._r.i32Insert<3>(a.w() % b);

   return r;
}



//////////////////////////////////
// Loads and Stores Definitions //
//////////////////////////////////

// Generic implementations do not use SSE loads because of the potential need for
// up-conversion on the load.  Specializations for U == float will use the SSE
// instructions

template <class U> inline Vector<int32, 4> Vector<int32, 4>::load(U * addr)
{
   Vector<int32, 4> v;
   for (int i = 0; i < 4; ++i) v[i] = (int32)(*(addr + i));
   return v;
}

template <class U> inline Vector<int32, 4> Vector<int32, 4>::loadu(U * addr)
{
   Vector<int32, 4> v;
   for (int i = 0; i < 4; ++i) v[i] = (int32)(*(addr + i));
   return v;
}

template <class U> inline Vector<int32, 4> Vector<int32, 4>::loadBroadcast(U * addr)
{ return Vector<int32, 4>((int32)(*addr)); }

template <class U> inline Vector<int32, 4>
Vector<int32, 4>::gather(U * addr, const Vector<int32, 4>& offsets)
{
   Vector v;
   for (int i = 0; i < 4; ++i) v[i] = (int32)(*(addr + offsets[i]));
   return v;
}

template <class U> inline Vector<int32, 4>
Vector<int32, 4>::gather(U * addr, const Vector<int32, 4>& offsets, const Mask& m)
{
   Vector v;
   for (int i = 0; i < 4; ++i)
      if (m.bitMask() & (1 << i)) { /*printf("  Ouch! (mask = %s)\n", *m.toString());*/ v[i] = (int32)(*(addr + offsets[i])); }
   return v;
}

template <class U> inline void Vector<int32, 4>::store(U * addr) const
{
   *(addr + 0) = (U)x();
   *(addr + 1) = (U)y();
   *(addr + 2) = (U)z();
   *(addr + 3) = (U)w();
}

template <class U> inline void Vector<int32, 4>::storeOne(U * addr) const
{ *addr = (U)x(); }

template <class U> inline void
Vector<int32, 4>::scatter(U * addr, const Vector<int32, 4>& offsets) const
{
   *(addr + offsets[0]) = (U)x();
   *(addr + offsets[1]) = (U)y();
   *(addr + offsets[2]) = (U)z();
   *(addr + offsets[3]) = (U)w();
}

template <class U> inline void
Vector<int32, 4>::scatter(U * addr, const Vector<int32, 4>& offsets, const Mask& m) const
{
   int mask = m.bitMask();
   if (mask & 0x1) *(addr + offsets[0]) = (U)x();
   if (mask & 0x2) *(addr + offsets[1]) = (U)y();
   if (mask & 0x4) *(addr + offsets[2]) = (U)z();
   if (mask & 0x8) *(addr + offsets[3]) = (U)w();
}

/// Specialization for U == int32
template <> inline Vector<int32, 4> Vector<int32, 4>::load(int32 * addr)
{ return Vector(SSERegister::load((const SSERegister*)addr)); }

template <> inline Vector<int32, 4> Vector<int32, 4>::loadu(int32 * addr)
{ return Vector(SSERegister::loadu((const SSERegister*)addr)); }

template <> inline Vector<int32, 4> Vector<int32, 4>::loadBroadcast(int32 * addr)
{ return Vector(SSERegister::loadBroadcast((const SSERegister*)addr)); }

template <> inline void Vector<int32, 4>::store(int32 * addr) const
{ _r.store((SSERegister*)addr); }


///////////////////////////////////////////////////////////////////////////
//                             Mathematics                               //
///////////////////////////////////////////////////////////////////////////

/// Mathematics. See Mathematics.h for complete interface specification
template <> struct MATHEMATICS<Vector<int32, 4> >
{
private:
   typedef Vector<int32, 4> Vec4;

public:

   static inline Vec4 zero()                 { return Vec4(SSERegister::zeroes()); }
   static inline Vec4 one()                  { return Vec4(ONE<int32>()); }
   static inline Vec4 minusOne()             { return Vec4(MINUS_ONE<int32>()); }
   static inline Vec4 inf()                  { return Vec4(INF<int32>()); }
   static inline Vec4 _inf()                 { return Vec4(_INF<int32>()); }
   static inline Vec4 abs(Vec4 x)            { return x._r.i32Abs(); }

   static inline Vec4 clampz(Vec4 x, Vec4 u)
   { return blend(x < ZERO<Vec4>(), ZERO<Vec4>(), blend(x > u, u, x)); }

   static inline Vec4 sign(Vec4 x)
   {
      Vec4 r = ZERO<Vec4>();
      r[x < ZERO<Vec4>()] = MINUS_ONE<Vec4>();
      r[x > ZERO<Vec4>()] = ONE<Vec4>();
      return r;
   }

   static inline Vec4 min(Vec4 x, Vec4 y) { return SSERegister::i32Min(x._r, y._r); }
   static inline Vec4 max(Vec4 x, Vec4 y) { return SSERegister::i32Max(x._r, y._r); }

   static inline Vec4 min3(Vec4 x, Vec4 y, Vec4 z)  { return min(x, min(y, z)); }
   static inline Vec4 max3(Vec4 x, Vec4 y, Vec4 z)  { return max(x, max(y, z)); }
};


///////////////////////////////////////////////////////////////////////////
//                           Bit Manipulation                            //
///////////////////////////////////////////////////////////////////////////

template <> struct BITS<Vector<int32, 4> >
{
private:
   typedef Vector<int32, 4> Vec4;  ///< For shorthand, syntactical convenience

public:

   /// Bit rotations
   static inline Vec4 rotr(Vec4 a, uint8 b)
   { return Vec4(a._r.rotateRight32(b)); }
   static inline Vec4 rotl(Vec4 a, uint8 b)
   { return Vec4(a._r.rotateLeft32(b)); }

   /// Bit interleaves, no SSE instructions for this, very slow
   static inline Vec4 interleave11(Vec4 a, Vec4 b)
   {
      return Vec4(INTERLEAVE11(a[0], b[0]), INTERLEAVE11(a[1], b[1]),
                  INTERLEAVE11(a[2], b[2]), INTERLEAVE11(a[3], b[3]));
   }
   static inline Vec4 interleave21(Vec4 a, Vec4 b)
   {
      return Vec4(INTERLEAVE21(a[0], b[0]), INTERLEAVE21(a[1], b[1]),
                  INTERLEAVE21(a[2], b[2]), INTERLEAVE21(a[3], b[3]));
   }

   /// Counts '1' bits in an integer
   //static inline Vec4 bitCount(T a);
};

}; // namespace ToolChest


#endif // VECTOR4F_H
