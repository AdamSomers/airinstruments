//
//  CursorView.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/21/13.
//
//

#include "CursorView.h"
#include "MotionServer.h"

CursorView::CursorView()
: targetX(0.f)
, targetY(0.f)
, enabled(true)
, fingerId(-1)
{
    
}

void CursorView::draw()
{
    if (!enabled)
        return;

    if (getBounds().x != targetX)
        setBounds(HUDRect(targetX, getBounds().y, getBounds().w, getBounds().h));
    if (getBounds().y != targetY)
        setBounds(HUDRect(getBounds().x, targetY, getBounds().w, getBounds().h));
    
    View2d::draw();
}

CursorView::~CursorView()
{
    
}

void CursorView::setX(float x)
{
    targetX = x;
}

void CursorView::setY(float y)
{
    targetY = y;
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
