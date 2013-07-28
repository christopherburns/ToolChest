#ifndef DELAUNAY_H
#define DELAUNAY_H

#include "Bounds.h"

#include <sstream>

class PeriodicDelaunay;

//////////////////////////////////////////////////////////////////////////////
//                           Delaunay Triangulator                          //
//////////////////////////////////////////////////////////////////////////////

class Delaunay
{
public:

   friend class PeriodicDelaunay;

   ///////////////
   // Interface //
   ///////////////

   typedef Vector2f Vertex;

   /// Constructor generates delaunay mesh
   inline Delaunay(const Vertex * points, const int NUM_POINTS)
   { tessellate(points, NUM_POINTS); }

   /// Accessors
   inline const Vertex * GetPoints() const { return &_points[0]; }
   inline const int * GetTriangles() const { return &_indices[0]; }

   inline int GetNumPoints() const { return _points.Size(); }
   inline int GetNumTriangles() const { return _indices.Size() / 3; }
   
   void Print() const;

private:

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
         if      (i0 < rhs.i0) return true;
         else if (rhs.i0 < i0) return false;
         else // lower bound is equal, check upper edge
         {
            if      (i1 < rhs.i1) return true;
            else if (rhs.i1 < i1) return false;
            else return false;
         }
      }
   };


   typedef Collections::Mutable::LinkedList<Triangle> TriangleList;
   typedef Collections::Mutable::Array<Vertex> PointList;
   typedef Collections::Immutable::Array<int> IndexList;

   /// Clears all existing data, generates a brand new triangulation
   void tessellate(const Vertex * points, const int NUM_POINTS);

   /// Adds a new point to the triangulation, must be INSIDE at least one
   /// existing triangle in the tessellation though
   void insertPoint(TriangleList& tList, Vertex v, int i);

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


   ////////////////////////
   // Triangulation Data //
   ////////////////////////

   PointList _points;
   IndexList _indices;
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


//class Printer { public:  void operator() (const Delaunay::Edge& e) const { printf("[%i %i] ", e.i0, e.i1); } };
//#define STREAM_OUT_DEF o << "[ "; Printer p; c.ForEach(p); o << "]"; return o;
//inline std::ostream& operator << (std::ostream& o, const Collections::Mutable::TreeSet<Delaunay::Edge>& c) { STREAM_OUT_DEF }


//inline std::ostream& operator << (std::ostream& o, const Delaunay::Edge& e)
//{
//   o << "[" << e.i0 << ", " << e.i1 << "]";
//   return o; 
//}

inline void Delaunay::insertPoint(TriangleList& tList, Vertex v, int i)
{
   typedef Collections::Mutable::TreeSet<Edge> EdgeSet;

   EdgeSet edgeSet = EdgeSet(); //edgeSet += Edge(); /// WTF?
   auto t = tList.GetIterator();

   _points[i] = v;

   /// This is why the algo is N^2 --> #triangles * #points

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

         tList.Remove(t); // t is automatically incremented
      }
      else t.Next();
   }

   /// Now we can iterate over these remaining edges, and build new triangles
   for (auto e = edgeSet.GetIterator(); e.HasNext(); e.Next())
      tList += Triangle
         ( _points[i], _points[e.Peek().i0], _points[e.Peek().i1]
         , i, e.Peek().i0, e.Peek().i1);
}


inline void Delaunay::tessellate(const Vertex * points, const int NUM_POINTS)
{
   if (NUM_POINTS < 3) return;

   _points = PointList(NUM_POINTS + 3);           // Exact allocation
   TriangleList triangles = TriangleList(NUM_POINTS + NUM_POINTS/4);    // Approx. allocation

   /// Compute the bounding box of the data, and build the list of points
   AABBox2f bounds = AABBox2f::InvertedBounds();
   for (int i = 0; i < NUM_POINTS; ++i) bounds |= points[i];

   /// Generate the super triangle
   Vertex superTriangle[3];
   generateSuperTriangle(bounds, superTriangle);
   for (int i = 0; i < 3; ++i) _points[i] = superTriangle[i];

   /// We start our tessellation with the super triangle
   triangles += Triangle
      ( superTriangle[0], superTriangle[1], superTriangle[2], 0, 1, 2 );

   /// Build the delaunay triangulation incrementally
   for (int v = 0; v < NUM_POINTS; ++v) 
      insertPoint(triangles, points[v], v + 3);

   /// Remove triangles referencing the super-triangle verts, and simultaneously
   /// rebase the vertex references in anticipation of removal of the first
   /// three (super-triangle) vertices
   auto indexListBuilder = Collections::Immutable::Array<int>::Builder();
   auto t = triangles.GetIterator();
   while (t.HasNext())
   {
      /// If *any* of t's vertices match *any* of the super triangle's
      /// vertices, then this triangle is removed. Super triangle vertices
      /// are at indices 0, 1, 2
      bool cullMe = false;
      for (int tv = 0; tv < 3; ++tv)
         cullMe |= (t.Peek().vIndices[tv] < 3); // Mark for removal if necessary

      if (cullMe) triangles.Remove(t);
      else 
      {
         for (int i = 0; i < 3; ++i) assert(t.Peek().vIndices[i]-3 >= 0);

         indexListBuilder.AddElement(t.Peek().vIndices[0]-3);
         indexListBuilder.AddElement(t.Peek().vIndices[1]-3);
         indexListBuilder.AddElement(t.Peek().vIndices[2]-3);
         t.Next(); 
      }
   }
   _indices = indexListBuilder.Result();


   /// Remove the super-triangle vertices from the list ...
   /// This is basically a copy, and we visit each triangle to
   /// rebase the indices (-3)
   PointList pList = PointList(NUM_POINTS);
   for (int i = 3; i < _points.Size(); ++i) pList[i-3] = _points[i];
   _points = pList; //< Copy by reference
}



inline void Delaunay::Print() const
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


#endif // DELAUNAY_H
