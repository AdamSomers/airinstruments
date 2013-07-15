//
//  TextLabel.cpp
//  AirHarp
//
//  Created by Adam Somers on 7/14/13.
//
//

#include "TextLabel.h"

HUDTextLabel::HUDTextLabel()
{
    setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void HUDTextLabel::setBounds(const HUDRect& r)
{
    HUDView::setBounds(r);
    createTextImage();
}

void HUDTextLabel::setText(String newText)
{
    text = newText;
    textChanged = true;
}

void HUDTextLabel::createTextImage()
{
    Image im(Image::PixelFormat::ARGB, (int) (getBounds().w*4), (int) (getBounds().h*4), true);
    Graphics g (im);
    g.setColour(Colours::white);
    g.setFont(Font(Environment::instance().getDefaultFont(), (getBounds().h * .75f) * 4.0f, Font::plain));
    g.drawText(text.toUpperCase(), 0, 0, (int) (getBounds().w*4), (int) (getBounds().h*4), Justification::centred, true);
    if (getDefaultTexture().textureId != 0)
        glDeleteTextures(1, &getDefaultTexture().textureId);
    setDefaultTexture(GfxTools::loadTextureFromJuceImage(im));
}
