#include <GLTools.h>
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
#include "FingerView.h"
#include "GfxTools.h"
#include "StringView.h"

#define SCREEN_X        800
#define SCREEN_Y        600

#define BUFFER_SIZE 512
#define NUM_STRINGS 24

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
    
    for (FingerView::Listener* listener : gStrings)
    {
        listener->needsReset = true;
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
                    
                    for (FingerView::Listener* listener : gStrings)
                    {
                        listener->updatePointedState(fv);
                    }
                    //printf("%1.2f %1.2f %1.2f\n", scaledX,scaledY,scaledZ);
                }
            }
        }
    }
    
    for (FingerView::Listener* listener : gStrings)
    {
        if (listener->needsReset) {
            listener->reset();
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
//    Environment::instance().cameraFrame.GetCameraMatrix(mCamera);
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
    //Environment::instance().cameraFrame.MoveForward(-15.0f);
    
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

    Environment::instance().transformPipeline.SetMatrixStacks(Environment::instance().modelViewMatrix, Environment::instance().projectionMatrix);
	Environment::instance().viewFrustum.SetPerspective(10.0f, float(w)/float(h), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
//	Environment::instance().modelViewMatrix.LoadIdentity();
}

void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
		//objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
        Environment::instance().cameraFrame.TranslateWorld(0, 0, .1);
	if(key == GLUT_KEY_DOWN)
		//objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
        Environment::instance().cameraFrame.TranslateWorld(0, 0, -.1);
	
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
