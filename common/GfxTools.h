#ifndef h_GfxTools
#define h_GfxTools

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "../JuceLibraryCode/JuceHeader.h"

namespace GfxTools
{
    void collide(M3DVector3f rOrigin, M3DVector3f rNormal, M3DVector3f pOrigin, M3DVector3f pNormal, M3DVector3f outCollisionPoint);
    void drawBatch(GLTriangleBatch* pBatch, bool drawWireFrame = false);
    void loadTextureFromJuceImate(const Image& iamge);
}

#endif // h_GfxTools
