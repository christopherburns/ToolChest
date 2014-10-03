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
   typedef std::set<int> STLMyTreeSet;

   const int N = 200000;

   srand(1001938110);
   int * orderedPool = new int[N];
   int * randomPool = new int[N];
   for (int i = 0; i < N; ++i) 
   { orderedPool[i] = i; randomPool[i] = rand(); }

   const int MINE = 0;
   const int THEIRS = 1;

   StopWatch watch;
   double orderedConstructionTimes[2];
   double randomConstructionTimes[2];
   double removalTimes[2];
   double reductionTimes[2];


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
   STLMyTreeSet stlMyTreeSet;
   for (int i = 0; i < N; ++i) stlMyTreeSet.insert(orderedPool[i]);
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

      watch.Start();
      STLMyTreeSet stlMyTreeSet;
      for (int i = 0; i < N; ++i) stlMyTreeSet.insert(randomPool[i]);
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

      watch.Start();
      for (int i = 0; i < N; i += 2) stlMyTreeSet.erase(orderedPool[i]);
      watch.Stop();
      removalTimes[THEIRS] = watch.ReadTime().ToMilliseconds();
   }
   //denseSet.PrintGraph("HalfGraph.dot");


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
   }

   {
      watch.Start();
      float stlSum = 0;
      for (auto itr = stlMyTreeSet.begin(); itr != stlMyTreeSet.end(); ++itr)
         stlSum += 1.0f / (*itr);
      watch.Stop();
      reductionTimes[THEIRS] = watch.ReadTime().ToMilliseconds();
   }


   delete [] orderedPool;
   delete [] randomPool;

   // Report results...
   printf("                                Burns          STL   \n");
   printf("TreeSet Ordered Construction:   %8.2f ms  %8.2f ms\n", (float)orderedConstructionTimes[MINE], orderedConstructionTimes[THEIRS]);
   printf("TreeSet Random  Construction:   %8.2f ms  %8.2f ms\n", (float)randomConstructionTimes[MINE], randomConstructionTimes[THEIRS]);
   printf("TreeSet Item Removal:           %8.2f ms  %8.2f ms\n", (float)removalTimes[MINE], removalTimes[THEIRS]);
   printf("TreeSet Reduction:              %8.2f ms  %8.2f ms\n", (float)reductionTimes[MINE]   , reductionTimes[THEIRS]);

   printf("Exiting profiling routine...\n");
}




void testMutableTreeSet() 
{
   typedef typename Mutable::TreeSet<int> Tree;
   typedef typename Tree::Iterator Iterator;
   int values[] = { 0, 1, 2, 3, 248, 101, 4, 5, 70, 6, 14, 7 };

   Tree s = Tree::Construct(12, values);
   Tree s_ = s + 80;
   s += 100;
   s += 200;

   cout << "s  = " << s  << endl;
   cout << "s_ = " << s_ << endl;

   s -= s_;

   cout << "s -= s_ -> " << s << endl;
}

template <class C> void testTreeSet()
{
   ////////////////////////////////
   // Immutable LinkedList Tests //
   ////////////////////////////////

   typedef C Tree;
   typedef typename Tree::Iterator Iterator;
   int values[] = { 0, 1, 2, 3, 248, 101, 4, 5, 70, 6, 14, 7 };

   /// For the immutable list, we're going to try to focus on the ops that
   /// avoid replicating data and share nodes with previous versions of the 
   /// lists

   printf("Testing TreeSet Operations ...\n");

   printf("TreeSet::Construct...\n");
   Tree s = Tree::Construct(12, values);

   Tree s_ = s + 80;
   Tree s__ = s_ + 90;
   Tree _s = -1 + s;
   Tree __s = -2 + _s;

   cout << "s   = " << s << endl;
   cout << "s_  = " << s_ << endl;
   cout << "s__ = " << s__ << endl;
   cout << "_s  = " << _s << endl;
   cout << "__s = " << __s << endl;

   cout << endl;

   {
      Tree s2 = s.Take(4) + 400 + 500 + 600;
      Tree s3 = s.Take(4) + 4000 + 5000;
      Tree s4 = s.Drop(4) + (300) + (200);

      cout << "s2 = " << s2 << endl;   
      cout << "s3 = " << s3 << endl;
      cout << "s4 = " << s4 << endl;
   } // calls destructors

   cout << "s   = " << s << endl;
   cout << "s_  = " << s_ << endl;
   cout << "s__ = " << s__ << endl;
   cout << "_s  = " << _s << endl;
   cout << "__s = " << __s << endl;
   cout << endl;

   /// Test removals...
   cout << "Stripping s of odd values..." << endl;
   Tree sNoOdds = s;
   sNoOdds = sNoOdds.Remove(70);
   while (sNoOdds.Exists(IsOdd()))
   {
      int n = 0;
      Iterator itr = sNoOdds.GetIterator();
      while (itr.HasNext() && (n % 2 == 0)) n = itr.Next();

      sNoOdds = sNoOdds.Remove(n);
      cout << "   -> " << sNoOdds << endl;

      //sNoOdds.PrintGraph(*(String("Removal") + n + ".dot"));
   }
   cout << "   sNoOdds = " << sNoOdds << endl << endl;

   cout << "s.Filter(IsEven())    = " << s.Filter(IsEven()) << endl;
   cout << "s.FilterNot(IsEven()) = " << s.FilterNot(IsEven()) << endl;
   cout << "s.Count(IsEven()) == s.Filter(IsEven()).Size() = " << 
      ((s.Count(IsEven()) == s.Filter(IsEven()).Size()) ? "true" : "false") << endl;

   auto sSqrt = MapValues<C, float, Sqrt>(s, Sqrt());
   cout << "MapValues(s, Sqrt()) = " << sSqrt << endl;

   cout << endl;
   testMutableTreeSet();
}

int main()
{
   //testTreeSet<Immutable::TreeSet<int> >();
   //testTreeSet<Mutable::TreeSet<int> >();
   performanceTestTreeSet<Mutable::TreeSet<int> >();

   printf("Exiting main...\n");
   return 0;
}




