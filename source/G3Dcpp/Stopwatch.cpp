/**
 @file Stopwatch.cpp
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
  
 @created 2005-10-05
 @edited  2005-10-05

 Copyright 2000-2003, Morgan McGuire.
 All rights reserved.
 */

#include "G3D/Stopwatch.h"
#include "G3D/System.h"

namespace G3D {

Stopwatch::Stopwatch() : inBetween(false), lastDuration(0), lastCycleCount(0), 
    lastTockTime(-1), m_fps(0), emwaFPS(0), emwaDuration(0) {
    computeOverhead();
}


void Stopwatch::computeOverhead() {
    cycleOverhead = 0;
    tick();
    tock();
    cycleOverhead = elapsedCycles();
}


void Stopwatch::tick() {
    // This is 'alwaysAssert' instead of 'debugAssert'
    // since people rarely profile in debug mode.
    alwaysAssertM(! inBetween, "Stopwatch::tick() called twice in a row.");
    inBetween = true;

    // We read RDTSC twice here, but it is more abstract to implement this
    // way and at least we're reading the cycle count last.
    timeStart = System::time();
    System::beginCycleCount(cycleStart);
}


void Stopwatch::tock() {
    System::endCycleCount(cycleStart);
    RealTime now = System::time();
    lastDuration = now - timeStart;
    if (emwaDuration == 0) {
        emwaDuration = lastDuration;
    } else {
        emwaDuration = lastDuration * 0.01 + emwaDuration * 0.99;
    }

    lastCycleCount = cycleStart - cycleOverhead;
    if (lastCycleCount < 0) {
        lastCycleCount = 0;
    }

    if (lastTockTime != -1.0) {
        m_fps = 1.0 / (now - lastTockTime);

        if (emwaFPS == 0) {
            emwaFPS = m_fps;
        } else {
            if (m_fps <= 10) {
                if (::abs(emwaFPS - m_fps) > .75) {
                    emwaFPS = floor(m_fps * 10.0 + 0.5) / 10.0;
                }
            } else {
                if (::abs(emwaFPS - m_fps) > 1) {
                    emwaFPS = floor(m_fps + 0.5);
                }
            }
        }
    }
    lastTockTime = now;

    alwaysAssertM(inBetween, "Stopwatch::tock() called without matching tick.");
    inBetween = false;
}

}