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
    
    addChild(&keyLabel);
    keyLabel.setText("Key");
    
    addChild(&modeLabel);
    modeLabel.setText("Mode");

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
    
    float maxWidth = 500.f;
    
    float maxHeight = jmax(530.f, Environment::instance().screenH - 150.f);
    
    float buttonWidth = 70.f;
    float buttonHeight = buttonWidth;
    HUDRect buttonRect(b.w / 2.f - buttonWidth / 2.f, 0, buttonWidth, buttonWidth);

    float labelHeight = 30.f;
    float initialY = jmin(b.h / 2.f + maxHeight / 2.f, b.h);
    float yOffset = jmax(30.f, maxHeight * .055f);
    keyLabel.setBounds(HUDRect(b.w / 2.f - maxWidth / 2.f,
                               initialY - labelHeight - 10,
                               maxWidth,
                               labelHeight));

    float keySelectorHeight = 80;
    float keySelectorWidth = 400;
    keySelector.setBounds(HUDRect(b.w / 2.f - keySelectorWidth / 2.f,
                                  keyLabel.getBounds().y - keySelectorHeight - 10,
                                  keySelectorWidth,
                                  keySelectorHeight));
    
    modeLabel.setBounds(HUDRect(b.w / 2.f - maxWidth / 2.f,
                                keySelector.getBounds().y - yOffset - 10.f,
                                maxWidth,
                                30.f));
    
    buttonRect.x = b.w / 2.f - buttonWidth - 20.f;
    buttonRect.y = modeLabel.getBounds().y - buttonHeight - 10.f;
    scalesButton.setBounds(buttonRect);
    buttonRect.x = b.w / 2.f + 20.f;
    chordsButton.setBounds(buttonRect);
    float scaleEditorWidth = maxWidth;
    float scaleEditorHeight = 160.f;
    scaleEditor.setBounds(HUDRect(b.w / 2.f - scaleEditorWidth / 2.f,
                                  buttonRect.y - scaleEditorHeight- yOffset,
                                  scaleEditorWidth,
                                  scaleEditorHeight));
    
    playButton.setBounds(HUDRect(b.w / 2.f - buttonWidth / 2.f,
                                 scaleEditor.getBounds().y - buttonHeight - yOffset,
                                 buttonWidth,
                                 buttonHeight));
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