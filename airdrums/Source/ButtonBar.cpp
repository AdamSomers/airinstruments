//
//  ButtonBar.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/25/13.
//
//

#include "ButtonBar.h"
#include "Drums.h"
#include "SkinManager.h"
#include "Main.h"

ButtonBar::ButtonBar()
{
    StringArray text;
    text.add("Clear");
    text.add("Pattern");
    clearPatternButton.setText(text, text);
    clearPatternButton.addListener(this);
    addChild(&clearPatternButton);
    clearPatternButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    
    text.clear();
    text.add("Clear");
    text.add("Track");
    clearLeftTrackButton.setText(text, text);
    clearLeftTrackButton.addListener(this);
    addChild(&clearLeftTrackButton);
    clearLeftTrackButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    
    text.clear();
    text.add("Clear");
    text.add("Track");
    clearRightTrackButton.setText(text, text);
    clearRightTrackButton.addListener(this);
    addChild(&clearRightTrackButton);
    clearRightTrackButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
}

void ButtonBar::setBounds(const HUDRect &b)
{
    HUDView::setBounds(b);
    layoutButtons();
}

void ButtonBar::layoutButtons()
{
    
    float buttonWidth = 75.f;
    clearPatternButton.setBounds(HUDRect(getBounds().w / 2.f - buttonWidth / 2,
                                         0,
                                         buttonWidth,
                                         buttonWidth));
    clearLeftTrackButton.setBounds(HUDRect(10,
                                         0,
                                         buttonWidth,
                                         buttonWidth));
    clearRightTrackButton.setBounds(HUDRect(getBounds().w - buttonWidth - 10,
                                         0,
                                         buttonWidth,
                                         buttonWidth));
}

void ButtonBar::buttonStateChanged(HUDButton* b)
{
    if (b == &clearPatternButton)
    {
        Drums::instance().clear();
    }
    else if (b == &clearLeftTrackButton)
    {
        int leftSelection = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("selectedNoteLeft", -1);
        if (leftSelection != -1)
            Drums::instance().clearTrack(leftSelection);
    }
    else if (b == &clearRightTrackButton)
    {
        int rightSelection = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("selectedNoteRight", -1);
        if (rightSelection != -1)
            Drums::instance().clearTrack(rightSelection);
    }
}

void ButtonBar::showButtons(bool shouldShow)
{
    clearPatternButton.setVisible(shouldShow);
    clearLeftTrackButton.setVisible(shouldShow);
    clearRightTrackButton.setVisible(shouldShow);
}
