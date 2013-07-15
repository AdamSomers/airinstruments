//
//  TextLabel.h
//  AirHarp
//
//  Created by Adam Somers on 7/14/13.
//
//

#ifndef __AirHarp__TextLabel__
#define __AirHarp__TextLabel__

#include "HUD.h"

class HUDTextLabel : public HUDView
{
public:
    HUDTextLabel();
    ~HUDTextLabel() {}
    
    // HUDView overrides
    void setBounds(const HUDRect& r);

    void setText(String newText);
    
private:
    void createTextImage();
    String text;
    bool textChanged;
};

#endif /* defined(__AirHarp__TextLabel__) */
