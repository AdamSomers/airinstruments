//
//  StrikeDetector.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/20/13.
//
//

#include "StrikeDetector.h"
#include "Drums.h"
#include "Main.h"

// How fast hand is moving downwards to initiate a strike gesture.
// Note that stike doesn't occur until another threshold is crossed.
#define VELOCITY_THRESHOLD -200.f

// Upper limit to velocity to avoid extreme cases due to tracking jitter
#define MAX_VELOCTIY -1000.f

// 0 -> strike on direction reversal
// 1 -> strike as soon as threshold is crossed
#define SENSITIVITY .5f

// Tracking is jittery up at upper edge of field of view, so ignore tracking there
#define Y_MAX 500.f 

StrikeDetector::StrikeDetector()
: crossedVelocityThreshold(false)
, recoilPending(false)
, maxVel(0.f)
{
}

void StrikeDetector::handMotion(const Leap::Hand& hand)
{
    // Get midi note to play based on left/right 
    int midiNote = 0;
    bool leftHand = isLeft(hand);
    midiNote = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue(leftHand ? "selectedNoteLeft" : "selectedNoteRight", 0);
    
    if (!recoilPending &&
        hand.palmVelocity().y < VELOCITY_THRESHOLD &&
        hand.palmPosition().y < Y_MAX)
    {
        crossedVelocityThreshold = true;
        if (hand.palmVelocity().y < maxVel)
            maxVel = hand.palmVelocity().y;
    }
    else if (hand.palmVelocity().y > 0) // palm moving up
    {
        // move to idle state
        recoilPending = false;
    }

    if (crossedVelocityThreshold &&
        (hand.palmVelocity().y > 0.f || // direction changed
         hand.palmVelocity().y > (maxVel * SENSITIVITY))) // slowed down 'enough'
    {
        // calc velocity in range [0.0, 1.0]
        float rangeLow = fabsf(VELOCITY_THRESHOLD);
        float rangeHigh = fabsf(MAX_VELOCTIY);
        float val = jmin(fabsf(maxVel), rangeHigh);
        float vel = (val - rangeLow) / (rangeHigh - rangeLow);
        
        // play the note
        Drums::instance().NoteOn(midiNote, vel);
        
        // move to post-strike state
        recoilPending = true;
        crossedVelocityThreshold = false;
        maxVel = 0.f;
        Logger::outputDebugString(String(leftHand ? "Left" : "Right") + " Hand " + String::formatted("%1.2f", vel));
        
        lastStrikeTime = Time::getCurrentTime();
    }
}

// Whether a hand is considered to be on the left or right side of the screen
// is first given by the palm vectors position, but if an extended finger is
// crossing the center line, the user probably intends to play the the opposite side.
bool StrikeDetector::isLeft(const Leap::Hand &hand)
{
    bool leftHand = false;
    if (hand.palmPosition().x < 0)
        leftHand = true;
    
    const Leap::PointableList& pointables = hand.pointables();
    const size_t numPointables = pointables.count();
    if (numPointables >= 1)
    {
        int pointableClosestToScreen = 0;
        float zMin = 999.f;
        for (unsigned int i = 0; i < numPointables; ++i)
        {
            const Leap::Pointable& p = pointables[i];
            if (p.tipPosition().z < zMin)
            {
                pointableClosestToScreen = i;
                zMin = p.tipPosition().z;
            }
        }
        
        if (!leftHand && pointables[pointableClosestToScreen].tipPosition().x < 0.f)
            leftHand = true;
        else if (leftHand && pointables[pointableClosestToScreen].tipPosition().x > 0.f)
            leftHand = false;
    }
    return leftHand;
}

const Time& StrikeDetector::getLastStrikeTime() const
{
    return lastStrikeTime;
}
