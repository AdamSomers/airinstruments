//
//  SettingsScreen.h
//  AirHarp
//
//  Created by Adam Somers on 7/2/13.
//
//

#ifndef __AirHarp__SettingsScreen__
#define __AirHarp__SettingsScreen__

#include <iostream>

#include "HUD.h"
#include "TextButton.h"
#include "KeySelector.h"
#include "ScaleEditor.h"
#include "TextLabel.h"

class SettingsScreen : public HUDView
                     , public HUDButton::Listener
                     , public ActionBroadcaster
{
public:
    SettingsScreen();
    ~SettingsScreen();
    
    // HUDView overrides
    void setBounds(const HUDRect& r);
    
    void buttonStateChanged(HUDButton* b);
    
    ScaleEditor* getScaleEditor() { return &scaleEditor; }

private:
    TextHUDButton playButton;
    KeySelector keySelector;
    TextHUDButton chordsButton;
    TextHUDButton scalesButton;
    ScaleEditor scaleEditor;
    HUDTextLabel keyLabel;
    HUDTextLabel modeLabel;
};

#endif /* defined(__AirHarp__SettingsScreen__) */
