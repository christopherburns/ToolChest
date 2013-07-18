
#pragma once

#ifndef ADJACENCY_GRAPH_H
#define ADJACENCY_GRAPH_H

#include <Burns.h>

using namespace Burns;

/// T is some kind of Traversable container that is not a set
template <class T> T RemoveDuplicates(const T& t)
{
   /// Build a set, which naturally filters duplicates
   Collections::Mutable::TreeSet<typename T::ElementType> set;
   auto itr = t.GetIterator();
   while (itr.HasNext()) set += itr.Next();

   /// Copy the set back to the original container type
   typename T::Builder tBuilder;
   auto setItr = set.GetIterator();
   while (setItr.HasNext()) tBuilder.AddElement(setItr.Next());
   return tBuilder.Result();
}


template <class T> class VertexAdjacencyGraph
{
public:
   class Node
   {
   public:
      T nodeData;
      Collections::Mutable::LinkedList<Node*> adjacency;
   };

private:
   Collections::Mutable::Array<Node> _nodes;

public:

   /// Default constructor
   inline VertexAdjacencyGraph() {}

   /// Constructor takes a list of items located int the 2D plane, and a list of
   /// triangle indicies to specify connectivity. From this, the adjacency list
   /// is constructed.  
   inline VertexAdjacencyGraph
      ( const T * itemList
      , const int N_VERTS
      , const int32 * triIndices
      , const int N_TRIS) : _nodes(N_VERTS)
   {
      for (int i = 0; i < N_TRIS; ++i)
      {
         int32 i0 = triIndices[i*3+0], i1 = triIndices[i*3+1], i2 = triIndices[i*3+2];

         _nodes[i0].adjacency += &_nodes[i1]; _nodes[i0].adjacency += &_nodes[i2];
         _nodes[i1].adjacency += &_nodes[i0]; _nodes[i1].adjacency += &_nodes[i2];
         _nodes[i2].adjacency += &_nodes[i0]; _nodes[i2].adjacency += &_nodes[i1];
      }

      for (int n = 0; n < N_VERTS; ++n) 
      {
         _nodes[n].nodeData = itemList[n];
         _nodes[n].adjacency = RemoveDuplicates(_nodes[n].adjacency);
      }
   }


   ///////////////
   // Interface //
   ///////////////

   inline int Size() const { return _nodes.size(); }
   inline const Node& operator [] (int i) const { return _nodes[i]; }





   /// Removes a vertex from the graph
   /// This requires that we re-implement using a LinkedList for the
   /// _nodes array, rather than the current Buffer, otherwise removal
   /// of a point would immediately invalidate all nodes' adjacency
   /// pointers
#if 0
   INLINE void RemovePoint(int p)
   {
      ASSERT(p < _nodes.size());
      for (auto i = _nodes[p].adjacency.first(); !i.isNull(); i++)
      {
         /// Find reference to myself in my neighbor's list
         for (auto j = (*i)->adjacency.first(); !j.isNull(); j++)
         {
            if ((*j) == &_nodes[p])
            { (*i)->adjacency.remove(j); break; }
         }
      }

      /// Remove this node from the list of _nodes
      /// ...
   }
#endif

   inline void InsertPoint(T p)
   {

   }
};






template <class T> class MeshAdjacencyGraph
{
public:
   class Node
   {
   public:
      T nodeData;
      Collections::Mutable::LinkedList<Node*> adjacency;
      Collections::Vector<int32> adjTriangleIndices;
   };

private:
   Collections::Mutable::Array<Node> _nodes;

public:
   
   /// Default constructor
   inline MeshAdjacencyGraph() {}

   /// Constructor takes a list of items located int the 2D plane, and a list of
   /// triangle indicies to specify connectivity. From this, the adjacency list
   /// is constructed.  
   inline MeshAdjacencyGraph
      ( const T* itemList
      , const uint8 * valences
      , const int N_VERTS
      , const int32 * triIndices
      , const int N_TRIS) 
      : _nodes(N_VERTS)
   {

      /// Visit each triangle, add two adjacency indices for each vertex pair
      /// in the triangle
      for (int n = 0; n < N_VERTS; ++n)
         _nodes[n].adjTriangleIndices = Collections::Vector<int32>::Construct(valences[n]);

      for (int i = 0; i < N_TRIS; ++i)
      {
         int32 i0 = triIndices[i*3+0];
         int32 i1 = triIndices[i*3+1];
         int32 i2 = triIndices[i*3+2];
         _nodes[i0].adjacency += &_nodes[i1];
         _nodes[i0].adjacency += &_nodes[i2];
         _nodes[i1].adjacency += &_nodes[i0];
         _nodes[i1].adjacency += &_nodes[i2];
         _nodes[i2].adjacency += &_nodes[i0];
         _nodes[i2].adjacency += &_nodes[i1];

         _nodes[i0].adjTriangleIndices.Push(i);
         _nodes[i1].adjTriangleIndices.Push(i);
         _nodes[i2].adjTriangleIndices.Push(i);
      }

      for (int n = 0; n < N_VERTS; ++n) 
      {
         _nodes[n].nodeData = itemList[n];
         _nodes[n].adjacency = RemoveDuplicates(_nodes[n].adjacency);
         //_nodes[n].adjacency.removeDuplicates();   /// This may be slow...
      }
   }

   ///////////////
   // Accessors //
   ///////////////

   inline int size() const { return _nodes.size(); }
   inline const Node& operator [] (int i) const { return _nodes[i]; }
};

#endif // ADJACNECY_GRAPH_H

