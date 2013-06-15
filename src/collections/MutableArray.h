
#pragma once

#ifndef MUTABLE_ARRAY_H
#define MUTABLE_ARRAY_H

#include "Array.h"

namespace Collections
{
   namespace Mutable
   {
      template <class E> class Array;

      template <class E> struct ArrayTraits
      {
         typedef Common::ArrayIterator<E> Iterator;
         typedef Common::ArrayBuilder<E, Array<E> > Builder;
      };

      template <class E> class Array : public Sequence<E, Array<E>, ArrayTraits<E> >
      {
      public:

         friend class Common::ArrayIterator<E>;
         friend class Common::ArrayBuilder<E, Array<E> >;

         typedef E ElementType;
         typedef Common::ArrayIterator<E> Iterator;
         typedef Common::ArrayBuilder<E, Array<E> > Builder;
         template <class U> struct SwapElementType { typedef Array<U> C; };


      protected:
      public:
         int _size;
         Burns::Ref<Common::InitializedBuffer<E> > _data;

         inline Array(int initSize) 
            : _size(initSize), _data(new Common::InitializedBuffer<E>(initSize)) {}

      public:
      
         /// Construct a mutable array of size zero
         inline Array() : _size(0), _data(new Common::InitializedBuffer<E>()) {}


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

         virtual int Size() const { return _size; }
         inline Iterator GetIterator() const { return Iterator(*this); }

         virtual E Last() const;

         /////////////////////////////
         // Inherited From Sequence //
         /////////////////////////////

         inline E& operator [] (int i) { return _data->Index(i); }
         inline const E& operator [] (int i) const { return _data->Index(i); }

         virtual Array<E> Reverse() const;
      };


      ///////////////////////
      // Array Definitions //
      ///////////////////////

      /// O(1)
      template <class E> E Array<E>::Last() const
      { return ((E*)*_data)[Size()-1]; }

      template <class E> Array<E> Array<E>::Reverse() const                           
      {                                                                               
         Array<E> newArray(Size());                                                   
         for (int i = 0; i < Size(); ++i)                                             
            ((E*)*newArray._data)[i] = (*this)[Size()-i-1];                            
         return newArray;                                                             
      } 
   } // namespace Mutable
} // namespace Collections

#endif