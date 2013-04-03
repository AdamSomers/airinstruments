#ifndef h_TrigView
#define h_TrigView

#include "HUD.h"

class TrigView : public HUDView
{
public:
    TrigView();
    ~TrigView();
    
    // HUDView overrides
    void setup();
    void draw();
    void loadTextures();
    
    void trigger();
    
private:
    GLBatch batch;
    float fade;
};

class TrigViewBank : public HUDView
{
public:
    TrigViewBank();
    ~TrigViewBank();
    
    // HUDView overrides
    void setup();
    void draw();
    void loadTextures();
    void setBounds(const HUDRect& b);
    
    void trigger(int midiNote);
    
private:
    GLBatch batch;
    GLuint textureID;
    std::vector<TrigView*> trigViews;
};

#endif // h_TrigView