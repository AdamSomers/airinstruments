//
//  TutorialSlide.h
//
//  Created by Adam Somers on 4/10/13.
//
//

#ifndef h_TutorialSlide
#define h_TutorialSlide

#include <iostream>

#include "HUD.h"

class TutorialSlide : public HUDView
{
public:
    TutorialSlide();
    ~TutorialSlide();
    
    // HUDView overrides
    void draw();
    void loadTextures();
    
    void next();
    void back();
    void end();
    void begin();
    bool isDone();
    
    const int getSlideIndex() const { return slideIndex; }
    
private:
    Array<TextureDescription> textures;
    
    float fade;
    float fadeTarget;
    int slideIndex;
    int nextSlide;
    bool done;
};

#endif // h_TutorialSlide
