#ifndef MUTABLE_LINKED_LIST_H
#define MUTABLE_LINKED_LIST_H

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
   namespace Mutable
   {
      using ToolChest::Ref;

      template <class E> class LinkedList;

      template <class E> struct LinkedListTraits
      {
         typedef Common::MutableLinkedListIterator<E, LinkedList<E> > Iterator;
         typedef Common::LinkedListBuilder<E, LinkedList<E> > Builder;
      };


      template <class E> 
      class LinkedList : public Sequence<E, LinkedList<E>, LinkedListTraits<E> >
      {
      public:
         friend class Common::MutableLinkedListIterator<E, LinkedList<E> >;
         friend class Common::LinkedListIterator<E, LinkedList<E> >;
         friend class Common::LinkedListBuilder<E, LinkedList<E> >;

         typedef E ElementType;
         typedef Common::MutableLinkedListIterator<E, LinkedList<E> > Iterator;
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
         inline LinkedList(int reserve = 4) 
            : _size(0), _head(-1), _tail(-1)
            , _nodePool(new Common::MemoryPool<Node>(reserve)) { }
      
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
      
         virtual LinkedList<E> Reverse() const;
         virtual LinkedList<E> Sorted() const;

         /////////////////////////////
         // Mutable LinkedList Only //
         /////////////////////////////

         inline E& operator [] (int i)
         { 
            assert(i < _size);
         
            int n = _head;
            for (int c = 0; c < i; ++c) n = _nodePool->Index(n).next;
            return _nodePool->Index(n).payload;
         }

         LinkedList<E>& operator += (const E& e);
         LinkedList<E>& operator += (const LinkedList<E>& list);

         /// Semantics: inserts an element at the location currently occupied
         /// by the value returned by Next(). That is, after Insert(itr, e),
         /// itr->Next() will return e.
         LinkedList<E>& Insert(Iterator& itr, const E& e);
         LinkedList<E>& Remove(Iterator& itr);
      };

      template <class E> LinkedList<E> LinkedList<E>::Reverse() const
      {
         Builder builder(Size());
         Iterator itr = GetIterator();
         while (itr.HasNext()) builder.PrependElement(itr.Next());
         return builder.Result();
      }

      template <class E> LinkedList<E>& LinkedList<E>::operator += (const E& e)
      {
         if (_size == 0) _head = _tail = _nodePool->Push(Node(e, -1));
         else
         {
            int newTail = _nodePool->Push(Node(e, -1));
            _nodePool->Index(_tail).next = newTail;
            _tail = newTail;
         }
         _size++;
         return *this;
      }

      template <class E> LinkedList<E>& LinkedList<E>::operator += (const LinkedList<E>& list)
      {
         /// Since these data structures are mutable, we need to copy new nodes
         Iterator itr = list.GetIterator();
         while (itr.HasNext()) (*this) += itr.Next();
         return *this;
      }

      template <class E> LinkedList<E>& 
      LinkedList<E>::Insert( Iterator& itr, const E& e)
      {
         /// We must detect whether we're prepending or appending or neither
         if (itr._next == -1) 
         { assert(itr._current == _tail); return (*this) += e; } // Placing at tail == Append
         else if (itr._next == _head)  // Placing at head == Prepend
         { 
            assert(itr._current == -1); 
            int N = _nodePool->Push(Node(e, _head));  // N -> itr._next
            itr._next = _head = N;
            _size++;
         }
         else 
         {
            /// insert in middle of linked list
            int N = _nodePool->Push(Node(e, itr._next));  // N -> itr._next
            _nodePool->Index(itr._current).next = N;
            itr._next = N;
            _size++;
         }

         return *this;
      }

      template <class E> LinkedList<E>& 
      LinkedList<E>::Remove( Iterator& itr)
      {
         assert(itr.HasNext());
         assert(itr._current != -1 ? _nodePool->Index(itr._current).next == itr._next : true);

         /// We remove the node indicated by "next"
         int n = _nodePool->Index(itr._next).next;
         if (itr._current != -1)
            _nodePool->Index(itr._current).next = n;

         if (_head == itr._next) _head = n;
         if (_tail == itr._next) _tail = itr._current;

         itr._next = n;
         _size--;
         return *this;
      }

      template <class E> LinkedList<E> LinkedList<E>::Sorted() const
      {
         Builder builder = this->clone(_size, _size);
         LinkedList newList = builder.Result();

         /// SORT HERE
         assert(false);

         return newList;
      }

   } // namespace Mutable
} // namespace Collections


#endif
