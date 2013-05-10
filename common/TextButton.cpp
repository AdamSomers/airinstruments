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
    
    String fontName = Environment::instance().getDefaultFont();
    
    int imageW = 256;
    int imageH = 256;
    Image imOn(Image::PixelFormat::ARGB, imageW, imageH, true);
    Graphics gOn (imOn);
    
    gOn.setColour(Colour::fromFloatRGBA(1.f, 1.f, 1.f, .25f));
    gOn.fillEllipse(0.f, 0.f, (float)imageW, (float)imageH);
    
    gOn.setColour(Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f));
    float textSize = jmin(imageH, imageW) * .22f;
    gOn.setFont(textSize);
    gOn.setFont(Font(fontName, textSize, Font::plain));

    float lineHeight = textSize;
    float y = imageH / 2.f  - (lineHeight * ceilf(onText.size() / 2.f));
    if (onText.size() % 2 > 0)
        y += lineHeight / 2.f;
    for (int i = 0; i < onText.size(); ++i)
    {
        gOn.drawText(onText[i].toUpperCase(), 0, (int)y, imageW, (int)lineHeight, Justification::centred, false);
        y += lineHeight;
    }
    
    if (onTextureDesc.textureId != 0)
        glDeleteTextures(1, &onTextureDesc.textureId);
    onTextureDesc = GfxTools::loadTextureFromJuceImage(imOn);
    
    Image imOff(Image::PixelFormat::ARGB, imageW, imageH, true);
    Graphics gOff (imOff);
    
    gOff.setColour(Colour::fromFloatRGBA(1.f, 1.f, 1.f, .25f));
    gOff.fillEllipse(0.f, 0.f, (float)imageW, (float)imageH);
    
    gOff.setColour(Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f));
    gOff.setFont(textSize);
    gOff.setFont(Font(fontName, textSize, Font::plain));

    lineHeight = textSize;
    y = imageH / 2.f  - (lineHeight * ceilf(offText.size() / 2.f));
    if (offText.size() % 2 > 0)
        y += lineHeight / 2.f;
    for (int i = 0; i < offText.size(); ++i)
    {
        gOff.drawText(offText[i].toUpperCase(), 0, (int)y, imageW, (int)lineHeight, Justification::centred, false);
        y += lineHeight;
    }
    
    if (offTextureDesc.textureId != 0)
        glDeleteTextures(1, &offTextureDesc.textureId);
    offTextureDesc = GfxTools::loadTextureFromJuceImage(imOff);
    
    setTextures(onTextureDesc, offTextureDesc);
}

void TextHUDButton::setText(StringArray on, StringArray off)
{
    onText = on;
    offText = off;
    textChanged = true;
}