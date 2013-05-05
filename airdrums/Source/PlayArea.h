#ifndef h_PlayArea
#define h_PlayArea

#include "HUD.h"

class PlayArea : public HUDView
{
public:
    PlayArea();
    ~PlayArea();
    
    // HUDView overrides
    void setBounds(const HUDRect& r);
    void setup();
    void draw();
    void loadTextures();
    
    void tap(int midiNote);
    
    const int getSelectedMidiNote() const { return selectedMidiNote; }
    void setSelectedMidiNote(int note);
    
    void setColor(const Colour& color);
    
private:
    Time fadeStartTime;
    int selectedMidiNote;
    
    HUDView icon;
    HUDRect iconBounds;
    float iconRotation;
    
    GLfloat offColor[4];
    GLfloat onColor[4];
    GLfloat hoverOffColor[4];
    GLfloat hoverOnColor[4];
};

#endif // h_PlayArea
