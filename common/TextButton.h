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
    
    void setTextColor(const Colour& newColor);
    void setBackgroundColor(const Colour& newColor);

private:
    StringArray onText;
    StringArray offText;
    bool textChanged;
    TextureDescription onTextureDesc;
    TextureDescription offTextureDesc;
    Colour textColor;
    Colour backgroundColor;
    
};

#endif /* defined(__AirBeats__TextButton__) */
