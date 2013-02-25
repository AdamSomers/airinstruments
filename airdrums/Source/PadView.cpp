#include "PadView.h"

GLFrame PadView::padSurfaceFrame;

PadView::PadView()
{
}

PadView::~PadView()
{
}

void PadView::makeMesh(M3DVector3f* inVerts, M3DVector3f* inNorms)
{
    float top = padHeight / 2.f;
    float bottom = -padHeight / 2.f;
    float left = -padWidth / 2.f;
    float right = padWidth / 2.f;
    float depth = 0.1f;
    float front = -depth / 2.f;
    float back = depth / 2.f;
    M3DVector3f verts[numVerts] = {
        // bottom
        left, bottom, front,
        right, bottom, front,
        right, bottom, back,
        
        right, bottom, back,
        left, bottom, back,
        left, bottom, front,

        // front face
        left, bottom, front,
        left, top, front,
        right, bottom, front,
        
        right, bottom, front,
        left, top, front,
        right, top, front,

        // rear face
        left, bottom, back,
        right, bottom, back,
        left, top, back,
        
        right, bottom, back,
        right, top, back,
        left, top, back,

        // left side
        left, bottom, front,
        left, top, back,
        left, top, front,
        
        left, top, back,
        left, bottom, front,
        left, bottom, back,
        
        
        // right side
        right, bottom, front,
        right, top, front,
        right, top, back,
        
        right, top, back,
        right, bottom, back,
        right, bottom, front,

        // top
        left, top, front,
        right, top, back,
        right, top, front,
        
        right, top, back,
        left, top, front,
        left, top, back,
        
    };
    
    M3DVector3f normals[numVerts] = {
        // bottom
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        // front face
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        
        // rear face
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        
        // left side
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        
        // right side
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        // top
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    memcpy(inVerts, verts, numVerts*sizeof(M3DVector3f));
    memcpy(inNorms, normals, numVerts*sizeof(M3DVector3f));
}

void PadView::setup()
{
 
    M3DVector3f verts[numVerts];
    M3DVector3f normals[numVerts];
    makeMesh(verts, normals);
    padBatch.Begin(GL_TRIANGLES, numVerts);
    padBatch.CopyVertexData3f(verts);
    padBatch.CopyNormalDataf(normals);
    padBatch.End();
}

void PadView::update()
{
    M3DVector3f verts[numVerts];
    M3DVector3f normals[numVerts];
    makeMesh(verts, normals);
    padBatch.CopyVertexData3f(verts);
    padBatch.CopyNormalDataf(normals);
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

    padBatch.Draw();
    Environment::instance().modelViewMatrix.PopMatrix();
}

void PadView::triggerDisplay(float amount)
{
    fade = amount;
}

bool PadView::hitTest(const M3DVector3f& point)
{
    M3DVector3f center;
    objectFrame.GetOrigin(center);
    M3DVector3f transformedCenter;
    M3DMatrix44f cameraMatrix;
    Environment::instance().cameraFrame.GetMatrix(cameraMatrix);
    m3dTransformVector3(transformedCenter, center, cameraMatrix);
    
    if (point[0] > transformedCenter[0] - padWidth / 2.f &&
        point[0] < transformedCenter[0] + padWidth / 2.f &&
        point[1] > transformedCenter[1] - padHeight / 2.f &&
        point[1] < transformedCenter[1] + padHeight / 2.f &&
        point[2] <= transformedCenter[2])
        return true;
    else
        return false;
}

void PadView::tap(float velocity)
{
    Drums::instance().NoteOn(padNum,velocity);
}

void PadView::updatePointedState(FingerView* inFingerView)
{
    M3DVector3f point;
    M3DVector3f ray;
    inFingerView->objectFrame.GetOrigin(point);
    inFingerView->objectFrame.GetForwardVector(ray);
    
    M3DVector3f prevPoint;
    inFingerView->prevFrame.GetOrigin(prevPoint);

    M3DVector3f center;
    objectFrame.GetOrigin(center);
    M3DVector3f transformedCenter;
    M3DMatrix44f cameraMatrix;
    Environment::instance().cameraFrame.GetMatrix(cameraMatrix);
    m3dTransformVector3(transformedCenter, center, cameraMatrix);

    if (fabsf(point[0] - transformedCenter[0]) < padWidth / 2.f ) {
        fingerPointing(inFingerView);
        //triggerDisplay(0.5f);
    }
    else
        fingerNotPointing(inFingerView);
    
    
    for (int i = 0; i < 6; ++i)
    {
        // TODO
#if 0
        // Get surface rect
        
        // Calculate collision distance of finger ray with surface rect
        M3DVector3f collisionPoint;
        M3DVector3f pNormal = { 0.f, 0.f, -1.f };
        m3dNormalizeVector3(ray);
        transformedCenter[0] -= padWidth / 2.f;
        GfxTools::collide(point, ray, transformedCenter, pNormal, collisionPoint);
        float distance = fabsf(collisionPoint[0] - transformedCenter[0]);
        // check that distance is within rect
        if (distance < padWidth / 2.f)
        {
            fingerPointing(inFingerView);
            triggerDisplay();
        }
        else
        {
            fingerNotPointing(inFingerView);
        }
#endif
    }

#if 0
    // When finger passes through pad volume, play the note
    if (hitTest(point) && !hitTest(prevPoint) &&
        prevPoint[1] > point[1])
    {
        //fade = 1.f;
        printf("trigger %d\n", padNum);
        Drums::instance().NoteOn(padNum,1.f);
    }
    
#endif
}