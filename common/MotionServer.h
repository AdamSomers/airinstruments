#ifndef h_MotionDispatcher
#define h_MotionDispatcher

#include "Leap.h"
#include "FingerView.h"
#include "HandView.h"
#include <map>
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"

class MotionDispatcher : public Leap::Listener
{
public:
    MotionDispatcher();
    ~MotionDispatcher();
    static MotionDispatcher& instance( void )
    {
        static MotionDispatcher s_instance;
        return s_instance;
    }
    virtual void onInit(const Leap::Controller&);
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onFrame(const Leap::Controller&);
    
    void spoof(float x, float y, float z);

    static std::map<int,FingerView*> fingerViews;
    static std::map<int,HandView*> handViews;
    static std::vector<FingerView::Listener*> fingerViewListeners;
    static std::vector<HandView::Listener*> handViewListeners;
    Leap::Controller controller;
    static float zLimit;
private:
    void processFinger(const Leap::Finger& finger);
    void processFinger(const Leap::Finger& finger, float x, float y, float z);
    inline float normalizedX(float x)
    {
        return x / 400.f;
    }

    inline float normalizedY(float y)
    {
        return y / 500.f;
    }

    inline float normalizedZ(float z)
    {
        return z / 250.f;
    }
};

#endif // h_MotionDispatcher
