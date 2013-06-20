//
//  MainView.h
//  AirBeats
//
//  Created by Adam Somers on 6/11/13.
//
//

#ifndef __AirBeats__MainView__
#define __AirBeats__MainView__

#include "HUD.h"
#include "TextButton.h"
#include "Types.h"

class MainView : public HUDView
               , public HUDButton::Listener
               , public ActionBroadcaster
{
public:
    MainView();
    ~MainView();
    
    // HUDView overrides
    void setBounds(const HUDRect& b);
    void setVisible(bool shouldBeVisible, int fadeTimeMs);
    
    // HUDButton::Listener override
    void buttonStateChanged(HUDButton* b);
    
    void setPadPos(int padIndex, GLfloat x, GLfloat y);
    void enableAssignButtons(bool shouldBeEnabled);
    void enableClearButtons(bool shouldBeEnabled);

private:
    std::vector<SharedPtr<TextHUDButton> > assignButtons;
    std::vector<SharedPtr<TextHUDButton> > clearButtons;
};

#endif /* defined(__AirBeats__MainView__) */
