#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "Sequence.h"
#include "LinkedListCommon.h"

/////////////////////////////////////////////////
// Class LinkedList <- Sequence <- Traversable //
/////////////////////////////////////////////////

// Problem: Mutable::Array<E> uses reference semantics for copy constructor
// but the _size member in the array is copied. Bug happens here:
//    list0 = MakeList();
//    list1 = list0 + 1;
//    list2 = list0 + 10; // <-- error, we just over-wrote 1 in list1

namespace Collections
{
   namespace Immutable
   {
      using ToolChest::Ref;

      template <class E> class LinkedList;

      template <class E> struct LinkedListTraits
      {
         typedef Common::LinkedListIterator<E, LinkedList<E> > Iterator;
         typedef Common::LinkedListBuilder<E, LinkedList<E> > Builder;
      };


      template <class E> 
      class LinkedList : public Sequence<E, LinkedList<E>, LinkedListTraits<E> >
      {
      public:
         friend class Common::LinkedListIterator<E, LinkedList<E> >;
         friend class Common::LinkedListBuilder<E, LinkedList<E> >;

         typedef E ElementType;
         typedef Common::LinkedListIterator<E, LinkedList<E> > Iterator;
         typedef Common::LinkedListNode<E> Node;
         typedef Common::LinkedListBuilder<E, LinkedList<E> > Builder;
         template <class U> struct SwapElementType { typedef LinkedList<U> C; };


      private:

         int _head, _tail, _size; 
         mutable Ref<Common::MemoryPool<Node> > _nodePool;

         // Used for creating new lists which alias the node array with 
         // an existing list
         inline LinkedList
            ( int size
            , int head
            , int tail
            , Ref<Common::MemoryPool<Node> > m)
            : _size(size)
            , _head(head)
            , _tail(tail)
            , _nodePool(m) {}
      
      public:

         ///////////////////////////////////////////
         // Copy Constructor, Reference Semantics //
         ///////////////////////////////////////////
      
         /// The data array's reference counter is automatically incremented
         /// Question: should we allocate a node pool in this constructor?
         inline LinkedList() 
            : _size(0), _head(-1), _tail(-1)
            , _nodePool(new Common::MemoryPool<Node>(0x10)) { }
      
         /// The data array's reference counter is automatically incremented
         inline LinkedList(const LinkedList& rhs)
            : _size(rhs._size)
            , _head(rhs._head)
            , _tail(rhs._tail)
            , _nodePool(rhs._nodePool) { }

         //////////////////////////////////////////////
         // Assignment Operator, Reference Semantics //
         //////////////////////////////////////////////
      
         inline LinkedList& operator = (const LinkedList& rhs)
         {  
            _head = rhs._head; _tail = rhs._tail; 
            _size = rhs._size; 
            _nodePool = rhs._nodePool; 
            return *this; 
         }


         // Debug
         void Print() const;

         ////////////////////////////////
         // Inherited From Traversable //
         ////////////////////////////////
      
         virtual int Size() const { return _size; }
         virtual Iterator GetIterator() const { return Iterator(*this); }

         virtual LinkedList<E> Tail() const;
         virtual LinkedList<E> Init() const;
         virtual LinkedList<E> Drop(int n) const;
         virtual LinkedList<E> Take(int n) const;
         virtual E Last() const;

    
         /////////////////////////////
         // Inherited From Sequence //
         /////////////////////////////
      
         inline const E& operator [] (int i) const 
         { 
            assert(i < _size);
         
            int n = _head;
            for (int c = 0; c < i; ++c) n = _nodePool->Index(n).next;
            return _nodePool->Index(n).payload;
         }
      
         virtual LinkedList<E> Append(const E& e) const;
         virtual LinkedList<E> Prepend(const E& e) const;
         virtual LinkedList<E> Reverse() const;
         virtual LinkedList<E> Sorted() const;
      };


      /// O(1)
      template <class E> E LinkedList<E>::Last() const
      { assert(Size() > 0); return _nodePool->Index(_tail).payload; }


      /// O(1)
      /// Returns a new container without the first element
      template <class E> LinkedList<E> LinkedList<E>::Tail() const
      { return Drop(1); }

      /// O(n)
      /// Returns a new container without the *last* element
      template <class E> LinkedList<E> LinkedList<E>::Init() const
      { return Take(ToolChest::MAX(0, Size()-1)); }

      /// O(n)
      /// Returns a container without the first n elements
      template <class E> LinkedList<E> LinkedList<E>::Drop(int n) const
      {
         const int N = ToolChest::MIN(n, _size);
         if (N == _size) return LinkedList<E>();
         else
         {
            int newHead = _head;
            for (int i = 0; i < N; ++i) newHead = _nodePool->Index(newHead).next;
            return LinkedList<E>(_size - N, newHead, _tail, _nodePool);
         }
      }

      /// O(n)
      /// Returns a container with only the first n elements
      template <class E> LinkedList<E> LinkedList<E>::Take(int n) const
      {
         const int N = ToolChest::MIN(n, _size);
         if (N == 0) return LinkedList<E>();
         else
         {
            /// Walk to the tail of the new list
            int newTail = _head;
            for (int i = 1; i < N; ++i) newTail = _nodePool->Index(newTail).next;
            return LinkedList<E>(N, _head, newTail, _nodePool);
         }
      }
      
      /// O(1)
      template <class E> LinkedList<E> LinkedList<E>::Prepend(const E& e) const
      {
         // Push a new node into the memory pool, return new list
         int newHead = _nodePool->Push(Node(e, _head));
         int newTail = _size == 0 ? newHead : _tail;
         return LinkedList<E>(_size+1, newHead, newTail, _nodePool);
      }

      template <class E> void LinkedList<E>::Print() const
      {
         printf("  s%i, h%i, t%i, freeIndex:%i [", _size, _head, _tail, _nodePool->NextFreeIndex());
         for (int i = 0; i < 14; ++i)
            printf("(%i, %i) ", _nodePool->Index(i).payload, _nodePool->Index(i).next);
         printf("]\n");
      }

      /// O(1)
      template <class E> LinkedList<E> LinkedList<E>::Append(const E& e) const
      {
         if (_size == 0)
         {
            assert(_head == -1 && _tail == -1);
            int index = _nodePool->Push(Node(e, -1));
            return LinkedList<E>(_size+1, index, index, _nodePool);            
         }
         else if (_nodePool->Index(_tail).next != -1)
            return Sequence<E, LinkedList<E>, LinkedListTraits<E> >::Append(e);
         else
         {
            int newTail = _nodePool->Push(Node(e, -1));
            _nodePool->Index(_tail).next = newTail;
            return LinkedList<E>(_size+1, _head, newTail, _nodePool);
         }
      }
      
      /// O(N)
      template <class E> LinkedList<E> LinkedList<E>::Reverse() const
      {
         if (_size <= 1) return *this;
      
         Builder builder = this->clone(_size, _size);
         LinkedList newList = builder.Result();
      
         int n = newList._head;
         int nPlusOne = newList._nodePool->Index(n).next;

         newList._nodePool->Index(n).next = -1; // head becomes tail
         while (nPlusOne != -1)
         {
            int nPlusTwo = newList._nodePool->Index(nPlusOne).next;
            newList._nodePool->Index(nPlusOne).next = n;
            n = nPlusOne;
            nPlusOne = nPlusTwo;
         }

         /// Swap the head and tail
         int temp = newList._head;
         newList._head = newList._tail;
         newList._tail = temp;

         return newList;
      }

      template <class E> LinkedList<E> LinkedList<E>::Sorted() const
      {
         Builder builder = this->clone(_size, _size);
         LinkedList newList = builder.Result();

         /// SORT HERE
         assert(false);

         return newList;
      }
   }
}

#endif
