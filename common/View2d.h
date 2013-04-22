//
//  View2d.h
//  AirBeats
//
//  Created by Adam Somers on 4/22/13.
//
//

#ifndef __AirBeats__View2d__
#define __AirBeats__View2d__

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

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

class View2d
{
public:
    View2d();
    virtual ~View2d();
    
    virtual void draw();
    virtual void update() {}
    virtual void setup();
    virtual void boundsChanged();
    virtual void setBounds(const HUDRect& b);
    const HUDRect& getBounds() const { return bounds; }
    virtual void loadTextures();
    void setDefaultTexture(GLuint texture);
    void setDefaultColor(GLfloat* color);
    
protected:
    HUDRect bounds;
    bool didSetup;
    GLBatch defaultBatch;
    
private:
    GLuint defaultTexture;
    GLfloat defaultColor[4];
    bool defaultColorSet;
};

#endif /* defined(__AirBeats__View2d__) */
