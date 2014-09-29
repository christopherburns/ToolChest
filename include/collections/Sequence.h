#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "Traversable.h"

namespace Collections
{
   ///////////////////////////////////////////////////////////////////////////////
   //                           Interface Sequence                              //
   ///////////////////////////////////////////////////////////////////////////////

   template <class E, class C, class CTraits>
   class Sequence : public Traversable<E, C, CTraits>
   {
   public:

      typedef typename Traversable<E, C, CTraits>::Predicate Predicate;
      typedef typename Traversable<E, C, CTraits>::Comparator Comparator;
      typedef typename Traversable<E, C, CTraits>::PredicateByValue PredicateByValue;
      typedef typename Traversable<E, C, CTraits>::ComparatorByValue ComparatorByValue;
      typedef typename CTraits::Iterator Iterator;
      typedef typename CTraits::Builder Builder;

      virtual const E& operator [] (int i) const = 0;

      virtual int IndexOf(const E& element) const;
      template <class P> int IndexWhere(P p) const;

      /// The default implementation of these assumes that the entire
      /// container is copied first, and there is no aliasing.  Immutable
      /// containers which make use of aliasing for efficiency will want
      /// to override these default implementations as needed.
      virtual C Append(const E& element) const;
      virtual C Prepend(const E& element) const;
      virtual C PadTo(int n, const E& element) const;

      virtual C Reverse() const = 0;

      /// Concatenation, Append, Prepend operators
      virtual C operator + (C rhs) const;
      inline friend C operator + (const C& lhs, const E& element)
      { return lhs.Append(element); }
      inline friend C operator + (const E& element, const C& rhs)
      { return rhs.Prepend(element); }

      //virtual C Sorted() const = 0;
      //virtual C SortedWith(Comparator lessThan) const = 0;
      //virtual C SortedWith(ComparatorByValue lessThan) const = 0;

      virtual bool Contains(const E& element) const;
   };



   //////////////////////////
   // Sequence Definitions //
   //////////////////////////

   /// O(n)
   template <class E, class C, class CTraits> int 
   Sequence<E, C, CTraits>::IndexOf(const E& element) const
   {
      int i = 0;
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext())
      {
         if (element == iterator.Next()) return i;
         i++;
      }
      return -1;
   }

   /// O(n)
   template <class E, class C, class CTraits> template <class P> int 
   Sequence<E, C, CTraits>::IndexWhere(P p) const
   {
      int i = 0;
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext())
      {
         if (p(iterator.Next())) return i;
         i++;
      }
      return -1;
   }

   /// O(n)
   template <class E, class C, class CTraits> 
   C Sequence<E, C, CTraits>::Append(const E& element) const
   { 
      int size = this->Size();
      Builder builder = this->clone(size, size + 1);
      builder.AddElement(element);
      return builder.Result();   
   }

   /// O(n)
   template <class E, class C, class CTraits> 
   C Sequence<E, C, CTraits>::Prepend(const E& element) const 
   { 
      Builder builder(this->Size()+1);
      builder.AddElement(element);

      Iterator itr = this->GetIterator();
      while (itr.HasNext()) builder.AddElement(itr.Next());
      return builder.Result();
   }

   /// O(n)
   template <class E, class C, class CTraits> 
   C Sequence<E, C, CTraits>::PadTo(int n, const E& element) const 
   { 
      const int N = max(0, n - this->Size());
      Builder builder = this->clone(this->Size(), N);   
      for (int i = 0; i < N; ++i) 
         builder.AddElement(element);
      return builder.Result();
   }

   /// O(n0+n1)
   template <class E, class C, class CTraits> 
   C Sequence<E, C, CTraits>::operator + (C rhs) const
   {
      Builder builder  = Builder(this->Size() + rhs.Size());
      Iterator itrLeft = this->GetIterator(), itrRight = rhs.GetIterator();
      while (itrLeft.HasNext() ) builder.AddElement(itrLeft.Next() );
      while (itrRight.HasNext()) builder.AddElement(itrRight.Next());
      return builder.Result();
   }

   /// O(n)
   template <class E, class C, class CTraits> bool 
   Sequence<E, C, CTraits>::Contains(const E& element) const
   {
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext())
         if (element == iterator.Next()) return true;
      return false;
   }

   /// Dummy implementation
   //template <class E, class C, class CTraits> 
   //C Sequence<E, C, CTraits>::Sorted() const
   //{
   //   Builder builder = this->clone(this->Size(), this->Size());   
   //   return builder.Result();
   //}
}

#endif