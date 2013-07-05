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

private:
    TextHUDButton playButton;
};

#endif /* defined(__AirHarp__SettingsScreen__) */
