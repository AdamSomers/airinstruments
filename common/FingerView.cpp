#include "FingerView.h"
#include "GfxTools.h"
#include "MotionServer.h"

FingerView::FingerView()
: inUse(false)
, id(-1)
, invalid(false)
{
}

void FingerView::setup()
{
    gltMakeCylinder(coneBatch, 0.f, 0.02f, -.1f, 10, 2);
    gltMakeCylinder(cylinderBatch, .001f, .01f, -.2f, 10, 2);
}

void FingerView::draw()
{
    Environment::instance().modelViewMatrix.PushMatrix();
    M3DMatrix44f mCamera;
    Environment::instance().cameraFrame.GetCameraMatrix(mCamera);
    Environment::instance().modelViewMatrix.MultMatrix(mCamera);
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);
    GLfloat vBlack [] = { 0.f, 0.f, 0.f, 1.f };
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    GfxTools::drawBatch(&cylinderBatch);
    GfxTools::drawBatch(&coneBatch);
    
    Environment::instance().modelViewMatrix.PopMatrix();
}

float FingerView::normalizedX()
{
    return (finger.tipPosition().x / 400.f);
}

float FingerView::normalizedY()
{
    return (finger.tipPosition().y / 500.f);
}

float FingerView::normalizedZ()
{
    return finger.tipPosition().z / 250.f;
}

void FingerView::getScreenPos(M3DVector2f& inVec)
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
    inVec[0] = win[0];
    inVec[1] = Environment::instance().screenH - win[1];
}

FingerView::Listener::Listener()
: needsReset(false)
{
    MotionDispatcher::instance().fingerViewListeners.push_back(this);
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