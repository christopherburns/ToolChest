

#include <stdio.h>
#include <iostream>

#include "../burns/Burns.h"

using namespace std;
using namespace Burns;
using namespace Collections;

bool gt4(int i) { return i > 4; }
bool lt4(int i) { return i < 4; }
bool even(int i) { return i % 2 == 0; }
int generator(int i) { return i-1 + i-2; }

struct ScaleBy10
{
public:
   int operator() (const int& i) const { return i * 10; }
   float operator() (const float& f) const { return f * 10; }
};

class Generator { public: int operator() (int i) { return i-1 + i-2; } };
class Printer 
{ 
public: 
   void operator() (const float& f) const { printf("%.1f ", f); }
   void operator() (const int& i) const { printf("%i ", i); } 
   void operator() (Common::KeyValuePair<int, float> p) const 
   { printf("(%i, %.2f) ", p.key, p.value); }
};

template <class Container> void prependAndDestroy(const Container& a)
{
   Container b = a.Prepend(100);
}
template <class Container> Container aliasing(const Container& a)
{
   Container b = a.Prepend(100);
   Container c = a;
   c = b;
   return c.Prepend(100);
}

/// No virtual base class, and cannot add template args to operator<<
#define STREAM_OUT_DEF o << "[ "; Printer p; c.ForEach(p); o << "]"; return o;
ostream& operator << (ostream& o, const Collections::Vector<int>& c)          { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Mutable::Array<int>& c)               { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Mutable::LinkedList<int>& c)          { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Mutable::TreeSet<int>& c)             { STREAM_OUT_DEF }
//ostream& operator << (ostream& o, const Mutable::TreeMap<int, float>& c)      { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::Array<int>& c)             { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::Array<float>& c)           { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::LinkedList<int>& c)        { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::LinkedList<float>& c)      { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::TreeSet<int>& c)           { STREAM_OUT_DEF }
ostream& operator << (ostream& o, const Immutable::TreeMap<int, float>& c)    { STREAM_OUT_DEF }


template <class Container> void testTraversable()
{
   int values[] = { 0, 1, 2, 3, 5, 8, 13 };
   Container c = Container::Construct(7, values); 
   Container c0 = Container(); 
   Container c1 = c.Take(1);

   cout << "Size     (" << c << ")     -> " << c.Size() << endl;
   cout << "IsEmpty  (" << c << ")     -> " << c.IsEmpty() << endl;
   cout << "NonEmpty (" << c << ")     -> " << c.NonEmpty() << endl;
   cout << "Head     (" << c << ")     -> " << c.Head() << endl;
   cout << "Last     (" << c << ")     -> " << c.Last() << endl;
   cout << "Find     (" << c << ", >4) -> " << c.Find(gt4) << endl;  

   cout << endl;
   
   cout << "ForAll   (" << c << ", >4) -> " << (c.ForAll(gt4) ? "true" : "false") << endl;
   cout << "Exists   (" << c << ", >4) -> " << (c.Exists(gt4) ? "true" : "false") << endl;
   cout << "Count    (" << c << ", >4) -> " << c.Count(gt4) << endl;

   cout << endl;

   cout << "Filter   (" << c << ", >4) -> " << c.Filter(gt4) << endl;
   cout << "NotFilter(" << c << ", >4) -> " << c.FilterNot(gt4) << endl;

   cout << "Tail     (" << c << ")     -> " << c.Tail()   << endl;
   cout << "Init     (" << c << ")     -> " << c.Init()   << endl;
   cout << "Init     (" << c0 << ")    -> " << c0.Init()  << endl;
   cout << "Init     (" << c1 << ")    -> " << c1.Init()  << endl;
   cout << "Take     (" << c << ", 2)  -> " << c.Take(2)  << endl;
   cout << "Take     (" << c0 << ", 2) -> " << c0.Take(2) << endl;
   cout << "Take     (" << c1 << ", 2) -> " << c1.Take(2) << endl;

   //cout << "c:         "; c.Print();
   //cout << "c.Take(13) "; c.Take(13).Print();

   cout << "Take     (" << c << ", 13) -> " << c.Take(13) << endl;
   cout << "Drop     (" << c << ", 2)  -> " << c.Drop(2)  << endl;
   cout << "Drop     (" << c0 << ", 2) -> " << c0.Drop(2) << endl;
   cout << "Drop     (" << c1 << ", 2) -> " << c1.Drop(2) << endl;
   cout << "Drop     (" << c << ", 13) -> " << c.Drop(13) << endl;

   cout << "TakeWhile(" << c << ", <4) -> " << c.TakeWhile(lt4) << endl;
   cout << "DropWhile(" << c << ", <4) -> " << c.DropWhile(lt4) << endl;

   ScaleBy10 s;
   cout << "MapValues(" << c << ", ScaleBy10) -> " << MapValues<Container, int, ScaleBy10>(c, s) << endl;
   cout << "c -> " << c << endl;

   cout << endl;

   cout << endl;

   for (int i = 0; i <= c.Size(); ++i)   
   {
      Pair<Container, Container> split = c.SplitAt(i);
      cout << "SplitAt  (" << c << ", " << i << ") -> (" << split.first << ", " << split.second << ")" << endl;
   }

   cout << endl;

   Pair<Container, Container> oddEven = c.Partition(even);
   cout << "Partition(" << c << ", even) -> (" << oddEven.first << ", " << oddEven.second << ")" << endl;

   cout << endl;
}

template <class Container> void testSet()
{
   testTraversable<Container>();

   int oddValues[] = {1, 3, 5, 15, 7, 11, 13, 9};
   int evenValues[] = {2, 4, 6, 8, 10, 12, 14, 16};
   int straightValues[] = { 5, 7, 6, 9, 8 };
   Container oddSet  = Container::Construct(8, oddValues);
   Container evenSet = Container::Construct(8, evenValues);
   Container smallerEvenSet = Container::Construct(4, evenValues);
   Container straightSet = Container::Construct(5, straightValues);
   Container emptySet;

   (evenSet | oddSet).PrintGraph("graph.dot");

   cout << "Contains   (" << oddSet << ", -12)        -> " << oddSet.Contains(-12) << endl;
   cout << "Contains   (" << evenSet << ", 10)        -> " << evenSet.Contains(10) << endl;
   cout << "IsSubsetOf (" << smallerEvenSet << ", " << smallerEvenSet << ")  -> " << smallerEvenSet.IsSubsetOf(smallerEvenSet) << endl;
   cout << "IsSubsetOf (" << evenSet << ", " << smallerEvenSet << ")  -> " << evenSet.IsSubsetOf(smallerEvenSet) << endl;
   cout << "IsSubsetOf (" << smallerEvenSet << ", " << evenSet << ") -> " << smallerEvenSet.IsSubsetOf(evenSet) << endl;
   cout << "IsSubsetOf ( Take (" << evenSet << ", 3), " << evenSet << ") -> " << evenSet.Take(3).IsSubsetOf(evenSet) << endl;

   cout << "Union (" << evenSet << ", " << oddSet << ") -> " << evenSet.Union(oddSet) << endl;
   cout << "Union (" << oddSet << ", " << evenSet << ") -> " << oddSet.Union(evenSet) << endl;

   cout << "Intersection (" << evenSet     << ", " << oddSet  << ") -> " << (evenSet & oddSet) << endl;
   cout << "Intersection (" << straightSet << ", " << evenSet << ") -> " << (straightSet & evenSet) << endl;
   cout << "Intersection (" << straightSet << ", " << oddSet  << ") -> " << (straightSet & oddSet) << endl;
   cout << "Intersection (" << emptySet    << ", " << oddSet  << ") -> " << (emptySet & oddSet) << endl;

   cout << "Difference   (" << straightSet << ", " << evenSet     << ") -> " << (straightSet - evenSet) << endl;
   cout << "Difference   (" << straightSet << ", " << oddSet      << ") -> " << (straightSet - oddSet) << endl;
   cout << "Difference   (" << oddSet      << ", " << evenSet     << ") -> " << (oddSet - evenSet) << endl;
   cout << "Difference   (" << emptySet    << ", " << straightSet << ") -> " << (emptySet - straightSet) << endl;

   cout << "Insert (" << straightSet << ", -1) -> " << (straightSet.Insert(-1)) << endl;


   ScaleBy10 s;
   cout << "MapValues(" << evenSet << ", ScaleBy10) -> " << MapValues<Container, int, ScaleBy10>(evenSet, s) << endl;
   cout << "evenSet -> " << evenSet << endl;

   cout << endl;

   Container n = straightSet.Insert(-1).Insert(5).Insert(100);
   cout << "Insert ( Insert ( Insert (" << straightSet << ", -1), 5), 100) -> " << 
      n << endl;

   Container n1 = n.Remove(6);
   cout << "Remove (" << n << ", 6) -> " << n1 << endl;
   Container n2 = n1.Remove(400);
   cout << "Remove (" << n1 << ", 400) -> " << n2 << endl;
   Container n3 = n2.Remove(-1);
   cout << "Remove (" << n2 << ", -1) -> " << n3 << endl;

   cout << "Remove ( Remove ( Remove (" << n << ", 6), 400), -1) -> " << 
      n.Remove(6).Remove(400).Remove(-1) << endl;



   cout << "Building medium sized set of 10000 items, dumping graph..." << endl;
   const int N = 10000;
   typename Container::Builder bigBuilder;
   for (int i = 0; i < N; ++i)
      bigBuilder.AddElement(rand());
   Container c = bigBuilder.Result();

   c.PrintGraph("big_graph.dot");

   cout << endl;
}

template <class Container> void testMutableSet()
{
   testSet<Container>();

   int oddValues[] = {1, 3, 5, 15, 7, 11, 13, 9};
   int evenValues[] = {2, 4, 6, 8, 10, 12, 14, 16};
   int straightValues[] = { 5, 7, 6, 9, 8 };
   Container oddSet  = Container::Construct(8, oddValues);
   Container evenSet = Container::Construct(8, evenValues);
   Container smallerEvenSet = Container::Construct(4, evenValues);
   Container s = Container::Construct(5, straightValues);
   Container emptySet;

   cout << "Testing mutability..." << endl;
   cout << "  s = " << s << endl;
   cout << "  s += 5;  // s -> " << (s += 5) << endl;
   cout << "  (s += 100) += 500; // s -> " << ((s += 100) += 500) << endl;
   cout << "  s += " << smallerEvenSet << "; // s -> " << (s += smallerEvenSet) << endl;

   cout << "  s -= " << Container::Construct(5, straightValues) << "; // s -> " << (s -= Container::Construct(5, straightValues)) << endl;

   cout << "  s -= 500; // s -> " << (s -= 500) << endl;
   cout << "  s += 0; // s -> " << (s += 0) << endl;
   cout << "  s -= 1; // s -> " << (s -= 1) << endl;

   cout << endl;
}

void testVector()
{
   int values[] = { 0, 1, 2, 3, 5, 8, 13 };
   Collections::Vector<int> v = Collections::Vector<int>::Construct(7, values);

   cout << v << endl;
   v.Push(500); 
   v.Push(500); 
   v.Push(500); 
   cout << v << endl;
   v.Pop(); 
   v.Pop(); 
   cout << v << endl;
   cout << endl;
}



struct KeyPredicateIsEven
{
public:
   bool operator () (const int& key) const { return key % 2 == 0; }
} IsEven;


template <class Container> void testImmutableMap()
{
   const int N = 6;
   int keys[] = { 3, 5, 13, 1, 0, 8 };
   float values[] = {0.14f, 4.5f, 100.0f, 3.14159f, 90.0f };

   typedef typename Container::Builder Builder;

   Builder b = Builder();
   for (int i = 0; i < N; ++i)
      b.AddElement(Common::KeyValuePair<int, float>(keys[i], values[i]));
   Container c = b.Result();

   cout << "c = " << c << endl;  


   typename Container::SetType kArray = c.Keys();
   cout << "Keys() = " << kArray << endl;

   typedef Immutable::Array<float> MySeq;
   MySeq theValues = c.template Values<MySeq>();
   cout << "Values<Array>() = " << theValues << endl;
   
   typedef Immutable::LinkedList<float> MySeq2;
   MySeq2 theValues2 = c.template Values<MySeq2>();
   cout << "Values<LinkedList>() = " << theValues2 << endl;

   cout << "FilterKeys(IsEven)  (" << c << ")  -> " << c.FilterKeys(IsEven) << endl;

   cout << "Insert  (" << c << ", (20, 0.01))  -> " << c.Insert(20, 0.01)  << endl;
   cout << "Insert  (" << c << ", (40, 0.02))  -> " << c.Insert(40, 0.02)  << endl;
   cout << "Insert  (" << c << ", (5 , 1.01))  -> " << c.Insert(5 , 1.01)  << endl;
   cout << "Remove  (" << c << ", 2)           -> " << c.Remove(2)         << endl;
   cout << "Remove  (" << c << ", 31)          -> " << c.Remove(31)        << endl;                   

   cout << "Contains (" << c << ", 3)          -> " << c.Contains(3)       << endl;
   cout << "Contains (" << c << ", -1)         -> " << c.Contains(-1)      << endl;

   cout << "GetOrElse(" << c << ", 2, -1.0f)   -> " << c.GetOrElse(2, -1.0f) << endl;
   cout << "GetOrElse(" << c << ", 5, 0.0f)    -> " << c.GetOrElse(5,  0.0f) << endl;

   cout << endl; 
}

/*template <class Container> void testMutableMap()
{
   testImmutableMap<Mutable::TreeMap<int, float> >();
   
   const int N = 6;
   int keys[] = { 3, 5, 13, 1, 0, 8 };
   float values[] = {0.14f, 4.5f, 100.0f, 3.14159f, 90.0f };

   Container c;
   for (int i = 0; i < N; ++i)
      c += Common::KeyValuePair<int, float>(keys[i], values[i]);
   cout << "c = " << c << endl;
   cout << "c -= 13  : " << (c -= 13) << endl;
   cout << "c -= 8   : " << (c -= 8) << endl;
   cout << "c -= 100 : " << (c -= 100) << endl;
}*/


template <class Container> void testSequence()
{
   testTraversable<Container>();

   int values[] = { 0, 1, 2, 3, 5, 8, 13 };
   Container sequence = Container::Construct(7, values); 

   cout << "Append   (" << sequence << ", 20)      -> " << sequence.Append(20)     << endl;
   cout << "Prepend  (" << sequence << ", 20)      -> " << sequence.Prepend(20)    << endl;
   cout << "PadTo    (" << sequence << ", 10, 201) -> " << sequence.PadTo(10, 201) << endl;
   cout << "PadTo    (" << sequence << ", 2, 201)  -> " << sequence.PadTo(2, 201)  << endl;

   cout << endl;

   Container cGen0 = Container::Construct(10, generator);
   Container cGen1 = Container::Construct(10, Generator());
   
   cout << "Construct(10, generator)   -> " << cGen0 << endl;
   cout << "Construct(10, Generator()) -> " << cGen1 << endl;

   cout << endl;

   cout << "+(" << cGen0 << ", " << sequence << ") -> " << cGen0 + sequence << endl;


   Container b = sequence.Prepend(40);
   Container c = b.Prepend(50);
   Container d = 70 + (50 + (40 + sequence)) + 90;

   cout << "70 + (50 + (40 + " << sequence << ")) + 90 -> " << 70 + (50 + (40 + sequence)) + 90 << endl;

   cout << endl;

   prependAndDestroy(sequence);
   cout << "After Execution:" << endl;
   cout << "   template <class Container> void prependAndDestroy(const Container& a)\n"
        << "   {\n"
        << "      Container b = a.Prepend(100);\n"
        << "   }\n"
        << "a -> " << sequence << endl;


   cout << endl;

   Container q = aliasing(sequence);
   cout << "After Execution:" << endl
        << "   template <class Container> Container aliasing(const Container& a)\n"
        << "   {\n"
        << "      Container b = a.Prepend(100);\n"
        << "      Container c = a;\n"
        << "      c = b;\n"
        << "      return c.Prepend(100);\n"
        << "   } -> " << q << endl
        << "a -> " << sequence << endl;

}


void testMutableLinkedList()
{
   Mutable::LinkedList<int> list; 

   for (int i = 0; i < 5; ++i)
      list += i;

   cout << "list = " << list << endl;
   cout << "Reverse(" << list << ") -> " << list.Reverse() << endl;

   Mutable::LinkedList<int> a = list.Reverse() + 100;
   list = list.Reverse();
   list += 100;
   cout << "list.Reverse() + 100 -> " << a << endl;
   cout << "list.Reverse() += 100 -> " << list << endl;
}

template <class Container> void testEmpty()
{
   Container empty;
   
   Printer p;
   cout << "ForEach(printer):" << endl;
   empty.ForEach(p); cout << endl;

   cout << endl;
   cout << "Size()      = " << empty.Size() << endl;
   cout << "IsEmpty()   = " << empty.IsEmpty() << endl;
   cout << "NonEmpty()  = " << empty.NonEmpty() << endl;
   //cout << "Head()      = " << empty.Head() << endl;
   //cout << "Last()      = " << empty.Last() << endl;
   try
   {
      cout << "Find(> 4)   = " << empty.Find(gt4) << endl;
   }
   catch (NoElementFoundException e)
   { cout << "   NoElementFoundException" << endl; }

   cout << endl;
   cout << "ForAll(> 4) = " << empty.ForAll(gt4) << endl;
   cout << "Exists(> 4) = " << empty.Exists(gt4) << endl;
   cout << "Count(> 4)  = " << empty.Count(gt4) << endl;
   
   cout << endl;
   cout << "Filter(gt4):" << endl;
   empty.Filter(gt4).ForEach(p); cout << endl;
   cout << "NotFilter(gt4):" << endl;
   empty.FilterNot(gt4).ForEach(p); cout << endl;
   
   cout << "Tail():" << endl;
   empty.Tail().ForEach(p); cout << endl;
   cout << "Init():" << endl;
   empty.Init().ForEach(p); cout << endl;
   cout << "Take(2):" << endl;
   empty.Take(2).ForEach(p); cout << endl;
   cout << "Drop(2):" << endl;
   empty.Drop(2).ForEach(p); cout << endl;
   cout << "Take(13):" << endl;
   empty.Take(13).ForEach(p); cout << endl;
   cout << "Drop(13):" << endl;
   empty.Drop(13).ForEach(p); cout << endl;

   cout << "TakeWhile(lt4):" << endl;
   empty.TakeWhile(lt4).ForEach(p); cout << endl;
   cout << "DropWhile(lt4)):" << endl;
   empty.DropWhile(lt4).ForEach(p); cout << endl;
   
   cout << endl << "Testing SplitAt( ) ..." << endl;
   for (int i = 0; i <= empty.Size(); ++i)   
   {
      cout << "   SplitAt(" << i << "):" << endl;
      Pair<Container, Container> split = empty.SplitAt(i);
      cout << "   ";
      split.first.ForEach(p); cout << "| "; split.second.ForEach(p); cout << endl;
   }
      
   cout << "Partition(even):" << endl;
   Pair<Container, Container> oddEven = empty.Partition(even);
   oddEven.first.ForEach(p); cout << "| "; oddEven.second.ForEach(p); cout << endl;

   cout << "Append(20):" << endl;
   empty.Append(20).ForEach(p); cout << endl;
   cout << "Prepend(20):" << endl;
   empty.Prepend(20).ForEach(p); cout << endl;
   cout << "PadTo(10, 20):" << endl;
   empty.PadTo(10, 20).ForEach(p); cout << endl;
   cout << "PadTo(2, 20):" << endl;
   empty.PadTo(2, 20).ForEach(p); cout << endl;
   
   cout << "Array::Construct(10, generator):" << endl;
   srand(148797098);
   Container::Construct(10, generator).ForEach(p); cout << endl;
   cout << "Array::Construct(10, Generator()):" << endl;
   Container::Construct(10, Generator()).ForEach(p); cout << endl;
   
   cout << "Concatenation..." << endl;
   Container first = Container::Construct(10, Generator());
   Container second = empty + first + empty;
   second.ForEach(p); cout << endl;
   first.ForEach(p); cout << endl;
   
   cout << "Prepend Again..." << endl;
   Container a;
   Container b = a.Prepend(40);
   Container c = b.Prepend(50);
   Container d = 70 + (50 + (40 + a)) + 90;
   prependAndDestroy(a);
   a.ForEach(p); cout << endl;
   b.ForEach(p); cout << endl;
   c.ForEach(p); cout << endl;
   d.ForEach(p); cout << endl;

   cout << "Complete. Press any key and Return." << endl;
}

int main()
{
   #if 0
   cout << "Testing Mutable::Array<int> ..." << endl;
   cout << "----------------------" << endl << endl;
   testSequence<Mutable::Array<int> >(); cout << endl;

   cout << "Testing Immutable::Array<int> ..." << endl;
   cout << "---------------------------------" << endl << endl;
   testSequence<Immutable::Array<int> >(); cout << endl;

   cout << "Testing Immutable::LinkedList<int> ..." << endl;
   cout << "---------------------------" << endl << endl;
   testSequence<Immutable::LinkedList<int> >(); cout << endl;

   cout << "Testing Mutable::LinkedList<int> ..." << endl;
   cout << "---------------------------" << endl << endl;
   testSequence<Mutable::LinkedList<int> >(); cout << endl;
   testMutableLinkedList(); cout << endl;


   cout << "Testing Immutable::TreeSet<int> ..." << endl;
   cout << "---------------------------" << endl << endl;
   testSet<Immutable::TreeSet<int> >(); cout << endl;

   //cout << "Testing Mutable::TreeSet<int> ..." << endl;
   //cout << "---------------------------" << endl << endl;
   //testMutableSet<Mutable::TreeSet<int> >(); cout << endl;


   cout << "Testing Vector<int> ..." << endl;
   cout << "---------------------------" << endl << endl;
   testVector();

   cout << "Testing Immutable::TreeMap<int, float> ..." << endl;
   cout << "---------------------------" << endl << endl;
   testImmutableMap<Immutable::TreeMap<int, float> >(); cout << endl;
#endif
   //cout << "Testing Mutable::TreeMap<int, float> ..." << endl;
   //cout << "---------------------------" << endl << endl;
   //testMutableMap<Mutable::TreeMap<int, float> >(); cout << endl;

   //cout << "Testing Empty Mutable::Array<int> ..." << endl;
   //cout << "----------------------------" << endl << endl;
   //testEmpty<Mutable::Array<int> >(); cout << endl;
//
   cout << "Testing Empty LinkedList<int> ..." << endl;
   cout << "----------------------------" << endl << endl;
   testEmpty<Immutable::LinkedList<int> >(); cout << endl;
   
   int i;
   cin >> i;
   cout << i;
   return 0;
}
