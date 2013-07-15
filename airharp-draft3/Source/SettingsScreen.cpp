//
//  SettingsScreen.cpp
//  AirHarp
//
//  Created by Adam Somers on 7/2/13.
//
//

#include "SettingsScreen.h"
#include "SkinManager.h"
#include "Main.h"

SettingsScreen::SettingsScreen()
{
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.5f };
    setDefaultColor(color);

    addChild(&playButton);
    playButton.setText(StringArray("Done"), StringArray("Done"));
    playButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    playButton.addListener(this);

    addChild(&scalesButton);
    scalesButton.setText(StringArray("Scales"), StringArray("Scales"));
    scalesButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    scalesButton.addListener(this);
    scalesButton.setBackgroundColor(Colour::fromFloatRGBA(1.f, 1.f, 1.f, .8f),
                          Colour::fromFloatRGBA(.5f, .5f, .5f, .5f));
    scalesButton.setTextColor(Colour::fromFloatRGBA(.2f, .2f, .2f, 1.f),
                    Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f));
    
    addChild(&chordsButton);
    chordsButton.setText(StringArray("Chords"), StringArray("Chords"));
    chordsButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    chordsButton.addListener(this);
    chordsButton.setBackgroundColor(Colour::fromFloatRGBA(1.f, 1.f, 1.f, .8f),
                                    Colour::fromFloatRGBA(.5f, .5f, .5f, .5f));
    chordsButton.setTextColor(Colour::fromFloatRGBA(.2f, .2f, .2f, 1.f),
                              Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f));
    
    bool chordMode = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getBoolValue("chordMode", false);
    chordsButton.setState(chordMode);
    scalesButton.setState(!chordMode);

    addChild(&keySelector);
    addChild(&scaleEditor);
}

SettingsScreen::~SettingsScreen()
{
}

void SettingsScreen::setBounds(const HUDRect& b)
{
    HUDView::setBounds(b);
    float buttonWidth = 100.f;
    HUDRect buttonRect(b.w / 2.f - buttonWidth / 2.f, buttonWidth / 2.f, buttonWidth, buttonWidth);
    playButton.setBounds(buttonRect);
    float keySelectorHeight = 80;
    float keySelectorWidth = 400;
    keySelector.setBounds(HUDRect(b.w / 2.f - keySelectorWidth / 2.f,
                                  b.h - keySelectorHeight - 50,
                                  keySelectorWidth,
                                  keySelectorHeight));
    buttonRect.x = b.w / 2.f - buttonWidth - 20.f;
    buttonRect.y = keySelector.getBounds().y - 120;
    scalesButton.setBounds(buttonRect);
    buttonRect.x = b.w / 2.f + 20.f;
    chordsButton.setBounds(buttonRect);
    float scaleEditorWidth = 500.f;
    float scaleEditorHeight = 160.f;
    scaleEditor.setBounds(HUDRect(b.w / 2.f - scaleEditorWidth / 2.f,
                                  buttonRect.y - scaleEditorHeight- 20,
                                  scaleEditorWidth,
                                  scaleEditorHeight));
}

void SettingsScreen::buttonStateChanged(HUDButton* b)
{
    if (b == &playButton)
    {
        sendActionMessage("playMode");
    }
    else if (b == &scalesButton)
    {
        scalesButton.setState(true);
        chordsButton.setState(false);
        sendActionMessage("scaleMode");
    }
    else if (b == &chordsButton)
    {
        scalesButton.setState(false);
        chordsButton.setState(true);
        sendActionMessage("chordMode");
    }
}