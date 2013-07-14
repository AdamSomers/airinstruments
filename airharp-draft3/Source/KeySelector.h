//
//  KeySelector.h
//  AirHarp
//
//  Created by Adam Somers on 7/14/13.
//
//

#ifndef __AirHarp__KeySelector__
#define __AirHarp__KeySelector__


#include "HUD.h"
#include "Types.h"

class KeySelector : public HUDView
                  , public HUDButton::Listener
{
public:
    KeySelector();
    ~KeySelector();
    
    // HUDView overrides
    void draw();
    void setBounds(const HUDRect& b);
    
    // HUDButton::Listener overrides
    void buttonStateChanged(HUDButton* b);
    
    void setKey(int keyIndex, int accidental = 0);
    void increment(int direction);
    void enableButtons(bool shouldBeEnabled);
    
    class Icon : public HUDView
    {
    public:
        Icon();
        //~Icon();
        void draw();
        
        void setKey(int key, int accidental = 0);
        int getKeyIndex() const { return keyIndex; }
        int getAccidental() const { return accidental; }

    private:
        TextureDescription textureDesc;
        int keyIndex;
        int accidental;
        bool keyChanged;
        Array<String> keyStrings;
    };
    
private:
    SharedPtr<Icon> icon;
    HUDButton leftButton;
    HUDButton rightButton;
    HUDButton flatButton;
    HUDButton naturalButton;
    HUDButton sharpButton;
};
#endif /* defined(__AirHarp__KeySelector__) */
