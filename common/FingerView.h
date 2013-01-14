#ifndef h_FingerView
#define h_FingerView

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "Environment.h"
#include "Leap.h"

class FingerView
{
public:
    FingerView();
    void setup();
    void draw();
    float normalizedX();
    float normalizedY();
    float normalizedZ();
    void getScreenPos(M3DVector2f& inVec);
    
    class Listener
    {
    public:
        Listener();
        virtual void updatePointedState(FingerView* fv) = 0;
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
};

#endif // h_FingerView
