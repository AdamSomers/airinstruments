#include "PadView.h"

PadView::PadView()
{
}

PadView::~PadView()
{
}

void PadView::setup()
{
    M3DVector3f verts[4] = {
        0.0f, -padHeight / 2.f, 0.0f,
        padWidth, -padHeight / 2.f, 0.0f,
        0.f, padHeight / 2.f, 0.0f,
        padWidth, padHeight / 2.f, 0.0f
    };
    
    M3DVector3f normals[4] = {
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f
    };
    
    padBatch.Begin(GL_TRIANGLE_STRIP, 4);
    padBatch.CopyVertexData3f(verts);
    padBatch.CopyNormalDataf(normals);
    padBatch.End();
}

void PadView::update()
{
    M3DVector3f verts[4] = {
        0.0f, -padHeight / 2.f, 0.0f,
        padWidth, -padHeight / 2.f, 0.0f,
        0.f, padHeight / 2.f, 0.0f,
        padWidth, padHeight / 2.f, 0.0f
    };
    padBatch.CopyVertexData3f(verts);
}

void PadView::draw()
{
    Environment::instance().modelViewMatrix.PushMatrix();
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);
    
    GLfloat padColor [] = { 0.7f, 0.7f, 1.f, 0.3f + fade * 0.5f };
    if (pointers.size() > 0) {
        if (fade < 1.f)
            fade += 0.3f;
    }
    else if (fade > 0.f)
        fade -= 0.05f;
    
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), padColor);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    padBatch.Draw();
    Environment::instance().modelViewMatrix.PopMatrix();
}

void PadView::updatePointedState(FingerView* inFingerView)
{
    M3DVector3f point;
    M3DVector3f ray;
    M3DVector3f center;
    inFingerView->objectFrame.GetOrigin(point);
    inFingerView->objectFrame.GetForwardVector(ray);
    objectFrame.GetOrigin(center);
    
    for (int i = 0; i < 6; ++i)
    {
        // TODO
#if 0
        // Get surface rect
        
        // Calculate collision distance of finger ray with surface rect
        M3DVector3f collisionPoint;
        M3DVector3f pNormal = { 0.f, 0.f, -1.f };
        m3dNormalizeVector3(ray);
        center[0] -= surfaceWidth / 2.f;
        GfxTools::collide(point, ray, center, pNormal, collisionPoint);
        float distance = fabsf(collisionPoint[0] - center[0]);
        // check that distance is within rect
        if (distance < stringWidth / 2.f)
        {
            fingerPointing(inFingerView);
        }
        else
        {
            fingerNotPointing(inFingerView);
        }
#endif
    }
    
#if 0
    // When finger passes through pad volume, play the note
    // TODO
    Drums::instance().noteOn ...
#endif
}