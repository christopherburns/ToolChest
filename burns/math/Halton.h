#ifndef HALTON_H
#define HALTON_H


namespace Burns
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

   uint32 _last1D, _last2D, _last3D, _last4D;

   float radicalInverse(uint32 n, int base, float invBase) const;

public:

   // Constructor
   INLINE Halton() : _last1D(0), _last2D(0), _last3D(0), _last4D(0) { }
   INLINE ~Halton() {}  ///< Destructor

   INLINE void seed2D(uint32 s) { _last2D = s; }
   INLINE void seed1D(uint32 s) { _last1D = s; }
   INLINE void seed3D(uint32 s) { _last3D = s; }
   INLINE void seed4D(uint32 s) { _last4D = s; }

   INLINE void reset2D() { _last2D = 0; }
   INLINE void reset1D() { _last1D = 0; }
   INLINE void reset3D() { _last3D = 0; }
   INLINE void reset4D() { _last4D = 0; }


   INLINE float getNext1D()
   { _last1D++; return radicalInverse(_last1D, 3, 1.0f/3.0f); }

   INLINE Vector2f getNext2D()
   {
      _last2D++;
      return Vector2f(radicalInverse(_last2D, 2, 0.5f),
                      radicalInverse(_last2D, 3, 1.0f/3.0f));
   }

   INLINE Vector3f getNext3D()
   {
      _last3D++;
      return Vector3f(radicalInverse(_last3D, 2, 0.5f),
                      radicalInverse(_last3D, 3, 1.0f/3.0f),
                      radicalInverse(_last3D, 5, 0.2f));
   }

   INLINE Vector4f getNext4D()
   {
      _last4D++;
      return Vector4f(radicalInverse(_last4D, 2, 0.5f),
                      radicalInverse(_last4D, 3, 1.0f/3.0f),
                      radicalInverse(_last4D, 5, 0.2f),
                      radicalInverse(_last4D, 7, 1.0f/7.0f));
   }

   // The first argument is the number of items to generate, the second is the
   // optional seed to use for the sequence
   float * getSequence1D(uint32 n, uint32 s = 0) const;
   Vector2f * getSequence2D(uint32 n, uint32 s = 0) const;
   Vector3f * getSequence3D(uint32 n, uint32 s = 0) const;
};


INLINE float Halton::radicalInverse(uint32 n, int base, float invBase) const
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

INLINE float * Halton::getSequence1D(uint32 n, uint32 s) const
{
   float * seq = new float[n]; assert(seq);
   for (uint32 i = s; i < s+n; ++i) seq[i-s] = radicalInverse(i+1, 2, 0.5f);
   return seq;
}

INLINE Vector2f * Halton::getSequence2D(uint32 n, uint32 s) const
{
   Vector2f * seq = new Vector2f[n]; assert(seq);
   for (uint32 i = s; i < s+n; ++i)
   {
      seq[i-s] = Vector2f(radicalInverse(i+1, 2, 0.5f),
                          radicalInverse(i+1, 3, 1.0f/3.0f));
   }
   return seq;
}

INLINE Vector3f * Halton::getSequence3D(uint32 n, uint32 s) const
{
   Vector3f * seq = new Vector3f[n]; assert(seq);
   for (uint32 i = s; i < s+n; ++i)
   {
      seq[i-s] = Vector3f(
         radicalInverse(i+1, 2, 0.5f),
         radicalInverse(i+1, 3, 1.0f/3.0f),
         radicalInverse(i+1, 5, 0.2f));
   }
   return seq;
}

}; // namespace Burns


#endif
