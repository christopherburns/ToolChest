#include <stdio.h>
#include <iostream>
#include <set>

#include <Mathematics.h>
#include <Collections.h>

#include "Clock.h"

using namespace std;
using namespace Mathematics;
using namespace Collections;

class Printer 
{ 
public: 
   void operator() (const float& f) const { printf("%.2f ", f); }
   void operator() (const int& i) const { printf("%i ", i); } 
   //void operator() (Common::KeyValuePair<int, float> p) const 
   //{ printf("(%i, %.2f) ", p.key, p.value); }
};

#define STREAM_OUT_DEF o << "[ "; Printer p; c.ForEach(p); o << "]"; return o;
//ostream& operator << (ostream& o, const Mutable::TreeSet<int>& c)   { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::TreeSet<int>  & c) { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::TreeSet<float>& c) { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Mutable::TreeSet<int>  & c)   { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Mutable::TreeSet<float>& c)   { STREAM_OUT_DEF }

struct IsEven { bool operator() (int i) const { return (i%2) == 0; }};
struct IsOdd { bool operator() (int i) const { return (i%2) != 0; }};
struct Sqrt { float operator() (int i) const { return sqrt((float)i); }};

/////////////////////////
// Performance Testing //
/////////////////////////

/// Here we test the performance of the data structure by comparing with an 
/// equivalent stl data structure. We compare:
///    tree construction (a sequence of append operations)
///    fold-right operation (traversal-reduction)


template <class C> void performanceTestTreeSet()
{
   typedef C MyTreeSet;

   const int N = 500000;

   srand(1001938110);
   int * orderedPool = new int[N];
   int * randomPool = new int[N];
   for (int i = 0; i < N; ++i) 
   { orderedPool[i] = i; randomPool[i] = rand(); }

   const int MINE = 0;
   const int THEIRS = 1;

   StopWatch watch;
   double orderedConstructionTimes[2] = { 0.0, 0.0 };
   double randomConstructionTimes[2] = { 0.0, 0.0 };
   double removalTimes[2] = { 0.0, 0.0 };
   double reductionTimes[2] = { 0.0, 0.0 };


   //////////////////////////////////
   // TreeSet Ordered Construction //
   //////////////////////////////////

   watch.Start();
   typename MyTreeSet::Builder builder(N);
   for (int i = 0; i < N; ++i) builder.AddElement(orderedPool[i]);
   MyTreeSet denseSet = builder.Result();
   watch.Stop();
   orderedConstructionTimes[MINE] = watch.ReadTime().ToMilliseconds();

   watch.Start();
   std::set<int> uglySTLSet;
   for (int i = 0; i < N; ++i) uglySTLSet.insert(orderedPool[i]);
   watch.Stop();
   orderedConstructionTimes[THEIRS] = watch.ReadTime().ToMilliseconds();


   /////////////////////////////////
   // TreeSet Random Construction //
   /////////////////////////////////

   {
      watch.Start();
      typename MyTreeSet::Builder builder(N);
      for (int i = 0; i < N; ++i) builder.AddElement(randomPool[i]);
      MyTreeSet l = builder.Result();
      watch.Stop();
      randomConstructionTimes[MINE] = watch.ReadTime().ToMilliseconds();

   }

   {
      watch.Start();
      std::set<int> randomSTLSet;
      for (int i = 0; i < N; ++i) randomSTLSet.insert(randomPool[i]);
      watch.Stop();
      randomConstructionTimes[THEIRS] = watch.ReadTime().ToMilliseconds();
   }

   /////////////////////
   // TreeSet Removal //
   /////////////////////

   //denseSet.PrintGraph("FullGraph.dot");
   {
      watch.Start();
      for (int i = 0; i < N; i += 2) denseSet -= orderedPool[i];
      watch.Stop();
      removalTimes[MINE] = watch.ReadTime().ToMilliseconds();
   }
   //denseSet.PrintGraph("HalfGraph.dot");
   {
      watch.Start();
      for (int i = 0; i < N; i += 2) uglySTLSet.erase(orderedPool[i]);
      watch.Stop();
      removalTimes[THEIRS] = watch.ReadTime().ToMilliseconds();
   }

   ///////////////////////
   // TreeSet Reduction //
   ///////////////////////


   {
      watch.Start();
      float sum = 0;
      auto itr = denseSet.GetIterator();
      while (itr.HasNext()) sum += 1.0f / float(itr.Next());
      watch.Stop();
      reductionTimes[MINE] = watch.ReadTime().ToMilliseconds();
      if (sum < 0.0f) printf("sum = %.2f\n", sum);
   }

   {
      watch.Start();
      float sum = 0;
      for (std::set<int>::iterator i = uglySTLSet.begin(); i != uglySTLSet.end(); ++i)
         sum += 1.0f / float(*i);
      watch.Stop();
      reductionTimes[THEIRS] = watch.ReadTime().ToMilliseconds();
      if (sum < 0.0f) printf("sum = %.2f\n", sum);
   }

   delete [] orderedPool;
   delete [] randomPool;

   // Report results...
   printf("                                Burns          STL   \n");
   printf("TreeSet Ordered Construction:   %8.2f ms  %8.2f ms\n", (float)orderedConstructionTimes[MINE], (float)orderedConstructionTimes[THEIRS]);
   printf("TreeSet Random  Construction:   %8.2f ms  %8.2f ms\n", (float)randomConstructionTimes[MINE], (float)randomConstructionTimes[THEIRS]);
   printf("TreeSet Item Removal:           %8.2f ms  %8.2f ms\n", (float)removalTimes[MINE], (float)removalTimes[THEIRS]);
   printf("TreeSet Reduction:              %8.2f ms  %8.2f ms\n", (float)reductionTimes[MINE]   , (float)reductionTimes[THEIRS]);

   printf("Exiting profiling routine...\n");
}


int main()
{
   performanceTestTreeSet<Mutable::TreeSet<int> >();
   return 0;
}




