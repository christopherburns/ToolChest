/* ************************************************************************* *\

                  INTEL CORPORATION PROPRIETARY INFORMATION
     This software is supplied under the terms of a license agreement or
     nondisclosure agreement with Intel Corporation and may not be copied
     or disclosed except in accordance with the terms of that agreement.
     Copyright (C) 2011 Intel Corporation. All Rights Reserved.

     Author: Burns, Christopher A.

\* ************************************************************************* */

#ifndef VECTOR4F_H
#define VECTOR4F_H

#pragma once

#include "Mask4.h"

namespace Burns
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
   FINLINE Vector() {}
   FINLINE explicit Vector(const float& t) : _r(t) {}
   FINLINE explicit Vector(const float t[4]) : _r(t[0], t[1], t[2], t[3]) {}  /// Reimplement using a load intrinsic
   FINLINE Vector(float i0, float i1, float i2, float i3) : _r(i0, i1, i2, i3) {}

   /// Construction from Vector3f + float
   FINLINE Vector(const Vector<float, 3>& v, float w) : _r(v.x(), v.y(), v.z(), w) {}

   /// Conversion from arbitrary (but cast-able) base type
   template <class S> FINLINE explicit Vector(const Vector<S, 4>& v) :
      _r((float)v.x(), (float)v.y(), (float)v.z(), (float)v.w()) {}


   /// Subscript accessors (introduces aliasing)
   FINLINE const float& operator [] (Index i) const { ASSERT(i < 4); return ((float*)&_r)[i]; }
   FINLINE float& operator [] (Index i) { ASSERT(i < 4); return ((float*)&_r)[i]; }

   /// Convenient static element accessors for Vector2, 3, 4
   FINLINE const float x() const { return _r.fpExtract<0>(); }
   FINLINE const float y() const { return _r.fpExtract<1>(); }
   FINLINE const float z() const { return _r.fpExtract<2>(); }
   FINLINE const float w() const { return _r.fpExtract<3>(); }

   FINLINE const Vector<float, 2> xy() const { return cast<Vector<float, 2>>(*this); } //{ return Vector2f(x(), y()); }
   FINLINE const Vector<float, 3> xyz() const { return cast<Vector<float, 3>>(*this); } //{ return Vector3f(x(), y(), z()); }

   template <int I> FINLINE float get() const { return _r.fpExtract<I>(); }
   template <int I> FINLINE void set(const float& v) { _r = _r.fpInsert<I>(v); }


   /// Conversion to String
   FINLINE String toString(int prec = 3) const
   { return String("[") + internalToString(prec) + "]"; }

   /// Type cast operators, defined in SSE.h
   operator Vector<int32, 4>() const;


   //////////////////////
   // Loads and Stores //
   //////////////////////

   /// All loads and stores are assumed to be aligned, address alignment will be
   /// asserted. Gather and Scatter operations are supported in base+offset
   /// format.


   template <class U> static Vector load(U * addr);      /// Aligned load
   template <class U> static Vector loadu(U * addr);     /// Unaligned load
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
   FINLINE Vector operator - () const                                       { return _r ^ SSERegister(0x80000000); }

   /// Binary arithmetical operators
   FINLINE friend Vector operator + (const Vector& a, const Vector& b)      { return SSERegister::fpAdd(a._r, b._r); }
   FINLINE friend Vector operator - (const Vector& a, const Vector& b)      { return SSERegister::fpSub(a._r, b._r); }
   FINLINE friend Vector operator * (const Vector& a, const Vector& b)      { return SSERegister::fpMul(a._r, b._r); }
   FINLINE friend Vector operator / (const Vector& a, const Vector& b)      { return SSERegister::fpDiv(a._r, b._r); }
   FINLINE friend Vector operator + (const Vector& a, const float& b)       { return a + Vector(b); }
   FINLINE friend Vector operator - (const Vector& a, const float& b)       { return a - Vector(b); }
   FINLINE friend Vector operator * (const Vector& a, const float& b)       { return a * Vector(b); }
   FINLINE friend Vector operator / (const Vector& a, const float& b)       { return a / Vector(b); }
   FINLINE friend Vector operator + (const float& a, const Vector& b)       { return b + a; }
   FINLINE friend Vector operator - (const float& a, const Vector& b)       { return Vector(a) - b; }
   FINLINE friend Vector operator * (const float& a, const Vector& b)       { return b * a; }
   FINLINE friend Vector operator / (const float& a, const Vector& b)       { return Vector(a) / b; }

   //FINLINE Vector operator << (const int bits)                              { return _r.shiftLeft32(bits); }
   //FINLINE Vector operator >> (const int bits)                              { return _r.shiftRightA32(bits); }

   /// Accumulation operators
   FINLINE Vector& operator += (const Vector& b)                            { return *this = *this + b; }
   FINLINE Vector& operator -= (const Vector& b)                            { return *this = *this - b; }
   FINLINE Vector& operator *= (const Vector& b)                            { return *this = *this * b; }
   FINLINE Vector& operator /= (const Vector& b)                            { return *this = *this / b; }
   FINLINE Vector& operator += (const float& b)                             { return *this += Vector(b); }
   FINLINE Vector& operator -= (const float& b)                             { return *this -= Vector(b); }
   FINLINE Vector& operator *= (const float& b)                             { return *this *= Vector(b); }
   FINLINE Vector& operator /= (const float& b)                             { return *this /= Vector(b); }



   ////////////////
   // Comparison //
   ////////////////

   FINLINE friend Mask operator == (const Vector& a, const Vector& b)       { return SSERegister::fpEquality(a._r, b._r); }
   FINLINE friend Mask operator != (const Vector& a, const Vector& b)       { return SSERegister::fpInequality(a._r, b._r); }
   FINLINE friend Mask operator <  (const Vector& a, const Vector& b)       { return SSERegister::fpLessThan(a._r, b._r); }
   FINLINE friend Mask operator >  (const Vector& a, const Vector& b)       { return SSERegister::fpGreaterThan(a._r, b._r); }
   FINLINE friend Mask operator <= (const Vector& a, const Vector& b)       { return SSERegister::fpLessThanOrEqual(a._r, b._r); }
   FINLINE friend Mask operator >= (const Vector& a, const Vector& b)       { return SSERegister::fpGreaterThanOrEqual(a._r, b._r); }


   ////////////////////////////////////
   // Blending, Masking, and Shuffle //
   ////////////////////////////////////

   /// Returns a vector blending the values from tValue where m is true with the
   /// values from fValue where m is false:
   ///     blend( (t,t,f,t), (1,2,3,4), (-1,-2,-3,-4) ) ==> (1,2,-3,4)
   friend FINLINE const Vector blend(const Mask& m, const Vector& tValue, const Vector& fValue)
   { return Vector(m._m.blend(tValue._r, fValue._r)); }

   /// Update
   friend FINLINE Vector& update(const Mask& m, Vector& lhs, const Vector& rhs)
   { lhs._r = m._m.blend(rhs._r, lhs._r); return lhs; }

   /// Masking operator
   FINLINE WriteMaskedVector<Vector> operator [] (const Mask& m)
   { return WriteMaskedVector<Vector>(*this, m); }


   /// Shuffle
   template<uint8 i0, uint8 i1, uint8 i2, uint8 i3> FINLINE Vector shuffle() const
   { return _r.shuffle32<i0, i1, i2, i3>(); }


   //////////////////////////
   // Reduction Operations //
   //////////////////////////

   FINLINE float reduceSum() const     { return _r.fpReduceAdd().fpExtract<0>(); }
   FINLINE float reduceProduct() const { return _r.fpReduceMul().fpExtract<0>(); }
   FINLINE float reduceMin() const     { return _r.fpReduceMin().fpExtract<0>(); }
   FINLINE float reduceMax() const     { return _r.fpReduceMax().fpExtract<0>(); }

   //////////////////////////
   // Geometric Operations //
   //////////////////////////

   FINLINE float average() const           { return reduceSum() / 4.0f; }
   FINLINE float length() const            { return SQRT(lengthSquared()); }
   FINLINE float lengthSquared() const     { return dot(*this, *this); }
   FINLINE Vector unit() const             { return (*this) * RCP_SQRT(lengthSquared()); }

   FINLINE static float dot(const Vector& v0, const Vector& v1)
   { return SSERegister::fpDotProduct4(v0._r, v1._r).fpExtract<0>(); }


   ////////////////////////////
   // Functional Programming //
   ////////////////////////////

   template <typename Functor> FINLINE Vector map(const Functor& f) const
   { return Vector(f(x()), f(y()), f(z()), f(w())); }


   ///////////////////////////////////////////////////////////////////////////
   //                      Private Data And Methods                         //
   ///////////////////////////////////////////////////////////////////////////

public:
   SSERegister _r;

   /// Private constructors
   FINLINE Vector(const __m128 reg) : _r(reg) {}
   FINLINE Vector(const SSERegister& reg) : _r(reg) {}

   /// Returns the register inside the object
   FINLINE SSERegister getRegister() const { return _r; }

   /// Vector<float, 4> is a base case, no more recursion
   INLINE String internalToString(int prec = 3) const
   {
      //int prec = 8;
      return
         String((*this)[0], prec) + ", " + String((*this)[1], prec) + ", " +
         String((*this)[2], prec) + ", " + String((*this)[3], prec);
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

template <class U> FINLINE Vector<float, 4> Vector<float, 4>::load(U * addr)
{
   Vector<float, 4> v;
   for (Index i = 0; i < 4; ++i) v[i] = (float)(*(addr + i));
   return v;
}

template <class U> FINLINE Vector<float, 4> Vector<float, 4>::loadu(U * addr)
{
   Vector<float, 4> v;
   for (Index i = 0; i < 4; ++i) v[i] = (float)(*(addr + i));
   return v;
}


template <class U> FINLINE Vector<float, 4> Vector<float, 4>::loadBroadcast(U * addr)
{ return Vector<float, 4>((float)(*addr)); }

template <class U> FINLINE Vector<float, 4>
Vector<float, 4>::gather(U * addr, const Vector<int32, 4>& offsets)
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
   for (Index i = 0; i < 4; ++i) v[i] = (float)(*(addr + offsets[i]));
   return v;
#endif
}

template <class U> FINLINE Vector<float, 4>
Vector<float, 4>::gather(U * addr, const Vector<int32, 4>& offsets, const Mask& m)
{
   Vector v;
   for (Index i = 0; i < 4; ++i)
      if (m.bitMask() & (1 << i)) v[i] = (float)(*(addr + offsets[i]));
   return v;
}


template <class U> FINLINE void Vector<float, 4>::store(U * addr) const
{
   *(addr + 0) = (U)x();
   *(addr + 1) = (U)y();
   *(addr + 2) = (U)z();
   *(addr + 3) = (U)w();
}

template <class U> FINLINE void Vector<float, 4>::storeOne(U * addr) const
{ *addr = (U)x(); }

template <class U> FINLINE void
Vector<float, 4>::scatter(U * addr, const Vector<int32, 4>& offsets) const
{
   *(addr + offsets[0]) = (U)x();
   *(addr + offsets[1]) = (U)y();
   *(addr + offsets[2]) = (U)z();
   *(addr + offsets[3]) = (U)w();
}

template <class U> FINLINE void
Vector<float, 4>::scatter(U * addr, const Vector<int32, 4>& offsets, const Mask& m) const
{
   int mask = m.bitMask();
   if (mask & 0x1) *(addr + offsets[0]) = (U)x();
   if (mask & 0x2) *(addr + offsets[1]) = (U)y();
   if (mask & 0x4) *(addr + offsets[2]) = (U)z();
   if (mask & 0x8) *(addr + offsets[3]) = (U)w();
}

/// Specialization for U == float
template <> FINLINE Vector<float, 4> Vector<float, 4>::load(float * addr)
{ return Vector(SSERegister::load((const SSERegister*)addr)); }

template <> FINLINE Vector<float, 4> Vector<float, 4>::loadu(float * addr)
{ return Vector(SSERegister::loadu((const SSERegister*)addr)); }


template <> FINLINE Vector<float, 4> Vector<float, 4>::loadBroadcast(float * addr)
{ return Vector(SSERegister::loadBroadcast((const SSERegister*)addr)); }

template <> FINLINE void Vector<float, 4>::store(float * addr) const
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

   static FINLINE Vec4 zero()                 { return Vec4(SSERegister::zeroes()); }
   static FINLINE Vec4 one()                  { return Vec4(ONE<float>()); }
   static FINLINE Vec4 minusOne()             { return Vec4(MINUS_ONE<float>()); }
   static FINLINE Vec4 inf()                  { return Vec4(INF<float>()); }
   static FINLINE Vec4 _inf()                 { return Vec4(_INF<float>()); }
   static FINLINE Vec4 maxv()                 { return Vec4(MAXV<float>()); }
   static FINLINE Vec4 e()                    { return Vec4(E<float>()); }
   static FINLINE Vec4 pi()                   { return Vec4(PI<float>()); }
   static FINLINE Vec4 rcpPi()                { return Vec4(RCP_PI<float>()); }
   static FINLINE Vec4 phi()                  { return Vec4(PHI<float>()); }
   static FINLINE Vec4 sqrt2()                { return Vec4(SQRT2<float>()); }

   //static FINLINE Vec4 cbrt(Vec4 x)           {  }
   static FINLINE Vec4 sqrt(Vec4 x)           { return x._r.fpSqrt(); }
   static FINLINE Vec4 rcpSqrt(Vec4 x)        { return x._r.fpRCPSqrt(); }
   static FINLINE Vec4 rcp(Vec4 x)            { return x._r.fpReciprocal(); }
   static FINLINE Vec4 cos(Vec4 x)            { return Vec4(COS(x.x()), COS(x.y()), COS(x.z()), COS(x.w())); }
   static FINLINE Vec4 sin(Vec4 x)            { return Vec4(SIN(x.x()), SIN(x.y()), SIN(x.z()), SIN(x.w())); }
   static FINLINE Vec4 tan(Vec4 x)            { return Vec4(TAN(x.x()), TAN(x.y()), TAN(x.z()), TAN(x.w())); }
   static FINLINE Vec4 pow(Vec4 x, Vec4 y)
   {
      return Vec4(POW(x.x(), y.x()), POW(x.y(), y.y()),
                  POW(x.z(), y.z()), POW(x.w(), y.w()));
   }
   //static FINLINE Vec4 exp(Vec4 x)            {  }
   static FINLINE Vec4 log(Vec4 x)
   { return Vec4(LOG(x.x()), LOG(x.y()), LOG(x.z()), LOG(x.w())); }
   static FINLINE Vec4 log2(Vec4 x)
   { return Vec4(LOG2(x.x()), LOG2(x.y()), LOG2(x.z()), LOG2(x.w())); }
   //static FINLINE Vec4 log10(Vec4 x)          {  }
   static FINLINE Vec4 abs(Vec4 x)            { return x._r.fpAbs(); }
   /*static FINLINE Vec4 acos(Vec4 x)           {  }
   static FINLINE Vec4 asin(Vec4 x)           {  }
   static FINLINE Vec4 atan2(Vec4 x, Vec4 y)  {  }*/
   static FINLINE Vec4 floor(Vec4 x)          { return x._r.fpFloor(); }
   static FINLINE Vec4 ceil(Vec4 x)           { return x._r.fpCeiling(); }

   static FINLINE Vec4 clampz(Vec4 x, Vec4 u)
   { return blend(x < ZERO<Vec4>(), ZERO<Vec4>(), blend(x > u, u, x)); }

   static FINLINE Vec4 sign(Vec4 x)
   {
      Vec4 r = ZERO<Vec4>();
      r[x < ZERO<Vec4>()] = MINUS_ONE<Vec4>();
      r[x > ZERO<Vec4>()] = ONE<Vec4>();
      /*printf("   x = %s\n", *x.toString());
      printf("   r < ZERO<Vec4>() = %s\n", *(x < ZERO<Vec4>()).toString());
      printf("   r > ZERO<Vec4>() = %s\n", *(x > ZERO<Vec4>()).toString());
      printf("   r = %s\n", *r.toString());*/
      return r;
   }

   static FINLINE Vec4 min(Vec4 x, Vec4 y) { return SSERegister::fpMin(x._r, y._r); }
   static FINLINE Vec4 max(Vec4 x, Vec4 y) { return SSERegister::fpMax(x._r, y._r); }

   static FINLINE Vec4 min3(Vec4 x, Vec4 y, Vec4 z)  { return min(x, min(y, z)); }
   static FINLINE Vec4 max3(Vec4 x, Vec4 y, Vec4 z)  { return max(x, max(y, z)); }
};

}; // namespace Burns


#endif // VECTOR4F_H
