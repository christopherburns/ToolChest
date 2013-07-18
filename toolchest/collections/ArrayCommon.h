#pragma once

#ifndef ARRAY_COMMON_H
#define ARRAY_COMMON_H

namespace Collections
{
   template <class E> class Vector;

   namespace Immutable
   {
      template <class E> class TreeSet;
      template <class E> class Array;
      template <class E> class LinkedList;
   }

   namespace Mutable
   {
      template <class E> class TreeSet;
      template <class E> class Array;
      template <class E> class LinkedList;
   }

	namespace Common
	{
      //////////////////////////////////////
      // Structures for Array Collections //
      //////////////////////////////////////


      /// This is the prototypical Iterator class interface for all collection
      /// classes, more or less. It is forward iterating only, It initially
      /// points to a fictional element before the first (index 0) element, and
      /// the call "Next" both advances the iterator to the next element and
      /// returns a const reference to it (Mutable collections will return a
      /// non-const reference)

      template <class E> class ArrayIterator
      {
         /// The initial iterator has _i=-1, to indicate the predecessor
         /// of the first element
      private:

         int _i;      
         int _size;
         ToolChest::Ref<Common::InitializedBuffer<E> > _data;
      
         inline ArrayIterator(const Mutable::Array<E>& a)   : _size(a._size), _data(a._data), _i(-1) {}
         inline ArrayIterator(const Immutable::Array<E>& a) : _size(a._size), _data(a._data), _i(-1) {}
      
      public:
         inline ArrayIterator(const ArrayIterator& itr) : _i(itr._i), _size(itr._size), _data(itr._data) {}
         inline bool HasNext() const { return _size > (_i+1); }
         inline const E& Next() { assert(HasNext()); _i++; return _data->Index(_i); }
      
         friend class Mutable::Array<E>;
         friend class Immutable::Array<E>;
         friend class Vector<E>;
      };


      /// The builder must periodically resize the array beneath it
      template <class E, class C> class ArrayBuilder
      {
      private:
         C _array;   
         int _nextEmptyIndex;

         /// Flag goes true when the result has been returned and the 
         /// builder can no longer be used (it is disposable)
         bool _complete;

      public:
         inline ArrayBuilder(int expectedSize = 1) 
            : _array(ToolChest::MAX(expectedSize, 1)), _nextEmptyIndex(0), _complete(false)
         { }


         //////////////////////////////////////////////
         // Copy and Assignment, Reference Semantics //
         //////////////////////////////////////////////

         /// The data array's reference counter is automatically incremented
         inline ArrayBuilder(const ArrayBuilder& rhs)
            : _complete(rhs._complete)
            , _array(rhs._array) 
            , _nextEmptyIndex(rhs._nextEmptyIndex) { }

         /// Again, Ref<E> handles all the reference counting correctly, even if
         /// _data == rhs._data
         inline ArrayBuilder& operator = (const ArrayBuilder& rhs)
         {  
            _complete       = rhs._complete; 
            _nextEmptyIndex = rhs._nextEmptyIndex; 
            _array          = rhs._array;
            return *this; 
         }

         inline void AddElement(E t) 
         {
            assert(!_complete);
            if (_nextEmptyIndex == _array.Size()) resize();

            _array._data->Index(_nextEmptyIndex) = t;
            _nextEmptyIndex++;         
         }

         inline C Result() 
         {
            /// Fix the recorded size of the underlying array to the actual number
            /// of elements added (we leave the allocation where it is)
            assert(_nextEmptyIndex <= _array.Size());
            _array._size = _nextEmptyIndex;
            _complete = true;
            return _array;
         }

      private:

         // Allocate a new array, copy the values, double the size
         void resize()
         {
            assert(!_complete);
            assert(_array.Size() > 0);
            C _array2x = C(_array.Size() * 2);
            for (int i = 0; i < _array._size; ++i)
               ((E*)(*_array2x._data))[i] = ((E*)(*_array._data))[i];
            _array = _array2x;
            assert(_nextEmptyIndex < _array.Size()); 
         }
      };
	}
}

#endif
