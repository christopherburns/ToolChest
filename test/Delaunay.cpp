
#include <stdio.h>
#include <stdlib.h>

#include <ToolChest.h>


#include "Delaunay.h"
#include "PeriodicDelaunay.h"

using namespace Mathematics;
using namespace ToolChest;
using Collections::Mutable::Array;

template <class T, class U> struct Pair
{
public:
   T first; U second;
   inline Pair() {}
   inline Pair(const T& a, const U& b) : first(a), second(b) {}
};


void GeneratePeriodicPatch(const int32 N)
{
   ////////////////////////
   // Create a Point Set //
   ////////////////////////

   #define TRIANGULATOR PeriodicDelaunay

   /// Generate Point Set
   uint32_t seed = 2342342; // 304820; //
   Array<Vector2f> pointSet(N);
   srand(seed);
   for (int i = 0; i < N; ++i) 
      pointSet[i] = Vector2f(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);

   /// Output the pointset to a file
   //FILE * fDump = fopen("Pointset.txt", "w");
   //fprintf(fDump, "{");
   //for (int i = 0; i < N-1; ++i)
   //   fprintf(fDump, "{%.4f, %.4f},", pointSet[i].x(), pointSet[i].y());
   //fprintf(fDump, "{%.4f, %.4f}}\n", pointSet[N-1].x(), pointSet[N-1].y());
   //fclose(fDump);

   /// Also, generate a periodic delaunay set
   StopWatch clk; clk.Start();
   TRIANGULATOR pDelaunay((const TRIANGULATOR::Vertex*)(&pointSet[0]), N);
   clk.Stop();

   pDelaunay.Print();

   printf
      ( "Tessellation time for %i points: %.3f s\n"
      , N
      , (float)clk.ReadTime().ToSeconds());
}

int main(int argc, const char ** argv)
{
   int N = 1000;
   if (argc > 1) N = atoi(argv[1]);
   GeneratePeriodicPatch(N);
	return 0;
}
