/**
  @file ReferenceCount.h

  Reference Counting Garbage Collector for C++

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @cite Adapted and extended from Justin Miller's "RGC" class that appeared in BYTE magazine.
  @cite See also http://www.jelovic.com/articles/cpp_without_memory_errors_slides.htm

  @created 2001-10-23
  @edited  2004-09-14

Example:

  <PRE>

class Foo : public G3D::ReferenceCountedObject {
public:
    int x;
    ~Foo() {
        printf("Deallocating 0x%x\n", this);
    }
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

#include "G3D/debug.h"

namespace G3D {


/**
 Objects that are reference counted inherit from this.  Subclasses 
 <B>must</B> have a public destructor (the default destructor is fine)
 and publicly inherit ReferenceCountedObject.
 */
class ReferenceCountedObject {
public:

    /**
     The long name is to keep this from accidentally conflicting with
     a subclass's variable name.  Do not explicitly manipulate this value.
     */
    int ReferenceCountedObject_refCount;

protected:

    ReferenceCountedObject() : ReferenceCountedObject_refCount(0) {
        debugAssertM(isValidHeapPointer(this), 
            "Reference counted objects must be allocated on the heap.");
    }

public:

    virtual ~ReferenceCountedObject() {}

    /**
      Note: copies will initially start out with 0 
      references like any other object.
     */
    ReferenceCountedObject(const ReferenceCountedObject& notUsed) : 
        ReferenceCountedObject_refCount(0) {
        (void)notUsed;
        debugAssertM(isValidHeapPointer(this), 
            "Reference counted objects must be allocated on the heap.");
    }
};


/**
 Use ReferenceCountedPointer<T> in place of T* in your program.
 T must subclass ReferenceCountedObject.
 */
template <class T>
class ReferenceCountedPointer {
private:

    T*           pointer;

public:

    inline T* getPointer() const {
        return pointer;
    }

private:

    void registerReference() { 
        pointer->ReferenceCountedObject_refCount += 1;
        //debugPrintf("  ++0x%x\n", pointer);
        //debugPrintf("  [0x%x] = %d\n", pointer, pointer->ReferenceCountedObject_refCount);
    }


    int deregisterReference() {
        if (pointer->ReferenceCountedObject_refCount > 0) {
            pointer->ReferenceCountedObject_refCount -= 1;
            //debugPrintf("  --0x%x\n", pointer);
            //debugPrintf("  [0x%x] = %d\n", pointer, pointer->ReferenceCountedObject_refCount);
        }

        return pointer->ReferenceCountedObject_refCount;
    }


    void zeroPointer() {
        if (pointer != NULL) {

            debugAssert(isValidHeapPointer(pointer));

            if (deregisterReference() <= 0) {
                // We held the last reference, so delete the object
                //debugPrintf("  delete 0x%x\n", pointer);
                delete pointer;
            }

            pointer = NULL;
        }
    }


    void setPointer(T* x) {
        if (x != pointer) {
            zeroPointer();

            if (x != NULL) {
                debugAssert(isValidHeapPointer(x));

		        pointer = x;
		        registerReference();
            }
        }
    }

public:      

    inline ReferenceCountedPointer() : pointer(NULL) {}

    /**
      Allow compile time subtyping rule 
      RCP&lt;<I>T</I>&gt; &lt;: RCP&lt;<I>S</I>&gt; if <I>T</I> &lt;: <I>S</I>
     */
    template <class S>
    inline ReferenceCountedPointer(const ReferenceCountedPointer<S>& p) : pointer(NULL) {
        setPointer(p.getPointer());
    }

    // We need an explicit version of the copy constructor as well or 
    // the default copy constructor will be used.
    inline ReferenceCountedPointer(const ReferenceCountedPointer<T>& p) : pointer(NULL) {
        setPointer(p.pointer);
    }

    /** Allows construction from a raw pointer.  That object will thereafter be
        reference counted -- do not call delete on it. */
    inline ReferenceCountedPointer(T* p) : pointer(NULL) { 
        setPointer(p); 
    }
    
    inline ~ReferenceCountedPointer() {
        zeroPointer();
    }
  

    inline const ReferenceCountedPointer<T>& operator=(const ReferenceCountedPointer<T>& p) {
        setPointer(p.pointer);
        return *this;
    }   


    inline ReferenceCountedPointer<T>& operator=(T* p) {
        setPointer(p);
        return *this;
    }


    inline bool operator==(const ReferenceCountedPointer<T>& y) const { 
        return (pointer == y.pointer); 
    }


    inline bool operator!=(const ReferenceCountedPointer<T>& y) const { 
        return (pointer != y.pointer); 
    }


    inline T& operator*() const {
        return (*pointer);
    }


    inline T* operator->() const {
        return pointer;
    }


    inline bool isNull() const {
        return (pointer == NULL);
    }

    inline bool notNull() const {
        return (pointer != NULL);
    }

    /**
     Returns true if this is the last reference to an object.
     Useful for flushing memoization caches-- a cache that holds the last
     reference is unnecessarily keeping an object alive.
     */
    inline int isLastReference() const {
        return (pointer->ReferenceCountedObject_refCount == 1);
    }
};

} // namespace

#endif





