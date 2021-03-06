#ifndef h_PadView
#define h_PadView

#include <GLTools.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "FingerView.h"
#include "Drums.h"
#include "GfxTools.h"

class PadView
{
public:
    PadView();
    ~PadView();
    void setup();
    void update();
    void draw();
    void triggerDisplay(float amount = 1.f);
    void setSelectedMidiNote(int note);
    int getSelectedMidiNote() const { return selectedMidiNote; }
    void setColor(const Colour& color);
    void setHovering(bool hover) { hovering = hover; }
    void getScreenPos(M3DVector2f& inVec);
    void tap(int midiNote);
    bool hitTest(const M3DVector3f& point);

    GLFrame objectFrame;
    int padNum;
    float padWidth;
    float padHeight;
    
    static GLFrame padSurfaceFrame;
    
private:
    
    int selectedMidiNote;
    int id;
    
    static const int numVerts = 36;
    void makePadMesh(M3DVector3f* inVerts, M3DVector3f* inNorms);
    void makeSurfaceMesh(M3DVector3f* inVerts, M3DVector3f* inNorms);
    void makeIconMesh(M3DVector3f* inVerts, M3DVector3f* inNorms);
    void makeTextMesh(M3DVector3f* inVerts, M3DVector3f* inNorms);

    GLBatch     padBatch;
    float fade;
    Time fadeStartTime;
    float hoverFade;
    float iconRotation;
    float padDepth;
    GLBatch bgBatch;
    TextureDescription backgroundOnTexture;
    TextureDescription backgroundOffTexture;
    
    GLBatch textBatch;
    TextureDescription textTexture;
    GLBatch iconBatch;
    TextureDescription iconTexture;
    GLfloat iconColor[4];
    
    bool hovering;
};

#endif // h_PadView
