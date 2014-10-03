#ifndef COLLECTIONS_REFERENCE_H
#define COLLECTIONS_REFERENCE_H

#include <new>

namespace Collections
{
   template<typename T> static inline void AddRef (T* a) { if (a) a->AddRef (); }
   template<typename T> static inline void Release(T* a) { if (a) a->Release(); }

   template<typename T>
   class Ref
   {
   private:
      T* p;

   public:
      inline Ref() : p(NULL) {}
      inline Ref(const Ref& a) : p(a.p) { Collections::AddRef(p); }
      inline Ref(T* a) : p(a) { Collections::AddRef(p); }
      inline ~Ref() { Collections::Release(p); }

      inline Ref& operator =(T* a)
      {
         if (p != a)
         {
            Collections::AddRef (a);
            Collections::Release(p);
            p = a;
         }
         return *this;
      }
      inline Ref& operator =(const Ref& a) { return *this = a.p; }

      inline operator T* const&() const { return p;  }
      inline operator T*      &()       { return p;  }
      inline T& operator  *    () const { return *p; }
      inline T* operator ->    () const { return  p; }
   };

   class Object
   {
   private:
      mutable int _refCtr;

   public:
      inline Object() : _refCtr(0) {}
      virtual ~Object() {}
      inline void AddRef () const { ++_refCtr; }
      inline void Release() const { if (--_refCtr <= 0) delete this; }
   };
}

#endif // REFERENCE_H

