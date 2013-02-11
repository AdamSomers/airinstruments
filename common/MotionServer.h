#ifndef h_MotionDispatcher
#define h_MotionDispatcher

#include "Leap.h"
#include "FingerView.h"
#include "HandView.h"
#include <map>
#include <vector>

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

    static std::map<int,FingerView*> fingerViews;
    static std::map<int,HandView*> handViews;
    static std::vector<FingerView::Listener*> fingerViewListeners;
    static std::vector<HandView::Listener*> handViewListeners;
    Leap::Controller controller;
    static float zLimit;
};

#endif // h_MotionDispatcher
