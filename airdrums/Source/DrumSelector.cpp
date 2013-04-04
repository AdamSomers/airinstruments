#include "DrumSelector.h"
#include "Drums.h"
#include "GfxTools.h"
#include "KitManager.h"
#include "SkinManager.h"

DrumSelector::DrumSelector()
: selection(0)
, needsLayout(false)
, trackedFinger(nullptr)
{
    for (int i = 0; i < 16; ++i)
    {
        Icon* icon = new Icon(i);
        icons.push_back(icon);
        addChild(icon);
    }
}

DrumSelector::~DrumSelector()
{
}

void DrumSelector::setBounds(const HUDRect& b)
{
    HUDView::setBounds(b);
    layoutIcons();
}

void DrumSelector::layoutIcons()
{
    int N = icons.size();

    const int selectedIconWidth = (int) getBounds().h;
    const int selectedIconHeight = (int) getBounds().h;
    const float iconWidth = (getBounds().w-selectedIconWidth) / (float)(N - (N%2));
    const float iconHeight = iconWidth;

    int first = (selection + N / 2) % N;
    int i = first;
    for (int count = 0; count < N; ++count)
    {
        
        Icon* icon = icons.at(i);
        if (count < N/2)
            icon->setBounds(HUDRect(iconWidth*count + 10, getBounds().h / 2 - iconHeight / 2, iconWidth, iconHeight));
        else if (count > N/2)
            icon->setBounds(HUDRect(iconWidth*(count-1) + selectedIconWidth + 10, getBounds().h / 2 - iconHeight / 2, iconWidth, iconHeight));
        else
            icon->setBounds(HUDRect(getBounds().w / 2 - selectedIconWidth  / 2 + 10, getBounds().h / 2 - selectedIconHeight / 2, (GLfloat) selectedIconWidth, (GLfloat) selectedIconHeight));
        i = (i+1) % N;
    }
}

void DrumSelector::setup()
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
        batch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    batch.CopyVertexData3f(verts);
    batch.CopyTexCoordData2f(texCoords, 0);
    batch.CopyNormalDataf(normals);
    if (!didSetup)
        batch.End();

    HUDView::setup();
}

void DrumSelector::draw()
{
    if (needsLayout) {
        layoutIcons();
        needsLayout = false;
    }

    GLfloat color[4] = { 1.f, 1.f, 1.f, 1.f };

    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), color);
    GLint polygonMode;
    glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.f);
    //batch.Draw();
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    GLuint textureId = SkinManager::instance().getSelectedSkin().getTexture("DrumSelectorBg");
    glBindTexture(GL_TEXTURE_2D, textureId);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, Environment::instance().transformPipeline.GetModelViewMatrix(), 0);
    batch.Draw();
    
    HUDView::draw();

}


void DrumSelector::loadTextures()
{
}

void DrumSelector::setSelection(int sel)
{
    if (sel < 0) sel = icons.size() - 1;
    if (sel >= (int) icons.size()) sel = 0;
    selection = sel;
    needsLayout = true;
}

void DrumSelector::fingerMotion(float x, float y, FingerView* fv)
{
    if (fv != trackedFinger)
        return;

    int inc = 0;
    if (x - prevFingerX < -3)
        inc = 1;
    else if (x - prevFingerX > 3)
        inc = -1;

    if (inc != 0 && !isTimerRunning()) {
        setSelection(selection + inc);
        for (Listener* l : listeners)
            l->drumSelectorChanged(this);
        startTimer(250);
    }

    prevFingerX = x;
    prevFingerY = y;
}

void DrumSelector::fingerEntered(float x, float y, FingerView* fv)
{
    if (!trackedFinger)
        trackedFinger = fv;

    if (fv == trackedFinger)
    {
        prevFingerX = x;
        prevFingerY = y;
    }
}

void DrumSelector::fingerExited(float x, float y, FingerView* fv)
{
    if (fv == trackedFinger)
    {
        trackedFinger = NULL;
        prevFingerX = x;
        prevFingerY = y;
    }
}

void DrumSelector::timerCallback()
{
    stopTimer();
}

void DrumSelector::addListener(DrumSelector::Listener *listener)
{
    auto iter = std::find(listeners.begin(), listeners.end(), listener);
    if (iter == listeners.end())
        listeners.push_back(listener);
}

void DrumSelector::removeListener(DrumSelector::Listener *listener)
{
    auto iter = std::find(listeners.begin(), listeners.end(), listener);
    if (iter != listeners.end())
        listeners.erase(iter);
}

DrumSelector::Icon::Icon(int inId)
: id(inId)
{
}

DrumSelector::Icon::~Icon()
{
}

void DrumSelector::Icon::setup()
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
        batch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    batch.CopyVertexData3f(verts);
    batch.CopyTexCoordData2f(texCoords, 0);
    batch.CopyNormalDataf(normals);
    if (!didSetup)
        batch.End();
    didSetup = true;
}

void DrumSelector::Icon::draw()
{
    if (fabsf(tempBounds.x - targetBounds.x) > 2 ||
        fabsf(tempBounds.y - targetBounds.y) > 2 ||
        fabsf(tempBounds.w - targetBounds.w) > 2 ||
        fabsf(tempBounds.h - targetBounds.h) > 2)
        updateBounds();
    else if (tempBounds != targetBounds)
        HUDView::setBounds(targetBounds);
    
    HUDView::draw();
    
    GLfloat color[4] = { 1.f, 1.f, 1.f, 1.f };
    
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), color);
    GLint polygonMode;
    glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.f);
    //batch.Draw();
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    
    
    GLuint textureID = KitManager::GetInstance().GetKit(0)->GetSample(id)->GetTexture();
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, Environment::instance().transformPipeline.GetModelViewMatrix(), 0);
    batch.Draw();
}

void DrumSelector::Icon::setBounds(const HUDRect &b)
{
    tempBounds = getBounds();
    targetBounds = b;
    
    HUDView::setBounds(b);
    
    xStep = (targetBounds.x - tempBounds.x) / 10.f;
    yStep = (targetBounds.y - tempBounds.y) / 10.f;
    wStep = (targetBounds.w - tempBounds.w) / 10.f;
    hStep = (targetBounds.h - tempBounds.h) / 10.f;
}

void DrumSelector::Icon::updateBounds()
{
    if (fabsf(tempBounds.x - targetBounds.x) < 2)
        tempBounds.x = targetBounds.x;
    else
        tempBounds.x += xStep;
    
    if (fabsf(tempBounds.y - targetBounds.y) < 2)
        tempBounds.y = targetBounds.y;
    else
        tempBounds.y += yStep;
    
    if (fabsf(tempBounds.w - targetBounds.w) < 2)
        tempBounds.w = targetBounds.w;
    else
        tempBounds.w += wStep;
    
    if (fabsf(tempBounds.h - targetBounds.h) < 2)
        tempBounds.h = targetBounds.h;
    else
        tempBounds.h += hStep;
    
    HUDView::setBounds(tempBounds);
}

void DrumSelector::Icon::loadTextures()
{
}