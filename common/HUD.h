#ifndef h_HUD
#define h_HUD

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

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
    
    bool contains(GLfloat _x, GLfloat _y) const
    {
        return _x > x && _y > y && _x < x+w && _y < y+h;
    }
    
    bool operator==(const HUDRect& other) const
    {
        return x == other.x &&
               y == other.y &&
               w == other.w &&
               h == other.h;
    }
    
    bool operator!=(const HUDRect &other) const
    {
        return !(*this == other);
    }
    
    GLfloat top() const { return y + h; }
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

    // Multi-finger interaction methods in screen coords.
    // x, y are FingerView position cooreds projected to screen plane 
    virtual void fingerMotion(float /*x*/, float /*y*/, FingerView* /*fv*/) {}
    virtual void fingerEntered(float /*x*/, float /*y*/, FingerView* /*fv*/) {}
    virtual void fingerExited(float /*x*/, float /*y*/, FingerView* /*fv*/) {}

    virtual void setBounds(const HUDRect& b);
    const HUDRect& getBounds() const { return bounds; }
    virtual void loadTextures();
    // FingerView::Listener override
    virtual void updatePointedState(FingerView* fv);
protected:
    bool trackingMouse;
    HUDRect bounds;
    bool hover;
    bool didSetup;
    
private:
    std::vector<HUDView*> children;
    HUDView* parent;
    GLBatch defaultBatch;
    std::vector<FingerView*> hoveringFingers;
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
    void setTextures(GLuint on, GLuint off);
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
    bool state;
    std::vector<Listener*> listeners;
    GLfloat offColor[4];
    GLfloat onColor[4];
    GLfloat hoverOffColor[4];
    GLfloat hoverOnColor[4];
    GLBatch batch;
    int prevNumPointers;
    int buttonId;
    GLuint onTextureID;
    GLuint offTextureID;
    float fade;
};

#endif