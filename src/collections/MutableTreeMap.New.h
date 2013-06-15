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
         typedef Common::BinaryTreeIterator<Common::KeyValuePair<K, V>, TreeMap<K, V> > Iterator;
         typedef Common::BinaryTreeBuilder<Common::KeyValuePair<K, V>, TreeMap<K, V> > Builder;
         typedef Mutable::TreeSet<K> SetType;
      };


  } // namespace Mutable

} // namespace Collections