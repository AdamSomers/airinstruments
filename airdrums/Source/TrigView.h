#ifndef h_TrigView
#define h_TrigView

#include "HUD.h"
#include "Types.h"

class TrigView : public HUDView
{
public:
    TrigView(int inId);
    ~TrigView();
    
    // HUDView overrides
    void setup();
    void draw();
    void loadTextures();
    
    void trigger();
    
private:
    float fade;
    int id;
};

class TrigViewBank : public HUDView
                   , public MidiKeyboardStateListener
{
public:
    TrigViewBank();
    ~TrigViewBank();
    
    // HUDView overrides
    void setup();
    void draw();
    void loadTextures();
    void setBounds(const HUDRect& b);
    
    // MidiKeyboardStateListener overrides
    void handleNoteOn (MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity);
    void handleNoteOff (MidiKeyboardState* /*source*/, int /*midiChannel*/, int /*midiNoteNumber*/) {}
    
    void trigger(int midiNote);
    
private:
    TextureDescription textureDesc;
    std::vector<SharedPtr<TrigView> > trigViews;
};

#endif // h_TrigView