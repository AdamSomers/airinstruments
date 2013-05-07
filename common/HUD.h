#ifndef h_HUD
#define h_HUD

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#include <iostream>
#include <vector>

#include "View2d.h"
#include "Environment.h"
#include "FingerView.h"
#include "CursorView.h"

// HUDView is hierarchical 2d UI element that has the following properties:
//
//  - rectangular mesh contained within the given bounds
//  - optional default color that, when set, is used to fill the rectangle
//  - if color is not set, rectangle is drawn in wireframe
//  - optional texture that, when set, is applied to rectangle (overrides color)
//
// Subclasses that need anything different than the above should override setup() and draw().
//
// Child HUDView objects can be added and their bounds set relative to the parent.
//
class HUDView : public View2d
              , public FingerView::Listener
              , public CursorView::Listener
{
public:
    HUDView();
    virtual ~HUDView() {}
    
    // View2d overrides
    virtual void draw();
    virtual void setup();
    virtual void loadTextures();
    
    void addChild(HUDView* child);
    void removeChild(HUDView* child);
    void removeAllChildren();
    void setParent(HUDView* p);
    HUDView* const getParent() { return parent; }
    
    virtual void mouseDown(float x, float y);
    virtual void motion(float x, float y);
    virtual void passiveMotion(float x, float y);
    
    // FingerView::Listener override
    virtual void updatePointedState(FingerView* fv);
    
    // CursorView::Listener override
    virtual void updateCursorState(float x, float y);

    // Multi-finger interaction methods in screen coords.
    // x, y are FingerView position cooreds projected to screen plane 
    virtual void fingerMotion(float /*x*/, float /*y*/, FingerView* /*fv*/) {}
    virtual void fingerEntered(float /*x*/, float /*y*/, FingerView* /*fv*/) {}
    virtual void fingerExited(float /*x*/, float /*y*/, FingerView* /*fv*/) {}
    
    virtual void cursorMoved(float /*x*/, float /*y*/) {}
    virtual void cursorEntered(float /*x*/, float /*y*/) {}
    virtual void cursorExited(float /*x*/, float /*y*/) {}

protected:
    bool trackingMouse;
    bool hover;
    bool cursorInside;

private:
    std::vector<HUDView*> children;
    HUDView* parent;
    std::vector<FingerView*> hoveringFingers;
};

class HUDButton : public HUDView, public Timer
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
    void setRingTexture(GLuint tex);
    void loadTextures();
    void setTimeout(int newTimeout);
    void setOnColor(GLfloat* color);
    void setOffColor(GLfloat* color);
    void setEnabled(bool shouldBeEnabled);
    void setButtonType(int type) { buttonType = type; };
    
    enum Type
    {
        kToggle = 0,
        kMomentary
    };
    
    class Listener
    {
    public:
        virtual void buttonStateChanged(HUDButton* b) = 0;
    };

    void addListener(Listener* l);
    void removeListener(Listener* l);
    
    // FingerView::Listener override
    virtual void updatePointedState(FingerView* fv);
    
    virtual void updateCursorState(float, float) {}
    virtual void cursorEntered(float x, float y);
    virtual void cursorExited(float x, float y);
    
    void timerCallback();
    
private:
    bool state;
    std::vector<Listener*> listeners;
    GLfloat offColor[4];
    GLfloat onColor[4];
    GLfloat hoverOffColor[4];
    GLfloat hoverOnColor[4];
    int prevNumPointers;
    int buttonId;
    GLuint onTextureID;
    GLuint offTextureID;
    float fade;
    GLBatch circleBatch;
    Time lastTimerStartTime;
    GLuint ringTextureID;
    int hoverTimeout;
    bool enabled;
    int buttonType;
};

#endif