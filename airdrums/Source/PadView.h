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
    
    // FingerView::Listener overrides
    void updatePointedState(FingerView* inFingerView);
    void tap(FingerView* fv, float velocity) {};

    GLFrame objectFrame;
    int padNum= 0;
    float padWidth = 0.1;
    float padHeight = 0.1;
    
private:
    GLBatch     padBatch;
    float fade = 1.f;
    float padDepth = 0.025;
};

#endif // h_PadView
