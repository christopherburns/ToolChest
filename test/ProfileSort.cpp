#include <stdio.h>
#include <iostream>
#include <vector>

#include "../toolchest/ToolChest.h"

using namespace std;
using namespace ToolChest;
using namespace Collections;

template <class T> struct ToString { constexpr static const char * const s = ""; };

template <> struct ToString<Immutable::Array<int> >           { constexpr static const char * const value = "Immutable::Array<int>"; };
template <> struct ToString<Mutable::Array<int>   >           { constexpr static const char * const value = "Mutable::Array<int>"; };
template <> struct ToString<Immutable::LinkedList<int> >      { constexpr static const char * const value = "Immutable::LinkedList<int>"; };
template <> struct ToString<Mutable::LinkedList<int>   >      { constexpr static const char * const value = "Mutable::LinkedList<int>"; };
template <> struct ToString<Immutable::TreeSet<int> >         { constexpr static const char * const value = "Immutable::TreeSet<int>"; };
template <> struct ToString<Mutable::TreeSet<int>   >         { constexpr static const char * const value = "Mutable::TreeSet<int>"; };
template <> struct ToString<Immutable::TreeMap<int, float> >  { constexpr static const char * const value = "Immutable::TreeMap<int, float>"; };
template <> struct ToString<Mutable::TreeMap<int, float> >    { constexpr static const char * const value = "Mutable::TreeMap<int, float>"; };

class Printer 
{ 
public: 
   void operator() (const float& f) const { printf("%.1f ", f); }
   void operator() (const int& i) const { printf("%i ", i); } 
   void operator() (Common::KeyValuePair<int, float> p) const 
   { printf("(%i, %.2f) ", p.key, p.value); }
};


#define STREAM_OUT_DEF o << "[ "; Printer p; c.ForEach(p); o << "]"; return o;
ostream& operator << (ostream& o, const Collections::Vector<int>& c)          { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Mutable::Array<int>& c)               { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Mutable::LinkedList<int>& c)          { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::Array<int>& c)             { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::Array<float>& c)           { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::LinkedList<int>& c)        { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::LinkedList<float>& c)      { STREAM_OUT_DEF }


void Profile_MutableArraySort
   ( const int N
   , float& sortedTime
   , float& inPlaceSortTime
   , float& stlTime)
{
   StopWatch watch;
   std::pair<float, float> times;

   /// Construct two arrays, one using our container, one using
   /// an equivalent stl container, both with same element values
   srand(1001938110);
   std::vector<int> stlArray;
   for (int i = 0; i < N; ++i) stlArray.push_back(rand() % (2*N));
   auto burnsArray = Mutable::Array<int>::Construct(N, &stlArray[0]);

   watch.Start();
   Mutable::Array<int> sorted = burnsArray.Sorted();
   watch.Stop(); sortedTime = watch.ReadTime().ToMilliseconds();

   watch.Start();
   burnsArray.SortInPlace();
   watch.Stop(); inPlaceSortTime = watch.ReadTime().ToMilliseconds();

   watch.Start();
   std::sort (stlArray.begin(), stlArray.begin()+N);  
   watch.Stop(); stlTime = watch.ReadTime().ToMilliseconds();
}


int main()
{
   cout << endl << "Testing Sort on Array...." << endl << endl;

   const int N = 5000000;

   float mArray[3] = {0.0f};
   Profile_MutableArraySort (N, mArray[0], mArray[1], mArray[2]);

   printf("                         Sorted()   SortInPlace()     std::sort()   \n");
   printf("Mutable::Array<int>   %8.2f ms     %8.2f ms     %8.2f ms\n", mArray[0], mArray[1], mArray[2]);
   

   return 0;
}