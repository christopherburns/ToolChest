#include <stdio.h>
#include <iostream>
#include <list>

#include "../Burns.h"

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
ostream& operator << (ostream& o, const Mutable::LinkedList<int>& c)   { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::LinkedList<int>& c) { STREAM_OUT_DEF }
//ostream& operator << (ostream& o, const Mutable::Array<int>& c) { STREAM_OUT_DEF }


// Produces a sorted list of the contents in list, N^2
template <class C, class E> Mutable::LinkedList<E> InsertionSort(const C& collection)
{
   typedef typename C::Iterator Iterator;

   Mutable::LinkedList<E> list;
   Iterator itr = collection.GetIterator();
   while (itr.HasNext())
   {
      auto item = itr.Next();
      Iterator lItr = list.GetIterator();
      while (lItr.HasNext() && lItr.Peek() < item) lItr.Next();
      list.Insert(lItr, item);
   }

   return list;
}


/////////////////////////
// Performance Testing //
/////////////////////////

/// Here we test the performance of the data structure by comparing with an 
/// equivalent stl data structure.  We compare:
///    list construction (a sequence of append operations)
///    fold-right operation (traversal-reduction)
///    list reversal


template <class C> void performanceTestMutableLinkedList()
{
   typedef C List;
   typedef std::list<int> STLList;

   /* 130953 crashes, 130592 works...*/
   const int N = 20000;
   const int R = 5000;

   srand(1001938110);
   int * pool = new int[N];
   for (int i = 0; i < N; ++i) pool[i] = rand(); 

   const int MINE = 0;
   const int THEIRS = 1;

   StopWatch watch;
   double insertions[2], sort[2];

   ///////////////////////
   // List Construction //
   ///////////////////////

   /// Build some initial lists
   typename List::Builder builder(N);
   for (int i = 0; i < N; ++i) builder.AddElement(pool[i]);
   List l = builder.Result();

   STLList stlList;
   for (int i = 0; i < N; ++i) stlList.push_front(pool[i]);

   ////////////////
   // Insertions //
   ////////////////

   watch.Start();
   for (int i = 0; i < R; ++i)
   {
      /// insert value pool[i] at location pool[i] % N;
      int location = pool[i] % l.Size();
      typename List::Iterator itr = l.GetIterator();

      /// advance to position
      while (location > 0) { assert(itr.HasNext()); itr.Next(); location--; }

      /// perform insertion
      l.Insert(itr, pool[i]);
   }
   watch.Stop();
   insertions[MINE] = watch.ReadTime().ToMilliseconds();


   watch.Start();
   for (int i = 0; i < R; ++i)
   {
      int location = pool[i] % stlList.size();
      STLList::iterator itr = stlList.begin();
      while (location > 0) { ++itr; --location; }
      stlList.insert(itr, pool[i]);
   }
   watch.Stop();
   insertions[THEIRS] = watch.ReadTime().ToMilliseconds();


   ///////////
   // Sorts //
   ///////////

   watch.Start();
   InsertionSort<Mutable::LinkedList<int>, int>(l);
   watch.Stop();
   sort[MINE] = watch.ReadTime().ToMilliseconds();

   watch.Start();
   stlList.sort();
   watch.Stop();
   sort[THEIRS] = watch.ReadTime().ToMilliseconds();



   delete [] pool;

   // Report results...
   printf("                        Burns         STL   \n");
   printf("List Insertions:   %8.2f ms  %8.2f ms\n", (float)insertions[MINE], insertions[THEIRS]);
   printf("List Sort:         %8.2f ms  %8.2f ms\n", (float)sort[MINE], sort[THEIRS]);

}
template <> void performanceTestMutableLinkedList<Immutable::LinkedList<int> >()
{ }


template <class C> void performanceTestLinkedList()
{
   typedef C List;
   typedef std::list<int> STLList;

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

   ///////////////////////
   // List Construction //
   ///////////////////////

   watch.Start();
   typename List::Builder builder(N);
   for (int i = 0; i < N; ++i) builder.AddElement(pool[i]);
   List l = builder.Result();
   watch.Stop();
   constructionTimes[MINE] = watch.ReadTime().ToMilliseconds();

   watch.Start();
   STLList stlList;
   for (int i = 0; i < N; ++i) stlList.push_front(pool[i]);
   watch.Stop();
   constructionTimes[THEIRS] = watch.ReadTime().ToMilliseconds();


   ////////////////////
   // List Reduction //
   ////////////////////

   {
      watch.Start();
      float sum = 0;
      typename List::Iterator itr = l.GetIterator();
      while (itr.HasNext()) sum += 1.0f / float(itr.Next());
      watch.Stop();
      reductionTimes[MINE] = watch.ReadTime().ToMilliseconds();
   }

   {
      watch.Start();
      float stlSum = 0;
      for (STLList::iterator itr = stlList.begin(); itr != stlList.end(); ++itr)
         stlSum += 1.0f / (*itr);
      watch.Stop();
      reductionTimes[THEIRS] = watch.ReadTime().ToMilliseconds();
   }


   ///////////////////
   // List Reversal //
   ///////////////////

   {
      watch.Start();
      List lReverse = l.Reverse(); // makes a copy 
      watch.Stop();
      reversalTimes[MINE] = watch.ReadTime().ToMilliseconds();
   }

   {
      watch.Start();
      STLList listCopy = stlList;
      listCopy.reverse();   // in-place reverse
      watch.Stop();
      reversalTimes[THEIRS] = watch.ReadTime().ToMilliseconds();
   }




   delete [] pool;

   // Report results...
   printf("                        Burns         STL   \n");
   printf("List Construction:   %8.2f ms  %8.2f ms\n", (float)constructionTimes[MINE], constructionTimes[THEIRS]);
   printf("List Reduction:      %8.2f ms  %8.2f ms\n", (float)reductionTimes[MINE]   , reductionTimes[THEIRS]);
   printf("List Reversal:       %8.2f ms  %8.2f ms\n", (float)reversalTimes[MINE]    , reversalTimes[THEIRS]);

   performanceTestMutableLinkedList<C>();

   printf("Exiting profiling routine...\n");
}




template <class C> void testMutableList()
{
   srand(1001938110);

   typedef C List;
   List l; l += 1;

   const int R = 20;
   for (int i = 0; i < R; ++i)
   {
      /// insert value pool[i] at location pool[i] % N;
      const int v = rand() % l.Size();
      typename List::Iterator itr = l.GetIterator();
      for (int t = 0; t < v; ++t)
      { assert(itr.HasNext()); itr.Next(); }

      /// perform insertion
      //cout << "Insert " << v << " @ " << v << " ";
      l.Insert(itr, v);
      //cout << l << endl;
   }

   cout << "Testing mutable list" << endl;
   cout << "  List l = " << l << endl;

   cout << "  Sorted l = " << InsertionSort<Mutable::LinkedList<int>, int>(l) << endl;

   cout << "  Removing even values:" << endl;

   {
      auto itr = l.GetIterator();
      while (itr.HasNext())
      {
         if (itr.Peek() % 2 == 0) l.Remove(itr);
         else itr.Next();
      }
   }

   cout << "  Sans even = " << l << endl;
   cout << "  Removing odd values now... should result in empty list..." << endl;

   {
      auto itr = l.GetIterator();
      while (itr.HasNext())
      {
         if (itr.Peek() % 2 == 1) l.Remove(itr);
         else itr.Next();
      }
   }
   
   cout << "  Empty list = " << l << endl;
}

template <> void testMutableList<Immutable::LinkedList<int> >() {}

template <class C> void testList()
{
   ////////////////////////////////
   // Immutable LinkedList Tests //
   ////////////////////////////////

   typedef C List;
   int values[] = { 0, 10, 20, 30, 40, 50, 60, 70 };

   /// For the immutable list, we're going to try to focus on the ops that
   /// avoid replicating data and share nodes with previous versions of the 
   /// lists

   printf("Testing LinkedList Operations ...\n");

   printf("List::Construct...\n");
   List s = List::Construct(8, values);

   List s_ = s + 80;
   List s__ = s_ + 90;
   List _s = -1 + s;
   List __s = -2 + _s;

   {
      List s2 = s.Take(4).Append(400).Append(500).Append(600);
      List s3 = s.Take(4).Append(4000).Append(5000);
      List s4 = s.Drop(4).Prepend(300).Prepend(200);

      cout << "s2 = " << s2 << ", s2.Reverse() = " << s2.Reverse() << endl;   
      cout << "s3 = " << s3 << ", s3.Reverse() = " << s3.Reverse() << endl;
      cout << "s4 = " << s4 << ", s4.Reverse() = " << s4.Reverse() << endl;
   } // calls destructors

   cout << "s   = " << s << endl;
   cout << "s_  = " << s_ << endl;
   cout << "s__ = " << s__ << endl;
   cout << "_s  = " << _s << endl;
   cout << "__s = " << __s << endl;

   testMutableList<C>();
}

int main()
{
   testList<Immutable::LinkedList<int> >();
   testList<Mutable::LinkedList<int> >();
   performanceTestLinkedList<Immutable::LinkedList<int> >();
   performanceTestLinkedList<Mutable::LinkedList<int> >();

   printf("Exiting main...\n");
   return 0;
}




