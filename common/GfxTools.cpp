#include "GfxTools.h"
#include "Environment.h"
#include "Types.h"

#define USE_MIPMAP 1
#define FORCE_POWER_OF_TWO_RESOURCES 1

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
    
    TextureDescription loadTextureFromJuceImage(const Image image, bool forcePowerOf2 /* = true */)
    {
        TextureDescription desc;
        glGenTextures(1, &desc.textureId);
        glBindTexture(GL_TEXTURE_2D, desc.textureId);
        
        desc.imageW = image.getWidth();
        desc.imageH = image.getHeight();
        
        Image imageToUse = image;
        
        if (forcePowerOf2)
        {
            if (image.getWidth() != image.getHeight() || !isPowerOfTwo(image.getWidth()) || !isPowerOfTwo(image.getHeight()))
            {
                int side = jmax(image.getWidth(), image.getHeight());
                if (!isPowerOfTwo(image.getWidth()))
                {
                    int v = nextPowerOfTwo(side);
                    Logger::outputDebugString("Old size: " + String(side) + " new size: " + String(v));
                    side = v;
                }
                Image newImage(image.getFormat(), side, side, true);
                Graphics g(newImage);
                g.drawImageAt(image, 0, 0);
                imageToUse = newImage;
                desc.texW = image.getWidth() / (float)side;
                desc.texH = image.getHeight() / (float)side;
            }
        }

        Image::BitmapData bitmapData(imageToUse, 0, 0, imageToUse.getWidth(), imageToUse.getHeight());
        GLbyte* bits = (GLbyte*)bitmapData.data;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
#if USE_MIPMAP
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
#if USE_MIPMAP
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
#endif

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, 4, imageToUse.getWidth(), imageToUse.getHeight(), 0,
                     imageToUse.getFormat() == Image::RGB ? GL_RGB : GL_BGRA, GL_UNSIGNED_BYTE, bits);
#if USE_MIPMAP
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glGenerateMipmap(GL_TEXTURE_2D);
#endif
        return desc;
    }
    
    Array<TextureDescription> loadTextureAtlas(const File atlasXml)
    {
        Array<TextureDescription> textures;

        if (!atlasXml.exists()) {
            Logger::outputDebugString("Error: " + atlasXml.getFileName() + " does not exist!");
            return textures;
        }
        
        UniquePtr<XmlElement> document(XmlDocument::parse(atlasXml));
        if (document == nullptr) {
            Logger::outputDebugString("Error: Could not parse " + atlasXml.getFileName());
            return textures;
        }
        
        if (!document->hasTagName("TextureAtlas")) {
            Logger::outputDebugString("Error: TextureAtlas tag not found in " + atlasXml.getFileName());
            return textures;
        }
        
        String atlasFileNmae = document->getStringAttribute("imagePath");
        File atlasImageFile(atlasXml.getParentDirectory().getChildFile(atlasFileNmae));
        if (!atlasImageFile.exists()) {
            Logger::outputDebugString("Error: " + atlasFileNmae + " not found");
            return textures;
        }
        
        Image atlasImage = ImageFileFormat::loadFrom(atlasImageFile);
        TextureDescription atlasTexture = GfxTools::loadTextureFromJuceImage(atlasImage, false);
        
        XmlElement* spriteElement = document->getFirstChildElement();
        while (spriteElement != nullptr) {
            TextureDescription td;
            td.textureId = atlasTexture.textureId;
            td.name = spriteElement->getStringAttribute("n").replace(".png", "");
            td.texX = spriteElement->getIntAttribute("x") / (float)atlasImage.getWidth();
            td.texY = spriteElement->getIntAttribute("y") / (float)atlasImage.getHeight();
            td.texW = spriteElement->getIntAttribute("w") / (float)atlasImage.getWidth();
            td.texH = spriteElement->getIntAttribute("h") / (float)atlasImage.getHeight();
            td.imageW = spriteElement->getIntAttribute("w");
            td.imageH = spriteElement->getIntAttribute("h");
            textures.add(td);
            spriteElement = spriteElement->getNextElementWithTagName("sprite");
        }
        
        return textures;
    }
}