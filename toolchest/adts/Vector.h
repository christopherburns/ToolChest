#ifndef VECTOR_H
#define VECTOR_H

#include "../collections/MutableArray.h"

namespace Collections
{
   template <class E> class Vector : public Mutable::Array<E>
   {
   private:
      int _currentSize;  /// Actual number of valid elements, less than capacity

      inline bool isFull() const { return Capacity() == Size(); }

      inline void resize(int newCapacity)
      {
         if (newCapacity <= Capacity()) return;

         ToolChest::Ref<Common::InitializedBuffer<E> > newData 
            = new Common::InitializedBuffer<E>(newCapacity);
         for (int i = 0; i < Size(); ++i)
            ((E*)*newData)[i] = ((E*)(*Mutable::Array<E>::_data))[i];

         Mutable::Array<E>::_data = newData;
         Mutable::Array<E>::_size = newCapacity;
      }

      inline void expandIfFull()
      {
         static const int MIN_CAPACITY = 0x40;
         if (isFull())
         {
            if (Capacity() == 0) resize(MIN_CAPACITY);
            else                 resize(2*Capacity());
         }
      }

   public:
      inline Vector() : _currentSize(0) {}
      inline Vector(const Vector& rhs) 
         : Mutable::Array<E>(rhs)
         , _currentSize(rhs._currentSize) { }
      inline Vector(const Mutable::Array<E>& rhs)
         : Mutable::Array<E>(rhs)
         , _currentSize(rhs.Size()) { }


      ///////////////
      // Factories // 
      ///////////////

      static Vector Construct(int capacity)
      {
         typename Mutable::Array<E>::Builder builder(capacity);
         return Vector(builder.Result());
      }

      static Vector Construct(int size, const E * values)
      {
         typename Mutable::Array<E>::Builder builder(size);
         for (int i = 0; i < size; ++i) builder.AddElement(values[i]);
         return Vector(builder.Result());
      }

      inline int Capacity() const { return Mutable::Array<E>::Size(); }
      virtual int Size() const { return _currentSize; }

      // We need to override the GetIterator function so that we can
      // set the iterator's 
      inline typename Mutable::Array<E>::Iterator GetIterator() const 
      {
         auto itr = Mutable::Array<E>::GetIterator();
         itr._size = _currentSize;     //< This is key
         return itr; 
      }

      inline Vector& Push(const E& e) 
      {
         expandIfFull();

         ((E*)(*Mutable::Array<E>::_data))[_currentSize].~E();   /// Call destructor on existing thing  
         new (& ((E*)(*Mutable::Array<E>::_data))[_currentSize++]) E(e); /// Construct new copy in its place 

         return *this;
      }

      inline Vector& operator += (const E& e) { return this->Push(e); }

      inline E Pop()
      {
         assert(Mutable::Array<E>::NonEmpty());
         return ((E*)(*Mutable::Array<E>::_data))[--_currentSize];
      }

      inline void Clear() { _currentSize = 0; }
   };

}

#endif