#include "HandView.h"
#include "GfxTools.h"
#include "MotionServer.h"

HandView::HandView()
: inUse(false)
, id(-1)
, invalid(false)
{
}

void HandView::setup()
{
    gltMakeDisk(batch, .02, .2, 20, 2);
}

void HandView::draw()
{
    Environment::instance().modelViewMatrix.PushMatrix();
    M3DMatrix44f mCamera;
    Environment::instance().cameraFrame.GetCameraMatrix(mCamera);
    Environment::instance().modelViewMatrix.MultMatrix(mCamera);
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);
    GLfloat color [] = { 0.f, 1.f, 0.f, 1.f };
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), color);
    GfxTools::drawBatch(&batch);
    
    Environment::instance().modelViewMatrix.PopMatrix();
}

float HandView::normalizedX()
{
    return (hand.palmPosition().x / 400.f);
}

float HandView::normalizedY()
{
    return (hand.palmPosition().y / 500.f);
}

float HandView::normalizedZ()
{
    return hand.palmPosition().z / 250.f;
}

void HandView::getScreenPos(M3DVector2f& inVec)
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

HandView::Listener::Listener()
: needsReset(false)
{
    MotionDispatcher::instance().handViewListeners.push_back(this);
}

HandView::Listener::~Listener()
{
}

void HandView::Listener::reset()
{
    pointers.clear();
    needsReset = false;
}

void HandView::Listener::handPointing(HandView* fv)
{
    auto iter = std::find(pointers.begin(), pointers.end(), fv);
    if (iter == pointers.end())
    {
        pointers.push_back(fv);
    }
    needsReset = false;
}

void HandView::Listener::handNotPointing(HandView* fv)
{
    auto iter = std::find(pointers.begin(), pointers.end(), fv);
    if (iter != pointers.end() && (*iter) == fv) {
        pointers.erase(iter);
        needsReset = false;
    }
}