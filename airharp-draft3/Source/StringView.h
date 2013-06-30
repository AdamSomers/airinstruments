#ifndef h_StringView
#define h_StringView

#include <GLTools.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include "FingerView.h"
#include "Harp.h"
#include "ScaleDegrees.h"
#include "GfxTools.h"
#include "SkinManager.h"

#define NUM_SAMPLES 64
static float gStringLineWidth = 0.015;

class StringView : public FingerView::Listener
{
public:
    StringView()
    : stringNum(0)
    , numSamples(NUM_SAMPLES)
    , harpNum(0)
    , stringWidth(0.06)
    , stringHeight(2.f)
    , yScale(1)
    , fade(0.f)
    {
        numSampleVerts = numSamples*2;
    }
    void setup()
    {
        M3DVector3f verts[4] = {
            0.0f, -stringHeight / 2.f, 0.0f,
            stringWidth, -stringHeight / 2.f, 0.0f,
            0.f, stringHeight / 2.f, 0.0f,
            stringWidth, stringHeight / 2.f, 0.0f
        };
        
        M3DVector3f normals[4] = {
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f
        };
        
        M3DVector2f texCoords[4] = {
            0.f, 1.f,
            1.f, 1.f,
            0.f, 0.f,
            1.f, 0.f
        };
        
        bgBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
        bgBatch.CopyVertexData3f(verts);
        bgBatch.CopyNormalDataf(normals);
        bgBatch.CopyTexCoordData2f(texCoords, 0);
        bgBatch.End();
        
        float w = gStringLineWidth;
        
        sampleVerts = new M3DVector3f[numSampleVerts];
        M3DVector3f *stringNormals = new M3DVector3f[numSampleVerts];
        M3DVector2f *stringTexCoords = new M3DVector2f[numSampleVerts];
        float yMin = -stringHeight / 2.f;
        float yMax = stringHeight / 2.f;
        float step = (yMax - yMin) / ((float)numSampleVerts / 2.f);
        float y = yMin;
        float texStep = 1.f / (float)(numSampleVerts / 2.f);
        float texY = 0.f;
        float x = stringWidth/2.f - w/2;
        float z = 0.f;
        for (int i = 0; i < numSampleVerts; ++i)
        {
            if (i % 2 == 0) {
                x = stringWidth/2.f + w/2;
            }
            else
                x = stringWidth/2.f - w/2;
            
            sampleVerts[i][0] = x;
            sampleVerts[i][1] = y;
            sampleVerts[i][2] = z;
            
            stringNormals[i][0] = 0.f;
            stringNormals[i][1] = 0.f;
            stringNormals[i][2] = -1.f;
            
            stringTexCoords[i][0] = (i % 2 == 0) ? 0.f : 1.f;
            stringTexCoords[i][1] = 1.f - texY;

            if (i % 2 == 1) {
                y += step;
                texY += texStep;
            }
        }
        
//        M3DVector2f texCoords[4] = {
//            0.f, 1.f,
//            1.f, 1.f,
//            0.f, 0.f,
//            1.f, 0.f
//        };
        
        stringBatch.Begin(GL_TRIANGLE_STRIP, numSampleVerts, 1);
        stringBatch.CopyTexCoordData2f(stringTexCoords, 0);
        stringBatch.CopyVertexData3f(sampleVerts);
        stringBatch.CopyNormalDataf(stringNormals);
        stringBatch.End();

        delete[] stringNormals;
        delete[] stringTexCoords;
    }
    
    void update()
    {
        ScopedLock sl(HarpManager::instance().getHarp(harpNum)->lock);
        
        float w = gStringLineWidth;
        float scale = 0.1f;
        
        if (HarpManager::instance().getHarp(harpNum)->GetBuffers().size() < stringNum+1)
            return;
        
        // skip this frame if not enough samples accumulated
        if (HarpManager::instance().getHarp(harpNum)->GetBuffers().at(stringNum)->GetSize() < NUM_SAMPLES)
            return;
        
        // pop the sample data from audio buffer and display along string
        SampleAccumulator::PeakBuffer peakBuffer = HarpManager::instance().getHarp(harpNum)->GetBuffers().at(stringNum)->Get();
        int numPeakBuffers = peakBuffer.size();
        for (int i = 0; i < numSampleVerts / 2; ++i)
        {
            float progress = (float)i / (float)(numSampleVerts / 2);
            float fSampleIndex = progress * numPeakBuffers;
            int iSampleIndex = (int)fSampleIndex;
            
            SampleAccumulator::PeakSample samp(0,0);
            if (iSampleIndex < numPeakBuffers) {
                samp = peakBuffer.at(iSampleIndex);
                prevSamp = iSampleIndex > 0 ? peakBuffer.at(iSampleIndex - 1) : prevSamp;
            }
            
            float val = fabsf(samp.first) > fabsf(samp.second) ? samp.first : samp.second;
            float prevVal = fabsf(prevSamp.first) > fabsf(prevSamp.second) ? prevSamp.first : prevSamp.second;
            val = linterp(prevVal, val, fSampleIndex - iSampleIndex);

            val *= scale;
            float x1 = stringWidth/2.f + w/2 + val;
            float x2 = x1;//stringWidth/2.f - w/2 + fabsf(val);
            sampleVerts[i * 2][0] = x1;
            sampleVerts[i * 2 + 1][0] = x2;
        }
        
        // update the vertex buffer (sends data to gpu)
        stringBatch.CopyVertexData3f(sampleVerts);
    }
    
    void updateStringBg()
    {
        M3DVector3f verts[4] = {
            0.0f, -stringHeight / 2.f, 0.0f,
            stringWidth, -stringHeight / 2.f, 0.0f,
            0.f, stringHeight / 2.f, 0.0f,
            stringWidth, stringHeight / 2.f, 0.0f
        };
        
        bgBatch.CopyVertexData3f(verts);
    }
    
    void draw()
    {
        Environment::instance().modelViewMatrix.PushMatrix();
        M3DMatrix44f mCamera;
        Environment::instance().cameraFrame.GetCameraMatrix(mCamera);
        Environment::instance().modelViewMatrix.MultMatrix(mCamera);
        M3DMatrix44f mObjectFrame;
        objectFrame.GetMatrix(mObjectFrame);
        Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);
        M3DMatrix44f mScale;
        m3dScaleMatrix44(mScale, 1.f, yScale, 1.f);
        //Environment::instance().modelViewMatrix.MultMatrix(mScale);
        
        GLfloat stringColor [] = { 1.f, 1.f, 1.f, 1.f };
        if (stringNum % HarpManager::instance().getHarp(0)->getScale().size() == 0) {
            stringColor[0] = 1.f;
            stringColor[1] = 1.f;
            stringColor[2] = .5f;
            stringColor[3] = 1.f;
        }
        
        GLfloat bgRectColor [] = {0.7f, 0.7f, 1.f, fade * 0.5f };
        GLfloat bgTexColor [] = { 1.0f, 1.f, 1.f, fade * 0.25f };
        if (pointers.size() > 0) {
            if (fade < 1.f)
                fade += 0.3f;
        }
        else if (fade > 0.f)
            fade -= 0.1f;

        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), bgRectColor);
        //bgBatch.Draw();
        glBindTexture(GL_TEXTURE_2D, SkinManager::instance().getSelectedSkin().getTexture("stringBg0").textureId);
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), bgTexColor, 0);
        //bgBatch.Draw();
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_DEPTH_TEST);
        glLineWidth(((1.f - (this->stringNum / 30.f)) * 8) / 2.f);
        
        glBindTexture(GL_TEXTURE_2D, SkinManager::instance().getSelectedSkin().getTexture("string0").textureId);
        //Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), 0);
        //Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), stringColor, 0);
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), stringColor);
        stringBatch.Draw();
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        Environment::instance().modelViewMatrix.PopMatrix();
    }
    
    void loadTextures()
    {
    }
    
    void updatePointedState(FingerView* inFingerView)
    {
        int numChords = HarpManager::instance().getHarp(0)->getNumSelectedChords();
        M3DVector3f point;
        M3DVector3f ray;
        M3DVector3f center;
        inFingerView->objectFrame.GetOrigin(point);
        inFingerView->objectFrame.GetForwardVector(ray);
        objectFrame.GetOrigin(center);
        
        // Calculate collision distance of finger ray with string rect
        M3DVector3f collisionPoint;
        M3DVector3f pNormal = { 0.f, 0.f, -1.f };
        m3dNormalizeVector3(ray);
        center[0] -= stringWidth / 2.f;
        GfxTools::collide(point, ray, center, pNormal, collisionPoint);
        float distanceX = fabsf(collisionPoint[0] - center[0]);
        float distanceY = fabsf(collisionPoint[1] - center[1]);
        // Ray intersecting rect = pointing at string
        if (distanceX < stringWidth / 2.f)// &&
            //distanceY < stringHeight * (1.f / (float)numChords) / 2.f)
        {
            fingerPointing(inFingerView);
        }
        else 
        {
            fingerNotPointing(inFingerView);
        }
        
        // When finger is on string plane, we can strum
        if (point[2] <= center[2])
        {
            // calculate collidion distance of previous finger ray with string
            M3DVector3f prevPoint;
            M3DVector3f prevRay;
            inFingerView->prevFrame.GetOrigin(prevPoint);
            inFingerView->prevFrame.GetForwardVector(prevRay);
            float distanceX = collisionPoint[0] - center[0];
            float distanceY = fabsf(collisionPoint[1] - center[1]);
            M3DVector3f prevCollisionPoint;
            GfxTools::collide(prevPoint, prevRay, center, pNormal, prevCollisionPoint);
            float prevDistanceX = prevCollisionPoint[0] - center[0];
            float prevDistanceY = prevCollisionPoint[1] - center[1];
            // String's center line was crossed = pluck
            if (((distanceX < 0 &&
                 prevDistanceX >= 0) ||
                (distanceX >= 0 &&
                 prevDistanceX < 0))) //&&
                //distanceY < stringHeight * (1.f / (float)numChords) / 2.f)
            {
                
                int direction = distanceX > 0.f ? 1 : -1;
                float pos = point[1];
                pos += 1.f;
                pos /= 2.f;
                
                Harp* h = HarpManager::instance().getHarp(harpNum);
                if (h->getChordMode()) {
                    int chordNum = pos * h->getNumSelectedChords();                    
                    h->setChord(chordNum);
                    pos = 0.5;
                }
                pluck(pos, 1, direction);
            }
        }
    }
    
    void tap(float velocity)
    {
        pluck(0.5f, velocity);
    }
    
    void pluck(float position, float velocity = 1.f, int direction = 1)
    {
        Harp* harp = HarpManager::instance().getHarp(0);
        std::vector<std::string>& scale = harp->getScale();
        int idx = stringNum % scale.size();
        int mult = (stringNum / (float)scale.size());
        int octaveSpan = (ScaleDegrees::getChromatic(scale[scale.size()-1]) / 12) + 1;
        int base = 32 + 12*octaveSpan*mult;
        int note = base + ScaleDegrees::getChromatic(scale[idx]);
        const int bufferSize = 512;
        float buffer[bufferSize];
        memset(buffer, 0, bufferSize);
        int midpoint = position * bufferSize;
        for (int x = 0; x < bufferSize; ++x)
        {
            if (x < midpoint)
                buffer[x] = -x / (float)midpoint;
            else
                buffer[x] = -(1.f - (x - midpoint) / (float)(bufferSize - midpoint));

            buffer[x] *= direction;
        }

        HarpManager::instance().getHarp(harpNum)->ExciteString(stringNum, note, 127.f * velocity, buffer, bufferSize);
        //HarpManager::instance().getHarp(harpNum)->NoteOn(stringNum, note, 127.f * velocity);
    }
    
    GLFrame objectFrame;
    int stringNum;
    int harpNum;
    float stringWidth;
    float stringHeight;
    float yScale;
    
private:
    inline float linterp(float v0,float v1,float t) { return v0+(v1-v0)*t; }

    GLBatch     bgBatch;
    GLBatch     stringBatch;
    GLuint      stringBgTextureID;
    GLuint      stringTextureID;
    int numSamples;
    int numSampleVerts;
    M3DVector3f* sampleVerts;
    SampleAccumulator::PeakSample prevSamp;

    float fade;
};


#endif // h_StringView
