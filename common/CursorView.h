//
//  CursorView.h
//  AirBeats
//
//  Created by Adam Somers on 4/21/13.
//
//

#ifndef __AirBeats__CursorView__
#define __AirBeats__CursorView__

#include "View2d.h"

class CursorView : public View2d
{
public:
    CursorView();
    ~CursorView();
    
    void draw();
    void setX(float x);
    void setY(float y);
    void setEnabled(bool shouldBeEnabled);
    bool isEnabled() const { return enabled; }

    class Listener
    {
    public:
        Listener();
        virtual ~Listener();
        virtual void updateCursorState(float x, float y) = 0;
    };

private:
    float targetX;
    float targetY;
    bool enabled;
};


#endif /* defined(__AirBeats__CursorView__) */
