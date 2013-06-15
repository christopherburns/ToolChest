#pragma once

#ifndef HASH_MAP_H
#define HASH_MAP_H


namespace Collections
{
	namespace Mutable
	{
		template <class K, class V> class HashMap;

		template <class K, class V> struct HashMapTraits
		{
			template <class U> struct SwapElementType { typedef HashMap<K, U> C; };

			class Iterator
			{
				
			};

			class Builder
			{

			};
		};


		template <class K, class V> 
		class HashMap : public Map<K, V, HashMap<K, V>, HashMapTraits<K, V> >
		{
		private:

			int _size;

		public:

			friend class HashMapTraits<K, V>::Iterator;
			friend class HashMapTraits<K, V>::Builder;

			typedef typename HashMapTraits<K, V>::Iterator Iterator;
			typedef typename HashMapTraits<K, V>::Builder Builder;


         ///////////////////////////////////////////
         // Copy Constructor, Reference Semantics //
         ///////////////////////////////////////////
      
         /// The data array's reference counter is automatically incremented
         FINLINE HashMap() : _size(0)  {}
      
         /// The data array's reference counter is automatically incremented
         FINLINE HashMap(const HashMap& rhs)
            : _size(rhs._size) { }


         //////////////////////////////////////////////
         // Assignment Operator, Reference Semantics //
         //////////////////////////////////////////////
      
         FINLINE HashMap& operator = (const HashMap& rhs)
         { _size = rhs._size; return *this; }


         ////////////////////////////////
         // Inherited From Traversable //
         ////////////////////////////////
      
         virtual int Size() const { return _size; }
         //virtual Iterator GetIterator() const { return Iterator(this->_root); }


         ////////////////////////
         // Inherited From Map //
         ////////////////////////

         virtual HashMap<K, V> Insert(const K& key, const V& value) const;
         virtual HashMap<K, V> Remove(const K& key) const;
         virtual typename HashMapTraits<K, V>::SetType Keys() const;
		};


	}
}







