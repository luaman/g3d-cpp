/**
  @file AABSPTree.h
  
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @created 2004-01-11
  @edited  2004-03-13

  Copyright 2000-2004, Morgan McGuire.
  All rights reserved.

Attached is one nasty case.  The red line is split plane.  The green box is hit before the yellow box (which is a Box, the AABox around it shown in dashed lines), but the yellow box is be returned first by the iterator.  The correct result in this case is:

Iterator returns Yellow.

Caller tests for intersection but rejects it because the true intersection is past maxDist.

Iterator returns Green.

Caller tests for intersection and remembers that as the best intersection.

Iterator returns Yellow.

Caller tests for intersection-- it passes the minDist and maxDist tests but fails to be closer than the previous intersection.

  <IMG SRc="aabsp-intersect.png">
  */

#ifndef G3D_AABSPTREE_H
#define G3D_AABSPTREE_H

#include "G3D/Array.h"
#include "G3D/Table.h"
#include "G3D/Vector3.h"
#include "G3D/AABox.h"
#include "G3D/Sphere.h"
#include "G3D/Box.h"
#include "G3D/Triangle.h"
#include "G3D/Ray.h"
#include <algorithm>

inline void getBounds(const G3D::Vector3& v, G3D::AABox& out) {
    out = G3D::AABox(v);
}


inline void getBounds(const G3D::AABox& a, G3D::AABox& out) {
    out = a;
}


inline void getBounds(const G3D::Sphere& s, G3D::AABox& out) {
    s.getBounds(out);
}


inline void getBounds(const G3D::Box& b, G3D::AABox& out) {
    b.getBounds(out);
}


inline void getBounds(const G3D::Triangle& t, G3D::AABox& out) {
    t.getBounds(out);
}

namespace G3D {

/**
 A G3D::Set that supports spatial queries.  Internally, objects
 are arranged into an axis-aligned BSP-tree according to their 
 axis-aligned bounds.  This increases the cost of insertion to
 O(log n) but allows fast overlap queries.

 <B>Moving Set Members</B>
 It is important that objects do not move without updating the
 AABSPTree.  If the axis-aligned bounds of an object are about
 to change, AABSPTree::remove it before they change and 
 AABSPTree::insert it again afterward.  For objects
 where the hashCode and == operator are invariant of position,
 you can use the AABSPTree::update method as a shortcut to
 insert/remove an object in one step after it has moved.
 
 <B>Template Parameters</B>
 <DT>The template parameter <I>T</I> must be one for which
 the following functions are overloaded:

  <BLOCKQUOTE>
  <P><CODE>void ::getBounds(const T&, G3D::AABox&);</CODE>
  <DT><CODE>bool operator==(const T&, const T&);</CODE>
  <DT><CODE>unsigned int ::hashCode(const T&);</CODE>
  <DT><CODE>T::T();</CODE> <I>(public constructor of no arguments)</I>
  </BLOCKQUOTE>

  When using large objects, consider making the template parameter
  a <I>pointer</I> to the object type because the T-values are
  copied many times during tree balancing.

 <B>Dimensions</B>
 Although designed as a 3D-data structure, you can use the AABSPTree
 for data distributed along 2 or 1 axes by simply returning bounds
 that are always zero along one or more dimensions.

*/
template<class T> class AABSPTree {
private:

    /** Wrapper for a value that includes a cache of its bounds. */
    class Handle {
    public:
        T                   value;
        AABox               bounds;

        Handle() {}

        inline Handle(const T& v) : value(v) {
            getBounds(v, bounds);
        }
    };


    /**
     A sort predicate that returns true if the midpoint of the
     first argument is less than the midpoint of the second
     along the specified axis.

     Used by makeNode.
     */
    class CenterLT {
    public:
        Vector3::Axis           sortAxis;

        CenterLT(Vector3::Axis a) : sortAxis(a) {}

        inline bool operator()(const Handle& a, const Handle& b) {
            const AABox& A = a.bounds;
            const AABox& B = b.bounds;

            // Only compare distance along the sort axis.  It is faster
            // to sum the low and high than average them.
            return
                (A.low()[sortAxis] + A.high()[sortAxis]) <
                (B.low()[sortAxis] + B.high()[sortAxis]);
        }
    };


    class Node {
    public:

        Vector3::Axis       splitAxis;

        /** Location along the specified axis */
        double              splitLocation;
 
        /** child[0] contains all values strictly 
            smaller than splitLocation along splitAxis.

            child[1] contains all values strictly
            larger.

            Both may be NULL if there are not enough
            values to bother recursing.
        */
        Node*               child[2];

        /** Array of values at this node (i.e. values
            straddling the split plane + all values if
            this is a leaf node). */
        Array<Handle>       valueArray;

        /** Creates node with NULL children */
        Node() {
            splitAxis     = Vector3::X_AXIS;
            splitLocation = 0;
            for (int i = 0; i < 2; ++i) {
                child[i] = NULL;
            }
        }

        /**
         Doesn't clone children.
         */
        Node(const Node& other) : valueArray(other.valueArray) {
            splitAxis       = other.splitAxis;
            splitLocation   = other.splitLocation;
            for (int i = 0; i < 2; ++i) {
                child[i] = NULL;
            }
        }

        /** Copies the specified subarray of pt into point, NULLs the children */
        Node(const Array<Handle>& pt, int beginIndex, int endIndex) {
            splitAxis     = Vector3::X_AXIS;
            splitLocation = 0;
            for (int i = 0; i < 2; ++i) {
                child[i] = NULL;
            }

            int n = endIndex - beginIndex + 1;

            valueArray.resize(n);
            for (int i = n - 1; i >= 0; --i) {
                valueArray[i] = pt[i + beginIndex];
            }
        }


        /** Deletes the children (but not the values) */
        ~Node() {
            for (int i = 0; i < 2; ++i) {
                delete child[i];
            }
        }


        /** Returns true if this node is a leaf (no children) */
        inline bool isLeaf() const {
            return (child[0] == NULL) && (child[1] == NULL);
        }


        /**
         Recursively appends all handles and children's handles
         to the array.
         */
        void getHandles(Array<Handle>& handleArray) const {
            handleArray.append(valueArray);
            for (int i = 0; i < 2; ++i) {
                if (child[i] != NULL) {
                    child[i]->getHandles(handleArray);
                }
            }
        }


        /** Returns the deepest node that completely contains bounds. */
        Node* findDeepestContainingNode(const AABox& bounds) {

            // See which side of the splitting plane the bounds are on
            if (bounds.high()[splitAxis] < splitLocation) {
                // Bounds are on the low side.  Recurse into the child
                // if it exists.
                if (child[0] != NULL) {
                    return child[0]->findDeepestContainingNode(bounds);
                }
            } else if (bounds.low()[splitAxis] > splitLocation) {
                // Bounds are on the high side, recurse into the child
                // if it exists.
                if (child[1] != NULL) {
                    return child[1]->findDeepestContainingNode(bounds);
                }
            }

            // There was no containing child, so this node is the
            // deepest containing node.
            return this;
        }


        /** Appends all members that intersect the box */
        void getIntersectingMembers(const AABox& box, Array<T>& members) const {
            // Test all values at this node
            for (int v = 0; v < valueArray.size(); ++v) {
                if (valueArray[v].bounds.intersects(box)) {
                    members.append(valueArray[v].value);
                }
            }

            // If the left child overlaps the box, recurse into it
            if ((child[0] != NULL) && (box.low()[splitAxis] < splitLocation)) {
                child[0]->getIntersectingMembers(box, members);
            }

            // If the right child overlaps the box, recurse into it
            if ((child[1] != NULL) && (box.high()[splitAxis] > splitLocation)) {
                child[1]->getIntersectingMembers(box, members);
            }
        }
    };

    /** Returns the X, Y, and Z extents of the point sub array. */
    static Vector3 computeExtent(const Array<Handle>& point, int beginIndex, int endIndex) {
        Vector3 lo = Vector3::INF3;
        Vector3 hi = -lo;

        for (int p = beginIndex; p <= endIndex; ++p) {
            lo = lo.min(point[p].bounds.low());
            hi = hi.max(point[p].bounds.high());
        }

        return hi - lo;
    }


    /**
     Recursively subdivides the subarray.
     Begin and end indices are inclusive.
     */
    Node* makeNode(Array<Handle>& point, int beginIndex, int endIndex, int valuesPerNode) {
        Node* node = NULL;

        if (endIndex - beginIndex + 1 <= valuesPerNode) {
            // Make a new leaf node
            node = new Node(point, beginIndex, endIndex);

            // Set the pointers in the memberTable
            for (int i = beginIndex; i <= endIndex; ++i) {
                memberTable.set(point[i].value, node);
            }

        } else {
            // Make a new internal node
            node = new Node();

            Vector3 extent = computeExtent(point, beginIndex, endIndex);

            Vector3::Axis splitAxis = extent.primaryAxis();

            // Compute the median along the axis

            // Sort only the subarray 
            std::sort(
                point.getCArray() + beginIndex,
                point.getCArray() + endIndex + 1,
                CenterLT(splitAxis));
            int midIndex = (beginIndex + endIndex) / 2;

            // Choose the split location between the two middle elements
            const Vector3 median = 
                (point[midIndex].bounds.high() +
                 point[iMin(midIndex + 1, point.size())].bounds.low()) * 0.5;

            node->splitAxis     = splitAxis;
            node->splitLocation = median[splitAxis];
            node->child[0]      = makeNode(point, beginIndex, midIndex, valuesPerNode);
            node->child[1]      = makeNode(point, midIndex + 1, endIndex, valuesPerNode);
        }

        return node;
    }

    /**
     Recursively clone the passed in node tree, setting
     pointers for members in the memberTable as appropriate.
     called by the assignment operator.
     */
    Node* cloneTree(Node* src) {
        Node* dst = new Node(*src);

        // Make back pointers
        for (int i = 0; i < dst->valueArray.size(); ++i) {
            memberTable.set(dst->valueArray[i].value, dst);
        }

        // Clone children
        for (int i = 0; i < 2; ++i) {
            if (src->child[i] != NULL) {
                dst->child[i] = cloneTree(src->child[i]);
            }
        }

        return dst;
    }

    /** Maps members to the node containing them */
    Table<T, Node*>         memberTable;

    Node*                   root;

public:

    /** To construct a balanced tree, insert the elements and then call
      AABSPTree::balance(). */
    AABSPTree() : root(NULL) {}


    AABSPTree(const AABSPTree& src) : root(NULL) {
        *this = src;
    }


    AABSPTree& operator=(const AABSPTree& src) {
        delete root;
        // Clone tree takes care of filling out the memberTable.
        root = cloneTree(src.root);
    }


    ~AABSPTree() {
        clear();
    }

    /**
     Throws out all elements of the set.
     */
    void clear() {
        memberTable.clear();
        delete root;
        root = NULL;
    }

    int size() const {
        return memberTable.size();
    }

    /**
     Inserts an object into the set if it is not
     already present.  O(log n) time.  Does not
     cause the tree to be balanced.
     */
    void insert(const T& value) {
        if (contains(value)) {
            // Already in the set
            return;
        }

        Handle h(value);

        if (root == NULL) {
            // This is the first node; create a root node
            root = new Node();
        }

        Node* node = root->findDeepestContainingNode(h.bounds);

        // Insert into the node
        node->valueArray.append(h);
        
        // Insert into the node table
        memberTable.set(value, node);
    }


    /**
     Returns true if this object is in the set, otherwise
     returns false.  O(1) time.
     */
    bool contains(const T& value) {
        return memberTable.containsKey(value);
    }


    /**
     Removes an object from the set in O(1) time.
     It is an error to remove members that are not already
     present.  May unbalance the tree.
    */
    void remove(const T& value) {
        debugAssertM(contains(value),
            "Tried to remove an element from a "
            "AABSPTree that was not present");

        Array<Handle>& list = memberTable[value]->valueArray;

        // Find the element and remove it
        for (int i = list.length() - 1; i >= 0; --i) {
            if (list[i].value == value) {
                list.fastRemove(i);
                break;
            }
        }
        memberTable.remove(value);
    }


    /**
     If the element is in the set, it is removed.
     The element is then inserted.

     This is useful when the == and hashCode methods
     on <I>T</I> are independent of the bounds.  In
     that case, you may call update(v) to insert an
     element for the first time and call update(v)
     again every time it moves to keep the tree 
     up to date.
     */
    void update(const T& value) {
        if (contains(value)) {
            remove(value);
        }
        insert(value);
    }


    /**
     Rebalances the tree (slow).  Call when objects
     have moved substantially from their original positions
     (which unbalances the tree and causes the spatial
     queries to be slow).
     @param valuesPerNode Maximum number of elements to put at
     a node.
     */
    void balance(int valuesPerNode = 5) {
        if (root == NULL) {
            // Tree must be empty
            return;
        }

        Array<Handle> handleArray;
        root->getHandles(handleArray);

        // Delete the old tree
        clear();

        root = makeNode(handleArray, 0, handleArray.size() - 1, valuesPerNode);
    }


    /**
     C++ STL style iterator variable.  See beginBoxIntersection().
     Overloads the -> (dereference) operator, so this acts like a pointer
     to the current member.
     */
    // This iterator turns Node::getIntersectingMembers into a
    // coroutine.  It first translates that method from recursive to
    // stack based, then captures the system state (analogous to a Scheme
    // continuation) after each element is appended to the member array,
    // and allowing the computation to be restarted.
    class BoxIntersectionIterator {
    private:
        friend class AABSPTree<T>;

        /** True if this is the "end" iterator instance */
        bool            isEnd;
        AABox           box;

        Node*           node;

        /** Nodes waiting to be processed */
        // We could use backpointers within the tree and careful
        // state management to avoid ever storing the stack-- but
        // it is much easier this way and only inefficient if the
        // caller uses post increment (which they shouldn't!).
        Array<Node*>    stack;

        /** The next index of current->valueArray to return. */
        int             v;

        BoxIntersectionIterator() : isEnd(true) {}
        
        BoxIntersectionIterator(const AABox& b, const Node* root) : 
           box(b), isEnd(node != NULL), v(-1), node(root) {

           // We intentionally start at the "-1" index of the current node
           // so we can use the preincrement operator to move ourselves to
           // element 0 instead of repeating all of the code from the preincrement
           // method.  Note that this might cause us to become the "end"
           // instance.
           ++(*this);
        }

    public:

        inline bool operator!=(const BoxIntersectionIterator& other) const {
            return ! (*this == other);
        }

        bool operator==(const BoxIntersectionIterator& other) const {
            if (isEnd) {
                return other.isEnd;
            } else if (other.isEnd) {
                return false;
            } else {
                // Two non-end iterators; see if they match.  This is kind of 
                // silly; users shouldn't call == on iterators in general unless
                // one of them is the end iterator.
                if ((box != other.box) || (node != other.node) || (v != other.v) ||
                    (stack.length() != other.stack.length())) {
                    return false;
                }

                // See if the stacks are the same
                for (int i = 0; i < stack.length(); ++i) {
                    if (stack[i] != other.stack[i]) {
                        return false;
                    }
                }

                // We failed to find a difference; they must be the same
                return true;
            }
        }

        /**
         Pre increment.
         */
        BoxIntersectionIterator& operator++() {
            ++v;
            while (! isEnd && (v >= current->valueArray.length())) {
                // We've exhausted the elements at this node (possibly because
                // we just switched to a child node with no members).

                // If the right child overlaps the box, push it onto the stack for
                // processing.
                if ((current->child[1] != NULL) &&
                    (box.high()[current->splitAxis] > current->splitLocation)) {
                    stack.push(current->child[1]);
                }
                
                // If the left child overlaps the box, push it onto the stack for
                // processing.
                if ((current->child[0] != NULL) &&
                    (box.low()[current->splitAxis] < current->splitLocation)) {
                    stack.push(current->child[0]);
                }

                if (stack.length() > 0) {
                    // Go on to the next node (which may be either one of the ones we 
                    // just pushed, or one from farther back the tree).
                    current = stack.pop();
                    v = 0;
                } else {
                    // That was the last node; we're done iterating
                    isEnd = true;
                }
            }
            return *this;
        }

        /**
         Post increment (much slower than preincrement).
         */
        BoxIntersectionIterator operator++(int) {
            BoxIntersectionIterator old = *this;
            ++this;
            return old;
        }

        /** Overloaded dereference operator so the iterator can masquerade as a pointer
            to a member */
        const T& operator*() const {
            alwaysAssertM(! isEnd, "Can't dereference the end element of an iterator");
            return node->valueArray[v].value;
        }

        /** Overloaded dereference operator so the iterator can masquerade as a pointer
            to a member */
        T* operator->() const {
            alwaysAssertM(! isEnd, "Can't dereference the end element of an iterator");
            return &(stack.last()->valueArray[v].value);
        }

        /** Overloaded cast operator so the iterator can masquerade as a pointer
            to a member */
        operator T*() const {
            alwaysAssertM(! isEnd, "Can't dereference the end element of an iterator");
            return &(stack.last()->valueArray[v].value);
        }
    };

    /**
     Iterates through the members that intersect the
     */
    BoxIntersectionIterator beginBoxIntersection(const AABox& box) const {
        return BoxIntersectionIterator(box, root);
    }

    BoxIntersectionIterator endBoxIntersection() const {
        // The "end" iterator instance
        return BoxIntersectionIterator();
    }

    /**
     Appends all members whose bounds intersect the box.
     See also AABSPTree::beginBoxIntersection.
     */
    void getIntersectingMembers(const AABox& box, Array<T>& members) const {
        if (root == NULL) {
            return;
        }
        root->getIntersectingMembers(box, members);
    }    


    /** See AABSPTree::beginRayIntersection */
    class RayIntersectionIterator {
    private:
        // TODO!  This state is temporary; it may need to change.
        Array<Node*>  stack;
        Node*         current;
        int           v;
        bool          isEnd;
        Ray           ray;

    public:

        double maxDistance;

        double minDistance;

        int    nodeCount;
    };

    /**
      Generates a RayIntersectionIterator that produces successive
      elements from the set whose bounding boxes are intersected by the ray.
      Typically used for ray tracing, hit-scan, and collision detection.

      The elements are generated mostly in the order that they are
      hit by the ray, so that iteration may end abruptly when the closest
      intersection to the ray origin has been reached.  Because the 
      elements within a given kd-tree node are unordered, iteration may
      need to proceed a little past the first member returned in order
      to find the closest intersection.  When a member has been returned
      and the "nodeCount" field is incremented it is safe to stop iterating.
      The iterator doesn't automatically find the first intersection because
      it is looking at bounding boxes, not the true intersections.  
      
      Complicating the matter further, some members straddle the plane.  The
      iterator produces these members <I>twice</I>.  The first time it is produced
      the caller should only consider intersections on the near side of the 
      split plane.  The second time, the caller should only consider intersections
      on the far side.  The minDistance and maxDistance fields specify the
      range on which intersections should be considered.  Be aware that they 
      be inf or zero.
      
      An example of how to use nodeCount, maxDistance, and maxDistance follows.
      Almost all ray intersection tests will have identical structure.

     <PRE>
       typedef AABSPTree<Object*>::RayIntersectionIterator IT;
       void findFirstIntersection(const Ray& ray, Object*& firstObject, double& firstTime) {
           int count     = -1;

           firstObject   = NULL;
           firstDistance = inf;

           const IT end = tree.endRayIntersection();

           for (IT obj = tree.beginRayIntersection(ray);
               (obj != end) && ((firstObject == NULL) || (count == obj.NodeCount)); 
               ++obj) {  // (preincrement is much faster than postincrement!) 

               // Call your accurate intersection test here.  It is guaranteed
               // that the ray hits the bounding box of obj.
               double t = obj->distanceUntilIntersection(ray);

               // Often methods like "distanceUntilIntersection" can be made more
               // efficient by providing them with the time at which to start and
               // to give up looking for an intersection; that is, 
               // obj.minDistance and iMin(firstDistance, obj.maxDistance).

               if ((t < firstDistance) && 
                   (t < obj.maxDistance) &&
                   (t >= obj.minDistance)) {
                   // This is the new best collision time
                   firstObject   = obj;
                   firstDistance = t;

                   // Even if we found an object we must keep iterating until
                   // we've exhausted all members at this node.
                   count         = obj.nodeCount;
               }
           }
       }
     </PRE>
    */
    RayIntersectionIterator beginRayIntersection(const Ray& ray) const;
    
    RayIntersectionIterator endRayIntersection() const;

    /**
     Returns an array of all members of the set.  See also AABSPTree::begin.
     */
    void getMembers(Array<T>& members) const {
        memberTable.getKeys(members);
    }


    /**
     C++ STL style iterator variable.  See begin().
     Overloads the -> (dereference) operator, so this acts like a pointer
     to the current member.
    */
    class Iterator {
    private:
        friend class AABSPTree<T>;

        // Note: this is a Table iterator, we are currently defining
        // Set iterator
        typename Table<T, Node*>::Iterator it;

        Iterator(const typename Table<T, Node*>::Iterator& it) : it(it) {}

    public:
        inline bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

        bool operator==(const Iterator& other) const {
            return it == other.it;
        }

        /**
         Pre increment.
         */
        Iterator& operator++() {
            ++it;
            return *this;
        }

        /**
         Post increment (slower than preincrement).
         */
        Iterator operator++(int) {
            Iterator old = *this;
            ++(*this);
            return old;
        }

        const T& operator*() const {
            return it->key;
        }

        T* operator->() const {
            return &(it->key);
        }

        operator T*() const {
            return &(it->key);
        }
    };


    /**
     C++ STL style iterator method.  Returns the first member.  
     Use preincrement (++entry) to get to the next element (iteration
     order is arbitrary).  
     Do not modify the set while iterating.
     */
    Iterator begin() const {
        return Iterator(memberTable.begin());
    }


    /**
     C++ STL style iterator method.  Returns one after the last iterator
     element.
     */
    const Iterator end() const {
        return Iterator(memberTable.end());
    }
};

#define KDTreeSet AABSPTree

}

#endif
