//
//  TempoControl.h
//  AirBeats
//
//  Created by Adam Somers on 4/16/13.
//
//

#ifndef __AirBeats__TempoControl__
#define __AirBeats__TempoControl__

#include "HUD.h"
#include <deque>

class TempoControl : public HUDView
                   , public MultiTimer
                   , public HUDButton::Listener
{
public:
    TempoControl();
    ~TempoControl();
    
    // HUDView overrides
    void draw();
    void setBounds(const HUDRect& b);
    void fingerMotion(float x, float y, FingerView* fv);
    void fingerEntered(float x, float y, FingerView* fv);
    void fingerExited(float x, float y, FingerView* fv);
    void cursorMoved(float x, float y);
    void cursorEntered(float x, float y);
    void cursorExited(float x, float y);
    
    // Juce::Timer override
    void timerCallback(int timerId);

    enum TimerId
    {
        kTimerSelectionDelay,
        kTimerTrackedFingerMissing
    };

    // HUDButton::Listener overrides
    void buttonStateChanged(HUDButton* b);
    
    void setTempo(float tempo);
    void increment(int direction);
    void enableButtons(bool shouldBeEnabled);
    
    class Icon : public HUDView
    {
    public:
        Icon();
        //~Icon();
        void draw();
        void setBounds(const HUDRect& b);
        
        void setBoundsImmediately(const HUDRect& b);
        
        void setTempo(float tempo);
        float getTempo() const { return tempoValue; }
    private:
        void updateBounds();
        HUDRect targetBounds;
        HUDRect tempBounds;
        float xStep, yStep, wStep, hStep;
        
        GLuint textureId;
        float tempoValue;
        bool tempoValueChanged;
    };
    
private:
    void layoutIcons();
    std::deque<Icon*> icons;
    bool needsLayout;
    
    float initialFingerX;
    float initialFingerY;
    FingerView* trackedFinger;
    
    HUDButton leftButton;
    HUDButton rightButton;
};

#endif /* defined(__AirBeats__TempoControl__) */
