#ifndef __AirHarp__HarpHUD__
#define __AirHarp__HarpHUD__

#include <iostream>
#include "HUD.h"

class Toolbar : public HUDView
{
public:
    Toolbar();
    ~Toolbar();
    // HUDView overrides
    void setup();
    void draw();
    
    void layoutControls();
    
private:
    std::vector<HUDButton*> buttons;
    //    HUDSlider s1;
    //    HUDSlider s2;
    //    HUDSwitch sw1;
    //    HUDSwitch sw2;
    GLBatch batch;
};

#endif /* defined(__AirHarp__HarpHUD__) */
