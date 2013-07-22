#ifndef PERIODIC_DELAUNAY_H
#define PERIODIC_DELAUNAY_H

#include "Delaunay.h"

//////////////////////////////////////////////////////////////////////////////
//                      Periodic Delaunay Triangulator                      //
//////////////////////////////////////////////////////////////////////////////


class PeriodicDelaunay
{
public:
   ///////////////
   // Interface //
   ///////////////

   typedef Vector2f Vertex;

   /// Constructor generates delaunay mesh
   inline PeriodicDelaunay(const Vertex * points, const int NUM_POINTS);

   /// Accessors
   inline const Vertex * GetPoints() const { return &_points[0]; }
   inline const int * GetTriangles() const { return &_indices[0]; }

   inline int GetNumPoints() const { return _points.Size(); }
   inline int GetNumTriangles() const { return _indices.Size() / 3; }
   
   void Print() const;

private:

   ////////////////////////
   // Triangulation Data //
   ////////////////////////

   Collections::Vector<Vertex> _points;
   Collections::Vector<int32> _indices;
};


inline void PeriodicDelaunay::Print() const
{
   printf("points = {\n");
   for (int i = 0; i < _points.Size()-1; ++i)
      printf("{%.3f, %.3f}, ", _points[i].x(), _points[i].y());
   printf("{%.3f, %.3f}", _points[_points.Size()-1].x(), _points[_points.Size()-1].y());
   printf("}\n");

   printf("edges = {\n");
   for (auto tItr = _indices.GetIterator(); tItr.HasNext(); )
   {
      auto i0 = tItr.Next();
      auto i1 = tItr.Next();
      auto i2 = tItr.Next();
      if (tItr.HasNext())
         printf
            ( "{%i, %i}, {%i, %i}, {%i, %i}, "
            , i0+1, i1+1, i1+1
            , i2+1, i2+1, i0+1 );
      else
         printf
            ( "{%i, %i}, {%i, %i}, {%i, %i}"
            , i0+1, i1+1, i1+1
            , i2+1, i2+1, i0+1 );            
   }   
   printf("}\n");
}



inline PeriodicDelaunay::PeriodicDelaunay(const PeriodicDelaunay::Vertex * points, const int NUM_POINTS)
{
   /// Assumptions:
   ///    points are located in the unit domain

   /// Replicate points to the 3x3 tile of unit domains. This is how we achieve the
   /// condition of periodicity
   Collections::Mutable::Array<Vertex> rPoints(9*NUM_POINTS);
   for (int i = 0; i < NUM_POINTS; i++)
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
   Delaunay dt(&rPoints[0], NUM_POINTS*9);

   auto triangulation = dt.GetTriangles();
   auto tPoints = dt.GetPoints();

   /// Ok, now we need to rip out redundant triangles
   AABBox2f UnitCube(Vector2f(1.0f), Vector2f(0.0f));
   _indices = Collections::Vector<int32>::Construct(dt.GetNumTriangles()/4);
   for (int i = 0; i < dt.GetNumTriangles(); ++i)
   {
      /// Gather the triangle, compute the bounds, check the lower corner
      Vertex v0 = tPoints[triangulation[3*i+0]];
      Vertex v1 = tPoints[triangulation[3*i+1]];
      Vertex v2 = tPoints[triangulation[3*i+2]];

      Vertex lower = MIN(v0, MIN(v1, v2));  /// Per component 3-way min
      if (UnitCube.inside(lower))
      {
         _indices.Push(triangulation[3*i+0]);
         _indices.Push(triangulation[3*i+1]);
         _indices.Push(triangulation[3*i+2]);      
      }
   }

   /// Now we have a reduced set of triangles.  We have to filter out the vertices
   /// from the vertex list that are no longer referenced. This is irritating. 
   /// First, compute vertex valences, then compute the adjacency graph
   Collections::Mutable::Array<uint8> valences(rPoints.Size());
   memset(&valences[0], 0, rPoints.Size());
   for (int i = 0; i < _indices.Size(); ++i)
      valences[_indices[i]]++;

   /// Now we use the adjacency graph to quickly lookup, for each vertex,
   /// the number of (and precisely which) triangles reference it. This allows
   /// us to adjust the index list as we compress the vertex list to cull 
   /// unused vertices
   int newintCounter = 0;
   _points = Collections::Vector<Vertex>::Construct(2*NUM_POINTS);
   for (int i = 0; i < rPoints.Size(); ++i)
   {
      /// if this vertex is referenced, we will keep it
      if (valences[i] > 0)
      {
         /// Update all triangle's which reference this vertex so they
         /// use the new vertex
         _points.Push(rPoints[i]);
         for (int ti = 0; ti < _indices.Size(); ++ti)
            if (_indices[ti] == i) _indices[ti] = newintCounter;
         newintCounter++;
         assert(_points.Size() == newintCounter);   
      }
   }
}


#endif