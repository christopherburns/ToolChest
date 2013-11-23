
#include <stdio.h>
#include <stdlib.h>

#include <ToolChest.h>

using namespace ToolChest;

#include "Mesh.h"

using Collections::Mutable::Array;


class Cube
{
public:
   struct Vertex 
   { 
      Vector4f position, tangent, bitangent; 
      friend inline Vertex operator * (float f, const Vertex& v)
      { Vertex r; r.position = f * v.position; r.tangent = f * v.tangent; r.bitangent = f * v.bitangent; return r; }
      friend inline Vertex operator + (const Vertex& l, const Vertex& r)
      { Vertex v; v.position = l.position + r.position; v.tangent = l.tangent * r.tangent; v.bitangent = l.bitangent * r.bitangent; return v; }
   };

   static const uint32 NumVertices = 24;
   static const uint32 NumTriangles = 12;

   static const Vertex * Vertices() 
   {
      static const Vertex pVertices[] = {
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
      return pVertices;
   }

   static const Triangle * Triangles()
   {
      static const Triangle pTriangles[] = {
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
      return pTriangles;
   }
};


class LoneTriangle
{
public:
   struct Vertex 
   { 
      Vector4f position; 
      friend inline Vertex operator * (float f, const Vertex& v)
      { Vertex r; r.position = f * v.position; return r; }
      friend inline Vertex operator + (const Vertex& l, const Vertex& r)
      { Vertex v; v.position = l.position + r.position; return v; }
   };

   static const uint32 NumVertices = 3;
   static const uint32 NumTriangles = 1;

   static const Vertex * Vertices() 
   {
      static const Vertex pVertices[] = {
         /// Face X+
         { Vector4f(  1.0f, -1.0f, -1.0f, 1.0f) },
         { Vector4f(  1.0f,  1.0f, -1.0f, 1.0f) },
         { Vector4f(  1.0f,  1.0f,  1.0f, 1.0f) }
      };
      return pVertices;
   }

   static const Triangle * Triangles()
   {
      static const Triangle pTriangles[] = { { 0,   1,  2 } };
      return pTriangles;
   }
};


int main(int argc, const char ** argv)
{
   {
      Ref<TriangleMesh<Cube::Vertex> > cubeTriangleMesh = new TriangleMesh<Cube::Vertex>(Cube::NumTriangles, Cube::NumVertices);
      memcpy(cubeTriangleMesh->GetVertices(), Cube::Vertices(), sizeof(Cube::Vertex)*Cube::NumVertices);
      memcpy(cubeTriangleMesh->GetTriangles(), Cube::Triangles(), sizeof(Triangle)*Cube::NumTriangles);

      Ref<Topology<Cube::Vertex> > cubeTopology = new Topology<Cube::Vertex>(cubeTriangleMesh);
      cubeTopology->Print();

      Ref<TriangleMesh<Cube::Vertex> > subdivided = Subdivider<Cube::Vertex>::Loop(cubeTopology);
      Ref<Topology<Cube::Vertex> > cubeTopology1 = new Topology<Cube::Vertex>(subdivided);
      cubeTopology1->Print();
   }

   printf("----------------------------------------------------------------\n");
   {
      Ref<TriangleMesh<LoneTriangle::Vertex> > triMesh = new TriangleMesh<LoneTriangle::Vertex>(LoneTriangle::NumTriangles, LoneTriangle::NumVertices);
      memcpy(triMesh->GetVertices(), LoneTriangle::Vertices(), sizeof(LoneTriangle::Vertex)*LoneTriangle::NumVertices);
      memcpy(triMesh->GetTriangles(), LoneTriangle::Triangles(), sizeof(Triangle)*LoneTriangle::NumTriangles);

      Ref<Topology<LoneTriangle::Vertex> > cubeTopology = new Topology<LoneTriangle::Vertex>(triMesh);
      cubeTopology->Print();

      Ref<TriangleMesh<LoneTriangle::Vertex> > subdivided = Subdivider<LoneTriangle::Vertex>::Loop(cubeTopology);
      Ref<Topology<LoneTriangle::Vertex> > cubeTopology1 = new Topology<LoneTriangle::Vertex>(subdivided);
      cubeTopology1->Print();
   }

   return 0;
}
