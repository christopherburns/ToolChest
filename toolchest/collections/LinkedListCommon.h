#ifndef LINKED_LIST_COMMON_H
#define LINKED_LIST_COMMON_H


namespace Collections
{
   namespace Immutable
   {
      template <class E> class LinkedList;
   }

   namespace Mutable
   {
      template <class E> class LinkedList;
   }

	namespace Common
	{
      using ToolChest::Ref;

      ///////////////////////////////////////////
      // Structures for LinkedList Collections //
      ///////////////////////////////////////////

      /// Consider an iterator method called Advance() which 
      /// has the same side effect as Next(), but returns *this so that you can:
      ///     itrToThirdItem = t.GetIterator().Advance().Advance();
      ///
      /// Consider operator* for equivalent of Iterator::Peek()

      template <class E> class LinkedListNode
      {
      public:
         E payload;
         int next;

         inline LinkedListNode() : next(-1) {}
         inline LinkedListNode(const E& p, int n) : payload(p), next(n) {}            
         inline ~LinkedListNode() {}

         inline bool operator == (const LinkedListNode& rhs) const
         { return rhs.payload == payload && rhs.next == next; }
         inline bool operator != (const LinkedListNode& rhs) const { return !operator==(rhs); }
      };

      template <class E, class C> class LinkedListIterator
      {
      protected:
         Ref<MemoryPool<LinkedListNode<E> > > _nodePool;
         int _next, _last;

         inline LinkedListIterator(const C& a) 
            : _next(a._head), _last(a._tail), _nodePool(a._nodePool) {}

      public:
         inline LinkedListIterator(const LinkedListIterator& itr)
            : _next(itr._next), _last(itr._last), _nodePool(itr._nodePool) {}
         inline bool HasNext() const 
         {
            assert( _next != -1 ? _last != -1 : true );
            return _next != -1 && _next != _nodePool->Index(_last).next;
         }
         inline const E& Next()
         { 
            assert(HasNext()); 
            const E& n = _nodePool->Index(_next).payload; 
            _next = _nodePool->Index(_next).next; 
            return n; 
         }
         inline const E& Peek() const
         {
            assert(HasNext());
            return _nodePool->Index(_next).payload;
         }

         friend class Immutable::LinkedList<E>;
         friend class Mutable::LinkedList<E>;
      };

      template <class E, class C> class MutableLinkedListIterator
      {
      protected:
         Ref<MemoryPool<LinkedListNode<E> > > _nodePool;
         int _current, _next, _last;

         inline MutableLinkedListIterator(const C& a) 
            : _current(-1), _next(a._head), _last(a._tail), _nodePool(a._nodePool) {}

      public:
         inline MutableLinkedListIterator(const MutableLinkedListIterator& itr)
            : _current(itr._current), _next(itr._next), _last(itr._last), _nodePool(itr._nodePool) {}
         inline bool HasNext() const 
         {
            assert( _next != -1 ? _last != -1 : true );
            return _next != -1 && _next != _nodePool->Index(_last).next;
         }
         inline E& Next()
         { 
            assert(HasNext()); 
            E& n = _nodePool->Index(_next).payload; 
            _current = _next;
            _next = _nodePool->Index(_next).next; 
            return n; 
         }
         inline const E& Peek() const
         {
            assert(HasNext());
            return _nodePool->Index(_next).payload;
         }

         friend class Immutable::LinkedList<E>;
         friend class Mutable::LinkedList<E>;
      };

      template <class E, class C> class LinkedListBuilder
      {
      private:
         bool _complete;
         int _size, _head, _tail;
         Ref<MemoryPool<LinkedListNode<E> > > _nodePool;

      public:

         static const int MIN_CAPACITY = 16;

         inline LinkedListBuilder(int expectedSize = 1)
            : _complete(false), _size(0), _head(-1), _tail(-1)
            , _nodePool(new MemoryPool<LinkedListNode<E> >(expectedSize))
         { }

         //////////////////////////////////////////////
         // Copy and Assignment, Reference Semantics //
         //////////////////////////////////////////////

         inline LinkedListBuilder(const LinkedListBuilder& rhs)
            : _complete(rhs._complete)
            , _size(rhs._size)
            , _head(rhs._head)
            , _tail(rhs._tail)
            , _nodePool(rhs._nodePool) { }

         /// This is dubious....
         inline LinkedListBuilder& operator = (const LinkedListBuilder& rhs)
         {
            _complete = rhs._complete;   _size = rhs._size;
            _head = rhs._head;           _tail = rhs._tail;
            _nodePool = rhs._nodePool;
            return *this;
         }

         // O(1)
         /// This is an append operation
         inline void AddElement(E e)
         {
            assert(!_complete);
            _size++;

            _nodePool->Push(LinkedListNode<E>(e, -1));
            int newTail = _nodePool->NextFreeIndex()-1;

            /// If newTail == 0, then this is the first entry
            if (newTail == 0) _head = _tail = 0;
            else
            {
               assert(_tail != -1); assert(_head != -1);
               _nodePool->Index(_tail).next = newTail;
               _tail = newTail;
            }
         }

         inline void PrependElement(E e)
         {
            assert(!_complete);
            _size++;

            _nodePool->Push(LinkedListNode<E>(e, _head));
            int newHead = _nodePool->NextFreeIndex()-1;
            if (newHead == 0) _head = _tail = 0;
            else _head = newHead;
         }


         inline C Result() 
         { 
            assert(!_complete); 
            _complete = true;
            C result = C(_size, _head, _tail, _nodePool);
            _size = 0;
            _head = _tail = -1;
            _nodePool = new MemoryPool<LinkedListNode<E> >(1);
            return result;
         }
      };
   }  // namespace Common
} // namespace Collections

#endif // LINKED_LIST_COMMON2_H





















