#ifndef __AirHarp__HarpHUD__
#define __AirHarp__HarpHUD__

#include <iostream>
#include "HUD.h"
#include "Leap.h"

class HarpToolbar : public HUDView
                  , public HUDButton::Listener
{
public:
    HarpToolbar();
    ~HarpToolbar();
    // HUDView overrides
    void setup();
    void draw();
    void updateButtons();
    
    void layoutControls();
    
    // HUDButton::Listener override
    void buttonStateChanged(HUDButton* b);
    
private:
    std::vector<HUDButton*> buttons;
    //    HUDSlider s1;
    //    HUDSlider s2;
    //    HUDSwitch sw1;
    //    HUDSwitch sw2;
    GLBatch batch;
};

class StatusBar : public HUDView
                , public Leap::Listener
{
public:
    StatusBar();
    ~StatusBar();
    // HUDView overrides
    void setup();
    void draw();
    
    void layoutControls();
    
    // Leap overrides
    void onInit(const Leap::Controller& controller);
    void onConnect(const Leap::Controller& controller);
    void onDisconnect(const Leap::Controller& controller);
private:
    HUDButton indicator;
    GLBatch batch;
};

#endif /* defined(__AirHarp__HarpHUD__) */
