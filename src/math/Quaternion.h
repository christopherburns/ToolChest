/* ************************************************************************* *\

                  INTEL CORPORATION PROPRIETARY INFORMATION
     This software is supplied under the terms of a license agreement or
     nondisclosure agreement with Intel Corporation and may not be copied
     or disclosed except in accordance with the terms of that agreement.
     Copyright (C) 2011 Intel Corporation. All Rights Reserved.

     Author: Burns, Christopher A.

\* ************************************************************************* */

#ifndef QUATERNION_H
#define QUATERNION_H

#pragma once


namespace Burns
{

template <class T>
class Quaternion
{
   typedef Vector<T, 4>                Vector4;
   typedef Vector<T, 3>                Vector3;
   typedef typename Vector<T, 4>::Mask Mask4;
   typedef typename TypeInfo<T>::Mask  Mask;


private:

   Vector4 _q;   // This should always be normalized...

   FINLINE Quaternion(const T& f0, const T& f1, const T& f2, const T& f3)
      : _q(f0, f1, f2, f3) { STATIC_ASSERT(!TypeInfo<T>::Integral); }

public:

   FINLINE Quaternion() {}
   FINLINE Quaternion(const T& f) : _q(f, ZERO<T>(), ZERO<T>(), ZERO<T>()) {}
   FINLINE explicit Quaternion(const Vector3& v) : _q(ZERO<T>(), v.x(), v.y(), v.z()) {}
   FINLINE explicit Quaternion(const Vector4& v) : _q(v) {}

   /// Accessors
   FINLINE T r() const { return _q.x(); }
   FINLINE T i() const { return _q.y(); }
   FINLINE T j() const { return _q.z(); }
   FINLINE T k() const { return _q.w(); }

   FINLINE Vector3 ijk() const { return Vector3(i(), j(), k()); }

   /// Temporary!!
   FINLINE T operator [] (int index) const
   {
      switch (index)
      {
         case 0: return _q.x();
         case 1: return _q.y();
         case 2: return _q.z();
         case 3: return _q.w();
         default: ASSERT(false);
      }
   }

   ////////////////
   // Arithmetic //
   ////////////////

   /// Unary operator -
   FINLINE Quaternion operator - () const { return Quaternion(-_q); }

   /// Conjugate (complex arithmetic)
   FINLINE Quaternion conjugate() const { return Quaternion(r(), -i(), -j(), -k()); }

   /// Reciprocal operation
   FINLINE Quaternion reciprocal() const
   { return Quaternion(conjugate() / (Vector4::dot(_q, _q))); }

   /// Dot product, four component
   static FINLINE T dot(const Quaternion& q)
   { return Vector4::dot(q._q, q._q); }

   static FINLINE T dot(const Quaternion& a, const Quaternion& b)
   { return Vector4::dot(a._q, b._q); }

   /// Returns the unit-length version of this quaternion
   FINLINE Quaternion unit() const
   { return (*this) * RCP_SQRT(dot(*this, *this)); }


   /// Quaternion arithmetic
   friend FINLINE Quaternion operator + (const Quaternion& lhs, const Quaternion& rhs)
   { return Quaternion(lhs._q + rhs._q); }
   friend FINLINE Quaternion operator - (const Quaternion& lhs, const Quaternion& rhs)
   { return Quaternion(lhs._q - rhs._q); }
   friend FINLINE Quaternion operator * (const Quaternion& lhs, const Quaternion& rhs)
   {
      return Quaternion(
         lhs.r()*rhs.r() - lhs.i()*rhs.i() - lhs.j()*rhs.j() - lhs.k()*rhs.k(),
         lhs.r()*rhs.i() + lhs.i()*rhs.r() + lhs.j()*rhs.k() - lhs.k()*rhs.j(),
         lhs.r()*rhs.j() - lhs.i()*rhs.k() + lhs.j()*rhs.r() + lhs.k()*rhs.i(),
         lhs.r()*rhs.k() + lhs.i()*rhs.j() - lhs.j()*rhs.i() + lhs.k()*rhs.r());
   }
   friend FINLINE Quaternion operator / (const Quaternion& lhs, const Quaternion& rhs)
   { return lhs * rhs.reciprocal(); }

   /// Scalar arithmetic
   friend FINLINE Quaternion operator + (const Quaternion& lhs, const T& rhs)
   { return Quaternion(lhs.r() + rhs, lhs.i(), lhs.j(), lhs.k()); }
   friend FINLINE Quaternion operator - (const Quaternion& lhs, const T& rhs)
   { return Quaternion(lhs.r() - rhs, lhs.i(), lhs.j(), lhs.k()); }
   friend FINLINE Quaternion operator * (const Quaternion& lhs, const T& rhs)
   { return Quaternion(lhs._q * rhs); }
   friend FINLINE Quaternion operator / (const Quaternion& lhs, const T & rhs)
   { return lhs * RCP(rhs); }

   FINLINE Quaternion& operator += (const Quaternion& rhs) { return (*this) = (*this) + rhs; }
   FINLINE Quaternion& operator -= (const Quaternion& rhs) { return (*this) = (*this) - rhs; }
   FINLINE Quaternion& operator *= (const Quaternion& rhs) { return (*this) = (*this) * rhs; }
   FINLINE Quaternion& operator /= (const Quaternion& rhs) { return (*this) = (*this) / rhs; }

   FINLINE Quaternion& operator += (const T& rhs) { return (*this) = (*this) + rhs; }
   FINLINE Quaternion& operator -= (const T& rhs) { return (*this) = (*this) - rhs; }
   FINLINE Quaternion& operator *= (const T& rhs) { return (*this) = (*this) * rhs; }
   FINLINE Quaternion& operator /= (const T& rhs) { return (*this) = (*this) / rhs; }


   ///////////////
   // Rotations //
   ///////////////

   /// Construction from an angle (radians) and a unit-length 3-vector axis of
   /// rotation
   FINLINE Quaternion(const T& angle, const Vector3& axis)
   {
      Vector3 s = axis * SIN(angle * 0.5f);
      _q = Vector4(COS(angle * 0.5f), s.x(), s.y(), s.z());
   }

   /// Construction from the three columns of a 3x3 ORTHOGONAL rotation matrix
   INLINE Quaternion(const Vector3& cX, const Vector3& cY, const Vector3& cZ)
   {
      /// We follow the "robust" algorithm outlined in the following article:
      /// http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation

#if 1
      T trace = cX.x() + cY.y() + cZ.z();
      if (trace >= ZERO<T>())
      {
         const T t = (ONE<T>() + trace);
         const T s = RCP_SQRT(t) * 0.5f;
         _q = Vector4(t*s, (cY.z()-cZ.y())*s, (cZ.x() - cX.z())*s, (cX.y()-cY.x())*s);
      }
      else
      {
         if ((cX.x()) > MAX((cY.y()), (cZ.z())))
         {
            const T t = ((ONE<T>() + cX.x()) - (cY.y() + cZ.z()));
            if (t == 0.0f) _q = Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
            else
            {
               const T s = RCP_SQRT(t) * 0.5f;
               _q = Vector4((cY.z() - cZ.y())*s, t*s, (cY.x()+cX.y())*s, (cZ.x()+cX.z())*s);
            }
         }
         else if ((cY.y()) > (cZ.z()))
         {
            const T t = ((ONE<T>() + cY.y()) - (cX.x() + cZ.z()));
            if (t == 0.0f) _q = Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
            else
            {
               const T s = RCP_SQRT(t) * 0.5f;
               _q = Vector4((cZ.x() - cX.z())*s, (cX.y()+cY.x())*s, t*s, (cZ.y()+cY.z())*s);
            }
         }
         else
         {
            const T t = ((ONE<T>() + cZ.z()) - (cX.x() + cY.y()));
            if (t == 0.0f) _q = Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
            else
            {
               const T s = RCP_SQRT(t) * 0.5f;
               _q = Vector4((cX.y() - cY.x())*s, (cX.z()+cZ.x())*s, (cY.z()+cZ.y())*s, t*s);
            }
         }
      }
#else
      T trace = cX.x() + cY.y() + cZ.z();
      if (trace >= ZERO<T>())
      {
         const T t = ABS(ONE<T>() + trace);
         const T s = RCP_SQRT(t) * 0.5f;
         _q = Vector4(t*s, (cY.z()-cZ.y())*s, (cZ.x() - cX.z())*s, (cX.y()-cY.x())*s);
      }
      else
      {
         //if (ABS(cX.x()) > MAX(ABS(cY.y()), ABS(cZ.z())))
         if (ABS(cX.x()) > MAX(ABS(cY.y()), ABS(cZ.z())))
         {
            const T t = ABS((ONE<T>() + cX.x()) - (cY.y() + cZ.z()));
            if (t == 0.0f) _q = Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
            else
            {
               const T s = RCP_SQRT(t) * 0.5f;
               _q = Vector4((cY.z() - cZ.y())*s, t*s, (cY.x()+cX.y())*s, (cZ.x()+cX.z())*s);
            }
         }
         else if (ABS(cY.y()) > ABS(cZ.z()))
         {
            const T t = ABS((ONE<T>() + cY.y()) - (cX.x() + cZ.z()));
            if (t == 0.0f) _q = Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
            else
            {
               const T s = RCP_SQRT(t) * 0.5f;
               _q = Vector4((cZ.x() - cX.z())*s, (cX.y()+cY.x())*s, t*s, (cZ.y()+cY.z())*s);
            }
         }
         else
         {
            const T t = ABS((ONE<T>() + cZ.z()) - (cX.x() + cY.y()));
            if (t == 0.0f) _q = Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
            else
            {
               const T s = RCP_SQRT(t) * 0.5f;
               _q = Vector4((cX.y() - cY.x())*s, (cX.z()+cZ.x())*s, (cY.z()+cZ.y())*s, t*s);
            }
         }
      }
#endif
   }

   /// Apply the quaternion (interpreted as a 3D rotation) to the vector u
   FINLINE Vector3 operator () (const Vector3& u) const
   {
      /// q u RCP(q) gives the rotation of u by quaterion q. u can be cast to
      /// a quaternion by attaching to it a real component of zero
      return ((*this) * Quaternion(u) * reciprocal()).ijk();
   }


   ////////////////////////////////////
   // Spherical Linear Interpolation //
   ////////////////////////////////////

   INLINE static Quaternion slerp(const Quaternion& qa, const Quaternion& qb, T t)
   {
      if (t <= ZERO<T>()) return qa;
      if (t >= ONE<T>()) return qb;

      T cosOmega = dot(qa, qb);

      Quaternion q = qb;
      if (cosOmega < ZERO<T>())
      {
         q = -q;
         cosOmega = -cosOmega;
      }

      ASSERT(cosOmega < 1.1f);

      /// Compute interpolation fraction, checking for quaternions almost exactly
      /// the same
      T k0, k1;
      if (cosOmega > 0.9999f)
      {
         k0 = ONE<T>() - t;
         k1 = t;
      }
      else
      {
         T sinOmega = SQRT(ONE<T>() - (cosOmega*cosOmega));
         T omega = ATAN2(sinOmega, cosOmega);
         T oneOverSinOmega = RCP(sinOmega);
         k0 = SIN((ONE<T>() - t) * omega) * oneOverSinOmega;
         k1 = SIN(t*omega)*oneOverSinOmega;
      }

      // Interpolate and return new quaternion
      return (k0 * qa) + (k1 * q);
   }



   /////////
   // I/O //
   /////////

   INLINE String toString(int prec = 3) const
   { return String("Quaternion = { ") + _q.toString(3) + "}"; }
};

typedef Quaternion<float>     Quaternionf;
typedef Quaternion<Vector4f>  Quaternion4f;
typedef Quaternion<Vector8f>  Quaternion8f;
typedef Quaternion<Vector16f> Quaternion16f;

}; // namespace Burns


#endif // QUATERNION_H