#pragma once

#ifndef MUTABLE_TREE_MAP_H
#define MUTABLE_TREE_MAP_H

#include "Map.h"
#include "Vector.h"
#include "TreeCommon.h"


/////////////////////////////////////////
// Class TreeMap <- Map <- Traversable //
/////////////////////////////////////////

namespace Collections
{
   namespace Mutable
   {
      template <class K, class V> class TreeMap;

      template <class K, class V> struct TreeMapTraits
      {
         typedef Common::TreeIterator<Common::KeyValuePair<K, V>, TreeMap<K, V> > Iterator;
         typedef Common::TreeBuilder<Common::KeyValuePair<K, V>, TreeMap<K, V> > Builder;
         typedef Mutable::TreeSet<K> SetType;
      };


      template <class K, class V> 
      class TreeMap : public Map<K, V, TreeMap<K, V>, TreeMapTraits<K, V> >
      {
      public:
      
         friend class Common::TreeIterator<Common::KeyValuePair<K, V>, TreeMap<K, V> >;
         friend class Common::TreeBuilder<Common::KeyValuePair<K, V>, TreeMap<K, V> >;
      
         template <class U> struct SwapElementType { typedef TreeMap<K, U> C; };

         typedef Common::KeyValuePair<K, V> ElementType;
         typedef Common::TreeIterator<Common::KeyValuePair<K, V>, TreeMap<K, V> > Iterator;
         typedef Common::TreeNode<Common::KeyValuePair<K, V> > Node;
         typedef Common::TreeBuilder<Common::KeyValuePair<K, V>, TreeMap<K, V> > Builder;

         /// The set container that corresponds to the TreeMap<K, V> is TreeSet<K>
         typedef Mutable::TreeSet<K> SetType;

      private:

         int _size;  //< Number of elements, for efficiency
         Ref<Node> _root;
      
         FINLINE TreeMap(int size, Ref<Node> root)
            : _size(size)
            , _root(root) {}
      
      
      public:
      
         ///////////////////////////////////////////
         // Copy Constructor, Reference Semantics //
         ///////////////////////////////////////////
      
         /// The data array's reference counter is automatically incremented
         FINLINE TreeMap() : _size(0), _root(Ref<Node>()) {}
      
         /// The data array's reference counter is automatically incremented
         FINLINE TreeMap(const TreeMap& rhs)
            : _size(rhs._size)
            , _root(rhs._root) { }


         //////////////////////////////////////////////
         // Assignment Operator, Reference Semantics //
         //////////////////////////////////////////////
      
         FINLINE TreeMap& operator = (const TreeMap& rhs)
         { 
            Builder b = rhs.clone(rhs.Size(), rhs.Size());
            TreeMap rhsClone = b.Result();
            _size = rhsClone._size;
            _root = rhsClone._root;
            return *this;
         }


         ////////////////////////////////
         // Inherited From Traversable //
         ////////////////////////////////
      
         virtual int Size() const { return _size; }
         virtual Iterator GetIterator() const { return Iterator(this->_root); }


         ////////////////////////
         // Inherited From Map //
         ////////////////////////

         virtual TreeMap<K, V> Insert(const K& key, const V& value) const;
         virtual TreeMap<K, V> Remove(const K& key) const;
         virtual typename TreeMap<K, V>::SetType Keys() const;

         ///////////////////
         // Miscellaneous //
         ///////////////////

         void PrintGraph(const char * fileName) const;   

         //////////////////////////
         // Mutable TreeMap Only //
         //////////////////////////
   
         TreeMap<K, V>& operator += (const Common::KeyValuePair<K, V>& keyValuePair);
         TreeMap<K, V>& operator += (const TreeMap<K, V>& map);   // destructive union
         TreeMap<K, V>& operator -= (const K& key);
         TreeMap<K, V>& operator -= (const TreeMap<K, V>& map);      


      protected:
         virtual Builder clone(int numElementsToCopy, int reservedSpace) const
         {
            /// Iterating and using the Builder's AddElement method would
            /// produce an n-log-n clone operation.  We should be able to
            /// clone a tree in O(n) time.

            if (Size() == 0) return Builder();

            /// We can use the CloneSubTree method in the common case
            const int N = MIN(numElementsToCopy, Size());
            if (N == Size()) return Builder(Node::CloneSubTree(_root), N);
            else return Traversable<Common::KeyValuePair<K, V>, TreeMap<K, V>, TreeMapTraits<K, V> >::clone(numElementsToCopy, reservedSpace);
         }                  
      };


      /////////////////////////////////
      // Immutable TreeMap Functions //
      /////////////////////////////////

      template <class K, class V> typename TreeMap<K, V>::SetType TreeMap<K, V>::Keys() const
      {
         typedef typename SetType::Builder SetBuilder;
         SetBuilder builder = SetBuilder(_size);
         Iterator iterator = this->GetIterator();
         while (iterator.HasNext())
            builder.AddElement(iterator.Next().key);
         return builder.Result();
      }

      /// Note: Insert and Remove are here and not in Map because of the
      /// "return *this" statements, which do not work in the abstract
      /// base class for some reason that I do not quite understand.

      /// O(n)
      template <class K, class V> TreeMap<K, V>
      TreeMap<K, V>::Insert(const K& key, const V& value) const
      {
         /// Can't use this early out because we may be updating the value
         //if (this->Contains(key)) return *this;

         Builder builder = this->clone(this->Size(), this->Size() + 1);
         builder.AddElement(Common::KeyValuePair<K, V>(key, value));
         return builder.Result();
      }

      /// O(n)
      template <class K, class V> TreeMap<K, V>
      TreeMap<K, V>::Remove(const K& key) const
      {
         TreeMap<K, V> map = TreeMap<K, V>(this->Size(), Node::CloneSubTree(_root));
         if (map._root->payload.key == key) Node::RemoveNodeFromSubtree(map._root, &map._root);         
         else
         {
            Ref<Node > n = Node::FindParentInSubtree(Common::KeyValuePair<K, V>(key), map._root); 
            assert(!n.IsNull());
            if (n->right && n->right->payload.key == key)
               Node::RemoveNodeFromSubtree(n->right, &n->right);
            else if (n->left && n->left->payload.key == key)
               Node::RemoveNodeFromSubtree(n->left, &n->left);
         }
         map._size--;
         return map;
      }   

      template <class K, class V> void TreeMap<K, V>::PrintGraph(const char * fileName) const
      { Node::PrintSubTree(_root, fileName); }


      //////////////////////////////////
      // Mutable Function Definitions //
      //////////////////////////////////

      template <class K, class V> TreeMap<K, V>& TreeMap<K, V>::operator += (const Common::KeyValuePair<K, V>& keyValuePair)
      {
         if (_root.IsNull())  
         {
            assert(_size == 0);
            _root = Ref<Node >::ObjectRef();
            _root->payload = keyValuePair; 
            _size++;
         }
         else if (Node::AddElementToSubtree(keyValuePair, _root)) _size++;
         return *this;
      }

      template <class K, class V> TreeMap<K, V>& TreeMap<K, V>::operator += (const TreeMap<K, V>& map)
      {
         assert(map._root != _root);

         Iterator itr = map.GetIterator();

         if (itr.HasNext() && _size == 0)
         {
            assert(!_root);
            _root = Ref<Node >::ObjectRef();
            _root->payload = itr.Next(); _size++;
         }

         while (itr.HasNext())
         { if (Node::AddElementToSubtree(itr.Next(), _root)) _size++; }
         return *this;
      }

      template <class K, class V> TreeMap<K, V>& TreeMap<K, V>::operator -= (const K& key)
      {
         if (_size == 0) return *this;
         
         if (_root->payload.key == key) Node::RemoveNodeFromSubtree(_root, &_root);         
         else
         {
            Ref<Node > n = Node::FindParentInSubtree(Common::KeyValuePair<K, V>(key), _root); 
            assert(!n.IsNull());
            if (n->right && n->right->payload.key == key)
               Node::RemoveNodeFromSubtree(n->right, &n->right);
            else if (n->left && n->left->payload.key == key)
               Node::RemoveNodeFromSubtree(n->left, &n->left);
         }
         _size--; return *this;         
      }

      /// This appears to be broken if (map == *this) ...
      template <class K, class V> TreeMap<K, V>& TreeMap<K, V>::operator -= (const TreeMap<K, V>& map)
      {
         assert(map._root != _root);
         
         if (_size == 0) return *this;

         Iterator itr = map.GetIterator();
         while (itr.HasNext()) (*this) -= itr.Next().key;
         return *this;
      }
   } // namespace Mutable   
} // namespace Collections

#endif // TREE_SET_H