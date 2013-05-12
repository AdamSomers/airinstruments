#ifndef h_GfxTools
#define h_GfxTools

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#if defined(_WIN32)
#include "GL/glew.h"
#endif

#include "../JuceLibraryCode/JuceHeader.h"

struct TextureDescription
{
    TextureDescription()
    : textureId(0), texX(0.f), texY(0.f), texW(1.f), texH(1.f), imageW(0), imageH(0) {}
    
    bool operator==(const TextureDescription& other) const
    {
        return name == other.name &&
        textureId == other.textureId &&
        texX == other.texX &&
        texY == other.texY &&
        texW == other.texW &&
        texH == other.texH &&
        imageW == other.imageW &&
        imageH == other.imageH;
    }
    
    bool operator!=(const TextureDescription& other) const
    {
        return !((*this) == other);
    }
    
    String name;
    GLuint textureId;
    GLfloat texX;
    GLfloat texY;
    GLfloat texW;
    GLfloat texH;
    int imageW;
    int imageH;
};

namespace GfxTools
{
    void collide(M3DVector3f rOrigin, M3DVector3f rNormal, M3DVector3f pOrigin, M3DVector3f pNormal, M3DVector3f outCollisionPoint);
    void drawBatch(GLTriangleBatch* pBatch, bool drawWireFrame = false);
    TextureDescription loadTextureFromJuceImage(const Image iamge, bool forcePowerOf2 = true);
    Array<TextureDescription> loadTextureAtlas(const File atlasXml);
}

#endif // h_GfxTools
