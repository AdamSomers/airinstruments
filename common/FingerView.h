#ifndef h_FingerView
#define h_FingerView

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "Environment.h"
#include "View2d.h"
#include "Leap.h"
#include "Pointer3d.h"

class FingerView : public Pointer3d
{
public:
    FingerView();
    void setup();
    void draw();
    void setColor(const Colour& c) { color = c; }

    void drawWithShader(int shaderId);
    
    GLFrame prevFrame;
    bool inUse;
    int id;
    bool invalid;
    bool didSetup;
private:
    GLTriangleBatch     coneBatch;
    GLTriangleBatch     cylinderBatch;
    Colour color;
};

#endif // h_FingerView
