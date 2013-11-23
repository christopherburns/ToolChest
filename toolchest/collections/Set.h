#ifndef SET_H
#define SET_H

#include "Traversable.h"

///////////////////////////////////////////////////////////////////////////////
//                               Interface Set                               //
///////////////////////////////////////////////////////////////////////////////

namespace Collections
{

   template <class E, class C, class CTraits>
   class Set : public Traversable<E, C, CTraits>
   {
   public:

      typedef typename Traversable<E, C, CTraits>::Predicate Predicate;
      typedef typename Traversable<E, C, CTraits>::PredicateByValue PredicateByValue;

      typedef typename CTraits::Iterator Iterator;
      typedef typename CTraits::Builder Builder;

      virtual Iterator Contains(const E& element) const;
      virtual bool IsSubsetOf(const C& set) const;

      virtual C Union(const C& set) const;
      virtual C Intersection(const C& set) const;
      virtual C Difference(const C& set) const;

      virtual C Insert(const E& element) const = 0;
      virtual C Remove(const E& element) const = 0;

      /// Operator versions of the above routines
      inline friend C operator | (const C& lhs, const C& rhs)
      { return lhs.Union(rhs); }
      inline friend C operator & (const C& lhs, const C& rhs)
      { return lhs.Intersection(rhs); }
      inline friend C operator - (const C& lhs, const C& rhs)
      { return lhs.Difference(rhs); }
      inline friend C operator + (const C& lhs, const E& element)
      { return lhs.Insert(element); }
      inline friend C operator + (const E& element, const C& lhs)
      { return lhs.Insert(element); }
      inline friend C operator - (const C& lhs, const E& element)
      { return lhs.Remove(element); }
   };

   /////////////////////
   // Set Definitions //
   /////////////////////

   /// Identical to Sequence::Contains
   template <class E, class C, class CTraits> typename Set<E, C, CTraits>::Iterator 
   Set<E, C, CTraits>::Contains(const E& element) const
   {
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext())
      {
         if (element == iterator.Peek()) return iterator;
         else iterator.Next();
      }
      return iterator;
   }

   /// O(n log n)
   template <class E, class C, class CTraits> bool 
   Set<E, C, CTraits>::IsSubsetOf(const C& set) const
   {
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext())
         if (!set.Contains(iterator.Next())) return false;
      return true;
   }


   /// O(n log n)
   template <class E, class C, class CTraits> C 
   Set<E, C, CTraits>::Union(const C& set) const
   {
      /// Produce a builder pre-loaded with 'this's elements, room for 
      /// up to all of 'set's elements
      Builder builder = this->clone(this->Size(), this->Size() + set.Size());

      /// Add all elements from 'set', Builder's implementation of AddElement
      /// is required to avoid inserting duplicates
      Iterator iterator = set.GetIterator();
      while (iterator.HasNext()) builder.AddElement(iterator.Next());
      return builder.Result();
   }

   /// O(n log n)
   template <class E, class C, class CTraits> C 
   Set<E, C, CTraits>::Intersection(const C& set) const
   {
      Builder builder = Builder(ToolChest::MAX(this->Size(), set.Size()));

      /// Iterate over elements in 'this', insert if also found in set
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext())
      {
         const E& e = iterator.Next();
         if (set.Contains(e)) builder.AddElement(e); /// include if both sets contain e
      }

      return builder.Result();
   }

   /// O(n log n)
   template <class E, class C, class CTraits> C 
   Set<E, C, CTraits>::Difference(const C& set) const
   {
      Builder builder = Builder(this->Size());

      Iterator iterator = this->GetIterator();
      while (iterator.HasNext())
      {
         const E& e = iterator.Next();
         if (!set.Contains(e)) builder.AddElement(e); /// include if not in 'set'
      }

      return builder.Result();
   }

} // namespace Collections




#endif


