//
//  StrikeDetector.h
//  AirBeats
//
//  Created by Adam Somers on 4/20/13.
//
//

#ifndef __AirBeats__StrikeDetector__
#define __AirBeats__StrikeDetector__

#include "GL/glew.h"
#include "Leap.h"
#include "../JuceLibraryCode/JuceHeader.h"

// Given a Leap::Hand, this class determines when a drum stike
// motion is performed by moving through various states

class StrikeDetector
{
public:
	enum WaitingState
	{
		kStateStrikeBegin = 0,	// Waiting for downward motion to begin (exceed min velocity threshold)
		kStateStrikeEnd = 1,	// Waiting for downward motion to end (velocity to drop sufficiently below peak level)
		kStateRecoilBegin = 2,	// Waiting for upward motion to begin
		kStateRecoilEnd = 3		// Waiting for upward motion to end
	};

    StrikeDetector();
    
    void handMotion(const Leap::Hand& hand);
    
    const Time& getLastStrikeTime() const;
    
private:
	WaitingState state;
    float maxVel;				// Velocity here is always positive, regardless of direction
	#define kHistoryDepth	3
	float lastVelocity[kHistoryDepth];		// Most recent velocities
	float lastDirection[kHistoryDepth];		// Most recent directions
    Time lastStrikeTime;
    
    bool isLeft(const Leap::Hand& hand);
};

#endif /* defined(__AirBeats__StrikeDetector__) */
