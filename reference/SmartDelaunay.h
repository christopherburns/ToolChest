
#pragma once

#include <core/Burns.h>
#include "HalfEdgeTopology.h"

class RelaxedPeriodicSmartDelaunay;
class PeriodicSmartDelaunay;

class SmartDelaunay
{
public:

   ///////////////
   // Interface //
   ///////////////

   class TriangleGeometry;

   typedef Burns::Vector2f Vertex;
   typedef HalfEdgeTopology<Burns::Vector2f, TriangleGeometry> Topology;

   INLINE SmartDelaunay() : _topology(NULL) {}
   INLINE ~SmartDelaunay() { if (_topology) delete _topology; }

   FINLINE const Topology * GetTopology() const { return _topology; }

   /// Clears all existing data, generates a brand new triangulation
   void Tessellate(const Vertex * points, const int NUM_POINTS);

   /// Adds a new point to the triangulation, must be INSIDE at least one
   /// existing triangle in the tessellation though
   void InsertPoint(Vertex v);

   /// Removes a point from the triangulation, re-triangulating the region
   //void RemovePoint(Topology::VertexIterator v);

   /// We rely on strict equality.  To account for floating point issues,
   /// we may decide to provide a version that takes an epsilon argument.
   Topology::VertexIterator LocatePoint(Vertex v) const;

   /// Bakes the tessellation into an indexed vertex list
   Burns::Pair< Burns::Buffer<int32>, Burns::Buffer<Vertex> > Bake() const;

   void Print() const { _topology->Print(); }
   void DumpPolygonsToMathematicaFile(const char * fileName) const;


   ///////////////////
   // Simplex Class //
   ///////////////////

   class TriangleGeometry
   {
   public:
      Vertex center;           //< Circumcircle center
      float radius, radiusSq;  //< Circumcircle radius

      FINLINE TriangleGeometry() : center(Burns::ZERO<Vertex>()), radius(0.0f), radiusSq(0.0f) {}
      TriangleGeometry(Vertex v0, Vertex v1, Vertex v2);

      FINLINE bool isInCircumcircle(Vertex p) const
      { return (p - center).lengthSquared() <= radiusSq; }

      INLINE Burns::String toString() const
      { return Burns::String("{ center = ") + center.toString() + ", radius = " + radius + " }"; }

   private:
      INLINE Vertex computeCircumcenter(Vertex v0, Vertex v1, Vertex v2);
   };


private:
   void generateSuperTriangle(Burns::AABBox2f bounds, Vertex verts[3]);
   void clear();
   Burns::String faceToString(Topology::FaceIterator fItr) const;

private:
   Topology * _topology;
};




//////////////////////////////////////////////////////////////////////////////
//                      Periodic Delaunay Triangulator                      //
//////////////////////////////////////////////////////////////////////////////


class PeriodicSmartDelaunay
{
public:
   typedef SmartDelaunay::Vertex Vertex;
   Burns::Pair< Burns::Collection<int32>, 
                Burns::Collection<Vertex> > 
   Tessellate(const Vertex * points, const int NUM_POINTS);
};


//////////////////////////////////////////////////////////////////////////////
//                 Relaxed Periodic Delaunay Triangulator                   //
//////////////////////////////////////////////////////////////////////////////

class RelaxedPeriodicSmartDelaunay
{
public:
   typedef SmartDelaunay::Vertex Vertex;
   Burns::Pair< Burns::Collection<int32>, 
                Burns::Collection<Vertex> > 
      Tessellate(const Vertex * points, const int NUM_POINTS);
};

    


