/**
  @file ReferenceCount.h

  Reference Counting Garbage Collector for C++

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @cite Converted from Justin Miller's "RGC" class, as appeared in BYTE magazine.

  @created 2001-10-23
  @edited  2003-01-03

Example:

  <PRE>

class Foo : public G3D::ReferenceCountedObject {
public:
    int x;
};

typedef G3D::ReferenceCountedPointer<Foo> FooRef;

int main(int argc, char *argv[]) {

    FooRef a = new Foo();

    {
        FooRef b = a;
    }
    return 0
}
</PRE>
*/

#ifndef G3D_RGC_H
#define G3D_RGC_H

#include "../G3D/debug.h"

namespace G3D {

/**
 Objects that are reference counted inherit from this.
 */
class ReferenceCountedObject {
private:
    int refCount;
  
public:
  
    void RegisterReference() { 
        ++refCount;
    }

    int DeregisterReference() {
        if (refCount > 0) {
            --refCount;
        }

        return refCount;
    }

    ReferenceCountedObject() : refCount(0) {}
    virtual ~ReferenceCountedObject() {}

    /**
      Note: copies will initially start out with 0 
      references like any other object.
     */
    ReferenceCountedObject(const ReferenceCountedObject &notUsed) : refCount(0) {}   

};


/**
 Use ReferenceCountedPointer<T> in place of T* in your program.
 */
template <class T>
class ReferenceCountedPointer {
private:
    T* pointer;

public:
    inline T* getPointer() const {
        return pointer;
    }

private:

    void zeroPointer() {
        if (pointer != NULL) {

            debugAssert(isValidHeapPointer(pointer));

            ReferenceCountedObject* p = (ReferenceCountedObject*)pointer;

            if (p->DeregisterReference() <= 0) {
                delete p;
            }

            pointer = NULL;
        }
    }

    void setPointer(T* x) {
        if (pointer != NULL) {
            zeroPointer();
        }

        if (x != NULL) {
            debugAssert(isValidHeapPointer(x));

            ReferenceCountedObject* p = (ReferenceCountedObject*)x;
		    p->RegisterReference();
		    pointer = x;
        }
    }

public:      

    template <class S>
    inline ReferenceCountedPointer(const ReferenceCountedPointer<S>& p) : pointer(NULL) {
        setPointer((T*)p.getPointer());
    }

    inline ReferenceCountedPointer() : pointer(NULL) {}

    inline ReferenceCountedPointer(T* p) : pointer(NULL) { 
        setPointer(p); 
    }
    
    inline ReferenceCountedPointer(const ReferenceCountedPointer<T>& r) : pointer(NULL) { 
        setPointer(r.getPointer());
    }

    inline ~ReferenceCountedPointer() {
        zeroPointer();
    }
  
    inline ReferenceCountedPointer<T>& operator= (const ReferenceCountedPointer<T>& p) { 
        setPointer(p.getPointer());
        return *this; 
    }

    inline ReferenceCountedPointer<T>& operator= (T* x) {
        setPointer(x);
        return *this;
    }

    inline int operator== (const ReferenceCountedPointer<T> &y) const { 
        return (pointer == y.pointer); 
    }

    inline T& operator* () const {
        return (*pointer);
    }

    inline T* operator-> () const {
        return pointer;
    }

    inline operator T*() const {
        return pointer;
    }

};

} // namespace

#endif





