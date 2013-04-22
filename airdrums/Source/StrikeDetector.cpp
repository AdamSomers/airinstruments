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
#define VELOCITY_THRESHOLD 300.f

// Upper limit to velocity to avoid extreme cases due to tracking jitter
#define MAX_VELOCTIY 1500.f

// 0 -> strike on direction reversal
// 1 -> strike as soon as threshold is crossed
#define SENSITIVITY .90f

// Tracking is jittery up at upper edge of field of view, so ignore tracking there
// Also, the pad area upper edge is at about 310
#define Y_MAX 300.f 

StrikeDetector::StrikeDetector() :
 state(kStateStrikeBegin)
, maxVel(0.f)
{
}

void StrikeDetector::handMotion(const Leap::Hand& hand)
{
	float position = hand.palmPosition().y;
	float velocity = hand.palmVelocity().y;
	float direction;
	if (velocity < 0.0f)
		direction = -1.0f;
	else
		direction = +1.0f;
	velocity = fabsf(velocity);

	for (int i = kHistoryDepth - 1; i > 0 ; --i)
	{
		lastVelocity[i] = lastVelocity[i - 1];
		lastDirection[i] = lastDirection[i - 1];
	}
	lastVelocity[0] = velocity;
	lastDirection[0] = direction;

	switch (state)
	{
		default :
			jassertfalse;
			return;
		case kStateRecoilEnd :
		{
			if (direction < 0.0f)	// Downward motion
			{
				state = kStateStrikeBegin;
				// And then fall through
			}
			else
				break;
		}
		case kStateStrikeBegin :
		{
			if ((direction < 0.0f)					// Downward motion
				&& (velocity > VELOCITY_THRESHOLD)	// Moving faster than minimum velocity to signify a strike
				&& (position < Y_MAX))				// Below top of valid Y range
			{
				state = kStateStrikeEnd;
				// And then fall through
			}
			else
				break;
		}
		case kStateStrikeEnd :
		{
			if (velocity > maxVel)
				maxVel = velocity;

			bool trigger = true;
			for (int i = 0; i < kHistoryDepth; ++i)
			{
				if (lastDirection[i] > 0.0f)					// Upward motion
				{
					trigger = false;
					break;
				}
				if (lastVelocity[i] >= (maxVel * SENSITIVITY))	// Velocity has not slowed down sufficiently to trigger a strike
				{
					trigger = false;
					break;
				}
			}

			if (trigger)
			{
				// calc velocity in range [0.0, 1.0]
				float rangeLow = VELOCITY_THRESHOLD;
				float rangeHigh = MAX_VELOCTIY;
				float val = jmin(maxVel, rangeHigh);
				float vel = (val - rangeLow) / (rangeHigh - rangeLow);
				maxVel = 0.0f;
        
				// Get midi note to play based on left/right 
				bool leftHand = isLeft(hand);
				int midiNote = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue(leftHand ? "selectedNoteLeft" : "selectedNoteRight", 0);

				// play the note
				Drums::instance().NoteOn(midiNote, vel);
//				Logger::outputDebugString(String(leftHand ? "Left" : "Right") + " Hand " + String::formatted("%1.2f", vel));

			    lastStrikeTime = Time::getCurrentTime();

				state = kStateRecoilBegin;
			}

			break;
		}
		case kStateRecoilBegin :
		{
			if (direction > 0.0f)	// Upward motion
				state = kStateRecoilEnd;
			break;
		}
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
