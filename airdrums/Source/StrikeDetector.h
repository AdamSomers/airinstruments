//
//  StrikeDetector.h
//  AirBeats
//
//  Created by Adam Somers on 4/20/13.
//
//

#ifndef __AirBeats__StrikeDetector__
#define __AirBeats__StrikeDetector__

#include "Drums.h"

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

    enum Layout
    {
        kLayout2x1 = 0,
        kLayout3x1,
        kLayout2x2,
        kLayout3x2
    };

    StrikeDetector();
    
    void handMotion(const Leap::Hand& hand);
    void pointableMotion(const Leap::Pointable& pointable);
    void motion(float position, float velocity, int64_t timestamp, float sensitivity);

    int getNoteForHand(const Leap::Hand& hand);
    int getNoteForPointable(const Leap::Pointable& pointable);
    int getPadNumberForHand(const Leap::Hand& hand);
    int getPadNumberForPointable(const Leap::Pointable& pointable);
    int getPadNumberForPosition(float x, float y, float z);

    const Time& getLastStrikeTime() const;
    
private:
	#define	kDebugHistory	0

	WaitingState state;
    float maxVel;				// Velocity here is always positive, regardless of direction
    Time lastStrikeTime;

	#define	kWeightFactor	0.75f
	#define	kMeanDepth		30000	// Time span in uS
#if kDebugHistory
	#define	kHistoryDepth	50
#else
	#define	kHistoryDepth	8
#endif
	float	positionHistory[kHistoryDepth];
	float	velocityHistory[kHistoryDepth];
#if kDebugHistory
	float	avgPositionHistory[kHistoryDepth];
	float	avgVelocityHistory[kHistoryDepth];
#endif
	int64_t timestampHistory[kHistoryDepth];
    
	void SmoothData(float& velocity, float& position, int64_t timestamp);
    
    int midiNote;
};

#endif /* defined(__AirBeats__StrikeDetector__) */
