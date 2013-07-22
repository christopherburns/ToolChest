#ifndef ROTATION_H
#define ROTATION_H

#include "Quaternion.h"

namespace ToolChest
{

/// \class Rotation represents an rotation in 3D vector space. It is little
/// more than a wrapper around the Quaternion object with an interface more
/// explicitly geared towards rotations in particular.
///
template <class T> class Rotation
{
private:

   typedef Vector<T, 3> Vector3;
   typedef Quaternion<T> Quaternion;

   Quaternion _q;


public:

   //////////////////
   // Constructors //
   //////////////////

   // Default constructor
   inline Rotation() {}

   explicit inline Rotation(const Quaternion& q) : _q(q) {}

   /// Constructs a quaternion rotation from an angle and an axis
   inline Rotation(float angle, const Vector3& axis) : _q(angle, axis) {}

   /// Constructs a quaternion rotation from a 3x3 column-major rotation matrix
   inline Rotation(const Vector3x3f& rotMatrix) : _q(rotMatrix) {}

   /// Constructs a rotation by setting the zAxis to the given unit vector
   explicit inline Rotation(const Vector3& zAxis) :
      _q(1.0f + zAxis.z(), zAxis.y(), -zAxis.x(), 0.0f)
   {
      if (_q == ZERO<Quaternion>()) _q = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
      else _q = _q.unit();
   }

   /// Returns a spherical linear interpolation between rotation a and
   /// rotation b using t (in [0,1]) as a parameter
   friend inline Rotation Slerp(const Rotation& a, const Rotation& b, float t)
   {
      float w1, w2;

      float cosTheta = Quaternion::dot(a._q, b._q);
      float theta    = ACOS(cosTheta);
      float sinTheta = SIN(theta);

      if ( sinTheta > 0.001f )
      {
         w1 = SIN( (1.0f-t)*theta ) / sinTheta;
         w2 = SIN(t*theta) / sinTheta;
      }
      else // when a ~= b, close enough a linear interp is best
      {
         w1 = 1.0f - t;
         w2 = t;
      }

      return a*w1 + b*w2;
   }

   friend inline Rotation operator * (const Rotation& lhs, const Rotation& rhs)
   { return Rotation(lhs._q * rhs._q); }

   inline void SetAxisAngle(float angle, const Vector3& axis)
   { _q = Quaternion(angle, axis); }

   inline Quaternion q() const { return _q; }

   inline String ToString(int prec = 3) const
   { return String("Rotation = { q = ") + _q.toString(prec) + " }"; }
};


///////////////////////////////////////////////////////////////////////////
//                             Mathematics                               //
///////////////////////////////////////////////////////////////////////////

/// See Mathematics.h for complete interface specification
template <class T> struct MATHEMATICS<Rotation<T> >
{
public:

   static inline Rotation<T> one()
   { return Rotation<T>(ONE<Quaternion<T> >()); }
   static inline Rotation<T> zero()
   { return Rotation<T>(ZERO<Quaternion<T> >()); }
};

typedef Rotation<float> Rotationf;
typedef Rotation<Vector<float, 4> > Rotation4f;
typedef Rotation<Vector<float, 8> > Rotation8f;
typedef Rotation<Vector<float, 16> > Rotation16f;

}; // namespace ToolChest

#endif // ROTATION_H