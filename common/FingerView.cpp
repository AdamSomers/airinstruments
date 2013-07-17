#include "FingerView.h"
#include "GfxTools.h"
#include "MotionServer.h"

FingerView::FingerView()
: inUse(false)
, id(-1)
, invalid(false)
, didSetup(false)
{
}

void FingerView::setup()
{
    //gltMakeCylinder(coneBatch, 0.f, 0.02f, -.1f, 10, 2);
    //gltMakeCylinder(cylinderBatch, .001f, .01f, -.2f, 10, 2);
    gltMakeSphere(cylinderBatch, .03f, 10, 10);
}

void FingerView::drawWithShader(int shaderId)
{
    if (!didSetup)
    {
        setup();
        didSetup = true;
    }

    updateScreenPos();
    
    Environment::instance().modelViewMatrix.PushMatrix();
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);
    GLfloat c[] = { color.getFloatRed(), color.getFloatGreen(), color.getFloatBlue(), 1.f };
    GLint iModelViewMatrix = glGetUniformLocation(shaderId, "mvMatrix");
    glUniformMatrix4fv(iModelViewMatrix, 1, GL_FALSE, Environment::instance().transformPipeline.GetModelViewMatrix());
    GLint iProjMatrix = glGetUniformLocation(shaderId, "pMatrix");
    glUniformMatrix4fv(iProjMatrix, 1, GL_FALSE, Environment::instance().transformPipeline.GetProjectionMatrix());
    GLint iColor = glGetUniformLocation(shaderId, "vColor");
    glUniform4fv(iColor, 1, c);
    GfxTools::drawBatch(&cylinderBatch);
    //GfxTools::drawBatch(&coneBatch);
    Environment::instance().modelViewMatrix.PopMatrix();
}

void FingerView::draw()
{
    updateScreenPos();
    if (!didSetup)
    {
        setup();
        didSetup = true;
    }
}

void FingerView::getScreenPos(M3DVector2f& inVec)
{
    inVec[0] = screenPos[0];
    inVec[1] = screenPos[1];
}

void FingerView::updateScreenPos()
{
    GLint viewport[4];
    viewport[0] = 0;
    viewport[1] = 0;
    viewport[2] = Environment::instance().screenW;
    viewport[3] = Environment::instance().screenH;
    
    M3DVector2f win;
    M3DVector3f origin;
    objectFrame.GetOrigin(origin);
    m3dProjectXY(win,
                 Environment::instance().transformPipeline.GetModelViewMatrix(),
                 Environment::instance().transformPipeline.GetProjectionMatrix(),
                 viewport,
                 origin);
    //printf("win %f %f\n", win[0], win[1]);
    screenPos[0] = win[0];
    screenPos[1] = Environment::instance().screenH - win[1];
}

FingerView::Listener::Listener()
: needsReset(false)
{
    MotionDispatcher::instance().fingerViewListeners.push_back(this);
}

FingerView::Listener::~Listener()
{
    auto i = std::find(MotionDispatcher::instance().fingerViewListeners.begin(), MotionDispatcher::instance().fingerViewListeners.end(), this);
    if (i != MotionDispatcher::instance().fingerViewListeners.end())
        MotionDispatcher::instance().fingerViewListeners.erase(i);
}

void FingerView::Listener::reset()
{
    pointers.clear();
    needsReset = false;
}

void FingerView::Listener::fingerPointing(FingerView* fv)
{
    auto iter = std::find(pointers.begin(), pointers.end(), fv);
    if (iter == pointers.end())
    {
        pointers.push_back(fv);
    }
    needsReset = false;
}

void FingerView::Listener::fingerNotPointing(FingerView* fv)
{
    auto iter = std::find(pointers.begin(), pointers.end(), fv);
    if (iter != pointers.end() && (*iter) == fv) {
        pointers.erase(iter);
        needsReset = false;
    }
}

void FingerView::Listener::tap(FingerView* fv, float velocity)
{
    auto iter = std::find(pointers.begin(), pointers.end(), fv);
    if (iter != pointers.end())
        this->tap(velocity);
}

void FingerView::Listener::circleBack(FingerView* fv)
{
    auto iter = std::find(pointers.begin(), pointers.end(), fv);
    if (iter != pointers.end())
        this->circleBack();
}