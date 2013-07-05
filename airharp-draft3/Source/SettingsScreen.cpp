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
    playButton.setText(StringArray("Play!"), StringArray("Play!"));
    playButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    playButton.addListener(this);
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.5f };
    setDefaultColor(color);
}

SettingsScreen::~SettingsScreen()
{
}

void SettingsScreen::setBounds(const HUDRect& b)
{
    HUDView::setBounds(b);
    playButton.setBounds(HUDRect(100,0,100,100));
}

void SettingsScreen::buttonStateChanged(HUDButton* b)
{
    if (b == &playButton)
    {
        sendActionMessage("playMode");
    }
}