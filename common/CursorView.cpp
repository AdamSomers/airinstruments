//
//  CursorView.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/21/13.
//
//

#include "CursorView.h"
#include "MotionServer.h"

#if JUCE_WINDOWS
   #include <float.h>
   #define isnan _isnan
#endif

CursorView::CursorView()
: x(0.f)
, y(0.f)
, targetX(0.f)
, targetY(0.f)
, prevX(0.f)
, prevY(0.f)
, filterCoeff(0.3f)
, enabled(true)
, fingerId(-1)
{
    
}

void CursorView::draw()
{
    x = prevX * filterCoeff + targetX * (1.f - filterCoeff);
    y = prevY * filterCoeff + targetY * (1.f - filterCoeff);
    prevX = x;
    prevY = y;

    if (getBounds().x != x)
        setBounds(HUDRect(x, getBounds().y, getBounds().w, getBounds().h));
    if (getBounds().y != y)
        setBounds(HUDRect(getBounds().x, y, getBounds().w, getBounds().h));
    
    View2d::draw();
}

CursorView::~CursorView()
{
    
}

void CursorView::setPosition(float inX, float inY)
{
    stopTimer(kTimerNoChange);
    startTimer(kTimerNoChange, 500);
    if (isnan(inX)) inX = 0;
    if (isnan(inY)) inY = 0;
    targetX = inX;
    targetY = inY;
}

void CursorView::setEnabled(bool shouldBeEnabled)
{
    enabled = shouldBeEnabled;
}

void CursorView::setFingerId(int id)
{
    fingerId = id;
}

CursorView::Listener::Listener()
{
    MotionDispatcher::instance().addCursorListener(*this);
}

CursorView::Listener::~Listener()
{
    MotionDispatcher::instance().removeCursorListener(*this);
}

void CursorView::timerCallback(int timerId)
{
    switch (timerId)
    {
        case kTimerNoChange:
            setEnabled(false);
            targetX = prevX = x = 0.f;
            targetY = prevY = y = 0.f;
            stopTimer(kTimerNoChange);
            break;
    }
}