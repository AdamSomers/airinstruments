#include "MotionServer.h"

std::map<int,FingerView*> MotionDispatcher::fingerViews;
std::map<int,HandView*> MotionDispatcher::handViews;
std::vector<FingerView::Listener*> MotionDispatcher::fingerViewListeners;
std::vector<HandView::Listener*> MotionDispatcher::handViewListeners;
float MotionDispatcher::zLimit = 0;

MotionDispatcher::MotionDispatcher()
{
    controller.addListener(*this);
    
    for (int i = 0; i < 50; ++i)
    {
        FingerView* fv = new FingerView;
        fingerViews.insert(std::make_pair(i, fv));
        fv->id = i;
    }
    
    for (int i = 0; i < 50; ++i)
    {
        HandView* hv = new HandView;
        handViews.insert(std::make_pair(i, hv));
        hv->id = i;
    }
    
    for (auto iter : fingerViews)
        iter.second->setup();
    
    for (auto iter : handViews)
        iter.second->setup();
}

MotionDispatcher::~MotionDispatcher()
{
}

void MotionDispatcher::onInit(const Leap::Controller& controller)
{
    std::cout << "Initialized" << std::endl;
}

void MotionDispatcher::onConnect(const Leap::Controller& controller)
{
    std::cout << "Connected" << std::endl;
}

void MotionDispatcher::onDisconnect(const Leap::Controller& controller)
{
    std::cout << "Disconnected" << std::endl;
}

void MotionDispatcher::onFrame(const Leap::Controller& controller)
{
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
    const Leap::Frame frame = controller.frame();
    const Leap::HandList& hands = frame.hands();
    const size_t numHands = hands.count();
    //    std::cout << "Frame id: " << frame.id()
    //    << ", timestamp: " << frame.timestamp()
    //    << ", hands: " << numHands << std::endl;
    
    if (numHands >= 1) {
        // Get the first hand
        for (int h = 0; h < numHands; ++h) {
            const Leap::Hand& hand = hands[h];
            
            bool inserted = false;
            HandView* hv = NULL;
            auto iter = handViews.find(hand.id());
            if (iter == handViews.end())
            {
                // Finger map is pre-allocated, if we don't find one too bad!
                printf("Error! No hand in map for id %d\n", hand.id());
                continue;
            }
            else
            {
                hv = (*iter).second;
                hv->hand = hand;
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
            
            M3DVector3f prev;
            hv->objectFrame.GetForwardVector(prev);
            hv->prevFrame.SetForwardVector(prev);
            hv->objectFrame.GetOrigin(prev);
            hv->prevFrame.SetOrigin(prev);
            
            hv->objectFrame.SetForwardVector(dirX,dirY,-dirZ);
            float scaledX = x*2*(Environment::screenW/(float)Environment::screenH);
            float scaledY = (y-.5)*4;
            if (z < zLimit) z = 0;
            float scaledZ = z*5-12;
            hv->objectFrame.SetOrigin(scaledX,scaledY,scaledZ);
            
            if (inserted) {
                hv->prevFrame.SetForwardVector(dirX,dirY,dirZ);
                hv->prevFrame.SetOrigin(scaledX,scaledY,scaledZ);
            }
            
            for (HandView::Listener* listener : handViewListeners)
            {
                listener->updatePointedState(hv);
            }
            //printf("%1.2f %1.2f %1.2f\n", scaledX,scaledY,scaledZ);
            
            // Check if the hand has any fingers
            const Leap::FingerList& fingers = hand.fingers();
            const size_t numFingers = fingers.count();
            if (numFingers >= 1)
            {
                // Calculate the hand's average finger tip position
                Leap::Vector pos(0, 0, 0);
                for (int i = 0; i < numFingers; ++i)
                {
                    bool inserted = false;
                    const Leap::Finger& f = fingers[i];
                    FingerView* fv = NULL;
                    auto iter = fingerViews.find(f.id());
                    if (iter == fingerViews.end())
                    {
                        // Finger map is pre-allocated, if we don't find one too bad!
                        printf("Error! No finger in map for id %d\n", f.id());
                        continue;
                    }
                    else
                    {
                        fv = (*iter).second;
                        fv->finger = f;
                        if (!fv->inUse) {
                            fv->inUse = true;
                            inserted = true;
                            //printf("Inserted finger %d\n", fv->id);
                        }
                        fv->invalid = false;
                    }
                    
                    float x = fv->normalizedX();
                    float y = fv->normalizedY();
                    float z = fv->normalizedZ();
                    
                    float dirX = f.direction().x;
                    float dirY = f.direction().y;
                    float dirZ = f.direction().z;
                    
                    M3DVector3f prev;
                    fv->objectFrame.GetForwardVector(prev);
                    fv->prevFrame.SetForwardVector(prev);
                    fv->objectFrame.GetOrigin(prev);
                    fv->prevFrame.SetOrigin(prev);
                    
                    fv->objectFrame.SetForwardVector(dirX,dirY,dirZ);
                    float scaledX = x*2*(Environment::screenW/(float)Environment::screenH);
                    float scaledY = (y-.5)*4;
                    if (z < zLimit) z = 0;
                    float scaledZ = z*5-12;
                    fv->objectFrame.SetOrigin(scaledX,scaledY,scaledZ);
                    
                    if (inserted) {
                        fv->prevFrame.SetForwardVector(dirX,dirY,dirZ);
                        fv->prevFrame.SetOrigin(scaledX,scaledY,scaledZ);
                    }

                    for (FingerView::Listener* listener : fingerViewListeners)
                    {
                        listener->updatePointedState(fv);
                    }
                    //printf("%1.2f %1.2f %1.2f\n", scaledX,scaledY,scaledZ);
                    
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
                }
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
        FingerView* fv = iter.second;
        if (fv->invalid && fv->inUse) {
            fv->inUse = false;
            //printf("Removed finger %d\n", fv->id);
        }
    }
    
    for (auto iter : handViews)
    {
        HandView* hv = iter.second;
        if (hv->invalid && hv->inUse) {
            hv->inUse = false;
            //printf("Removed hand %d\n", hv->id);
        }
    }
}