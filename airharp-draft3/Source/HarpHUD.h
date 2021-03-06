#ifndef __AirHarp__HarpHUD__
#define __AirHarp__HarpHUD__

#include <iostream>
#include "HUD.h"
#include "Leap.h"
#include "TextButton.h"
#include "TextLabel.h"

class HarpToolbar : public HUDView
                  , public HUDButton::Listener
                  , public ChangeBroadcaster
                  , public ActionBroadcaster

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
    void updateButtonText();

    std::vector<TextHUDButton*> buttons;
    TextHUDButton settingsButton;
    TextHUDButton helpButton;
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
    void setBounds(const HUDRect& r);

    void setId(int inId);
    int getId() const { return id; }
private:
    HUDTextLabel label;

    int id;
    bool isActive;
};

#endif /* defined(__AirHarp__HarpHUD__) */
