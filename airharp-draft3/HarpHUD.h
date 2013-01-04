#ifndef __AirHarp__HarpHUD__
#define __AirHarp__HarpHUD__

#include <iostream>
#include "HUD.h"

class Toolbar : public HUDView
{
public:
    Toolbar();
    // HUDView overrides
    void setup();
    void draw();
    
private:
    HUDButton b1;
    HUDButton b2;
    HUDButton b3;
    HUDButton b4;
    //    HUDSlider s1;
    //    HUDSlider s2;
    //    HUDSwitch sw1;
    //    HUDSwitch sw2;
    GLBatch batch;
};

#endif /* defined(__AirHarp__HarpHUD__) */
