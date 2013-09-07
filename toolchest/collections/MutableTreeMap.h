#ifndef MUTABLE_TREE_MAP_H
#define MUTABLE_TREE_MAP_H

#include "Map.h"
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
         typedef Common::BinaryTreeIterator<Common::KeyValuePair<K, V>, TreeMap<K, V> > Iterator;
         typedef Common::BinaryTreeBuilder<Common::KeyValuePair<K, V>, TreeMap<K, V> > Builder;
         typedef Mutable::TreeSet<K> SetType;
      };

      template <class K, class V> 
      class TreeMap : public Map<K, V, TreeMap<K, V>, TreeMapTraits<K, V> >
      {
      public:
      
         friend class Common::BinaryTreeIterator<Common::KeyValuePair<K, V>, TreeMap<K, V> >;
         friend class Common::BinaryTreeBuilder<Common::KeyValuePair<K, V>, TreeMap<K, V> >;
      
         template <class U> struct SwapElementType { typedef TreeMap<K, U> C; };

         typedef Common::KeyValuePair<K, V> ElementType;
         typedef Common::BinaryTree<Common::KeyValuePair<K, V> > Tree;
         typedef Common::BinaryTreeIterator<Common::KeyValuePair<K, V>, TreeMap<K, V> > Iterator;
         typedef Common::BinaryTreeNode<Common::KeyValuePair<K, V> > Node;
         typedef Common::BinaryTreeBuilder<Common::KeyValuePair<K, V>, TreeMap<K, V> > Builder;

         /// The set container that corresponds to the TreeMap<K, V> is TreeSet<K>
         typedef TreeSet<K> SetType;

      private:

         int _size;  //< Number of elements, for efficiency
         Tree _tree;
      
         inline TreeMap(int size, const Tree& tree)
            : _size(size)
            , _tree(tree) {}
      
      public:

         ///////////////////////////////////////////
         // Copy Constructor, Reference Semantics //
         ///////////////////////////////////////////
      
         /// The data array's reference counter is automatically incremented
         inline TreeMap() : _size(0), _tree(0) {}
      
         /// The data array's reference counter is automatically incremented
         inline TreeMap(const TreeMap& rhs)
            : _size(rhs._size)
            , _tree(rhs._tree) { }

         //////////////////////////////////////////////
         // Assignment Operator, Reference Semantics //
         //////////////////////////////////////////////
      
         inline TreeMap& operator = (const TreeMap& rhs)
         { _tree = rhs._tree; _size = rhs._size; return *this; }


         ////////////////////////////////
         // Inherited From Traversable //
         ////////////////////////////////
      
         virtual int Size() const { return _size; }
         virtual Iterator GetIterator() const { return Iterator(this->_tree); }


         ////////////////////////
         // Inherited From Map //
         ////////////////////////

         virtual TreeMap<K, V> Insert(const K& key, const V& value) const;
         virtual TreeMap<K, V> Remove(const K& key) const;
         virtual SetType Keys() const;


         ///////////////////
         // Miscellaneous //
         ///////////////////

         void PrintGraph(const char * fileName) const { _tree.Print(fileName); }   

         //////////////////////////
         // Mutable TreeMap Only //
         //////////////////////////
   
         TreeMap<K, V>& operator += (const Common::KeyValuePair<K, V>& keyValuePair);
         TreeMap<K, V>& operator += (const TreeMap<K, V>& map);   // destructive union
         TreeMap<K, V>& operator -= (const K& key);
         TreeMap<K, V>& operator -= (const TreeMap<K, V>& map);    
      };


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

         auto pTree = _tree.Clone();
         pTree.Insert(Common::KeyValuePair<K, V>(key, value));
         return TreeMap(_size+1, pTree);
      }

      /// O(n)
      template <class K, class V> TreeMap<K, V>
      TreeMap<K, V>::Remove(const K& key) const
      {
         // Ok, this is simple. First locate the item to remove
         int parent, target;
         _tree.Find(Common::KeyValuePair<K, V>(key), parent, target);

         /// if it was not present, then we're done!
         if (target == -1) return *this;

         /// else, clone the tree and remove the node
         auto pTree = _tree.Clone();
         pTree.Remove(target, parent);
         return TreeMap(_size-1, pTree);
      }   

      template <class K, class V> TreeMap<K, V>& 
      TreeMap<K, V>::operator += (const Common::KeyValuePair<K, V>& keyValuePair)
      {
         if (_tree.Insert(keyValuePair)) _size++;
         return *this;
      }

      template <class K, class V> TreeMap<K, V>& 
      TreeMap<K, V>::operator += (const TreeMap<K, V>& map)
      {
         if (&map != this)
         {
            Iterator itr = map.GetIterator();
            while (itr.HasNext()) *this += itr.Next();
         }
         return *this;  
      }

      template <class K, class V> TreeMap<K, V>& 
      TreeMap<K, V>::operator -= (const K& key)
      {
         // Ok, this is simple. First locate the item to remove
         int parent, target; _tree.Find(key, parent, target);

         if (target == -1) return *this; /// if not present, we're done!
         _tree.Remove(target, parent); _size--;
         return *this;
      }

      template <class K, class V> TreeMap<K, V>& 
      TreeMap<K, V>::operator -= (const TreeMap<K, V>& map)
      {
         if (&map != this)
         {
            Iterator itr = map.GetIterator();
            while (itr.HasNext()) *this -= itr.Next();
         }
         return *this; 
      }

   } // namespace Mutable
} // namespace Collections

#endif // MUTABLE_TREE_MAP_H
