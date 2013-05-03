#ifndef h_DrumSelector
#define h_DrumSelector

#include "HUD.h"
#include "Types.h"


class DrumSelector : public HUDView
                   , public HUDButton::Listener
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

    class Icon : public HUDButton
    {
    public:
        Icon(int id);
        ~Icon();
        void draw();
        void setBounds(const HUDRect& b);
        void setPadNumber(int pad) { padNumber = pad; }
        int getPadNumber() const { return padNumber; }
    private:
        void updateBounds();
        
        HUDRect targetBounds;
        HUDRect tempBounds;
        int padNumber;
        float xStep, yStep, wStep, hStep;
    };

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void drumSelectorChanged(DrumSelector* selector) = 0;
    };
    
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    void layoutIcons();

    std::vector<SharedPtr<Icon> > icons;
    bool needsLayout;
    std::vector<Listener*> listeners;
};

#endif // h_DrumSelector
