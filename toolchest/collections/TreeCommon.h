#ifndef TREE_COMMON_H
#define TREE_COMMON_H

#include "../adts/Vector.h"

namespace Collections
{
   namespace Immutable
   {
      template <class K, class V> class TreeMap;
      template <class E> class TreeSet;
      template <class E> class Array;
      template <class E> class LinkedList;
   }

   namespace Mutable
   {
      template <class K, class V> class TreeMap;
      template <class E> class TreeSet;
      template <class E> class Array;
      template <class E> class LinkedList;
   }

	namespace Common
	{
      using ToolChest::Ref;

      /////////////////////////////////////
      // Structures For Tree Collections //
      /////////////////////////////////////

      /// Parameterize the comparators so that Map ADTs based on the tree
      /// data structure can order based on the key portion of the payload, 
      /// rather than the entire payload

      template <class T> inline bool
      LessThan(const T& lhs, const T& rhs) { return lhs < rhs; }

      template <class T> inline bool
      Equals(const T& lhs, const T& rhs) { return lhs == rhs; }
      

      template <class E> class BinaryTreeNode
      {
      public:
         int left, right, priority;
         E payload;
      };

      template <class E> class BinaryTree
      {
      private:
         void Remove(int& n);

      public:
         int _root;
         Ref<MemoryPool<BinaryTreeNode<E> > > _pool;

         inline BinaryTree(int allocation = 1)
            : _root(-1), _pool(new MemoryPool<BinaryTreeNode<E> >(allocation)) {}
         inline BinaryTree(int r, Ref<MemoryPool<BinaryTreeNode<E> > > pool) :
            _root(r), _pool(pool) {}

         /// Finds the element e in the binary tree, records the index of the 
         /// parent node and the target node in the out parameters. If target
         /// is either not found (-1) or is the root node, then parent := -1
         void Find(const E& e, int& parent, int& target) const;
         
         /// This is set insertion, returns false if 'e' was pre-existant, 
         /// automatically avoids inserting duplicates, returns true if a 
         /// new node was added successfully.
         ///
         /// The ordering property of the binary search tree is preserved as
         /// long as this is performed on the root node of the tree
         ///
         bool Insert(const E& e);

         /// Removes the node 'n' from the tree, preserving the ordering property
         void Remove(int n, int parent);

         /// Tree Rotations, used to maintain balance
         void RotateLeft(int a, int b, int aParent);
         void RotateRight(int a, int b, int bParent);

         void Print(const char * fileName) const;
         void PrintArray(int n) const;

         inline BinaryTree Clone() const
         {
            BinaryTree b;
            b._root = _root;
            b._pool = _pool->Clone();
            return b;
         }
      };


      //////////////////////////
      // Binary Tree Iterator //
      //////////////////////////

      template <class E, class C> class BinaryTreeIterator
      {
      private:

         friend class BinaryTree<E>;

         friend class Immutable::TreeSet<E>;
         friend class Mutable::TreeSet<E>;
         friend class Immutable::TreeMap<typename C::KeyType, typename C::ValueType>;
         friend class Mutable::TreeMap<typename C::KeyType, typename C::ValueType>;

         Ref<MemoryPool<BinaryTreeNode<E> > > _pool;
         Vector<int> _stack;
         int _nextNode;

         inline BinaryTreeIterator(const BinaryTree<E>& a) 
            : _nextNode(a._root) 
            , _pool(a._pool)
         {
            /// The initial node should be the left-most node
            /// and the stack should contain the path to get there
            if (_nextNode != -1)
            {
               while (_pool->Index(_nextNode).left != -1)
               {
                  _stack.Push(_nextNode);
                  _nextNode = _pool->Index(_nextNode).left;
               }
            }            
         }

      public:

         inline BinaryTreeIterator(const BinaryTreeIterator<E, C>& itr)
            : _pool(itr._pool)
            , _stack(itr._stack.Copy())  //< Copy to make sure it's not aliased
            , _nextNode(itr._nextNode)
         {}

         inline bool HasNext() const { return _nextNode != -1; }

         /// Amortized O(1) i believe
         inline E& Next()
         {
            assert(HasNext());

            E& e = _pool->Index(_nextNode).payload;

            BinaryTreeNode<E>& n = _pool->Index(_nextNode);

            /// We are looking for the next-left-most node. The next node is not
            /// in our left subtree, but is the left-most node in the right
            /// subtree, if we have a right subtree. If we do not have a right
            /// subtree, then we pop.
            if (n.right >= 0)
            {
               _nextNode = n.right;
               while (_pool->Index(_nextNode).left != -1)
               {
                  _stack.Push(_nextNode);
                  _nextNode = _pool->Index(_nextNode).left;
               }
            }
            else if (/* n.right == -1 && */_stack.Size() > 0) _nextNode = _stack.Pop();
            else /* n.right == -1 && _stack.Empty() */ _nextNode = -1;

            return e;
         }

         inline const E& Peek() const
         {
            assert(HasNext());
            return _pool->Index(_nextNode).payload;
         }

         inline E& Peek() 
         {
            assert(HasNext());
            return _pool->Index(_nextNode).payload;
         }
      };


      /////////////////////////
      // Binary Tree Builder //
      /////////////////////////

      template <class E, class C> class BinaryTreeBuilder
      {
      private:

         friend class BinaryTree<E>;
      
         friend class Immutable::TreeSet<E>;
         friend class Mutable::TreeSet<E>;
         friend class Immutable::TreeMap<typename C::KeyType, typename C::ValueType>;
         friend class Mutable::TreeMap<typename C::KeyType, typename C::ValueType>;

         /// Flag goes true when the result has been returned and the 
         /// builder can no longer be used (it is disposable)
         bool _complete;
         int _size;

         BinaryTree<E> _tree; //< The tree under construction

         inline BinaryTreeBuilder(const BinaryTree<E>& tree, int size)
            : _tree(tree), _size(size), _complete(false) {}
         
      public:

         inline BinaryTreeBuilder(int allocation = 1) 
            : _complete(false), _size(0), _tree(allocation) {}
      
         //////////////////////////////////////////////
         // Copy and Assignment, Reference Semantics //
         //////////////////////////////////////////////

         inline BinaryTreeBuilder(const BinaryTreeBuilder& rhs)
            : _complete(rhs._complete)
            , _size(rhs._size)
            , _tree(rhs._tree) { }

         /// Should we allow this??  This strikes me as highly questionable
         inline BinaryTreeBuilder& operator = (const BinaryTreeBuilder& rhs)
         {  
            _complete = rhs._complete;  
            _tree = rhs._tree;  
            _size = rhs._size;       
            return *this; 
         }

         /// This is set insertion, returns false if 'e' was preexistant, 
         /// automatically avoids inserting duplicates
         /// We should move the implementation of this routine to the node
         /// object, so that it can be used internally by the tree collections
         /// without a builder (general in-place insertion).
         inline bool AddElement(E e) 
         {
            assert(!_complete);  
            if (_tree.Insert(e)) { _size++; return true; }
            return false;       
         }

         inline C Result() { _complete = true; return C(_size, _tree); }
      };



      /// Tree Rotations, used to maintain balance
      template <class E> void BinaryTree<E>::RotateLeft(int a, int b, int aParent)
      {
         ///         a                b    
         ///        / \              / \   
         ///       x   b     -->    a   z                
         ///          / \          / \       
         ///         y   z        x   y 

         assert(a != -1); assert(b != -1);  
         MemoryPool<BinaryTreeNode<E> >& pool = *_pool;

         // re-root the subtree
         if (aParent != -1)
         {
            if (pool[aParent].left == a) pool[aParent].left = b;
            else { assert(pool[aParent].right == a); pool[aParent].right = b; }
         }

         assert(pool[a].right == b);

         int x = pool[a].left;
         int y = pool[b].left;
         int z = pool[b].right;

         pool[b].left  = a; pool[a].right = y;

         if (_root == a) _root = b;
      }

      template <class E> void BinaryTree<E>::RotateRight(int b, int a, int bParent)
      {
         ///         a                b    
         ///        / \              / \   
         ///       x   b     <--    a   z                
         ///          / \          / \       
         ///         y   z        x   y 

         assert(a != -1); assert(b != -1); 
         MemoryPool<BinaryTreeNode<E> >& pool = *_pool;

         // re-root the subtree
         if (bParent != -1)
         {
            if (pool[bParent].left == b) pool[bParent].left = a;
            else { assert(pool[bParent].right == b); pool[bParent].right = a; }
         }

         assert(pool[b].left == a);

         int x = pool[a].left;
         int y = pool[a].right;
         int z = pool[b].right;

         pool[b].left  = y; pool[a].right = b;

         if (_root == b) _root = a;
      }


      ////////////
      // Remove //
      ////////////

      template <class E> void BinaryTree<E>::Remove(int n, int parent)
      {
         MemoryPool<BinaryTreeNode<E> >& pool = *_pool;

         while (pool[n].left != -1 || pool[n].right != -1)
         {
            /// Walk down the tree, rotating n towards a leaf position, while
            /// maintaining the heap priority ordering property

            if (pool[n].left == -1)
            {
               /// thre's a right child, but no left child, just snip it out
               if (parent == -1) { assert(_root == n); _root = pool[n].right; }
               else if (pool[parent].left == n) pool[parent].left = pool[n].right;
               else pool[parent].right = pool[n].right;
               return;
            }
            else if (pool[n].right == -1)
            {
               /// there's only a left child
               if (parent == -1) { assert(_root == n); _root = pool[n].left; }
               else if (pool[parent].left == n) pool[parent].left = pool[n].left;
               else pool[parent].right = pool[n].left;
               return;
            }
            else
            {
               /// We have two children
               assert(pool[n].left != -1 && pool[n].right != -1);

               /// We rotate downward, preserving the priority ordering
               if (pool[pool[n].left].priority < pool[pool[n].right].priority)
               {
                  int newParent = pool[n].right;
                  RotateLeft( n, pool[n].right, parent );
                  parent = newParent;
               }
               else
               {
                  int newParent = pool[n].left;
                  RotateRight( n, pool[n].left, parent );
                  parent = newParent;
               }
            }
         }

         assert(pool[n].left == -1 && pool[n].right == -1);
         assert(parent != -1 ? pool[parent].left == n || pool[parent].right == n : true);

         if      (parent == -1)           _root              = -1;
         else if (pool[parent].left == n) pool[parent].left  = -1;
         else                             pool[parent].right = -1;
      }



      ////////////
      // Insert //
      ////////////

      /// This function is not thread-safe
      /// should return the int where it was inserted ?
      template <class E> bool BinaryTree<E>::Insert(const E& e)
      {
         MemoryPool<BinaryTreeNode<E> >& pool = *_pool;
         static Vector<int> trail = Vector<int>::Construct(0x40);
         trail.Clear();

         /// we're going to walk down the tree until we find the location where
         /// this element belongs, and we need to maintain a stack
         /// containing the path down the tree so that we can maintain
         /// the priority ordering. We would use Find() if we did not need
         /// to retain the entire path to the resulting node

         trail.Push(-1);
         int cNode = _root; 
         while (cNode != -1)
         {
            if (pool[cNode].payload < e)
            { trail.Push(cNode); cNode = pool[cNode].right; }
            else if (e < pool[cNode].payload)
            { trail.Push(cNode); cNode = pool[cNode].left; }
            else /* e == pool[cNode].payload */ break; 
         }
         
         int pNode = trail.Last();

         if (cNode == -1) // we should insert here
         {

            BinaryTreeNode<E> newNode;
            newNode.payload  = e;
            newNode.left     = newNode.right = -1;
            newNode.priority = rand();

            if (pNode == -1)
            {
               /// we insert at the root
               assert(_root == -1);
               _root = pool.Push(newNode);
            }
            else
            {
               int n = pool.Push(newNode);

               /// we have a leaf node that is a parent-to-be
               if (pool.Index(pNode).payload < e)
               { assert(pool[pNode].right == -1); pool[pNode].right = n; }
               else if (e < pool[pNode].payload)
               { assert(pool[pNode].left == -1); pool[pNode].left = n; }

               /// This is the case where we may have to maintain the
               /// auto-balance property of the binary tree

               int p = trail.Pop();    /// remove from the trail
               while (p > -1 && pool[p].priority < pool[n].priority)
               {
                  int grandparent = trail.Pop();

                  if (pool[p].right == n) RotateLeft (p, n, grandparent);
                  else                    RotateRight(p, n, grandparent);

                  p = grandparent;
               }
            }
            
            return true;
         }
         else return false;
      }


      //////////
      // Find //
      //////////

      template <class E> void BinaryTree<E>::Find(const E& e, int& parent, int& target) const
      {
         parent = -1;
         target = _root;

         const MemoryPool<BinaryTreeNode<E> >& pool = *_pool;

         while (target != -1)
         {
            if (pool[target].payload < e)
            { parent = target; target = pool[target].right; }
            else if (e < pool[target].payload)
            { parent = target; target = pool[target].left; }
            else /* e == pool[target].payload */ return; 
         }

         /// if we get here, target is -1, parent is where the
         /// node with value 'e' should be inserted
         assert(target == -1);
      }


      //////////////
      // Printing //
      //////////////

      /// Prints the tree in array representation, first n nodes
      template <class E> void BinaryTree<E>::PrintArray(int n) const
      {
         using namespace std;
         typedef BinaryTreeNode<E> Node;
         MemoryPool<Node>& pool = *_pool;

         cout << "_root = " << _root << endl;
         for (int i = 0; i < n; ++i)
            cout << "pool[" << i << "] = (" 
                 << pool[i].left << ", "
                 << pool[i].right << ", "
                 << pool[i].priority << ", "
                 << pool[i].payload << ")" << endl;
      }

      /// This method outputs a graphviz tree representationt into the 
      /// indicated file
      template <class E> void BinaryTree<E>::Print(const char * fileName) const
      {
         using namespace std;
         typedef BinaryTreeIterator<E, Immutable::TreeSet<E> > Iterator;
         typedef BinaryTreeNode<E> Node;

         std::ofstream f(fileName);
         f << "digraph Tree {" << endl;
         f << "   node [shape = oval]" << endl;

         {
            Iterator itr(*this);
            while (itr.HasNext()) 
            { 
               const E& e = itr.Next();
               f << "   Node_" << e << " [ label = \"" << e << "\" ]" << std::endl;
            }
         }

         /// Now we must walk through the nodes explicitly, rather than through the
         /// abstraction of the iterator interface, in order to expose the topology
         Iterator iterator(*this);
         MemoryPool<Node>& pool = *_pool;
         while (iterator.HasNext()) 
         {

            const E& e = pool[iterator._nextNode].payload;
            auto left = pool[iterator._nextNode].left;
            auto right = pool[iterator._nextNode].right;
            if (left  >= 0) f << "   Node_" << e << " -> Node_" << pool[left].payload << endl;
            if (right >= 0) f << "   Node_" << e << " -> Node_" << pool[right].payload << endl;
            iterator.Next();
         }

         f << "}" << endl;
         f.close();
      }
   } // namespace Common
} // namespace Collections

#endif