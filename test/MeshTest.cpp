
#include <stdio.h>
#include <stdlib.h>

#include <ToolChest.h>

using namespace ToolChest;

#include "Mesh.h"

using Collections::Mutable::Array;


struct Vertex { Vector4f position, tangent, bitangent; };

const Vertex pVertices[] = {

   /// Face X+
   { Vector4f(  1.0f, -1.0f, -1.0f, 1.0f), Vector4f( 0.0f, 0.0f,  1.0f, 0.0f), Vector4f(0.0f, 1.0f,  0.0f, 0.0f) },
   { Vector4f(  1.0f,  1.0f, -1.0f, 1.0f), Vector4f( 0.0f, 0.0f,  1.0f, 0.0f), Vector4f(0.0f, 1.0f,  0.0f, 1.0f) },
   { Vector4f(  1.0f,  1.0f,  1.0f, 1.0f), Vector4f( 0.0f, 0.0f,  1.0f, 1.0f), Vector4f(0.0f, 1.0f,  0.0f, 1.0f) },
   { Vector4f(  1.0f, -1.0f,  1.0f, 1.0f), Vector4f( 0.0f, 0.0f,  1.0f, 1.0f), Vector4f(0.0f, 1.0f,  0.0f, 0.0f) },

   /// Face X-
   { Vector4f( -1.0f, -1.0f, -1.0f, 1.0f), Vector4f( 0.0f, 0.0f, -1.0f, 0.0f), Vector4f(0.0f, 1.0f,  0.0f, 0.0f) },
   { Vector4f( -1.0f,  1.0f, -1.0f, 1.0f), Vector4f( 0.0f, 0.0f, -1.0f, 0.0f), Vector4f(0.0f, 1.0f,  0.0f, 1.0f) },
   { Vector4f( -1.0f,  1.0f,  1.0f, 1.0f), Vector4f( 0.0f, 0.0f, -1.0f, 1.0f), Vector4f(0.0f, 1.0f,  0.0f, 1.0f) },
   { Vector4f( -1.0f, -1.0f,  1.0f, 1.0f), Vector4f( 0.0f, 0.0f, -1.0f, 1.0f), Vector4f(0.0f, 1.0f,  0.0f, 0.0f) },

   /// Face Y+
   { Vector4f( -1.0f,  1.0f, -1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 0.0f), Vector4f(0.0f, 0.0f,  1.0f, 0.0f) },
   { Vector4f(  1.0f,  1.0f, -1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 0.0f), Vector4f(0.0f, 0.0f,  1.0f, 1.0f) },
   { Vector4f(  1.0f,  1.0f,  1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 1.0f), Vector4f(0.0f, 0.0f,  1.0f, 1.0f) },
   { Vector4f( -1.0f,  1.0f,  1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 1.0f), Vector4f(0.0f, 0.0f,  1.0f, 0.0f) },

   /// Face Y-
   { Vector4f( -1.0f, -1.0f, -1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 0.0f), Vector4f(0.0f, 0.0f, -1.0f, 0.0f) },
   { Vector4f(  1.0f, -1.0f, -1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 0.0f), Vector4f(0.0f, 0.0f, -1.0f, 1.0f) },
   { Vector4f(  1.0f, -1.0f,  1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 1.0f), Vector4f(0.0f, 0.0f, -1.0f, 1.0f) },
   { Vector4f( -1.0f, -1.0f,  1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 1.0f), Vector4f(0.0f, 0.0f, -1.0f, 0.0f) },

   /// Face Z+
   { Vector4f( -1.0f, -1.0f,  1.0f, 1.0f), Vector4f(-1.0f, 0.0f,  0.0f, 0.0f), Vector4f(0.0f, 1.0f, 0.0f, 0.0f) },
   { Vector4f(  1.0f, -1.0f,  1.0f, 1.0f), Vector4f(-1.0f, 0.0f,  0.0f, 0.0f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f) },
   { Vector4f(  1.0f,  1.0f,  1.0f, 1.0f), Vector4f(-1.0f, 0.0f,  0.0f, 1.0f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f) },
   { Vector4f( -1.0f,  1.0f,  1.0f, 1.0f), Vector4f(-1.0f, 0.0f,  0.0f, 1.0f), Vector4f(0.0f, 1.0f, 0.0f, 0.0f) },

   /// Face Z-
   { Vector4f( -1.0f, -1.0f, -1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 0.0f), Vector4f(0.0f, 1.0f, 0.0f, 0.0f) },
   { Vector4f(  1.0f, -1.0f, -1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 0.0f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f) },
   { Vector4f(  1.0f,  1.0f, -1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 1.0f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f) },
   { Vector4f( -1.0f,  1.0f, -1.0f, 1.0f), Vector4f( 1.0f, 0.0f,  0.0f, 1.0f), Vector4f(0.0f, 1.0f, 0.0f, 0.0f) },
};
const uint32 numVertices = 24;

//! 12 triangles
const Triangle pTriangles[] = {

   /// Face X+
   { 0,   1,  2 },  
   { 0,   2,  3 }, 
   { 4,   5,  6 },  
   { 4,   6,  7 }, 
   { 8,   9, 10 },  
   { 8,  10, 11 }, 
   { 12, 13, 14 },  
   { 12, 14, 15 }, 
   { 16, 17, 18 },  
   { 16, 18, 19 }, 
   { 20, 21, 22 },  
   { 20, 22, 23 } 
};
const uint32 numTriangles = 12;

int main(int argc, const char ** argv)
{
   printf("sizeof(Triangle) = %i\n", (int)sizeof(Triangle));
   printf("sizeof(Vertex) = %i\n", (int)sizeof(Vertex));

   Ref<TriangleMesh<Vertex> > cubeTriangleMesh = new TriangleMesh<Vertex>(numTriangles, numVertices);
   memcpy(cubeTriangleMesh->GetVertices(), pVertices, sizeof(Vertex)*numVertices);
   memcpy(cubeTriangleMesh->GetTriangles(), pTriangles, sizeof(Triangle)*numTriangles);

   Ref<Topology<Vertex> > cubeTopology = new Topology<Vertex>(cubeTriangleMesh);
   cubeTopology->Print();

   return 0;
}
