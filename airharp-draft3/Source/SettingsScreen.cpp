//
//  SettingsScreen.cpp
//  AirHarp
//
//  Created by Adam Somers on 7/2/13.
//
//

#include "SettingsScreen.h"
#include "SkinManager.h"

SettingsScreen::SettingsScreen()
{
    addChild(&playButton);
    playButton.setText(StringArray("Done"), StringArray("Done"));
    playButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    playButton.addListener(this);
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.5f };
    setDefaultColor(color);
    
    addChild(&keySelector);
}

SettingsScreen::~SettingsScreen()
{
}

void SettingsScreen::setBounds(const HUDRect& b)
{
    HUDView::setBounds(b);
    playButton.setBounds(HUDRect(b.w / 2.f - 50,50,100,100));
    float keySelectorHeight = 80;
    float keySelectorWidth = 400;
    keySelector.setBounds(HUDRect(b.w / 2.f - keySelectorWidth / 2.f,
                                  b.h - keySelectorHeight - 50,
                                  keySelectorWidth,
                                  keySelectorHeight));
}

void SettingsScreen::buttonStateChanged(HUDButton* b)
{
    if (b == &playButton)
    {
        sendActionMessage("playMode");
    }
}