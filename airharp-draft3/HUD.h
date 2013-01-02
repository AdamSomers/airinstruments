#ifndef h_HUD
#define h_HUD

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

#include <iostream>
#include <vector>

#include "Environment.h"

struct HUDRect {
    HUDRect() : x(0.f), y(0.f), w(0.f), h(0.f) {}
    HUDRect(GLfloat _x, GLfloat _y, GLfloat _w, GLfloat _h)
    {
        x = _x; y = _y; w = _w; h = _h;
    }
    bool contains(GLfloat _x, GLfloat _y)
    {
        return _x > x && _y > y && _x < x+w && _y < y+h;
    }
    GLfloat x, y, w , h;
};

class HUDView
{
public:
    HUDView();
    virtual ~HUDView() {}
    void addChild(HUDView* child);
    void setParent(HUDView* p);
    virtual void draw();
    virtual void update() {}
    virtual void setup();
    virtual void boundsChanged();
    virtual void mouse(int button, int state, float x, float y);
    void motion(float x, float y);
    void passiveMotion(float x, float y);
    void setBounds(const HUDRect& b);
    
protected:
    bool trackingMouse;
    HUDRect bounds;
    bool hover;
    
private:
    std::vector<HUDView*> children;
    HUDView* parent;
    GLBatch defaultBatch;
};

class HUDButton : public HUDView
{
public:
    HUDButton();
    void draw();
    void setup();
    void mouse(int button, int state, float x, float y);
    void motion(float x, float y);
    void passiveMotion(float x, float y);
    
private:
    GLfloat offColor[4];
    GLfloat onColor[4];
    GLfloat hoverOffColor[4];
    GLfloat hoverOnColor[4];
    bool state;
    GLBatch batch;
};

class Toolbar : public HUDView
{
public:
    Toolbar();
    // HUDView overrides
    void setup();
    void draw();
    
private:
    HUDButton b1;
    HUDButton b2;
    HUDButton b3;
    HUDButton b4;
    //    HUDSlider s1;
    //    HUDSlider s2;
    //    HUDSwitch sw1;
    //    HUDSwitch sw2;
    GLBatch batch;
};

#endif