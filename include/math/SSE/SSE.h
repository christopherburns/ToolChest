#ifndef SSE_H
#define SSE_H

#ifdef ___SSE

#include "../Mathematics.h"
#include "../Vector.h"

namespace Mathematics
{
   template <> class Vector<float, 4>;
   template <> class Vector<int32_t, 4>;
}; // namespace Mathematics

template <typename T, typename U> inline const T& cast(const U& from) { return *(T*)(&from); }
template <typename T, typename U> inline       T& cast(      U& from) { return *(T*)(&from); }

#include "Vector4i.h"
#include "Vector4f.h"

namespace Mathematics
{
   /// Casting operators
   inline Vector<int32_t, 4>::operator Vector<float, 4>() const
   { return Vector<float, 4>(_mm_cvtepi32_ps(_r.m128i())); }

   inline Vector<float, 4>::operator Vector<int32_t, 4>() const
   { return Vector<int32_t, 4>(_mm_cvttps_epi32(_r.m128())); }

   /// Conversion constructors
   template <> inline Vector<int32_t, 4>::Vector(const Vector<float, 4>& v) :
      _r(_mm_cvttps_epi32(v._r.m128())) {}

   template <> inline Vector<float, 4>::Vector(const Vector<int32_t, 4>& v) :
      _r(_mm_cvtepi32_ps(v._r.m128i())) {}
}; // namespace Mathematics


#endif // ___SSE

#endif // SSE_H
