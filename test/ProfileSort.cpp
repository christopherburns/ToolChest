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
ostream& operator << (ostream& o, const Mutable::TreeSet<int>& c)             { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Mutable::TreeMap<int, float>& c)      { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::Array<int>& c)             { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::Array<float>& c)           { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::LinkedList<int>& c)        { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::LinkedList<float>& c)      { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::TreeSet<int>& c)           { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::TreeMap<int, float>& c)    { STREAM_OUT_DEF }


std::pair<float, float> Profile_MutableArraySort(const int N)
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
   watch.Stop(); times.first = watch.ReadTime().ToMilliseconds();

   watch.Start();
   std::sort (stlArray.begin(), stlArray.begin()+N);  
   watch.Stop(); times.second = watch.ReadTime().ToMilliseconds();

   return times;
}

void Test_QuickSortInPlace (Mutable::Array<int> a)
{

}

void Test_TreeSort         (Mutable::Array<int> a)
{

}



int main()
{
   cout << endl << "Testing Sort on Array...." << endl << endl;

   const int N = 1000000;

   auto rSorted = Profile_MutableArraySort(N);

   printf("                                   Burns        STL   \n");
   printf("Mutable::Array<int> Sorted()   %8.2f ms  %8.2f ms\n", rSorted.first, rSorted.second);
   

   return 0;
}