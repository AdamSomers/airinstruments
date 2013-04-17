//
//  WheelSelector.h
//  AirBeats
//
//  Created by Adam Somers on 4/9/13.
//
//

#ifndef __AirBeats__WheelSelector__
#define __AirBeats__WheelSelector__

#include "HUD.h"

#include <iostream>

class WheelSelector : public HUDView
                    , public MultiTimer
{
public:
    WheelSelector(bool left = false);
    ~WheelSelector();
    
    // HUDView overrides
    void setBounds(const HUDRect& b);
    void draw();
    void fingerMotion(float x, float y, FingerView* fv);
    void fingerEntered(float x, float y, FingerView* fv);
    void fingerExited(float x, float y, FingerView* fv);
    
    // Juce::Timer override
    void timerCallback(int timerId);
    
    enum TimerId
    {
        kTimerSelectionDelay = 0,
        kTimerShowControl,
        kTimerIdle
    };
    
    void setSelection(int sel);
    int getSelection() const { return selection; }
    void incSelection(int direction);
    
    void setEnabled(bool shouldBeEnabled) { enabled = shouldBeEnabled; }
    bool isEnabled() const { return enabled; }
    
    class Icon : public HUDView
    {
    public:
        Icon(int id, bool left = false);
        ~Icon();
        void draw();
        void setBounds(const HUDRect& b);
        void setRotationCoefficient(float rotation);
        void rotate(float rotation);
        void setImage(const Image& im);
        const Image& getImage() const;
    private:
        void updateBounds();
        int id;
        float targetRotationCoeff;
        float rotationCoeff;
        float rotationInc;
        HUDRect targetBounds;
        HUDRect tempBounds;
        float xStep, yStep, wStep, hStep;
        Image image;
        bool leftHanded;
    };
    
    void addIcon(Icon* icon);
    void removeAllIcons();
    
    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void wheelSelectorChanged(WheelSelector* selector) = 0;
    };
    
    void addListener(Listener* listener);
    void removeListener(Listener* listener);
    
private:
    void updateBounds();
    HUDRect targetBounds;
    HUDRect tempBounds;
    float xStep;
    float yStep;
    float wStep;
    float hStep;
    bool enabled;
    bool leftHanded;
    
    void layoutIcons();
    std::vector<Icon*> icons;
    int selection;
    bool needsLayout;
    FingerView* trackedFinger;
    std::vector<Listener*> listeners;
};

#endif /* defined(__AirBeats__WheelSelector__) */
