#pragma once

#ifndef ARRAY_H
#define ARRAY_H

#include "Sequence.h"
#include "ArrayCommon.h"


////////////////////////////////////////////
// Class Array <- Sequence <- Traversable //
////////////////////////////////////////////

/// Class Array is Immutable - Once constructed, elements are unchanged

namespace Collections
{

   namespace Immutable
   {
      template <class E> class Array;

      template <class E> struct ArrayTraits
      {
         typedef Common::ArrayIterator<E> Iterator;
         typedef Common::ArrayBuilder<E, Array<E> > Builder;
      };


      template <class E> class Array : public Sequence<E, Array<E>, ArrayTraits<E> >
      {
      protected:
         int _size;
         ToolChest::Ref<Common::InitializedBuffer<E> > _data;

         

      public:

         friend class Common::ArrayIterator<E>;
         friend class Common::ArrayBuilder<E, Array<E> >;
      
         typedef E ElementType;
         typedef Common::ArrayIterator<E> Iterator;
         typedef Common::ArrayBuilder<E, Array<E> > Builder;
         template <class U> struct SwapElementType { typedef Array<U> C; };

         /// Construct an immutable array of size zero
         //inline Array() : _size(0), _data(new Common::InitializedBuffer<E>()) {}
         inline Array(int initSize = 0) 
            : _size(initSize), _data(new Common::InitializedBuffer<E>(initSize)) {}

         //////////////////////////////////////////////
         // Copy and Assignment, Reference Semantics //
         //////////////////////////////////////////////
         
         /// The data array's reference counter is automatically incremented
         inline Array(const Array& rhs) : _size(rhs.Size()), _data(rhs._data) { }

         /// Again, Ref<E> handles all the reference counting correctly, even if
         /// _data == rhs._data
         inline Array& operator = (const Array& rhs)
         { _size = rhs._size; _data = rhs._data; return *this; }
      

         ////////////////////////////////
         // Inherited From Traversable //
         ////////////////////////////////
      
         inline int Size() const { return _size; } 
         inline Iterator GetIterator() const { return Iterator(*this); }

         /// These have efficient overrides for Immutable::Array
         virtual E Last() const;
         virtual Array<E> Init() const;
         virtual Array<E> Take(int n) const;


         /////////////////////////////
         // Inherited From Sequence //
         /////////////////////////////
      
         inline const E& operator [] (int i) const { return _data->Index(i); }
      
         virtual Array<E> Reverse() const;
         virtual Array<E> Sorted() const;
      };


      ///////////////////////
      // Array Definitions //
      ///////////////////////

      /// O(1)
      template <class E> E Array<E>::Last() const
      { assert(_size > 0); return ((E*)*_data)[_size-1]; }

      /// O(1)
      template <class E> Array<E> Array<E>::Init() const                              
      {                                                                               
         Array<E> copy = *this;
         copy._size = ToolChest::MAX(0, copy._size-1);
         return copy;
      }                                                                               
              
      /// O(1)                                                                        
      template <class E> Array<E> Array<E>::Take(int n) const                         
      {                                                                               
         Array<E> copy = *this;                                                       
         copy._size = ToolChest::MIN(n, copy._size);                                             
         return copy;                                                                 
      }     

      /// O(n)                                                                        
      template <class E> Array<E> Array<E>::Reverse() const                           
      {                                                                               
         Array<E> newArray(Size());                                                   
         for (int i = 0; i < Size(); ++i) 
            ((E*) *newArray._data)[i] = (*this)[Size()-i-1];
         return newArray;                                                             
      }  

      template <class E> 
      Array<E> Array<E>::Sorted() const
      {
         /// 1. Make a copy
         /// 2. In-place quicksort the copy
         Builder builder = this->clone(this->Size(), this->Size()); 
         Array<E> sorted = builder.Result();
         Common::SortInPlace(((E*)*sorted._data), 0, Size()-1);
         return sorted;
      }                                                                             
   } // namespace Immutable
} // namespace Collections

#endif // ARRAY_H
