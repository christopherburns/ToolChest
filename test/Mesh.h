#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <iomanip>
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



///                          ,,      ,...                   ,,                  
/// `7MMF'  `7MMF'         `7MM    .d' ""`7MM"""YMM       `7MM                  
///   MM      MM             MM    dM`     MM    `7         MM                  
///   MM      MM   ,6"Yb.    MM   mMMmm    MM   d      ,M""bMM  .P"Ybmmm .gP"Ya 
///   MMmmmmmmMM  8)   MM    MM    MM      MMmmMM    ,AP    MM :MI  I8  ,M'   Yb
///   MM      MM   ,pm9MM    MM    MM      MM   Y  , 8MI    MM  WmmmP"  8M""""""
///   MM      MM  8M   MM    MM    MM      MM     ,M `Mb    MM 8M       YM.    ,
/// .JMML.  .JMML.`Moo9^Yo..JMML..JMML.  .JMMmmmmMMM  `Wbmd"MML.YMMMMMb  `Mbmmd'
///                                                            6'     dP        
///                                                            Ybmmmd'          
/// 

template <class V> class HalfEdgeStructure : public ToolChest::Object
{
public:
   struct HalfEdge
   {
      int v, t, next, twin;  // indexes the vertex, triangle, halfedge, halfedge arrays
      inline HalfEdge() : v(-1), t(-1), next(-1), twin(-1) {}
      inline bool operator == (const HalfEdge& rhs) const
      { return v == rhs.v && t == rhs.t; }
      inline bool operator < (const HalfEdge& rhs) const
      { return v < rhs.v || (v == rhs.v && t < rhs.t); }
      inline friend std::ostream& operator << (std::ostream& s, const HalfEdge& e) 
      { 
         return s 
            << "[" << std::setw(3) << e.v    << " " << std::setw(3) << e.t 
            << " " << std::setw(3) << e.next << " " << std::setw(3) << e.twin << "]"; 
      }
   };

private:
   ToolChest::Ref<TriangleMesh<V> > _mesh;
   Collections::Mutable::Array<HalfEdge> _halfEdges;

public:
   /// Accessors
   inline ToolChest::Ref<TriangleMesh<V> > GetMesh() const { return _mesh; }
   inline Collections::Mutable::Array<HalfEdge> GetHalfEdges() const { return _halfEdges; }

   inline HalfEdgeStructure(ToolChest::Ref<TriangleMesh<V> > mesh) 
      : _mesh(mesh)
      , _halfEdges(Collections::Mutable::Array<HalfEdge>(mesh->NumTriangles() * 3))
   {
      assert(mesh);

      /// During construction, we use a map with O(n log n) lookup
      typedef Collections::Mutable::TreeMap< ToolChest::uint64, int > EdgeMap;
      EdgeMap edgeMap;

      /// Initialize the _halfEdges' v, t, and next fields
      Triangle * tris = _mesh->GetTriangles();
      for (int t = 0; t < mesh->NumTriangles(); ++t)
      {
         _halfEdges[t*3+0].t = t; _halfEdges[t*3+0].v = tris[t].i; _halfEdges[t*3+0].next = t*3+1;
         _halfEdges[t*3+1].t = t; _halfEdges[t*3+1].v = tris[t].j; _halfEdges[t*3+1].next = t*3+2;
         _halfEdges[t*3+2].t = t; _halfEdges[t*3+2].v = tris[t].k; _halfEdges[t*3+2].next = t*3+0;

         edgeMap += EdgeMap::ElementType( (ToolChest::uint64(tris[t].i) << 32) | tris[t].j, t*3+0 );
         edgeMap += EdgeMap::ElementType( (ToolChest::uint64(tris[t].j) << 32) | tris[t].k, t*3+1 );
         edgeMap += EdgeMap::ElementType( (ToolChest::uint64(tris[t].k) << 32) | tris[t].i, t*3+2 );
      }

      for (int e = 0; e < _halfEdges.Size(); ++e)
      {
         int v0 = _halfEdges[e].v;
         int v1 = _halfEdges[_halfEdges[e].next].v;
         ToolChest::uint64 twinKey = (ToolChest::uint64(v1) << 32) | v0;  //< reverse order of edge e
         auto twin = edgeMap.GetOrElse(twinKey, -1);
         if (twin != -1)
         {
            assert(twin < _halfEdges.Size());
            _halfEdges[twin].twin = e;
            _halfEdges[e].twin = twin;
         }
      } 
   }


   inline bool Invariant() const
   {
      /// Only the twin field is allowed to be -1
      bool r = true;
      int i = 0;
      auto eItr = _halfEdges.GetIterator();
      while (eItr.HasNext() && r)
      {
         HalfEdge e = eItr.Next();
         r &= e.v >= 0 && e.v < _mesh->NumVertices();     // assert(e.v >= 0 && e.v < _mesh->NumVertices());
         r &= e.t >= 0 && e.t < _mesh->NumTriangles();    // assert(e.t >= 0 && e.t < _mesh->NumTriangles());
         r &= e.next >= 0 && e.next < _halfEdges.Size();  // assert(e.next >= 0 && e.next < _halfEdges.Size());

         if (e.twin != -1)
         {
            bool safe = e.twin >= 0 && e.twin < _halfEdges.Size();
            if (safe) r &= _halfEdges[e.twin].twin == i;
            else r = false;
         }

         i++;
      }
      return r;
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

      printf("\nHalfEdges: %i\n", _halfEdges.Size());
      auto eItr = _halfEdges.GetIterator(); int i = 0;
      while (eItr.HasNext())
      {
         auto edge = eItr.Next();
         std::cout << edge << "  ";
         if ((i+1) % 4 == 0) printf("\n");
         i++;
      }
      printf("\n");
   }
};



///                                                                  ,,               ,,    ,,              ,,            ,,                     
/// `7MMF'                                        .M"""bgd          *MM             `7MM    db              db            db                     
///   MM                                         ,MI    "Y           MM               MM                                                         
///   MM         ,pW"Wq.   ,pW"Wq.`7MMpdMAo.     `MMb.   `7MM  `7MM  MM,dMMb.    ,M""bMM  `7MM `7M'   `MF'`7MM  ,pP"Ybd `7MM  ,pW"Wq.`7MMpMMMb.  
///   MM        6W'   `Wb 6W'   `Wb MM   `Wb       `YMMNq. MM    MM  MM    `Mb ,AP    MM    MM   VA   ,V    MM  8I   `"   MM 6W'   `Wb MM    MM  
///   MM      , 8M     M8 8M     M8 MM    M8     .     `MM MM    MM  MM     M8 8MI    MM    MM    VA ,V     MM  `YMMMa.   MM 8M     M8 MM    MM  
///   MM     ,M YA.   ,A9 YA.   ,A9 MM   ,AP     Mb     dM MM    MM  MM.   ,M9 `Mb    MM    MM     VVV      MM  L.   I8   MM YA.   ,A9 MM    MM  
/// .JMMmmmmMMM  `Ybmd9'   `Ybmd9'  MMbmmd'      P"Ybmmd"  `Mbod"YML.P^YbmdP'   `Wbmd"MML..JMML.    W     .JMML.M9mmmP' .JMML.`Ybmd9'.JMML  JMML.
///                                 MM                                                                                                           
///                               .JMML.     

template <class V> class Subdivider : public ToolChest::Object
{
private:
   static inline float beta(float n)
   {
      return 5.0f/8.0f 
         - ToolChest::POW( 3.0f 
                         * ToolChest::COS( 2.0f * ToolChest::PI<float>() / n)
                                         , 2.0f) 
         / 64.0f;
   }

public:

   static inline ToolChest::Ref<TriangleMesh<V> > Loop(ToolChest::Ref<HalfEdgeStructure<V> > topology)
   {
      auto mesh = topology->GetMesh();
      auto halfEdges = topology->GetHalfEdges();

      /// 1. Allocate new vertex storage, copy old verts over
      /// 2. Iterate over edges, create new midpoint vertices
      /// 3. allocate new triangle storage, do NOT copy
      /// 4. iterate over edges, add four new faces for each adj face if
      ///        the face is not already been processed, and mark the ones added

      const int NUM_OLD_VERTICES = mesh->NumVertices();
      const int NUM_OLD_TRIANGLES = mesh->NumTriangles();

      const int NUM_NEW_TRIANGLES = NUM_OLD_TRIANGLES * 4;

      /// The number of new vertices is numEdgesWithTwins / 2 + numEdgesWithoutTwins;
      int numEdgesWithTwins = 0;
      for (int i = 0; i < halfEdges.Size(); ++i) if (halfEdges[i].twin != -1) numEdgesWithTwins++;
      const int NUM_NEW_VERTICES 
         = NUM_OLD_VERTICES 
         + numEdgesWithTwins/2 
         + (halfEdges.Size()-numEdgesWithTwins);

      TriangleMesh<V> * newMesh = new TriangleMesh<V>(NUM_NEW_TRIANGLES, NUM_NEW_VERTICES);
      memcpy(newMesh->GetVertices(), mesh->GetVertices(), sizeof(V) * mesh->NumVertices());

      V * oldVertices = mesh->GetVertices();
      V * vertices = newMesh->GetVertices();
      Triangle * newTriangles = newMesh->GetTriangles();
      Triangle * oldTriangles = mesh->GetTriangles();

      /////////////////////////////
      // Update Vertex Positions //
      /////////////////////////////

      /// 1. Calculate valences and border bits for each vertex
      /// 2. Iterate over verts, initialize with correct self-weighted value
      /// 3. Iterate over half-edges and accumulate to their origin verts

      /// in the valences array, the high bit indicates whether the vertex is on a border
      /// leaving us with a maximum representable valence of 128
      ToolChest::uint8 * valences = new ToolChest::uint8[NUM_OLD_VERTICES];
      memset(valences, 0, NUM_OLD_VERTICES);
      for (int i = 0; i < halfEdges.Size(); ++i)
      {
         const int v = halfEdges[i].v;
         assert(v >= 0 && v < NUM_OLD_VERTICES);
         valences[v] = (valences[v] & 0x80) | ((valences[v] & 0x7f) + 1);
         if (halfEdges[i].twin == -1) valences[v] |= 0x80;
      }

      //for (int i = 0; i < NUM_OLD_VERTICES; ++i)
      //   printf("Valences[%i] = %i, %s\n", i, valences[i] & 0x7f, (valences[i] & 0x80) ? "true" : "false");

      for (int v = 0; v < NUM_OLD_VERTICES; ++v)
      {
         if (valences[v] & 0x80) vertices[v] = 0.75f * oldVertices[v];
         else vertices[v] = (1.0f - beta(float(valences[v] & 0x7f))) * oldVertices[v];
      }

      for (int i = 0; i < halfEdges.Size(); ++i)
      {
         const int v = halfEdges[i].v;
         if (halfEdges[i].twin == -1)
         {
            /// Distribute in both directions, both are edge vertices
            int v0 = halfEdges[i].v;
            int v1 = halfEdges[ halfEdges[i].next ].v;
            vertices[v0] = vertices[v0] + 0.125f * oldVertices[v1];
            vertices[v1] = vertices[v1] + 0.125f * oldVertices[v0];
         }
         else
         {
            float n = float(valences[v] & 0x7f);
            vertices[v] = vertices[v] + (beta(n) / n) * oldVertices[ halfEdges[halfEdges[i].next].v ];
         }
      }
      delete [] valences;


      //////////////////////////////////
      // Compute New Vertex Locations //
      //////////////////////////////////

      Collections::Mutable::TreeMap<int, int> halfEdgeToVertexMap;
      {
         int v = 0;
         for (int e = 0; e < halfEdges.Size(); ++e) 
         {
            if (!halfEdgeToVertexMap.Contains(e))
            {
               int v0 = halfEdges[e].v;
               int v1 = halfEdges[ halfEdges[e].next ].v;
               int v2 = NUM_OLD_VERTICES + v++;

               /// Here we apply the loop-subdivision mask
               if (halfEdges[e].twin == -1)
                  vertices[v2] = ToolChest::lerp(oldVertices[v0], oldVertices[v1], 0.5f);
               else
               {
                  int vA = halfEdges[ halfEdges[ halfEdges[e].next ].next ].v;
                  int vB = halfEdges[ halfEdges[ halfEdges[ halfEdges[e].twin ].next ].next ].v;
                  vertices[v2] 
                     = 0.375f * oldVertices[v0] + 0.375f * oldVertices[v1]
                     + 0.125f * oldVertices[vA] + 0.125f * oldVertices[vB];
               }

               /// Should we use v, or NUM_OLD_VERTIES+v ?  Probably v...
               halfEdgeToVertexMap += Collections::Common::KeyValuePair<int, int>(e, v2);
               if (halfEdges[e].twin) 
                  halfEdgeToVertexMap += Collections::Common::KeyValuePair<int, int>(halfEdges[e].twin, v2);
            }
         }
      }


      ////////////////
      // Tessellate //
      ////////////////

      /// We got our midpoints, now we retessellate
      {
         // We can iterate over triangles, and create four new triangles
         for (int t = 0; t < NUM_OLD_TRIANGLES; ++t)
         {
            // The mapping from triangle to half edge is implicit
            int he0 = t*3+0, he1 = t*3+1, he2 = t*3+2;

            int v0 = oldTriangles[t].i;
            int v1 = halfEdgeToVertexMap.GetOrElse(he0, -1); assert(v1 != -1);
            int v2 = oldTriangles[t].j;
            int v3 = halfEdgeToVertexMap.GetOrElse(he1, -1); assert(v3 != -1);
            int v4 = oldTriangles[t].k;
            int v5 = halfEdgeToVertexMap.GetOrElse(he2, -1); assert(v5 != -1);

            { Triangle& tri = newTriangles[t*4+0]; tri.i = v0; tri.j = v1; tri.k = v5; }
            { Triangle& tåri = newTriangles[t*4+1]; tri.i = v1; tri.j = v2; tri.k = v3; }
            { Triangle& tri = newTriangles[t*4+2]; tri.i = v5; tri.j = v3; tri.k = v4; }
            { Triangle& tri = newTriangles[t*4+3]; tri.i = v1; tri.j = v3; tri.k = v5; }
         }
      }

      return newMesh;   /// silently wrapped in a reference counter
   }
};


#endif
























