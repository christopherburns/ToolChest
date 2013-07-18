
#include <stdio.h>
#include <stdlib.h>

#include "../toolchest/ToolChest.h"

using namespace Burns;

#include "Delaunay.h"

using Collections::Mutable::Array;

template <class T, class U> struct Pair
{
public:
   T first; U second;
   inline Pair() {}
   inline Pair(const T& a, const U& b) : first(a), second(b) {}
};


//Pair<Array<int32>, Array<Vector2f> > GeneratePeriodicPatch(const int32 N)
void GeneratePeriodicPatch(const int32 N)
{
   ////////////////////////
   // Create a Point Set //
   ////////////////////////


   #define TRIANGULATOR Delaunay


   /// Generate Point Set
   uint32 seed = 1032481; // 304820; //
   Array<Vector2f> pointSet(N);
   srand(seed);
   for (int i = 0; i < N; ++i) 
   {
      //if (i == 1) { rand(); rand(); };
      pointSet[i] = Vector2f(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
   }

   /// Output the pointset to a file
   FILE * fDump = fopen("Pointset.txt", "w");
   fprintf(fDump, "{");
   for (int i = 0; i < N-1; ++i)
      fprintf(fDump, "{%.4f, %.4f},", pointSet[i].x(), pointSet[i].y());
   fprintf(fDump, "{%.4f, %.4f}}\n", pointSet[N-1].x(), pointSet[N-1].y());
   fclose(fDump);

   /// Also, generate a periodic delaunay set
   TRIANGULATOR pDelaunay;
   /*auto pTile =*/ pDelaunay.Tessellate((const TRIANGULATOR::Vertex*)(&pointSet[0]), N);

   /*const int32 N_TRIS  = pTile.first.size() / 3;
   const int32 N_VERTS = pTile.second.size();

   Vector2f * vertexBuffer = (Vector2f*)&pTile.second[0];
   int32 *    indexBuffer  = (int32*)&pTile.first[0];

   /// Fix the polygon winding - very fast
   for (int i = 0; i < N_TRIS; ++i)
   {
      /// Load the indices, gather the vertices
      int32    i0 = indexBuffer[i*3+0], i1 = indexBuffer[i*3+1], i2 = indexBuffer[i*3+2];
      Vector3f v0 = Vector3f(vertexBuffer[i0].x(), vertexBuffer[i0].y(), 0.0f);
      Vector3f v1 = Vector3f(vertexBuffer[i1].x(), vertexBuffer[i1].y(), 0.0f);
      Vector3f v2 = Vector3f(vertexBuffer[i2].x(), vertexBuffer[i2].y(), 0.0f);

      /// Check the winding, and flip it if we're wrong
      Vector3f n = cross(v1 - v0, v2 - v0);
      if (n.z() < 0.0f) swap(indexBuffer[i*3+0], indexBuffer[i*3+1]);
   }

   Array<int32> indices(pTile.first.size());
   for (int i = 0; i < pTile.first.size(); ++i) 
      indices[i] = pTile.first[i];

   Array<Vector2f> vertices(pTile.second.size());
   for (int i = 0; i < pTile.second.size(); ++i) 
      vertices[i] = Vector2f(pTile.second[i].x(), pTile.second[i].y());

   return Pair<Array<int32>, Array<Vector2f> >(indices, vertices);*/
}

int main()
{
   /*auto patch =*/ GeneratePeriodicPatch(20);
	return 0;
}
