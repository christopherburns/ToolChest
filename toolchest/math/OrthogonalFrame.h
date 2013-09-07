#ifndef ORTHOGONAL_FRAME_H
#define ORTHOGONAL_FRAME_H

#include "Rotation.h"


namespace ToolChest
{

/// \class OrthogonalFrame represents an orthogonal cartesian frame of reference
/// in the form of a quaternion and an origin. This form is useful for storage,
/// interpolation, and composition of frames, but not fast for transforms. To
/// transform a vector or a point, convert it to an AffineFrame first.
///
template <class T>
class OrthogonalFrame
{
   typedef Vector<T, 3>                Vector3;
   typedef Rotation<T>                 Rotation;
   typedef Quaternion<T>               Quaternion;
   typedef typename Vector<T, 3>::Mask Mask3;
   typedef typename TypeInfo<T>::Mask  Mask;


public:

   //Quaternion _q;
   Rotation rotation;
   Vector3 origin;

   Quaternion q()   const { return rotation.q(); }

   //////////////////
   // Constructors //
   //////////////////

   inline OrthogonalFrame() { *this = ONE<OrthogonalFrame>(); }
   inline OrthogonalFrame(const Rotation& r) : origin(ZERO<Vector3>()), rotation(r) {}
   inline OrthogonalFrame(const Rotation& r, const Vector3& o) : origin(o), rotation(r) {}
   inline OrthogonalFrame(const Quaternion& q, const Vector3& o) : origin(o), rotation(q) {}

   static inline OrthogonalFrame OrthogonalFrameLH(const Vector3& lookDir_, const Vector3& origin_, const Vector3& up_);
   static inline OrthogonalFrame OrthogonalFrameRH(const Vector3& lookDir_, const Vector3& origin_, const Vector3& up_);
   OrthogonalFrame(const Vector3& lookDir_, const Vector3& origin_, const Vector3& up_);

   ////////////////
   // String I/O //
   ////////////////

   inline String ToString(int prec = 3) const
   {
      return String("OrthogonalFrame = { ") +
         "rotation = " + rotation.toString(prec) + ", " +
         "origin = "   + origin.toString(prec)   + " }";
   }
};

typedef OrthogonalFrame<float>              OrthogonalFramef;
typedef OrthogonalFrame<Vector<float, 4> >  OrthogonalFrame4f;
typedef OrthogonalFrame<Vector<float, 8> >  OrthogonalFrame8f;
typedef OrthogonalFrame<Vector<float, 16> > OrthogonalFrame16f;


template <class T> inline OrthogonalFrame<T> OrthogonalFrame<T>::OrthogonalFrameLH
   ( const Vector3& lookDir_, const Vector3& origin_, const Vector3& up_) 
{
   // Establish the three normalized vectors for our orthogonal system. We
   // assert the right to adjust the up vector to achieve this condition.
   Vector3 Z = lookDir_.Unit();
   Vector3 yPrime = Vector3::Dot(up_, Z) * Z;
   Vector3 Y = (up_ - yPrime).Unit();
   Vector3 X = cross(Y, Z);

   OrthogonalFrame<T> ortho;
   ortho.origin = origin_;
   ortho.rotation = Rotation(Quaternion(X, Y, Z));
   return ortho;
}

template <class T> inline OrthogonalFrame<T> OrthogonalFrame<T>::OrthogonalFrameRH
   ( const Vector3& lookDir_, const Vector3& origin_, const Vector3& up_) 
{
   // Establish the three normalized vectors for our orthogonal system. We
   // assert the right to adjust the up vector to achieve this condition.
   Vector3 Z = -lookDir_.Unit();
   Vector3 yPrime = Vector3::Dot(up_, Z) * Z;
   Vector3 Y = (up_ - yPrime).Unit();
   Vector3 X = Cross(Y, Z);

   OrthogonalFrame<T> ortho;
   ortho.origin = origin_;
   ortho.rotation = Rotation(Quaternion(X, Y, Z));
   return ortho;
}

template <class T> inline OrthogonalFrame<T>::OrthogonalFrame
   ( const Vector3& lookDir_, const Vector3& origin_, const Vector3& up_) 
{ *this = OrthogonalFrame<T>::OrthogonalFrameRH(lookDir_, origin_, up_); }



///////////////////////////////////////////////////////////////////////////
//                             Mathematics                               //
///////////////////////////////////////////////////////////////////////////

/// See Mathematics.h for complete interface specification
template <class T> struct MATHEMATICS<OrthogonalFrame<T> >
{
public:

   /// The "Identity" affine frame is the canonical cartesian frame at origin
   static inline OrthogonalFrame<T> one()
   { return OrthogonalFrame<T>(ToolChest::ONE<Rotation<T> >(), ToolChest::ZERO<Vector<T, 3> >()); }

   /// Not sure this works properly... rcp(rcp(frame)) does not produce identity...
   /*static inline OrthogonalFrame<T> rcp(const OrthogonalFrame<T>& frame)
   {
      return OrthogonalFrame<T>(
         frame.q().conjugate(),
         Vector<T, 3>((frame.q().conjugate() * Quaternion<T>(frame.origin()) * frame.q()).ijk()  /
                -Quaternion<T>::dot(frame.q())));
   }*/
};

}; // namespace ToolChest


#endif // ORTHOGONAL_FRAME_H


