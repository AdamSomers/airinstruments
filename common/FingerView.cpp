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
