#pragma once

#ifndef MAP_H
#define MAP_H

#include "Traversable.h"

///////////////////////////////////////////////////////////////////////////////
//                               Interface Map                               //
///////////////////////////////////////////////////////////////////////////////

namespace Collections
{

   namespace Common
   {
      template <class A, class B> class KeyValuePair
      {
      public:
         A key; B value;
         inline KeyValuePair() {}
         inline KeyValuePair(const A& k) : key(k) {}
         inline KeyValuePair(const A& k, const B& v) : key(k), value(v) {}

         bool operator < (const A& rhs) const { return key < rhs; }
         bool operator < (const KeyValuePair& rhs) const { return key < rhs.key; }
         bool operator >= (const A& rhs) const { return key >= rhs; }
         bool operator >= (const KeyValuePair& rhs) const { return key >= rhs.key; }
         bool operator == (const KeyValuePair& rhs) const { return key == rhs.key; }
         bool operator != (const KeyValuePair& rhs) const { return key != rhs.key; }
      };

      //template <class A, class B> MakePair(const A& a, const B& b) { return KeyValuePair<A, B>(a, b); }
   }

   template <class K, class V, class C, class CTraits>
   class Map : public Traversable<Common::KeyValuePair<K, V>, C, CTraits>
   {
   public:

      typedef K KeyType;
      typedef V ValueType;

      typedef bool (*KeyPredicate) (const K&);
      typedef bool (*KeyComparator) (const K&, const K&);
      typedef bool (*KeyPredicateByValue) (K);
      typedef bool (*KeyComparatorByValue) (K, K);

      typedef typename Traversable<Common::KeyValuePair<K, V>, C, CTraits>::Predicate Predicate;
      typedef typename Traversable<Common::KeyValuePair<K, V>, C, CTraits>::PredicateByValue PredicateByValue;

      typedef typename CTraits::Iterator Iterator;
      typedef typename CTraits::Builder Builder;

      /// Access
      virtual bool Contains(const K& key) const;
      virtual const V& GetOrElse(const K& key, const V& otherwise) const;

      /// Functional Updates. A new container is returned with the specified mapping
      /// added, removed, or updated as appropriate. 
      virtual C Insert(const K& key, const V& value) const = 0;
      virtual C Remove(const K& key) const = 0;

      /// Returns a set container containing only the keys in the map
      virtual typename CTraits::SetType Keys() const = 0;

      /// Returns the values in the map as a traversable sequence
      template <class T> T Values() const;

      /// P must be of type KeyPredicate, or KeyPredicateByValue
      template <class P> C FilterKeys(P p) const;
   };

   /////////////////////
   // Map Definitions //
   /////////////////////

   template <class K, class V, class C, class CTraits> bool 
   Map<K, V, C, CTraits>::Contains(const K& key) const
   {
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext())
         if (key == iterator.Next().key) return true;
      return false;
   }

   template <class K, class V, class C, class CTraits> const V& 
   Map<K, V, C, CTraits>::GetOrElse(const K& key, const V& otherwise) const
   {
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext())
      {
         Common::KeyValuePair<K, V>& e = iterator.Next();
         if (key == e.key) return e.value;
      }
      return otherwise;
   }

   template <class K, class V, class C, class CTraits> 
   template <class T> T Map<K, V, C, CTraits>::Values() const
   {
      Iterator iterator = this->GetIterator();
      typename T::Builder builder = typename T::Builder(this->Size());
      while (iterator.HasNext())
      {
         auto v = iterator.Next().value;
         //printf("  Adding %.2f to list..\n", v);
         builder.AddElement( /*iterator.Next().value*/ v );
      }
      return builder.Result();
   }

   /// P must be of type KeyPredicate or KeyPredicateByValue
   template <class K, class V, class C, class CTraits> template <class P> 
   C Map<K, V, C, CTraits>::FilterKeys(P p) const
   {
      Iterator iterator = this->GetIterator();
      Builder builder  = Builder(this->Size());
      while (iterator.HasNext())
      {
         Common::KeyValuePair<K, V> e = iterator.Next();
         if (p(e.key)) builder.AddElement(e);
      }
      return builder.Result();
   }
} // namespace Collections




#endif


