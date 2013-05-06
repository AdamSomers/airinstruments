#ifndef h_PlayArea
#define h_PlayArea

#include "HUD.h"
#include "TextButton.h"

class PlayArea : public HUDView
               , public HUDButton::Listener
               , public ActionBroadcaster
{
public:
    PlayArea(int inId);
    ~PlayArea();
    
    // HUDView overrides
    void setBounds(const HUDRect& r);
    void setup();
    void draw();
    void loadTextures();
    
    void tap(int midiNote);
    
    const int getSelectedMidiNote() const { return selectedMidiNote; }
    void setSelectedMidiNote(int note);
    
    void setId(int newId) { id = newId; }
    const int getId() const { return id; }
    
    void setColor(const Colour& color);
    
    void enableAssignButton(bool shouldBeEnabled);
    void enableClearButton(bool shouldBeEnabled);
    
    // HUDButton::Listener override
    void buttonStateChanged(HUDButton* b);
    
private:
    Time fadeStartTime;
    int selectedMidiNote;
    int id;
    
    HUDView icon;
    HUDRect iconBounds;
    float iconRotation;
    
    TextHUDButton assignButton;
    TextHUDButton clearButton;
    
    GLfloat offColor[4];
    GLfloat onColor[4];
    GLfloat hoverOffColor[4];
    GLfloat hoverOnColor[4];
};

#endif // h_PlayArea
