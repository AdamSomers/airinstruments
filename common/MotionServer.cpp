#include "MotionServer.h"

float MotionDispatcher::zLimit = 0;
MotionDispatcher* MotionDispatcher::s_instance = nullptr;

MotionDispatcher::MotionDispatcher()
: paused(false)
, useHandsAndFingers(false)
{
    addListener(*this);
    controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
    controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
    controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
    
    cursor = SharedPtr<CursorView>(new CursorView);
    cursor->setBounds(HUDRect(0, 0, 20, 20));
    
    for (auto iter : fingerViews)
        iter.second->setup();
    
    for (auto iter : handViews)
        iter.second->setup();
}

MotionDispatcher::~MotionDispatcher()
{
    removeListener(*this);
    fingerViewListeners.clear();
    handViewListeners.clear();
	cursorViewListeners.clear();
}

void MotionDispatcher::addListener(Leap::Listener& l)
{
    auto iter = std::find(listeners.begin(), listeners.end(), &l);
    if (iter == listeners.end())
	{
		controller.addListener(l);
		listeners.push_back(&l);
	}
}

void MotionDispatcher::removeListener(Leap::Listener& l)
{
    auto iter = std::find(listeners.begin(), listeners.end(), &l);
    if (iter != listeners.end())
	{
	    controller.removeListener(l);
        listeners.erase(iter);
	}
}

void MotionDispatcher::pause()
{
    if (!paused)
    {
        for (Leap::Listener* l : listeners)
            controller.removeListener(*l);
        paused = true;
    }
}

void MotionDispatcher::resume()
{
    if (paused)
    {
        for (Leap::Listener* l : listeners)
            controller.addListener(*l);
        paused = false;
    }
}

void MotionDispatcher::stop()
{
    controller.removeListener(*this);
}

void MotionDispatcher::addCursorListener(CursorView::Listener& listener)
{
	ScopedLock lock(listenerLock);
    cursorViewListeners.push_back(&listener);
}


void MotionDispatcher::removeCursorListener(CursorView::Listener& listener)
{
	ScopedLock lock(listenerLock);
    auto i = std::find(cursorViewListeners.begin(), cursorViewListeners.end(), &listener);
    if (i != cursorViewListeners.end())
        cursorViewListeners.erase(i);
}

void MotionDispatcher::onInit(const Leap::Controller& /*controller*/)
{
    std::cout << "Initialized" << std::endl;
}

void MotionDispatcher::onConnect(const Leap::Controller& /*controller*/)
{
    std::cout << "Connected" << std::endl;
}

void MotionDispatcher::onDisconnect(const Leap::Controller& /*controller*/)
{
    std::cout << "Disconnected" << std::endl;
}

void MotionDispatcher::onFrame(const Leap::Controller& controller)
{
    const Leap::Frame frame = controller.frame();

    const Leap::PointableList& pointables = frame.pointables();
    const Leap::ScreenList& screens = controller.calibratedScreens();
    if (pointables.count() > 0 && screens.count() > 0)
    {
        float x = 0.f;
        float y = 0.f;
        const Leap::Pointable& p = frame.pointable(cursor->getFingerId());
        if (p.isValid())
        {
            const Leap::Screen& s = screens[0];
            const Leap::Vector& v = s.intersect(p, true);
            x = v.x * Environment::instance().screenW;
            y = v.y * Environment::instance().screenH;
        }
        else
        {
            const Leap::Screen& s = screens[0];
            const Leap::Vector& v = s.intersect(pointables[0], true);
            x = v.x * Environment::instance().screenW;
            y = v.y * Environment::instance().screenH;
            int closestIndex = 0;
            for (int i = 0; i < pointables.count(); ++i)
            {
                const Leap::Pointable& p = pointables[i];
                if (p.tipPosition().z < pointables[closestIndex].tipPosition().z)
                    closestIndex = i;
            }
            cursor->setFingerId(pointables[closestIndex].id());
        }

        
        if (!cursor->isEnabled())
            x = y = 0.f;
        
        cursor->setPosition(x, y);
		ScopedLock lock(listenerLock);
        for (CursorView::Listener* l : cursorViewListeners)
            l->updateCursorState(x + cursor->getBounds().w / 2.f, y + cursor->getBounds().h / 2.f);
    }
    else
    {
        // cursor handles disabling itself via timer
    }

    if (!useHandsAndFingers)
        return;

    if (!Environment::instance().ready)
        return;
    
    for (auto i : fingerViews)
    {
        i.second->invalid = true;
    }
    
    for (FingerView::Listener* listener : fingerViewListeners)
    {
        listener->needsReset = true;
    }
    
    for (auto i : handViews)
    {
        i.second->invalid = true;
    }
    
    for (HandView::Listener* listener : handViewListeners)
    {
        listener->needsReset = true;
    }
    
    // Get the most recent frame and report some basic information
    const Leap::HandList& hands = frame.hands();
    const size_t numHands = hands.count();
    //    std::cout << "Frame id: " << frame.id()
    //    << ", timestamp: " << frame.timestamp()
    //    << ", hands: " << numHands << std::endl;
    
    if (numHands >= 1) {
        // Get the first hand
        for (unsigned int h = 0; h < numHands; ++h) {
            const Leap::Hand& hand = hands[h];
            
            bool inserted = false;
            SharedPtr<HandView> hv;
            auto iter = handViews.find(hand.id());
            if (iter == handViews.end())
            {
                Logger::outputDebugString("Adding hand " + String(hand.id()) + " to map");
                SharedPtr<HandView> newHv(new HandView);
                newHv->inUse = true;
                newHv->invalid = false;
                newHv->id = hand.id();
                handViews.insert(std::make_pair(hand.id(), newHv));
                hv = newHv;
                inserted = true;

            }
            else
            {
                hv = (*iter).second;
                if (!hv->inUse) {
                    hv->inUse = true;
                    inserted = true;
                    //printf("Inserted hand %d\n", hv->id);
                }
                hv->invalid = false;
            }
            
            float x = hv->normalizedX();
            float y = hv->normalizedY();
            float z = hv->normalizedZ();
            
            float dirX = hand.direction().x;
            float dirY = hand.direction().y;
            float dirZ = hand.direction().z;
            
            float upX = hand.palmNormal().x;
            float upY = hand.palmNormal().y;
            float upZ = hand.palmNormal().z;
            
            M3DVector3f prev;
            hv->objectFrame.GetForwardVector(prev);
            hv->prevFrame.SetForwardVector(prev);
            hv->objectFrame.GetOrigin(prev);
            hv->prevFrame.SetOrigin(prev);
            
            hv->objectFrame.SetForwardVector(dirX,dirY,dirZ);
            hv->objectFrame.SetUpVector(upX,upY,upZ);
            float scaledX = x*2*(Environment::screenW/(float)Environment::screenH);
            float scaledY = (y-.5f)*4;
            if (z < zLimit) z = 0;
            float scaledZ = z*5-12;
            hv->objectFrame.SetOrigin(scaledX,scaledY,scaledZ);
            
            if (inserted) {
                hv->prevFrame.SetForwardVector(dirX,dirY,dirZ);
                hv->prevFrame.SetOrigin(scaledX,scaledY,scaledZ);
            }
            
            for (HandView::Listener* listener : handViewListeners)
            {
                listener->updatePointedState(hv.get());
            }
            //printf("%1.2f %1.2f %1.2f\n", scaledX,scaledY,scaledZ);
            
            // Check if the hand has any fingers
            const Leap::PointableList& pointables = hand.pointables();
            const size_t numPointables = pointables.count();
            if (numPointables >= 1)
            {
                // Calculate the hand's average finger tip position
                Leap::Vector pos(0, 0, 0);
                for (unsigned int i = 0; i < numPointables; ++i)
                {
                    const Leap::Pointable& pointable = pointables[i];
                    Pointer p;
                    p.position = pointable.tipPosition();
                    p.direction = pointable.direction();
                    p.id = pointable.id();
                    processFinger(p, frame);
                }
            }
            else
            {
                Pointer p;
                p.position = hand.palmPosition();
                p.direction = hand.direction();
                p.id = hand.id();
                processFinger(p, frame);
            }
        }
    }
    
    for (FingerView::Listener* listener : fingerViewListeners)
    {
        if (listener->needsReset) {
            listener->reset();
        }
    }
    
    for (HandView::Listener* listener : handViewListeners)
    {
        if (listener->needsReset) {
            listener->reset();
        }
    }
    
    for (auto iter : fingerViews)
    {
        SharedPtr<FingerView> fv = iter.second;
        if (fv->invalid && fv->inUse) {
            fv->inUse = false;
            //printf("Removed finger %d\n", fv->id);
        }
    }
    
    for (auto iter : handViews)
    {
        SharedPtr<HandView> hv = iter.second;
        if (hv->invalid && hv->inUse) {
            hv->inUse = false;
            //printf("Removed hand %d\n", hv->id);
        }
    }
}

void MotionDispatcher::processFinger(const Pointer& pointer, const Leap::Frame& frame)
{
    bool inserted = false;
    SharedPtr<FingerView> fv;
    auto iter = fingerViews.find(pointer.id);
    if (iter == fingerViews.end())
    {
        Logger::outputDebugString("Adding finger " + String(pointer.id) + " to map");
        SharedPtr<FingerView> newFv(new FingerView);
        newFv->inUse = true;
        newFv->invalid = false;
        newFv->id = pointer.id;
        fingerViews.insert(std::make_pair(pointer.id, newFv));
        fv = newFv;
    }
    else
    {
        fv = (*iter).second;
        if (!fv->inUse) {
            fv->inUse = true;
            inserted = true;
            //printf("Inserted finger %d\n", fv->id);
        }
        fv->invalid = false;
    }
    
    float x = normalizedX(pointer.position.x);
    float y = normalizedY(pointer.position.y);
    float z = normalizedZ(pointer.position.z);
    
    float dirX = pointer.direction.x;
    float dirY = pointer.direction.y;
    float dirZ = pointer.direction.z;
    
    M3DVector3f prev;
    fv->objectFrame.GetForwardVector(prev);
    fv->prevFrame.SetForwardVector(prev);
    fv->objectFrame.GetOrigin(prev);
    fv->prevFrame.SetOrigin(prev);
    
    fv->objectFrame.SetForwardVector(dirX,dirY,dirZ);
    float scaledX = x*2*(Environment::screenW/(float)Environment::screenH);
    float scaledY = (y-.5f)*8;
    if (z < zLimit) z = 0;
    float scaledZ = z*5-12;
    fv->objectFrame.SetOrigin(scaledX,scaledY,scaledZ);
    
    if (inserted) {
        fv->prevFrame.SetForwardVector(dirX,dirY,dirZ);
        fv->prevFrame.SetOrigin(scaledX,scaledY,scaledZ);
    }
    
    for (FingerView::Listener* listener : fingerViewListeners)
    {
        if (fv->inUse)
            listener->updatePointedState(fv.get());
    }
    //printf("%1.2f %1.2f %1.2f\n", scaledX,scaledY,scaledZ);
    
    
#if 0
    // This was my home-spun tapping gesture before Leap provided Gesture API
    Leap::Finger prevFinger = controller.frame(1).finger(f.id());
    if (prevFinger.isValid())
    {
        float prevVel = prevFinger.tipVelocity().y / 1000.f;
        float vel = f.tipVelocity().y / 1000.f;
        float diff = fabsf(vel - prevVel);
        if (vel < -.3 && prevVel > -.3)
        {
            for (FingerView::Listener* listener : fingerViewListeners)
            {
                listener->tap(fv, fminf((fabsf(vel) - fabsf(prevVel)) * 3.f + 0.5, 1.f));
            }
        }
    }
#endif
    
    const Leap::GestureList& gestures = frame.gestures();
    Leap::GestureList::const_iterator i = gestures.begin();
    Leap::GestureList::const_iterator end = gestures.end();
    for ( ; i != end; ++i)
    {
        const Leap::Gesture& g = *i;
        
        // Need to make sure current finger is associated with the gesture
        const Leap::PointableList& pointables = g.pointables();
        Leap::PointableList::const_iterator pIter = pointables.begin();
        Leap::PointableList::const_iterator pIterEnd = pointables.end();
        for ( ; pIter != pIterEnd; ++pIter)
        {
            const Leap::Pointable& p = *pIter;
            if (p.id() == pointer.id)
            {
                switch (g.type())
                {
                    case Leap::Gesture::TYPE_KEY_TAP:
                    case Leap::Gesture::TYPE_SCREEN_TAP:
                    {
                        Leap::KeyTapGesture tap(g);
                        
                        // Current finger applies to this gesture, broadcast to listeners
                        // Those being pointed to by current finger view should handle the tap
                        for (FingerView::Listener* listener : fingerViewListeners)
                        {
                            listener->tap(fv.get(), 1.f);
                        }
                    }
                        break;
                    case Leap::Gesture::TYPE_CIRCLE:
                    {
                        Leap::CircleGesture circle(g);
                        if (circle.normal().z > 0) // z-direction of circle's normal vector indicates angular direction
                        {
                            for (FingerView::Listener* listener : fingerViewListeners)
                            {
                                listener->circleBack(fv.get());
                            }
                        }
                    }
                        
                    default:
                        break;
                }
            }
        }
    }
    
}

void MotionDispatcher::spoof(float inX, float inY, float inZ)
{
    printf("%1.2f %1.2f %1.2f\n", inX,inY,inZ);
    bool inserted = false;
    SharedPtr<FingerView> fv;
    int fakeId = 0;
    Leap::Finger fakeFinger;
    float fakeDir[3] = { 0, 0, -1 };
    auto iter = fingerViews.find(fakeId);
    if (iter == fingerViews.end())
    {
        // Finger map is pre-allocated, if we don't find one too bad!
        printf("Error! No finger in map for id %d\n", fakeId);
        return;
    }
    else
    {
        fv = (*iter).second;
        if (!fv->inUse) {
            fv->inUse = true;
            inserted = true;
            printf("Inserted finger %d\n", fv->id);
        }
        fv->invalid = false;
    }
    
    float x = normalizedX(inX);
    float y = normalizedY(inY);
    float z = normalizedZ(inZ);
    
    float dirX = fakeDir[0];
    float dirY = fakeDir[1];
    float dirZ = fakeDir[2];
    
    M3DVector3f prev;
    fv->objectFrame.GetForwardVector(prev);
    fv->prevFrame.SetForwardVector(prev);
    fv->objectFrame.GetOrigin(prev);
    fv->prevFrame.SetOrigin(prev);
    
    fv->objectFrame.SetForwardVector(dirX,dirY,dirZ);
    float scaledX = x*2*(Environment::screenW/(float)Environment::screenH);
    float scaledY = (y-.5f)*4;
    if (z < zLimit) z = 0;
    float scaledZ = z*5-12;
    fv->objectFrame.SetOrigin(scaledX,scaledY,scaledZ);
    
    if (inserted) {
        fv->prevFrame.SetForwardVector(dirX,dirY,dirZ);
        fv->prevFrame.SetOrigin(scaledX,scaledY,scaledZ);
    }
    
    for (FingerView::Listener* listener : fingerViewListeners)
    {
        listener->updatePointedState(fv.get());
    }
    //printf("%1.2f %1.2f %1.2f\n", scaledX,scaledY,scaledZ);
    
//    Leap::Finger prevFinger = controller.frame(1).finger(f.id());
//    if (prevFinger.isValid())
//    {
//        float prevVel = prevFinger.tipVelocity().y / 1000.f;
//        float vel = f.tipVelocity().y / 1000.f;
//        float diff = fabsf(vel - prevVel);
//        if (vel < -.3 && prevVel > -.3)
//        {
//            for (FingerView::Listener* listener : fingerViewListeners)
//            {
//                listener->tap(fv, fminf((fabsf(vel) - fabsf(prevVel)) * 3.f + 0.5, 1.f));
//            }
//        }
//    }

    for (FingerView::Listener* listener : fingerViewListeners)
    {
        if (listener->needsReset) {
            listener->reset();
        }
    }
    
    for (auto iter : fingerViews)
    {
        SharedPtr<FingerView> fv = iter.second;
        if (fv->invalid && fv->inUse) {
            fv->inUse = false;
            printf("Removed finger %d\n", fv->id);
        }
    }
    
}

void MotionDispatcher::setCursorTexture(TextureDescription texture)
{
    if (cursor)
        cursor->setDefaultTexture(texture);
}