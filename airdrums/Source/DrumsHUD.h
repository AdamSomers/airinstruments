#ifndef h_DrumsHUD
#define h_DrumsHUD

#include <iostream>
#include "HUD.h"
#include "Leap.h"
#include "BeatCountView.h"

class DrumsToolbar : public HUDView
                   , public ChangeListener
                   , public HUDButton::Listener
                   , public Timer
{
public:
    DrumsToolbar();
    ~DrumsToolbar();

    // HUDView overrides
    void setup();
    void draw();
    void setBounds(const HUDRect& b);

    // ChangeListener overrides
    void changeListenerCallback(ChangeBroadcaster* source);

    // HUDButton::Listener overrides
    void buttonStateChanged(HUDButton* b);

    // Timer overrides
    void timerCallback();

    void layoutControls();

private:
    HUDButton resetButton;
    HUDButton playButton;
    HUDButton recordButton;
    HUDButton metronomeButton;
    BeatCountView beatCountView;
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
    
    void setCursorMode(bool isPlaying);
    
    // Leap overrides
    void onInit(const Leap::Controller& controller);
    void onConnect(const Leap::Controller& controller);
    void onDisconnect(const Leap::Controller& controller);
private:
    HUDButton indicator;
    HUDView cursorModeView;
};

#endif
