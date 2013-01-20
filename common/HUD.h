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
#include "FingerView.h"

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

class HUDView : public FingerView::Listener
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
    virtual void mouseDown(float x, float y);
    virtual void motion(float x, float y);
    virtual void passiveMotion(float x, float y);
    virtual void setBounds(const HUDRect& b);
    virtual void loadTextures();
    // FingerView::Listener override
    virtual void updatePointedState(FingerView* fv);
protected:
    bool trackingMouse;
    HUDRect bounds;
    bool hover;
    
private:
    std::vector<HUDView*> children;
    HUDView* parent;
    GLBatch defaultBatch;
    std::vector<FingerView*> fingers;
};

class HUDButton : public HUDView
{
public:
    HUDButton(int id = -1);
    void draw();
    void setup();
    void mouseDown(float x, float y);
    void setState(bool state, bool broadcast = false);
    bool getState() const { return state; }
    int getId() const { return buttonId; }
    void loadTextures();
    class Listener
    {
    public:
        virtual void buttonStateChanged(HUDButton* b) = 0;
    };

    void addListener(Listener* l);
    void removeListener(Listener* l);
    
    // FingerView::Listener override
    virtual void updatePointedState(FingerView* fv);
    
private:
    bool state = false;
    std::vector<Listener*> listeners;
    GLfloat offColor[4];
    GLfloat onColor[4];
    GLfloat hoverOffColor[4];
    GLfloat hoverOnColor[4];
    GLBatch batch;
    int prevNumPointers = 0;
    int buttonId;
    GLuint onTextureID;
    GLuint offTextureID;
    float fade = 0.f;
};

#endif