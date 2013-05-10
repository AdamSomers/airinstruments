#ifndef h_GfxTools
#define h_GfxTools

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "../JuceLibraryCode/JuceHeader.h"

// Images can be non-power of 2 (NPOT), but they are converted
// internally on load to next POT.  The description contains the
// original bounds and the ratio of original bounds to adjusted
// bounds for use in proper texture mapping.
struct TextureDescription
{
    TextureDescription()
    : textureId(0), imageW(0), imageH(0), texX(1.f), texY(1.f) {}
    
    bool operator==(const TextureDescription& other) const
    {
        return textureId == other.textureId &&
        imageW == other.imageW &&
        imageH == other.imageH &&
        texX == other.texX &&
        texY == other.texY;
    }
    
    bool operator!=(const TextureDescription& other) const
    {
        return !((*this) == other);
    }
    
    GLuint textureId;
    int imageW;   // Actual image width
    int imageH;   // Actual image height
    GLfloat texX; // Ratio of original to adjusted width
    GLfloat texY; // Ratio of original to adjusted height
};

namespace GfxTools
{
    void collide(M3DVector3f rOrigin, M3DVector3f rNormal, M3DVector3f pOrigin, M3DVector3f pNormal, M3DVector3f outCollisionPoint);
    void drawBatch(GLTriangleBatch* pBatch, bool drawWireFrame = false);
    TextureDescription loadTextureFromJuceImage(const Image& iamge);
}

#endif // h_GfxTools
