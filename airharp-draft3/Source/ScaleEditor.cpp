//
//  ScaleEditor.cpp
//  AirHarp
//
//  Created by Adam Somers on 7/14/13.
//
//

#include "ScaleEditor.h"

#include "SkinManager.h"
#include "Harp.h"
#include "GfxTools.h"
#include "Main.h"

ScaleEditor::ScaleEditor()
{
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.f };
    setDefaultColor(color);
    
    addChild(&textLabel);
    textLabel.setText("Custom Scale");

    String degrees[] = { "1", "m2", "2", "m3", "M3", "P4", "b5", "P5", "m6", "M6", "m7", "M7" };
    for (int i = 0; i < 12; ++i) {
        SharedPtr<TextHUDButton> b(new TextHUDButton);
        b->setId(i);
        b->setText(StringArray(degrees[i]), StringArray(degrees[i]));
        b->setBackgroundColor(Colour::fromFloatRGBA(1.f, 1.f, 1.f, .8f),
                              Colour::fromFloatRGBA(.5f, .5f, .5f, .5f));
        b->setTextColor(Colour::fromFloatRGBA(.2f, .2f, .2f, 1.f),
                        Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f));
        b->setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
        b->setUpperCase(false);
        scaleDegreeButtons.push_back(b);
        addChild(b.get());
        b->addListener(this);
    }
    
    String customScale = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("customScale", "1 M3 b5 P5 M7");
    AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("customScale", customScale);
    StringArray customScaleDegrees;
    customScaleDegrees.addTokens(customScale, false);
    for (SharedPtr<TextHUDButton> b : scaleDegreeButtons)
    {
        for (int i = 0; i < customScaleDegrees.size(); ++i)
        {
            if (b->getOnText()[0] == customScaleDegrees[i])
            {
                b->setState(true);
            }
        }
    }
}

void ScaleEditor::setBounds(const HUDRect &r)
{
    HUDView::setBounds(r);
    textLabel.setBounds(HUDRect(0, r.h - 30.f, r.w, 30.f));
    float buttonOffset = 10.f;
    float buttonWidth = (r.w - buttonOffset * 7.f) / 7.f;
    HUDRect buttonRect(0,0,buttonWidth,buttonWidth);
    float xInc = (buttonWidth + buttonOffset / 2.f) - buttonWidth / 2.f;
    float yInc = buttonWidth;
    for (int i = 0; i < scaleDegreeButtons.size(); ++i)
    {
        scaleDegreeButtons.at(i)->setBounds(buttonRect);
        buttonRect.x += xInc;
        buttonRect.y += yInc;
        yInc = -yInc;
        if (i == 4) {
            buttonRect.x += xInc;
            buttonRect.y += yInc;
            yInc = -yInc;
        }
    }
}

void ScaleEditor::buttonStateChanged(HUDButton *b)
{
    int count = 0;
    for (SharedPtr<TextHUDButton> b : scaleDegreeButtons)
        if (b->getState())
            count++;
    if (count == 0) {
        b->setState(true);
        return;
    }
    std::vector<std::string> scale;
    for (SharedPtr<TextHUDButton> b : scaleDegreeButtons)
        if (b->getState())
            scale.push_back(b->getOnText()[0].toStdString());
    HarpManager::instance().getHarp(0)->setCustomScale(scale);
    String setting;
    for (std::string s : scale)
        setting += s + " ";
    AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("customScale", setting);
}