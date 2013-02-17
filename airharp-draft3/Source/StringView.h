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

#define NUM_SAMPLES 128
static float gStringLineWidth = 0.01;

class StringView : public FingerView::Listener
{
public:
    StringView()
    : stringNum(0)
    , numSamples(NUM_SAMPLES)
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
        
        bgBatch.Begin(GL_TRIANGLE_STRIP, 4);
        bgBatch.CopyVertexData3f(verts);
        bgBatch.CopyNormalDataf(normals);
        bgBatch.End();
        
        float w = gStringLineWidth;
        
        sampleVerts = new M3DVector3f[numSampleVerts];
        M3DVector3f stringNormals[numSampleVerts];
        float yMin = -stringHeight / 2.f;
        float yMax = stringHeight / 2.f;
        float step = (yMax - yMin) / ((float)numSampleVerts / 2.f);
        float y = yMin;
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
            
            if (i % 2 == 1) {
                y += step;
            }
        }
        
        stringBatch.Begin(GL_TRIANGLE_STRIP, numSampleVerts);
        stringBatch.CopyVertexData3f(sampleVerts);
        stringBatch.CopyNormalDataf(stringNormals);
        stringBatch.End();
    }
    
    void update()
    {
        float w = gStringLineWidth;
        float scale = 0.1f;
        
        // skip this frame if not enough samples accumulated
        if (Harp::instance().GetBuffers().at(stringNum)->GetSize() < NUM_SAMPLES)
            return;
        
        // pop the sample data from audio buffer and display along string
        SampleAccumulator::PeakBuffer peakBuffer = Harp::instance().GetBuffers().at(stringNum)->Get();
        for (int i = 0; i < numSampleVerts / 2; ++i)
        {
            long numPeakBuffers = peakBuffer.size();
            SampleAccumulator::PeakSample samp(0,0);
            if (i < numPeakBuffers)
                samp = peakBuffer.at(i);
            float val = fabsf(samp.first) > fabsf(samp.second) ? samp.first : samp.second;
            val *= scale;
            float x1 = stringWidth/2.f + w/2 + fabsf(val);
            float x2 = stringWidth/2.f - w/2 - fabsf(val);
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
        
        GLfloat stringColor [] = { 1, 1, 1, 1 };
        if (stringNum % Harp::gScale.size() == 0) {
            stringColor[0] = 1.f;
            stringColor[1] = 1.f;
            stringColor[2] = .5f;
            stringColor[3] = 1.f;
        }
        //        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_SMOOTH);

        glDisable(GL_CULL_FACE);
        
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), stringColor);
        stringBatch.Draw();
        
        GLfloat color [] = { 0.7f, 0.7f, 1.f, fade * 0.3f };
        if (pointers.size() > 0) {
            if (fade < 1.f)
                fade += 0.3f;
        }
        else if (fade > 0.f)
            fade -= 0.1f;

        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), color);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        bgBatch.Draw();
        Environment::instance().modelViewMatrix.PopMatrix();
    }
    
    void updatePointedState(FingerView* inFingerView)
    {
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
        float distance = fabsf(collisionPoint[0] - center[0]);
        // Ray intersecting rect = pointing at string
        if (distance < stringWidth / 2.f)
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
            distance = collisionPoint[0] - center[0];
            M3DVector3f prevCollisionPoint;
            GfxTools::collide(prevPoint, prevRay, center, pNormal, prevCollisionPoint);
            float prevDistance = prevCollisionPoint[0] - center[0];
            // String's center line was crossed = pluck
            if ((distance < 0 &&
                 prevDistance >= 0) ||
                (distance >= 0 &&
                 prevDistance < 0))
            {
                float pos = point[1];
                pos += 1.f;
                pos /= 2.f;
                
                printf("%f\n", pos);
                pluck(pos);
            }
        }
    }
    
    void tap(FingerView* inFingerView, float velocity)
    {
        M3DVector3f point;
        M3DVector3f center;
        inFingerView->objectFrame.GetOrigin(point);
        objectFrame.GetOrigin(center);
        if (point[2] <= center[2])
        {
            return;
        }
        auto iter = std::find(pointers.begin(), pointers.end(), inFingerView);
        if (iter != pointers.end())
            pluck(0.5f, velocity);
    }
    
    void pluck(float position, float velocity = 1.f)
    {
        int idx = stringNum % Harp::gScale.size();
        int mult = (stringNum / (float)Harp::gScale.size());
        int octaveSpan = (ScaleDegrees::getChromatic(Harp::gScale.at(Harp::gScale.size()-1)) / 12) + 1;
        int base = 32 + 12*octaveSpan*mult;
        int note = base + ScaleDegrees::getChromatic(Harp::gScale.at(idx));
        int bufferSize = 512;
        float buffer[bufferSize];
        memset(buffer, 0, bufferSize);
        int midpoint = position * bufferSize;
        for (int x = 0; x < bufferSize; ++x)
        {
            if (x < midpoint)
                buffer[x] = -x / (float)midpoint;
            else
                buffer[x] = -(1.f - (x - midpoint) / (float)(bufferSize - midpoint));
            
            //if (fingerPrevX > t)
            //    buffer[x] = -buffer[x];
        }
        Harp::instance().ExciteString(stringNum, note, 127.f * velocity, buffer, bufferSize);
    }
    
    GLFrame objectFrame;
    int stringNum;
    float stringWidth = 0.06;
    float stringHeight = 2.f;
    
private:
    GLBatch     bgBatch;
    GLBatch     stringBatch;
    int numSamples;
    int numSampleVerts;
    M3DVector3f* sampleVerts;
    float fade = 0.f;
};


#endif // h_StringView
