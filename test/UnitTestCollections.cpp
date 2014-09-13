#include <stdio.h>
#include <iostream>

#include <ToolChest.h>

using namespace std;
using namespace ToolChest;
using namespace Collections;

///////////////////////
// Helper Constructs //
///////////////////////

bool gt4(int i) { return i > 4; }
bool lt4(int i) { return i < 4; }
int generator(int i) { return i-1 + i-2; }

template <class T> int ToKey(const T& t);
template <> int ToKey(const int& t) { return t; }
template <> int ToKey<Common::KeyValuePair<int, float> >(const Common::KeyValuePair<int, float>& t) { return t.key; }

auto isNonNegative  = [] (int i) -> bool { return i >= 0; };
auto isNegative     = [] (int i) -> bool { return i < 0; };
auto alwaysSucceeds = [] (int i) -> bool { return true; };
auto alwaysFails    = [] (int i) -> bool { return false; };
auto isEven         = [] (int i) -> bool { return i % 2 == 0; };
auto isOdd          = [] (int i) -> bool { return i % 2 != 0; };

auto keyIsNonNegative  = [] (Common::KeyValuePair<int, float> i) -> bool { return i.key >= 0; };
auto keyIsNegative     = [] (Common::KeyValuePair<int, float> i) -> bool { return i.key < 0; };
auto keyAlwaysSucceeds = [] (Common::KeyValuePair<int, float> i) -> bool { return true; };
auto keyAlwaysFails    = [] (Common::KeyValuePair<int, float> i) -> bool { return false; };
auto keyIsEven         = [] (Common::KeyValuePair<int, float> i) -> bool { return i.key % 2 == 0; };
auto keyIsOdd          = [] (Common::KeyValuePair<int, float> i) -> bool { return i.key % 2 != 0; };


/// Templated routines for converting integers into the appropriate thing
/// that goes into a given container (i.e. a key-value pair perhaps). This
/// is simply so we can use the same unit tests for Maps as for Sets/Sequences,
/// mostly. If i think of another use I'll let you know.
template <class T> typename T::ElementType ToElement(int i) { return i; }
template <> typename Immutable::TreeMap<int, float>::ElementType 
ToElement<Immutable::TreeMap<int, float> >(int i) 
{ return Common::KeyValuePair<int, float>(i, i * 0.5f); }
//template <> typename Mutable::TreeMap<int, float>::ElementType 
//ToElement<Mutable::TreeMap<int, float> >(int i) 
//{ return Common::KeyValuePair<int, float>(i, i * 0.5f); }



struct ScaleBy10
{
public:
   int operator() (const int& i) const { return i * 10; }
   float operator() (const float& f) const { return f * 10; }
};


template <class T> class Generator { public: int operator() (int i) { return i-1 + i-2; } };

class Printer 
{ 
public: 
   void operator() (const float& f) const { printf("%.1f ", f); }
   void operator() (const int& i) const { printf("%i ", i); } 
   void operator() (Common::KeyValuePair<int, float> p) const 
   { printf("(%i, %.2f) ", p.key, p.value); }
};


template <class T> struct ToString { constexpr static const char * const s = ""; };

template <> struct ToString<Immutable::Array<int> >           { constexpr static const char * const value = "Immutable::Array<int>"; };
template <> struct ToString<Mutable::Array<int>   >           { constexpr static const char * const value = "Mutable::Array<int>"; };
template <> struct ToString<Immutable::LinkedList<int> >      { constexpr static const char * const value = "Immutable::LinkedList<int>"; };
template <> struct ToString<Mutable::LinkedList<int>   >      { constexpr static const char * const value = "Mutable::LinkedList<int>"; };
template <> struct ToString<Immutable::TreeSet<int> >         { constexpr static const char * const value = "Immutable::TreeSet<int>"; };
template <> struct ToString<Mutable::TreeSet<int>   >         { constexpr static const char * const value = "Mutable::TreeSet<int>"; };
template <> struct ToString<Immutable::TreeMap<int, float> >  { constexpr static const char * const value = "Immutable::TreeMap<int, float>"; };
template <> struct ToString<Mutable::TreeMap<int, float> >    { constexpr static const char * const value = "Mutable::TreeMap<int, float>"; };


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

template <class T> bool IsEqual(const T& t1, const T& t2)
{
   if (t1.Size() != t2.Size()) return false;
   auto itr1 = t1.GetIterator(), itr2 = t2.GetIterator();
   while (itr1.HasNext()) 
      if (itr1.Next() != itr2.Next()) return false;
   return true;
}

/*template <> bool IsEqual(const Mutable::LinkedList<int>& t1, const Mutable::LinkedList<int>& t2)
{
   if (t1.Size() != t2.Size()) { printf("hi!\n"); return false; }
   auto itr1 = t1.GetIterator(), itr2 = t2.GetIterator();
   while (itr1.HasNext()) 
   {
      auto a = itr1.Next(), b = itr2.Next();
      printf("%i == %i\n", a, b);
      if (a != b) return false;
   }
   return true;
}*/

///////////////////////////////////////////////////////////////////////////////
//                            Traversable Unit Tests                         //
///////////////////////////////////////////////////////////////////////////////

// All unit functions take a container implementing Traversable, return a bool
// indicating pass or fail

template <class T> bool Test_ForEach()
{
   const int N = 12;
   auto t = T::Construct(N, Generator<T>());
   auto tm = MapValues<T, int, ScaleBy10>(t, ScaleBy10());

   if (tm.Size() != t.Size() || tm.Size() != N) return false;

   auto itrA = t.GetIterator();
   auto itrB = tm.GetIterator();
   while (itrA.HasNext()) if (itrB.Next() != (10*itrA.Next())) return false;

   return true;
}

template <class T> bool Test_Find()
{
   const int N = 12;
   auto t = T::Construct(N, Generator<T>());

   try { if (t.Find ( isOdd ) != -3) return false; }
   catch (NoElementFoundException e) { return false; }

   try                               { auto i = t.Find(isEven); }
   catch (NoElementFoundException e) { return true; }

   return false;
}

template <class T> bool Test_Head()
{
   const int N = 8;
   const typename T::ElementType values[N] = { 
      0, 1, 2, 3, 9, 10, 11, 12 
   };
   
   for (int i = 0; i < N; ++i)
      if (T::Construct(N-i, values+i).Head() != values[i]) return false;
   return true;
}

template <class T> bool Test_Last()
{
   const int N = 8;
   const typename T::ElementType values[N] = { 0, 1, 2, 3, 9, 10, 11, 12 };
   
   for (int i = 0; i < N; ++i)
   {
      auto t = T::Construct(N-i, values);
      if (t.Last() != values[N-i-1]) return false;
   }
   return true;
}

template <class T> bool Test_Tail()
{
   const int N = 8;
   const typename T::ElementType values[N] = { 0, 1, 2, 3, 9, 10, 11, 12 };
   
   for (int i = 0; i < N; ++i)
   {
      auto t = T::Construct(N-i, values+i);
      auto tail = t.Tail();
      auto itr = tail.GetIterator();
      int j = 0;
      while (itr.HasNext()) 
      { 
         if (itr.Next() != values[i + j + 1]) return false; 
         j++; 
      }
   }

   /// Test the empty container
   if (T().Tail().Size() != 0) return false;

   return true;
}

template <class T> bool Test_Init()
{
   const int N = 8;
   const typename T::ElementType values[N] = { 0, 1, 2, 3, 9, 10, 11, 12 };
   
   for (int i = 0; i < N; ++i)
   {
      auto t = T::Construct(N-i, values+i);
      auto init = t.Init();

      auto itrA = t.GetIterator();
      auto itrB = init.GetIterator();

      if (init.Size() != t.Size()-1) return false;
      while (itrB.HasNext()) if (itrA.Next() != itrB.Next()) return false;
   }

   /// Test the empty container
   if (T().Init().Size() != 0) return false;

   return true;
}

template <class T> bool Test_Take()
{
   const int N = 8;
   const typename T::ElementType values[N] = { 0, 1, 2, 3, 9, 10, 11, 12 };
   auto t = T::Construct(N, values);

   for (int i = 0; i < N; ++i)
   {
      auto take = t.Take(i);
      if (take.Size() != i) return false;
      auto itrA = take.GetIterator();
      auto itrB = t.GetIterator();
      while (itrA.HasNext()) if (itrA.Next() != itrB.Next()) return false;
   }

   /// Test the empty container
   auto empty = t.Take(0);           if (empty.Size() != 0) return false;
   auto stillEmpty = empty.Take(10); if (stillEmpty.Size() != 0) return false;
   empty = empty.Take(10);           if (empty.Size() != 0) return false;  

   return true;
}

template <class T> bool Test_Drop()
{
   const int N = 8;
   const typename T::ElementType values[N] = { 0, 1, 2, 3, 9, 10, 11, 12 };
   auto t = T::Construct(N, values);

   for (int i = 0; i <= N; ++i)
   {
      auto drop = t.Drop(i);
      if (drop.Size() != N-i) return false;
      auto itrA = t.GetIterator();
      auto itrB = drop.GetIterator();

      for (int j = 0; j < i; ++j) itrA.Next();
      while (itrA.HasNext()) if (itrA.Next() != itrB.Next()) return false;
   }
   return true;
}

template <class T> bool Test_TakeWhile()
{
   typedef typename T::ElementType E;
   const int N = 9;
   const E values[N] = { -1, 0, 1, 2, 3, 9, 10, 11, 12 };
   auto t = T::Construct(N, values);

   auto takeWhile = t.TakeWhile( [] (E i) -> bool { return i < 9; } );
   if (takeWhile.Size() != 5) return false;

   auto itr = takeWhile.GetIterator(); int i = 0;
   while (itr.HasNext()) if (itr.Next() != values[i++]) return false;

   return true;
}

template <class T> bool Test_DropWhile()
{
   typedef typename T::ElementType E;
   const int N = 9;
   const E values[N] = { -1, 0, 1, 2, 3, 9, 10, 11, 12 };
   auto t = T::Construct(N, values);

   auto lessThanThree = [] (E i) -> bool { return i < 3; };
   auto dropWhile = t.DropWhile( lessThanThree );
   if (dropWhile.Size() != 5) return false;

   auto itr = dropWhile.GetIterator(); 
   while (itr.HasNext()) if (lessThanThree(itr.Next())) return false;

   return true;
}


template <class T> bool Test_Filter()
{
   const int N = 8;
   int values[N] = { 0, 1, 2, 3, 4, 5, 6, 7 };
   auto t = T::Construct(N, values);

   auto tOdd = t.Filter(isOdd);    if (tOdd.Size() != 4) return false;
   auto tEven = t.Filter(isEven);  if (tEven.Size() != 4) return false;

   {
      auto itr = tOdd.GetIterator(); int i = 0;
      while (itr.HasNext()) if (itr.Next() != values[i++ * 2+1]) return false;
   }
   {
      auto itr = tEven.GetIterator(); int i = 0;
      while (itr.HasNext()) if (itr.Next() != values[i++ * 2]) return false;
   }

   if (tOdd.Filter(isEven).Size() != 0) return false;
   if (T().Filter(isOdd).Size() != 0) return false;
   return true;
}

template <class T> bool Test_Filter_Map()
{
   typedef typename T::ElementType E;
   const int N = 8;
   E values[N] = { 0, 1, 2, 3, 4, 5, 6, 7 };
   auto t = T::Construct(N, values);

   auto keyIsEven = [] (E i) -> bool { return i.key % 2 == 0; };
   auto keyIsOdd  = [] (E i) -> bool { return i.key % 2 != 0; };

   auto tOdd = t.Filter(keyIsOdd);    if (tOdd.Size() != 4) return false;
   auto tEven = t.Filter(keyIsEven);  if (tEven.Size() != 4) return false;

   {
      auto itr = tOdd.GetIterator(); int i = 0;
      while (itr.HasNext()) if (itr.Next() != values[i++ * 2+1]) return false;
   }
   {
      auto itr = tEven.GetIterator(); int i = 0;
      while (itr.HasNext()) if (itr.Next() != values[i++ * 2]) return false;
   }

   if (tOdd.Filter(keyIsEven).Size() != 0) return false;
   if (T().Filter(keyIsOdd).Size() != 0) return false;
   return true;
}

template <class T> bool Test_SplitAt()
{
   typedef typename T::ElementType E;
   const int N = 8;
   const E values[N] = { 0, 1, 2, 3, 4, 5, 6, 7 };
   auto t = T::Construct(N, values);

   for (int i = 0; i <= t.Size(); ++i)   
   {
      auto split = t.SplitAt(i);
      if (split.first.Size() != i) return false;
      if (split.second.Size() != t.Size()-i) return false;

      auto itr = split.first.GetIterator();
      for (int j = 0; j < i; j++) if (itr.Next() != values[j]) return false;
      itr = split.second.GetIterator();
      for (int j = i; j < N; j++) if (itr.Next() != values[j]) return false;
   }

   auto emptySplit = T().SplitAt(3);
   if (emptySplit.first.Size()  != 0) return false;
   if (emptySplit.second.Size() != 0) return false;

   return true;
}


template <class T> bool Test_Partition()
{
   typedef typename T::ElementType E;
   const int N = 12;
   const E values[N] = { -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6 };
   
   {
      auto p = T::Construct(N, values).Partition(isNonNegative);
      {
         auto itr = p.first.GetIterator(); 
         while (itr.HasNext()) if (!isNonNegative(itr.Next())) return false;
      }
      {
         auto itr = p.second.GetIterator(); 
         while (itr.HasNext()) if (isNonNegative(itr.Next())) return false;
      }
   }

   {  
      auto p = T::Construct(N, values).Partition(isEven);
      {
         auto itr = p.first.GetIterator(); 
         while (itr.HasNext()) if (!isEven(itr.Next())) return false;
      }
      {
         auto itr = p.second.GetIterator(); 
         while (itr.HasNext()) if (isEven(itr.Next())) return false;
      }
   }


   if (T().Partition(isNonNegative).first.Size()  != 0) return false;
   if (T().Partition(isNonNegative).second.Size() != 0) return false;

   return true;
}

template <class T> bool Test_Partition_Map()
{
   typedef typename T::ElementType E;
   const int N = 12;
   const E values[N] = { -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6 };
   
   {
      auto p = T::Construct(N, values).Partition(keyIsNonNegative);
      {
         auto itr = p.first.GetIterator(); 
         while (itr.HasNext()) if (!keyIsNonNegative(itr.Next())) return false;
      }
      {
         auto itr = p.second.GetIterator(); 
         while (itr.HasNext()) if (keyIsNonNegative(itr.Next())) return false;
      }
   }

   {  
      auto p = T::Construct(N, values).Partition(keyIsEven);
      {
         auto itr = p.first.GetIterator(); 
         while (itr.HasNext()) if (!keyIsEven(itr.Next())) return false;
      }
      {
         auto itr = p.second.GetIterator(); 
         while (itr.HasNext()) if (keyIsEven(itr.Next())) return false;
      }
   }


   if (T().Partition(keyIsNonNegative).first.Size()  != 0) return false;
   if (T().Partition(keyIsNonNegative).second.Size() != 0) return false;

   return true;
}

template <class T> bool Test_ForAll()
{
   typedef typename T::ElementType E;
   const int N = 8;
   const E values[N] = { 0, 1, 2, 3, 4, 5, 6, 7 };
   auto t = T::Construct(N, values);
   
   if (!t.ForAll(isNonNegative)) return false;
   if (t.ForAll(isEven)) return false;

   if (!T().ForAll(isEven)) return false;
   if (!T().ForAll(alwaysFails)) return false;

   return true;
}

template <class T> bool Test_ForAll_Map()
{
   typedef typename T::ElementType E;
   const int N = 8;
   const E values[N] = { 0, 1, 2, 3, 4, 5, 6, 7 };
   auto t = T::Construct(N, values);
   
   if (!t.ForAll(keyIsNonNegative)) return false;
   if (t.ForAll(keyIsEven)) return false;

   if (!T().ForAll(keyIsEven)) return false;
   if (!T().ForAll(keyAlwaysFails)) return false;

   return true;
}


template <class T> bool Test_Exists()
{
   typedef typename T::ElementType E;
   const int N = 8;
   const E values[N] = { 0, 1, 2, 3, 4, 5, 6, 7 };
   auto t = T::Construct(N, values);
   
   if (!t.Exists([] (E i) -> bool { return i >= 0; })) return false;
   if (t.Exists ([] (E i) -> bool { return i < 0; })) return false;

   if (T().Exists([] (E i) -> bool { return i < 0; })) return false;
   if (T().Exists([] (E i) -> bool { return true; })) return false;

   return true;
}


template <class T> bool Test_Count()
{
   const int N = 8;
   const int values[N] = { 0, 1, 2, 3, 4, 5, 6, 7 };
   auto t = T::Construct(N, values);

   bool r = true
      && t.Count(isOdd) == 4
      && t.Count(isEven) == 4
      && t.Count(alwaysSucceeds) == t.Size()
      && t.Count(alwaysFails) == 0 
      && T().Count(alwaysSucceeds) == 0
      && T().Count(isEven) == 0;

   return r;
}

template <class T> bool Test_Count_Map()
{
   typedef typename T::ElementType E;
   const int N = 8;
   const E values[N] = { 0, 1, 2, 3, 4, 5, 6, 7 };
   auto t = T::Construct(N, values);

   bool r = true
      && t.Count(keyIsOdd) == 4
      && t.Count(keyIsEven) == 4
      && t.Count(keyAlwaysSucceeds) == t.Size()
      && t.Count(keyAlwaysFails) == 0 
      && T().Count(keyAlwaysSucceeds) == 0
      && T().Count(keyIsEven) == 0;

   return r;
}

template <class T> bool Test_CountWhile()
{
   typedef typename T::ElementType E;
   const int N = 8; const E values[N] = { 0, 1, 2, 3, 4, 5, 6, 7 };
   auto t = T::Construct(N, values);

   for (int i = 0; i < N; ++i)
      if (t.CountWhile( [=] (E a) -> bool { return a < values[i]; } ) != i)
         return false;

   return true;
}

template <class T> bool Test_Copy()
{
   typedef typename T::ElementType E;
   const int N = 8;
   const E values[N] = { 0, 1, 2, 3, 4, 5, 6, 7 };

   int i = 0;
   auto itr = T::Construct(N, values).Copy().GetIterator(); 
   while (itr.HasNext()) if (itr.Next() != values[i++]) return false;

   return true;
}

template <class T> bool Test_Traversable()
{
   bool b = true;
   cout << "Test_ForEach<"    << ToString<T>::value << "> ... " << ( (b &= Test_ForEach<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Find<"       << ToString<T>::value << "> ... " << ( (b &= Test_Find<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Head<"       << ToString<T>::value << "> ... " << ( (b &= Test_Head<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Last<"       << ToString<T>::value << "> ... " << ( (b &= Test_Last<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Take<"       << ToString<T>::value << "> ... " << ( (b &= Test_Take<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Drop<"       << ToString<T>::value << "> ... " << ( (b &= Test_Drop<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Tail<"       << ToString<T>::value << "> ... " << ( (b &= Test_Tail<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Init<"       << ToString<T>::value << "> ... " << ( (b &= Test_Init<T>()) ? "Passed" : "FAILED") << endl;
 
   cout << "Test_TakeWhile<"  << ToString<T>::value << "> ... " << ( (b &= Test_TakeWhile<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_DropWhile<"  << ToString<T>::value << "> ... " << ( (b &= Test_DropWhile<T>()) ? "Passed" : "FAILED") << endl;
 
   cout << "Test_Filter<"     << ToString<T>::value << "> ... " << ( (b &= Test_Filter<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_SplitAt<"    << ToString<T>::value << "> ... " << ( (b &= Test_SplitAt<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Partition<"  << ToString<T>::value << "> ... " << ( (b &= Test_Partition<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_ForAll<"     << ToString<T>::value << "> ... " << ( (b &= Test_ForAll<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Exists<"     << ToString<T>::value << "> ... " << ( (b &= Test_Exists<T>()) ? "Passed" : "FAILED") << endl;

   cout << "Test_Count<"      << ToString<T>::value << "> ... " << ( (b &= Test_Count<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_CountWhile<" << ToString<T>::value << "> ... " << ( (b &= Test_CountWhile<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Copy<"       << ToString<T>::value << "> ... " << ( (b &= Test_Copy<T>()) ? "Passed" : "FAILED") << endl;
   return b;
}

template <class T> bool Test_TraversableMap()
{
   bool b = true;
   //cout << "Test_ForEach<"    << ToString<T>::value << "> ... " << ( (b &= Test_ForEach<T>()) ? "Passed" : "FAILED") << endl;
   //cout << "Test_Find<"       << ToString<T>::value << "> ... " << ( (b &= Test_Find<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Head<"       << ToString<T>::value << "> ... " << ( (b &= Test_Head<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Last<"       << ToString<T>::value << "> ... " << ( (b &= Test_Last<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Take<"       << ToString<T>::value << "> ... " << ( (b &= Test_Take<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Drop<"       << ToString<T>::value << "> ... " << ( (b &= Test_Drop<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Tail<"       << ToString<T>::value << "> ... " << ( (b &= Test_Tail<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Init<"       << ToString<T>::value << "> ... " << ( (b &= Test_Init<T>()) ? "Passed" : "FAILED") << endl;
 
   cout << "Test_TakeWhile<"  << ToString<T>::value << "> ... " << ( (b &= Test_TakeWhile<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_DropWhile<"  << ToString<T>::value << "> ... " << ( (b &= Test_DropWhile<T>()) ? "Passed" : "FAILED") << endl;
 
   cout << "Test_Filter<"     << ToString<T>::value << "> ... " << ( (b &= Test_Filter_Map<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_SplitAt<"    << ToString<T>::value << "> ... " << ( (b &= Test_SplitAt<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Partition<"  << ToString<T>::value << "> ... " << ( (b &= Test_Partition_Map<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_ForAll<"     << ToString<T>::value << "> ... " << ( (b &= Test_ForAll_Map<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Exists<"     << ToString<T>::value << "> ... " << ( (b &= Test_Exists<T>()) ? "Passed" : "FAILED") << endl;

   cout << "Test_Count<"      << ToString<T>::value << "> ... " << ( (b &= Test_Count_Map<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_CountWhile<" << ToString<T>::value << "> ... " << ( (b &= Test_CountWhile<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Copy<"       << ToString<T>::value << "> ... " << ( (b &= Test_Copy<T>()) ? "Passed" : "FAILED") << endl;
   return b;
}


///////////////////////////////////////////////////////////////////////////////
//                             Sequence Unit Tests                           //
///////////////////////////////////////////////////////////////////////////////



template <class T> bool Test_SquareBrackets()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);

   for (int i = 0; i < N; ++i) if (t[i] != values[i]) return false;
   return true;
}

template <class T> bool Test_IndexOf()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);
   for (int i = 0; i < N; ++i) if (t.IndexOf(values[i]) != i) return false;
   if (t.IndexOf(-1) != -1) return false;
   return true;
}

template <class T> bool Test_IndexWhere()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);

   if (t.IndexWhere( isNegative ) != 5) return false;
   if (t.IndexWhere( alwaysSucceeds ) != 0) return false;
   if (t.IndexWhere( alwaysFails ) != -1) return false;
   if (T().IndexWhere(alwaysSucceeds) != -1) return false;

   return true;
}

template <class T> bool Test_Append()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };

   auto t = T();
   for (int i = 0; i < N; ++i) t = t.Append(values[i]);
   if (t.Size() != N) return false;

   auto itr = t.GetIterator(); int i = 0;
   while (itr.HasNext()) if (itr.Next() != values[i++]) return false;

   return true;
}

template <class T> bool Test_Prepend()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };

   auto t = T();
   for (int i = 0; i < N; ++i) t = t.Prepend(values[N-i-1]);
   if (t.Size() != N) return false;

   auto itr = t.GetIterator(); int i = 0;
   while (itr.HasNext()) if (itr.Next() != values[i++]) return false;

   return true;
}

template <class T> bool Test_PadTo()
{
   for (int i = 0; i < 10; ++i)
      if (T().PadTo(i, 100).Size() != i) return false;

   if (T().PadTo(5, 100).PadTo(10, 200).Size()  != 10) return false;
   if (T().PadTo(5, 100).PadTo(5, 200) .Size() != 5) return false;

   auto t = T().PadTo(5, 100).PadTo(5, 200);
   if (t.Count( [] (int i) -> bool { return i == 100; } ) != t.Size()) return false;

   if (T().PadTo(-13, 99).Size() != 0) return false;

   return true;
}

template <class T> bool Test_Reverse()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t1 = T::Construct(N, values);
   auto t2 = t1.Reverse();
   auto t3 = t1.Reverse().Reverse();

   if (t1.Size() != t2.Size() || t1.Size() != t3.Size()) return false;

   auto itr1 = t1.GetIterator(), itr3 = t3.GetIterator();
   while (itr1.HasNext()) if (itr1.Next() != itr3.Next()) return false;

   if (T().Reverse().Size() != 0) return false;

   return true;
}

template <class T> bool Test_Contains()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);

   bool b = true;
   for (int i = 0; i < N; ++i) b &= t.Contains(values[i]);
   if (!b) return false;

   if (t.Contains(-1)) return false;
   if (T().Contains(100)) return false;

   return true;
}

template <class T> bool Test_OperatorPlus()
{
   const int N = 8; 
   const int valuesA[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   const int valuesB[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };

   auto t = T::Construct(N, valuesA);
   
   {
      auto t1 = t + T(); 
      if (t1.Size() != t.Size()) return false;
      auto itr = t.GetIterator(), itr1 = t1.GetIterator();
      while (itr.HasNext()) if (itr.Next() != itr1.Next()) return false;
   }
   {
      auto t1 = T() + t; 
      if (t1.Size() != t.Size()) return false;
      auto itr = t.GetIterator(), itr1 = t1.GetIterator();
      while (itr.HasNext()) if (itr.Next() != itr1.Next()) return false;
   }

   auto t1 = t + t;
   if (t1.Size() != t.Size() * 2) return false;
   auto itr = t1.GetIterator(); int i = 0;
   while (itr.HasNext()) if (itr.Next() != valuesA[i++ % N]) return false;

   return true;
}

template <class T> bool Test_Sequence()
{
   bool b = true;
   cout << "Test_SquareBrackets<" << ToString<T>::value << "> ... " << ( (b &= Test_SquareBrackets<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_IndexOf<"        << ToString<T>::value << "> ... " << ( (b &= Test_IndexOf       <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_IndexWhere<"     << ToString<T>::value << "> ... " << ( (b &= Test_IndexWhere    <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Append<"         << ToString<T>::value << "> ... " << ( (b &= Test_Append        <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Prepend<"        << ToString<T>::value << "> ... " << ( (b &= Test_Prepend       <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_PadTo<"          << ToString<T>::value << "> ... " << ( (b &= Test_PadTo         <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Reverse<"        << ToString<T>::value << "> ... " << ( (b &= Test_Reverse       <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Contains<"       << ToString<T>::value << "> ... " << ( (b &= Test_Contains      <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_OperatorPlus<"   << ToString<T>::value << "> ... " << ( (b &= Test_OperatorPlus  <T>()) ? "Passed" : "FAILED") << endl;
 
   return b;  
}


///////////////////////////////////////////////////////////////////////////////
//                          Mutable Sequence Tests                           //
///////////////////////////////////////////////////////////////////////////////


template <class T> bool Test_SquareBracketUpdate()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);

   for (int i = 0; i < N; ++i) 
   {
      t[i]++;
      if (t[i] != values[i]+1) return false;
   }

   for (int i = 0; i < N; ++i) t[i] = values[i];
   for (int i = 0; i < N; ++i) if (t[i] != values[i]) return false;

   return true;
}

template <class T> bool Test_DestructiveAppendElement()
{
   auto t = T(); int i = 0;
   while (t.Size() < 10)
   {
      t += i++;
      if (t.Size() != i) return false;
      if (t[i-1] != i-1) return false;
   }

   t = (T() += 5);
   if (t.Size() != 1) return false;
   if (t[0] != 5) return false;

   return true;
}

template <class T> bool Test_DestructiveAppendList()
{
   int v[] = {-1, -2, -3, 1, 2, 3};
   auto p = (((T() += -1) += -2) += -3);  if (p.Size() != 3) return false;
   auto q = (((T() +=  1) +=  2) +=  3);  if (q.Size() != 3) return false;

   p += q;

   int i = 0;
   auto itr = p.GetIterator();
   while (itr.HasNext()) if (itr.Next() != v[i++]) return false;

   return true;
}

template <class T> bool Test_DestructiveListInsert()
{
   const int N = 6; int v[N] = {-1, -2, -3, 1, 2, 3};
   auto t = T::Construct(N, v);

   {
      auto itr = t.GetIterator();
      while (itr.HasNext())
         if (itr.Next() == -3) { t.Insert(itr, 0); break; }
   }

   {
      auto itr = t.GetIterator();
      while (itr.HasNext())
         if (itr.Next() == 3) { t.Insert(itr, 0); break; }
   }

   auto itr = t.GetIterator();
   t.Insert(itr, 0);  //< Same as prepend

   if (t[0] != 0) return false;
   if (t[4] != 0) return false;
   if (t[8] != 0) return false;

   return true;
}

template <class T> bool Test_DestructiveListRemove()
{
   const int N1 = 9; int v1[N1] = {100, -1, -2, -3, 200, 1, 2, 3, 300};
   const int N2 = 6; int v2[N2] = {-1, -2, -3, 1, 2, 3};
   auto t = T::Construct(N1, v1);

   auto itr = t.GetIterator();
   while (itr.HasNext())
      if (itr.Peek() > 10) t.Remove(itr);
      else itr.Next(); 

   if (!IsEqual(T::Construct(N2, v2), t)) return false;


   itr = t.GetIterator();
   while (itr.HasNext()) t.Remove(itr);
   if (t.Size() > 0) return false;

   return true;
}

template <class T> bool Test_DestructiveSetInsertElement()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T();
   for (int i = 0; i < N; ++i)    t += values[i];
   for (int i = N-1; i >= 0; --i) t += values[i];
   if (!IsEqual(t, T::Construct(N, values))) return false;
   return true;
}       

template <class T> bool Test_DestructiveSetUnion()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);
   auto tPrime = T::Construct(N, values);

   if (!IsEqual( t, tPrime += t  )) return false;
   if (!IsEqual( t, tPrime += T())) return false;

   for (int i = 0; i < N; ++i) 
      if (!IsEqual( t, tPrime += T().Insert(values[i]))) return false;

   if (!IsEqual(T() += T(), T())) return false;

   ((tPrime += T().Insert(1)) += T().Insert(1)) += T().Insert(1);
   if (tPrime.Size() != t.Size() + 1) return false;
   if (!tPrime.Contains(1)) return false;

   return true;
}      

template <class T> bool Test_DestructiveSetRemove()
{
   const int N = 8; 
   const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   const int indices[N] = { 4, 5, 0, 7, 2, 6, 3, 1 };
   auto t = T::Construct(N, values);
   t -= 1; t -= 400;
   if (t.Size() != N) assert(false);

   for (int i = 0; i < N; ++i)
   {
      t -= values[indices[i]];
      if (t.Size() != N-i-1) assert(false);
      if (t.Contains(values[indices[i]])) assert(false);
   }

   auto empty = T(); empty -= 3;
   if (empty.Size() != 0) assert(false);
   return true;
}   

template <class T> bool Test_DestructiveSetDifference()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);
   if (!IsEqual(t -= T(), T::Construct(N, values))) return false;
   if (!IsEqual(t -= T::Construct(N, values), T())) return false;
   if (!IsEqual(t -= t, T())) return false;
   return true;
}       

template <class T> bool Test_MutableArray()
{
   bool b = true;
   cout << "Test_SquareBracketUpdate<" << ToString<T>::value << "> ... " << ( (b &= Test_SquareBracketUpdate<T>()) ? "Passed" : "FAILED") << endl;
   return b;
}

template <class T> bool Test_MutableLinkedList()
{
   bool b = true;
   cout << "Test_SquareBracketUpdate<"      << ToString<T>::value << "> ... " << ( (b &= Test_SquareBracketUpdate<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_DestructiveAppendElement<" << ToString<T>::value << "> ... " << ( (b &= Test_DestructiveAppendElement<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_DestructiveAppendList<"    << ToString<T>::value << "> ... " << ( (b &= Test_DestructiveAppendList<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_DestructiveListInsert<"    << ToString<T>::value << "> ... " << ( (b &= Test_DestructiveListInsert<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_DestructiveListRemove<"    << ToString<T>::value << "> ... " << ( (b &= Test_DestructiveListRemove<T>()) ? "Passed" : "FAILED") << endl;
   return b;
}

template <class T> bool Test_MutableTreeSet()
{
   bool b = true;
   cout << "Test_DestructiveSetInsertElement<" << ToString<T>::value << "> ... " << ( (b &= Test_DestructiveSetInsertElement<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_DestructiveSetUnion<"         << ToString<T>::value << "> ... " << ( (b &= Test_DestructiveSetUnion<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_DestructiveSetRemove<"        << ToString<T>::value << "> ... " << ( (b &= Test_DestructiveSetRemove<T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_DestructiveSetDifference<"    << ToString<T>::value << "> ... " << ( (b &= Test_DestructiveSetDifference<T>()) ? "Passed" : "FAILED") << endl;
   return b;
}


///////////////////////////////////////////////////////////////////////////////
//                              Set Unit Tests                               //
///////////////////////////////////////////////////////////////////////////////


template <class T> bool Test_SetContains()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);

   for (int i = 0; i < N; ++i) if (!t.Contains(values[i])) return false;
   if (T().Contains(0)) return false;

   return true;
}
template <class T> bool Test_IsSubsetOf()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);

   for (int i = 0; i < N; ++i) if (!T().Insert(values[i]).IsSubsetOf(t)) 
      return false;
   for (int i = 0; i < N; ++i) 
      if (!T().Insert(values[i]).Insert(values[(i+1)%N]).IsSubsetOf(t)) 
         return false;

   if (!T().IsSubsetOf(t)) return false;
   if (t.IsSubsetOf(T())) return false;

   return true;
}
template <class T> bool Test_Union()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);

   if (!IsEqual( t, t | t )) return false;
   if (!IsEqual( t, t | T() )) return false;

   for (int i = 0; i < N; ++i) 
      if (!IsEqual( t, t | T().Insert(values[i]))) return false;

   auto u = t | T().Insert(1) | T().Insert(1) | T().Insert(1);
   if (u.Size() != t.Size() + 1) return false;
   if (!u.Contains(1)) return false;

   return true;
}
template <class T> bool Test_Intersection()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);

   if (!IsEqual( t, t & t ))     return false;
   if (!IsEqual( T(), t & T() )) return false;  
   for (int i = 0; i < N; ++i) 
      if (!IsEqual( T() + values[i], t & T() + values[i] )) 
         return false;

   return true;
}
template <class T> bool Test_Difference()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);

   if (!IsEqual( T(), t - t )) return false;
   if (!IsEqual( t, t - T() )) return false;
   if ((t - T().Insert(0)).Size() != t.Size()-1) return false;
   if ((T() - T()).Size() != 0) return false;

   return true;
}
template <class T> bool Test_Insert()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T();
   for (int i = 0; i < N; ++i) t = t + values[i];
   if (!IsEqual( t, T::Construct(N, values) )) return false;
   return true;
}
template <class T> bool Test_Remove()
{
   const int N = 8; const int values[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };
   auto t = T::Construct(N, values);

   int indirection[N] = { 0, 4, 2, 6, 3, 5, 7, 1 };
   for (int i = 0; i < N; ++i) t = t - values[indirection[i]];
   if (t.Size() != 0) return false;
   if (!IsEqual( t, T() )) return false;

   return true;
}


template <class T> bool Test_Set()
{
   bool b = true;
   cout << "Test_SetContains<"   << ToString<T>::value << "> ... " << ( (b &= Test_SetContains  <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_IsSubsetOf<"    << ToString<T>::value << "> ... " << ( (b &= Test_IsSubsetOf   <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Union<"         << ToString<T>::value << "> ... " << ( (b &= Test_Union        <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Intersection<"  << ToString<T>::value << "> ... " << ( (b &= Test_Intersection <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Difference<"    << ToString<T>::value << "> ... " << ( (b &= Test_Difference   <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Insert<"        << ToString<T>::value << "> ... " << ( (b &= Test_Insert       <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Remove<"        << ToString<T>::value << "> ... " << ( (b &= Test_Remove       <T>()) ? "Passed" : "FAILED") << endl;
   return b;  
}


///////////////////////////////////////////////////////////////////////////////
//                              Map Unit Tests                               //
///////////////////////////////////////////////////////////////////////////////

/// Access
template <class T> bool Test_Contains_Map() 
{ 
   typedef typename T::ElementType E;
   const int N = 8; const E values[N] = { ToElement<T>(5), ToElement<T>(16), ToElement<T>(77), ToElement<T>(90), ToElement<T>(191), ToElement<T>(-249), ToElement<T>(-29), ToElement<T>(0) };
   auto t = T::Construct(N, values);

   bool b = true;
   for (int i = 0; i < N; ++i) b &= t.Contains(values[i].key);
   if (!b) return false;

   if (t.Contains(-1)) return false;
   if (T().Contains(100)) return false;

   return true;
}

template <class T> bool Test_GetOrElse_Map() 
{ 
   typedef typename T::ElementType E; const int N = 8; 
   const E values[N] = { ToElement<T>(5), ToElement<T>(16), ToElement<T>(77), ToElement<T>(90), ToElement<T>(191), ToElement<T>(-249), ToElement<T>(-29), ToElement<T>(0) };
   auto t = T::Construct(N, values);

   bool b = true;
   for (int i = 0; i < N; ++i) 
   {
      b &= (t.GetOrElse(values[i].key, -3.14159f) == values[i].value);
      b &= (t.GetOrElse(500, -3.14159f) == -3.14159f);
   }
   if (!b) return false;
   if (T().GetOrElse(500, -3.14159f) != -3.14159f) return false;

   return true;
}


/// Functional Updates. A new container is returned with the specified mapping
/// added, removed, or updated as appropriate. 
template <class T> bool Test_Insert_Map() 
{ 
   typedef typename T::ElementType E; const int N = 8; 
   const E valuesA[N] = { ToElement<T>(5), ToElement<T>(16), ToElement<T>(77), ToElement<T>(90), ToElement<T>(191), ToElement<T>(-249), ToElement<T>(-29), ToElement<T>(0) };
   const E valuesB[N] = { ToElement<T>(40), ToElement<T>(1), ToElement<T>(2), ToElement<T>(3), ToElement<T>(4), ToElement<T>(50), ToElement<T>(6), ToElement<T>(7) };
   auto t = T::Construct(N, valuesA);

   for (int i = 0; i < N; ++i)
      if (!t.Contains(valuesA[i].key)) return false;

   for (int i = 0; i < N; ++i)
   {
      t = t.Insert(valuesB[i].key, valuesB[i].value);
      if (!t.Contains(valuesB[i].key)) return false;
      if (!t.Contains(valuesA[i].key)) return false;
      if (t.GetOrElse(valuesB[i].key, -100.0f) != valuesB[i].value) return false;
   }

   if (!T().Insert(10, 3.14159f).Contains(10)) 
      return false;

   return true;
}

template <class T> bool Test_Remove_Map() 
{ 
   typedef typename T::ElementType E; const int N = 8; 
   const E valuesA[N] = { ToElement<T>(5), ToElement<T>(16), ToElement<T>(77), ToElement<T>(90), ToElement<T>(191), ToElement<T>(-249), ToElement<T>(-29), ToElement<T>(0) };
   const E valuesB[N] = { ToElement<T>(40), ToElement<T>(1), ToElement<T>(2), ToElement<T>(3), ToElement<T>(4), ToElement<T>(50), ToElement<T>(6), ToElement<T>(7) };
   auto t = T::Construct(N, valuesA);

   for (int i = 0; i < N; ++i)
   {
      t = t.Remove(valuesA[i].key).Remove(valuesB[i].key);
      if (t.Contains(valuesA[i].key)) return false;
      for (int j = i+1; j < N; ++j)
         if (!t.Contains(valuesA[j].key)) return false;
   }

   if (T().Remove(10).Size() != 0) return false;

   return true;
}

/// Returns a set container containing only the keys in the map
template <class T> bool Test_Keys_Map() 
{ 
   typedef typename T::ElementType E; const int N = 8; 
   const E values[N] = { ToElement<T>(5), ToElement<T>(16), ToElement<T>(77), ToElement<T>(90), ToElement<T>(191), ToElement<T>(-249), ToElement<T>(-29), ToElement<T>(0) };
   const int keyValues[N] = { 5, 16, 77, 90, 191, -249, -29, 0 };

   T t;
   if (t.Keys().Size() != 0) return false;

   for (int i = 0; i < N; ++i)
   {
      t = t.Insert(values[i].key, values[i].value);

      auto k0 = t.Keys();
      auto k1 = T::SetType::Construct(i+1, keyValues);
      if (!IsEqual(k0, k1))  return false;
   }

   return true;
}

/// Returns the values in the map as a traversable sequence
template <class T> bool Test_Values_Map() 
{ 
   typedef typename T::ElementType E; const int N = 8; 
   const E values[N] = { ToElement<T>(5), ToElement<T>(16), ToElement<T>(77), ToElement<T>(90), ToElement<T>(191), ToElement<T>(-249), ToElement<T>(-29), ToElement<T>(0) };
   auto t = T::Construct(N, values);

   typedef Immutable::LinkedList<float> List;
   typedef Immutable::Array<float> Array;

   List valuesLL = t.template Values<List>();
   Array valuesA = t.template Values<Array>();

   if (valuesLL.Size() != t.Size()) return false;
   if (valuesA.Size()  != t.Size()) return false;

   {
      auto itr = valuesLL.GetIterator();
      auto mItr = t.GetIterator();
      while (itr.HasNext())
         if (itr.Next() != mItr.Next().value) return false;  
   }

   {
      auto itr = valuesA.GetIterator();
      auto mItr = t.GetIterator();
      while (itr.HasNext())
         if (itr.Next() != mItr.Next().value) return false; 
   }

   if (T().template Values<List>().Size() != 0) return false;
   if (T().template Values<Array>().Size() != 0) return false;
   
   return true;
}

/// P must be of type KeyPredicate, or KeyPredicateByValue
template <class T> bool Test_FilterKeys_Map() 
{ 
   typedef typename T::ElementType E; const int N = 8; 
   const E values[N] = { ToElement<T>(5), ToElement<T>(16), ToElement<T>(77), ToElement<T>(90), ToElement<T>(191), ToElement<T>(-249), ToElement<T>(-29), ToElement<T>(0) };
   auto t = T::Construct(N, values);

   auto filtered = t.FilterKeys([] (int i) -> bool { return i > 0; });
   auto itr = filtered.GetIterator();
   while (itr.HasNext()) if (itr.Next().key <= 0) return false;
   if (filtered.Size() != 5) return false;

   if (!t.FilterKeys([] (int i) -> bool { return false; }).IsEmpty()) return false;
   if (!IsEqual(t, t.FilterKeys([] (int i) -> bool { return true; }))) return false;

   return true;
}

template <class T> bool Test_Map()
{
   bool b = true;
   cout << "Test_Contains_Map  <"  << ToString<T>::value << "> ... " << ( (b &= Test_Contains_Map   <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_GetOrElse_Map <"  << ToString<T>::value << "> ... " << ( (b &= Test_GetOrElse_Map  <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Insert_Map    <"  << ToString<T>::value << "> ... " << ( (b &= Test_Insert_Map     <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Remove_Map    <"  << ToString<T>::value << "> ... " << ( (b &= Test_Remove_Map     <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Keys_Map      <"  << ToString<T>::value << "> ... " << ( (b &= Test_Keys_Map       <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_Values_Map    <"  << ToString<T>::value << "> ... " << ( (b &= Test_Values_Map     <T>()) ? "Passed" : "FAILED") << endl;
   cout << "Test_FilterKeys_Map<"  << ToString<T>::value << "> ... " << ( (b &= Test_FilterKeys_Map <T>()) ? "Passed" : "FAILED") << endl;
   return b;  
}



int main()
{
   cout << endl << "Testing Array Structure...." << endl << endl;

   Test_Traversable<Immutable::Array<int> >();       Test_Sequence<Immutable::Array<int> >();      
   Test_Traversable<Mutable::Array<int> >();         Test_Sequence<Mutable::Array<int> >();  
   Test_MutableArray<Mutable::Array<int> >();
   
   cout << endl << "Testing LinkedList Structure...." << endl << endl;

   Test_Traversable<Immutable::LinkedList<int> >();      Test_Sequence<Immutable::LinkedList<int> >();       
   Test_Traversable<Mutable::LinkedList<int> >();        Test_Sequence<Mutable::LinkedList<int> >();   
   Test_MutableLinkedList<Mutable::LinkedList<int> >();

   cout << endl << "Testing TreeSet Structure...." << endl << endl;
   
   Test_Traversable<Immutable::TreeSet<int> >();     Test_Set<Immutable::TreeSet<int> >();   
   Test_Traversable<Mutable::TreeSet<int> >();       Test_Set<Mutable::TreeSet<int> >(); 
   Test_MutableTreeSet<Mutable::TreeSet<int> >(); 

   cout << endl << "Testing TreeMap Structure ....." << endl << endl;

   Test_TraversableMap<Immutable::TreeMap<int, float> >();  Test_Map<Immutable::TreeMap<int, float> >();
   Test_TraversableMap<Mutable::TreeMap<int, float> >();    Test_Map<Mutable::TreeMap<int, float> >();

   //cout << endl << "Testing Mutable Operations ....."

   //Test_MutableMap<Mutable::TreeMap<int, float> >();

   return 0;
}


