#ifndef VECTOR4F_H
#define VECTOR4F_H

#include "Mask4.h"

namespace Mathematics
{

//////////////////////////////////////////////////////////////////////////////
//                                 TypeInfo                                 //
//////////////////////////////////////////////////////////////////////////////

template <> class TypeInfo<Vector<float, 4> >
{
public:
   static const bool Integral = TypeInfo<float>::Integral;
   static const bool Unsigned = TypeInfo<float>::Unsigned;
   static const int  Rank = 1;
   static const int  Width = 4;

   typedef float ElementType;
   typedef Mask<bool, 4> Mask;
};

template <> class Vector<float, 4>
{
   ///////////////////////////////////////////////////////////////////////////
   //                           Public Interface                            //
   ///////////////////////////////////////////////////////////////////////////

public:

   /// Define our mask type
   typedef TypeInfo<Vector>::Mask Mask;

   /// Constructors
   inline Vector() {}
   inline explicit Vector(const float& t) : _r(t) {}
   inline explicit Vector(const float t[4]) : _r(t[0], t[1], t[2], t[3]) {}  /// Reimplement using a load intrinsic
   inline Vector(float i0, float i1, float i2, float i3) : _r(i0, i1, i2, i3) {}

   /// Construction from Vector3f + float
   inline Vector(const Vector<float, 3>& v, float w) : _r(v.x(), v.y(), v.z(), w) {}

   /// Conversion from arbitrary (but cast-able) base type
   template <class S> inline explicit Vector(const Vector<S, 4>& v) :
      _r((float)v.x(), (float)v.y(), (float)v.z(), (float)v.w()) {}


   /// Subscript accessors (introduces aliasing)
   inline const float& operator [] (int i) const { assert(i < 4); return ((float*)&_r)[i]; }
   inline float& operator [] (int i) { assert(i < 4); return ((float*)&_r)[i]; }

   /// Convenient static element accessors for Vector2, 3, 4
   inline const float x() const { return _r.fpExtract<0>(); }
   inline const float y() const { return _r.fpExtract<1>(); }
   inline const float z() const { return _r.fpExtract<2>(); }
   inline const float w() const { return _r.fpExtract<3>(); }

   inline const Vector<float, 2> xy() const { return cast<Vector<float, 2>>(*this); } //{ return Vector2f(x(), y()); }
   inline const Vector<float, 3> xyz() const { return cast<Vector<float, 3>>(*this); } //{ return Vector3f(x(), y(), z()); }

   template <int I> inline float Get() const { return _r.fpExtract<I>(); }
   template <int I> inline void Set(const float& v) { _r = _r.fpInsert<I>(v); }


   /// Conversion to std::string
   inline std::string ToString(int prec = 3) const
   { return std::string("[") + internalToString(prec) + "]"; }

   /// Type cast operators, defined in SSE.h
   operator Vector<int32_t, 4>() const;


   //////////////////////
   // Loads and Stores //
   //////////////////////

   /// All loads and stores are assumed to be aligned, address alignment will be
   /// asserted. Gather and Scatter operations are supported in base+offset
   /// format.


   template <class U> static inline Vector Load(U * addr);      /// Aligned load
   template <class U> static inline Vector Loadu(U * addr);     /// Unaligned load
   template <class U> static inline Vector LoadBroadcast(U * addr);
   template <class U> static inline Vector Gather(U * addr, const Vector<int32_t, 4>& offsets);
   template <class U> static inline Vector Gather(U * addr, const Vector<int32_t, 4>& offsets, const Mask& m);

   template <class U> inline void Store(U * addr) const;
   template <class U> inline void StoreOne(U * addr) const;
   template <class U> inline void Scatter(U * addr, const Vector<int32_t, 4>& offsets) const;
   template <class U> inline void Scatter(U * addr, const Vector<int32_t, 4>& offsets, const Mask& m) const;


   ////////////////////////////
   // Mathematical Operators //
   ////////////////////////////

   /// Unary minus
   inline Vector operator - () const                                       { return _r ^ SSERegister(0x80000000); }

   /// Binary arithmetical operators
   inline friend Vector operator + (const Vector& a, const Vector& b)      { return SSERegister::fpAdd(a._r, b._r); }
   inline friend Vector operator - (const Vector& a, const Vector& b)      { return SSERegister::fpSub(a._r, b._r); }
   inline friend Vector operator * (const Vector& a, const Vector& b)      { return SSERegister::fpMul(a._r, b._r); }
   inline friend Vector operator / (const Vector& a, const Vector& b)      { return SSERegister::fpDiv(a._r, b._r); }
   inline friend Vector operator + (const Vector& a, const float& b)       { return a + Vector(b); }
   inline friend Vector operator - (const Vector& a, const float& b)       { return a - Vector(b); }
   inline friend Vector operator * (const Vector& a, const float& b)       { return a * Vector(b); }
   inline friend Vector operator / (const Vector& a, const float& b)       { return a / Vector(b); }
   inline friend Vector operator + (const float& a, const Vector& b)       { return b + a; }
   inline friend Vector operator - (const float& a, const Vector& b)       { return Vector(a) - b; }
   inline friend Vector operator * (const float& a, const Vector& b)       { return b * a; }
   inline friend Vector operator / (const float& a, const Vector& b)       { return Vector(a) / b; }

   //inline Vector operator << (const int bits)                              { return _r.shiftLeft32(bits); }
   //inline Vector operator >> (const int bits)                              { return _r.shiftRightA32(bits); }

   /// Accumulation operators
   inline Vector& operator += (const Vector& b)                            { return *this = *this + b; }
   inline Vector& operator -= (const Vector& b)                            { return *this = *this - b; }
   inline Vector& operator *= (const Vector& b)                            { return *this = *this * b; }
   inline Vector& operator /= (const Vector& b)                            { return *this = *this / b; }
   inline Vector& operator += (const float& b)                             { return *this += Vector(b); }
   inline Vector& operator -= (const float& b)                             { return *this -= Vector(b); }
   inline Vector& operator *= (const float& b)                             { return *this *= Vector(b); }
   inline Vector& operator /= (const float& b)                             { return *this /= Vector(b); }



   ////////////////
   // Comparison //
   ////////////////

   inline friend Mask operator == (const Vector& a, const Vector& b)       { return SSERegister::fpEquality(a._r, b._r); }
   inline friend Mask operator != (const Vector& a, const Vector& b)       { return SSERegister::fpInequality(a._r, b._r); }
   inline friend Mask operator <  (const Vector& a, const Vector& b)       { return SSERegister::fpLessThan(a._r, b._r); }
   inline friend Mask operator >  (const Vector& a, const Vector& b)       { return SSERegister::fpGreaterThan(a._r, b._r); }
   inline friend Mask operator <= (const Vector& a, const Vector& b)       { return SSERegister::fpLessThanOrEqual(a._r, b._r); }
   inline friend Mask operator >= (const Vector& a, const Vector& b)       { return SSERegister::fpGreaterThanOrEqual(a._r, b._r); }


   ////////////////////////////////////
   // Blending, Masking, and Shuffle //
   ////////////////////////////////////

   /// Returns a vector blending the values from tValue where m is true with the
   /// values from fValue where m is false:
   ///     blend( (t,t,f,t), (1,2,3,4), (-1,-2,-3,-4) ) ==> (1,2,-3,4)
   friend inline const Vector Blend(const Mask& m, const Vector& tValue, const Vector& fValue)
   { return Vector(m._m.blend(tValue._r, fValue._r)); }

   /// Update
   friend inline Vector& Update(const Mask& m, Vector& lhs, const Vector& rhs)
   { lhs._r = m._m.blend(rhs._r, lhs._r); return lhs; }

   /// Masking operator
   inline WriteMaskedVector<Vector> operator [] (const Mask& m)
   { return WriteMaskedVector<Vector>(*this, m); }


   /// Shuffle
   template<uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3> inline Vector Shuffle() const
   { return _r.shuffle32<i0, i1, i2, i3>(); }


   //////////////////////////
   // Reduction Operations //
   //////////////////////////

   inline float ReduceSum() const     { return _r.fpReduceAdd().fpExtract<0>(); }
   inline float ReduceProduct() const { return _r.fpReduceMul().fpExtract<0>(); }
   inline float ReduceMin() const     { return _r.fpReduceMin().fpExtract<0>(); }
   inline float ReduceMax() const     { return _r.fpReduceMax().fpExtract<0>(); }

   //////////////////////////
   // Geometric Operations //
   //////////////////////////

   inline float Average() const           { return ReduceSum() / 4.0f; }
   inline float Length() const            { return SQRT(LengthSquared()); }
   inline float LengthSquared() const     { return Dot(*this, *this); }
   inline Vector Unit() const             { return (*this) * RCP_SQRT(LengthSquared()); }

   inline static float Dot(const Vector& v0, const Vector& v1)
   { return SSERegister::fpDotProduct4(v0._r, v1._r).fpExtract<0>(); }


   ////////////////////////////
   // Functional Programming //
   ////////////////////////////

   template <typename Functor> inline Vector Map(const Functor& f) const
   { return Vector(f(x()), f(y()), f(z()), f(w())); }


   ///////////////////////////////////////////////////////////////////////////
   //                      Private Data And Methods                         //
   ///////////////////////////////////////////////////////////////////////////

private:
   SSERegister _r;

   /// Private constructors
   inline Vector(const __m128 reg) : _r(reg) {}
   inline Vector(const SSERegister& reg) : _r(reg) {}

   /// Returns the register inside the object
   inline SSERegister getRegister() const { return _r; }

   /// Vector<float, 4> is a base case, no more recursion
   inline std::string internalToString(int prec = 3) const
   {
      //int prec = 8;
      return
         std::string((*this)[0], prec) + ", " + std::string((*this)[1], prec) + ", " +
         std::string((*this)[2], prec) + ", " + std::string((*this)[3], prec);
   }

   friend struct MATHEMATICS<Vector<float, 4> >;
   template <class U, int M> friend class Vector;
};


//////////////////////
// Loads and Stores //
//////////////////////

// Generic implementations do not use SSE loads because of the potential need for
// up-conversion on the load.  Specializations for U == float will use the SSE
// instructions

template <class U> inline Vector<float, 4> Vector<float, 4>::Load(U * addr)
{
   Vector<float, 4> v;
   for (int i = 0; i < 4; ++i) v[i] = (float)(*(addr + i));
   return v;
}

template <class U> inline Vector<float, 4> Vector<float, 4>::Loadu(U * addr)
{
   Vector<float, 4> v;
   for (int i = 0; i < 4; ++i) v[i] = (float)(*(addr + i));
   return v;
}


template <class U> inline Vector<float, 4> Vector<float, 4>::LoadBroadcast(U * addr)
{ return Vector<float, 4>((float)(*addr)); }

template <class U> inline Vector<float, 4>
Vector<float, 4>::Gather(U * addr, const Vector<int32_t, 4>& offsets)
{
#if 1
   return
      Vector(
         (float)(*(addr + offsets.x())),
         (float)(*(addr + offsets.y())),
         (float)(*(addr + offsets.z())),
         (float)(*(addr + offsets.w())));
#else
   Vector v;
   for (int i = 0; i < 4; ++i) v[i] = (float)(*(addr + offsets[i]));
   return v;
#endif
}

template <class U> inline Vector<float, 4>
Vector<float, 4>::Gather(U * addr, const Vector<int32_t, 4>& offsets, const Mask& m)
{
   Vector v;
   for (int i = 0; i < 4; ++i)
      if (m.BitMask() & (1 << i)) v[i] = (float)(*(addr + offsets[i]));
   return v;
}


template <class U> inline void Vector<float, 4>::Store(U * addr) const
{
   *(addr + 0) = (U)x();
   *(addr + 1) = (U)y();
   *(addr + 2) = (U)z();
   *(addr + 3) = (U)w();
}

template <class U> inline void Vector<float, 4>::StoreOne(U * addr) const
{ *addr = (U)x(); }

template <class U> inline void
Vector<float, 4>::Scatter(U * addr, const Vector<int32_t, 4>& offsets) const
{
   *(addr + offsets[0]) = (U)x();
   *(addr + offsets[1]) = (U)y();
   *(addr + offsets[2]) = (U)z();
   *(addr + offsets[3]) = (U)w();
}

template <class U> inline void
Vector<float, 4>::Scatter(U * addr, const Vector<int32_t, 4>& offsets, const Mask& m) const
{
   int mask = m.BitMask();
   if (mask & 0x1) *(addr + offsets[0]) = (U)x();
   if (mask & 0x2) *(addr + offsets[1]) = (U)y();
   if (mask & 0x4) *(addr + offsets[2]) = (U)z();
   if (mask & 0x8) *(addr + offsets[3]) = (U)w();
}

/// Specialization for U == float
template <> inline Vector<float, 4> Vector<float, 4>::Load(float * addr)
{ return Vector(SSERegister::load((const SSERegister*)addr)); }

template <> inline Vector<float, 4> Vector<float, 4>::Loadu(float * addr)
{ return Vector(SSERegister::loadu((const SSERegister*)addr)); }


template <> inline Vector<float, 4> Vector<float, 4>::LoadBroadcast(float * addr)
{ return Vector(SSERegister::loadBroadcast((const SSERegister*)addr)); }

template <> inline void Vector<float, 4>::Store(float * addr) const
{ _r.store((SSERegister*)addr); }



///////////////////////////////////////////////////////////////////////////
//                             Mathematics                               //
///////////////////////////////////////////////////////////////////////////

/// Mathematics. See Mathematics.h for complete interface specification
template <> struct MATHEMATICS<Vector<float, 4> >
{
private:
   typedef Vector<float, 4> Vec4;

public:

   static inline Vec4 zero()                 { return Vec4(SSERegister::zeroes()); }
   static inline Vec4 one()                  { return Vec4(ONE<float>()); }
   static inline Vec4 minusOne()             { return Vec4(MINUS_ONE<float>()); }
   static inline Vec4 inf()                  { return Vec4(INF<float>()); }
   static inline Vec4 _inf()                 { return Vec4(_INF<float>()); }
   static inline Vec4 maxv()                 { return Vec4(MAXV<float>()); }
   static inline Vec4 e()                    { return Vec4(E<float>()); }
   static inline Vec4 pi()                   { return Vec4(PI<float>()); }
   static inline Vec4 rcpPi()                { return Vec4(RCP_PI<float>()); }
   static inline Vec4 phi()                  { return Vec4(PHI<float>()); }
   static inline Vec4 sqrt2()                { return Vec4(SQRT2<float>()); }

   //static inline Vec4 cbrt(Vec4 x)           {  }
   static inline Vec4 sqrt(Vec4 x)           { return x._r.fpSqrt(); }
   static inline Vec4 rcpSqrt(Vec4 x)        { return x._r.fpRCPSqrt(); }
   static inline Vec4 rcp(Vec4 x)            { return x._r.fpReciprocal(); }
   static inline Vec4 cos(Vec4 x)            { return Vec4(COS(x.x()), COS(x.y()), COS(x.z()), COS(x.w())); }
   static inline Vec4 sin(Vec4 x)            { return Vec4(SIN(x.x()), SIN(x.y()), SIN(x.z()), SIN(x.w())); }
   static inline Vec4 tan(Vec4 x)            { return Vec4(TAN(x.x()), TAN(x.y()), TAN(x.z()), TAN(x.w())); }
   static inline Vec4 pow(Vec4 x, Vec4 y)
   {
      return Vec4(POW(x.x(), y.x()), POW(x.y(), y.y()),
                  POW(x.z(), y.z()), POW(x.w(), y.w()));
   }
   //static inline Vec4 exp(Vec4 x)            {  }
   static inline Vec4 log(Vec4 x)
   { return Vec4(LOG(x.x()), LOG(x.y()), LOG(x.z()), LOG(x.w())); }
   static inline Vec4 log2(Vec4 x)
   { return Vec4(LOG2(x.x()), LOG2(x.y()), LOG2(x.z()), LOG2(x.w())); }
   //static inline Vec4 log10(Vec4 x)          {  }
   static inline Vec4 abs(Vec4 x)            { return x._r.fpAbs(); }
   /*static inline Vec4 acos(Vec4 x)           {  }
   static inline Vec4 asin(Vec4 x)           {  }
   static inline Vec4 atan2(Vec4 x, Vec4 y)  {  }*/
   static inline Vec4 floor(Vec4 x)          { return x._r.fpFloor(); }
   static inline Vec4 ceil(Vec4 x)           { return x._r.fpCeiling(); }

   static inline Vec4 clampz(Vec4 x, Vec4 u)
   { return Blend(x < ZERO<Vec4>(), ZERO<Vec4>(), Blend(x > u, u, x)); }

   static inline Vec4 sign(Vec4 x)
   {
      Vec4 r = ZERO<Vec4>();
      r[x < ZERO<Vec4>()] = MINUS_ONE<Vec4>();
      r[x > ZERO<Vec4>()] = ONE<Vec4>();
      /*printf("   x = %s\n", *x.Tostd::string());
      printf("   r < ZERO<Vec4>() = %s\n", *(x < ZERO<Vec4>()).Tostd::string());
      printf("   r > ZERO<Vec4>() = %s\n", *(x > ZERO<Vec4>()).Tostd::string());
      printf("   r = %s\n", *r.Tostd::string());*/
      return r;
   }

   static inline Vec4 min(Vec4 x, Vec4 y) { return SSERegister::fpMin(x._r, y._r); }
   static inline Vec4 max(Vec4 x, Vec4 y) { return SSERegister::fpMax(x._r, y._r); }

   static inline Vec4 min3(Vec4 x, Vec4 y, Vec4 z)  { return min(x, min(y, z)); }
   static inline Vec4 max3(Vec4 x, Vec4 y, Vec4 z)  { return max(x, max(y, z)); }
};

}; // namespace Mathematics


#endif // VECTOR4F_H
