//
//  BeatCountView.h
//  AirBeats
//
//  Created by Adam Somers on 5/20/13.
//
//

#ifndef __AirBeats__BeatCountView__
#define __AirBeats__BeatCountView__

#include "HUD.h"
#include "Types.h"

class BeatCountView : public HUDView
{
public:
    BeatCountView();
    // HUDView overrides
    void draw();
    void setBounds(const HUDRect& b);
public:
    std::vector<SharedPtr<HUDView> > tickViews;
};


#endif /* defined(__AirBeats__BeatCountView__) */
