
#include "SmartDelaunay.h"

using namespace Burns;


////////////////////
// Implementation //
////////////////////

SmartDelaunay::Vertex SmartDelaunay::TriangleGeometry::computeCircumcenter
   ( SmartDelaunay::Vertex p1
   , SmartDelaunay::Vertex p2
   , SmartDelaunay::Vertex p3)
{
   float d_ca = Vertex::dot(p3-p1, p2-p1);
   float d_ba = Vertex::dot(p3-p2, p1-p2);
   float d_cb = Vertex::dot(p1-p3, p2-p3);

   float n1 = d_ba * d_cb;
   float n2 = d_cb * d_ca;
   float n3 = d_ca * d_ba;

   float invBottom = RCP(2.0f * (n1 + n2 + n3));
   return ((n2+n3) * p1 + (n3+n1) * p2 + (n1+n2) * p3) * invBottom;
}

/// Triangle constructor, builds a circumcircle in the process
SmartDelaunay::TriangleGeometry::TriangleGeometry
   ( SmartDelaunay::Vertex v0
   , SmartDelaunay::Vertex v1
   , SmartDelaunay::Vertex v2)
{
   center = computeCircumcenter(v0, v1, v2);
   radiusSq = (v0 - center).lengthSquared();
   radius = SQRT(radiusSq);
}

void SmartDelaunay::generateSuperTriangle(AABBox2f bounds, SmartDelaunay::Vertex verts[3])
{
   static const float sqrt3 = SQRT<float>(3.0f);

   Vector2f delta = bounds.ur - bounds.ll;

   // Make the bounding box slightly bigger, just to feel safe.
   delta *= 1.01f;

   bounds.ll -= delta;
   bounds.ur += delta;
   delta += 2 * delta;

   verts[0] = Vertex(bounds.ll.x() - delta.y() * sqrt3 / 3.0f, bounds.ll.y());
   verts[1] = Vertex((bounds.ll.x() + bounds.ur.x()) * 0.5f, bounds.ur.y() + delta.x() * sqrt3 * 0.5f);
   verts[2] = Vertex(bounds.ur.x() + delta.y() * sqrt3 / 3.0f, bounds.ll.y());
}

void SmartDelaunay::clear()
{
   if (_topology) delete _topology;
   _topology = NULL;
}


String SmartDelaunay::faceToString(Topology::FaceIterator fItr) const
{
   String s = "Polygon[{ ";
   auto edge0 = fItr.edge();
   do 
   {
      s += String("{ ") + edge0->end->geometry.x() + ", " + edge0->end->geometry.y() + " }";
      if (edge0->next != fItr.edge()) s += ", ";
      edge0 = edge0->next;
   } while (edge0 != fItr.edge());
   return s + "} ]";
}




void SmartDelaunay::DumpPolygonsToMathematicaFile(const char * fileName) const
{
   String s = "polygonList = { EdgeForm[{Thick, Red}], FaceForm[{Yellow, Opacity[0.8]}], Polygon[{{0.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}}], EdgeForm[{Thick, Blue}], FaceForm[Yellow], ";
   Topology::FaceIterator fItr = _topology->GetFace().next();
   int i = 0;
   do 
   {
      if (fItr.next() != _topology->GetFace().next())
         s += faceToString(fItr) + ", ";
      fItr++; i++;
   } while (fItr != _topology->GetFace().next());
   s += "};";

   FILE * f = fopen(fileName, "w"); assert(f);
   fwrite(*s, sizeof(char), s.length()-1, f);
   fclose(f);
}


void SmartDelaunay::InsertPoint(SmartDelaunay::Vertex v)
{

   /// For each face f in _topology:
   ///    collect all three edges (assert there's only three)
   ///    if (edge.isInSet)
   ///    add/remove each edge from the set, accordingly
   /// We have a set of edges (half edges) that are to be removed

   /// Nope, we'll just build a list of triangles to remove, and send
   /// them to the halfedge structure for batch removal
   /// 

   /// Do not manipulate the topology during iteration
   /// This linear search through the polygons is what causes this
   /// algorithm to be O(N^2) rather than O(N log N)
   Collection<Topology::FaceIterator> facesToRemove(10);
   Topology::FaceIterator fItr = _topology->GetFace();
   do 
   {
      if (fItr.geometry().isInCircumcircle(v)) facesToRemove.add(fItr);
      fItr++;
   } while (fItr != _topology->GetFace());

   /// Submit the list of faces (face iterators) to the data structure
   /// for merging into a single face. The geometry guarantees that the
   /// collection of faces is contiguous and can be in fact merged

   ASSERT(facesToRemove.size() > 0);

   //printf("Inserting point %s\n", *String(v));
   //printf("Merging %i faces...\n", facesToRemove.size());
   //for (int i = 0; i < facesToRemove.size(); ++i)
   //   printf("   Face %i:  %s\n", 
   //      i, 
   //      faceToString(facesToRemove[i]));


   Topology::FaceIterator newFace = 
      (facesToRemove.size() > 1)
      ? _topology->MergeFaces( &facesToRemove[0], facesToRemove.size())
      : facesToRemove[0];

   ASSERT(!newFace.isNull());  /// Should definitely succeed ...

   /// if this all worked out, all iterators in the facesToRemove array
   /// should be null now
   if (facesToRemove.size() > 1)
      for (Index i = 0; i < facesToRemove.size(); ++i)
         ASSERT(facesToRemove[i].isNull());

   /// Now we subdivide the new face using the point we inserted
   ///    Adds the new vertex
   ///    Adds edges from all of the newFace's vertices to the new vertex
   ///    newFace is destroyed, and replaced with the new faces created
   Topology::VertexIterator point = _topology->SubdivideFaceByVertex(newFace, v);

   //printf("   Computing geometric properties of new triangles...\n");
   /// We need to set the geometric properties of the new faces, so we 
   /// iterate over the faces that fan out from the inserted point
   Topology::OneRingIterator oneItr = _topology->GetOneRingIterator(point);
   Topology::OneRingIterator terminus = oneItr;
   int i = 0;
   do 
   {
      //printf("      face %i...\n", i); i++;
      Topology::Face * f = oneItr.face();

      /// Get this triangle's three vertices
      Vertex v[3];
      Topology::HalfEdge * e = f->edge;
      for (Index i = 0; i < 3; ++i, e = e->next) v[i] = e->end->geometry;

      /// Create the triangle's geometry and assign it
      f->geometry = TriangleGeometry(v[0], v[1], v[2]);

      oneItr = oneItr.next();
   } while (oneItr != terminus);
}




void SmartDelaunay::Tessellate(const SmartDelaunay::Vertex* points, const int NUM_POINTS)
{
   clear();

   /// Compute the bounding box of the data, and build the list of points
   AABBox2f bounds = AABBox2f::invertedBounds();
   for (Index i = 0; i < NUM_POINTS; ++i) bounds |= points[i];

   /// Generate the super triangle
   Vertex superTriangle[3];
   generateSuperTriangle(bounds, superTriangle);

   /// Initialiaze our half-edge data structure with the super triangle
   _topology = new HalfEdgeTopology<Vertex, TriangleGeometry>( superTriangle[0], superTriangle[1], superTriangle[2] );

   /// Create the triangle's geometry and assign it
   Topology::Face * f = _topology->GetFace().element();
   f->geometry = TriangleGeometry(ZERO<Vertex>(), ZERO<Vertex>(), ZERO<Vertex>());  /// External triangle
   f->next->geometry = TriangleGeometry(superTriangle[0], superTriangle[1], superTriangle[2]);  /// Super triangle

   /// Cache away pointers to the super triangle vertices in the structure
   Topology::VertexIterator sv0 = _topology->GetVertex();
   Topology::VertexIterator sv1 = _topology->GetVertex().next();
   Topology::VertexIterator sv2 = _topology->GetVertex().next().next();

   // Now we incrementally add the points from the pointset
   for (Index v = 0; v < NUM_POINTS; ++v) InsertPoint(points[v]);

   /// And finally, we remove the super triangle vertices from the topology
   assert(sv2.element()->invariant()); _topology->RemoveVertex(sv2);
   assert(sv1.element()->invariant()); _topology->RemoveVertex(sv1);
   assert(sv0.element()->invariant()); _topology->RemoveVertex(sv0);
}



int32 locateVertex
   ( SmartDelaunay::Topology::Vertex * v
   , SmartDelaunay::Topology::Vertex * iterator)
{
   int32 i = 0;
   while (iterator != v) { iterator = iterator->next; i++; }
   return i;
}


Pair<Buffer<int32>, Buffer<SmartDelaunay::Vertex> > SmartDelaunay::Bake() const
{

   Buffer<int32> indices(_topology->NumFaces()*3 - 3);
   Buffer<Vertex> vertices(_topology->NumVertices());

   /// Iterate over vertices..
   Topology::VertexIterator v = _topology->GetVertex();
   for (Index i = 0; i < _topology->NumVertices(); i++, v++)
      vertices[i] = v.geometry();

   /// Now faces...
   Topology::FaceIterator f = _topology->GetFace().next();   // Skip the first (external)
   for (Index t = 0; t < _topology->NumFaces()-1; t++, f++)
   {
      /// Get this triangle's three vertices
      const Topology::HalfEdge * e = f.edge();
      for (Index v = 0; v < 3; ++v, e = e->next) 
      {
         int32 idx = locateVertex(e->end, _topology->GetVertex().element());
         indices[t*3 + v] = idx;
         ASSERT(indices[t*3+v] < _topology->NumVertices());
      }
   }

   return Pair<Buffer<int32>, Buffer<Vertex> >(indices, vertices);
}


SmartDelaunay::Topology::VertexIterator SmartDelaunay::LocatePoint(SmartDelaunay::Vertex v) const
{
   /// OK... for now we just do a linear search!  horrible!
   Topology::VertexIterator vItr = _topology->GetVertex();
   for (Index i = 0; i < _topology->NumVertices(); i++, vItr++)
      if (ALL(v == vItr.element()->geometry)) return vItr;
   return Topology::VertexIterator();
}





//////////////////////////////////////
// Periodic Delaunay Implementation //
//////////////////////////////////////

Burns::Pair<Collection<int32>, Collection<PeriodicSmartDelaunay::Vertex> > 
   PeriodicSmartDelaunay::Tessellate(const PeriodicSmartDelaunay::Vertex * points, const int NUM_POINTS)
{
   /// Assumptions:
   ///    points are located in the unit domain

   /// Replicate points to the 3x3 tile of unit domains. This is how we achieve the
   /// condition of periodicity
   /// 
   const int TOTAL_POINTS = 9*NUM_POINTS;

   Buffer<Vertex> rPoints(TOTAL_POINTS);
   for (Index i = 0; i < NUM_POINTS; i++)
   {
      rPoints[9*i+0] = points[i] + Vector2f(-1.0f, -1.0f);
      rPoints[9*i+1] = points[i] + Vector2f( 0.0f, -1.0f);
      rPoints[9*i+2] = points[i] + Vector2f( 1.0f, -1.0f);
      rPoints[9*i+3] = points[i] + Vector2f(-1.0f,  0.0f);
      rPoints[9*i+4] = points[i] + Vector2f( 0.0f,  0.0f);
      rPoints[9*i+5] = points[i] + Vector2f( 1.0f,  0.0f);
      rPoints[9*i+6] = points[i] + Vector2f(-1.0f,  1.0f);
      rPoints[9*i+7] = points[i] + Vector2f( 0.0f,  1.0f);
      rPoints[9*i+8] = points[i] + Vector2f( 1.0f,  1.0f);
   }

   /// Now we build the delaunay from these points


   auto tStart = Burns::readClockCounter();
   SmartDelaunay dt;
   dt.Tessellate(&rPoints[0], TOTAL_POINTS);
   auto tEnd = Burns::readClockCounter();      
   printf("Time to triangulate %i points: %.4f ms\n", TOTAL_POINTS, float(tEnd - tStart) / 1000000.0f);

   auto meshData = dt.Bake();
   Buffer<int32> triangulation = meshData.first;
   Buffer<Vector2f> vertices   = meshData.second;

   /// Ok, now we need to rip out redundant triangles
   AABBox2f UnitCube(Vector2f(1.0f), Vector2f(0.0f));
   Collection<int32> reducedTriangulation(triangulation.size());
   for (Index i = 0; i < triangulation.size()/3; ++i)
   {
      /// Gather the triangle, compute the bounds, check the lower corner
      Vertex v0 = vertices[triangulation[3*i+0]];
      Vertex v1 = vertices[triangulation[3*i+1]];
      Vertex v2 = vertices[triangulation[3*i+2]];

      Vertex lower = MIN3(v0, v1, v2);  /// Per component 3-way min
      if (UnitCube.inside(lower))
      {
         reducedTriangulation.add(triangulation[3*i+0]);
         reducedTriangulation.add(triangulation[3*i+1]);
         reducedTriangulation.add(triangulation[3*i+2]);      
      }
   }

   /// Now we have a reduced set of triangles.  We have to filter out the vertices
   /// from the vertex list that are no longer referenced. This is irritating. 
   /// First, compute vertex valences, then compute the adjacency graph
   InitializedBuffer<uint8> valences(vertices.size());
   ASSERT(valences[0] == 0);
   for (Index i = 0; i < reducedTriangulation.size(); ++i)
      valences[reducedTriangulation[i]]++;

   /// Now we use the adjacency graph to quickly lookup, for each vertex,
   /// the number of (and precisely which) triangles reference it. This allows
   /// us to adjust the index list as we compress the vertex list to cull 
   /// unused vertices
   Index newIndexCounter = 0;
   Collection<Vertex> reducedVertices(TOTAL_POINTS);
   for (Index i = 0; i < vertices.size(); ++i)
   {
      /// if this vertex is referenced, we will keep it
      if (valences[i] > 0)
      {
         /// Update all triangle's which reference this vertex so they
         /// use the new vertex
         reducedVertices.add(vertices[i]);
         for (Index ti = 0; ti < reducedTriangulation.size(); ++ti)
            if (reducedTriangulation[ti] == i) 
               reducedTriangulation[ti] = newIndexCounter;
         newIndexCounter++;
         ASSERT(reducedVertices.size() == newIndexCounter);   
      }
   }

   /// Returning the un-reduced vertex + index lists
   #if 1
   Collection<int32> fullIndexList(triangulation.size());
   for (Index i = 0; i < triangulation.size(); i++) fullIndexList.add(triangulation[i]);
   Collection<Vertex> fullVertexList(vertices.size());
   for (Index i = 0; i < vertices.size(); i++) fullVertexList.add(vertices[i]);
   return Pair<Collection<int32>, Collection<Vertex> >(fullIndexList, fullVertexList);
   #else
   return Pair<Collection<int32>, Collection<Vertex> >(reducedTriangulation, reducedVertices);
   #endif
}




//////////////////////////////////////////////
// Relaxed Periodic Delaunay Implementation //
//////////////////////////////////////////////

float minNeighborDistance
   ( SmartDelaunay::Topology::OneRingIterator ringIterator
   , SmartDelaunay::Topology::VertexIterator vIterator)
{
   float d_x = INF<float>();
   auto firstRingVertex = ringIterator;
   do 
   {
      float distance = (vIterator.element()->geometry - ringIterator.vertex()->geometry).lengthSquared();
      d_x = MIN(distance, d_x);
      ringIterator++;
   } while (ringIterator != firstRingVertex);

   return SQRT(d_x);
}


Vector2f lowerLeftCorner(SmartDelaunay::Topology::FaceIterator fIterator)
{
   /// ignore triangles whose lower left corner is not in unit square region
   auto edge = fIterator.edge();

   AABBox2f bounds = AABBox2f::invertedBounds();
   do 
   {
      bounds |= edge->end->geometry;
      edge = edge->next;
   } while (edge != fIterator.edge());

   return bounds.ll;
}


Burns::Pair<Collection<int32>, Collection<RelaxedPeriodicSmartDelaunay::Vertex> > 
   RelaxedPeriodicSmartDelaunay::Tessellate(const RelaxedPeriodicSmartDelaunay::Vertex * points, const int NUM_POINTS)
{
   /// Assumptions:
   ///    points are located in the unit domain

   const int TOTAL_POINTS = NUM_POINTS*9 + 4;

   SmartDelaunay dt;

   Vector2f cloneOffsets[] = 
   {
      Burns::Vector2f(-1.0f, -1.0f),
      Burns::Vector2f( 0.0f, -1.0f),
      Burns::Vector2f( 1.0f, -1.0f),
      Burns::Vector2f(-1.0f,  0.0f),
      Burns::Vector2f( 0.0f,  0.0f),
      Burns::Vector2f( 1.0f,  0.0f),
      Burns::Vector2f(-1.0f,  1.0f),
      Burns::Vector2f( 0.0f,  1.0f),
      Burns::Vector2f( 1.0f,  1.0f)
   };

   /// Replicate points to the 3x3 tile of unit domains. This is how we achieve the
   /// condition of periodicity
   {
      Buffer<Vertex> rPoints(TOTAL_POINTS);
      for (Index i = 0; i < NUM_POINTS; i++)
         for (int c = 0; c < 9; ++c) 
            rPoints[9*i+c] = points[i] + cloneOffsets[c];

      /// A super-quad to contain the whole thing, making insertions in the interval
      /// [-1, 2]^2 safe 
      rPoints[TOTAL_POINTS-4] = Vector2f(-2.0f, -2.0f);
      rPoints[TOTAL_POINTS-3] = Vector2f(-2.0f,  3.0f);
      rPoints[TOTAL_POINTS-2] = Vector2f( 3.0f,  3.0f);
      rPoints[TOTAL_POINTS-1] = Vector2f( 3.0f, -2.0f);

      /// Now we build the delaunay from these points
      dt.Tessellate(&rPoints[0], TOTAL_POINTS);
   }


   //dt.Print();

   ////////////////
   // Relaxation //
   ////////////////

   /// Tis loop is exactly one iteration... ugh

   //printf("Beginnning relaxation iteration 0..\n");


   /// OK, big problem. We're gonna have to implement the removeVertex method on
   /// the delaunay class, because we can't create a whole new delaunay
   /// mesh, since that invalidates our vertex iterator(s) !

   for (int iteration = 0; iteration < 3; ++iteration)
   {
      String filename = String("Iteration_") + iteration + ".txt";
      //dt.DumpPolygonsToMathematicaFile(*filename);

      printf("Iteration %i..\n", iteration);

      for (int i = 0; i < dt.GetTopology()->NumVertices(); ++i)
      {
         /// Iterate to the i'th vertex. There is no random-access,
         /// but since the DT is rebuilt on every iteration, vertex
         /// iterators are invalidated and do not persist between 
         /// loop iterations =(
         const SmartDelaunay::Topology * topology = dt.GetTopology();
         auto vIterator = topology->GetVertex();
         for (int v = 0; v < i; ++v) vIterator++;

         /// Iterate over vertices, calculating their local min distance d_x
         ASSERT(vIterator.element());
         Vertex pivotVertex = vIterator.geometry();
         if (AABBox2f::unitBounds().inside(pivotVertex))
         {
            /// Compute d_x, the smallest distance to any of this vertex's
            /// neighbors in the delaunay graph. Traverse all neighbor points
            /// and calculate this min distance
            auto ringIterator = topology->GetOneRingIterator(vIterator);
            float d_x = minNeighborDistance(ringIterator, vIterator);

            #if 0
            dt.RemoveVertex( ... );

            #else
            /// If we had a remove operation, we'd just do that here, but for
            /// now we'll rebuild the tessellation from scratch with a reduced
            /// point set..
            /// Build a list of the current vertex list, minus this point and its
            /// 8 periodic clones
            Collection<Vertex> reducedSet(9*NUM_POINTS);
            auto vItr = topology->GetVertex();
            for (int i = 0; i < topology->NumVertices(); ++i, vItr++)
            {
               /// Insert it into our reduced list, iff it is not one of the 9 points
               bool oneOfUs = false;
               for (int c = 0; c < 9; ++c)
                  if (ALL(vItr.geometry() == pivotVertex + cloneOffsets[c])) 
                     oneOfUs = true;

               if (!oneOfUs) reducedSet.add(vItr.geometry());
            }
            ASSERT(reducedSet.size() == TOTAL_POINTS - 9);

            dt.Tessellate(&reducedSet[0], reducedSet.size());

            //String filename = String("Iteration_") + iteration + "r.txt";
            //dt.DumpPolygonsToMathematicaFile(*filename);

            /// Now what?  We search the new triangulation for the biggest circumcircle
            auto t = dt.GetTopology();

            /// We skip the first triangle, as that is the "exterior" triangle. Recall
            /// that topologically this is a closed surface
            auto tItr = t->GetFace().next().next();
            float largestRadius = -1.0f;
            Vector2f largestCenter;
            for (int c = 0; c < t->NumFaces(); c++, tItr++)
            {
               /// Skip triangles who circumcenter is not in the unit domain
               if (!AABBox2f::unitBounds().inside(tItr.geometry().center)) continue;
               if (tItr.geometry().radius > largestRadius)
               {
                  largestRadius = tItr.geometry().radius;
                  largestCenter = tItr.geometry().center;
               }
            }
            ASSERT(largestRadius > 0.0f);

            /// What happens if this insertion fails ?
            /// Ok, now we insert new points at this triangle's center, and at the other
            /// eight locations in the 3x3 unit domain
            for (int c = 0; c < 9; ++c) 
               dt.InsertPoint(largestCenter + cloneOffsets[c]);
            #endif
         }
      }

   } // iterations

   /////////////////////
   // Build Mesh Tile //
   /////////////////////

   auto meshData = dt.Bake();
   Buffer<int32> triangulation = meshData.first;
   Buffer<Vector2f> vertices   = meshData.second;

   /// Ok, now we need to rip out redundant triangles
   AABBox2f UnitCube(Vector2f(1.0f), Vector2f(0.0f));
   Collection<int32> reducedTriangulation(triangulation.size());
   for (Index i = 0; i < triangulation.size()/3; ++i)
   {
      /// Gather the triangle, compute the bounds, check the lower corner
      Vertex v0 = vertices[triangulation[3*i+0]];
      Vertex v1 = vertices[triangulation[3*i+1]];
      Vertex v2 = vertices[triangulation[3*i+2]];

      Vertex lower = MIN3(v0, v1, v2);  /// Per component 3-way min
      if (UnitCube.inside(lower))
      {
         reducedTriangulation.add(triangulation[3*i+0]);
         reducedTriangulation.add(triangulation[3*i+1]);
         reducedTriangulation.add(triangulation[3*i+2]);      
      }
   }

   /// Now we have a reduced set of triangles.  We have to filter out the vertices
   /// from the vertex list that are no longer referenced. This is irritating. 
   /// First, compute vertex valences, then compute the adjacency graph
   InitializedBuffer<uint8> valences(vertices.size());
   ASSERT(valences[0] == 0);
   for (Index i = 0; i < reducedTriangulation.size(); ++i)
      valences[reducedTriangulation[i]]++;

   /// Now we use the adjacency graph to quickly lookup, for each vertex,
   /// the number of (and precisely which) triangles reference it. This allows
   /// us to adjust the index list as we compress the vertex list to cull 
   /// unused vertices
   Index newIndexCounter = 0;
   Collection<Vertex> reducedVertices(9*NUM_POINTS);
   for (Index i = 0; i < vertices.size(); ++i)
   {
      /// if this vertex is referenced, we will keep it
      if (valences[i] > 0)
      {
         /// Update all triangle's which reference this vertex so they
         /// use the new vertex
         reducedVertices.add(vertices[i]);
         for (Index ti = 0; ti < reducedTriangulation.size(); ++ti)
            if (reducedTriangulation[ti] == i) 
               reducedTriangulation[ti] = newIndexCounter;
         newIndexCounter++;
         ASSERT(reducedVertices.size() == newIndexCounter);   
      }
   }

   /// Returning the un-reduced vertex + index lists
#if 0
   Collection<int32> fullIndexList(triangulation.size());
   for (Index i = 0; i < triangulation.size(); i++) fullIndexList.add(triangulation[i]);
   Collection<Vertex> fullVertexList(vertices.size());
   for (Index i = 0; i < vertices.size(); i++) fullVertexList.add(vertices[i]);
   return Pair<Collection<int32>, Collection<Vertex> >(fullIndexList, fullVertexList);
#else
   return Pair<Collection<int32>, Collection<Vertex> >(reducedTriangulation, reducedVertices);
#endif
}
