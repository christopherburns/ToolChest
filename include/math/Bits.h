#ifndef BITS_H
#define BITS_H

namespace Mathematics
{

   /// Generalized static assertion. Any constant boolean expression will work.
   //#ifndef SYSTEM_H
   //template <bool> struct STATIC_ASSERTION_FAILURE;
   //template <> struct STATIC_ASSERTION_FAILURE<true> {};
   //#define STATIC_ASSERT(f) /*sizeof(STATIC_ASSERTION_FAILURE<(bool)(f)>);*/
   //#endif

   /// This should be private somehow
   inline int64_t rotl64(int64_t a, uint8_t b) { return (a << b) | ((uint64_t)a >> (64-b)); }
   inline int64_t rotr64(int64_t a, uint8_t b) { return ((uint64_t)a >> b) | (a << (64-b)); }
   inline int32_t rotl32(int32_t a, uint8_t b) { return (a << b) | ((uint32_t)a >> (32-b)); }
   inline int32_t rotr32(int32_t a, uint8_t b) { return ((uint32_t)a >> b) | (a << (32-b)); }
   inline int16_t rotl16(int16_t a, uint8_t b) { return (a << b) | ((uint16_t)a >> (16-b)); }
   inline int16_t rotr16(int16_t a, uint8_t b) { return ((uint16_t)a >> b) | (a << (16-b)); }
   inline int8_t  rotl8 (int8_t  a, uint8_t b)  { return (a << b) | ((uint8_t)a >> ( 8-b)); }
   inline int8_t  rotr8 (int8_t  a, uint8_t b)  { return ((uint8_t)a >> b) | (a << ( 8-b)); }

   /// Why is this here?
   inline void __cpuid(uint32_t CPUInfo[], const uint32_t InfoType)
   { __asm__ __volatile__("cpuid" : "=a" (CPUInfo[0]), "=b" (CPUInfo[1]), "=c" (CPUInfo[2]), "=d" (CPUInfo[3]) : "a" (InfoType)); }
   inline uint64_t __rdtsc(void)
   { uint64_t retval; __asm__ __volatile__("rdtsc" : "=A"(retval)); return retval; }

   /////////////////////////////
   // Bit Counting Operations //
   /////////////////////////////

   static inline int32_t BitCount( const uint8_t input ) 
   { 
      const uint8_t table[0x100] = 
      {
         0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8 
      }; 
      return int32_t(table[int(input)]);
   }

   static inline int32_t BitCount( const uint16_t input ) { return BitCount(uint8_t (input)) + BitCount(uint8_t (input >>  8)); }
   static inline int32_t BitCount( const uint32_t input ) { return BitCount(uint16_t(input)) + BitCount(uint16_t(input >> 16)); }
   static inline int32_t BitCount( const uint64_t input ) { return BitCount(uint32_t(input)) + BitCount(uint32_t(input >> 32)); }

   static inline int8_t  BitCount( const int8_t  input ) { return (int8_t) BitCount(uint8_t (input)); }
   static inline int32_t BitCount( const int16_t input ) { return (int32_t)BitCount(uint8_t (input)) + BitCount(int8_t (input >>  8)); }
   static inline int32_t BitCount( const int32_t input ) { return (int32_t)BitCount(uint16_t(input)) + BitCount(int16_t(input >> 16)); }
   static inline int32_t BitCount( const int64_t input ) { return (int32_t)BitCount(uint32_t(input)) + BitCount(int32_t(input >> 32)); }



   /// An early BitMask class, which we will eventually use to replace the Mask class
   /// used for the Vector types... (cannot support multidimensional BitMasks though,
   /// and this might be a problem for masking things like Vector3xNf...)
   template <int N> class BitMask
   {
   private:
      uint64_t _bits;  /// Generalize this, eventually...

   public:
      inline BitMask() { /*STATIC_ASSERT(N <= 64 && N > 0);*/ }
      inline BitMask(const uint64_t& b) : _bits(b) { /*STATIC_ASSERT(N <= 64 && N > 0);*/ }

      /// Accessors and Modifiers
      uint64_t ToInteger() const { return _bits; }
      bool operator [] (int i) const { return (bool)((_bits >> i) & 0x1); }
      void Set(int i) { _bits |= (0x1 << i); }
      void Clear(int i) { _bits &= (~0x1 << i); }

      ///////////////////////
      // Logical Operators //
      ///////////////////////

      inline BitMask operator ! () const { return BitMask(~_bits); }
      inline BitMask operator ~ () const { return BitMask(~_bits); }

      friend inline BitMask operator & (const BitMask& l, const BitMask& r) { return BitMask(l._bits & r._bits); }
      friend inline BitMask operator | (const BitMask& l, const BitMask& r) { return BitMask(l._bits | r._bits); }
      friend inline BitMask operator ^ (const BitMask& l, const BitMask& r) { return BitMask(l._bits ^ r._bits); }

      inline BitMask& operator &= (const BitMask& r) { _bits &= r._bits; return *this; }
      inline BitMask& operator |= (const BitMask& r) { _bits |= r._bits; return *this; }
      inline BitMask& operator ^= (const BitMask& r) { _bits ^= r._bits; return *this; }

      //////////////////////////
      // Comparison Operators //
      //////////////////////////

      /// For a bit-by-bit test, use the xor (^) operator
      friend inline bool operator == (const BitMask& l, const BitMask& r) { return l._bits == r._bits; }
      friend inline bool operator != (const BitMask& l, const BitMask& r) { return l._bits != r._bits; }
   };


   ///////////////////////////////
   // Bit Manipulation Routines //
   ///////////////////////////////

   template <class T> struct BITS
   {
      /// Bit rotations
      static inline T Rotr(T a, uint8_t b);
      static inline T Rotl(T a, uint8_t b);

      /// Bit interleaVes
      static inline T Interleave11(T a, T b);
      static inline T Interleave21(T a, T b);

      /// Bit de-interLeaving
      static inline T Deinterleave11(T bits);

      /// Counts '1' bits in an integer
      static inline T BitCount(T a);
   };


   ////////////////////////////////
   // Bit Interleave Definitions //
   ////////////////////////////////

   /// Class providing bit interleave functions for integer types. Not a publicly
   /// accessible interface, only for BITS
   class BitInterleave
   {
   private:
      template <class T> static T Interleave11(T a, T b);
      template <class T> static T Interleave21(T a, T B);
      template <class T> static T Deinterleave11(T bits);

      friend struct BITS<uint16_t>;
      friend struct BITS<uint32_t>;
      friend struct BITS<uint64_t>;
   };

   /// De-interleaves the bits in a 32-bit integer, placing the two results
   /// in the high and low 16-bit segments of a 32-bit integer
   template <> inline uint32_t BitInterleave::Deinterleave11<uint32_t>(uint32_t bits)
   {
      uint32_t upper = 0, lower = 0;
      for (int i = 0; i < 16; ++i)
      {
         upper |= (bits & 0x1) << i;
         lower |= ((bits & 0x2) >> 1) << i;
         bits = bits >> 2;
      }
      //printf("   upper = 0x%x, lower = 0x%x\n", upper, lower);
      return (upper << 16) | lower;
   }

   template <> inline uint16_t BitInterleave::Deinterleave11<uint16_t>(uint16_t bits)
   {
      uint32_t upper = 0, lower = 0;
      for (int i = 0; i < 8; ++i)
      {
         upper = (upper << 1) | (bits & 0x1);
         lower = (lower << 1) | (bits & 0x2);
         bits = bits >> 2;
      }
      return uint16_t((upper << 8) | lower);
   }



   template <> inline uint16_t BitInterleave::Interleave11<uint16_t>(uint16_t b, uint16_t a)
   {
      uint16_t d = 0;
      for (int i = 0; i < 8; i++) 
      {
         uint16_t bit_a = (a >> i) & 1;
         uint16_t bit_b = (b >> i) & 1;
         d |= (bit_a << (2*i + 0));
         d |= (bit_b << (2*i + 1));
      }
      return d;
   }

   template <> inline uint32_t BitInterleave::Interleave11<uint32_t>(uint32_t b, uint32_t a)
   {
      uint32_t d = 0;
      for (int i=0; i<16; i++) 
      {
         uint32_t bit_a = (a >> i) & 1;
         uint32_t bit_b = (b >> i) & 1;
         d |= (bit_a << (2*i + 0));
         d |= (bit_b << (2*i + 1));
      }
      return d;
   }

   template <> inline uint64_t BitInterleave::Interleave11<uint64_t>(uint64_t b, uint64_t a)
   {
      uint64_t d = 0;
      for (int i=0; i<32; i++) 
      {
         uint64_t bit_a = (a >> i) & 1;
         uint64_t bit_b = (b >> i) & 1;
         d |= (bit_a << (2*i + 0));
         d |= (bit_b << (2*i + 1));
      }
      return d;
   }

   template <> inline uint16_t BitInterleave::Interleave21<uint16_t>(uint16_t a, uint16_t b)
   {
      uint16_t d = 0;
      for (short i=0; i<16/3; i++) 
      {
         uint16_t bit_a = (a >> 2*i) & 3;
         uint16_t bit_b = (b >> i) & 1;
         d |= (bit_a << (3*i + 1));
         d |= (bit_b << (3*i + 0));
      }
      return d;
   }

   template <> inline uint32_t BitInterleave::Interleave21<uint32_t>(uint32_t a, uint32_t b)
   {
      // from the back: 1 bit of b, then 2 bits of a
      uint32_t d = 0;
      for (int i=0; i<32/3; i++) 
      {
         uint32_t bit_a = (a >> 2*i) & 3;
         uint32_t bit_b = (b >> i) & 1;
         d |= (bit_a << (3*i + 1));
         d |= (bit_b << (3*i + 0));
      }
      return d;
   }

   template <> inline uint64_t BitInterleave::Interleave21<uint64_t>(uint64_t b, uint64_t a)
   {
      uint64_t d = 0;
      for (long i=0; i<64/3; i++) 
      {
         uint64_t bit_a = (a >> 2*i) & 3;
         uint64_t bit_b = (b >> i) & 1;
         d |= (bit_a << (3*i + 1));
         d |= (bit_b << (3*i + 0));
      }
      return d;
   }



   /// Specializations
   ///
   /// Many of the signed integer specializations pass the buck to the unsigned
   /// versions

   template <> struct BITS<uint8_t>
   {
      /// Bit rotations
      static inline uint8_t Rotr(uint8_t a, uint8_t b) { return rotr8(a, b); }
      static inline uint8_t Rotl(uint8_t a, uint8_t b) { return rotl8(a, b); }
   };

   template <> struct BITS<uint16_t>
   {
      /// Bit rotations
      static inline uint16_t Rotr(uint16_t a, uint8_t b) { return rotr16(a, b); }
      static inline uint16_t Rotl(uint16_t a, uint8_t b) { return rotl16(a, b); }

      /// Bit population count
      static inline uint16_t BitCount(uint16_t b) { return (uint16_t)Mathematics::BitCount(b); }

      /// Bit Interleaves
      static inline uint16_t Interleave11(uint16_t a, uint16_t b)
      { return BitInterleave::Interleave11<uint16_t>(a, b); }
      static inline uint16_t Interleave21(uint16_t a, uint16_t b)
      { return BitInterleave::Interleave21<uint16_t>(a, b); }

      static inline uint16_t Deinterleave11(uint16_t bits)
      { return BitInterleave::Deinterleave11<uint16_t>(bits); }
   };

   template <> struct BITS<uint32_t>
   {
      /// Bit rotations
      static inline uint32_t Rotr(uint32_t a, uint8_t b) { return rotr32((int32_t)a, b); }
      static inline uint32_t Rotl(uint32_t a, uint8_t b) { return rotl32((int32_t)a, b); }

      /// Bit population count
      static inline uint32_t BitCount(uint32_t b) { return Mathematics::BitCount(b); }

      /// Bit Interleaves
      static inline uint32_t Interleave11(uint32_t a, uint32_t b)
      { return BitInterleave::Interleave11<uint32_t>(a, b); }
      static inline uint32_t Interleave21(uint32_t a, uint32_t b)
      { return BitInterleave::Interleave21<uint32_t>(a, b); }

      static inline uint32_t Deinterleave11(uint32_t bits)
      { return BitInterleave::Deinterleave11<uint32_t>(bits); }
   };

   template <> struct BITS<uint64_t>
   {
      /// Bit rotations
      static inline uint64_t Rotr(uint64_t a, uint8_t b) { return rotr64((int64_t)a, b); }
      static inline uint64_t Rotl(uint64_t a, uint8_t b) { return rotl64((int64_t)a, b); }

      /// Bit population count
      static inline int64_t BitCount(int64_t b) { return Mathematics::BitCount(b); }

      /// Bit Interleaves
      static inline uint64_t Interleave11(uint64_t a, uint64_t b)
      { return BitInterleave::Interleave11<uint64_t>(a, b); }
      static inline uint64_t Interleave21(uint64_t a, uint64_t b)
      { return BitInterleave::Interleave21<uint64_t>(a, b); }
   };


   template <> struct BITS<int8_t>
   {
      /// Bit rotations
      static inline int8_t Rotr(int8_t a, uint8_t b) { return rotr8(a, b); }
      static inline int8_t Rotl(int8_t a, uint8_t b) { return rotl8(a, b); }
   };

   template <> struct BITS<int16_t>
   {
      /// Bit rotations
      static inline int16_t Rotr(int16_t a, uint8_t b) { return rotr16(a, b); }
      static inline int16_t Rotl(int16_t a, uint8_t b) { return rotl16(a, b); }

      /// Bit population count
      static inline int16_t BitCount(int16_t b) { return (int16_t)Mathematics::BitCount(b); }

      /// Bit Interleaves
      static inline int16_t Interleave11(int16_t a, int16_t b)
      { return (int16_t)BITS<uint16_t>::Interleave11((uint16_t)a, (uint16_t)b); }
      static inline int16_t Interleave21(int16_t a, int16_t b)
      { return (int16_t)BITS<uint16_t>::Interleave21((uint16_t)a, (uint16_t)b); }
   };

   template <> struct BITS<int32_t>
   {
      /// Bit rotations
      static inline int32_t Rotr(int32_t a, uint8_t b) { return rotr32(a, b); }
      static inline int32_t Rotl(int32_t a, uint8_t b) { return rotl32(a, b); }

      /// Bit population count
      static inline int32_t BitCount(int32_t b) { return Mathematics::BitCount(b); }

      /// Bit Interleaves
      static inline int32_t Interleave11(int32_t a, int32_t b)
      { return (int32_t)BITS<uint32_t>::Interleave11((uint32_t)a, (uint32_t)b); }
      static inline int32_t Interleave21(int32_t a, int32_t b)
      { return (int32_t)BITS<uint32_t>::Interleave21((uint32_t)a, (uint32_t)b); }
   };

   template <> struct BITS<int64_t>
   {
      /// Bit rotations
      static inline int64_t Rotr(int64_t a, uint8_t b) { return rotr64(a, b); }
      static inline int64_t Rotl(int64_t a, uint8_t b) { return rotl64(a, b); }

      /// Bit population count
      static inline int64_t BitCount(int64_t b) { return Mathematics::BitCount(b); }

      /// Bit Interleaves
      static inline int64_t Interleave11(int64_t a, int64_t b)
      { return (int64_t)BITS<uint64_t>::Interleave11((uint64_t)a, (uint64_t)b); }
      static inline int64_t Interleave21(int64_t a, int64_t b)
      { return (int64_t)BITS<uint64_t>::Interleave21((uint64_t)a, (uint64_t)b); }
   };

   /// Global interface for BITS namespace functions
   template <class T> inline T ROTR(T a, uint8_t b)   { return BITS<T>::Rotr(a, b); }
   template <class T> inline T ROTL(T a, uint8_t b)   { return BITS<T>::Rotl(a, b); }
   template <class T> inline T INTERLEAVE11(T a, T b) { return BITS<T>::Interleave11(a, b); }
   template <class T> inline T INTERLEAVE21(T a, T b) { return BITS<T>::Interleave21(a, b); }
   template <class T> inline T DEINTERLEAVE11(T bits) { return BITS<T>::Deinterleave11(bits); }
   template <class T> inline T BITCOUNT(T b)          { return BITS<T>::BitCount(b); }

}; // namespace ToolChest


#endif // BITS_H
