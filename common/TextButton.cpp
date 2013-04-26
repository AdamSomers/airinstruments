//
//  TextButton.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/25/13.
//
//

#include "TextButton.h"
#include "GfxTools.h"

TextHUDButton::TextHUDButton(StringArray on /*= "on"*/, StringArray off /*= "off"*/)
: onText(on)
, offText(off)
, textChanged(false)
, onTexture(0)
, offTexture(0)
{
    
}

void TextHUDButton::draw()
{
    if (textChanged)
    {
        loadTextures();
        textChanged = false;
    }
    
    HUDButton::draw();
}

void TextHUDButton::loadTextures()
{
    HUDButton::loadTextures();
    
    int imageW = 256;
    int imageH = 256;
    Image imOn(Image::PixelFormat::ARGB, imageW, imageH, true);
    Graphics gOn (imOn);
    
    gOn.setColour(Colour(0.f, 0.f, 0.f, .3f));
    gOn.fillEllipse(0, 0, imageW, imageH);
    
    gOn.setColour(Colours::white);
    float textSize = jmin(imageH, imageW) * .22;
    gOn.setFont(textSize);

    float lineHeight = textSize;
    float y = imageH / 2.f  - (lineHeight * ceilf(onText.size() / 2.f));
    if (onText.size() % 2 > 0)
        y += lineHeight / 2.f;
    for (int i = 0; i < onText.size(); ++i)
    {
        gOn.drawText(onText[i], 0, y, imageW, lineHeight, Justification::centred, false);
        y += lineHeight;
    }
    
    if (onTexture != 0)
        glDeleteTextures(1, &onTexture);
    glGenTextures(1, &onTexture);
    glBindTexture(GL_TEXTURE_2D, onTexture);
    GfxTools::loadTextureFromJuceImage(imOn);
    
    Image imOff(Image::PixelFormat::ARGB, imageW, imageH, true);
    Graphics gOff (imOff);
    
    gOff.setColour(Colour(0.f, 0.f, 0.f, .3f));
    gOff.fillEllipse(0, 0, imageW, imageH);
    
    gOff.setColour(Colours::white);
    gOff.setFont(textSize);

    lineHeight = textSize;
    y = imageH / 2.f  - (lineHeight * ceilf(offText.size() / 2.f));
    if (offText.size() % 2 > 0)
        y += lineHeight / 2.f;
    for (int i = 0; i < offText.size(); ++i)
    {
        gOff.drawText(offText[i], 0, y, imageW, lineHeight, Justification::centred, false);
        y += lineHeight;
    }
    
    if (offTexture != 0)
        glDeleteTextures(1, &offTexture);
    glGenTextures(1, &offTexture);
    glBindTexture(GL_TEXTURE_2D, offTexture);
    GfxTools::loadTextureFromJuceImage(imOff);
    
    setTextures(onTexture, offTexture);
}

void TextHUDButton::setText(StringArray on, StringArray off)
{
    onText = on;
    offText = off;
    textChanged = true;
}