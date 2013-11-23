#ifndef MUTABLE_TREE_SET_H
#define MUTABLE_TREE_SET_H

#include "Set.h"
#include "TreeCommon.h"

/////////////////////////////////////////
// Class TreeSet <- Set <- Traversable //
/////////////////////////////////////////

namespace Collections
{
   namespace Mutable
   {
      template <class E> class TreeSet;

      template <class E> struct TreeSetTraits
      {
         typedef Common::MutableBinaryTreeIterator<E, TreeSet<E> > Iterator;
         typedef Common::BinaryTreeBuilder<E, TreeSet<E> > Builder;
      };


      template <class E> 
      class TreeSet : public Set<E, TreeSet<E>, TreeSetTraits<E> >
      {
      public:
      
         friend class Common::MutableBinaryTreeIterator<E, TreeSet<E> >;
         friend class Common::BinaryTreeBuilder<E, TreeSet<E> >;
      
         typedef E ElementType;
         typedef Common::BinaryTree<E> Tree;
         typedef Common::MutableBinaryTreeIterator<E, TreeSet<E> > Iterator;
         typedef Common::BinaryTreeNode<E> Node;
         typedef Common::BinaryTreeBuilder<E, TreeSet<E> > Builder;

         template <class U> struct SwapElementType { typedef TreeSet<U> C; };

      private:
      public:

         int _size;  //< Number of elements, for efficiency
         Tree _tree;
      
         inline TreeSet(int size, const Tree& tree)
            : _size(size)
            , _tree(tree) {}
      
      
      public:
      
         ///////////////////////////////////////////
         // Copy Constructor, Reference Semantics //
         ///////////////////////////////////////////
      
         /// The data array's reference counter is automatically incremented
         inline TreeSet() : _size(0), _tree(0) {}
      
         /// The data array's reference counter is automatically incremented
         inline TreeSet(const TreeSet& rhs)
            : _size(rhs._size)
            , _tree(rhs._tree) { }

         //////////////////////////////////////////////
         // Assignment Operator, Reference Semantics //
         //////////////////////////////////////////////
      
         inline TreeSet& operator = (const TreeSet& rhs)
         { _tree = rhs._tree; _size = rhs._size; return *this; }


         ////////////////////////////////
         // Inherited From Traversable //
         ////////////////////////////////
      
         virtual int Size() const { return _size; }
         virtual Iterator GetIterator() const { return Iterator(this->_tree); }


         ////////////////////////
         // Inherited From Set //
         ////////////////////////

         virtual TreeSet<E> Insert(const E& element) const;
         virtual TreeSet<E> Remove(const E& element) const;

         ///////////////////
         // Miscellaneous //
         ///////////////////

         void PrintGraph(const char * fileName) const;


         //////////////////////////
         // Mutable TreeSet Only //
         //////////////////////////
   
         TreeSet<E>& operator += (const E& e);
         TreeSet<E>& operator += (const TreeSet<E>& set);   // destructive union
         TreeSet<E>& operator -= (const E& e);
         TreeSet<E>& operator -= (const TreeSet<E>& set);   // destructive set difference
      };


      template <class E> TreeSet<E>& TreeSet<E>::operator += (const E& e)
      {
         if (_tree.Insert(e)) _size++;
         return *this;
      }

      template <class E> TreeSet<E>& TreeSet<E>::operator += (const TreeSet<E>& set)
      {
         /// We need to handle the strange case where the rhs is the same container
         /// as on the lhs
         if (&set == this) return *this;
         Iterator itr = set.GetIterator();
         while (itr.HasNext()) *this += itr.Next();
         return *this;
      }

      template <class E> TreeSet<E>& TreeSet<E>::operator -= (const E& e)
      {
         int parent, target;
         _tree.Find(e, parent, target);
         if (target == -1) return *this;
         _tree.Remove(target, parent); _size--;
         return *this;
      }

      template <class E> TreeSet<E>& TreeSet<E>::operator -= (const TreeSet<E>& set)
      {
         /// We need to handle the strange case where the rhs is the same container
         /// as on the lhs, in which case we get an empty set
         if (&set == this) 
         {
            _tree._root = -1;
            _size = 0;
            return *this;
         }
         Iterator itr = set.GetIterator();
         while (itr.HasNext()) *this -= itr.Next();
         return *this;
      }



      /// Note: Insert and Remove are here and not in Set because of the
      /// "return *this" statements, which do not work in the abstract
      /// base class for some reason that I do not quite understand.

      /// O(n)
      template <class E> TreeSet<E>
      TreeSet<E>::Insert(const E& e) const
      {
         if (this->Contains(e)) return *this;
         auto pTree = _tree.Clone();
         pTree.Insert(e);
         return TreeSet(_size+1, pTree);
      }

      /// O(n log n)
      template <class E> TreeSet<E>
      TreeSet<E>::Remove(const E& e) const
      {
         // Ok, this is simple. First locate the item to remove
         int parent, target;
         _tree.Find(e, parent, target);

         /// if it was not present, then we're done!
         if (target == -1) return *this;

         /// else, clone the tree and remove the node
         auto pTree = _tree.Clone();
         pTree.Remove(target, parent);
         return TreeSet(_size-1, pTree);
      } 
      
      template <class E> void TreeSet<E>::PrintGraph(const char * fileName) const
      { _tree.Print(fileName); }

   } // namespace Immutable
} // namespace Collections

#endif // TREE_SET_H
