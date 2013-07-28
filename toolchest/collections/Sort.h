
#ifndef SORT_H
#define SORT_H

template <class T> static 
typename Collections::Common::InitializedBuffer<T> QuickSort
(int size, Collections::Common::InitializedBuffer<T> a)
{
   if (size == 1)
   {
      return a;
   }
   else if (size == 2)
   {
      Collections::Common::InitializedBuffer<T> sorted(size);
      sorted[0] = a[0] < a[1] ? a[0] : a[1];
      sorted[1] = a[0] < a[1] ? a[1] : a[0];
      return sorted;
   }
   else
   {
      Collections::Common::InitializedBuffer<T> less(size);    
      Collections::Common::InitializedBuffer<T> greater(size); 

      int l = 0, g = 0;
      int pivotIndex = size/2;
      int pivot = a[pivotIndex];
      for (int i = 0; i < size; ++i)
      {
         if (a[i] < pivot) less   [l++] = a[i];
         else              greater[g++] = a[i];
      }

      auto lSorted = QuickSort(l, less);
      auto gSorted = QuickSort(g, greater);

      /// Move these items into the less array
      for (int i = 0; i < l       ; ++i) less[i] = lSorted[i  ];
      for (int i = l; i < a.Size(); ++i) less[i] = gSorted[i-l];

      return less;
   }
}   

namespace Collections
{
   namespace Mutable
   {
      template <class E> Array<E> Array<E>::Sorted() const
      {
         auto sortedData = QuickSort<E>(Size(), *_data);
         return Collections::Mutable::Array<E>(Size(), sortedData);
      }
   }
};






/*template <class T> void QuickSortInPlace(Mutable::Array<T> a)
{

}*/

#endif