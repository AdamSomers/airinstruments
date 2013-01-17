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
#include <cmath>

#include "HUD.h"
#include "HarpHUD.h"
#include "Environment.h"
#include "GfxTools.h"

#include "AudioServer.h"
#include "Harp.h"

#include "MotionServer.h"
#include "FingerView.h"
#include "StringView.h"

#define SCREEN_X        800
#define SCREEN_Y        600

#define BUFFER_SIZE 512
#define NUM_STRINGS 24

std::vector<StringView*> gStrings;
std::vector<HUDView*> gViews;
Toolbar* gToolbar = NULL;
StatusBar* gStatusBar = NULL;

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
    //UnProject(x, y, 0.01, &pos3D_x, &pos3D_y, &pos3D_z);
    for (HUDView* v : gViews)
        v->mouse(button, state, x, y);
}

void Motion(int x, int y)
{
    GLdouble pos3D_x, pos3D_y, pos3D_z;
    //UnProject(x, y, 0.01, &pos3D_x, &pos3D_y, &pos3D_z);
    for (HUDView* v : gViews)
        v->motion(x, y);
}

void PassiveMotion(int x, int y)
{
    GLdouble pos3D_x, pos3D_y, pos3D_z;
    //UnProject(x, y, 0.01, &pos3D_x, &pos3D_y, &pos3D_z);
    for (HUDView* v : gViews)
        v->passiveMotion(x, y);
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
    
//    Environment::instance().modelViewMatrix.PushMatrix();
//    M3DMatrix44f mCamera;
//    Environment::instance().cameraFrame.GetCameraMatrix(mCamera);
//    Environment::instance().modelViewMatrix.MultMatrix(mCamera);
//    Environment::instance().modelViewMatrix.PopMatrix();

	Environment::instance().viewFrustum.SetPerspective(10.0f, float(Environment::instance().screenW)/float(Environment::instance().screenH), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    Environment::instance().modelViewMatrix.LoadIdentity();

    glEnable(GL_DEPTH_TEST);
    
    for (StringView* sv : gStrings)
        sv->draw();

	Environment::instance().viewFrustum.SetOrthographic(0, Environment::instance().screenW, 0.0f, Environment::instance().screenH, 800.0f, -800.0f);
	Environment::instance().modelViewMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    
    glDisable(GL_DEPTH_TEST);
    
    for (HUDView* v : gViews)
        v->draw();
    
    glEnable(GL_DEPTH_TEST);
    
    Environment::instance().viewFrustum.SetPerspective(10.0f, float(Environment::instance().screenW)/float(Environment::instance().screenH), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    Environment::instance().modelViewMatrix.LoadIdentity();
    
    for (auto iter : MotionDispatcher::instance().fingerViews)
        if (iter.second->inUse)
            iter.second->draw();
    
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
    glEnable(GL_MULTISAMPLE);

    //Environment::instance().cameraFrame.MoveForward(-15.0f);

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
    
    

    glEnable(GL_DEPTH_TEST);
    Environment::instance().shaderManager.InitializeStockShaders();
    
    Toolbar* tb = new Toolbar;
    gViews.push_back(tb);
    gToolbar = tb;
    
    StatusBar* sb = new StatusBar;
    gViews.push_back(sb);
    gStatusBar = sb;
    
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
    
    //gToolbar->setBounds(HUDRect(-1,.8,2,.2));
    gToolbar->setBounds(HUDRect(0,h-50,w,50));
    gStatusBar->setBounds(HUDRect(0,0,w,20));
    
    layoutStrings();

    Environment::instance().transformPipeline.SetMatrixStacks(Environment::instance().modelViewMatrix, Environment::instance().projectionMatrix);
	Environment::instance().viewFrustum.SetPerspective(10.0f, float(w)/float(h), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
	Environment::instance().modelViewMatrix.LoadIdentity();
    
    Environment::instance().ready = true;
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
    usleep(30000);
}

int mainn(int argc, char* argv[])
{
    RtAudioDriver driver(BUFFER_SIZE);
    for (int i = 0; i < NUM_STRINGS-1; ++i)
        Harp::GetInstance()->AddString();
    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
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
#if 0
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
#endif
    
	SetupRC();
    
	glutMainLoop();
    
    for (HUDView* v : gViews)
        delete v;
    gViews.clear();
    
	return 0;
}
