#ifndef HALF_EDGE_TOPOLOGY_H
#define HALF_EDGE_TOPOLOGY_H

#pragma once

#include <core/Burns.h>


/// The PointGeometry defines the geometry of vertices, FaceGeometry that of
/// the polgyons. 

template <class PointGeometry, class FaceGeometry> class HalfEdgeTopology
{
public:

   class HalfEdge;
   class Vertex;
   class Face;

   /// The absolute minimum topology for this data structure is a
   /// single triangle (and its external triangle twin). This structure
   /// consists of two faces, six half edges (Edge), and three vertices.

   /// HalfEdges form circular, singly-linked lists within each face. Each
   /// half edge is linked to its twin, which must exist since we model only
   /// topologically closed 2-manifolds
   struct HalfEdge
   {
      Vertex * end;
      Face * face;
      HalfEdge * twin, * next;

      bool invariant() const;
   };

   /// Each vertex and face is a node in a linked list of vertices and
   /// faces, as well as possessed of pointers to the connected edge 
   /// loops. The order of elements in the linked list is entirely 
   /// arbitrary and is possessed of no topological or geometric 
   /// information whatsoever
   struct Vertex 
   { 
      PointGeometry geometry; 
      HalfEdge * edge; 
      Vertex * next, * prev; 

      typedef PointGeometry GeometryType;
      bool invariant() const;
   };

   struct Face 
   { 
      FaceGeometry geometry;
      HalfEdge * edge; 
      Face * next, * prev; 

      typedef FaceGeometry GeometryType;
      bool invariant() const;
   };



   ////////////////////////////////////////////////////////////////////////////
   //                                Iterators                               //
   ////////////////////////////////////////////////////////////////////////////

   /// Iterates around the vertices that form the 1-ring around a pivot vertex
   /// Also can be used as an iterator on the faces around the vertex's 1-ring
   class OneRingIterator
   {
   private:
      FINLINE OneRingIterator(Vertex * pivot) 
         : _pivot(pivot), _toCurrentVertex(pivot->edge) {}
   public:
      FINLINE OneRingIterator() : _pivot(NULL), _toCurrentVertex(NULL) {}
      FINLINE OneRingIterator(const OneRingIterator& vi) 
         : _pivot(vi._pivot), _toCurrentVertex(vi._toCurrentVertex) {}

      FINLINE bool isNull() const { return !(bool)_pivot; }
      FINLINE OneRingIterator next() const
      { 
         ASSERT(_toCurrentVertex); ASSERT(_toCurrentVertex->invariant());
         ASSERT(_pivot);           ASSERT(_pivot->invariant());

         OneRingIterator r(*this);
         r._toCurrentVertex = r._toCurrentVertex->twin->next;
         return r;
      } 
      FINLINE void operator ++ (int) { *this = next(); }      /// Postfix

      FINLINE bool operator == (const OneRingIterator& rhs) const 
      { return _pivot == rhs._pivot && _toCurrentVertex == rhs._toCurrentVertex; }
      FINLINE bool operator != (const OneRingIterator& rhs) const 
      { return _pivot != rhs._pivot || _toCurrentVertex != rhs._toCurrentVertex; }
      
      FINLINE Face * face()     const { ASSERT(_toCurrentVertex); return _toCurrentVertex->face; }
      FINLINE Vertex * vertex() const { ASSERT(_toCurrentVertex); return _toCurrentVertex->end;  }

      friend class HalfEdgeTopology;

   private:
      Vertex * _pivot;
      HalfEdge * _toCurrentVertex;
   };

   /// Legal instantiations: Vertex, Face
   template <class Element> class Iterator
   {
   private:
      FINLINE Iterator(Element * f) : _element(f) {}

   public:
      FINLINE Iterator() : _element(NULL) {}
      FINLINE Iterator(const Iterator& i) : _element(i._element) {}

      FINLINE bool isNull()       const { return !(bool)_element; }
      FINLINE Iterator next()     const 
      { ASSERT(_element); ASSERT(_element->next); return Iterator(_element->next); } 
      FINLINE Iterator previous() const 
      { ASSERT(_element); ASSERT(_element->prev); return Iterator(_element->prev); } 

      /// Note these cannot be used as R-values
      FINLINE void operator ++ (int) { *this = next(); }      /// Postfix
      FINLINE void operator -- (int) { *this = previous(); }

      FINLINE bool operator == (const Iterator& rhs) const { return _element == rhs._element; }
      FINLINE bool operator != (const Iterator& rhs) const { return _element != rhs._element; }

      FINLINE Element * element() const { return _element; }
      FINLINE const HalfEdge * edge() const { ASSERT(_element); return _element->edge; }
      FINLINE const typename Element::GeometryType& geometry() const 
      { ASSERT(_element); return _element->geometry; }

      friend class HalfEdgeTopology;

   private:
      Element * _element;  /// NULL if "end"
   };


   typedef Iterator<Face> FaceIterator;
   typedef Iterator<Vertex> VertexIterator;


private:
   /// The entire structure can be traversed from a single edge
   HalfEdge * _headEdge;
   int _numFaces, _numVertices, _numEdges;  

public:

   /// Constructor, takes the minimum point set from which a valid 2-manifold
   /// can be constructed
   HalfEdgeTopology(PointGeometry p0, PointGeometry p1, PointGeometry p2);
   ~HalfEdgeTopology();

   /// We must ban assignment and copy constructor, unless we want to recursively
   /// implement them (probably not)
   HalfEdgeTopology(const HalfEdgeTopology& h);
   HalfEdgeTopology& operator = (const HalfEdgeTopology& h);

   /// All data is copied and stored internally, no external references are maintained
   HalfEdgeTopology(const PointGeometry * points, int * triangleList, const int N_POINTS, const int N_TRIS);
   
   /// Consider making these operations private..
   FaceIterator RemoveEdge(HalfEdge * f); //< Returns an iterator to the new merged face

   /// Removes a vertex, thereby merging all of the faces which share it. Returns an
   /// iterator to the new merged face
   FaceIterator RemoveVertex(VertexIterator v);

   /// These operations invalidate the iterator
   FaceIterator MergeFaces(FaceIterator faceA, FaceIterator faceB);
   FaceIterator MergeFaces(FaceIterator * faceList, int numFaces);
   VertexIterator SubdivideFaceByVertex(FaceIterator face, PointGeometry p); //  perhaps return a VertexIterator?

   ///////////////
   // Accessors //
   ///////////////
   
   FINLINE int NumFaces()     const  { return _numFaces;    }
   FINLINE int NumVertices()  const  { return _numVertices; }
   FINLINE int NumEdges()     const  { return _numEdges;    }

   FINLINE FaceIterator GetFace() const { return FaceIterator(_headEdge->face); }
   FINLINE VertexIterator GetVertex() const { return VertexIterator(_headEdge->end); }

   FINLINE OneRingIterator GetOneRingIterator(VertexIterator vi) const 
   { return OneRingIterator(vi._element); }


   ///////////
   // Debug //
   ///////////
   
   INLINE void Print() const
   { 
      FaceIterator f = this->GetFace(); int i = 0;
      do 
      {
         ASSERT(f.element()->invariant());

         int v = 0;
         const HalfEdge * e = f.edge();
         ASSERT(e->invariant());
         printf("Face %i (eX%p -> eX%p -> eX%p)\n", i, e, e->next, e->next->next);
         do 
         {
            printf("   Vertex %i (eX%p): %s\n", v, e->end->edge, *e->end->geometry.toString());
            e = e->next; v++;
         } while (e != f.edge());

         printf("   Geometry = %s\n", *f.geometry().toString() );

         f = f.next();
         i++;
      } while (f != this->GetFace());
   }

   INLINE void PrintVertices() const
   {
      VertexIterator v = this->GetVertex(); int i = 0;
      do 
      {
         printf("Vertex %i (%p)\n", i, v.element());
         printf("   edgeFromHere: %p points to: %p\n", v.edge(), v.edge()->end);
         printf("   %s\n", *v.geometry().toString());
         ASSERT(v.element()->invariant());
         ASSERT(v.edge()->invariant());

         v++; i++;
      } while (v != this->GetVertex());

   }



private:
   /// Delinks vertices and faces from their own linked lists and 
   /// de-allocates their memory. It is assumed that all other pointers
   /// to these items have already been cleaned up and there are no remaining
   /// outstanding references to the deleted element.  For the insertion routines,
   /// new elements are allocated and inserted into the linked lists. The edge
   /// members of vertices and faces are initialized to NULL, as the elements are
   /// not integrated into the topology (they will fail the invariant())
   Vertex * insertVertex(PointGeometry p);
   Face *   insertFace  (FaceGeometry  g);
   
   template <class Element> void delinkElement(Element *& e, int& elementCounter);

   bool isEdgeInFace(const Face * f, const HalfEdge * e) const;
   void locateSharedPath(const Face * faceA, const Face * faceB, HalfEdge *& beginningA, HalfEdge *& endA) const;
   bool areFacesAdjacent( const Face * faceA, const Face * faceB) const;

};


/////////////////
// Constructor //
/////////////////

template <class PointGeometry, class FaceGeometry> 
HalfEdgeTopology<PointGeometry, FaceGeometry>::HalfEdgeTopology
   (PointGeometry p0, PointGeometry p1, PointGeometry p2)
{
   /// Three vertices in a ring
   Vertex * v0 = new Vertex(); v0->geometry = p0; v0->edge = NULL;
   Vertex * v1 = new Vertex(); v1->geometry = p1; v1->edge = NULL;
   Vertex * v2 = new Vertex(); v2->geometry = p2; v2->edge = NULL;

   v0->next = v1; v1->next = v2; v2->next = v0;  
   v0->prev = v2; v1->prev = v0; v2->prev = v1;

   /// Create two faces, link them together
   Face * f0 = new Face, * f1 = new Face;
   f0->next = f0->prev = f1; f1->next = f1->prev = f0;

   /// Create six half edges (three edges). Edges 0-2 for face 0, 3-5 for face 1
   /// This is carefully arranged so that the interior triangle (p0, p1, p2)
   /// has clockwise winding, and the exterior triangle has CCW winding
   HalfEdge * e0 = new HalfEdge();  e0->face = f0; e0->end = v1 ;
   HalfEdge * e1 = new HalfEdge();  e1->face = f0; e1->end = v2 ;
   HalfEdge * e2 = new HalfEdge();  e2->face = f0; e2->end = v0 ;
   HalfEdge * e3 = new HalfEdge();  e3->face = f1; e3->end = v0 ;
   HalfEdge * e4 = new HalfEdge();  e4->face = f1; e4->end = v1 ;
   HalfEdge * e5 = new HalfEdge();  e5->face = f1; e5->end = v2 ;

   /// Inner Triangle             Outer Triangle
   e0->next = e1; e0->twin = e3;  e3->next = e5; e3->twin = e0;
   e1->next = e2; e1->twin = e4;  e4->next = e3; e4->twin = e1;
   e2->next = e0; e2->twin = e5;  e5->next = e4; e5->twin = e2;
   
   v0->edge = e0; v1->edge = e1; v2->edge = e2;
   f0->edge = e0; f1->edge = e3;

   _numFaces = 2;
   _numVertices = 3;
   _numEdges = 3;

   /// We choose e3 so that the lead face is the external face
   /// This property is maintained as an invariant
   _headEdge = e3; 

   ASSERT(e0->invariant());  ASSERT(v0->invariant());  ASSERT(f0->invariant());
   ASSERT(e1->invariant());  ASSERT(v1->invariant());  ASSERT(f1->invariant());
   ASSERT(e2->invariant());  ASSERT(v2->invariant()); 
   ASSERT(e3->invariant());  
   ASSERT(e4->invariant());  
   ASSERT(e5->invariant()); 
}


template <class PointGeometry, class FaceGeometry> HalfEdgeTopology<PointGeometry, FaceGeometry>::~HalfEdgeTopology()
{
   /// ok... we must traverse the vertex linked list and the face linked list
   /// and destroy everything, and we must somehow traverse all edges and destroy them

   Face * f = _headEdge->face;
   Vertex * v = _headEdge->end;

   /// Delete all faces 
   Face * fTerminus = f;
   do 
   {      
      /// Destroy the edge loop for this face
      HalfEdge * e = f->edge;
      do 
      {
         HalfEdge * dead = e;
         e = e->next;
         delete dead;

      } while (e != f->edge);

      Face * dead = f;
      f = f->next;
      delete dead;

   } while (f != fTerminus);

   /// At this point _headEdge is invalid, and so is f

   /// Now delete the vertex list
   Vertex * vTerminus = v;
   do 
   {
      Vertex * dead = v;
      v = v->next;
      delete dead;

   } while (v != vTerminus);
}



/////////////////////////
// Private Subroutines //
/////////////////////////

template <class PointGeometry, class FaceGeometry> FINLINE bool 
   HalfEdgeTopology<PointGeometry, FaceGeometry>::HalfEdge::invariant() const
{
   return (bool) end && (bool) face  && (bool) twin  && (bool) next
       && next != this && twin != this && twin != next;
}
template <class PointGeometry, class FaceGeometry> FINLINE bool 
   HalfEdgeTopology<PointGeometry, FaceGeometry>::Vertex::invariant() const
{
   /// There must be at least three vertices
   return (bool) edge && (bool) next  && (bool) prev && next != this         
       && prev != this && next != prev;   /// at least three
}
template <class PointGeometry, class FaceGeometry> FINLINE bool 
   HalfEdgeTopology<PointGeometry, FaceGeometry>::Face::invariant() const
{
   /// There must be at least two faces
   return (bool) edge && (bool) next  && (bool) prev          
       && next != this && prev != this;              
}


template <class PointGeometry, class FaceGeometry> 
template <class Element>
void HalfEdgeTopology<PointGeometry, FaceGeometry>::delinkElement(
   Element *& e, int& elementCounter)
{
   Element * previous = e->prev;   
   Element * next     = e->next;

   previous->next = next;
   next->prev = previous;

   delete e; e = NULL;
   elementCounter--;
}


template <class PointGeometry, class FaceGeometry> 
typename HalfEdgeTopology<PointGeometry, FaceGeometry>::Vertex * 
   HalfEdgeTopology<PointGeometry, FaceGeometry>::insertVertex(PointGeometry p)
{
   Vertex * v = new Vertex();
   v->geometry = p;
   v->edge = NULL;

   /// Prepend it to the linked list, immediately after the head vertex
   Vertex * before = _headEdge->end;
   Vertex * after = before->next;

   before->next = v;    after->prev = v;
   v->next = after;     v->prev = before;

   _numVertices++;
   return v;
}

template <class PointGeometry, class FaceGeometry> 
typename HalfEdgeTopology<PointGeometry, FaceGeometry>::Face *   
   HalfEdgeTopology<PointGeometry, FaceGeometry>::insertFace(FaceGeometry  g)
{
   Face * f = new Face();
   f->geometry = g;
   f->edge = NULL;

   /// Prepend it to the linked list, immediately after the head vertex
   Face * before = _headEdge->face;
   Face * after = before->next;

   before->next = f;   after->prev = f;
   f->next = after;    f->prev = before;

   ASSERT(_headEdge->face == before);

   _numFaces++;
   return f;
}


////////////////////////
// Public Subroutines //
////////////////////////

/// Removal of either half of an edge constitutes removal of the entire edge. The polygons
/// on either side of the edge MUST not share more than one edge, or this operation
/// will fail
template <class PointGeometry, class FaceGeometry> 
typename HalfEdgeTopology<PointGeometry, FaceGeometry>::FaceIterator
   HalfEdgeTopology<PointGeometry, FaceGeometry>::RemoveEdge(
   typename HalfEdgeTopology<PointGeometry, FaceGeometry>::HalfEdge * edge)
{
   HalfEdge * edgeA   = edge;         ASSERT(edgeA  );   ASSERT(edgeA  ->invariant());
   HalfEdge * edgeB   = edge->twin;   ASSERT(edgeB  );   ASSERT(edgeB  ->invariant());
   Face     * faceA   = edgeA->face;  ASSERT(faceA  );   ASSERT(faceA  ->invariant());
   Face     * faceB   = edgeB->face;  ASSERT(faceB  );   ASSERT(faceB  ->invariant());
   Vertex   * vertexA = edgeA->end;   ASSERT(vertexA);   ASSERT(vertexA->invariant());
   Vertex   * vertexB = edgeB->end;   ASSERT(vertexB);   ASSERT(vertexB->invariant());

   HalfEdge * edgeAPrevious = edgeA;
   while (edgeAPrevious->next != edgeA) edgeAPrevious = edgeAPrevious->next;
   HalfEdge * edgeBPrevious = edgeB;
   while (edgeBPrevious->next != edgeB) edgeBPrevious = edgeBPrevious->next;


   /// We'll keep polygon A, and we'll merge B into A. Therefore, traverse
   /// the edges of polygon B, and switch their face pointer to faceA
   HalfEdge * e = edgeB;
   do 
   {
      e->face = faceA;
      e = e->next;
   } while (e != edgeB);

   /// Change the "next" pointers for each edge loop so that we have
   /// one big loop, cutting out the edges being removed
   edgeAPrevious->next = edgeB->next;
   edgeBPrevious->next = edgeA->next;

   /// Next, we must update the vertices of this edge in case either one
   /// points to one of the removed half edges. Note that the edge pointed
   /// to by a vertex is not pointed to by that edge - that is, vertexA
   /// might point to edgeB, while edgeB points to vertexB
   if (vertexB->edge == edgeA) vertexB->edge = edgeB->next;
   if (vertexA->edge == edgeB) vertexA->edge = edgeA->next;

   /// If faceA might points to edgeA, change it
   if (faceA->edge == edgeA) faceA->edge = edgeA->next;

   /// If our head edge pointer is either edgeA or edgeB, change it
   /// Note that if either faceA or faceB were the external face, the
   /// new merged face is now the external face. If head edge was an 
   /// edge on either of these triangles, it will remain on the external 
   /// triangle
   if (_headEdge->face == faceA || _headEdge->face == faceB) 
      _headEdge = edgeA->next;

   /// Now, we remove edgeA, edgeB, and faceB from the system
   ASSERT(_headEdge->face != faceB);
   delinkElement(faceB, _numFaces); 
   ASSERT(!faceB);
   delete edgeA;
   delete edgeB;
   _numEdges--;

   return FaceIterator(faceA);
}


template <class PointGeometry, class FaceGeometry> bool HalfEdgeTopology<PointGeometry, FaceGeometry>::isEdgeInFace
   ( typename const HalfEdgeTopology<PointGeometry, FaceGeometry>::Face * f
   , typename const HalfEdgeTopology<PointGeometry, FaceGeometry>::HalfEdge * e) const
{
   ASSERT(f->invariant()); ASSERT(e->invariant());

   HalfEdge * edge = f->edge;
   do 
   {
      if (edge == e) return true;
      edge = edge->next;
   } while (edge != f->edge);
   return false;
}


/// Identifies the shared sequences of edges between two faces, if any. Pointers to
/// the first and last half edges in faceA that are shared with faceB are written to
/// the two pointers passed in. NULL is returned in both if there are no shared edges,
/// and if there is only one shared edge, they will point to that same edge. 
template <class PointGeometry, class FaceGeometry> void HalfEdgeTopology<PointGeometry, FaceGeometry>::locateSharedPath
   ( typename const HalfEdgeTopology<PointGeometry, FaceGeometry>::Face * faceA
   , typename const HalfEdgeTopology<PointGeometry, FaceGeometry>::Face * faceB
   , typename HalfEdgeTopology<PointGeometry, FaceGeometry>::HalfEdge *& beginningA
   , typename HalfEdgeTopology<PointGeometry, FaceGeometry>::HalfEdge *& endA) const
{
   ASSERT(faceA->invariant()); ASSERT(faceB->invariant());

   beginningA = endA = NULL;

   /// Locate an edge in faceA that is NOT shared with faceB. There is guaranteed
   /// to be at least one.
   HalfEdge * n0 = faceA->edge;
   while (isEdgeInFace(faceB, n0->twin)) n0 = n0->next;

   /// Now, locate the first edge from here that IS shared with faceB, store it in
   beginningA = n0->next;
   while (!isEdgeInFace(faceB, beginningA->twin) && beginningA != n0) 
      beginningA = beginningA->next; 

   /// If none were found, there are no shared edges, we exit early do nothing
   if (n0 == beginningA) { beginningA = endA = NULL; return; }

   /// otherwise, we search for endA
   endA = beginningA;
   while (isEdgeInFace(faceB, endA->next->twin)) endA = endA->next;
}

template <class PointGeometry, class FaceGeometry> bool 
   HalfEdgeTopology<PointGeometry, FaceGeometry>::areFacesAdjacent
   ( typename const HalfEdgeTopology<PointGeometry, FaceGeometry>::Face * faceA
   , typename const HalfEdgeTopology<PointGeometry, FaceGeometry>::Face * faceB) const
{
   ASSERT(faceA->invariant()); ASSERT(faceB->invariant());

   HalfEdge * a = faceA->edge;
   do { if (isEdgeInFace(faceB, a->twin)) return true; else a = a->next; } 
   while (a != faceA->edge);
   return false;
}



template <class PointGeometry, class FaceGeometry> 
typename HalfEdgeTopology<PointGeometry, FaceGeometry>::FaceIterator 
   HalfEdgeTopology<PointGeometry, FaceGeometry>::MergeFaces
   ( typename HalfEdgeTopology<PointGeometry, FaceGeometry>::FaceIterator * facesItr
   , int numFaces)
{
   ASSERT(numFaces >= 2);
   /// As faces are merged, we replace their pointer in the input array with NULL

   FaceIterator m = facesItr[0];
   facesItr[0] = FaceIterator(NULL);

   /// Search for a triangle that borders f0, merge it, then repeat
   for (int i = 1; i < numFaces; ++i)
   {
      if (!facesItr[i].isNull() && areFacesAdjacent(m._element, facesItr[i]._element))
      {
         m = MergeFaces(m, facesItr[i]);
         facesItr[i] = FaceIterator(NULL);
         i = 0;   /// Restart the loop
      }
   }

   /// OK, let's check to see if we got them all...
   int n = 0;
   for (int i = 0; i < numFaces; ++i)
      if (!facesItr[i].isNull()) n++;

   if (n > 0) 
      printf("Warning: failed to merge %i out of %i faces...\n", n, numFaces);

   return m;
}

template <class PointGeometry, class FaceGeometry> 
typename HalfEdgeTopology<PointGeometry, FaceGeometry>::FaceIterator 
   HalfEdgeTopology<PointGeometry, FaceGeometry>::MergeFaces
   ( typename HalfEdgeTopology<PointGeometry, FaceGeometry>::FaceIterator faceAItr
   , typename HalfEdgeTopology<PointGeometry, FaceGeometry>::FaceIterator faceBItr)
{
   if (_numFaces <= 2) return FaceIterator(NULL);

   Face * faceA = faceAItr._element, * faceB = faceBItr._element;
   HalfEdge * beginningA, * endA;
   locateSharedPath(faceA, faceB, beginningA, endA);

   /// If the faces share no edges, we exit and do nothing
   if (!beginningA && !endA) return FaceIterator(NULL);

   ASSERT(beginningA && endA);
   ASSERT(beginningA->invariant()); ASSERT(endA->invariant());

   /// If the faces share one edge, we special case it to edge removal
   if (beginningA == endA) { return RemoveEdge(beginningA); }


   /// Else, it gets more complex
   
   /// 0. Locate some local elements of interest

   HalfEdge * beginningB = endA->twin;    ASSERT(beginningB->invariant());
   HalfEdge * endB = beginningA->twin;    ASSERT(endB->invariant());

   HalfEdge * previousA = beginningA;
   while (previousA->next != beginningA) previousA = previousA->next;
   HalfEdge * previousB = beginningB;
   while (previousB->next != beginningB) previousB = previousB->next;
   
   Vertex * vertexA = previousA->end;    ASSERT(vertexA->invariant());
   Vertex * vertexB = previousB->end;    ASSERT(vertexB->invariant());
   

   /// 1. Remove interior vertices to the path
   HalfEdge * e = beginningA;
   while (e != endA) { delinkElement(e->end, _numVertices); e = e->next; }

   /// 2. Stitch the edge seam at the beginning and end ("cut" out the path)
   previousA->next = endB->next;
   previousB->next = endA->next;

   /// 3. Fix vertices at beginning and end, in case they point to an edge
   /// that is on the shared path
   if (vertexA->edge == beginningA) vertexA->edge = endB->next;
   if (vertexB->edge == beginningB) vertexB->edge = endA->next;

   /// 4. Walk the edges of B, set the face pointers to face A (A eats B)
   e = endB->next;
   while (e->face != faceA) { e->face = faceA; e = e->next; }

   /// if the head edge was any edge on either of the two merged triangles,
   /// then we simply move it
   if (_headEdge->face == faceA || _headEdge->face == faceB)
      _headEdge = previousA->next;

   /// change faceA's edge to an edge guaranteed to exist
   faceA->edge = endA->next;

   /// 5. Destroy faceB, and the edges on the shared path
   ASSERT(_headEdge->face != faceB);
   delinkElement(faceB, _numFaces); ASSERT(!faceB);
   e = beginningA;
   HalfEdge * terminus = endA->next;
   do
   {
      HalfEdge * last = e;
      e = e->next;
      delete last->twin;
      delete last;

      _numEdges--;
   } while (e != terminus);

   return FaceIterator(faceA);
}


template <class PointGeometry, class FaceGeometry> 
   typename HalfEdgeTopology<PointGeometry, FaceGeometry>::VertexIterator
   HalfEdgeTopology<PointGeometry, FaceGeometry>::SubdivideFaceByVertex
   ( typename HalfEdgeTopology<PointGeometry, FaceGeometry>::FaceIterator f
   , PointGeometry p)
{
   Face * oldFace = f._element;
   ASSERT(oldFace->invariant());

   Vertex   * newVertex     = insertVertex(p);
   HalfEdge * terminus      = oldFace->edge->next;
   HalfEdge * cEdge         = oldFace->edge->next;
   Vertex   * corner0       = oldFace->edge->end;
   Vertex   * corner1       = oldFace->edge->next->end;
   HalfEdge * priorFaceLeg0 = NULL;

   int iterations = 0;

   do
   {
      ASSERT(cEdge->end == corner1);

      HalfEdge * nextEdge = cEdge->next;

      /// We will iterate around the perimeter of the face, creating a new
      /// face for each leg, and creating the half edges in each one
      Face * newFace = insertFace(FaceGeometry());
      HalfEdge * leg0 = new HalfEdge();
      HalfEdge * leg1 = new HalfEdge();

      /// Build the edge loop
      newFace->edge = cEdge;
      cEdge->next = leg0;     cEdge->face = newFace;
      leg0 ->next = leg1;     leg0 ->face = newFace;
      leg1 ->next = cEdge;    leg1 ->face = newFace;

      /// Set the new half edges' vertex end points
      leg0->end = newVertex;
      leg1->end = corner0;

      if (priorFaceLeg0) { leg1->twin = priorFaceLeg0; priorFaceLeg0->twin = leg1; }

      /// this happens only on the first iteration
      if (!priorFaceLeg0) newVertex->edge = leg1;

      /// Advance pointers for next iteration
      cEdge = nextEdge;
      priorFaceLeg0 = leg0;
      corner0 = corner1;
      corner1 = nextEdge->end;

      /// This happens only on the LAST iteration
      if (cEdge == terminus)
      {
         leg0->twin = newVertex->edge;
         newVertex->edge->twin = leg0;
      }

      iterations++;

   } while (cEdge != terminus);

   /// We remove the old face that was just subdivided
   delinkElement(oldFace, _numFaces);

   _numEdges += iterations;

   ASSERT(newVertex->invariant());
   return newVertex;
}

template <class PointGeometry, class FaceGeometry> 
   typename HalfEdgeTopology<PointGeometry, FaceGeometry>::FaceIterator 
   HalfEdgeTopology<PointGeometry, FaceGeometry>::RemoveVertex(
   typename HalfEdgeTopology<PointGeometry, FaceGeometry>::VertexIterator v)
{
   /// To remove a vertex, we simply merge all of the faces which
   /// share this vertex into a single face. 

   ASSERT(_numFaces > 3); 

   Collection<FaceIterator> facesToMerge(10);
   OneRingIterator oneItr = GetOneRingIterator(v);
   OneRingIterator terminus = oneItr;
   int i = 0;
   do 
   {
      facesToMerge.add(FaceIterator(oneItr.face()));
      oneItr = oneItr.next();
   } while (oneItr != terminus);

   ASSERT(facesToMerge.size() >= 2);

   return MergeFaces(&facesToMerge[0], facesToMerge.size());
}








//////////////////
// Garbage Dump //
//////////////////


#if 0


template <class PointGeometry, class FaceGeometry> void 
   HalfEdgeTopology<PointGeometry, FaceGeometry>::delinkFace(
   typename HalfEdgeTopology<PointGeometry, FaceGeometry>::Face *& f)
{
   /// Since the list is circular, no pointers are NULL
   Face * previous = f->prev;    ASSERT(previous);
   Face * next = f->next;        ASSERT(next);

   previous->next = next;
   next->prev = previous;

   delete f;
   f = NULL;

   _numFaces--;
}

template <class PointGeometry, class FaceGeometry> void 
   HalfEdgeTopology<PointGeometry, FaceGeometry>::delinkVertex(
   typename HalfEdgeTopology<PointGeometry, FaceGeometry>::Vertex *& v)
{
   /// Since the list is circular, no pointers are NULL
   Vertex * previous = v->prev;    ASSERT(previous);
   Vertex * next = v->next;        ASSERT(next);

   previous->next = next;
   next->prev = previous;

   delete v;
   v = NULL;

   _numVertices--;
}


#endif


#endif
