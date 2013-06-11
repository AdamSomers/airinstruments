//
//  ListSelector.h
//  AirBeats
//
//  Created by Adam Somers on 6/5/13.
//
//

#ifndef __AirBeats__ListSelector__
#define __AirBeats__ListSelector__

#include "HUD.h"
#include "TextButton.h"
#include <deque>

class ListSelector : public HUDView
                   , public HUDButton::Listener
                   , public ChangeBroadcaster
{
public:
    ListSelector(String name, bool left = false);
    ~ListSelector();
    
    // HUDView overrides
    void setBounds(const HUDRect& b);
    void draw();
    
    // HUDButton::Listener overrides
    void buttonStateChanged(HUDButton* b);
    
    void setSelection(int sel);
    int getSelection() const { return selection; }
    
    void setXRange(float shown, float hidden);
    
    void setEnabled(bool shouldBeEnabled);
    bool isEnabled() const { return enabled; }
    
    class Icon : public HUDButton
    {
    public:
        Icon(int id);
        ~Icon();
        void draw();
        void setBounds(const HUDRect& b);
        int getId() const { return id; }
        HUDView highlightView;
        bool isSelected;

    private:
        void updateBounds();
        int id;
        HUDRect targetBounds;
        HUDRect tempBounds;
    };
    
    void addIcon(Icon* icon);
    void removeAllIcons();
    
private:
    void updateBounds();
    void layoutIcons();
    void updateVisibleIcons();

    int selection;
    bool needsLayout;
    bool leftHanded;
    float shownX;
    float hiddenX;
    bool enabled;
    float iconSpacing;
    float iconHeight;
    int maxVisibleIcons;
    
    std::vector<Icon*> icons;
    std::deque<Icon*> visibleIcons;

    TextHUDButton displayToggleButton;
    HUDButton upButton;
    HUDButton downButton;
    HUDView tabView;
    
    Time displayChangeStart;
};

#endif /* defined(__AirBeats__ListSelector__) */
