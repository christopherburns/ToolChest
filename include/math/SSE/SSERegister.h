#ifndef SSEREGISTER_H
#define SSEREGISTER_H

#include "SSE.h"

#include <xmmintrin.h>
#include <pmmintrin.h>
#include <emmintrin.h>
#include <tmmintrin.h>

#ifdef ___SSE4
#include <smmintrin.h>  // SSE4
#endif


namespace Mathematics
{

/// Class to encapsulate and abstract a SIMD 4-wide SSE register. It is
/// essentially untyped and makes no assumptions about the contents of the
/// register. Provides a base type to use inside the typed SIMD SSE wrapper
/// classes.
class SSERegister
{
public:
  //private:

   /// Notes:
   /// Tried a union to have access to __128, __m128i, __m128d all in the same
   /// register, but the compiler was so worried about aliasing that every
   /// operation resulting in a refresh of all registers from the stack.
   /// Using this and a series of accessors which to a cast cast appears
   /// to work best.
   __m128 data;

public:

   /// Define the type which fits in the immediate field of the instruction set
   typedef uint8_t Imm8;

   /// Constructors
   inline SSERegister() {}
   inline SSERegister(const __m128 f)  : data(f) {}
   inline SSERegister(const __m128i i) : data(_mm_castsi128_ps(i)) {}
   inline SSERegister(const __m128d d) : data(_mm_castpd_ps(d)) {}

   /// Construct from data
   inline SSERegister(float f) : data(_mm_set1_ps(f)) {}
   inline SSERegister(int32_t i) : data(_mm_castsi128_ps(_mm_set1_epi32(i))) {}
   inline SSERegister(uint32_t i) : data(_mm_castsi128_ps(_mm_set1_epi32(i))) {}
   inline SSERegister(const uint64_t i0, const uint64_t i1) :
      data(_mm_castsi128_ps(_mm_set_epi32((uint32_t)i0, (uint32_t)(i0 >> 32),
                                          (uint32_t)i1, (uint32_t)(i1 >> 32)))) {}
   inline SSERegister(const float f0, const float f1,
                      const float f2, const float f3)
                      : data(_mm_set_ps(f3, f2, f1, f0)) {}
   inline SSERegister(const uint32_t i0, const uint32_t i1,
                      const uint32_t i2, const uint32_t i3)
                      : data(_mm_castsi128_ps(_mm_set_epi32(i3, i2, i1, i0))) {}
   inline SSERegister(const int32_t i0, const int32_t i1,
                      const int32_t i2, const int32_t i3)
                      : data(_mm_castsi128_ps(_mm_set_epi32(i3, i2, i1, i0))) {}


   /// Pseudo-constructors
   static inline SSERegister zeroes() { return _mm_setzero_ps(); }
   static inline SSERegister ones()
   { return _mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps()); }


   /// Accessors
   inline __m128  m128()  const { return data; }
   inline __m128i m128i() const { return _mm_castps_si128(data); }
   inline __m128d m128d() const { return _mm_castps_pd(data); }

   //////////////////////////////
   // Insertion and Extraction //
   //////////////////////////////

   // Extracts a value from the __m128() type and casts it to float or int32_t
   template <Imm8 i> inline float fpExtract() const;
   template <Imm8 i> inline int32_t i32Extract() const;

   template <Imm8 idx> inline SSERegister fpInsert(float v) const;
   template <Imm8 idx> inline SSERegister i32Insert(int32_t v) const;


   ////////////////////
   // Load and Store //
   ////////////////////

   /// On Nehalem and more recent architectures, an unaligned load on an aligned
   /// address is just as fast as an aligned load on an aligned address
   static inline SSERegister loadu(const SSERegister * p)
   { return _mm_loadu_ps((float*)p); }
   static inline SSERegister load(const SSERegister * p)
   { assert((cast<int>(p) & (0xf)) == 0); return _mm_load_ps((float*)p); }
   static inline SSERegister loadBroadcast(const SSERegister * p)
   { assert((cast<int>(p) & (0xf)) == 0); return _mm_load1_ps((float*)p); }
   inline void store(SSERegister * p) const
   { assert((cast<int>(p) & (0xf)) == 0);  _mm_store_ps((float *)p, m128()); }
   inline void storeNoCache(SSERegister * p) const
   { assert((cast<int>(p) & (0xf)) == 0); _mm_stream_ps((float*)p, m128()); }


   //////////////////
   // Bit Shifting //
   //////////////////

   /// 32-bit shifting

   /// Static shifting operations
   template <Imm8 bits> inline SSERegister shiftLeft32() const
   { return _mm_slli_epi32(m128i(), bits); }
   template <Imm8 bits> inline SSERegister shiftRightL32() const
   { return _mm_srli_epi32(m128i(), bits); }
   template <Imm8 bits> inline SSERegister shiftRightA32() const
   { return _mm_srai_epi32(m128i(), bits); }

   /// Dynamic shifting operations
   inline SSERegister shiftLeft32(const uint8_t bits) const
   { return _mm_slli_epi32(m128i(), bits); }
   inline SSERegister shiftRightL32(const uint8_t bits) const
   { return _mm_srli_epi32(m128i(), bits); }
   inline SSERegister shiftRightA32(const uint8_t bits) const
   { return _mm_srai_epi32(m128i(), bits); }

   /// Static rotate operations
   template <Imm8 bits> inline SSERegister rotateLeft32() const
   { return _mm_or_si128(_mm_slli_epi32(m128i(), bits), _mm_srli_epi32(m128i(), 32-bits)); }
   template <Imm8 bits> inline SSERegister rotateRight32() const
   { return _mm_or_si128(_mm_slli_epi32(m128i(), 32-bits), _mm_srli_epi32(m128i(), bits)); }

   /// Dynamic rotate operations
   inline SSERegister rotateLeft32(const uint8_t bits) const
   { return _mm_or_si128(_mm_slli_epi32(m128i(), bits), _mm_srli_epi32(m128i(), 32-bits)); }
   inline SSERegister rotateRight32(const uint8_t bits) const
   { return _mm_or_si128(_mm_slli_epi32(m128i(), 32-bits), _mm_srli_epi32(m128i(), bits)); }


   /// 64-bit shifting

   /// Static shifting operations
   template <Imm8 bits> inline SSERegister shiftLeft64() const
   { return _mm_slli_epi64(m128i(), bits); }
   template <Imm8 bits> inline SSERegister shiftRightL64() const
   { return _mm_srli_epi64(m128i(), bits); }
   //template <Imm8 bits> inline SSERegister shiftRightA64() const
   //{ return _mm_srai_epi64(m128i(), bits); }

   /// Dynamic shifting operations
   inline SSERegister shiftLeft64(const uint8_t bits) const
   { return _mm_slli_epi64(m128i(), bits); }
   inline SSERegister shiftRightL64(const uint8_t bits) const
   { return _mm_srli_epi64(m128i(), bits); }
   //inline SSERegister shiftRightA64(const uint8_t bits) const
   //{ return _mm_srai_epi64(m128i(), bits); }

   /// Static rotate operations
   template <Imm8 bits> inline SSERegister rotateLeft64() const
   { return _mm_or_si128(_mm_slli_epi64(m128i(), bits), _mm_srli_epi64(m128i(), 64-bits)); }
   template <Imm8 bits> inline SSERegister rotateRight64() const
   { return _mm_or_si128(_mm_slli_epi64(m128i(), 64-bits), _mm_srli_epi64(m128i(), bits)); }

   /// Dynamic rotate operations
   inline SSERegister rotateLeft64(const uint8_t bits) const
   { return _mm_or_si128(_mm_slli_epi64(m128i(), bits), _mm_srli_epi64(m128i(), 64-bits)); }
   inline SSERegister rotateRight64(const uint8_t bits) const
   { return _mm_or_si128(_mm_slli_epi64(m128i(), 64-bits), _mm_srli_epi64(m128i(), bits)); }


	/////////////////////////
	// Shuffling Functions //
	/////////////////////////

	template<Imm8 i0, Imm8 i1> inline SSERegister shuffle64() const;

	template<Imm8 i0, Imm8 i1, Imm8 i2, Imm8 i3> inline SSERegister shuffle32() const
   { return _mm_shuffle_epi32(m128i(), _MM_SHUFFLE(i3, i2, i1, i0)); }

   template <Imm8 i0> SSERegister broadcast32() const;

   // Public static 64-bit two-argument shuffle routine(s)
	template<Imm8 idx0, Imm8 idx1> static inline SSERegister
   shuffle64( const SSERegister i0, const SSERegister i1 );

   // Public static 32-bit two-argument shuffle routine(s)
   template<Imm8 idx0, Imm8 idx1, Imm8 idx2, Imm8 idx3> static
	inline SSERegister shuffle32(const SSERegister i0, const SSERegister i1 )
   { return _mm_shuffle_ps(i0.m128(), i1.m128(), _MM_SHUFFLE(idx3, idx2, idx1, idx0)); }


   /////////////////
   // Comparisons //
   /////////////////

   /// Single-precision floating point
   static inline SSERegister fpEquality(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_cmpeq_ps(lhs.m128(), rhs.m128()); }
   static inline SSERegister fpInequality(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_cmpneq_ps(lhs.m128(), rhs.m128()); }
   static inline SSERegister fpLessThan(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_cmplt_ps(lhs.m128(), rhs.m128()); }
   static inline SSERegister fpGreaterThan(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_cmpgt_ps(lhs.m128(), rhs.m128()); }
   static inline SSERegister fpLessThanOrEqual(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_cmple_ps(lhs.m128(), rhs.m128()); }
   static inline SSERegister fpGreaterThanOrEqual(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_cmpge_ps(lhs.m128(), rhs.m128()); }

   /// 32-bit integer
   static inline SSERegister i32Equality(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_cmpeq_epi32(lhs.m128i(), rhs.m128i()); }
   static inline SSERegister i32Inequality(const SSERegister& lhs, const SSERegister& rhs)
   { return ~i32Equality(lhs, rhs); }
   static inline SSERegister i32LessThan(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_cmplt_epi32(lhs.m128i(), rhs.m128i()); }
   static inline SSERegister i32GreaterThan(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_cmpgt_epi32(lhs.m128i(), rhs.m128i()); }
   static inline SSERegister i32LessThanOrEqual(const SSERegister& lhs, const SSERegister& rhs)
   { return ~i32GreaterThan(lhs, rhs); }
   static inline SSERegister i32GreaterThanOrEqual(const SSERegister& lhs, const SSERegister& rhs)
   { return ~i32LessThan(lhs, rhs); }


   ////////////////
   // Arithmetic //
   ////////////////

   /// Floating point
   inline static SSERegister fpAdd(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_add_ps(lhs.m128(), rhs.m128()); }
   inline static SSERegister fpMul(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_mul_ps(lhs.m128(), rhs.m128()); }
   inline static SSERegister fpSub(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_sub_ps(lhs.m128(), rhs.m128()); }
   inline static SSERegister fpDiv(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_div_ps(lhs.m128(), rhs.m128()); }


   /// 32-bit Integer
   inline static SSERegister i32Add(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_add_epi32(lhs.m128i(), rhs.m128i()); }
   inline static SSERegister i32Sub(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_sub_epi32(lhs.m128i(), rhs.m128i()); }
   static SSERegister i32Mul(const SSERegister& lhs, const SSERegister& rhs); // Definition at end of file
   inline static SSERegister i32Div(const SSERegister& lhs, const SSERegister& rhs)
   {
      SSERegister r;
      for (int i = 0; i < 4; ++i)
         *((int*)&r+i) = *((int*)&lhs+i) / *((int*)&rhs+i);
      return r;
   }



   ////////////////////////
   // Miscellaneous Math //
   ////////////////////////

   /// Spingle-Precision Dot Product! Yay SSE 4.1!
   inline static SSERegister
   fpDotProduct4(const SSERegister& lhs, const SSERegister& rhs)
   {
      #ifdef ___SSE4
      // The mask can be used to generate a dp3, or a dp2, etc. we may later
      // make this a template function to generalize and make use of the mask
      return _mm_dp_ps(lhs.m128(), rhs.m128(), 0xff);
      #else
      return fpMul(lhs, rhs).fpReduceAdd();
      #endif
   }

   /// Floating point reciprocal
   inline SSERegister fpReciprocal() const
   {
      SSERegister x = SSERegister(_mm_rcp_ps(m128()));
      return fpSub(fpAdd(x, x), fpMul((*this), fpMul(x, x)));
   }

   /// Floating point sqrt
   inline SSERegister fpSqrt() const { return _mm_sqrt_ps(m128()); }

   /// Floating point reciprocal sqrt
   inline SSERegister fpRCPSqrt() const
   { return fpSqrt().fpReciprocal(); /*return _mm_rsqrt_ps(m128());*/ }

   /// Min/Max. For each component, returns the min/max of the two arguments,
   inline static SSERegister fpMin(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_min_ps(lhs.m128(), rhs.m128()); }
   inline static SSERegister fpMax(const SSERegister& lhs, const SSERegister& rhs)
   { return _mm_max_ps(lhs.m128(), rhs.m128()); }
   inline static SSERegister i32Min(const SSERegister& lhs, const SSERegister& rhs)
   {
      #ifdef ___SSE4
      return _mm_min_epi32(lhs.m128i(), rhs.m128i());
      #else
      return i32LessThan(lhs, rhs).blend(lhs, rhs);
      #endif
   }
   inline static SSERegister i32Max(const SSERegister& lhs, const SSERegister& rhs)
   {
      #ifdef ___SSE4
      return _mm_max_epi32(lhs.m128i(), rhs.m128i());
      #else
      return i32GreaterThan(lhs, rhs).blend(lhs, rhs);
      #endif
   }

   // Absolute value. Integer assumes a signed integer datatype (of course)
   inline SSERegister i32Abs() const
   { return _mm_abs_epi32(m128i()); }
   inline SSERegister fpAbs() const
   { return _mm_and_ps(m128(), _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff))); }

   // Clamping
   inline SSERegister fpClamp(const SSERegister& lower, const SSERegister& upper) const
   { return fpMax(fpMin(*this, upper), lower); }
   inline SSERegister i32Clamp(const SSERegister& lower, const SSERegister& upper) const
   { return i32Max(i32Min(*this, upper), lower); }

   // Ceiling and Flooring, assumes the value is in 32-bit signed integer range
   inline SSERegister fpCeiling() const
   {
      #ifdef ___SSE4
      return _mm_ceil_ps(m128());
      #else
      /// This seems to be wrong, somehow
      return _mm_sub_ps(_mm_cvtepi32_ps(_mm_add_epi32(_mm_cvtps_epi32(m128()), _mm_set1_epi32(1))), _mm_set1_ps(1.0f));
      #endif
   }

   inline SSERegister fpFloor() const
   {
      #ifdef ___SSE4
	   return _mm_floor_ps(m128());
      #else
      __m128 t = _mm_cvtepi32_ps(_mm_cvttps_epi32(m128()));
	   return _mm_sub_ps(t, _mm_and_ps(_mm_cmplt_ps(m128(), t), _mm_set1_ps(1.0f)));
      #endif
   }



   //////////////////////////////////////
   // Horizonal (Reduction) Operations //
   //////////////////////////////////////

   /// Reductions, fills all four components of the register with the result
   inline SSERegister fpReduceAdd() const;
   inline SSERegister fpReduceMul() const;
   inline SSERegister fpReduceMin() const;
   inline SSERegister fpReduceMax() const;

   /// Reductions, fills all four components of the register with the result
   inline SSERegister i32ReduceAdd() const;
   inline SSERegister i32ReduceMul() const;
   inline SSERegister i32ReduceMin() const;
   inline SSERegister i32ReduceMax() const;


   ////////////////////////
   // Bitwise Operations //
   ////////////////////////

	inline SSERegister operator &( const SSERegister input) const  ///< AND
   { return _mm_and_ps(m128(), input.m128()); }
	inline SSERegister operator |( const SSERegister input) const  ///< OR
   { return _mm_or_ps (m128(), input.m128()); }
	inline SSERegister operator ^( const SSERegister input) const  ///< XOR
   { return _mm_xor_ps(m128(), input.m128()); }
	inline SSERegister andnot( const SSERegister input) const      ///< AND NOT
   { return _mm_andnot_ps(input.m128(), m128()); }

	inline SSERegister operator &=( const SSERegister v ) { return *this = *this & v; }
	inline SSERegister operator |=( const SSERegister v ) { return *this = *this | v; }
	inline SSERegister operator ^=( const SSERegister v ) { return *this = *this ^ v; }

   /// Bitwise negation
	inline SSERegister operator ~( void ) const { return *this ^ ones(); }


   //////////////
   // Blending //
   //////////////

   /// Creates a 4-bit mask from the most significant bits of the four
   /// single-precision, floating-point values. Look to the low four bits.
   inline uint32_t signMask() const { return _mm_movemask_ps(m128()); }


   /// Blend operation, very similar to a selection operation (cond ? a : b).
   /// The "*this" object acts as the mask.
   ///
   /// May add a second blend here, for static blend masks... SSE 4.1 instruction.
   /// This would only be helpful if "blendps" is faster than "blendvps".
   /// According to Agner, "blendps" has latency and throughput of 1, while
   /// "blendvps" has latency and rcp throughput of 2.
   ///
   inline SSERegister blend(const SSERegister iTrue, const SSERegister iFalse) const
   {
      #ifdef ___SSE4
      /// blendv only relies on the sign bit of the mask, and can use a register
      /// quantity as the mask (dynamically computed masks work). Uses the
      /// instruction "blendvps"
      return _mm_blendv_ps(iFalse.m128(), iTrue.m128(), m128());
      #else
      return (iTrue & *this) | iFalse.andnot(*this);
      #endif
   }



private:

   //////////////////////////
	// Unpacking Intrinsics //
	//////////////////////////

	static inline SSERegister unpackloPD( const SSERegister i0, const SSERegister i1 )
   { return _mm_unpacklo_pd   (i0.m128d(), i1.m128d()); }
	static inline SSERegister unpackhiPD( const SSERegister i0, const SSERegister i1 )
   { return _mm_unpackhi_pd   (i0.m128d(), i1.m128d()); }
	static inline SSERegister unpacklo64( const SSERegister i0, const SSERegister i1 )
   { return _mm_unpacklo_epi64(i0.m128i(), i1.m128i()); }
	static inline SSERegister unpackhi64( const SSERegister i0, const SSERegister i1 )
   { return _mm_unpackhi_epi64(i0.m128i(), i1.m128i()); }
	static inline SSERegister unpacklo32( const SSERegister i0, const SSERegister i1 )
   { return _mm_unpacklo_ps   (i0.m128(),    i1.m128() ); }
	static inline SSERegister unpackhi32( const SSERegister i0, const SSERegister i1 )
   { return _mm_unpackhi_ps   (i0.m128(),    i1.m128() ); }
	static inline SSERegister unpacklo16( const SSERegister i0, const SSERegister i1 )
   { return _mm_unpacklo_epi16(i0.m128i(), i1.m128i()); }
	static inline SSERegister unpackhi16( const SSERegister i0, const SSERegister i1 )
   { return _mm_unpackhi_epi16(i0.m128i(), i1.m128i()); }
	static inline SSERegister unpacklo8 ( const SSERegister i0, const SSERegister i1 )
   { return _mm_unpacklo_epi8 (i0.m128i(), i1.m128i()); }
	static inline SSERegister unpackhi8 ( const SSERegister i0, const SSERegister i1 )
   { return _mm_unpackhi_epi8 (i0.m128i(), i1.m128i()); }


	/////////////////////////
	// Movement Intrinsics //
	/////////////////////////

	static inline SSERegister movelo2lo32(const SSERegister value, const SSERegister input)
   { return _mm_move_ss(value.m128(), input.m128()); }
	static inline SSERegister movelo2lo64(const SSERegister value, const SSERegister input)
   { return _mm_move_sd(value.m128d(), input.m128d()); }
	static inline SSERegister movelo2hi64(const SSERegister value, const SSERegister input)
   { return _mm_movelh_ps(value.m128(), input.m128()); }
	static inline SSERegister movehi2lo64(const SSERegister value, const SSERegister input)
   { return _mm_movehl_ps(value.m128(), input.m128()); }

	inline SSERegister moveloup32() const { return _mm_moveldup_ps(m128()); }
	inline SSERegister movehiup32() const { return _mm_movehdup_ps(m128()); }
	inline SSERegister moveloup64() const { return _mm_movedup_pd(m128d()); }

   /// Shuffle32{lo,hi}
	template<Imm8 i0, Imm8 i1> inline SSERegister shufflelo32() const;
	template<Imm8 i0, Imm8 i1> inline SSERegister shufflehi32() const;

   /// Shuffle16{lo,hi}
	template<Imm8 i0, Imm8 i1, Imm8 i2, Imm8 i3>
   inline SSERegister shufflelo16 () const
   { return _mm_shufflelo_epi16(m128i(), _MM_SHUFFLE(i3, i2, i1, i0)); }
	template<Imm8 i0, Imm8 i1, Imm8 i2, Imm8 i3>
   inline SSERegister shufflehi16 () const
   { return _mm_shufflehi_epi16(m128i(), _MM_SHUFFLE(i3, i2, i1, i0)); }


	//////////////////////
	// Expand Functions //
	//////////////////////

	template<Imm8 index> inline SSERegister expand64() const;
	template<Imm8 index> inline SSERegister expand32() const;


   /////////////
   // Friends //
   /////////////

   //friend class SIMD4Float;
   //friend class SIMD4Int;
   //friend class SSEMask;
};


// Expand function specializations
template<> inline SSERegister SSERegister::expand64<0>() const { return moveloup64(); }
template<> inline SSERegister SSERegister::expand64<1>() const { return unpackhi64(*this, *this); }
template<> inline SSERegister SSERegister::expand32<0>() const { return moveloup32().expand64<0>(); } //expand64<0>(moveloup32(input)); }
template<> inline SSERegister SSERegister::expand32<1>() const { return movehiup32().expand64<0>(); }
template<> inline SSERegister SSERegister::expand32<2>() const { return moveloup32().expand64<1>(); }
template<> inline SSERegister SSERegister::expand32<3>() const { return movehiup32().expand64<1>(); }

/// 64-bit two-argument shuffle specializationsconst
template<> inline SSERegister SSERegister::shuffle64<0, 0>(const SSERegister i0, const SSERegister i1) { return unpacklo64(i0, i1); }
template<> inline SSERegister SSERegister::shuffle64<0, 1>(const SSERegister i0, const SSERegister i1) { return movelo2lo64(i1, i0); }
template<> inline SSERegister SSERegister::shuffle64<1, 0>(const SSERegister i0, const SSERegister i1) { return _mm_shuffle_pd(i0.m128d(), i1.m128d(), 1); }
template<> inline SSERegister SSERegister::shuffle64<1, 1>(const SSERegister i0, const SSERegister i1) { return unpackhi64(i0, i1); }

/// 32-bit two-argument shuffle specializations
template<> inline SSERegister SSERegister::shuffle32<0, 1, 0, 1>( const SSERegister i0, const SSERegister i1 ) { return shuffle64<0, 0>(i0, i1); }
template<> inline SSERegister SSERegister::shuffle32<2, 3, 0, 1>( const SSERegister i0, const SSERegister i1 ) { return shuffle64<1, 0>(i0, i1); }
template<> inline SSERegister SSERegister::shuffle32<0, 1, 2, 3>( const SSERegister i0, const SSERegister i1 ) { return shuffle64<0, 1>(i0, i1); }
template<> inline SSERegister SSERegister::shuffle32<2, 3, 2, 3>( const SSERegister i0, const SSERegister i1 ) { return shuffle64<1, 1>(i0, i1); }

/// 64-bit shuffle specializations
template<> inline SSERegister SSERegister::shuffle64<0, 0>() const
{ return expand64<0>(); }
template<> inline SSERegister SSERegister::shuffle64<1, 0>() const
{ return movehi2lo64(movelo2hi64(_mm_setzero_ps(), *this), *this); }
template<> inline SSERegister SSERegister::shuffle64<0, 1>() const
{ return *this; }
template<> inline SSERegister SSERegister::shuffle64<1, 1>() const
{ return expand64<1>(); }


/// Shuffle lo and hi for 16-bits
template<> inline SSERegister SSERegister::shufflelo16<0, 1, 2, 3>() const { return *this; }
template<> inline SSERegister SSERegister::shufflehi16<0, 1, 2, 3>() const { return *this; }

/// Shuffle lo function specializations
template<> inline SSERegister SSERegister::shufflelo32<0, 1>() const { return *this; }
template<> inline SSERegister SSERegister::shufflelo32<0, 0>() const { return shufflelo16<0, 1, 0, 1>(); }
template<> inline SSERegister SSERegister::shufflelo32<1, 0>() const { return shufflelo16<2, 3, 0, 1>(); }
template<> inline SSERegister SSERegister::shufflelo32<1, 1>() const { return shufflelo16<2, 3, 2, 3>(); }

// Shuffle hi function specializations
template<> inline SSERegister SSERegister::shufflehi32<0, 1>() const { return *this; }
template<> inline SSERegister SSERegister::shufflehi32<0, 0>() const { return SSERegister::shufflehi16<0, 1, 0, 1>(); }
template<> inline SSERegister SSERegister::shufflehi32<1, 0>() const { return SSERegister::shufflehi16<2, 3, 0, 1>(); }
template<> inline SSERegister SSERegister::shufflehi32<1, 1>() const { return SSERegister::shufflehi16<2, 3, 2, 3>(); }


/// 32-bit Shuffle operation specializations
/// Optimized specializations for pre-Penryn machines. We're using availability
/// of SSE4.1 as a proxy for detecting a machine post-Penryn, which are faster
/// using the default, un-specialized implementation for arbitrary shuffles
#ifndef ___SSE4
template<> inline SSERegister SSERegister::shuffle32<0, 1, 2, 3>() const { return *this; }
template<> inline SSERegister SSERegister::shuffle32<0, 1, 0, 1>() const { return expand64<0>(); }
template<> inline SSERegister SSERegister::shuffle32<2, 3, 0, 1>() const { return shuffle64<1, 0>(); }
template<> inline SSERegister SSERegister::shuffle32<2, 3, 2, 3>() const { return expand64<1>(); }
template<> inline SSERegister SSERegister::shuffle32<0, 0, 2, 3>() const { return shufflelo32<0, 0>(); }
template<> inline SSERegister SSERegister::shuffle32<1, 0, 2, 3>() const { return shufflelo32<1, 0>(); }
template<> inline SSERegister SSERegister::shuffle32<1, 1, 2, 3>() const { return shufflelo32<1, 1>(); }
template<> inline SSERegister SSERegister::shuffle32<0, 1, 2, 2>() const { return shufflehi32<0, 0>(); }
template<> inline SSERegister SSERegister::shuffle32<0, 1, 3, 2>() const { return shufflehi32<1, 0>(); }
template<> inline SSERegister SSERegister::shuffle32<0, 1, 3, 3>() const { return shufflehi32<1, 1>(); }
template<> inline SSERegister SSERegister::shuffle32<0, 0, 2, 2>() const { return moveloup32(); }
template<> inline SSERegister SSERegister::shuffle32<1, 1, 3, 3>() const { return movehiup32(); }
template<> inline SSERegister SSERegister::shuffle32<1, 1, 2, 2>() const { return shufflelo32<1, 0>().moveloup32(); }
template<> inline SSERegister SSERegister::shuffle32<0, 0, 3, 3>() const { return shufflehi32<1, 0>().moveloup32(); }
template<> inline SSERegister SSERegister::shuffle32<0, 0, 3, 2>() const { return shufflehi32<1, 0>().shufflelo32<0, 0>(); }
template<> inline SSERegister SSERegister::shuffle32<1, 0, 2, 2>() const { return shufflehi32<0, 0>().shufflelo32<1, 0>(); }
template<> inline SSERegister SSERegister::shuffle32<1, 0, 3, 2>() const { return shufflehi32<1, 0>().shufflelo32<1, 0>(); }
template<> inline SSERegister SSERegister::shuffle32<1, 0, 3, 3>() const { return shufflehi32<1, 1>().shufflelo32<1, 0>(); }
template<> inline SSERegister SSERegister::shuffle32<1, 1, 3, 2>() const { return shufflehi32<1, 0>().shufflelo32<1, 1>(); }
template<> inline SSERegister SSERegister::shuffle32<1, 0, 1, 0>() const { return shufflelo32<1, 0>().expand64<0>(); }
template<> inline SSERegister SSERegister::shuffle32<3, 2, 3, 2>() const { return shufflehi32<1, 0>().expand64<1>(); }
template<> inline SSERegister SSERegister::shuffle32<0, 0, 0, 0>() const { return expand32<0>(); }
template<> inline SSERegister SSERegister::shuffle32<1, 1, 1, 1>() const { return expand32<1>(); }
template<> inline SSERegister SSERegister::shuffle32<2, 2, 2, 2>() const { return expand32<2>(); }
template<> inline SSERegister SSERegister::shuffle32<3, 3, 3, 3>() const { return expand32<3>(); }
template<> inline SSERegister SSERegister::shuffle32<2, 2, 0, 0>() const { return shuffle64<1, 0>(moveloup32(), moveloup32()); }
template<> inline SSERegister SSERegister::shuffle32<3, 3, 1, 1>() const { return shuffle64<1, 0>(movehiup32(), movehiup32()); }
template<> inline SSERegister SSERegister::shuffle32<2, 3, 0, 0>() const { return shuffle64<1, 0>(shufflelo32<0, 0>(), shufflelo32<0, 0>()); }
template<> inline SSERegister SSERegister::shuffle32<2, 3, 1, 0>() const { return shuffle64<1, 0>(shufflelo32<1, 0>(), shufflelo32<1, 0>()); }
template<> inline SSERegister SSERegister::shuffle32<2, 3, 1, 1>() const { return shuffle64<1, 0>(shufflelo32<1, 1>(), shufflelo32<1, 1>()); }
template<> inline SSERegister SSERegister::shuffle32<2, 2, 0, 1>() const { return shuffle64<1, 0>(shufflehi32<0, 0>(), shufflehi32<0, 0>()); }
template<> inline SSERegister SSERegister::shuffle32<3, 2, 0, 1>() const { return shuffle64<1, 0>(shufflehi32<1, 0>(), shufflehi32<1, 0>()); }
template<> inline SSERegister SSERegister::shuffle32<3, 3, 0, 1>() const { return shuffle64<1, 0>(shufflehi32<1, 1>(), shufflehi32<1, 1>()); }
#endif // USE_SSE4



/// The following definitions must appear after the preceding specializations of
/// the various shuffle routines, as the gcc compiler complains of using
/// specialized definitions before the specializations have been formally
/// defined in the file, even if the general definition/decl has been given.

template <uint8_t i> inline SSERegister SSERegister::broadcast32() const { return shuffle32<i, i, i, i>(); }


// Static Extractors
template <>          inline float SSERegister::fpExtract<0>() const { return _mm_cvtss_f32(m128()); }
template <uint8_t i> inline float SSERegister::fpExtract   () const { return _mm_cvtss_f32(broadcast32<i>().m128()); }

#ifdef ___SSE4
template <uint8_t i> inline int32_t SSERegister::i32Extract() const { return _mm_extract_epi32(m128i(), i); }
#else
template <>          inline int32_t SSERegister::i32Extract<0>() const { return _mm_cvtsi128_si32(m128i()); }
template <uint8_t i> inline int32_t SSERegister::i32Extract   () const { return _mm_cvtsi128_si32(broadcast32<i>().m128i()); }
#endif


// clang++ on OS X 10.9 says:
//   ./src/utility/math/SSE/SSERegister.h:604:10: error: conversion from '__v4si' to 'Mathematics::SSERegister' is ambiguous
//   { return _mm_insert_epi32(m128i(), v, idx); }
//
// Static insertions
//#ifdef ___SSE4
//template <uint8_t idx> inline SSERegister SSERegister::fpInsert(float v) const
//{ return _mm_insert_ps(m128(), SSERegister(v).m128(), (idx << 4)); }
//template <uint8_t idx> inline SSERegister SSERegister::i32Insert(int32_t v) const
//{ return _mm_insert_epi32(m128i(), v, idx); }
//#else // ___SSE4
template <uint8_t idx> inline SSERegister SSERegister::fpInsert(float v) const
{
   SSERegister r = *this;
   *(((float*)&r.data)+idx) = v;
   return r;
}
template <uint8_t idx> inline SSERegister SSERegister::i32Insert(int32_t v) const
{
   SSERegister r = *this;
   *(((int32_t*)&r.data)+idx) = v;
   return r;
}
//#endif




inline SSERegister SSERegister::i32Mul(const SSERegister& lhs, const SSERegister& rhs)
{
   #ifndef ___SSE4
   return SSERegister::shuffle32<0, 2, 0, 2>(
      _mm_mul_epu32(lhs.m128i(), rhs.m128i()),
      _mm_mul_epu32(lhs.movehiup32().m128i(),
                    rhs.movehiup32().m128i())).shuffle32<0, 2, 1, 3>();
   #else
   return _mm_mullo_epi32(lhs.m128i(), rhs.m128i());
   #endif
}

/// Reductions, fills all four components of the register with the result
inline SSERegister SSERegister::fpReduceAdd() const
{
#if 1
   __m128 t = _mm_hadd_ps(m128(), m128()); return _mm_hadd_ps(t, t);
#else
   return _mm_dp_ps(m128(), _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f), 0xff);
#endif
}
inline SSERegister SSERegister::fpReduceMul() const
{ SSERegister r = fpMul(*this, shuffle32<1,0,3,2>()); return fpMul(r, r.shuffle32<2,3,0,1>()); }
inline SSERegister SSERegister::fpReduceMin() const
{ SSERegister r = fpMin(*this, shuffle32<1,0,3,2>()); return fpMin(r, r.shuffle32<2,3,0,1>()); }
inline SSERegister SSERegister::fpReduceMax() const
{ SSERegister r = fpMax(*this, shuffle32<1,0,3,2>()); return fpMax(r, r.shuffle32<2,3,0,1>()); }

/// Reductions, fills all four components of the register with the result
inline SSERegister SSERegister::i32ReduceAdd() const
{ __m128i t = _mm_hadd_epi32(m128i(), m128i()); return _mm_hadd_epi32(t, t); }
inline SSERegister SSERegister::i32ReduceMul() const
{ SSERegister r = i32Mul(*this, shuffle32<1,0,3,2>()); return i32Mul(r, r.shuffle32<2,3,0,1>()); }
inline SSERegister SSERegister::i32ReduceMin() const
{ SSERegister r = i32Min(*this, shuffle32<1,0,3,2>()); return i32Min(r, r.shuffle32<2,3,0,1>()); }
inline SSERegister SSERegister::i32ReduceMax() const
{ SSERegister r = i32Max(*this, shuffle32<1,0,3,2>()); return i32Max(r, r.shuffle32<2,3,0,1>()); }


}; // namespace Mathematics



#endif // SSEREGISTER_H
