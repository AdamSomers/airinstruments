//
//  ScaleEditor.h
//  AirHarp
//
//  Created by Adam Somers on 7/14/13.
//
//

#ifndef __AirHarp__ScaleEditor__
#define __AirHarp__ScaleEditor__

#include "HUD.h"
#include "TextButton.h"
#include "Types.h"
#include "TextLabel.h"

class ScaleEditor : public HUDView
                  , public HUDButton::Listener
                  , public ActionBroadcaster
{
public:
    ScaleEditor();
    ~ScaleEditor() {}
    
    // HUDView overrides
    void setBounds(const HUDRect& r);

    void buttonStateChanged(HUDButton* b);
    
private:
    void createTextLabel();
    std::vector<SharedPtr<TextHUDButton> > scaleDegreeButtons;
    HUDTextLabel textLabel;
};

#endif /* defined(__AirHarp__ScaleEditor__) */
