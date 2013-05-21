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
#include "Types.h"

class TempoControl : public HUDView
                   , public HUDButton::Listener
{
public:
    TempoControl();
    ~TempoControl();
    
    // HUDView overrides
    void draw();
    void setBounds(const HUDRect& b);

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

        void setTempo(float tempo);
        float getTempo() const { return tempoValue; }
    private:        
        TextureDescription textureDesc;
        float tempoValue;
        bool tempoValueChanged;
    };
    
private:
    SharedPtr<Icon> icon;    
    HUDButton leftButton;
    HUDButton rightButton;
};

#endif /* defined(__AirBeats__TempoControl__) */
