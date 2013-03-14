#ifndef h_PlayArea
#define h_PlayArea

#include "HUD.h"

class PlayArea : public HUDView
{
public:
    PlayArea();
    ~PlayArea();
    
    // HUDView overrides
    void setup();
    void draw();
    
    void tap();
    
private:
    GLBatch batch;
    GLuint onTextureID;
    GLuint offTextureID;
    float fade;
    bool hovering;
    
    GLfloat offColor[4];
    GLfloat onColor[4];
    GLfloat hoverOffColor[4];
    GLfloat hoverOnColor[4];
};

#endif // h_PlayArea
