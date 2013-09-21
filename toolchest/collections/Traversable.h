#ifndef TRAVERSABLE_H
#define TRAVERSABLE_H

#include <stdarg.h>
#include "../Reference.h"

/* Design Principles
 - implementations handle reference counting under the hood, so that a data
   structure can be returned from a function without a deep copy
 - Class inheritance here is not intended to provide for virtualization for
   the user, but rather to reduce redundant implementation and ensure
   consistent interfaces across collections. A virtual wrapper may be added later.
 - There will be both mutable and immutable concrete implementations, generally
 - Mutable containers will support a super-set of the operations supported by the 
   complementary Immutable container
*/

/*
 *    Traversable
 *       Sequence
 *          LinkedList   (Mutable, Immutable)
 *          Array        (Mutable, Immutable)
 *       Set
 *          HashSet
 *          TreeSet (Sorted, Mutable, Immutable)
 *       Map
 *          HashMap
 *          TreeMap (SortedMap)
 */

/* Temporary ... until we add our math library */
/*namespace Math
{
   template <class T> inline T MAX(T a, T b) { return (a > b ? a : b); }
   template <class T> inline T MIN(T a, T b) { return (a < b ? a : b); }
}*/

//using namespace Math;


namespace Collections
{
   /// The following reference class is for internal use by collections classes,
   /// not for users of the library. It differs in that it can reference primitive
   /// types, but is slightly more dangerous in use. The reference count is stored
   /// in the reference, not the referenced object, which opens up the danger of
   /// two Ref<int>() objects pointing to the same int, with separate counters.

   template <class A, class B> class Pair
   {
   public:
      A first; B second;
      inline Pair(const A& a, const B& b) : first(a), second(b) {}
   };

   namespace Common
   {      
      template <class E> class InitializedBuffer : public ToolChest::Object
      {
      private:
         E * _pool;
         int _capacity;

      public:
         inline int Capacity() const { return _capacity; }

         /// Constructor with initial size
         inline InitializedBuffer(int initialCapacity = 0)
            : _pool(nullptr), _capacity(initialCapacity)
         {
            if (_capacity > 0) _pool = new E[_capacity];
         }

         inline ~InitializedBuffer()
         { if (_pool) { delete [] _pool; _pool = nullptr; } }

         inline const E& operator [] (int i) const { return Index(i); }
         inline E& operator [] (int i) { return Index(i); }

         inline const E& Index (int i) const { assert(i < _capacity); return _pool[i]; }
         inline E& Index (int i) { assert(i < _capacity); return _pool[i]; } 

         inline operator E* const&() const { return _pool; }
         inline operator E*      &()       { return _pool; }     
      };


      template <class E> class MemoryPool : public ToolChest::Object
      {
      private:
         static const int MIN_CAPACITY = 0x10;

         // _pool is an UNINITIALIZED buffer, cannot use assignment for new
         // entries...
         E * _pool;
         int _capacity, _nextFreeIndex;

         /// Private constructor that does no allocations, used internally only
         inline MemoryPool() : _pool(nullptr), _capacity(0), _nextFreeIndex(-1) {}

         inline bool isFull() const { return _capacity == _nextFreeIndex; }

         void resize(int newCapacity)
         {
            if (newCapacity <= Capacity()) return;

            E * newPool = (E*)malloc(newCapacity * sizeof(E));
            for (int i = 0; i < _nextFreeIndex; ++i)
            {
               new (newPool+i) E(_pool[i]);  //< Copy
               _pool[i].~E();                //< Destroy
            }

            if (_pool) free(_pool);
            _pool = newPool;
            _capacity = newCapacity;
         }

         inline void expandIfFull()
         {
            if (isFull())
            {
               if (Capacity() == 0) resize(MIN_CAPACITY);
               else                 resize(2*Capacity());
            }
         }

      public:
         inline int Capacity() const { return _capacity; }
         inline int NextFreeIndex() const { return _nextFreeIndex; }

         ToolChest::Ref<MemoryPool> Clone() const
         {
            ToolChest::Ref<MemoryPool> p = new MemoryPool();
            p->_capacity = _capacity;
            p->_nextFreeIndex = _nextFreeIndex;
            p->_pool = (E*)malloc(_capacity * sizeof(E));
            for (int i = 0; i < _nextFreeIndex; ++i)
               new (p->_pool+i) E(_pool[i]);  //< Copy Each Object
            return p;
         }

         /// Constructor with initial size
         MemoryPool(int initialCapacity)
            : _pool(nullptr), _capacity(0), _nextFreeIndex(0)
         {
            resize(MIN_CAPACITY > initialCapacity ? MIN_CAPACITY : initialCapacity);
            assert(_capacity >= initialCapacity);
         }

         /// Adds a new element to the pool, returns the index for the newly
         /// added element
         int Push(const E& e)
         {
            expandIfFull();
            new (_pool + _nextFreeIndex) E(e);
            _nextFreeIndex++;
            return _nextFreeIndex-1;
         }

         void Pop()
         {
            assert(_nextFreeIndex > 0);
            (_pool + _nextFreeIndex-1)->~E();
            _nextFreeIndex--;
         }

         inline const E& operator [] (int i) const { return Index(i); }
         inline E& operator [] (int i) { return Index(i); }

         inline const E& Index (int i) const { /*assert(i < _nextFreeIndex);*/ return _pool[i]; }
         inline E& Index (int i) { /*assert(i < _nextFreeIndex);*/ return _pool[i]; }
      };

   } // namespace Common
} // namespace Collections






namespace Collections
{
   class EmptyCollectionException {};
   class NoElementFoundException {};


   /* What's the "CTraits" parameter for?

   Every container has a compantion class containing typedefs that the container's
   parent classes in the inheritance chain need to access. These typedefs cannot
   be accessed directly from the container by the container's parent classes because
   the container's body has not been defined at the point where the parent template
   classes are instantiated.  So a typical container looks like this:

       template <class E> Container;   // forward declaration
       template <class E> ContainerTraits
       { ... typedefs here ... };
       template <class E> Container 
          : public ParentContainer<E, Container<E>, ContainerTraits<E> >
       { ... };

   Note how at the point of instantiation for ParentContainer, no typedefs defined
   in Container are visible yet, hence the need for ContainerTraits to "bootstrap"
   the whole thing.  =/

   */


   // E is the element type, C is the concrete derived class, e.g. Array<int>
   template <class E, class C, class CTraits>
   class Traversable : public ToolChest::Object
   {
   public:

      typedef E KeyType;
      typedef E ValueType;

      typedef bool (*Predicate) (const E&);
      typedef bool (*Comparator) (const E&, const E&);
      typedef bool (*PredicateByValue) (E);
      typedef bool (*ComparatorByValue) (E, E);
      
      typedef typename CTraits::Iterator Iterator;
      typedef typename CTraits::Builder Builder;

      // Traversable functionality for each collection is defined
      // mainly by the implementation of the Iterator class
      virtual typename CTraits::Iterator GetIterator() const = 0;

      // not virtual because they are templates! 
      template <class F> void ForEach(F& vf) const;  // function object

      virtual int Size() const = 0;
      inline bool IsEmpty() const { return Size() == 0; }
      inline bool NonEmpty() const { return !IsEmpty(); }

      virtual E Head() const;
      virtual E Last() const;
      
      template <class P> E Find(P p) const; // what to do if no value is found?

      // these must be implmented by the concrete classes as they depend on
      // whether the class is immutable or not
      virtual C Tail() const;       /// Everything but the first element
      virtual C Init() const;       /// Everything but the last element
      virtual C Take(int n) const;  /// Take the first n elements
      virtual C Drop(int n) const;  /// Drop the first n elements
        
      /// Fuck, these template functions cannot be virtual, thus efficient
      /// overrides are not possible unless we can ditch the template parameter
      /// somehow
      template <class P> inline C TakeWhile(P p) const { return Take(CountWhile(p)); }
      template <class P> inline C DropWhile(P p) const { return Drop(CountWhile(p)); }

      template <class P> C Filter(P p) const;
      template <class P> C FilterNot(P p) const;

      inline Pair<C, C> SplitAt(int n) const { return Pair<C, C>(Take(n), Drop(n)); }
      template <class P> inline Pair<C, C> Span(P p) const { return SplitAt(CountWhile(p)); }

      virtual Pair<C, C> Partition(Predicate p) const { return partition(p); }
      virtual Pair<C, C> Partition(PredicateByValue p) const { return partition(p); }

      template <class P> inline bool ForAll     (P p) const;
      template <class P> inline bool Exists     (P p) const;
      template <class P> inline int  Count      (P p) const;
      template <class P> inline int  CountWhile (P p) const;
      

      ///////////////
      // Factories // 
      ///////////////

      static C Construct(int N, const E * values)
      {
         Builder builder(N);
         for (int i = 0; i < N; ++i) builder.AddElement(values[i]);
         return builder.Result();
      }

      static C Construct(int N, E * values)
      { return Construct(N, (const E*)values); }

      template <class G> static C Construct(int N, const G& g)
      { return generator(N, g); }
      template <class G> static C Construct(int N, G& g)
      { return generator(N, g); }

      virtual C Copy() const 
      {
         Builder b = clone(Size(), Size());
         return b.Result();
      }

   private:   
      template <class P> Pair<C, C> partition(P p) const;

      template <class G> static C generator(int N, G g)
      {
         Builder builder(N);
         for (int i = 0; i < N; ++i) builder.AddElement(g(i));
         return builder.Result();
      }


   protected:
      /// Returns an incomplete builder with all of the existing
      /// nodes already copied in (ready to append, pad, or complete)
      virtual Builder clone(int numElementsToCopy, int reservedSpace) const
      {
         /// We will not copy more elements than exist, nor reserve
         /// less space than we are about to copy
         const int N = ToolChest::MIN(numElementsToCopy, Size());
         const int R = ToolChest::MAX(reservedSpace, N);
         Builder builder(R);
         Iterator itr = GetIterator();
         for (int i = 0; i < N; ++i) 
         { assert(itr.HasNext()); builder.AddElement(itr.Next()); }
         return builder;
      }
   };


   /////////////////////////////
   // Traversable Definitions //
   /////////////////////////////

   template <class E, class C, class CTraits> template <class F>
   void Traversable<E, C, CTraits>::ForEach(F& vf) const
   {
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext()) vf(iterator.Next());
   }

   template <class E, class C, class CTraits> E Traversable<E, C, CTraits>::Head() const
   {
      assert(Size() > 0);
      return this->GetIterator().Next();
   }

   // most subclasses should override this one for efficiency reasons
   template <class E, class C, class CTraits> E Traversable<E, C, CTraits>::Last() const
   {
      assert(Size() > 0);
      Iterator predecessor = this->GetIterator(), iterator = this->GetIterator();
      while (iterator.HasNext()) { predecessor = iterator; iterator.Next(); }
      
      assert(predecessor.HasNext());
      return predecessor.Next();
   }

   template <class E, class C, class CTraits> template <class P> 
   E Traversable<E, C, CTraits>::Find(P p) const
   {
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext())
      {
         E e = iterator.Next();
         if (p(e)) return e;
      }

      throw NoElementFoundException();
   }


   template <class E, class C, class CTraits> 
   C Traversable<E, C, CTraits>::Tail() const                              
   { return Drop(1); } 

   template <class E, class C, class CTraits> 
   C Traversable<E, C, CTraits>::Init() const                              
   { return Take(ToolChest::MAX(0, Size()-1)); }                                                                               

   template <class E, class C, class CTraits> 
   C Traversable<E, C, CTraits>::Take(int n) const                         
   { return clone(n, n).Result(); } 

   template <class E, class C, class CTraits> 
   C Traversable<E, C, CTraits>::Drop(int n) const                         
   {              
      Builder builder(Size()-n);
      Iterator itr = GetIterator(); 

      /// Skip n items (or until the end, whichever comes first)
      for (int i = 0; i < n && itr.HasNext(); i++) itr.Next(); 

      /// Accumulate the remaining
      while (itr.HasNext()) { builder.AddElement(itr.Next()); }
      return builder.Result();
   }     

   template <class E, class C, class CTraits> template <class P> 
   C Traversable<E, C, CTraits>::Filter(P p) const
   {
      Iterator iterator = this->GetIterator();
      Builder builder  = Builder(Size());
      while (iterator.HasNext())
      {
         E e = iterator.Next();
         if (p(e)) builder.AddElement(e);
      }
      return builder.Result();
   }

   template <class E, class C, class CTraits> template <class P> 
   C Traversable<E, C, CTraits>::FilterNot(P p) const
   {
      Iterator iterator = this->GetIterator();
      Builder builder  = Builder(Size());
      while (iterator.HasNext())
      {
         E e = iterator.Next();
         if (!p(e)) builder.AddElement(e);
      }
      return builder.Result();
   }

   template <class E, class C, class CTraits> template <class P> 
   bool Traversable<E, C, CTraits>::ForAll(P p) const
   {
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext()) if (!p(iterator.Next())) return false;
      return true;
   }
   template <class E, class C, class CTraits> template <class P> 
   bool Traversable<E, C, CTraits>::Exists(P p) const
   {
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext()) if (p(iterator.Next())) return true;
      return false;
   }
   template <class E, class C, class CTraits> template <class P> 
   int Traversable<E, C, CTraits>::Count (P p) const
   {
      int c = 0;
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext()) c += (p(iterator.Next()) ? 1 : 0);
      return c;
   }

   template <class E, class C, class CTraits> template <class P> 
   int Traversable<E, C, CTraits>::CountWhile (P p) const
   {
      int c = 0;
      Iterator iterator = this->GetIterator();
      while (iterator.HasNext() && p(iterator.Next())) c++;
      return c;
   }

   template <class E, class C, class CTraits> template <class P> 
   Pair<C, C> Traversable<E, C, CTraits>::partition(P p) const
   {
      Iterator iterator = this->GetIterator();
      Builder builderTrue  = Builder(Size()/2);
      Builder builderFalse = Builder(Size()/2);
      while (iterator.HasNext())
      {
         E e = iterator.Next();
         if (p(e)) builderTrue.AddElement(e);
         else      builderFalse.AddElement(e);
      }
      return Pair<C, C>(builderTrue.Result(), builderFalse.Result());
   }


   //////////////////////////////////////
   // Non-Member Traversable Operators //
   //////////////////////////////////////

   template <class Container, class U, class TtoU> 
   typename Container::template SwapElementType<U>::C
   MapValues(const Container& c, const TtoU& vf)
   {
      typedef typename Container::template SwapElementType<U>::C Target;

      typename Container::Iterator iterator = c.GetIterator();
      typename Target::Builder builder = typename Target::Builder(c.Size());
      while (iterator.HasNext()) 
         builder.AddElement(vf(iterator.Next()));
      return builder.Result();
   }



} // namespace Collections

#endif
