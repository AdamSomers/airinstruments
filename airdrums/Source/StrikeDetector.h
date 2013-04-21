//
//  StrikeDetector.h
//  AirBeats
//
//  Created by Adam Somers on 4/20/13.
//
//

#ifndef __AirBeats__StrikeDetector__
#define __AirBeats__StrikeDetector__

#include "../JuceLibraryCode/JuceHeader.h"
#include "Leap.h"

// Given a Leap::Hand, this class determines when a drum stike
// motion is performed by moving through various states:
//
// State1, Idle: Detector is waiting for rapid downwards motion.
//
// State2, Mid-Strike: hand has moved quickly in donwards motion,
// and we expect an abrupt change in velocity.  Track a maximim
// downwards velocity for comparison
//
// State3, Post-Strike: hand has slowed down from maximum velocity
// or changed direction, causing a strike.  The amount that the hand
// needs to have slowed is set by sensitivity control.
//
// Return to state 1 when hand has moved in upwards motion (recoil)

class StrikeDetector
{
public:
    StrikeDetector();
    
    void handMotion(const Leap::Hand& hand);
    
    const Time& getLastStrikeTime() const;
    
private:
    bool crossedVelocityThreshold;
    bool recoilPending;
    float maxVel;
    Time lastStrikeTime;
    
    bool isLeft(const Leap::Hand& hand);
};

#endif /* defined(__AirBeats__StrikeDetector__) */
