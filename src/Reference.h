
#pragma once

#ifndef REFERENCE_H
#define REFERENCE_H

#include <new>

namespace Burns
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
      inline Ref(const Ref& a) : p(a.p) { AddRef(p); }
      inline Ref(T* a) : p(a) { AddRef(p); }
      inline ~Ref() { Release(p); }

      inline Ref& operator =(T* a)
      {
         if (p != a)
         {
            AddRef (a);
            Release(p);
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
      int _refCtr;

   public:
      inline Object() : _refCtr(0) {}
      virtual ~Object() {}
      inline void AddRef () { ++_refCtr; }
      inline void Release() { if (--_refCtr <= 0) delete this; }
   };
}

#endif // REFERENCE_H

