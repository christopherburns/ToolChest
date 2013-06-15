#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>

#include "../burns/Burns.h"

using namespace std;
using namespace Burns;
using namespace Collections;

class Printer 
{ 
public: 
   void operator() (const float& f) const { printf("%.1f ", f); }
   void operator() (const int& i) const { printf("%i ", i); } 
   //void operator() (Common::KeyValuePair<int, float> p) const 
   //{ printf("(%i, %.2f) ", p.key, p.value); }
};

#define STREAM_OUT_DEF o << "[ "; Printer p; c.ForEach(p); o << "]"; return o;
ostream& operator << (ostream& o, const Mutable::Array<int>& c)        { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Collections::Vector<float>& c) { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::Array<int>& c)      { STREAM_OUT_DEF }


void performanceTestVector()
{
   typedef Collections::Vector<int> Vec;
   typedef std::vector<int> STLVector;

   const int N = 2000000;

   srand(1001938110);
   int * pool = new int[N];
   for (int i = 0; i < N; ++i) pool[i] = i; //rand();

   const int MINE = 0;
   const int THEIRS = 1;

   StopWatch watch;
   double constructionTimes[2];
   double reductionTimes[2];
   double reversalTimes[2];

   //////////////////
   // Construction //
   //////////////////

   watch.Start();
   Vec v = Vec::Construct(N, pool);
   watch.Stop();
   constructionTimes[MINE] = watch.ReadTime().ToMilliseconds();

   watch.Start();
   STLVector stlVec;
   for (int i = 0; i < N; ++i) stlVec.push_back(pool[i]);
   watch.Stop();
   constructionTimes[THEIRS] = watch.ReadTime().ToMilliseconds();

   ///////////////
   // Reduction //
   ///////////////

   watch.Start();
   float sum = 0;
   Vec::Iterator itr = v.GetIterator();
   while (itr.HasNext()) sum += 1.0f / float(itr.Next());
   watch.Stop();
   reductionTimes[MINE] = watch.ReadTime().ToMilliseconds();

   watch.Start();
   float stlSum = 0;
   for (STLVector::iterator itr = stlVec.begin(); itr != stlVec.end(); ++itr)
      stlSum += 1.0f / (*itr);
   watch.Stop();
   reductionTimes[THEIRS] = watch.ReadTime().ToMilliseconds();

   printf("%.2f %.2f\n", sum, stlSum);

   delete [] pool;

   // Report results...
   printf("                        Burns         STL   \n");
   printf("Vector Construction:   %8.2f ms  %8.2f ms\n", (float)constructionTimes[MINE], constructionTimes[THEIRS]);
   printf("Vector Reduction:      %8.2f ms  %8.2f ms\n", (float)reductionTimes[MINE]   , reductionTimes[THEIRS]);

   printf("Exiting profiling routine...\n");
}


void testImmutableArray()
{
   ///////////////////////////
   // Immutable Array Tests //
   ///////////////////////////

   typedef Immutable::Array<int> Array;

   /// For the immutable list, we're going to try to focus on the ops that
   /// avoid replicating data and share nodes with previous versions of the 
   /// lists

   printf("Testing Immutable Array Operations ...\n");

   int values[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
   Array s = Array::Construct(8, values);
   Array s_ = s.Init();
   Array s__ = s_.Init();
   Array s__drop = s__.Drop(2);
   Array s__drop_reverse = s__drop.Reverse();
   Array _s = s.Take(4);

   Array mirror = s__drop + s__drop.Reverse();

   {
      Array s2 = s_.Take(6);

      auto itr = s2.GetIterator();
      while (itr.HasNext()) 
         cout << itr.Next() << endl;

      cout << "s2 = " << s2;
      auto s2R = s2.Reverse();

      /// Broken!
      cout << ", s2.Reverse() = " << s2R << endl;   
   } // calls destructors

   cout << "s   = " << s << endl;
   cout << "s_  = " << s_ << endl;
   cout << "s__ = " << s__ << endl;
   cout << "s__drop = " << s__drop << endl;
   cout << "s__drop_reverse = " << s__drop_reverse << endl;
   cout << "_s  = " << _s << endl;
   cout << "mirror = " << mirror << endl;
}

void testVector()
{
   typedef Collections::Vector<float> Vec;

   Vec v; v += 1; v += 2; v.Push(3.14159f);
   cout << "v = " << v << endl;

   Vec vR = v.Reverse();
   cout << "v.Reverse() = " << vR << endl;

   Vec vv = v + vR;
   cout << "v + v.Reverse() = " << vv << endl;

   while (vv.Size() > 0)
      cout << vv.Pop() << " <- " << vv << endl;
   vv.Clear();

   cout << "v = " << v << endl;
}


int main()
{
   testImmutableArray();

   testVector();
   performanceTestVector();

   printf("Exiting main...\n");
   return 0;
}




