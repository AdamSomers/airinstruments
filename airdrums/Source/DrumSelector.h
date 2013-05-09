#ifndef h_DrumSelector
#define h_DrumSelector

#include "HUD.h"
#include "Types.h"


class DrumSelector : public HUDView
                   , public HUDButton::Listener
                   , public ActionBroadcaster
{
public:
    DrumSelector();
    ~DrumSelector();
    
    // HUDView overrides
    void setup();
    void draw();
    void loadTextures();
    void setBounds(const HUDRect& b);
    
    // HUDButton::Listener override
    void buttonStateChanged(HUDButton* b);

    void setPadAssociation(int note, int pad);
    int getPadForNote(int note) const;
    int getNoteForPad(int pad) const;
    
    int getSelection() const { return selectedItem; }
    void setSelection(int selection);

    class Icon : public HUDButton
    {
    public:
        Icon(int id);
        ~Icon();
        void draw();
        void setBounds(const HUDRect& b);
        void setPadNumber(int pad);
        int getPadNumber() const { return padNumber; }
        void setHighlighted(bool shouldBeHighlighted);
    private:
        void updateBounds();
        
        HUDRect targetBounds;
        HUDRect tempBounds;
        int padNumber;
        float xStep, yStep, wStep, hStep;
        bool highlighted;
        Time flashStart;
        bool flashState;
    };

private:
    void layoutIcons();

    std::vector<SharedPtr<Icon> > icons;
    bool needsLayout;
    
    int selectedItem;
};

#endif // h_DrumSelector
