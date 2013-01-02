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
#include "Environment.h"

#include "AudioServer.h"
#include "Harp.h"

// Array of small stars
#define SMALL_STARS     100
#define MEDIUM_STARS     40
#define LARGE_STARS      15

#define SCREEN_X        800
#define SCREEN_Y        600

#define NUM_STRINGS 20
float gStringWidth = 0.06;

GLFrame viewFrame;

GLMatrixStack projectionMatrix;


GLFrustum viewFrustum;
GLBatch smallStarBatch;
GLBatch mediumStarBatch;
GLBatch largeStarBatch;
GLBatch mountainRangeBatch;
GLBatch moonBatch;
GLFrame moonFrame;

GLFrame	cameraFrame;
GLTriangleBatch sphereBatch;

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
    FingerView() : inUse(false), id(-1)
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
    Leap::Finger finger;
    bool inUse;
    int id;
private:
    GLTriangleBatch     coneBatch;
    GLTriangleBatch     cylinderBatch;
};

class StringView
{
public:
    StringView() : pointed(false)
    {}
    void setup()
    {
        M3DVector3f verts[4] = {
            0.0f, -.8f, 0.0f,
            gStringWidth, -.8f, 0.0f,
            0.f, .8, 0.0f,
            gStringWidth, .8, 0.0f
        };
        
        M3DVector3f normals[4] = {
            0.0f, 0.f, -1.f,
            0.0f, 0.f, -1.f,
            0.0f, 0.f, -1.f,
            0.0f, 0.f, -1.f
        };
        
        bgBatch.Begin(GL_TRIANGLE_STRIP, 4);
        bgBatch.CopyVertexData3f(verts);
        bgBatch.CopyNormalDataf(normals);
        bgBatch.End();
        
        float w = 0.01;
        
        M3DVector3f stringVerts[4] = {
            gStringWidth/2.f - w/2, -.8f, 0.0f,
            gStringWidth/2.f + w/2, -.8f, 0.0f,
            gStringWidth/2.f - w/2, .8, 0.0f,
            gStringWidth/2.f + w/2, .8, 0.0f
        };
        
        stringBatch.Begin(GL_TRIANGLE_STRIP, 4);
        stringBatch.CopyVertexData3f(stringVerts);
        stringBatch.CopyNormalDataf(normals);
        stringBatch.End();
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
    
    void updatePointedState(GLFrame& inFrame)
    {
        M3DVector3f point;
        M3DVector3f ray;
        M3DVector3f center;
        inFrame.GetOrigin(point);
        inFrame.GetForwardVector(ray);
        objectFrame.GetOrigin(center);

        M3DVector3f collisionPoint;
        M3DVector3f pNormal = { 0.f, 0.f, -1.f };
        //m3dNormalizeVector3(pNormal);
        m3dNormalizeVector3(ray);
        collide(point, ray, center, pNormal, collisionPoint);

        if (fabsf(collisionPoint[0] - center[0]) < gStringWidth / 2.f)
        {
            pointed = true;
        }
        else
            pointed = false;
    }
    
    GLFrame objectFrame;
private:
    GLBatch     bgBatch;
    GLBatch     stringBatch;
    bool pointed;
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
        i.second->inUse = false;
    }

    gLock.unlock();
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
                        if (!fv->inUse) {
                            fv->finger = f;
                            fv->inUse = true;
                        }
                    }
                    gLock.unlock();
    
//                    Finger* f = &(*iter).second;
//                    
//                    float prevX = f->normalizedX();
//                    float prevY = f->normalizedY();
//                    float prevZ = f->normalizedZ();
//                    f->finger = finger;
//                    airMotion(f, prevX , prevY, prevZ);

                    float x = fv->normalizedX();
                    float y = fv->normalizedY();
                    float z = fv->normalizedZ();

                    float dirX = f.direction().x;
                    float dirY = f.direction().y;
                    float dirZ = f.direction().z;
                    
                    fv->objectFrame.SetForwardVector(dirX,dirY,dirZ);
                    float scaledX = x*2;
                    float scaledY = (y-.5)*2;
                    if (z < 0) z = 0;
                    float scaledZ = z*5-10;
                    fv->objectFrame.SetOrigin(scaledX,scaledY,scaledZ);
                    
                    for (StringView* sv : gStrings)
                    {
                        sv->updatePointedState(fv->objectFrame);
                    }
                    //printf("%1.2f %1.2f %1.2f\n", scaledX,scaledY,scaledZ);
                }
            }
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
    
    
//    Environment::instance().modelViewMatrix.PushMatrix();
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    squareBatch.Draw();
//    Environment::instance().modelViewMatrix.PopMatrix();
#if 0
    M3DMatrix44f mScreenSpace;
    m3dMakeOrthographicMatrix(mScreenSpace, 0.0f, Environment::instance().screenW, 0.0f, Environment::instance().screenH, 0.0f, 1.0f);
    GLfloat vRed [] = { 1.f, 0.f, 0.f, 0.f };
    M3DMatrix44f m;
    m3dCopyMatrix44(m, Environment::instance().modelViewMatrix.GetMatrix());
    M3DMatrix44f m2;
    m3dMatrixMultiply44(m2, m, mScreenSpace);
    Environment::instance().modelViewMatrix.PushMatrix(m2);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), vRed);
    
    // Everything is white
    GLfloat vWhite [] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), vWhite);

    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Draw small stars
    glPointSize(1.0f);
    smallStarBatch.Draw();
    
    // Draw medium sized stars
    glPointSize(4.0f);
    mediumStarBatch.Draw();
    
    // Draw largest stars
    glPointSize(8.0f);
    largeStarBatch.Draw();
    
   Environment::instance().modelViewMatrix.PushMatrix();
   M3DMatrix44f mMoon;
   moonFrame.GetMatrix(mMoon);
   Environment::instance().modelViewMatrix.MultMatrix(mMoon);
   Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), vRed);
    moonBatch.Draw();
    Environment::instance().modelViewMatrix.PopMatrix();

    // Draw the "moon"
    
    // Draw distant horizon
    glLineWidth(3.5);
    mountainRangeBatch.Draw();
    
    //moonBatch.Draw();
    
    GLfloat vBlue [] = { 0.5f, 0.5f, 1.0f, 1.0f };
//    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, viewFrustum.GetProjectionMatrix(), vBlue);
    
    
//    glDisable(GL_DEPTH_TEST);

    
    Environment::instance().modelViewMatrix.PopMatrix();
#endif
    
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
    glutPostRedisplay();
}


// This function does any needed initialization on the rendering
// context.
void SetupRC()
{
    //cameraFrame.MoveForward(-15.0f);
    
    for (int i = 0; i < 50; ++i)
    {
        gFingers.insert(std::make_pair(i, new
                                       FingerView()));
    }
    
    gStringWidth = 2.f / NUM_STRINGS;
    float totalWidth = gStringWidth * NUM_STRINGS;
    //float xtra = 2.f - totalWidth;
    //float pad = xtra / (float)NUM_STRINGS;
    float step = gStringWidth;
    float pos = -1 + (gStringWidth);
    for (int i = 0; i < NUM_STRINGS; ++i)
    {
        StringView* sv = new StringView;
        sv->setup();
        sv->objectFrame.TranslateWorld(pos, 0, - 10);
        gStrings.push_back(sv);
        pos += step;
    }
    
    for (auto iter : gFingers)
        iter.second->setup();

//    viewFrame.RotateWorld(m3dDegToRad(180.f), 0.0f, 1.0f, 0.0f);
    M3DVector3f vVerts[SMALL_STARS];       // SMALL_STARS is the largest batch we are going to need
    int i;
    
    glEnable(GL_DEPTH_TEST);
    Environment::instance().shaderManager.InitializeStockShaders();
    
    // Populate star list
    smallStarBatch.Begin(GL_POINTS, SMALL_STARS);
    for(i = 0; i < SMALL_STARS; i++)
    {
        vVerts[i][0] = (GLfloat)(rand() % SCREEN_X);
        vVerts[i][1] = (GLfloat)(rand() % (SCREEN_Y - 100)) + 100.0f;
        vVerts[i][2] = 0.0f;
    }
    smallStarBatch.CopyVertexData3f(vVerts);
    smallStarBatch.End();
    
    // Populate star list
    mediumStarBatch.Begin(GL_POINTS, MEDIUM_STARS);
    for(i = 0; i < MEDIUM_STARS; i++)
    {
        vVerts[i][0] = (GLfloat)(rand() % SCREEN_X);
        vVerts[i][1] = (GLfloat)(rand() % (SCREEN_Y - 100)) + 100.0f;
        vVerts[i][2] = 0.0f;
    }
    mediumStarBatch.CopyVertexData3f(vVerts);
    mediumStarBatch.End();
    
    // Populate star list
    largeStarBatch.Begin(GL_POINTS, LARGE_STARS);
    for(i = 0; i < LARGE_STARS; i++)
    {
        vVerts[i][0] = (GLfloat)(rand() % SCREEN_X);
        vVerts[i][1] = (GLfloat)(rand() % (SCREEN_Y - 100)) + 100.0f;
        vVerts[i][2] = 0.0f;
    }
    largeStarBatch.CopyVertexData3f(vVerts);
    largeStarBatch.End();
    
    M3DVector3f vMountains[4] = {
        0.0f, 25.0f, 0.0f,
        50.0f, 100.0f, 0.0f,
        100.0f, 25.0f, 0.0f,
        225.0f, 125.0f, 0.0f };
//        300.0f, 50.0f, 0.0f,
//        375.0f, 100.0f, 0.0f,
//        460.0f, 25.0f, 0.0f,
//        525.0f, 100.0f, 0.0f,
//        600.0f, 20.0f, 0.0f,
//        675.0f, 70.0f, 0.0f,
//        750.0f, 25.0f, 0.0f,
//        800.0f, 90.0f, 0.0f };
    
    M3DVector3f normals[4] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f };
//        0.0f, 0.0f, -1.0f,
//        0.0f, 0.0f, -1.0f,
//        0.0f, 0.0f, -1.0f,
//        0.0f, 0.0f, -1.0f,
//        0.0f, 0.0f, -1.0f,
//        0.0f, 0.0f, -1.0f,
//        0.0f, 0.0f, -1.0f,
//        0.0f, 0.0f, -1.0f };
    
    M3DVector4f colors[4] = {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f };
//        1.0f, 1.0f, 1.0f, 1.0f,
//        1.0f, 1.0f, 1.0f, 1.0f,
//        1.0f, 1.0f, 1.0f, 1.0f,
//        1.0f, 1.0f, 1.0f, 1.0f,
//        1.0f, 1.0f, 1.0f, 1.0f,
//        1.0f, 1.0f, 1.0f, 1.0f,
//        1.0f, 1.0f, 1.0f, 1.0f,
//        1.0f, 1.0f, 1.0f, 1.0f };
    
    mountainRangeBatch.Begin(GL_LINE_STRIP, 4);
    mountainRangeBatch.CopyVertexData3f(vMountains);
    mountainRangeBatch.CopyNormalDataf(normals);
    mountainRangeBatch.CopyColorData4f(colors);
    mountainRangeBatch.End();
    
    // The Moon
    GLfloat x = 0.0f;     // Location and radius of moon
    GLfloat y = 0.0f;
    GLfloat r = 10.0f;
    GLfloat angle = 0.0f;   // Another looping variable
    
    moonBatch.Begin(GL_TRIANGLE_FAN, 34);
    int nVerts = 0;
    vVerts[nVerts][0] = x;
    vVerts[nVerts][1] = y;
    vVerts[nVerts][2] = 0.0f;
    for(angle = 0; angle < 2.0f * 3.141592f; angle += 0.2f) {
        nVerts++;
        vVerts[nVerts][0] = x + float(cos(angle)) * r;
        vVerts[nVerts][1] = y + float(sin(angle)) * r;
        vVerts[nVerts][2] = 0.0f;
    }
    nVerts++;
    
    vVerts[nVerts][0] = x + r;;
    vVerts[nVerts][1] = y;
    vVerts[nVerts][2] = 0.0f;
    moonBatch.CopyVertexData3f(vVerts);
    moonBatch.End();
    
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

int main(int argc, char* argv[])
{
    RtAudioDriver driver(256);
    for (int i = 0; i < NUM_STRINGS-1; ++i)
        Harp::GetInstance()->AddString();
    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Smoothing Out The Jaggies");
	
	// Create the Menu
	glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("Antialiased Rendering",1);
	glutAddMenuEntry("Normal Rendering",2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
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
