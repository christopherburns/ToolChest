#pragma once

#ifndef DELAUNAY_H
#define DELAUNAY_H

#include "../burns/Burns.h"
#include "Bounds.h"



//////////////////////////////////////////////////////////////////////////////
//                           Delaunay Triangulator                          //
//////////////////////////////////////////////////////////////////////////////

class Delaunay
{
public:

   ///////////////
   // Interface //
   ///////////////

   typedef Vector2f Vertex;

   /// Clears all existing data, generates a brand new triangulation
   void Tessellate(const Vertex * points, const int NUM_POINTS);

   /// Adds a new point to the triangulation, must be INSIDE at least one
   /// existing triangle in the tessellation though
   void InsertPoint(Vertex v, int i);

private:
   static void generateSuperTriangle(AABBox2f bounds, Vertex verts[3]);
   
   enum Compare { LESS_THAN, EQUAL, GREATER_THAN };
   inline static Compare compareVertices(Vertex p0, Vertex p1)
   {
      for (int i = 0; i < TypeInfo<Vertex>::Width; ++i)
      {
              if (p0[i] == p1[i]) continue;
         else if (p0[i] >  p1[i]) return GREATER_THAN;
         else if (p0[i] <  p1[i]) return LESS_THAN ;
      }
      return EQUAL;
   }

   friend class Triangle;
   friend class Edge;

   typedef Collections::Mutable::Array<Vertex> PointList;


   ///////////////////
   // Simplex Class //
   ///////////////////

   class Triangle
   {
   public:
      int vIndices[3];
      Vertex center;                //< Circumcircle center
      float radius, radiusSq;       //< Circumcircle radius

      Triangle
         ( Vertex v0, Vertex v1, Vertex v2
         , int i0, int i1, int i2);
     
      inline bool IsInCircumcircle(Vertex p) const
      { return (p - center).lengthSquared() <= radiusSq; }
      
      inline int operator [] (int i) const { return vIndices[i]; }

      inline String ToString() const
      { 
         return String("[ (") 
                    + String(vIndices[0]) 
            + ") (" + String(vIndices[1]) 
            + ") (" + String(vIndices[2]) + ") ]"; 
      }

      inline bool operator == (const Triangle& rhs) const
      {
         return vIndices[0] == rhs.vIndices[0] 
            &&  vIndices[1] == rhs.vIndices[1] 
            &&  vIndices[2] == rhs.vIndices[2];
      }
      
   private:
      inline Vertex circumcenter(Vertex v0, Vertex v1, Vertex v2);
   };

   typedef Collections::Mutable::LinkedList<Triangle> TriangleList;


   ////////////////
   // Edge Class //
   ////////////////

   class Edge
   {
   public:

      int i0, i1;
      inline Edge() {}
      inline Edge(int p0, int p1) // : i0(p0), i1(p1) {}
      {
         /// Store these in sorted order, to make comparisons cheaper
         i0 = MIN(p0, p1); 
         i1 = MAX(p0, p1);
         assert(p0 != p1); /// Degenerate edges not allowed
      }

      inline String ToString() const
      { return String("[ (") + String(i0) + ") (" + String(i1) + ") ]"; }

      inline bool operator == (const Edge& rhs) const
      { return (i0 == rhs.i0 && i1 == rhs.i1); }
      inline bool operator != (const Edge& rhs) const 
      { return !operator == (rhs); }

      /// It is critical that !(a < b) && !(b < a) == (a != b)
      inline bool operator < (const Edge& rhs) const
      {
         //Compare c0 = Delaunay::compareVertices(v0, rhs.v0);
         //Compare c1 = Delaunay::compareVertices(v1, rhs.v1);
         //return (c0 == EQUAL ? c1 == LESS_THAN : c0 == LESS_THAN);
         if (i0 < rhs.i0) return true;
         else if (rhs.i0 < i0) return false;
         else // lower bound is equal, check upper edge
         {
            if (i1 < rhs.i1) return true;
            else if (rhs.i1 < i1) return true;
            else return false;
         }
      }
   };


   ////////////////////////
   // Triangulation Data //
   ////////////////////////

   PointList    _points;
   TriangleList _simplexMesh;
};

////////////////////
// Implementation //
////////////////////

inline Delaunay::Vertex 
Delaunay::Triangle::circumcenter(Delaunay::Vertex p1, Delaunay::Vertex p2, Delaunay::Vertex p3)
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
inline Delaunay::Triangle::Triangle
   ( Vertex v0, Vertex v1, Vertex v2
   , int i0, int i1, int i2 )
{
   vIndices[0] = i0; vIndices[1] = i1; vIndices[2] = i2;
   
   center = circumcenter(v0, v1, v2);
   radiusSq = (v0 - center).lengthSquared();
   radius = SQRT(radiusSq);
   
   //printf("triangle: %s, %s, %s,   circumcenter = %s\n", *String(v0->coordinate()), *String(v1->coordinate()), *String(v2->coordinate()), *String(center));
}

inline void Delaunay::generateSuperTriangle(AABBox2f bounds, Delaunay::Vertex verts[3])
{
   static const float sqrt3 = SQRT<float>(3.0f);

   auto delta = bounds.ur - bounds.ll;

   // Make the bounding box slightly bigger, just to feel safe.
   delta *= 1.01f;

   bounds.ll -= delta;
   bounds.ur += delta;
   delta += 2 * delta;

   verts[0] = Vertex(bounds.ll.x() - delta.y() * sqrt3 / 3.0f, bounds.ll.y());
   verts[1] = Vertex(bounds.ur.x() + delta.y() * sqrt3 / 3.0f, bounds.ll.y());
   verts[2] = Vertex((bounds.ll.x() + bounds.ur.x()) * 0.5f, bounds.ur.y() + delta.x() * sqrt3 * 0.5f);
}


inline void Delaunay::InsertPoint(Vertex v, int i)
{
   typedef Collections::Mutable::TreeSet<Edge> EdgeSet;

   EdgeSet edgeSet = EdgeSet(); //edgeSet += Edge(); /// WTF?
   auto t = _simplexMesh.GetIterator();

   _points[i] = v;

   /// This is why the algo is N^2 --> #triangles * #points

   //while (!t.isNull())
   while (t.HasNext())
   {
      auto triangle = t.Peek();
      if (triangle.IsInCircumcircle(v))
      {
         Edge e0 = Edge(triangle.vIndices[0], triangle.vIndices[1]);
         Edge e1 = Edge(triangle.vIndices[1], triangle.vIndices[2]);
         Edge e2 = Edge(triangle.vIndices[2], triangle.vIndices[0]);

         if (!edgeSet.Contains(e0)) edgeSet += e0;
         else edgeSet -= e0;

         if (!edgeSet.Contains(e1)) edgeSet += e1;
         else edgeSet -= e1;

         if (!edgeSet.Contains(e2)) edgeSet += e2;
         else edgeSet -= e2;

         _simplexMesh.Remove(t); // t is automatically incremented
      }
      else t.Next();
   }

   /// Now we can iterate over these remaining edges, and build new triangles
   for (auto e = edgeSet.GetIterator(); e.HasNext(); e.Next())
      _simplexMesh += Triangle
         ( _points[i], _points[e.Peek().i0], _points[e.Peek().i1]
         , i, e.Peek().i0, e.Peek().i1);
}

inline void Delaunay::Tessellate(const Vertex * points, const int NUM_POINTS)
{
   if (NUM_POINTS < 3) return;

   _points = PointList(NUM_POINTS + 3);                       // Exact allocation
   _simplexMesh = TriangleList(NUM_POINTS + NUM_POINTS/4);    // Approx. allocation

   /// Compute the bounding box of the data, and build the list of points
   AABBox2f bounds = AABBox2f::invertedBounds();
   for (int i = 0; i < NUM_POINTS; ++i) bounds |= points[i];

   /// Generate the super triangle
   Vertex superTriangle[3];
   generateSuperTriangle(bounds, superTriangle);
   for (int i = 0; i < 3; ++i) _points[i] = superTriangle[i];

   /// We start our tessellation with the super triangle
   _simplexMesh += Triangle
      ( superTriangle[0], superTriangle[1], superTriangle[2], 0, 1, 2 );

   /// Build the delaunay triangulation incrementally
   for (int v = 0; v < NUM_POINTS; ++v) InsertPoint(points[v], v + 3);

   /// Remove triangles referencing the super-triangle verts
   auto t = _simplexMesh.GetIterator();
   while (t.HasNext())
   {
      /// If *any* of t's vertices match *any* of the super triangle's
      /// vertices, then this triangle is removed. Super triangle vertices
      /// are at indices 0, 1, 2
      bool cullMe = false;
      for (int tv = 0; tv < 3; ++tv)
         cullMe |= (t.Peek().vIndices[tv] < 3);

      if (cullMe) _simplexMesh.Remove(t);
      else t.Next(); 
   }


   /// Remove the super-triangle vertices from the list ...

   printf("Complete!\n");
   printf("NumVertices: %i\n", _points.Size());
   printf("NumTriangles: %i\n", _simplexMesh.Size());


   printf("points = {\n");
   for (int i = 0; i < _points.Size(); ++i)
      printf("{%.3f, %.3f}, ", _points[i].x(), _points[i].y());
   printf("}\n");

   printf("edges = {\n");
   for (auto tItr = _simplexMesh.GetIterator(); tItr.HasNext(); )
   {
      auto t = tItr.Next();
      printf
         ( "{%i, %i}, {%i, %i}, {%i, %i}, "
         , t.vIndices[0]+1, t.vIndices[1]+1, t.vIndices[1]+1
         , t.vIndices[2]+1, t.vIndices[2]+1, t.vIndices[0]+1 );
   }

   printf("}\n");

   //_points.Remove(_points.GetIterator());
   //_points.Remove(_points.GetIterator());
   //_points.Remove(_points.GetIterator());
}

#endif // DELAUNAY_H
