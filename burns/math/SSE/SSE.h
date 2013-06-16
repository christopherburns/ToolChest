/* ************************************************************************* *\

                  INTEL CORPORATION PROPRIETARY INFORMATION
     This software is supplied under the terms of a license agreement or
     nondisclosure agreement with Intel Corporation and may not be copied
     or disclosed except in accordance with the terms of that agreement.
     Copyright (C) 2011 Intel Corporation. All Rights Reserved.

     Author: Burns, Christopher A.

\* ************************************************************************* */

#ifndef SSE_H
#define SSE_H

#pragma once

#ifdef ___SSE

#include "../Mathematics.h"
#include "../Vector.h"

namespace Burns
{

template <> class Vector<float, 4>;
template <> class Vector<int32, 4>;

}; // namespace Burns


#include "Vector4i.h"
#include "Vector4f.h"

namespace Burns
{

/// Casting operators
FINLINE Vector<int32, 4>::operator Vector<float, 4>() const
{ return Vector<float, 4>(_mm_cvtepi32_ps(_r.m128i())); }

FINLINE Vector<float, 4>::operator Vector<int32, 4>() const
{ return Vector<int32, 4>(_mm_cvttps_epi32(_r.m128())); }

/// Conversion constructors
template <> FINLINE Vector<int32, 4>::Vector(const Vector<float, 4>& v) :
   _r(_mm_cvttps_epi32(v._r.m128())) {}

template <> FINLINE Vector<float, 4>::Vector(const Vector<int32, 4>& v) :
   _r(_mm_cvtepi32_ps(v._r.m128i())) {}

}; // namespace Burns

#endif // ___SSE

#endif // SSE_H