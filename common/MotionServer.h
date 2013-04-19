#ifndef h_MotionDispatcher
#define h_MotionDispatcher

#include "Leap.h"
#include "FingerView.h"
#include "HandView.h"
#include "Types.h"
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
		if (s_instance == nullptr)
	        s_instance = new MotionDispatcher;
        return *s_instance;
    }
	static void destruct( void )
	{
		if (s_instance != nullptr)
		{
			delete s_instance;
			s_instance = nullptr;
		}
	}
    
    void addListener(Leap::Listener& l);
    void removeListener(Leap::Listener& l);
    void removeAllListeners();
    void pause();
    void resume();
    
    virtual void onInit(const Leap::Controller&);
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onFrame(const Leap::Controller&);
    
    void spoof(float x, float y, float z);

    std::map<int, SharedPtr<FingerView> > fingerViews;
    std::map<int, SharedPtr<HandView> > handViews;
    std::vector<FingerView::Listener*> fingerViewListeners;
    std::vector<HandView::Listener*> handViewListeners;
    Leap::Controller controller;
    static float zLimit;
private:
    void processFinger(const Leap::Finger& finger, const Leap::Frame& frame);
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

	static MotionDispatcher* s_instance;
    
    std::vector<Leap::Listener*> listeners;
    bool paused;
};

#endif // h_MotionDispatcher
