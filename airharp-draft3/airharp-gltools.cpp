// Smoother.cpp
// OpenGL SuperBible
// Demonstrates point and line antialiasing
// Program by Richard S. Wright Jr.
#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#ifdef __APPLE__
#include <glut/glut.h>
#include <OpenGL/OpenGL.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#include <vector>
#include <map>
#include <mutex>
#include <cmath>

#include "Leap.h"

#include "HUD.h"
#include "HarpHUD.h"
#include "Environment.h"

#include "AudioServer.h"
#include "Harp.h"

// Array of small stars
#define SMALL_STARS     100
#define MEDIUM_STARS     40
#define LARGE_STARS      15

#define SCREEN_X        800
#define SCREEN_Y        600

#define NUM_SAMPLES 128
#define BUFFER_SIZE 512
#define NUM_STRINGS 24
float gStringWidth = 0.06;
float gStringLineWidth = 0.01;

GLFrame viewFrame;

GLMatrixStack projectionMatrix;


GLFrustum viewFrustum;
GLFrame	cameraFrame;

// Pentatonic Major
const int gPentatonicMajor[] = { 0, 2, 5, 7, 9};
const int gPentatonicMajorIntervals = 5;

// Pentatonic Minor
const int gPentatonicMinor[] = { 0, 3, 5, 7, 10};
const int gPentatonicMinorIntervals = 5;

// Whole-tone
const int gWholeTone[] = { 0, 2, 4, 6, 8, 10};
const int gWholeToneIntervals = 6;

// Diatonic
const int gDiatonic[] = { 0, 2, 4, 5, 7, 9, 11};
const int gDiatonicIntervals = 7;

const int* gScale = gPentatonicMajor;
int gScaleIntervals = gPentatonicMajorIntervals;

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

void DrawWireFramedBatch(GLTriangleBatch* pBatch);

class FingerView
{
public:
    FingerView()
    : inUse(false)
    , id(-1)
    , invalid(false)
    {}
    void setup()
    {
        gltMakeCylinder(coneBatch, 0.f, 0.02f, -.1f, 10, 2);
        gltMakeCylinder(cylinderBatch, .001f, .01f, -.2f, 10, 2);
    }
    
    void draw()
    {
        Environment::instance().modelViewMatrix.PushMatrix();
        M3DMatrix44f mCamera;
        cameraFrame.GetCameraMatrix(mCamera);
        Environment::instance().modelViewMatrix.MultMatrix(mCamera);
        M3DMatrix44f mObjectFrame;
        objectFrame.GetMatrix(mObjectFrame);
        Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);
        GLfloat vBlack [] = { 0.f, 0.f, 0.f, 1.f };
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), vBlack);
        DrawWireFramedBatch(&cylinderBatch);
        DrawWireFramedBatch(&coneBatch);
        
        Environment::instance().modelViewMatrix.PopMatrix();
    }
    
    float normalizedX()
    {
        return (finger.tipPosition().x / 400.f);
    }
    
    float normalizedY()
    {
        return (finger.tipPosition().y / 500.f);
    }
    
    float normalizedZ()
    {
        return finger.tipPosition().z / 250.f;
    }

    GLFrame             objectFrame;
    GLFrame             prevFrame;
    Leap::Finger finger;
    bool inUse;
    int id;
    bool invalid;
private:
    GLTriangleBatch     coneBatch;
    GLTriangleBatch     cylinderBatch;
};

class StringView
{
public:
    StringView()
    : pointed(false)
    , stringNum(0)
    , lastPointer(NULL)
    , invalid(false)
    , numSamples(NUM_SAMPLES)
    {
        numSampleVerts = numSamples*2;
    }
    void setup()
    {
        M3DVector3f verts[4] = {
            0.0f, -.8f, 0.0f,
            gStringWidth, -.8f, 0.0f,
            0.f, .8, 0.0f,
            gStringWidth, .8, 0.0f
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
        float yMin = -0.8;
        float yMax = 0.8;
        float step = (yMax - yMin) / ((float)numSampleVerts / 2.f);
        float y = -.8f;
        float x = gStringWidth/2.f - w/2;
        float z = 0.f;
        for (int i = 0; i < numSampleVerts; ++i)
        {
            if (i % 2 == 0) {
                x = gStringWidth/2.f + w/2;
            }
            else
                x = gStringWidth/2.f - w/2;
            
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
        if (Harp::GetInstance()->GetBuffers().at(stringNum)->GetSize() < NUM_SAMPLES)
           return;
        
        // pop the sample data from audio buffer and display along string
        SampleAccumulator::PeakBuffer peakBuffer = Harp::GetInstance()->GetBuffers().at(stringNum)->Get();
        for (int i = 0; i < numSampleVerts / 2; ++i)
        {
            long numPeakBuffers = peakBuffer.size();
            SampleAccumulator::PeakSample samp(0,0);
            if (i < numPeakBuffers)
                samp = peakBuffer.at(i);
            float val = fabsf(samp.first) > fabsf(samp.second) ? samp.first : samp.second;
            val *= scale;
            float x1 = gStringWidth/2.f + w/2 + fabsf(val);
            float x2 = gStringWidth/2.f - w/2 - fabsf(val);
            sampleVerts[i * 2][0] = x1;
            sampleVerts[i * 2 + 1][0] = x2;
        }
        
        // update the vertex buffer (sends data to gpu)
        stringBatch.CopyVertexData3f(sampleVerts);
    }
    
    void updateStringBg()
    {
        M3DVector3f verts[4] = {
            0.0f, -.8f, 0.0f,
            gStringWidth, -.8f, 0.0f,
            0.f, .8, 0.0f,
            gStringWidth, .8, 0.0f
        };
        
        bgBatch.CopyVertexData3f(verts);
    }
    
    void draw()
    {
        Environment::instance().modelViewMatrix.PushMatrix();
        M3DMatrix44f mCamera;
        cameraFrame.GetCameraMatrix(mCamera);
        Environment::instance().modelViewMatrix.MultMatrix(mCamera);
        M3DMatrix44f mObjectFrame;
        objectFrame.GetMatrix(mObjectFrame);
        Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);
        
        GLfloat white [] = { 1, 1, 1, 1 };
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), white);
        stringBatch.Draw();

        GLfloat color [] = { 0.6f, 0.6f, .6f, .0f };
        if (pointed) {
            color[0] = .7f;
            color[1] = .7f;
            color[2] = 1.f;
            color[3] = .5f;
        }
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
        center[0] -= gStringWidth / 2.f;
        collide(point, ray, center, pNormal, collisionPoint);
        float distance = fabsf(collisionPoint[0] - center[0]);
        // Ray intersecting rect = pointing at string
        if (distance < gStringWidth / 2.f)
        {
            if (lastPointer != inFingerView)
            {
                lastPointer = inFingerView;
                pointed = true;
            }
            invalid = false;
        }
        else if (lastPointer == inFingerView)
        {
            pointed = false;
            lastPointer = NULL;
            invalid = false;
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
            collide(prevPoint, prevRay, center, pNormal, prevCollisionPoint);
            float prevDistance = prevCollisionPoint[0] - center[0];
            // String's center line was crossed = pluck
            if ((distance < 0 &&
                 prevDistance >= 0) ||
                (distance >= 0 &&
                 prevDistance < 0))
            {
                pluck(0.5);
            }
        }
    }
    
    void pluck(float position)
    {
        int idx = stringNum % gScaleIntervals;
        int mult = (stringNum / (float)gScaleIntervals);
        int base = 32 + 12*mult;
        int note = base + gScale[idx];
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
        Harp::GetInstance()->ExciteString(stringNum, note, 127, buffer, bufferSize);
    }
    
    void reset()
    {
        pointed = false;
        lastPointer = NULL;
        invalid = false;
    }
    
    GLFrame objectFrame;
    int stringNum;
    FingerView* lastPointer;
    bool invalid;

private:
    GLBatch     bgBatch;
    GLBatch     stringBatch;
    bool pointed;
    int numSamples;
    int numSampleVerts;
    M3DVector3f* sampleVerts;
};

std::map<int,FingerView*> gFingers;
std::vector<StringView*> gStrings;

std::mutex gLock;
float fingerX, fingerY, fingerZ = 0;

class HarpListener : public Leap::Listener {
public:
    virtual void onInit(const Leap::Controller&);
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onFrame(const Leap::Controller&);
};

void HarpListener::onInit(const Leap::Controller& controller) {
    std::cout << "Initialized" << std::endl;
}

void HarpListener::onConnect(const Leap::Controller& controller) {
    std::cout << "Connected" << std::endl;
}

void HarpListener::onDisconnect(const Leap::Controller& controller) {
    std::cout << "Disconnected" << std::endl;
}

void HarpListener::onFrame(const Leap::Controller& controller) {

    gLock.lock();
    for (auto i : gFingers)
    {
        i.second->invalid = true;
    }
    gLock.unlock();
    
    for (StringView* sv : gStrings)
    {
        sv->invalid = true;
    }
    
    // Get the most recent frame and report some basic information
    const Leap::Frame frame = controller.frame();
    const Leap::HandList& hands = frame.hands();
    const size_t numHands = hands.count();
    //    std::cout << "Frame id: " << frame.id()
    //    << ", timestamp: " << frame.timestamp()
    //    << ", hands: " << numHands << std::endl;
    
    if (numHands >= 1) {
        // Get the first hand
        for (int h = 0; h < numHands; ++h) {
            const Leap::Hand& hand = hands[h];
            
            
            // Check if the hand has any fingers
            const Leap::FingerList& fingers = hand.fingers();
            const size_t numFingers = fingers.count();
            if (numFingers >= 1)
            {
                // Calculate the hand's average finger tip position
                Leap::Vector pos(0, 0, 0);
                for (int i = 0; i < numFingers; ++i)
                {
                    bool inserted = false;
                    const Leap::Finger& f = fingers[i];
                    gLock.lock();
                    FingerView* fv = NULL;
                    auto iter = gFingers.find(f.id());
                    if (iter == gFingers.end())
                    {
                        // Finger map is pre-allocated, if we don't find one too bad!
                        printf("Error! No finger in map for id %d\n", f.id());
                        continue;
                    }
                    else
                    {
                        fv = (*iter).second;
                        fv->finger = f;
                        if (!fv->inUse) {
                            fv->inUse = true;
                            inserted = true;
                            printf("Inserted finger %d\n", fv->id);
                        }
                        fv->invalid = false;
                    }
                    gLock.unlock();

                    float x = fv->normalizedX();
                    float y = fv->normalizedY();
                    float z = fv->normalizedZ();

                    float dirX = f.direction().x;
                    float dirY = f.direction().y;
                    float dirZ = f.direction().z;
                    
                    M3DVector3f prev;
                    fv->objectFrame.GetForwardVector(prev);
                    fv->prevFrame.SetForwardVector(prev);
                    fv->objectFrame.GetOrigin(prev);
                    fv->prevFrame.SetOrigin(prev);
                    
                    fv->objectFrame.SetForwardVector(dirX,dirY,dirZ);
                    float scaledX = x*2*(Environment::screenW/(float)Environment::screenH);
                    float scaledY = (y-.5)*2;
                    if (z < 0) z = 0;
                    float scaledZ = z*5-12;
                    fv->objectFrame.SetOrigin(scaledX,scaledY,scaledZ);
                    
                    if (inserted) {
                        fv->prevFrame.SetForwardVector(dirX,dirY,dirZ);
                        fv->prevFrame.SetOrigin(scaledX,scaledY,scaledZ);
                    }
                    
                    for (StringView* sv : gStrings)
                    {
                        sv->updatePointedState(fv);
                    }
                    //printf("%1.2f %1.2f %1.2f\n", scaledX,scaledY,scaledZ);
                }
            }
        }
    }
    
    for (StringView* sv : gStrings)
    {
        if (sv->invalid) {
            sv->reset();
        }
    }
    
    for (auto iter : gFingers)
    {
        FingerView* fv = iter.second;
        if (fv->invalid && fv->inUse) {
            fv->inUse = false;
            printf("Removed finger %d\n", fv->id);
        }
    }
}

HarpListener listener;
std::vector<HUDView*> gViews;
Toolbar* gToolbar = NULL;

void UnProject(GLint x, GLint y, GLint z, GLdouble* pos3D_x, GLdouble* pos3D_y, GLdouble* pos3D_z)
{
    // arrays to hold matrix information
    GLdouble model_view[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
    
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    
    GLint viewport[4];// = { 0, 0, Environment::instance().screenH, Environment::instance().screenW };
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    gluUnProject(x, y, z,
                 model_view, projection, viewport,
                 pos3D_x, pos3D_y, pos3D_z);
    //printf("3d %f %f %f\n", pos3D_x, pos3D_y, pos3D_z);

}

void Mouse(int button, int state, int x, int y)
{
    GLdouble pos3D_x, pos3D_y, pos3D_z;
    UnProject(x, y, 0.01, &pos3D_x, &pos3D_y, &pos3D_z);
    for (HUDView* v : gViews)
        v->mouse(button, state, pos3D_x, pos3D_y);
}

void Motion(int x, int y)
{
    GLdouble pos3D_x, pos3D_y, pos3D_z;
    UnProject(x, y, 0.01, &pos3D_x, &pos3D_y, &pos3D_z);
    for (HUDView* v : gViews)
        v->motion(pos3D_x, pos3D_y);
}

void PassiveMotion(int x, int y)
{
    GLdouble pos3D_x, pos3D_y, pos3D_z;
    UnProject(x, y, 0.01, &pos3D_x, &pos3D_y, &pos3D_z);
    for (HUDView* v : gViews)
        v->passiveMotion(pos3D_x, pos3D_y);
}

///////////////////////////////////////////////////////////////////////
// Reset flags as appropriate in response to menu selections
void ProcessMenu(int value)
{
    switch(value)
    {
        case 1:
            // Turn on antialiasing, and give hint to do the best
            // job possible.
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            glEnable(GL_POINT_SMOOTH);
            glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glEnable(GL_POLYGON_SMOOTH);
            glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
            break;
            
        case 2:
            // Turn off blending and all smoothing
            glDisable(GL_BLEND);
            glDisable(GL_LINE_SMOOTH);
            glDisable(GL_POINT_SMOOTH);
            break;
            
        default:
            break;
    }
    
    // Trigger a redraw
    glutPostRedisplay();
}

void DrawWireFramedBatch(GLTriangleBatch* pBatch)
{
    
    GLfloat vGreen [] = { 0.f, 1.f, 0.f, 1.f };
    //Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), vGreen);
    glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), vGreen);
    pBatch->Draw();
//    // Draw black outline
//    glPolygonOffset(-1.0f, -1.0f);
//    glEnable(GL_LINE_SMOOTH);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_POLYGON_OFFSET_LINE);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    glLineWidth(1.f);
//    GLfloat vBlue [] = { 0.f, 0.f, 1.f, 1.f };
//    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), vBlue);
//    pBatch->Draw();
    
    // Restore polygon mode and depht testing
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}


///////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{
    // Clear the window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    

    double dviewport[4];
    glGetDoublev(GL_VIEWPORT, dviewport);
    GLint viewport[4];
    viewport[0] = (GLint) dviewport[0];
    viewport[1] = (GLint) dviewport[1];
    viewport[2] = (GLint) dviewport[2];
    viewport[3] = (GLint) dviewport[3];
    FingerView* fv = NULL;
    for (auto iter : gFingers)
    {
        fv = iter.second;
        if (fv->inUse)
            break;
    }
    
    if (fv)
    {
        M3DVector2f win;
        M3DVector3f origin;
        fv->objectFrame.GetOrigin(origin);
        m3dProjectXY(win,
                 Environment::instance().transformPipeline.GetModelViewMatrix(),
                 Environment::instance().transformPipeline.GetProjectionMatrix(),
                 viewport,
                 origin);
        //printf("win %f %f\n", win[0], win[1]);
        for (HUDView* v : gViews)
            v->passiveMotion(win[0], win[1]);
    }
    
    glDisable(GL_DEPTH_TEST);
    
//    Environment::instance().modelViewMatrix.PushMatrix();
//    M3DMatrix44f mCamera;
//    cameraFrame.GetCameraMatrix(mCamera);
//    Environment::instance().modelViewMatrix.MultMatrix(mCamera);


    for (HUDView* v : gViews)
        v->draw();
    
//    Environment::instance().modelViewMatrix.PopMatrix();

    glEnable(GL_DEPTH_TEST);

    for (auto iter : gFingers)
        if (iter.second->inUse)
            iter.second->draw();
    
    for (StringView* sv : gStrings)
        sv->draw();

    // Swap buffers
    glutSwapBuffers();
}

void layoutStrings()
{
    float aspectRatio = Environment::screenW / (float)Environment::screenH;
    gStringWidth = (2.f * aspectRatio) / (float)NUM_STRINGS;
    float pos = -aspectRatio + gStringWidth;
    float step = gStringWidth;
    for (StringView* sv : gStrings)
    {
        sv->objectFrame.SetOrigin(pos, 0, -12);
        pos += step;
        sv->updateStringBg();
    }
}

// This function does any needed initialization on the rendering
// context.
void SetupRC()
{
    //cameraFrame.MoveForward(-15.0f);
    
    for (int i = 0; i < 50; ++i)
    {
        FingerView* fv = new FingerView;
        gFingers.insert(std::make_pair(i, fv));
        fv->id = i;
    }
    
    gStringWidth = 2.f / NUM_STRINGS;
    float step = gStringWidth;
    float aspectRatio = Environment::screenW / (float)Environment::screenH;
    float pos = -1 + gStringWidth;
    for (int i = 0; i < NUM_STRINGS; ++i)
    {
        StringView* sv = new StringView;
        sv->setup();
        sv->objectFrame.TranslateWorld(pos, 0, -12);
        sv->stringNum = i;
        gStrings.push_back(sv);
        pos += step;
    }
    
    for (auto iter : gFingers)
        iter.second->setup();

    glEnable(GL_DEPTH_TEST);
    Environment::instance().shaderManager.InitializeStockShaders();
    
    Toolbar* tb = new Toolbar;
//    tb->setBounds(HUDRect(0,500,800,100));
    gViews.push_back(tb);
    gToolbar = tb;
    
//    for (HUDView* v : gViews)
//        v->setup();
    
    // Black background
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f );
}



void ChangeSize(int w, int h)
{
    // Prevent a divide by zero
    if(h == 0)
        h = 1;
    
    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);
    
    Environment::instance().screenW = w;
    Environment::instance().screenH = h;
    
    gToolbar->setBounds(HUDRect(-1,.8,2,.2));
    
    layoutStrings();
    
    // Establish clipping volume (left, right, bottom, top, near, far)
//    viewFrustum.SetOrthographic(0, w, 0.0f, h, 800.0f, -800.0f);
//    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
//    Environment::instance().transformPipeline.SetMatrixStacks(Environment::instance().modelViewMatrix, projectionMatrix);

    Environment::instance().transformPipeline.SetMatrixStacks(Environment::instance().modelViewMatrix, projectionMatrix);
	viewFrustum.SetPerspective(10.0f, float(w)/float(h), 0.01f, 500.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
//	Environment::instance().modelViewMatrix.LoadIdentity();
}

void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
		//objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
        cameraFrame.TranslateWorld(0, 0, .1);
	if(key == GLUT_KEY_DOWN)
		//objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
        cameraFrame.TranslateWorld(0, 0, -.1);
	
//	if(key == GLUT_KEY_LEFT)
//		objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
//    
//	if(key == GLUT_KEY_RIGHT)
//		objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    
	// Refresh the Window
	glutPostRedisplay();
}

static void idle(void)
{
    
    for (StringView* sv : gStrings)
        sv->update();
    glutPostRedisplay();
    usleep(10000);
}

int main(int argc, char* argv[])
{
    RtAudioDriver driver(BUFFER_SIZE);
    for (int i = 0; i < NUM_STRINGS-1; ++i)
        Harp::GetInstance()->AddString();
    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("AirHarp");
	
	// Create the Menu
	glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("Antialiased Rendering",1);
	glutAddMenuEntry("Normal Rendering",2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutIdleFunc(&idle);
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutPassiveMotionFunc(PassiveMotion);
    glutSpecialFunc(SpecialKeys);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
	SetupRC();
    
    Leap::Controller controller;
    controller.addListener(listener);
    
	glutMainLoop();
    
    for (HUDView* v : gViews)
        delete v;
    gViews.clear();
    
	return 0;
}
