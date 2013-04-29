#include "GfxTools.h"
#include "Environment.h"

namespace GfxTools
{
    void drawBatch(GLTriangleBatch* pBatch, bool drawWireFrame)
    {
        pBatch->Draw();
        
        if (drawWireFrame)
        {
            // Draw wrireframe
            glPolygonOffset(-1.0f, -1.0f);
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glLineWidth(1.f);
            GLfloat vBlue [] = { 0.f, 0.f, 1.f, 1.f };
            Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), vBlue);
            pBatch->Draw();
        }
    }
    
    // Ray-plane collision
    void collide(M3DVector3f rOrigin, M3DVector3f rNormal, M3DVector3f pOrigin, M3DVector3f pNormal, M3DVector3f outCollisionPoint)
    {
        // point on plane
        float px = pOrigin[0];
        float py = pOrigin[1];
        float pz = pOrigin[2];
        
        // origin of ray
        float ox = rOrigin[0];
        float oy = rOrigin[1];
        float oz = rOrigin[2];
        
        // line vector
        float dx = rNormal[0];
        float dy = rNormal[1];
        float dz = rNormal[2];
        
        // t = -((rOrigin - pOrigin) . pNormal)/(rNormal . pNormal)
        // <pntx,pnty,pntz> = rOrigin + t*rNormal
        
        M3DVector3f v1 = {ox-px,oy-py,oz-pz};
        float t = -(m3dDotProduct3(v1,pNormal)/m3dDotProduct3(
                                                              rNormal,pNormal));
        float  pX = ox + t*dx;
        float  pY = oy + t*dy;
        float  pZ = oz + t*dz;
        outCollisionPoint[0] = pX;
        outCollisionPoint[1] = pY;
        outCollisionPoint[2] = pZ;
    }
    
    void loadTextureFromJuceImage(const Image& image)
    {
        Image::BitmapData bitmapData(image, 0, 0, image.getWidth(), image.getHeight());
        GLbyte* bits = (GLbyte*)bitmapData.data;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        GLfloat fLargest;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, 4, image.getWidth(), image.getHeight(), 0,
                     image.getFormat() == Image::RGB ? GL_RGB : GL_BGRA, GL_UNSIGNED_BYTE, bits);
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glGenerateMipmap(GL_TEXTURE_2D);

    }
}