#ifndef SSE_H
#define SSE_H

#ifdef ___SSE

#include "../Mathematics.h"
#include "../Vector.h"

namespace ToolChest
{
   template <> class Vector<float, 4>;
   template <> class Vector<int32, 4>;
}; // namespace ToolChest


#include "Vector4i.h"
#include "Vector4f.h"

namespace ToolChest
{
   /// Casting operators
   inline Vector<int32, 4>::operator Vector<float, 4>() const
   { return Vector<float, 4>(_mm_cvtepi32_ps(_r.m128i())); }

   inline Vector<float, 4>::operator Vector<int32, 4>() const
   { return Vector<int32, 4>(_mm_cvttps_epi32(_r.m128())); }

   /// Conversion constructors
   template <> inline Vector<int32, 4>::Vector(const Vector<float, 4>& v) :
      _r(_mm_cvttps_epi32(v._r.m128())) {}

   template <> inline Vector<float, 4>::Vector(const Vector<int32, 4>& v) :
      _r(_mm_cvtepi32_ps(v._r.m128i())) {}
}; // namespace ToolChest

#endif // ___SSE

#endif // SSE_H
