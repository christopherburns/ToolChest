#ifndef AFFINE_FRAME_H
#define AFFINE_FRAME_H

#include "OrthogonalFrame.h"

/// \class AffineFrame represents an affine frame of reference in 3D. It
/// consists of an origin and three linearly independent basis vectors. These
/// vectors do not need to be orthogonal. The class provides methods for
/// transforming points, vectors, and other affine frames (that is, a
/// mechanism for composing affine frames).

namespace Burns
{

template <class T>
class AffineFrame
{
   typedef Vector<T, 3>                Vector3;
   typedef typename Vector<T, 3>::Mask Mask3;
   typedef typename TypeInfo<T>::Mask  Mask;


private:
   Vector3 _vx, _vy, _vz;
   Vector3 _origin;

public:

   /// We initialize to the canonical cartesian frame of reference
   FINLINE AffineFrame() { *this = ONE<AffineFrame>(); }
   FINLINE AffineFrame(const Vector3& x, const Vector3& y, const Vector3& z) :
      _vx(x), _vy(y), _vz(z), _origin(ZERO<Vector3>()) {}
   FINLINE AffineFrame(
      const Vector3& x, const Vector3& y, const Vector3& z, const Vector3& origin) :
      _vx(x), _vy(y), _vz(z), _origin(origin) {}
   // Initialize from a column-major array of matrix components
      /// Not sure if this works....
   FINLINE AffineFrame(const float c[16]) :
      _vx(Vector3f(c[0], c[1], c[2])),
      _vy(Vector3f(c[4], c[5], c[6])),
      _vz(Vector3f(c[8], c[9], c[10])),
      _origin(Vector3f(c[12], c[13], c[14]))
   {
   }

   /// Construction from an orthogonal frame of reference. The OrthogonalFrame
   /// class uses a quaternion to represent orientation
   FINLINE AffineFrame(const OrthogonalFrame<T>& oFrame);

   ///////////////
   // Accessors //
   ///////////////

   FINLINE Vector3 x() const { return _vx; }
   FINLINE Vector3 y() const { return _vy; }
   FINLINE Vector3 z() const { return _vz; }
   FINLINE Vector3 origin() const { return _origin; }

   /// Produces a 4x4 matrix implementing the affine frame as a 3DH transform.
   /// The matrix is produced in COLUMN-MAJOR order!
   FINLINE Vector4x4f generate4x4() const
   {
      return Vector4x4f(
         Vector4f(_vx.x(), _vx.y(), _vx.z(), 0.0f),               // Column 0
         Vector4f(_vy.x(), _vy.y(), _vy.z(), 0.0f),               // Column 1
         Vector4f(_vz.x(), _vz.y(), _vz.z(), 0.0f),               // Column 2
         Vector4f(_origin.x(), _origin.y(), _origin.z(), 1.0f));  // Column 3
   }


   //////////////////////////
   // Transform Operations //
   //////////////////////////

   /// These generate affine frames with the requested transformation
   FINLINE static AffineFrame Scale(float s)
   { return Scale(s, s, s); }
   FINLINE static AffineFrame Scale(const Vector3& s)
   { return Scale(s.x(), s.y(), s.z()); }
   FINLINE static AffineFrame Scale(T x, T y, T z)
   { return AffineFrame(Vector3(x, ZERO<T>(), ZERO<T>()), Vector3(ZERO<T>(), y, ZERO<T>()), Vector3(ZERO<T>(), ZERO<T>(), z)); }

   FINLINE static AffineFrame Translate(T x, T y, T z)
   { return Translate(Vector3(x, y, z)); }
   FINLINE static AffineFrame Translate(Vector3 v)
   { AffineFrame f; f._origin = v; return f; }

   FINLINE static AffineFrame Rotate(const Vector3& vx, const Vector3& vy, const Vector3& vz)
   { return AffineFrame(vx, vy, vz); }
   FINLINE static AffineFrame Rotate(const T angle, const Vector3& axis)
   { return AffineFrame(OrthogonalFrame<T>(Quaternion<T>(angle, axis), ZERO<Vector3>())); }

   /// Vector transformations occur with the following conventions
   ///     _vx, _vy, _vz are the column vectors of a 3x3 affine matrix
   ///     multiplication occurs with the "column vector on right" convention

   /// These generate affine frames with the requested transformation

   FINLINE Vector3 xFormVector(const Vector3& v) const
   { return v.x() * _vx + v.y() * _vy + v.z() * _vz; }
   FINLINE Vector3 xFormPoint(const Vector3& p) const
   { return _origin + xFormVector(p); }

   FINLINE AffineFrame operator () (const AffineFrame& frame) const
   { return xFormAffineFrame(frame); }
   FINLINE AffineFrame operator * (const AffineFrame& frame) const
   { return xFormAffineFrame(frame); }



private:
   FINLINE AffineFrame xFormAffineFrame(const AffineFrame& f) const
   {
      return
         AffineFrame(xFormVector(f._vx), xFormVector(f._vy), xFormVector(f._vz),
                     xFormPoint(f._origin));
   }

public:

   ////////////////
   // Reciprocal //
   ////////////////

   /// The reciprocal is defined such that F * RCP(F) == F. The transform
   /// matrix that results should be the inverse of the original frame's
   /// transform matrix
   INLINE AffineFrame reciprocal() const
   {
      // Transpose of the 3x3 rotation matrix
      const Vector3 tX(_vx.x(), _vy.x(), _vz.x());
      const Vector3 tY(_vx.y(), _vy.y(), _vz.y());
      const Vector3 tZ(_vx.z(), _vy.z(), _vz.z());

      const T id = RCP(Vector3::dot(tX, cross(tY, tZ)));

      /*Vector<Vector3, 3> newV = Vector<Vector3, 3>(Vector3(id)) *
         Vector<Vector3, 3>(cross(tY, tZ), cross(tZ, tX), cross(tX, tY));

      return AffineFrame(-(newV * _origin).reduceSum(), newV.x(), newV.y(), newV.z());*/

      const Vector3 vx = cross(tY, tZ) * id;
      const Vector3 vy = cross(tZ, tX) * id;
      const Vector3 vz = cross(tX, tY) * id;
      const Vector3 p( -(vx*_origin.x() + vy*_origin.y() + vz*_origin.z()) );

      return AffineFrame(vx, vy, vz, p);
   }


   /////////
   // I/O //
   /////////

   INLINE String toString(int prec = 3) const
   {
      return String("AffineFrame = { ") +
         "vx = " + _vx.toString(prec) + ", " +
         "vy = " + _vy.toString(prec) + ", " +
         "vz = " + _vz.toString(prec) + ", " +
         "origin = " + _origin.toString(prec) + "}";
   }
};

typedef AffineFrame<float> AffineFramef;
typedef AffineFrame<Vector<float, 4> > AffineFrame4f;
typedef AffineFrame<Vector<float, 8> > AffineFrame8f;
typedef AffineFrame<Vector<float, 16> > AffineFrame16f;

#define QUAT frame.rotation.q()
template<class T>
AffineFrame<T>::AffineFrame(const OrthogonalFrame<T>& frame) :
    _vx(     QUAT.r()*QUAT.r() + QUAT.i()*QUAT.i() - QUAT.j()*QUAT.j() - QUAT.k()*QUAT.k(),
       2.0f*(QUAT.i()*QUAT.j() + QUAT.r()*QUAT.k()),
       2.0f*(QUAT.i()*QUAT.k() - QUAT.r()*QUAT.j())),

   _vy(2.0f*(QUAT.i()*QUAT.j() - QUAT.r()*QUAT.k()),
            (QUAT.r()*QUAT.r() - QUAT.i()*QUAT.i() + QUAT.j()*QUAT.j() - QUAT.k()*QUAT.k()),
       2.0f*(QUAT.j()*QUAT.k() + QUAT.r()*QUAT.i())),

   _vz(2.0f*(QUAT.i()*QUAT.k() + QUAT.r()*QUAT.j()),
       2.0f*(QUAT.j()*QUAT.k() - QUAT.r()*QUAT.i()),
            (QUAT.r()*QUAT.r() - QUAT.i()*QUAT.i() - QUAT.j()*QUAT.j() + QUAT.k()*QUAT.k())),

   _origin(frame.origin)
{}
#undef QUAT

///////////////////////////////////////////////////////////////////////////
//                             Mathematics                               //
///////////////////////////////////////////////////////////////////////////

/// See Mathematics.h for complete interface specification
template <class T> struct MATHEMATICS<AffineFrame<T> >
{
   typedef Vector<T, 3>                Vector3;
   typedef typename Vector<T, 3>::Mask Mask3;
   typedef typename TypeInfo<T>::Mask  Mask;

public:

   /// The "Identity" affine frame is the canonical cartesian frame at origin
   static FINLINE AffineFrame<T> one()
   {
      return AffineFrame<T>(
         Vector3(ONE<T>(),  ZERO<T>(), ZERO<T>()),
         Vector3(ZERO<T>(), ONE<T>(),  ZERO<T>()),
         Vector3(ZERO<T>(), ZERO<T>(), ONE<T>()),
         ZERO<Vector3>());
   }

   static FINLINE AffineFrame<T> rcp(const AffineFrame<T>& af) { return af.reciprocal(); }
};

}; // namespace Burns


#endif // AFFINE_FRAME_H








#if 0
#ifndef AFFINE_FRAME_H
#define AFFINE_FRAME_H

#pragma once

#include "OrthogonalFrame.h"

/// \class AffineFrame represents an affine frame of reference in 3D. It
/// consists of an origin and three linearly independent basis vectors. These
/// vectors do not need to be orthogonal. The class provides methods for
/// transforming points, vectors, and other affine frames (that is, a
/// mechanism for composing affine frames).

template <class T>
class AffineFrame
{
   typedef Vector<T, 3>                Vector3;
   typedef typename Vector<T, 3>::Mask Mask3;
   typedef typename TypeInfo<T>::Mask  Mask;


private:
   Vector3 _vx, _vy, _vz;
   Vector3 _origin;

public:

   /// We initialize to the canonical cartesian frame of reference
   FINLINE AffineFrame() { *this = ONE<AffineFrame>(); }
   FINLINE AffineFrame(const Vector3& x, const Vector3& y, const Vector3& z) :
      _vx(x), _vy(y), _vz(z), _origin(ZERO<Vector3>()) {}
   FINLINE AffineFrame(
      const Vector3& x, const Vector3& y, const Vector3& z, const Vector3& origin) :
      _vx(x), _vy(y), _vz(z), _origin(origin) {}
   FINLINE AffineFrame(const float c[12]) :
      _vx(Vector3f(c[0], c[1], c[2])),
      _vy(Vector3f(c[3], c[4], c[5])),
      _vz(Vector3f(c[6], c[7], c[8])),
      _origin(Vector3f(c[9], c[10], c[11]))
   {
      printf("_origin = %s\n", *_origin.toString());
   }

   /// Construction from an orthogonal frame of reference. The OrthogonalFrame
   /// class uses a quaternion to represent orientation
   FINLINE AffineFrame(const OrthogonalFrame<T>& oFrame);

   ///////////////
   // Accessors //
   ///////////////

   FINLINE Vector3 x() const { return _vx; }
   FINLINE Vector3 y() const { return _vy; }
   FINLINE Vector3 z() const { return _vz; }
   FINLINE Vector3 origin() const { return _origin; }


   //////////////////////////
   // Transform Operations //
   //////////////////////////

   FINLINE AffineFrame translate(Vector3f v) const
   {
      AffineFrame f = *this;
      f._origin += v;
      return f;
   }


   /// Vector transformations occur with the following conventions
   ///     _vx, _vy, _vz are the column vectors of a 3x3 affine matrix
   ///     multiplication occurs with the "column vector on right" convention

   FINLINE Vector3 xFormVector(const Vector3& v) const
   { return v.x() * _vx + v.y() * _vy + v.z() * _vz; }
   FINLINE Vector3 xFormPoint(const Vector3& p) const
   { return _origin + xFormVector(p); }
   FINLINE AffineFrame xFormAffineFrame(const AffineFrame& f) const
   {
      return
         AffineFrame(xFormVector(f._vx), xFormVector(f._vy), xFormVector(f._vz),
                     xFormPoint(f._origin));
   }

   FINLINE AffineFrame operator () (const AffineFrame& frame) const
   { return xFormAffineFrame(frame); }

   FINLINE AffineFrame operator * (const AffineFrame& frame) const
   { return xFormAffineFrame(frame); }


   ////////////////
   // Reciprocal //
   ////////////////

   /// The reciprocal is defined such that F * RCP(F) == F
   INLINE AffineFrame reciprocal() const
   {
      // Transpose of the 3x3 rotation matrix
      const Vector3 tX(_vx.x(), _vy.x(), _vz.x());
      const Vector3 tY(_vx.y(), _vy.y(), _vz.y());
      const Vector3 tZ(_vx.z(), _vy.z(), _vz.z());

      const T id = RCP(Vector3::dot(tX, cross(tY, tZ)));

      /*Vector<Vector3, 3> newV = Vector<Vector3, 3>(Vector3(id)) *
         Vector<Vector3, 3>(cross(tY, tZ), cross(tZ, tX), cross(tX, tY));

      return AffineFrame(-(newV * _origin).reduceSum(), newV.x(), newV.y(), newV.z());*/

      const Vector3 vx = cross(tY, tZ) * id;
      const Vector3 vy = cross(tZ, tX) * id;
      const Vector3 vz = cross(tX, tY) * id;
      const Vector3 p( -(vx*_origin.x() + vy*_origin.y() + vz*_origin.z()) );

      return AffineFrame(vx, vy, vz, p);
   }


   /////////
   // I/O //
   /////////

   INLINE String toString(int prec = 3) const
   {
      return String("AffineFrame = { ") +
         "vx = " + _vx.toString(prec) + ", " +
         "vy = " + _vy.toString(prec) + ", " +
         "vz = " + _vz.toString(prec) + ", " +
         "origin = " + _origin.toString(prec) + "}";
   }
};

typedef AffineFrame<float> AffineFramef;
typedef AffineFrame<Vector<float, 4> > AffineFrame4f;
typedef AffineFrame<Vector<float, 8> > AffineFrame8f;
typedef AffineFrame<Vector<float, 16> > AffineFrame16f;


template<class T>
AffineFrame<T>::AffineFrame(const OrthogonalFrame<T>& frame) :
	_vx(frame.rotation.q().r()*frame.rotation.q().r() + frame.rotation.q().i()*frame.rotation.q().i() - frame.rotation.q().j()*frame.rotation.q().j() - frame.rotation.q().k()*frame.rotation.q().k(),
       2.0f*(frame.rotation.q().i()*frame.rotation.q().j() + frame.rotation.q().r()*frame.rotation.q().k()),
       2.0f*(frame.rotation.q().i()*frame.rotation.q().k() - frame.rotation.q().r()*frame.rotation.q().j())),

   _vy(2.0f*(frame.rotation.q().i()*frame.rotation.q().j() - frame.rotation.q().r()*frame.rotation.q().k()),
       (frame.rotation.q().r()*frame.rotation.q().r() - frame.rotation.q().i()*frame.rotation.q().i() + frame.rotation.q().j()*frame.rotation.q().j() - frame.rotation.q().k()*frame.rotation.q().k()),
       2.0f*(frame.rotation.q().j()*frame.rotation.q().k() + frame.rotation.q().r()*frame.rotation.q().i())),

   _vz(2.0f*(frame.rotation.q().i()*frame.rotation.q().k() + frame.rotation.q().r()*frame.rotation.q().j()),
       2.0f*(frame.rotation.q().j()*frame.rotation.q().k() - frame.rotation.q().r()*frame.rotation.q().i()),
       (frame.rotation.q().r()*frame.rotation.q().r() - frame.rotation.q().i()*frame.rotation.q().i() - frame.rotation.q().j()*frame.rotation.q().j() + frame.rotation.q().k()*frame.rotation.q().k())),

   _origin(frame.origin)
{}


///////////////////////////////////////////////////////////////////////////
//                             Mathematics                               //
///////////////////////////////////////////////////////////////////////////

/// See Mathematics.h for complete interface specification
template <class T> struct MATHEMATICS<AffineFrame<T> >
{
   typedef Vector<T, 3>                Vector3;
   typedef typename Vector<T, 3>::Mask Mask3;
   typedef typename TypeInfo<T>::Mask  Mask;

public:

   /// The "Identity" affine frame is the canonical cartesian frame at origin
   static FINLINE AffineFrame<T> one()
   {
      return AffineFrame<T>(
         Vector3(ONE<T>(),  ZERO<T>(), ZERO<T>()),
         Vector3(ZERO<T>(), ONE<T>(),  ZERO<T>()),
         Vector3(ZERO<T>(), ZERO<T>(), ONE<T>()),
         ZERO<Vector3>());
   }

   static FINLINE AffineFrame<T> rcp(const AffineFrame<T>& af) { return af.reciprocal(); }
};



#endif // AFFINE_FRAME_H

#endif // 0