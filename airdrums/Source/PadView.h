#ifndef h_PadView
#define h_PadView

#include <GLTools.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "FingerView.h"
#include "Drums.h"
#include "GfxTools.h"

class PadView : public FingerView::Listener
{
public:
    PadView();
    ~PadView();
    void setup();
    void update();
    void draw();
    void triggerDisplay(float amount = 1.f);
    
    // FingerView::Listener overrides
    void updatePointedState(FingerView* inFingerView);
    void tap(float velocity);
    void circleBack();

    GLFrame objectFrame;
    int padNum;
    float padWidth;
    float padHeight;
    
    static GLFrame padSurfaceFrame;
    
private:
    bool hitTest(const M3DVector3f& point);
    
    static const int numVerts = 36;
    void makeMesh(M3DVector3f* inVerts, M3DVector3f* inNorms);
    GLBatch     padBatch;
    float fade;
    float redFade;
    float padDepth;
};

#endif // h_PadView
