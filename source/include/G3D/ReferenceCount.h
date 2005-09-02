/**
  @file ReferenceCount.h

  Reference Counting Garbage Collector for C++

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @cite Adapted and extended from Justin Miller's "RGC" class that appeared in BYTE magazine.
  @cite See also http://www.jelovic.com/articles/cpp_without_memory_errors_slides.htm

  @created 2001-10-23
  @edited  2005-09-01

*/

#ifndef G3D_RGC_H
#define G3D_RGC_H

#include "G3D/debug.h"

namespace G3D {

/** Base class for WeakReferenceCountedPointer */
class _WeakPtr {
public:
    inline virtual ~_WeakPtr() {}

protected:
    friend class ReferenceCountedObject;

    /** Called by ReferenceCountedObject to tell a weak pointer that its underlying object was collected. */
    virtual void objectCollected() = 0;
};

/** Used internally by ReferenceCountedObject */
class _WeakPtrLinkedList {
public:
    _WeakPtr*                   weakPtr;
    _WeakPtrLinkedList*         next;

    inline _WeakPtrLinkedList() : weakPtr(NULL), next(NULL) {}

    /** Inserts this node into the head of the list that previously had n as its head. */
    inline _WeakPtrLinkedList(_WeakPtr* p, _WeakPtrLinkedList* n) : weakPtr(p), next(n) {}
};

/**
 Objects that are reference counted inherit from this.  Subclasses 
 <B>must</B> have a public destructor (the default destructor is fine)
 and <B>publicly</B> inherit ReferenceCountedObject.

 Multiple inheritance from a reference counted object is dangerous-- use 
 at your own risk.


<B>Usage Example</B>

  <PRE>

class Foo : public G3D::ReferenceCountedObject {
public:
    int x;
};

class Bar : public Foo {};

typedef G3D::ReferenceCountedPointer<Foo> FooRef;
typedef G3D::WeakReferenceCountedPointer<Foo> WeakFooRef;
typedef G3D::ReferenceCountedPointer<Bar> BarRef;


int main(int argc, char *argv[]) {

    WeakFooRef x;

    {
        FooRef a = new Foo();

        // Reference count == 1

        x = a;
        // Weak references do not increase count

        {
            FooRef b = a;
            // Reference count == 2
        }

        // Reference count == 1
    }
    // No more strong references; object automatically deleted.
    // x is set to NULL automatically.

    // Example of using dynamic cast on reference counted objects
    BarRef b = new Bar();

    // No cast needed to go down the heirarchy.
    FooRef f = b;

    // We can't cast the reference object because it is a class.
    // Instead we must extract the pointer and cast that:
    b = dynamic_cast<Bar*>(&*f);

    return 0;
}
</PRE>
 */
class ReferenceCountedObject {
public:

    /**
     The long name is to keep this from accidentally conflicting with
     a subclass's variable name.  Do not explicitly manipulate this value.
     */
    int                         ReferenceCountedObject_refCount;
 
    /**
     Linked list of all weak pointers that reference this (some may be on the stack!). 
     Do not explicitly manipulate this value.
     */
    _WeakPtrLinkedList*         ReferenceCountedObject_weakPointer;

protected:

    ReferenceCountedObject() : ReferenceCountedObject_refCount(0), ReferenceCountedObject_weakPointer(0) {
        debugAssertM(isValidHeapPointer(this), 
            "Reference counted objects must be allocated on the heap.");
    }

public:

    virtual ~ReferenceCountedObject() {
        // Tell all of my weak pointers that I'm gone
        
        _WeakPtrLinkedList* node = ReferenceCountedObject_weakPointer;

        while (node != 0) {

            // Notify
            node->weakPtr->objectCollected();

            // Free the node and advance
            _WeakPtrLinkedList* tmp = node;
            node = node->next;
            delete tmp;
        }
    }

    /**
      Note: copies will initially start out with 0 
      references and 0 weak references like any other object.
     */
    ReferenceCountedObject(const ReferenceCountedObject& notUsed) : 
        ReferenceCountedObject_refCount(0),
        ReferenceCountedObject_weakPointer(0) {
        (void)notUsed;
        debugAssertM(G3D::isValidHeapPointer(this), 
            "Reference counted objects must be allocated on the heap.");
    }

    ReferenceCountedObject& operator=(const ReferenceCountedObject& other) {
        // Nothing changes when I am assigned; the reference count is the same
        // (although my super-class probably changes).
        return *this;
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

            debugAssert(G3D::isValidHeapPointer(pointer));

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
                debugAssert(G3D::isValidHeapPointer(x));

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

    // TODO: distinguish between last strong and last any pointer
    /**
     Returns true if this is the last reference to an object.
     Useful for flushing memoization caches-- a cache that holds the last
     reference is unnecessarily keeping an object alive.
     @deprecated Use WeakReferenceCountedPointer for caches
     */
    inline int isLastReference() const {
        return (pointer->ReferenceCountedObject_refCount == 1);
    }
};


/**
 A weak pointer allows the object it references to be garbage collected.
 Weak pointers are commonly used in caches, where it is important to hold
 a pointer to an object without keeping that object alive solely for the
 cache's benefit (i.e., the object can be collected as soon as all
 pointers to it <B>outside</B> the cache are gone).

 Weak pointers may become NULL at any point (when their target is collected).
 Therefore the only way to reference the target is to convert to a strong
 pointer and then check that it is not NULL.
 */
template <class T>
class WeakReferenceCountedPointer : public _WeakPtr {
private:

    T*          pointer;

    void setPointer(T* p) {
        zeroPointer();
        pointer = p;

        if (pointer != 0) {
            // Add myself to the head of my target's list of weak pointers
            _WeakPtrLinkedList* head = 
                new _WeakPtrLinkedList(this, pointer->ReferenceCountedObject_weakPointer);
            pointer->ReferenceCountedObject_weakPointer = head;
        }
    }

    /** Removes this from its target's list of weak pointers */
    void zeroPointer() {
        if (pointer != 0) {
            debugAssertM(pointer->ReferenceCountedObject_weakPointer != NULL,
                "Weak pointer exists without a backpointer from the object.");
            
            // Remove myself from my target's list of weak pointers
            _WeakPtrLinkedList** node = &pointer->ReferenceCountedObject_weakPointer;
            while ((*node)->weakPtr != this) {
                node = &((*node)->next);
                debugAssertM(*node != NULL, 
                    "Weak pointer exists without a backpointer from the object (2).");
            }

            // Node must now point at the node for me.  Remove node and
            // close the linked list behind it.
            _WeakPtrLinkedList* temp = *node;
            *node = temp->next;
            
            // Now delete the node corresponding to me
            delete temp;
        }
    }

public:

    WeakReferenceCountedPointer() : pointer(0) {}

    /**
      Allow compile time subtyping rule 
      RCP&lt;<I>T</I>&gt; &lt;: RCP&lt;<I>S</I>&gt; if <I>T</I> &lt;: <I>S</I>
     */
    template <class S>
    inline WeakReferenceCountedPointer(const WeakReferenceCountedPointer<S>& p) : pointer(0) {
        setPointer(p.pointer);
    }

    template <class S>
    inline WeakReferenceCountedPointer(const ReferenceCountedPointer<S>& p) : pointer(0) {
        setPointer(p.getPointer());
    }

    // Gets called a *lot* when weak pointers are on the stack
    WeakReferenceCountedPointer(
        const WeakReferenceCountedPointer<T>& weakPtr) : pointer(0) {
        setPointer(weakPtr.pointer);
    }

    WeakReferenceCountedPointer(
        const ReferenceCountedPointer<T>& strongPtr) : pointer(0) {
        setPointer(strongPtr.getPointer());
    }

    ~WeakReferenceCountedPointer() {
        zeroPointer();
    }

    WeakReferenceCountedPointer<T>& operator=(const WeakReferenceCountedPointer<T>& other) {
        // I now point at other's target
        setPointer(other.pointer);

        return *this;
    }

    WeakReferenceCountedPointer<T>& operator=(const ReferenceCountedPointer<T>& other) {
        // I now point at other's target
        setPointer(other.getPointer());

        return *this;
    }

    bool operator==(const WeakReferenceCountedPointer<T>& other) const {
        return pointer == other.pointer;
    }

    bool operator!=(const WeakReferenceCountedPointer<T>& other) const {
        return pointer != other.pointer;
    }

protected:

    virtual void objectCollected() {
        debugAssert(pointer != NULL);
        pointer = NULL;
    }

public:
    /**
      Creates a strong pointer, which prevents the object from being garbage collected.
      The strong pointer may be NULL, which means that the underlying.
      */
    //  There is intentionally no way to check if the WeakReferenceCountedPointer has a 
    //  null reference without creating a strong pointer since there is no safe way to
    //  use that information-- the pointer could be collected by a subsequent statement.
    ReferenceCountedPointer<T> createStrongPtr() const {
        return ReferenceCountedPointer<T>(pointer);
    };

};

} // namespace

#endif

