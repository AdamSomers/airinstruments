#include "HUD.h"
#include "GfxTools.h"

HUDView::HUDView()
: parent(NULL)
, trackingMouse(false)
, hover(false)
, didSetup(false)
, defaultTexture(0)
, defaultColorSet(false)
{
    defaultColor[0] = 1.f;
    defaultColor[1] = 1.f;
    defaultColor[2] = 1.f;
    defaultColor[3] = 1.f;
}

void HUDView::addChild(HUDView* child)
{
    children.push_back(child);
    child->setParent(this);
}

void HUDView::removeChild(HUDView* child)
{
    auto iter = std::find(children.begin(), children.end(), child);
    if (iter != children.end())
        children.erase(iter);
}

void HUDView::removeAllChildren()
{
    children.clear();
}

void HUDView::setParent(HUDView* p)
{
    parent = p;
}

void HUDView::draw()
{
    if (0 != defaultTexture)
    {
        glBindTexture(GL_TEXTURE_2D, defaultTexture);
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, Environment::instance().transformPipeline.GetModelViewMatrix(), 0);
        defaultBatch.Draw();
    }
    else
    {
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode[0]);
        if (!defaultColorSet)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), defaultColor);
        glLineWidth(1.f);
        defaultBatch.Draw();
        glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
    }

    for (HUDView* v : children)
    {
        Environment::instance().modelViewMatrix.PushMatrix();
        Environment::instance().modelViewMatrix.Translate(bounds.x, bounds.y, 0);
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
    
    M3DVector2f texCoords[4] = {
        0.f, 1.f,
        1.f, 1.f,
        0.f, 0.f,
        1.f, 0.f
    };
    
    if (!didSetup)
        defaultBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    defaultBatch.CopyVertexData3f(verts);
    defaultBatch.CopyTexCoordData2f(texCoords, 0);
    defaultBatch.CopyNormalDataf(normals);
    if (!didSetup)
        defaultBatch.End();
    
    didSetup = true;
    
    for (HUDView* v : children)
        v->setup();
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
                //printf("hover\n");
            }
            child->passiveMotion(localX, localY);
        }
        else {
            if (child->hover) {
                child->hover = false;
                //printf("un-hover\n");
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

    if (x > 0 && x < 2000 )
        ;//printf("ok\n");
    else
        ;//printf("not ok\n");
    if (bounds.contains(x, y))
    {
        auto iter = std::find(hoveringFingers.begin(), hoveringFingers.end(), fv);
        if (iter == hoveringFingers.end())
        {
            fingerEntered(x, y, fv);
            hoveringFingers.push_back(fv);
        }
        fingerMotion(x, y, fv);
    }
    else
    {
        auto iter = std::find(hoveringFingers.begin(), hoveringFingers.end(), fv);
        if (iter != hoveringFingers.end())
        {
            fingerExited(x, y, fv);
            hoveringFingers.erase(iter);
        }
    }

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

void HUDView::loadTextures()
{
    for (HUDView* child : children)
    {
        child->loadTextures();
    }
}

void HUDView::setDefaultTexture(GLuint texture)
{
    defaultTexture = texture;
}

void HUDView::setDefaultColor(GLfloat* color)
{
    memcpy(defaultColor, color, 4 * sizeof(GLfloat));
    defaultColorSet = true;
}

HUDButton::HUDButton(int id)
: buttonId(id)
, state(false)
, prevNumPointers(0)
, fade(0.f)
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
    //Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), color);
    
    GLfloat onTexColor[4] = { 1.f, 1.f, 1.f, fade };
    GLfloat offTexColor[4] = { 1.f, 1.f, 1.f, 1.f - fade };
    
    //int textureID = state ? onTextureID : offTextureID;	// Unused variable
    glBindTexture(GL_TEXTURE_2D, onTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), onTexColor, 0);
    glLineWidth(1.f);
    defaultBatch.Draw();
    glBindTexture(GL_TEXTURE_2D, offTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), offTexColor, 0);
    defaultBatch.Draw();
    
    if (state && fade < 1.f)
    {
        fade += 0.3f;
        if (fade > 1.f) fade = 1.f;
    }
    if (!state && fade > 0.f)
    {
        fade -= 0.11f;
        if (fade < 0.f) fade = 0.f;
    }
}

void HUDButton::setup()
{
    HUDView::setup();
    
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

void HUDButton::mouseDown(float /*x*/, float /*y*/)
{
    setState(!state, true);
}

void HUDButton::updatePointedState(FingerView* /*fv*/)
{
    if (prevNumPointers == 0 && pointers.size() > 0)
        setState(!state, true);

    prevNumPointers = pointers.size();
}

void HUDButton::loadTextures()
{
//    glGenTextures(1, &onTextureID);
//    glBindTexture(GL_TEXTURE_2D, onTextureID);
//    GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::button_small_on_png, BinaryData::button_small_on_pngSize));
//    glGenTextures(1, &offTextureID);
//    glBindTexture(GL_TEXTURE_2D, offTextureID);
//    GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::button_small_off_png, BinaryData::button_small_off_pngSize));
}

void HUDButton::setTextures(GLuint on, GLuint off)
{
    onTextureID = on;
    offTextureID = off;
}
