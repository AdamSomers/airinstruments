#include "HUD.h"

HUDView::HUDView()
: parent(NULL)
, trackingMouse(false)
, hover(false)
{
}

void HUDView::addChild(HUDView* child)
{
    children.push_back(child);
    child->setParent(this);
}

void HUDView::setParent(HUDView* p)
{
    parent = p;
}

void HUDView::draw()
{
#if 0
    HUDRect parentBounds(0,0,0,0);
    if (parent)
        parentBounds = parent->bounds;
    Environment::instance().modelViewMatrix.PushMatrix(viewFrame);
    Environment::instance().modelViewMatrix.Translate(parentBounds.x, parentBounds.y, 0);
    GLfloat vRed [] = { 1.f, 0.f, 0.f, 0.f };
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), vRed);
    glLineWidth(1.f);
    defaultBatch.Draw();
    Environment::instance().modelViewMatrix.PopMatrix();
#endif
    for (HUDView* v : children)
    {
        Environment::instance().modelViewMatrix.PushMatrix();
        Environment::instance().modelViewMatrix.Translate(bounds.x + v->bounds.x, bounds.y + v->bounds.y, 0);
        v->draw();
        Environment::instance().modelViewMatrix.PopMatrix();
    }
}

void HUDView::setup()
{
    M3DVector3f verts[4] = {
        bounds.x, bounds.y, 0.f,
        bounds.x + bounds.w, bounds.y, 0.f,
        bounds.x, bounds.y + bounds.h, 0.f,
        bounds.x + bounds.w, bounds.y + bounds.h, 0.f
    };
    
    M3DVector3f normals[4] = {
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f
    };
    
    defaultBatch.Begin(GL_TRIANGLE_STRIP, 4);
    defaultBatch.CopyVertexData3f(verts);
    defaultBatch.CopyNormalDataf(normals);
    defaultBatch.End();
}

void HUDView::boundsChanged()
{
    setup();
}

void HUDView::mouseDown(float x, float y)
{
    y = Environment::instance().screenH - y;
    for (HUDView* child : children)
    {
        float localX = x - bounds.x;
        float localY = y - bounds.y;
        //printf("%f %f %f %f %f %f\n", x, y, localX, localY, child->bounds.x,child->bounds.y);
        if (child->bounds.contains(localX,localY) || child->trackingMouse) {
            child->mouseDown(localX, localY);
        }
    }
}

void HUDView::motion(float x, float y)
{
    y = Environment::instance().screenH - y;
    for (HUDView* child : children)
    {
        float localX = x - bounds.x;
        float localY = y - bounds.y;
        if (child->bounds.contains(localX,localY) || child->trackingMouse)
            child->motion(localX, localY);
    }
}

void HUDView::passiveMotion(float x, float y)
{
    y = Environment::instance().screenH - y;
    for (HUDView* child : children)
    {
        float localX = x - bounds.x;
        float localY = y - bounds.y;
        //printf("%f %f %f %f\n",x, y, localX,localY);
        if (child->bounds.contains(localX,localY))
        {
            if (!child->hover) {
                child->hover = true;
                printf("hover\n");
            }
            child->passiveMotion(localX, localY);
        }
        else {
            if (child->hover) {
                child->hover = false;
                printf("un-hover\n");
            }
        }
    }
}

void HUDView::setBounds(const HUDRect& b)
{
    bounds = b;
    boundsChanged();
}

void HUDView::updatePointedState(FingerView* fv)
{
    M3DVector2f screenPos;
    fv->getScreenPos(screenPos);
    float x = screenPos[0];
    float y = screenPos[1];
    y = Environment::instance().screenH - y;
    for (HUDView* child : children)
    {
        float localX = x - bounds.x;
        float localY = y - bounds.y;
        //printf("%f %f %f %f\n",x, y, localX,localY);
        if (child->bounds.contains(localX,localY))
        {
            child->fingerPointing(fv);
        }
        else
        {
            child->fingerNotPointing(fv);
        }
    }
}

HUDButton::HUDButton(int id)
: buttonId(id)
{
}

void HUDButton::setState(bool inState, bool broadcast)
{
    state = inState;
    if (broadcast)
    {
        for (Listener* l : listeners)
            l->buttonStateChanged(this);
    }
}

void HUDButton::addListener(HUDButton::Listener* l)
{
    auto iter = std::find(listeners.begin(), listeners.end(), l);
    if (iter == listeners.end())
        listeners.push_back(l);
}

void HUDButton::removeListener(HUDButton::Listener* l)
{
    auto iter = std::find(listeners.begin(), listeners.end(), l);
    if (iter != listeners.end())
        listeners.erase(iter);
}

void HUDButton::draw()
{
    GLfloat* color;
    if (state)
    {
        if (hover || pointers.size() > 0)
            color = hoverOnColor;
        else
            color = onColor;
    }
    else
    {
        if (hover || pointers.size() > 0)
            color = hoverOffColor;
        else
            color = offColor;
    }
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), color);
    glLineWidth(1.f);
    batch.Draw();
}

void HUDButton::setup()
{
    HUDView::setup();
    M3DVector3f verts[4] = {
        0, 0, 0.f,
        bounds.w, 0, 0.f,
        0, bounds.h, 0.f,
        bounds.w, bounds.h, 0.f
    };
    
    batch.Begin(GL_TRIANGLE_STRIP, 4);
    batch.CopyVertexData3f(verts);
    //batch.CopyNormalDataf(normals);
    batch.End();
    
    offColor[0] = 0.3f;
    offColor[1] = 0.3f;
    offColor[2] = 0.3f;
    offColor[3] = 1.0f;
    onColor[0] = 0.f;
    onColor[1] = 1.f;
    onColor[2] = 0.f;
    onColor[3] = 1.f;
    hoverOffColor[0] = 0.4f;
    hoverOffColor[1] = 0.4f;
    hoverOffColor[2] = 0.4f;
    hoverOffColor[3] = 1.0f;
    hoverOnColor[0] = 0.f;
    hoverOnColor[1] = .9f;
    hoverOnColor[2] = 0.f;
    hoverOnColor[3] = 1.f;
}

void HUDButton::mouseDown(float x, float y)
{
    setState(!state, true);
}

void HUDButton::updatePointedState(FingerView* fv)
{
    if (prevNumPointers == 0 && pointers.size() > 0)
        setState(!state, true);

    prevNumPointers = pointers.size();
}
