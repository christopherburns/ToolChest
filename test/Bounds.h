#ifndef BOUNDS_H
#define BOUNDS_H

#include <Mathematics.h>
#include <Collections.h>


/// For an AABBox3f, use T = Vector3f,
/// For an AABBox2i, use T = Vector2i, for example
template <class T> class Bounds
{
   typedef typename Mathematics::TypeInfo<T>::Mask Mask;
   
 public:
   T ur, ll; /// we want these 16-byte aligned...
   
 public:
   
   /// Default constructor performs no initialization
   inline Bounds() {}
   inline Bounds(const T& u, const T& l) : ur(u), ll(l) {}
   
   ////////////////////////
   // Boolean operations //
   ////////////////////////
   
   // Operator | gives union, & gives intersection
   friend inline const Bounds operator | (const Bounds& a, const Bounds& b ) { return Bounds(Mathematics::MAX(a.ur, b.ur), Mathematics::MIN(a.ll, b.ll)); }
   friend inline const Bounds operator | (const Bounds& a, const T& b)       { return Bounds(Mathematics::MAX(a.ur, b),    Mathematics::MIN(a.ll, b)); }
	friend inline const Bounds operator & (const Bounds& a, const Bounds& b ) { return Bounds(Mathematics::MIN(a.ur, b.ur), Mathematics::MAX(a.ll, b.ll)); }
   
   inline const Bounds& operator |= (const Bounds& b ) { *this = *this | b; return *this; }
   inline const Bounds& operator |= (const T& b)       { *this = *this | b; return *this; } 
	inline const Bounds& operator &= (const Bounds& b ) { *this = *this & b; return *this; }
   
   inline static Bounds BUnion(const Bounds& a, const Bounds& b)          { return a | b; }
   inline static Bounds BUnion(const Bounds& a, const T& point)           { return a | point; }
   inline static Bounds BIntersection(const Bounds& a, const Bounds& b)   { return a & b; }

   
   
   inline Bounds ClipTo(const Bounds& b) const
   { return BIntersection(*this, b); }
   
   /// Creates a unit-size hypercube, [0, 1]^N where N is the width of T
   inline static Bounds UnitBounds() { return Bounds(Mathematics::ONE<T>(), Mathematics::ZERO<T>()); }
   /// Creates a bounds with -infinity volume
   inline static Bounds InvertedBounds() { return Bounds(-Mathematics::MAXV<T>(), Mathematics::MAXV<T>()); }
   
   /// Returns the numerical center of the bounds
   inline T Centroid() const { return (ur + ll) * T(0.5f); }

   inline bool Inside(const T& point) const
   { return ((point < ur) & (point >= ll)).All(); }
   
   /// Getting the sign right is tricky and depends on the number of dimensions
   /// to the bounds, so we'll punt on that for this function
   inline typename Mathematics::TypeInfo<T>::ElementType UnsignedVolume() const
   { return ABS((ur - ll).ReduceProduct()); }

   /// Enlarges the box by pushing the ur away from the ll so that all 
   /// dimensions are as large as the largest one, making it a minimal hypercube
   /// AABB which encloses the given AABB. There are many ways to compute this,
   /// choosing to push the ur away from the ll is an arbitrary choice
   inline Bounds EnlargeToSquare() const
   {
      T delta = ur - ll;
      typename Mathematics::TypeInfo<T>::ElementInfo maxExtent = delta.ReduceMax();
      return Bounds(ll + T(maxExtent), ll);
   }
   
   inline std::string ToString(int prec = 3) const
   { return std::string("AABBox3 = { ur = ") + ur.ToString(prec) + ", ll = " + ll.ToString(prec) + " }"; }
};


typedef Bounds<Mathematics::Vector3f> AABBox3f;
typedef Bounds<Mathematics::Vector2f> AABBox2f;
typedef Bounds<Mathematics::Vector3i> AABBox3i;
typedef Bounds<Mathematics::Vector2i> AABBox2i;

#endif // BOUNDS_H



