//
//  ShadowView.h
//  AirBeats
//
//  Created by Adam Somers on 8/1/13.
//
//

#ifndef __AirBeats__ShadowView__
#define __AirBeats__ShadowView__

#include <GLTools.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "FingerView.h"
#include "Drums.h"
#include "GfxTools.h"

class ShadowView
{
public:
    ShadowView();
    ~ShadowView();
    void setup();
    void update();
    void draw();
    
    GLFrame objectFrame;    
private:
    
    int selectedMidiNote;
    int id;
    
    static const int numVerts = 36;
    void makeMesh(M3DVector3f* inVerts, M3DVector3f* inNorms, M3DVector2f *inTexCoords);
    
    GLBatch     shadowBatch;
    TextureDescription shadowTexture;
    GLTriangleBatch ballBatch;
};

#endif /* defined(__AirBeats__ShadowView__) */
