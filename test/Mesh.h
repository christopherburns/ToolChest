#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <ToolChest.h>

struct Triangle { int i, j, k; void Clear() { i = j = k = -1; }};

///                     ,,                                       ,,        
/// MMP""MM""YMM        db      `7MMM.     ,MMF'               `7MM        
/// P'   MM   `7                  MMMb    dPMM                   MM        
///      MM  `7Mb,od8 `7MM        M YM   ,M MM  .gP"Ya  ,pP"Ybd  MMpMMMb.  
///      MM    MM' "'   MM        M  Mb  M' MM ,M'   Yb 8I   `"  MM    MM  
///      MM    MM       MM        M  YM.P'  MM 8M"""""" `YMMMa.  MM    MM  
///      MM    MM       MM        M  `YM'   MM YM.    , L.   I8  MM    MM  
///    .JMML..JMML.   .JMML.    .JML. `'  .JMML.`Mbmmd' M9mmmP'.JMML  JMML.
                                           

template <class V> class TriangleMesh : public ToolChest::Object
{
public:
   inline TriangleMesh(int numTriangles, int numVertices)
      : _numTriangles(numTriangles)
      , _numVertices(numVertices)
   {
      _triangleList = new Triangle[_numTriangles];
      _vertexList = new V[_numVertices];
   }

   inline ~TriangleMesh()
   {
      delete [] _triangleList;
      delete [] _vertexList;      
   }

   inline int NumTriangles() const { return _numTriangles; }
   inline int NumVertices() const { return _numVertices; }

   inline Triangle * GetTriangles() { return _triangleList; }
   inline V * GetVertices() { return _vertexList; }

private:

   Triangle * _triangleList;
   V * _vertexList;

   int _numTriangles;
   int _numVertices;
};


///                                      ,,                                
/// MMP""MM""YMM                       `7MM                                
/// P'   MM   `7                         MM                                
///      MM  ,pW"Wq.`7MMpdMAo.  ,pW"Wq.  MM  ,pW"Wq.   .P"Ybmmm `7M'   `MF'
///      MM 6W'   `Wb MM   `Wb 6W'   `Wb MM 6W'   `Wb :MI  I8     VA   ,V  
///      MM 8M     M8 MM    M8 8M     M8 MM 8M     M8  WmmmP"      VA ,V   
///      MM YA.   ,A9 MM   ,AP YA.   ,A9 MM YA.   ,A9 8M            VVV    
///    .JMML.`Ybmd9'  MMbmmd'   `Ybmd9'.JMML.`Ybmd9'   YMMMMMb      ,V     
///                   MM                              6'     dP    ,V      
///                 .JMML.                            Ybmmmd'   OOb"       
/// 

template <class V> class Topology : public ToolChest::Object
{
public:
   struct Edge 
   {
      int v0, v1, f0, f1; 
      inline void Clear() { v0 = v1 = f0 = f1 = -1; }
      inline bool operator == (const Edge& rhs) const
      { return (v0 == rhs.v0 && v1 == rhs.v1) || (v1 == rhs.v0 && v0 == rhs.v1); }
      inline bool operator < (const Edge& rhs) const
      { 
         if (MIN(v0, v1) < MIN(rhs.v0, rhs.v1)) return true;
         else if (MIN(v0, v1) == MIN(rhs.v0, rhs.v1)) return MAX(v0, v1) < MAX(rhs.v0, rhs.v1);
         else return false;
      }

      inline friend std::ostream& operator << (std::ostream& s, const Edge& e) 
      { return s << "[" << e.v0 << " " << e.v1 << " " << e.f0 << " " << e.f1 << "]"; }
       
   };

   inline Topology(Ref<TriangleMesh<V> > mesh) 
      : _mesh(mesh)
      , _adjTriangles(Collections::Mutable::Array<int>(mesh->NumTriangles()*3))
   {
      //printf("Topology():\n");
      //printf("   AdjList Size: %i\n", _adjTriangles.Size());
      //printf("   Building Edge list...\n");


      const Triangle * tris = mesh->GetTriangles();
      for (int t = 0; t < mesh->NumTriangles(); ++t)
      {
         Edge e[3]; 
         e[0].v0 = tris[t].i; e[0].v1 = tris[t].j; e[0].f0 = t; e[0].f1 = -1; 
         e[1].v0 = tris[t].j; e[1].v1 = tris[t].k; e[1].f0 = t; e[1].f1 = -1; 
         e[2].v0 = tris[t].k; e[2].v1 = tris[t].i; e[2].f0 = t; e[2].f1 = -1; 

         for (int c = 0; c < 3; c++)
         {
            auto edge = _edges.Contains(e[c]);

            //if (edge.HasNext()) { auto e_ = edge.Peek(); std::cout << "   Edge " << e_ << " already in set\n"; }
            //else std::cout << "   Adding edge " << e[c] << "to set\n";

            if (edge.HasNext()) edge.Next().f1 = t;
            else _edges += e[c];
         }
      }

      //printf("   Built %i edges\n", (int)_edges.Size());

      // Iterate over edges, and for the triangles we can set their
      // adjacency bits. First, initialize the adj list
      for (int i = 0; i < _adjTriangles.Size(); ++i) _adjTriangles[i] = -1;
      //printf("   Initialized %i triangle adjacency indices...\n", _adjTriangles.Size());
      auto eItr = _edges.GetIterator();
      while (eItr.HasNext())
      {
         auto edge = eItr.Next();
         if (edge.f0 != -1 && edge.f1 != -1)
         {
            //printf("      Edge (%i, %i) has two triangle neighbors (%i, %i)\n", edge.v0, edge.v1, edge.f0, edge.f1);
            /// This is kind of ugly. For each triangle, iterate over it's 
            /// three adjacency slots looking for an empty one, then fill it
            /// in
            {
               int v = 0;
               //printf("          adjTris[3*%i] = { %i, %i, %i}\n", edge.f0
               //   , _adjTriangles[3*edge.f0+0]
               //   , _adjTriangles[3*edge.f0+1]
               //   , _adjTriangles[3*edge.f0+2]);

               while (_adjTriangles[3*edge.f0+v] != -1 && v < 3) v++;
               assert(v < 3);
               _adjTriangles[3*edge.f0+v] = edge.f1;

            }
            {
               int v = 0;
               //printf("          adjTris[3*%i] = { %i, %i, %i}\n", edge.f1
               //   , _adjTriangles[3*edge.f1+0]
               //   , _adjTriangles[3*edge.f1+1]
               //   , _adjTriangles[3*edge.f1+2]);

               while (_adjTriangles[3*edge.f1+v] != -1 && v < 3) v++;
               assert(v < 3);
               _adjTriangles[3*edge.f1+v] = edge.f0;
            }
         }
      }

      /// OK< i think we're done, we have our adjacency structure
   }


   inline void Print() const
   {
      const Triangle * tris = _mesh->GetTriangles();

      printf("\nTriangles: %i\n", _mesh->NumTriangles());
      for (int i = 0; i < _mesh->NumTriangles(); ++i)
      {
         printf(" [%3i %3i %3i]", tris[i].i, tris[i].j, tris[i].k);
         if ((i+1) % 6 == 0) printf("\n");
      }

      printf("\nAdjacency: \n");
      for (int i = 0; i < _mesh->NumTriangles(); ++i)
      {
         printf(" [%3i %3i %3i]", _adjTriangles[3*i + 0], _adjTriangles[3*i + 1], _adjTriangles[3*i + 2]);
         if ((i+1) % 6 == 0) printf("\n");
      }

      printf("\nEdges: %i\n", _edges.Size());
      auto eItr = _edges.GetIterator(); int i = 0;
      while (eItr.HasNext())
      {
         auto edge = eItr.Next();
         printf(" [%3iv %3iv %3it %3it]", edge.v0, edge.v1, edge.f0, edge.f1);
         if ((i+1) % 4 == 0) printf("\n");
         i++;
      }
      printf("\n");
   }


private:
   Ref<TriangleMesh<V> > _mesh;
   Collections::Mutable::TreeSet<Edge> _edges;
   Collections::Mutable::Array<int> _adjTriangles;
};



#endif
