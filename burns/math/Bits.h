/* ************************************************************************* *\

                  INTEL CORPORATION PROPRIETARY INFORMATION
     This software is supplied under the terms of a license agreement or
     nondisclosure agreement with Intel Corporation and may not be copied
     or disclosed except in accordance with the terms of that agreement.
     Copyright (C) 2011 Intel Corporation. All Rights Reserved.

     Author: Burns, Christopher A.

\* ************************************************************************* */

#ifndef BITS_H
#define BITS_H

#include "../System.h"


namespace Burns
{

/// An early BitMask class, which we will eventually use to replace the Mask class
/// used for the Vector types... (cannot support multidimensional BitMasks though,
/// and this might be a problem for masking things like Vector3xNf...)
template <int N> class BitMask
{
private:
   uint64 _bits;  /// Generalize this, eventually...

public:
   FINLINE BitMask() { STATIC_ASSERT(N <= 64 && N > 0); }
   FINLINE BitMask(const uint64& b) : _bits(b) { STATIC_ASSERT(N <= 64 && N > 0); }

   /// Accessors and Modifiers
   uint64 toInteger() const { return _bits; }
   bool operator [] (Index i) const { return (bool)((_bits >> i) & 0x1); }
   void set(Index i) { _bits |= (0x1 << i); }
   void clear(Index i) { _bits &= (~0x1 << i); }

   ///////////////////////
   // Logical Operators //
   ///////////////////////

   FINLINE BitMask operator ! () const                                    { return BitMask(~_bits); }
   FINLINE BitMask operator ~ () const                                    { return BitMask(~_bits); }

   friend FINLINE BitMask operator & (const BitMask& l, const BitMask& r) { return BitMask(l._bits & r._bits); }
   friend FINLINE BitMask operator | (const BitMask& l, const BitMask& r) { return BitMask(l._bits | r._bits); }
   friend FINLINE BitMask operator ^ (const BitMask& l, const BitMask& r) { return BitMask(l._bits ^ r._bits); }

   FINLINE BitMask& operator &= (const BitMask& r)                        { _bits &= r._bits; return *this; }
   FINLINE BitMask& operator |= (const BitMask& r)                        { _bits |= r._bits; return *this; }
   FINLINE BitMask& operator ^= (const BitMask& r)                        { _bits ^= r._bits; return *this; }

   //////////////////////////
   // Comparison Operators //
   //////////////////////////

   /// For a bit-by-bit test, use the xor (^) operator
   friend FINLINE bool operator == (const BitMask& l, const BitMask& r)      { return l._bits == r._bits; }
   friend FINLINE bool operator != (const BitMask& l, const BitMask& r)      { return l._bits != r._bits; }


   ////////////////
   // Reductions //
   ////////////////

   //T reduceAnd() const;
   //T reduceOr()  const;
   //T all() const;
   //T none() const;
   //T any() const;

};








///////////////////////////////
// Bit Manipulation Routines //
///////////////////////////////


template <class T> struct BITS
{
   /// Bit rotations
   static INLINE T rotr(T a, uint8 b);
   static INLINE T rotl(T a, uint8 b);

   /// Bit interleaves
   static INLINE T interleave11(T a, T b);
   static INLINE T interleave21(T a, T b);

   /// Bit de-interleaving
   static INLINE T deinterleave11(T bits);

   /// Counts '1' bits in an integer
   static INLINE T bitCount(T a);

   /// Returns the place of the least significant bit. Results are undefined
   /// for a == ZERO<T>(). msb returns the most significant bit position.
   //static INLINE T lsb(T a);
   //static INLINE T msb(T a);
};


////////////////////////////////
// Bit Interleave Definitions //
////////////////////////////////

/// Class providing bit interleave functions for integer types. Not a publicly
/// accessible interface, only for BITS
class BitInterleave
{
private:
   template <class T> static T interleave11(T a, T b);
   template <class T> static T interleave21(T a, T B);

   template <class T> static T deinterleave11(T bits);

   friend struct BITS<uint16>;
   friend struct BITS<uint32>;
   friend struct BITS<uint64>;
};

/// De-interleaves the bits in a 32-bit integer, placing the two results
/// in the high and low 16-bit segments of a 32-bit integer
template <> INLINE uint32 BitInterleave::deinterleave11<uint32>(uint32 bits)
{
   uint32 upper = 0, lower = 0;
   for (Index i = 0; i < 16; ++i)
   {
      upper |= (bits & 0x1) << i;
      lower |= ((bits & 0x2) >> 1) << i;
      bits = bits >> 2;
   }
   //printf("   upper = 0x%x, lower = 0x%x\n", upper, lower);
   return (upper << 16) | lower;
}

template <> INLINE uint16 BitInterleave::deinterleave11<uint16>(uint16 bits)
{
   uint32 upper = 0, lower = 0;
   for (Index i = 0; i < 8; ++i)
   {
      upper = (upper << 1) | (bits & 0x1);
      lower = (lower << 1) | (bits & 0x2);
      bits = bits >> 2;
   }
   return uint16((upper << 8) | lower);
}



template <> INLINE uint16 BitInterleave::interleave11<uint16>(uint16 b, uint16 a)
{
   uint16 d = 0;
   for (int i = 0; i < 8; i++) {
      uint16 bit_a = (a >> i) & 1;
      uint16 bit_b = (b >> i) & 1;
      d |= (bit_a << (2*i + 0));
      d |= (bit_b << (2*i + 1));
   }
   return d;
}

template <> INLINE uint32 BitInterleave::interleave11<uint32>(uint32 b, uint32 a)
{
   uint32 d = 0;
   for (int i=0; i<16; i++) {
      uint32 bit_a = (a >> i) & 1;
      uint32 bit_b = (b >> i) & 1;
      d |= (bit_a << (2*i + 0));
      d |= (bit_b << (2*i + 1));
   }
   return d;
}

template <> INLINE uint64 BitInterleave::interleave11<uint64>(uint64 b, uint64 a)
{
   uint64 d = 0;
   for (int i=0; i<32; i++) {
      uint64 bit_a = (a >> i) & 1;
      uint64 bit_b = (b >> i) & 1;
      d |= (bit_a << (2*i + 0));
      d |= (bit_b << (2*i + 1));
   }
   return d;
}

template <> INLINE uint16 BitInterleave::interleave21<uint16>(uint16 a, uint16 b)
{
   uint16 d = 0;
   for (short i=0; i<16/3; i++) {
      uint16 bit_a = (a >> 2*i) & 3;
      uint16 bit_b = (b >> i) & 1;
      d |= (bit_a << (3*i + 1));
      d |= (bit_b << (3*i + 0));
   }
   return d;
}

template <> INLINE uint32 BitInterleave::interleave21<uint32>(uint32 a, uint32 b)
{
   // from the back: 1 bit of b, then 2 bits of a
   uint32 d = 0;
   for (int i=0; i<32/3; i++) {
      uint32 bit_a = (a >> 2*i) & 3;
      uint32 bit_b = (b >> i) & 1;
      d |= (bit_a << (3*i + 1));
      d |= (bit_b << (3*i + 0));
   }
   return d;
}

template <> INLINE uint64 BitInterleave::interleave21<uint64>(uint64 b, uint64 a)
{
   uint64 d = 0;
   for (long i=0; i<64/3; i++) {
      uint64 bit_a = (a >> 2*i) & 3;
      uint64 bit_b = (b >> i) & 1;
      d |= (bit_a << (3*i + 1));
      d |= (bit_b << (3*i + 0));
   }
   return d;
}



/// Specializations
///
/// Many of the signed integer specializations pass the buck to the unsigned
/// versions

template <> struct BITS<uint8>
{
   /// Bit rotations
   static INLINE uint8 rotr(uint8 a, uint8 b)           { return _rotr8(a, b); }
   static INLINE uint8 rotl(uint8 a, uint8 b)           { return _rotl8(a, b); }
};

template <> struct BITS<uint16>
{
   /// Bit rotations
   static INLINE uint16 rotr(uint16 a, uint8 b)           { return _rotr16(a, b); }
   static INLINE uint16 rotl(uint16 a, uint8 b)           { return _rotl16(a, b); }

   /// Bit population count
   static INLINE uint16 bitCount(uint16 b)                { return (uint16)popcnt(b); }

   /// Bit interleaves
   static INLINE uint16 interleave11(uint16 a, uint16 b)
   { return BitInterleave::interleave11<uint16>(a, b); }
   static INLINE uint16 interleave21(uint16 a, uint16 b)
   { return BitInterleave::interleave21<uint16>(a, b); }

   static INLINE uint16 deinterleave11(uint16 bits)
   { return BitInterleave::deinterleave11<uint16>(bits); }
};

template <> struct BITS<uint32>
{
   /// Bit rotations
   static INLINE uint32 rotr(uint32 a, uint8 b)           { return _rotr((int32)a, b); }
   static INLINE uint32 rotl(uint32 a, uint8 b)           { return _rotl((int32)a, b); }

   /// Bit population count
   static INLINE uint32 bitCount(uint32 b)                { return popcnt(b); }

   /// Bit interleaves
   static INLINE uint32 interleave11(uint32 a, uint32 b)
   { return BitInterleave::interleave11<uint32>(a, b); }
   static INLINE uint32 interleave21(uint32 a, uint32 b)
   { return BitInterleave::interleave21<uint32>(a, b); }

   static INLINE uint32 deinterleave11(uint32 bits)
   { return BitInterleave::deinterleave11<uint32>(bits); }
};

template <> struct BITS<uint64>
{
   /// Bit rotations
   static INLINE uint64 rotr(uint64 a, uint8 b)           { return _rotr64((int64)a, b); }
   static INLINE uint64 rotl(uint64 a, uint8 b)           { return _rotl64((int64)a, b); }

   /// Bit population count
   static INLINE int64 bitCount(int64 b)                { return popcnt(b); }

   /// Bit interleaves
   static INLINE uint64 interleave11(uint64 a, uint64 b)
   { return BitInterleave::interleave11<uint64>(a, b); }
   static INLINE uint64 interleave21(uint64 a, uint64 b)
   { return BitInterleave::interleave21<uint64>(a, b); }
};


template <> struct BITS<int8>
{
   /// Bit rotations
   static INLINE int8 rotr(int8 a, uint8 b)           { return _rotr8(a, b); }
   static INLINE int8 rotl(int8 a, uint8 b)           { return _rotl8(a, b); }
};

template <> struct BITS<int16>
{
   /// Bit rotations
   static INLINE int16 rotr(int16 a, uint8 b)           { return _rotr16(a, b); }
   static INLINE int16 rotl(int16 a, uint8 b)           { return _rotl16(a, b); }

   /// Bit population count
   static INLINE int16 bitCount(int16 b)                { return (int16)popcnt(b); }

   /// Bit interleaves
   static INLINE int16 interleave11(int16 a, int16 b)
   { return (int16)BITS<uint16>::interleave11((uint16)a, (uint16)b); }
   static INLINE int16 interleave21(int16 a, int16 b)
   { return (int16)BITS<uint16>::interleave21((uint16)a, (uint16)b); }
};

template <> struct BITS<int32>
{
   /// Bit rotations
   static INLINE int32 rotr(int32 a, uint8 b)           { return _rotr(a, b); }
   static INLINE int32 rotl(int32 a, uint8 b)           { return _rotl(a, b); }

   /// Bit population count
   static INLINE int32 bitCount(int32 b)                { return popcnt(b); }

   /// Bit interleaves
   static INLINE int32 interleave11(int32 a, int32 b)
   { return (int32)BITS<uint32>::interleave11((uint32)a, (uint32)b); }
   static INLINE int32 interleave21(int32 a, int32 b)
   { return (int32)BITS<uint32>::interleave21((uint32)a, (uint32)b); }
};

template <> struct BITS<int64>
{
   /// Bit rotations
   static INLINE int64 rotr(int64 a, uint8 b)           { return _rotr64(a, b); }
   static INLINE int64 rotl(int64 a, uint8 b)           { return _rotl64(a, b); }

   /// Bit population count
   static INLINE int64 bitCount(int64 b)                { return popcnt(b); }

   /// Bit interleaves
   static INLINE int64 interleave11(int64 a, int64 b)
   { return (int64)BITS<uint64>::interleave11((uint64)a, (uint64)b); }
   static INLINE int64 interleave21(int64 a, int64 b)
   { return (int64)BITS<uint64>::interleave21((uint64)a, (uint64)b); }
};




/// Global interface for BITS namespace functions
template <class T> INLINE T ROTR(T a, uint8 b)     { return BITS<T>::rotr(a, b); }
template <class T> INLINE T ROTL(T a, uint8 b)     { return BITS<T>::rotl(a, b); }
template <class T> INLINE T INTERLEAVE11(T a, T b) { return BITS<T>::interleave11(a, b); }
template <class T> INLINE T INTERLEAVE21(T a, T b) { return BITS<T>::interleave21(a, b); }
template <class T> INLINE T DEINTERLEAVE11(T bits) { return BITS<T>::deinterleave11(bits); }
template <class T> INLINE T BITCOUNT(T b)          { return BITS<T>::bitCount(b); }

}; // namespace Burns


#endif // BITS_H
