#ifndef AFFINE_FRAME_H
#define AFFINE_FRAME_H

#include "OrthogonalFrame.h"

/// \class AffineFrame represents an affine frame of reference in 3D. It
/// consists of an origin and three linearly independent basis vectors. These
/// vectors do not need to be orthogonal. The class provides methods for
/// transforming points, vectors, and other affine frames (that is, a
/// mechanism for composing affine frames).

namespace Mathematics
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
      inline AffineFrame() { *this = ONE<AffineFrame>(); }
      inline AffineFrame(const Vector3& x, const Vector3& y, const Vector3& z) :
         _vx(x), _vy(y), _vz(z), _origin(ZERO<Vector3>()) {}
      inline AffineFrame(
         const Vector3& x, const Vector3& y, const Vector3& z, const Vector3& origin) :
         _vx(x), _vy(y), _vz(z), _origin(origin) {}
      // Initialize from a column-major array of matrix components
         /// Not sure if this works....
      inline AffineFrame(const float c[16]) :
         _vx(Vector3f(c[0], c[1], c[2])),
         _vy(Vector3f(c[4], c[5], c[6])),
         _vz(Vector3f(c[8], c[9], c[10])),
         _origin(Vector3f(c[12], c[13], c[14]))
      {
      }

      /// Construction from an orthogonal frame of reference. The OrthogonalFrame
      /// class uses a quaternion to represent orientation
      inline AffineFrame(const OrthogonalFrame<T>& oFrame);

      ///////////////
      // Accessors //
      ///////////////

      inline Vector3 x() const { return _vx; }
      inline Vector3 y() const { return _vy; }
      inline Vector3 z() const { return _vz; }
      inline Vector3 Origin() const { return _origin; }

      /// Produces a 4x4 matrix implementing the affine frame as a 3DH transform.
      /// The matrix is produced in COLUMN-MAJOR order!
      inline Vector4x4f Generate4x4() const
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
      inline static AffineFrame Scale(float s)
      { return Scale(s, s, s); }
      inline static AffineFrame Scale(const Vector3& s)
      { return Scale(s.x(), s.y(), s.z()); }
      inline static AffineFrame Scale(T x, T y, T z)
      { return AffineFrame(Vector3(x, ZERO<T>(), ZERO<T>()), Vector3(ZERO<T>(), y, ZERO<T>()), Vector3(ZERO<T>(), ZERO<T>(), z)); }

      inline static AffineFrame Translate(T x, T y, T z)
      { return Translate(Vector3(x, y, z)); }
      inline static AffineFrame Translate(Vector3 v)
      { AffineFrame f; f._origin = v; return f; }

      inline static AffineFrame Rotate(const Vector3& vx, const Vector3& vy, const Vector3& vz)
      { return AffineFrame(vx, vy, vz); }
      inline static AffineFrame Rotate(const T angle, const Vector3& axis)
      { return AffineFrame(OrthogonalFrame<T>(Quaternion<T>(angle, axis), ZERO<Vector3>())); }

      /// Vector transformations occur with the following conventions
      ///     _vx, _vy, _vz are the column vectors of a 3x3 affine matrix
      ///     multiplication occurs with the "column vector on right" convention

      /// These generate affine frames with the requested transformation

      inline Vector3 XFormVector(const Vector3& v) const
      { return v.x() * _vx + v.y() * _vy + v.z() * _vz; }
      inline Vector3 XFormPoint(const Vector3& p) const
      { return _origin + XFormVector(p); }

      inline AffineFrame operator () (const AffineFrame& frame) const
      { return XFormAffineFrame(frame); }
      inline AffineFrame operator * (const AffineFrame& frame) const
      { return XFormAffineFrame(frame); }



   private:
      inline AffineFrame XFormAffineFrame(const AffineFrame& f) const
      {
         return
            AffineFrame(XFormVector(f._vx), XFormVector(f._vy), XFormVector(f._vz),
                        XFormPoint(f._origin));
      }

   public:

      ////////////////
      // Reciprocal //
      ////////////////

      /// The reciprocal is defined such that F * RCP(F) == F. The transform
      /// matrix that results should be the inverse of the original frame's
      /// transform matrix
      inline AffineFrame Reciprocal() const
      {
         // Transpose of the 3x3 rotation matrix
         const Vector3 tX(_vx.x(), _vy.x(), _vz.x());
         const Vector3 tY(_vx.y(), _vy.y(), _vz.y());
         const Vector3 tZ(_vx.z(), _vy.z(), _vz.z());

         const T id = RCP(Vector3::Dot(tX, Cross(tY, tZ)));

         /*Vector<Vector3, 3> newV = Vector<Vector3, 3>(Vector3(id)) *
            Vector<Vector3, 3>(Cross(tY, tZ), Cross(tZ, tX), Cross(tX, tY));

         return AffineFrame(-(newV * _origin).reduceSum(), newV.x(), newV.y(), newV.z());*/

         const Vector3 vx = Cross(tY, tZ) * id;
         const Vector3 vy = Cross(tZ, tX) * id;
         const Vector3 vz = Cross(tX, tY) * id;
         const Vector3 p( -(vx*_origin.x() + vy*_origin.y() + vz*_origin.z()) );

         return AffineFrame(vx, vy, vz, p);
      }


      /////////
      // I/O //
      /////////

      inline std::string ToString(int prec = 3) const
      {
         return std::string("AffineFrame = { ") +
            "vx = " + _vx.ToString(prec) + ", " +
            "vy = " + _vy.ToString(prec) + ", " +
            "vz = " + _vz.ToString(prec) + ", " +
            "origin = " + _origin.ToString(prec) + "}";
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
      static inline AffineFrame<T> one()
      {
         return AffineFrame<T>(
            Vector3(ONE<T>(),  ZERO<T>(), ZERO<T>()),
            Vector3(ZERO<T>(), ONE<T>(),  ZERO<T>()),
            Vector3(ZERO<T>(), ZERO<T>(), ONE<T>()),
            ZERO<Vector3>());
      }

      static inline AffineFrame<T> rcp(const AffineFrame<T>& af) { return af.reciprocal(); }
   };

}; // namespace Mathematics

#endif // AFFINE_FRAME_H
