//
//  StickView.cpp
//  AirBeats
//
//  Created by Adam Somers on 7/24/13.
//
//

#include "StickView.h"
#include "SkinManager.h"
#include "Main.h"

#define FADE_TIME 300

#define DISTANCE_THRESHOLD 0.25f

StickView::StickView()
: fade(0.f)
, pointableId(-1)
, handId(-1)
, inUse(false)
{
}

StickView::~StickView()
{
}

void StickView::makePadMesh(M3DVector3f* inVerts, M3DVector3f* inNorms)
{
    float top = 1 / 2.f;
    float bottom = -1 / 2.f;
    float left = -1 / 2.f;
    float right = 1 / 2.f;
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


void StickView::setup()
{
    
    gltMakeSphere(stickBatch, .05f, 20, 20);
    M3DVector3f verts[numVerts];
    M3DVector3f normals[numVerts];
    makePadMesh(verts, normals);
    padBatch.Begin(GL_TRIANGLES, numVerts);
    padBatch.CopyVertexData3f(verts);
    padBatch.CopyNormalDataf(normals);
    padBatch.End();
    
    shadow.setup();
    shadow.objectFrame.RotateWorld((float) m3dDegToRad(-60), 1, 0, 0);
}

void StickView::update()
{
    M3DVector3f padVerts[numVerts];
    M3DVector3f padNormals[numVerts];
    makePadMesh(padVerts, padNormals);
    padBatch.CopyVertexData3f(padVerts);
    padBatch.CopyNormalDataf(padNormals);
}

void StickView::draw()
{
    updateScreenPos();

    Environment::instance().modelViewMatrix.PushMatrix();
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);
    
    GLfloat padColor [] = { .5f + fade * 0.25f,
        .5f + fade * 0.25f,
        .5f + fade * 0.25f,
        1.f };
    
    RelativeTime diff = Time::getCurrentTime() - fadeStartTime;
    if (diff < RelativeTime::milliseconds(FADE_TIME))
        fade = 1.f - diff.inMilliseconds() / (float)FADE_TIME;
    else
        fade = 0;
    
    glEnable(GL_DEPTH_TEST);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), padColor);
    stickBatch.Draw();
    
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Environment::instance().modelViewMatrix.PopMatrix();
    
    if (calcStickDistance() > DISTANCE_THRESHOLD)
        shadow.draw();
}

void StickView::setOrigin(float x, float y, float z, bool limitToPlane)
{
    ghostY = y;
    M3DVector3f collisionPoint;
    calcCollisionPoint(collisionPoint);
    if (limitToPlane)
        objectFrame.SetOrigin(x,jmax(y, collisionPoint[1] + 0.2f),z);
    else
        objectFrame.SetOrigin(x,y,z);
    shadow.objectFrame.SetOrigin(x, collisionPoint[1] + 0.2f ,z);
}

void StickView::calcCollisionPoint(M3DVector3f collisionPoint)
{
    M3DVector3f pOrigin, pNormal, rOrigin;
    M3DVector3f rNormal = { 0.f, -1.f, 0.f };
    PadView::padSurfaceFrame.GetUpVector(pNormal);
    PadView::padSurfaceFrame.GetOrigin(pOrigin);
    objectFrame.GetOrigin(rOrigin);
    M3DMatrix33f m;
    M3DVector3f pNormalRot;
    m3dRotationMatrix33(m, m3dDegToRad(90.f), 1.f, 0.f, 0.f);
    m3dRotateVector(pNormalRot, pNormal, m);
    GfxTools::collide(rOrigin, rNormal, pOrigin, pNormalRot, collisionPoint);
}

float StickView::calcStickDistance()
{
    M3DVector3f collisionPoint;
    calcCollisionPoint(collisionPoint);
    M3DVector3f rOrigin;
    objectFrame.GetOrigin(rOrigin);
    float dist = ghostY - collisionPoint[1];
    return dist;
}