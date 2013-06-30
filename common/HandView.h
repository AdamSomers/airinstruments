#ifndef h_HandView
#define h_HandView

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "Environment.h"
#include "Leap.h"

class HandView
{
public:
    HandView();
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
        virtual ~Listener();
        virtual void updatePointedState(HandView* fv) = 0;
        void reset();
        bool needsReset;
        std::vector<HandView*> pointers;
    protected:
        void handPointing(HandView* fv);
        void handNotPointing(HandView* fv);
    private:
    };
    
    GLFrame objectFrame;
    GLFrame prevFrame;
    Leap::Hand hand;
    bool inUse;
    int id;
    bool invalid;
private:
    int shaderId;
    GLBatch batch;
};

#endif // h_HandView
