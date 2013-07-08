//
//  StrikeDetector.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/20/13.
//
//

#include "StrikeDetector.h"
#include "Main.h"

// How fast hand is moving downwards to initiate a strike gesture.
// Note that stike doesn't occur until another threshold is crossed.
#define VELOCITY_THRESHOLD 300.f

// Upper limit to velocity to avoid extreme cases due to tracking jitter
#define MAX_VELOCTIY 1500.f

// 0 -> strike on direction reversal
// 1 -> strike as soon as threshold is crossed
#define SENSITIVITY .50f

// Tracking is jittery up at upper edge of field of view, so ignore tracking there
// Also, the pad area upper edge is at about 310
#define Y_MAX 300.f 
// Position of X-Z strike plane
#define	Y_TRIGGER_BOUNDARY 250.0f

StrikeDetector::StrikeDetector() :
 state(kStateStrikeBegin)
, maxVel(0.f)
, midiNote(-1)
{
	for (int i = 0; i < kHistoryDepth; ++i)
	{
		positionHistory[i] = 0.0f;
		velocityHistory[i] = 0.0f;
#if kDebugHistory
		avgPositionHistory[i] = 0.0f;
		avgVelocityHistory[i] = 0.0f;
#endif
		timestampHistory[i] = 0;
	}
}

#if kDebugHistory
static int noteCount = 0;
static int dataCount = 0;
#endif
void StrikeDetector::handMotion(const Leap::Hand& hand)
{
    midiNote = getNoteForHand(hand);

	float position = hand.palmPosition().y;
	float velocity = hand.palmVelocity().y;
    //int64_t timestamp = hand.frame().timestamp(); // causes crash
	int64_t timestamp = Time::getCurrentTime().currentTimeMillis() * 1000;
    motion(position, velocity, timestamp);
}

void StrikeDetector::pointableMotion(const Leap::Pointable& pointable)
{
    midiNote = getNoteForPointable(pointable);

	float position = pointable.tipPosition().y;
	float velocity = pointable.tipVelocity().y;
    //pointable.frame().timestamp(); // causes crash
	int64_t timestamp = Time::getCurrentTime().currentTimeMillis() * 1000;
    motion(position, velocity, timestamp);
}

void StrikeDetector::motion(float position, float velocity, int64_t timestamp)
{
	SmoothData(velocity, position, timestamp);

	float direction;
	if (velocity < 0.0f)
		direction = -1.0f;
	else
		direction = +1.0f;
	velocity = fabsf(velocity);

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

			bool trigger = direction > 0 || velocity <= (maxVel * SENSITIVITY) /*|| (position <= Y_TRIGGER_BOUNDARY)*/;
            
			if (trigger)
			{
				// calc velocity in range [0.0, 1.0]
				float rangeLow = VELOCITY_THRESHOLD;
				float rangeHigh = MAX_VELOCTIY;
				float val = jmin(maxVel, rangeHigh);
				float vel = (val - rangeLow) / (rangeHigh - rangeLow);

				// play the note
                if (midiNote != -1)
                    Drums::instance().NoteOn(midiNote, vel);
//				Logger::writeToLog(String(leftHand ? "Left" : "Right") + " Hand " + String::formatted("%1.2f", vel));

#if kDebugHistory
				noteCount++;
				dataCount = 0;
#endif

			    lastStrikeTime = Time::getCurrentTime();

				maxVel = 0.0f;
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

int StrikeDetector::getNoteForHand(const Leap::Hand &hand)
{
    const Leap::PointableList& pointables = hand.pointables();
    const size_t numPointables = pointables.count();
    if (numPointables >= 1)
    {
        return getNoteForPointable(pointables.frontmost());
    }
    else
    {
        int padNumber = getPadNumberForPosition(hand.palmPosition().x,
                                                hand.palmPosition().y,
                                                hand.palmPosition().z);
        PropertiesFile* settings = AirHarpApplication::getInstance()->getProperties().getUserSettings();
        return settings->getIntValue("selectedNote" + String(padNumber), -1);
    }
}

int StrikeDetector::getNoteForPointable(const Leap::Pointable& pointable)
{    
    int padNumber = getPadNumberForPointable(pointable);
    PropertiesFile* settings = AirHarpApplication::getInstance()->getProperties().getUserSettings();
    int midiNote = settings->getIntValue("selectedNote" + String(padNumber), -1);
    return midiNote;
}

int StrikeDetector::getPadNumberForHand(const Leap::Hand& hand)
{
    const Leap::PointableList& pointables = hand.pointables();
    const size_t numPointables = pointables.count();
    if (numPointables >= 1)
    {
        return getPadNumberForPointable(pointables.frontmost());
    }
    else
    {
        return getPadNumberForPosition(hand.palmPosition().x,
                                       hand.palmPosition().y,
                                       hand.palmPosition().z);
    }
}

int StrikeDetector::getPadNumberForPointable(const Leap::Pointable& pointable)
{
    return getPadNumberForPosition(pointable.tipPosition().x,
                                   pointable.tipPosition().y,
                                   pointable.tipPosition().z);
}

int StrikeDetector::getPadNumberForPosition(float x, float /*y*/, float z)
{
    int layout = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("layout", -1);
    jassert(layout != -1);
    
    float pointableX = x;
    float pointableZ = z;
    
    int padNumber = 0;
    bool inLeftHalf = false;
    bool inFrontHalf = false;
    bool inMiddleThird = false;
    
    if (pointableX < 0.f)
        inLeftHalf = true;
    
    if (pointableZ > -50)
        inFrontHalf = true;
    
    if (pointableX > -50 && pointableX < 50.f)
        inMiddleThird = true;
    
    switch (layout) {
        case kLayout2x1:
            if (inLeftHalf)
                padNumber = 0;
            else
                padNumber = 1;
            break;
        case kLayout3x1:
            if (inMiddleThird)
                padNumber = 1;
            else if (inLeftHalf)
                padNumber = 0;
            else
                padNumber = 2;
            break;
        case kLayout2x2:
            if (inLeftHalf)
                padNumber = 0;
            else
                padNumber = 1;
            
            if (!inFrontHalf)
                padNumber += 2;
            break;
        case kLayout3x2:
            if (inMiddleThird)
                padNumber = 1;
            else if (inLeftHalf)
                padNumber = 0;
            else
                padNumber = 2;
            
            if (!inFrontHalf)
                padNumber += 3;
            break;
        default:
            break;
    }
    
    return padNumber;
}

const Time& StrikeDetector::getLastStrikeTime() const
{
    return lastStrikeTime;
}


void StrikeDetector::SmoothData(float& velocity, float& position, int64_t timestamp)
{
#if kDebugHistory
	if (noteCount >= 5)
	{
		dataCount++;
		if (dataCount >= 20)
		{
			noteCount = 0;
		}
	}
#endif

	for (int i = kHistoryDepth - 1; i > 0; --i)
	{
		positionHistory[i] = positionHistory[i - 1];
		velocityHistory[i] = velocityHistory[i - 1];
#if kDebugHistory
		avgPositionHistory[i] = avgPositionHistory[i - 1];
		avgVelocityHistory[i] = avgVelocityHistory[i - 1];
#endif
		timestampHistory[i] = timestampHistory[i - 1];
	}

	positionHistory[0] = position;
	velocityHistory[0] = velocity;
	timestampHistory[0] = timestamp;

	float	velTotal = 0.0f;
	float	posTotal = 0.0f;
	float	weightTotal = 0.0f;
	float	weight = 1.0f;

	int count = 0;
	while (count < kHistoryDepth)
	{
		posTotal += (positionHistory[count] * weight);
		velTotal += (velocityHistory[count] * weight);
		weightTotal += weight;
		weight *= kWeightFactor;
		int64_t timeDif = timestamp - timestampHistory[count];
		count++;
		if (timeDif > kMeanDepth)
			break;
	}

	velTotal /= weightTotal;
	posTotal /= weightTotal;

	velocity = velTotal;
	position = posTotal;

#if kDebugHistory
	avgPositionHistory[0] = position;
	avgVelocityHistory[0] = velocity;
#endif
}
