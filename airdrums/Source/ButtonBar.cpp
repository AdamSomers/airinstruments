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
    
    StringArray offText;
    offText.add("Clear");
    offText.add("Track");
    text.clear();
    text.add("Cancel");
    text.add("Clear");
    clearTrackButton.setText(text, offText);
    clearTrackButton.addListener(this);
    addChild(&clearTrackButton);
    clearTrackButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
}

void ButtonBar::setBounds(const HUDRect &b)
{
    HUDView::setBounds(b);
    layoutButtons();
}

void ButtonBar::layoutButtons()
{
    
    float buttonWidth = 75.f;
    clearPatternButton.setBounds(HUDRect(getBounds().w - buttonWidth - 10,
                                         0,
                                         buttonWidth,
                                         buttonWidth));

    clearTrackButton.setBounds(HUDRect(10,
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
    else if (b == &clearTrackButton)
    {
        

        sendActionMessage(b->getState() ? "clearTrack" : "cancelClear");
    }
}

void ButtonBar::showButtons(bool shouldShow)
{
    clearPatternButton.setVisible(shouldShow);
    clearTrackButton.setVisible(shouldShow);
}

void ButtonBar::resetClearButton()
{
    clearTrackButton.setState(false);
}
