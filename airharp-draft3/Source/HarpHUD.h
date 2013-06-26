#ifndef __AirHarp__HarpHUD__
#define __AirHarp__HarpHUD__

#include <iostream>
#include "HUD.h"
#include "Leap.h"

class HarpToolbar : public HUDView
                  , public HUDButton::Listener
                  , public ChangeBroadcaster

{
public:
    HarpToolbar();
    ~HarpToolbar();
    // HUDView overrides
    void setup();
    void draw();
    void loadTextures();
    
    void updateButtons();
    void setButtonTextures(TextureDescription on, TextureDescription off);
    
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
    GLBatch imageBatch;
    GLuint textureID;
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
    void setIndicatorTextures(TextureDescription on, TextureDescription off);
    
    // Leap overrides
    void onInit(const Leap::Controller& controller);
    void onConnect(const Leap::Controller& controller);
    void onDisconnect(const Leap::Controller& controller);
private:
    HUDButton indicator;
    GLBatch batch;
    GLBatch imageBatch;
    GLuint textureID;
};

class ChordRegion : public HUDView
{
public:
    ChordRegion();
    ~ChordRegion();
    // HUDView overrides
    void setup();
    void draw();
    void loadTextures();
    void setId(int inId) { id = inId; }
    int getId() const { return id; }
    void setActive(bool shouldBeActive);
private:
    GLBatch batch;
    GLBatch imageBatch;
    GLuint textureID;
    int id;
    bool isActive;
    float fade;
};

#endif /* defined(__AirHarp__HarpHUD__) */
