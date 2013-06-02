#include "HUD.h"
#include "GfxTools.h"

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
    View2d::draw();

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
    View2d::setup();
    
    for (HUDView* v : children)
        v->setup();
}


void HUDView::loadTextures()
{
    for (HUDView* child : children)
    {
        child->loadTextures();
    }
}

void HUDView::setVisible(bool shouldBeVisible, int fadeTimeMs /*= 500*/)
{
    View2d::setVisible(shouldBeVisible, fadeTimeMs);

    for (HUDView* child : children)
    {
        child->setVisible(shouldBeVisible, fadeTimeMs);
    }
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

void HUDView::updateCursorState(float x, float y)
{
    if (bounds.contains(x, y))
    {
        if (!cursorInside)
        {
            cursorInside = true;
            cursorEntered(x, y);
        }
        cursorMoved(x, y);
    }
    else
    {
        if (cursorInside)
        {
            cursorExited(x, y);
            cursorInside = false;
        }
    }
    
    for (HUDView* child : children)
    {
        float localX = x - bounds.x;
        float localY = y - bounds.y;
        //printf("%f %f %f %f\n",x, y, localX,localY);
        if (child->bounds.contains(localX,localY) && !child->cursorInside)
        {
            child->cursorEntered(localX, localY);
            child->cursorInside = true;
        }
        else if (!child->bounds.contains(localX,localY) && child->cursorInside)
        {
            child->cursorExited(localX, localY);
            child->cursorInside = false;

        }
    }
}

HUDButton::HUDButton(int id)
: buttonId(id)
, state(false)
, prevNumPointers(0)
, fade(0.f)
, hoverTimeout(750)
, enabled(true)
, buttonType(kToggle)
{
    // set a transparent color for the background
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.f };
    setDefaultColor(color);
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
    HUDView::draw();

    if (isTimerRunning())
        setup();
    
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

    // scale RGB components by A component (premultiplied alpha)
    float a = fade * opacity;
    GLfloat onTexColor[4] = { onColor[0] * a, onColor[1] * a, onColor[2] * a, a};
    a = (1.f - fade) * opacity;
    GLfloat offTexColor[4] = { offColor[0] * a, offColor[1] * a, offColor[2] * a, a};
    
    glBindTexture(GL_TEXTURE_2D, offTextureDesc.textureId);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), offTexColor, 0);
    defaultBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, onTextureDesc.textureId);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), onTexColor, 0);
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
    
    if (enabled && isTimerRunning()) {
        GLfloat circleColor[4] = { 1.f, 1.f, 1.f, 1.f };
        //Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), circleColor);
        glBindTexture(GL_TEXTURE_2D, ringTextureDesc.textureId);
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), circleColor, 0);
        circleBatch.Draw();
    }
}

void HUDButton::setup()
{
    offColor[0] = 1.f;
    offColor[1] = 1.f;
    offColor[2] = 1.f;
    offColor[3] = 1.f;
    onColor[0] = 1.f;
    onColor[1] = 1.f;
    onColor[2] = 1.f;
    onColor[3] = 1.f;
    hoverOffColor[0] = 0.4f;
    hoverOffColor[1] = 0.4f;
    hoverOffColor[2] = 0.4f;
    hoverOffColor[3] = 1.0f;
    hoverOnColor[0] = 0.f;
    hoverOnColor[1] = .9f;
    hoverOnColor[2] = 0.f;
    hoverOnColor[3] = 1.f;
    
    const int numVerts = 51;
    M3DVector3f verts[numVerts];
    M3DVector2f texCoords[numVerts];

    M3DVector3f middle = { bounds.w / 2.f, bounds.h / 2.f, 0.f };
    float r = m3dGetVectorLength3(middle) + 5;
    
    verts[0][0] = bounds.x + bounds.w / 2.f;
    verts[0][1] = bounds.y + bounds.h / 2.f;
    verts[0][2] = 0.f;
    texCoords[0][0] = 0.5f;
    texCoords[0][1] = 0.5f;
    
    for (int i = 1; i < numVerts; ++i)
    {
        RelativeTime elapsed = Time::getCurrentTime() - lastTimerStartTime;
        float progress = elapsed.inMilliseconds() / (float)hoverTimeout;
        float phase = -1.f * ((i - 1) / (float)(numVerts-2)) * progress;
        float offset = 3.14159f / 2.f;
        verts[i][0] = verts[0][0] + r * cosf(offset + 2*3.14159f*phase);
        verts[i][1] = verts[0][1] + r * sinf(offset + 2*3.14159f*phase);
        verts[i][2] = 0;
        texCoords[i][0] = 0.5f + 0.5f * cosf(-offset + 2*3.14159f*-phase);
        texCoords[i][1] = 0.5f + 0.5f * sinf(-offset + 2*3.14159f*-phase);
    }
    
    if (!didSetup)
        circleBatch.Begin(GL_TRIANGLE_FAN, numVerts, 1);
    circleBatch.CopyVertexData3f(verts);
    circleBatch.CopyTexCoordData2f(texCoords, 0);
    if (!didSetup)
        circleBatch.End();
    
    HUDView::setup();
}

void HUDButton::mouseDown(float /*x*/, float /*y*/)
{
    if (!isVisible())
        return;
    setState(!state, true);
}

void HUDButton::updatePointedState(FingerView* /*fv*/)
{
    if (!isVisible())
        return;
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

void HUDButton::setTextures(TextureDescription on, TextureDescription off)
{
    onTextureDesc = on;
    offTextureDesc = off;
    TextureDescription desc;
    desc.texX = on.texX;
    desc.texY = on.texY;
    desc.texW = on.texW;
    desc.texH = on.texH;
    setDefaultTexture(desc);
}

void HUDButton::setRingTexture(TextureDescription tex)
{
    ringTextureDesc = tex;
}

void HUDButton::setTimeout(int newTimeout)
{
    hoverTimeout = newTimeout;
}

void HUDButton::cursorEntered(float, float)
{
    if (!isVisible())
        return;
    lastTimerStartTime = Time::getCurrentTime();
    startTimer(hoverTimeout);
    //Logger::outputDebugString("Entered");
}

void HUDButton::cursorExited(float, float)
{
    stopTimer();
    //Logger::outputDebugString("Exited");
}

void HUDButton::timerCallback()
{
    //Logger::outputDebugString("Boom");
    if (!enabled)
        return;

    setState(!getState(), true);
    lastTimerStartTime = Time::getCurrentTime();
    if (buttonType != kMomentary)
        stopTimer();
}

void HUDButton::setOnColor(GLfloat *color)
{
    memcpy(onColor, color, 4 * sizeof(GLfloat));
}

void HUDButton::setOffColor(GLfloat *color)
{
    memcpy(offColor, color, 4 * sizeof(GLfloat));
}

void HUDButton::setEnabled(bool shouldBeEnabled)
{
    enabled = shouldBeEnabled;
}
