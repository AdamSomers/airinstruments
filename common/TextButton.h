//
//  TextButton.h
//  AirBeats
//
//  Created by Adam Somers on 4/25/13.
//
//

#ifndef __AirBeats__TextButton__
#define __AirBeats__TextButton__

#include "HUD.h"

class TextHUDButton : public HUDButton
{
public:
    TextHUDButton(StringArray onText = StringArray("on"), StringArray offText = StringArray("off"));
    
    void draw();
    void loadTextures();
    
    
    void setText(StringArray onText, StringArray offText);
    StringArray getOnText() const { return onText; }
    StringArray getOffText() const { return offText; }
    
    void setTextColor(const Colour& onColor, const Colour& offColour);
    void setBackgroundColor(const Colour& onColor, const Colour& offColour);
    
    void setUpperCase(bool shouldBeUpperCase) { uppercase = shouldBeUpperCase; }

private:
    StringArray onText;
    StringArray offText;
    bool textChanged;
    TextureDescription onTextureDesc;
    TextureDescription offTextureDesc;
    Colour onTextColor;
    Colour offTextColor;
    Colour onBackgroundColor;
    Colour offBackgroundColor;
    bool uppercase;
};

#endif /* defined(__AirBeats__TextButton__) */
