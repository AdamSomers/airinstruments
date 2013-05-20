//
//  TempoControl.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/16/13.
//
//

#include "TempoControl.h"
#include "GfxTools.h"
#include "Main.h"
#include "Drums.h"

TempoControl::TempoControl()
{
    icon = SharedPtr<Icon>(new Icon);
    addChild(icon.get());
    
    addChild(&leftButton);
    addChild(&rightButton);
    leftButton.addListener(this);
    rightButton.addListener(this);

    leftButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("left"), SkinManager::instance().getSelectedSkin().getTexture("left"));
    rightButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("right"), SkinManager::instance().getSelectedSkin().getTexture("right"));
    
    
    leftButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    rightButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    
    leftButton.setTimeout(300);
    rightButton.setTimeout(300);
    
    leftButton.setButtonType(HUDButton::kMomentary);
    rightButton.setButtonType(HUDButton::kMomentary);
    
    GLfloat color[4] = {0.f, 0.f, 0.f, 0.f};
    setDefaultColor(color);
}

TempoControl::~TempoControl()
{
}

void TempoControl::draw()
{
    HUDView::draw();
}

void TempoControl::setBounds(const HUDRect &b)
{
    HUDView::setBounds(b);
    float buttonHeight = b.h * 0.6;
    TextureDescription td = SkinManager::instance().getSelectedSkin().getTexture("left");
    float aspectRatio = td.imageW / (float)td.imageH;
    float buttonWidth = aspectRatio * buttonHeight;
    icon->setBounds(HUDRect(0,0,b.w,b.h));
    leftButton.setBounds(HUDRect(0,
                                 b.h - buttonHeight - 5,
                                 buttonWidth,
                                 buttonHeight));
    rightButton.setBounds(HUDRect(b.w - buttonWidth,
                                  b.h - buttonHeight - 5,
                                  buttonWidth,
                                  buttonHeight));
}

void TempoControl::setTempo(float tempo)
{
    icon->setTempo(tempo);
}

void TempoControl::increment(int direction)
{
    icon->setTempo(icon->getTempo() + direction);   
    Drums::instance().setTempo(icon->getTempo());
}

void TempoControl::enableButtons(bool shouldBeEnabled)
{
    leftButton.setVisible(shouldBeEnabled);
    rightButton.setVisible(shouldBeEnabled);
}

void TempoControl::buttonStateChanged(HUDButton* b)
{
    if (b == &leftButton)
        increment(-1);
    if (b == &rightButton)
        increment(1);
}

TempoControl::Icon::Icon()
: tempoValue(-999)
, tempoValueChanged(true)
{
}

void TempoControl::Icon::draw()
{
    if (tempoValueChanged) // need to generate a new texture
    {
        float line1H = getBounds().h * 1.f;
        float line2H = getBounds().h * 0.4f;
        Image im(Image::PixelFormat::ARGB, (int) (getBounds().w*4), (int) (getBounds().h*4), true);
        Graphics g (im);
        g.setColour(Colour(80,80,80));
        g.setFont(Font(Environment::instance().getDefaultFont(), (line1H * .75f) * 4.0f, Font::plain));
        g.drawText(String((int)tempoValue), 0, -20, (int) (getBounds().w*4), (int) (line1H*4), Justification::centred, true);
        g.setColour(Colour(128,128,128));
        g.setFont(Font(Environment::instance().getDefaultFont(), (line2H * .75f) * 4.0f, Font::plain));
        g.drawText("BPM", 0, (line1H * .75f) * 4.0f - 20, (int) (getBounds().w*4), (int) (line2H*4), Justification::centred, true);

        if (textureDesc.textureId != 0)
            glDeleteTextures(1, &textureDesc.textureId);
        textureDesc = GfxTools::loadTextureFromJuceImage(im);
        setDefaultTexture(textureDesc);
        tempoValueChanged = false;
    }
    
    if (getBounds().x + getBounds().w * .33 > 0 && getBounds().x + getBounds().w * .66 < getParent()->getBounds().w)
        HUDView::draw();
}

void TempoControl::Icon::setTempo(float tempo)
{
    tempoValue = tempo;
    tempoValueChanged = true;
}
