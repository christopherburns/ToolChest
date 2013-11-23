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

   struct TriAdjacency
   {
      int t[3]; int e[3];
      inline void Clear() { t[0] = t[1] = t[2] = e[0] = e[1] = e[2] = -1; }
      inline bool operator == (const TriAdjacency& rhs) const
      { bool r = true; for (int i = 0; i < 3; ++i) r &= t[i] == rhs.t[i] && e[i] == rhs.e[i]; return r; }
      inline bool operator < (const TriAdjacency& rhs) const
      {
         if (MIN(t[0], MIN(t[1], t[2])) < MIN(rhs.t[0], MIN(rhs.t[1], rhs.t[2]))) return true;
         else if (MIN(t[0], MIN(t[1], t[2])) == MIN(rhs.t[0], MIN(rhs.t[1], rhs.t[2]))) 
            return (MAX(t[0], MAX(t[1], t[2])) < MAX(rhs.t[0], MAX(rhs.t[1], rhs.t[2])));
         else return false;
      }
   };

   inline Topology(Ref<TriangleMesh<V> > mesh) 
      : _mesh(mesh)
      , _adjTriangles(Collections::Mutable::Array<TriAdjacency>(mesh->NumTriangles()))
   {
      //printf("Topology():\n");
      //printf("   AdjList Size: %i\n", _adjTriangles.Size());
      //printf("   Building Edge list...\n");

      Collections::Mutable::TreeSet<Edge> edgeSet;

      const Triangle * tris = mesh->GetTriangles();
      for (int t = 0; t < mesh->NumTriangles(); ++t)
      {
         Edge e[3]; 
         e[0].v0 = tris[t].i; e[0].v1 = tris[t].j; e[0].f0 = t; e[0].f1 = -1; 
         e[1].v0 = tris[t].j; e[1].v1 = tris[t].k; e[1].f0 = t; e[1].f1 = -1; 
         e[2].v0 = tris[t].k; e[2].v1 = tris[t].i; e[2].f0 = t; e[2].f1 = -1; 

         for (int c = 0; c < 3; c++)
         {
            auto edge = edgeSet.Contains(e[c]);

            //if (edge.HasNext()) { auto e_ = edge.Peek(); std::cout << "   Edge " << e_ << " already in set\n"; }
            //else std::cout << "   Adding edge " << e[c] << "to set\n";

            if (edge.HasNext()) edge.Next().f1 = t;
            else edgeSet += e[c];
         }
      }


      /// Task: Copy the edges into the array that we've allocated for them, so they
      /// can be random accessed in constant time
      _edges = Collections::Mutable::Array<Edge>(edgeSet.Size());
      {
         auto eItr = edgeSet.GetIterator(); int e = 0;
         while (eItr.HasNext()) _edges[e++] = eItr.Next();
      }

      //printf("   Built %i edges\n", (int)_edges.Size());

      // Iterate over edges, and for the triangles we can set their
      // adjacency bits. First, initialize the adj list
      for (int i = 0; i < _adjTriangles.Size(); ++i) _adjTriangles[i].Clear();
      //printf("   Initialized %i triangle adjacency indices...\n", _adjTriangles.Size());
      //auto eItr = _edges.GetIterator();
      //while (eItr.HasNext())
      for (int e = 0; e < _edges.Size(); ++e)
      {
         int face0 = _edges[e].f0;
         int face1 = _edges[e].f1;

         /// If we have two triangles, we link them in the triangle adjacency array
         if (face0 != -1 && face1 != -1)
         {
            //printf("      Edge (%i, %i) has two triangle neighbors (%i, %i)\n", edge.v0, edge.v1, face0, face1);
            /// This is kind of ugly. For each triangle, iterate over it's 
            /// three adjacency slots looking for an empty one, then fill it
            /// in
            {
               int v = 0;
               //printf("          adjTris[3*%i] = { %i, %i, %i}\n", face0
               //   , _adjTriangles[3*face0+0]
               //   , _adjTriangles[3*face0+1]
               //   , _adjTriangles[3*face0+2]);

               while (_adjTriangles[face0].t[v] != -1 && v < 3) v++;   assert(v < 3);
               _adjTriangles[face0].t[v] = face1;

            }
            {
               int v = 0;
               //printf("          adjTris[3*%i] = { %i, %i, %i}\n", face1
               //   , _adjTriangles[3*face1+0]
               //   , _adjTriangles[3*face1+1]
               //   , _adjTriangles[3*face1+2]);

               while (_adjTriangles[face1].t[v] != -1 && v < 3) v++;   assert(v < 3);
               _adjTriangles[face1].t[v] = face0;
            }
         }

         if (face0 != -1)
         {
            int v = 0;
            while (_adjTriangles[face0].e[v] != -1 && v < 3) v++;   assert(v < 3);
            _adjTriangles[face0].e[v] = e;
         }

         if (face1 != -1)
         {
            int v = 0;
            while (_adjTriangles[face1].e[v] != -1 && v < 3) v++;   assert(v < 3);
            _adjTriangles[face1].e[v] = e;
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
         printf(" [%3it %3it %3it | %3ie %3ie %3ie]"
            , _adjTriangles[i].t[0], _adjTriangles[i].t[1], _adjTriangles[i].t[2]
            , _adjTriangles[i].e[0], _adjTriangles[i].e[1], _adjTriangles[i].e[2]);
         if ((i+1) % 3 == 0) printf("\n");
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

   /// Accessors
   inline Ref<TriangleMesh<V> > GetMesh() const { return _mesh; }
   inline Collections::Mutable::Array<Edge> GetEdges() const { return _edges; }
   inline Collections::Mutable::Array<TriAdjacency> GetAdjacency() const { return _adjTriangles; }

private:
   Ref<TriangleMesh<V> > _mesh;
   Collections::Mutable::Array<Edge> _edges;
   Collections::Mutable::Array<TriAdjacency> _adjTriangles;
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
#if 1
template <class V> class Subdivider : public ToolChest::Object
{
public:

   static Ref<TriangleMesh<V> > Loop(Ref<Topology<V> > topology)
   {
      auto mesh = topology->GetMesh();
      auto edges = topology->GetEdges();
      auto adj = topology->GetAdjacency();

      /// 1. Allocate new vertex storage, copy old verts over
      /// 2. Iterate over edges, create new midpoint vertices
      /// 3. allocate new triangle storage, do NOT copy
      /// 4. iterate over edges, add four new faces for each adj face if
      ///        the face is not already been processed, and mark the ones added

      const int NUM_OLD_VERTICES = mesh->NumVertices();
      const int NUM_OLD_TRIANGLES = mesh->NumTriangles();

      const int NUM_NEW_VERTICES = NUM_OLD_VERTICES + edges.Size();
      const int NUM_NEW_TRIANGLES = NUM_OLD_TRIANGLES * 4;

      TriangleMesh<V> * newMesh = new TriangleMesh<V>(NUM_NEW_TRIANGLES, NUM_NEW_VERTICES);

      memcpy(newMesh->GetVertices(), mesh->GetVertices(), sizeof(V) * mesh->NumVertices());

      V * vertices = newMesh->GetVertices();
      Triangle * newTriangles = newMesh->GetTriangles();
      Triangle * oldTriangles = mesh->GetTriangles();

      {
         int e = 0;
         auto eItr = edges.GetIterator();
         while (eItr.HasNext())
         {
            auto edge = eItr.Next(); 
            assert(edge.v0 >= 0 && edge.v0 < NUM_OLD_VERTICES && edge.v1 >= 0 && edge.v1 < NUM_OLD_VERTICES);
            assert(NUM_OLD_VERTICES + e < NUM_NEW_VERTICES);
            vertices[NUM_OLD_VERTICES + e] = lerp(vertices[edge.v0], vertices[edge.v1], 0.5f);
            e++;
         }
      }

      /// We got our midpoints, now we retessellate
      {
         bool * tMask = new bool[NUM_OLD_TRIANGLES];
         for (int i = 0; i < NUM_OLD_TRIANGLES; ++i) tMask[i] = true;

         int t = 0; // New triangle count
         for (int e = 0; e < edges.Size(); ++e)
         {
            int f0 = edges[e].f0;
            int f1 = edges[e].f1;

            //std::cout << "   Attacking edge " << edges[e] << "\n";


            ///////////////////////
            // Tessellate Face 0 //
            ///////////////////////

            if (f0 != -1 && tMask[f0])
            {
               //std::cout << "      Tessellating face " << f0 << "\n";

               // Gather the 6 vertices and get them sorted simply
               auto v0 = oldTriangles[f0].i;
               auto v2 = oldTriangles[f0].j;
               auto v4 = oldTriangles[f0].k;

               /// Locate v1, v3, v5 via edge links. The difficulty here is that the edges do
               /// not orient themselves w.r. to the triangle's winding, because they are not
               /// half edges. 

               int e0 = adj[f0].e[0], e1 = adj[f0].e[1], e2 = adj[f0].e[2];

               typename Topology<V>::Edge eA; eA.v0 = v0; eA.v1 = v2;
               typename Topology<V>::Edge eB; eB.v0 = v2; eB.v1 = v4;
               typename Topology<V>::Edge eC; eC.v0 = v4; eC.v1 = v0;

               /// Comparisons would be 2x faster if the edge's vertex indices were always sorted
               auto v1 
                  = NUM_OLD_VERTICES
                  + (eA == edges[e0] ? e0 
                  : (eA == edges[e1] ? e1
                  : e2));
               auto v3
                  = NUM_OLD_VERTICES
                  + (eB == edges[e0] ? e0 
                  : (eB == edges[e1] ? e1
                  : e2));
               auto v5
                  = NUM_OLD_VERTICES
                  + (eC == edges[e0] ? e0 
                  : (eC == edges[e1] ? e1
                  : e2));

               //printf("      Vertex ordering: %i %i %i %i %i %i\n", v0, v1, v2, v3, v4, v5);

               { Triangle& tri = newTriangles[t++]; tri.i = v5; tri.j = v0; tri.k = v1; }
               { Triangle& tri = newTriangles[t++]; tri.i = v1; tri.j = v2; tri.k = v3; }
               { Triangle& tri = newTriangles[t++]; tri.i = v3; tri.j = v4; tri.k = v5; }
               { Triangle& tri = newTriangles[t++]; tri.i = v1; tri.j = v3; tri.k = v5; }

               tMask[f0] = false;
            }

            ///////////////////////
            // Tessellate Face 1 //
            ///////////////////////

            if (f1 != -1 && tMask[f1])
            {
               //std::cout << "      Tessellating face " << f0 << "\n";

               // Gather the 6 vertices and get them sorted simply
               auto v0 = oldTriangles[f1].i;
               auto v2 = oldTriangles[f1].j;
               auto v4 = oldTriangles[f1].k;

               /// Locate v1, v3, v5 via edge links. The difficulty here is that the edges do
               /// not orient themselves w.r. to the triangle's winding, because they are not
               /// half edges. 

               int e0 = adj[f1].e[0], e1 = adj[f1].e[1], e2 = adj[f1].e[2];

               typename Topology<V>::Edge eA; eA.v0 = v0; eA.v1 = v2;
               typename Topology<V>::Edge eB; eB.v0 = v2; eB.v1 = v4;
               typename Topology<V>::Edge eC; eC.v0 = v4; eC.v1 = v0;

               /// Comparisons would be 2x faster if the edge's vertex indices were always sorted
               auto v1 
                  = NUM_OLD_VERTICES
                  + (eA == edges[e0] ? e0 
                  : (eA == edges[e1] ? e1
                  : e2));
               auto v3
                  = NUM_OLD_VERTICES
                  + (eB == edges[e0] ? e0 
                  : (eB == edges[e1] ? e1
                  : e2));
               auto v5
                  = NUM_OLD_VERTICES
                  + (eC == edges[e0] ? e0 
                  : (eC == edges[e1] ? e1
                  : e2));

               //printf("      Vertex ordering: %i %i %i %i %i %i\n", v0, v1, v2, v3, v4, v5);

               Triangle& tri = newTriangles[t++];
               tri.i = v5; tri.j = v0; tri.k = v1; 
               tri.i = v1; tri.j = v2; tri.k = v3; 
               tri.i = v3; tri.j = v4; tri.k = v5; 
               tri.i = v1; tri.j = v3; tri.k = v5; 

               tMask[f1] = false;
            }
         }

         delete [] tMask;
      }


      /// Last step is to apply the mask to the vertex positions, smoothing, etc.



      return newMesh;   /// silently wrapped in a reference counter
   }

};
#endif

#endif
























