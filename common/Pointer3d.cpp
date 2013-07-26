//
//  Pointer3d.cpp
//  AirBeats
//
//  Created by Adam Somers on 7/25/13.
//
//

#include "Pointer3d.h"
#include "GfxTools.h"
#include "MotionServer.h"

void Pointer3d::getScreenPos(M3DVector2f& inVec)
{
    inVec[0] = screenPos[0];
    inVec[1] = screenPos[1];
}

void Pointer3d::updateScreenPos()
{
    GLint viewport[4];
    viewport[0] = 0;
    viewport[1] = 0;
    viewport[2] = Environment::instance().screenW;
    viewport[3] = Environment::instance().screenH;
    
    M3DVector2f win;
    M3DVector3f origin;
    objectFrame.GetOrigin(origin);
    m3dProjectXY(win,
                 Environment::instance().transformPipeline.GetModelViewMatrix(),
                 Environment::instance().transformPipeline.GetProjectionMatrix(),
                 viewport,
                 origin);
    //printf("win %f %f\n", win[0], win[1]);
    screenPos[0] = win[0];
    screenPos[1] = Environment::instance().screenH - win[1];
}


Pointer3d::Listener::Listener()
: needsReset(false)
{
    MotionDispatcher::instance().fingerViewListeners.push_back(this);
}

Pointer3d::Listener::~Listener()
{
    auto i = std::find(MotionDispatcher::instance().fingerViewListeners.begin(), MotionDispatcher::instance().fingerViewListeners.end(), this);
    if (i != MotionDispatcher::instance().fingerViewListeners.end())
        MotionDispatcher::instance().fingerViewListeners.erase(i);
}

void Pointer3d::Listener::reset()
{
    pointers.clear();
    needsReset = false;
}

void Pointer3d::Listener::fingerPointing(Pointer3d* fv)
{
    auto iter = std::find(pointers.begin(), pointers.end(), fv);
    if (iter == pointers.end())
    {
        pointers.push_back(fv);
    }
    needsReset = false;
}

void Pointer3d::Listener::fingerNotPointing(Pointer3d* fv)
{
    auto iter = std::find(pointers.begin(), pointers.end(), fv);
    if (iter != pointers.end() && (*iter) == fv) {
        pointers.erase(iter);
        needsReset = false;
    }
}

void Pointer3d::Listener::tap(Pointer3d* fv, float velocity)
{
    auto iter = std::find(pointers.begin(), pointers.end(), fv);
    if (iter != pointers.end())
        this->tap(velocity);
}

void Pointer3d::Listener::circleBack(Pointer3d* fv)
{
    auto iter = std::find(pointers.begin(), pointers.end(), fv);
    if (iter != pointers.end())
        this->circleBack();
}