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
, onTextColor(Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f))
, offTextColor(Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f))
, onBackgroundColor(Colour::fromFloatRGBA(.3f, .3f, .3f, .5f))
, offBackgroundColor(Colour::fromFloatRGBA(.3f, .3f, .3f, .5f))
, uppercase(true)
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
    
    gOn.setColour(onBackgroundColor);
    gOn.fillEllipse(0.f, 0.f, (float)imageW, (float)imageH);
    
    gOn.setColour(onTextColor);
    float textSize = jmin(imageH, imageW) * .22f;
    gOn.setFont(textSize);
    gOn.setFont(Font(fontName, textSize, Font::plain));

    float lineHeight = textSize;
    float y = imageH / 2.f  - (lineHeight * ceilf(onText.size() / 2.f));
    if (onText.size() % 2 > 0)
        y += lineHeight / 2.f;
    for (int i = 0; i < onText.size(); ++i)
    {
        String text = onText[i];
        if (uppercase)
            text = text.toUpperCase();
        gOn.drawText(text, 0, (int)y, imageW, (int)lineHeight, Justification::centred, false);
        y += lineHeight;
    }
    
    if (onTextureDesc.textureId != 0)
        glDeleteTextures(1, &onTextureDesc.textureId);
    onTextureDesc = GfxTools::loadTextureFromJuceImage(imOn);
    
    Image imOff(Image::PixelFormat::ARGB, imageW, imageH, true);
    Graphics gOff (imOff);
    
    gOff.setColour(offBackgroundColor);
    gOff.fillEllipse(0.f, 0.f, (float)imageW, (float)imageH);
    
    gOff.setColour(offTextColor);
    gOff.setFont(textSize);
    gOff.setFont(Font(fontName, textSize, Font::plain));

    lineHeight = textSize;
    y = imageH / 2.f  - (lineHeight * ceilf(offText.size() / 2.f));
    if (offText.size() % 2 > 0)
        y += lineHeight / 2.f;
    for (int i = 0; i < offText.size(); ++i)
    {
        String text = offText[i];
        if (uppercase)
            text = text.toUpperCase();
        gOff.drawText(text, 0, (int)y, imageW, (int)lineHeight, Justification::centred, false);
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

void TextHUDButton::setTextColor(const Colour& onColor, const Colour& offColor)
{
    onTextColor = onColor;
    offTextColor = offColor;
    textChanged = true;
}

void TextHUDButton::setBackgroundColor(const Colour& onColor, const Colour& offColor)
{
    onBackgroundColor = onColor;
    offBackgroundColor = offColor;
    textChanged = true;
}