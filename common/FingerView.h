#ifndef h_FingerView
#define h_FingerView

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "Environment.h"
#include "View2d.h"
#include "Leap.h"

class FingerView
{
public:
    FingerView();
    void setup();
    void draw();
    void getScreenPos(M3DVector2f& inVec);
    
    void drawToTexture(const TextureDescription& td);

    class Listener
    {
    public:
        Listener();
        ~Listener();
        virtual void updatePointedState(FingerView* fv) = 0;
        void tap(FingerView* fv, float velocity);
        virtual void tap(float /*velocity*/) {}
        void circleBack(FingerView* fv);
        virtual void circleBack() {}
        void reset();
        bool needsReset;
        std::vector<FingerView*> pointers;
    protected:
        void fingerPointing(FingerView* fv);
        void fingerNotPointing(FingerView* fv);
    private:
    };
    
    GLFrame objectFrame;
    GLFrame prevFrame;
    Leap::Finger finger;
    bool inUse;
    int id;
    bool invalid;
private:
    GLTriangleBatch     coneBatch;
    GLTriangleBatch     cylinderBatch;
    int shaderId;
};

#endif // h_FingerView
