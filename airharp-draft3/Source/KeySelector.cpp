//
//  KeySelector.cpp
//  AirHarp
//
//  Created by Adam Somers on 7/14/13.
//
//

#include "KeySelector.h"


#include "KeySelector.h"
#include "GfxTools.h"
#include "Main.h"

KeySelector::KeySelector()
{
    icon = SharedPtr<Icon>(new Icon);
    icon->setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    addChild(icon.get());
    
    addChild(&leftButton);
    addChild(&rightButton);
    leftButton.addListener(this);
    rightButton.addListener(this);
    
    leftButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("arrow_down_white"), SkinManager::instance().getSelectedSkin().getTexture("arrow_down_white"));
    rightButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("arrow_up_white"), SkinManager::instance().getSelectedSkin().getTexture("arrow_up_white"));
    
    
    leftButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    rightButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    
    leftButton.setButtonType(HUDButton::kMomentary);
    rightButton.setButtonType(HUDButton::kMomentary);
    
    addChild(&flatButton);
    addChild(&naturalButton);
    addChild(&sharpButton);
    flatButton.addListener(this);
    naturalButton.addListener(this);
    sharpButton.addListener(this);
    flatButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("flat_on"),
                           SkinManager::instance().getSelectedSkin().getTexture("flat_off"));
    naturalButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("natural_on"),
                           SkinManager::instance().getSelectedSkin().getTexture("natural_off"));
    sharpButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("sharp_on"),
                           SkinManager::instance().getSelectedSkin().getTexture("sharp_off"));
    flatButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    naturalButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    sharpButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));

    int keyIndex = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("keyIndex", 2);
    int accidental = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("accidental", 0);
    setKey(keyIndex, accidental);
    
    GLfloat color[4] = {0.f, 0.f, 0.f, 0.f};
    setDefaultColor(color);
}

KeySelector::~KeySelector()
{
}

void KeySelector::draw()
{
    HUDView::draw();
}

void KeySelector::setBounds(const HUDRect &b)
{
    HUDView::setBounds(b);

    HUDRect keyIndexRect = b;
    keyIndexRect.w = b.w / 2.f;
    HUDRect accidentalRect = b;
    accidentalRect.w = b.w / 2.f;
    accidentalRect.x = b.w / 2.f;

    float buttonHeight = keyIndexRect.h * 0.6f;
    TextureDescription td = SkinManager::instance().getSelectedSkin().getTexture("arrow_down_white");
    float aspectRatio = td.imageW / (float)td.imageH;
    float buttonWidth = aspectRatio * buttonHeight;
    icon->setBounds(HUDRect(0,0,keyIndexRect.w,keyIndexRect.h));
    leftButton.setBounds(HUDRect(0,
                                 keyIndexRect.h - buttonHeight - 5,
                                 buttonWidth,
                                 buttonHeight));
    rightButton.setBounds(HUDRect(keyIndexRect.w - buttonWidth,
                                  keyIndexRect.h - buttonHeight - 5,
                                  buttonWidth,
                                  buttonHeight));
    float buttonOffset = 5;
    buttonWidth = (accidentalRect.w / 3.f) - buttonOffset * 3.f;
    buttonHeight = buttonWidth;
    HUDRect buttonRect(accidentalRect.x,
                       keyIndexRect.h - buttonHeight - 5,
                       buttonWidth,
                       buttonHeight);
    buttonRect.x += buttonOffset + 10;
    flatButton.setBounds(buttonRect);
    buttonRect.x += buttonWidth + buttonOffset;
    naturalButton.setBounds(buttonRect);
    buttonRect.x += buttonWidth + buttonOffset;
    sharpButton.setBounds(buttonRect);
}

void KeySelector::setKey(int keyIndex, int accidental)
{
    icon->setKey(keyIndex, accidental);
    flatButton.setState(accidental == -1);
    naturalButton.setState(accidental == 0);
    sharpButton.setState(accidental == 1);
    AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("keyIndex", keyIndex);
    AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("accidental", accidental);
}

void KeySelector::increment(int direction)
{
    int next = icon->getKeyIndex() + direction;
    if (next < 0) next = 6;
    if (next > 6) next = 0;
    setKey(next, icon->getAccidental());
}

void KeySelector::enableButtons(bool shouldBeEnabled)
{
    leftButton.setVisible(shouldBeEnabled);
    rightButton.setVisible(shouldBeEnabled);
}

void KeySelector::buttonStateChanged(HUDButton* b)
{
    if (b == &leftButton)
        increment(-1);
    else if (b == &rightButton)
        increment(1);
    else if (b == &flatButton)
        setKey(icon->getKeyIndex(), -1);
    else if (b == &naturalButton)
        setKey(icon->getKeyIndex(), 0);
    else if (b == &sharpButton)
        setKey(icon->getKeyIndex(), 1);
}

KeySelector::Icon::Icon()
: keyIndex(0)
, keyChanged(true)
{
    keyStrings.add("A");
    keyStrings.add("B");
    keyStrings.add("C");
    keyStrings.add("D");
    keyStrings.add("E");
    keyStrings.add("F");
    keyStrings.add("G");
}

void KeySelector::Icon::draw()
{
    if (keyChanged) // need to generate a new texture
    {
        float line1H = getBounds().h * 1.f;
        float line2H = getBounds().h * 0.4f;
        Image im(Image::PixelFormat::ARGB, (int) (getBounds().w*4), (int) (getBounds().h*4), true);
        Graphics g (im);
        g.setColour(Colour(200,200,200));
        g.setFont(Font(Environment::instance().getDefaultFont(), (line1H * .75f) * 4.0f, Font::plain));
        g.drawText(String(keyStrings[keyIndex]), 0, -30, (int) (getBounds().w*4), (int) (line1H*4), Justification::centred, true);
        g.setColour(Colour(128,128,128));
        g.setFont(Font(Environment::instance().getDefaultFont(), (line2H * .75f) * 4.0f, Font::plain));
        g.drawText("KEY", 0, (int) ((line1H * .75f) * 4.0f - 20), (int) (getBounds().w*4), (int) (line2H*4), Justification::centred, true);
        
        if (textureDesc.textureId != 0)
            glDeleteTextures(1, &textureDesc.textureId);
        textureDesc = GfxTools::loadTextureFromJuceImage(im);
        setDefaultTexture(textureDesc);
        keyChanged = false;
    }
    
    if (getBounds().x + getBounds().w * .33 > 0 && getBounds().x + getBounds().w * .66 < getParent()->getBounds().w)
        HUDView::draw();
}

void KeySelector::Icon::setKey(int newKeyIndex, int newAccidental)
{
    keyIndex = newKeyIndex;
    accidental = newAccidental;
    keyChanged = true;
}
