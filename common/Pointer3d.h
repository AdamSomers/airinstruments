//
//  Pointer3d.h
//  AirBeats
//
//  Created by Adam Somers on 7/25/13.
//
//

#ifndef __AirBeats__Pointer3d__
#define __AirBeats__Pointer3d__

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "Environment.h"
#include "View2d.h"
#include "Leap.h"


class Pointer3d
{
public:
    void getScreenPos(M3DVector2f& inVec);

    class Listener
    {
    public:
        Listener();
        ~Listener();
        virtual void updatePointedState(Pointer3d* fv) = 0;
        void tap(Pointer3d* fv, float velocity);
        virtual void tap(float /*velocity*/) {}
        void circleBack(Pointer3d* fv);
        virtual void circleBack() {}
        void reset();
        bool needsReset;
        std::vector<Pointer3d*> pointers;
    protected:
        void fingerPointing(Pointer3d* fv);
        void fingerNotPointing(Pointer3d* fv);
    private:
    };

    GLFrame objectFrame;
protected:
    void updateScreenPos();

    M3DVector2f screenPos;

};
#endif /* defined(__AirBeats__Pointer3d__) */
