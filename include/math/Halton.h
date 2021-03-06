#ifndef HALTON_H
#define HALTON_H

namespace Mathematics
{

   ///////////////////////////////
   // Halton Sequence Generator //
   ///////////////////////////////

   /// This object will provide Halton sequence generation functions
   /// for 1, 2, and 3 dimensions. We will use primes 3, 5, and 7 for
   /// each of the three dimensions. All results come out in range [0,1].
   ///
   class Halton
   {
   private:

      uint32_t _last1D, _last2D, _last3D, _last4D;

      float radicalInverse(uint32_t n, int base, float invBase) const;

   public:

      // Constructor
      inline Halton() : _last1D(0), _last2D(0), _last3D(0), _last4D(0) { }
      inline ~Halton() {}  ///< Destructor

      inline void Seed2D(uint32_t s) { _last2D = s; }
      inline void Seed1D(uint32_t s) { _last1D = s; }
      inline void Seed3D(uint32_t s) { _last3D = s; }
      inline void Seed4D(uint32_t s) { _last4D = s; }

      inline void Reset2D() { _last2D = 0; }
      inline void Reset1D() { _last1D = 0; }
      inline void Reset3D() { _last3D = 0; }
      inline void Reset4D() { _last4D = 0; }


      inline float GetNext1D()
      { _last1D++; return radicalInverse(_last1D, 3, 1.0f/3.0f); }

      inline Vector2f GetNext2D()
      {
         _last2D++;
         return Vector2f(radicalInverse(_last2D, 2, 0.5f),
                         radicalInverse(_last2D, 3, 1.0f/3.0f));
      }

      inline Vector3f GetNext3D()
      {
         _last3D++;
         return Vector3f(radicalInverse(_last3D, 2, 0.5f),
                         radicalInverse(_last3D, 3, 1.0f/3.0f),
                         radicalInverse(_last3D, 5, 0.2f));
      }

      inline Vector4f GetNext4D()
      {
         _last4D++;
         return Vector4f(radicalInverse(_last4D, 2, 0.5f),
                         radicalInverse(_last4D, 3, 1.0f/3.0f),
                         radicalInverse(_last4D, 5, 0.2f),
                         radicalInverse(_last4D, 7, 1.0f/7.0f));
      }

      // The first argument is the number of items to generate, the second is the
      // optional seed to use for the sequence
      float    * GetSequence1D(uint32_t n, uint32_t s = 0) const;
      Vector2f * GetSequence2D(uint32_t n, uint32_t s = 0) const;
      Vector3f * GetSequence3D(uint32_t n, uint32_t s = 0) const;
   };


   inline float Halton::radicalInverse(uint32_t n, int base, float invBase) const
   {
      float val = 0.0f, invBi = invBase;
      while (n > 0)
      {
         int d = n % base;
         n /= base; // integer divide
         val += d * invBi;
         invBi *= invBase;
      }
      return val;
   }

   inline float * Halton::GetSequence1D(uint32_t n, uint32_t s) const
   {
      float * seq = new float[n]; assert(seq);
      for (uint32_t i = s; i < s+n; ++i) seq[i-s] = radicalInverse(i+1, 2, 0.5f);
      return seq;
   }

   inline Vector2f * Halton::GetSequence2D(uint32_t n, uint32_t s) const
   {
      Vector2f * seq = new Vector2f[n]; assert(seq);
      for (uint32_t i = s; i < s+n; ++i)
      {
         seq[i-s] = Vector2f(radicalInverse(i+1, 2, 0.5f),
                             radicalInverse(i+1, 3, 1.0f/3.0f));
      }
      return seq;
   }

   inline Vector3f * Halton::GetSequence3D(uint32_t n, uint32_t s) const
   {
      Vector3f * seq = new Vector3f[n]; assert(seq);
      for (uint32_t i = s; i < s+n; ++i)
      {
         seq[i-s] = Vector3f(
            radicalInverse(i+1, 2, 0.5f),
            radicalInverse(i+1, 3, 1.0f/3.0f),
            radicalInverse(i+1, 5, 0.2f));
      }
      return seq;
   }

}; // namespace Mathematics


#endif
