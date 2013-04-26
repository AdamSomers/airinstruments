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

private:
    StringArray onText;
    StringArray offText;
    bool textChanged;
    GLuint onTexture;
    GLuint offTexture;
};

#endif /* defined(__AirBeats__TextButton__) */
