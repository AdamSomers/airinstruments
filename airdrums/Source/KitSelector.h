//
//  KitSelector.h
//  AirBeats
//
//  Created by Adam Somers on 4/9/13.
//
//

#ifndef __AirBeats__KitSelector__
#define __AirBeats__KitSelector__

#include "HUD.h"

#include <iostream>

class KitSelector : public HUDView
                  , public Timer
{
public:
    KitSelector();
    ~KitSelector();
    
    // HUDView overrides
    void setBounds(const HUDRect& b);
    void draw();
    void fingerMotion(float x, float y, FingerView* fv);
    void fingerEntered(float x, float y, FingerView* fv);
    void fingerExited(float x, float y, FingerView* fv);
    
    // Juce::Timer override
    void timerCallback();
    
    void setSelection(int sel);
    int getSelection() const { return selection; }
    void incSelection(int direction);
    
    void setEnabled(bool shouldBeEnabled) { enabled = shouldBeEnabled; }
    
    class Icon : public HUDView
    {
    public:
        Icon(int id);
        ~Icon();
        void draw();
        void setBounds(const HUDRect& b);
        void setRotationCoefficient(float rotation);
        void rotate(float rotation);
    private:
        void updateBounds();
        int id;
        float targetRotationCoeff;
        float rotationCoeff;
        float rotationInc;
        HUDRect targetBounds;
        HUDRect tempBounds;
        float xStep, yStep, wStep, hStep;
    };
    
    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void kitSelectorChanged(KitSelector* selector) = 0;
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
    
    void layoutIcons();
    std::vector<Icon*> icons;
    int selection;
    bool needsLayout;
    float prevFingerX;
    float prevFingerY;
    FingerView* trackedFinger;
    std::vector<Listener*> listeners;
};

#endif /* defined(__AirBeats__KitSelector__) */
