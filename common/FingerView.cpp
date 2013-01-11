#include "FingerView.h"
#include "GfxTools.h"

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

FingerView::Listener::Listener()
: needsReset(false)
, pointed(false)
, lastPointer(NULL)
{
}

void FingerView::Listener::reset()
{
    pointed = false;
    lastPointer = NULL;
    needsReset = false;
}