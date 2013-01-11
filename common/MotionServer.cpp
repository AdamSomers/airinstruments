#include "MotionServer.h"

std::map<int,FingerView*> MotionDispatcher::fingerViews;
std::vector<FingerView::Listener*> MotionDispatcher::fingerViewListeners;

MotionDispatcher::MotionDispatcher()
{
    controller.addListener(*this);
    
    for (int i = 0; i < 50; ++i)
    {
        FingerView* fv = new FingerView;
        fingerViews.insert(std::make_pair(i, fv));
        fv->id = i;
    }
    
    for (auto iter : fingerViews)
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
    for (auto i : fingerViews)
    {
        i.second->invalid = true;
    }
    
    for (FingerView::Listener* listener : fingerViewListeners)
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
                            printf("Inserted finger %d\n", fv->id);
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
                    float scaledY = (y-.5)*2;
                    if (z < 0) z = 0;
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
    
    for (auto iter : fingerViews)
    {
        FingerView* fv = iter.second;
        if (fv->invalid && fv->inUse) {
            fv->inUse = false;
            printf("Removed finger %d\n", fv->id);
        }
    }
}