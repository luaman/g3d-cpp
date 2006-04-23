/**
 @file GModule.h

 @maintainer Morgan McGuire, morgan3d@users.sourceforge.net


 @created 2006-04-22
 @edited  2006-04-22
*/

#ifndef GLG3D_GMODULE_H
#define GLG3D_GMODULE_H

#include "G3D/platform.h"
#include "G3D/Array.h"
#include "G3D/ReferenceCount.h"
#include "GLG3D/PosedModel.h"
#include "GLG3D/GWindow.h"

namespace G3D {

class RenderDevice;
class UserInput;

typedef ReferenceCountedPointer<class GModule> GModuleRef;

/**
 Interface for 2D or 3D objects that experience standard
 virtual world events and are rendered.  This is the most primitive
 form of scene graph representation; it enables a number of 
 other useful G3D pieces to mix and match.

 Modules are intended for use with GApp and GApplet; 
 the GApplet should maintain a list of them and
 invoke the appropriate methods each frame.

 @beta
 */
class GModule : public ReferenceCountedObject {
public:
    // Keep this class pure virtual!

    /** 
     Appends a posed model for this object to the array, if it has a graphic representation.
     The posed model appended is allowed to reference the agent and is allowed to mutate
     if the agent is mutated. 
     */
    virtual void getPosedModel(
        Array<PosedModelRef>& posedArray, 
        Array<PosedModel2DRef>& posed2DArray) = 0;

    // TODO: should this be divided between before/on/after?
    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt) = 0;

    /** Returning true consumes the event and prevents other GModules from seeing it. */
    virtual bool onEvent(const GEvent& event) = 0;

    virtual void onUserInput(UserInput* ui) = 0;

    virtual void onNetwork() = 0;

    virtual void onLogic() = 0;
};

typedef ReferenceCountedPointer<class GModuleManager> GModuleManagerRef;

/**
 Manages a group of GModules.  This is used internally by GApp and GApplet
 to process their modules.  It also enables use of GModules without
 the GApp infrastructure.

 You can use GModules without this class.
 */
class GModuleManager : public GModule {
public:

    enum EventPriority {LOW_PRIORITY, NORMAL_PRIORITY, HIGH_PRIORITY, NUM_PRIORITY};

private:
    
    /** Module index is the priority */
    Array<GModuleRef>   m_moduleArray[NUM_PRIORITY];

    class Add {
    public:
        GModuleRef      module;
        EventPriority   priority;
        Add() {}
        Add(const GModuleRef& m, EventPriority p) : module(m), priority(p) {}
    };

    Array<Add>          m_addList;

    Array<GModuleRef>   m_removeList;

    bool                m_locked;

    /** If true, when the lock is lifted all objects should be removed. */
    bool                m_removeAll;

    GModuleManager();

public:

    static GModuleManagerRef create();

    /** 
      Between beginLock and endLock, add and remove operations are delayed so that 
      iteration is safe.  Locks may not be executed recursively; only one level of
      locking is allowed.
      */
    void beginLock();

    void endLock();

    /** Priorities should generally not be used; they are largely for supporting
        debugging components at HIGH_PRIORITY that intercept events before they
        can hit the regular infrastructure.
      */
    void add(const GModuleRef& m, EventPriority p = NORMAL_PRIORITY);

    void remove(const GModuleRef& m);

    void clear();

    void getPosedModel(
        Array<PosedModelRef>& posedArray, 
        Array<PosedModel2DRef>& posed2DArray);

    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt);

    virtual bool onEvent(const GEvent& event);

    virtual void onUserInput(UserInput* ui);

    virtual void onNetwork();

    virtual void onLogic();
};

} // G3D

#endif
