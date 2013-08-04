//
//  ShadowView.cpp
//  AirBeats
//
//  Created by Adam Somers on 8/1/13.
//
//

#include "ShadowView.h"
#include "SkinManager.h"

ShadowView::ShadowView()
{
    shadowTexture = SkinManager::instance().getSelectedSkin().getTexture("shadow");
}

ShadowView::~ShadowView()
{
    
}

void ShadowView::makeMesh(M3DVector3f *inVerts, M3DVector3f *inNorms, M3DVector2f *inTexCoords)
{
    float w = .2f;
    float h = .2f;
    float depth = 0.1f;
    float top = h / 2.f;
    float bottom = -h / 2.f;
    float left = w / 2.f;
    float right = -w / 2.f;
    //float front = -depth / 2.f;
    float back = depth / 2.f;
    
    M3DVector3f verts[4] = {
        left, bottom, back,
        right, bottom, back,
        left, top, back,
        right, top, back
    };
    
    M3DVector3f normals[4] = {
        // top
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f
    };
    
    M3DVector2f texCoords[4] = {
        shadowTexture.texX, shadowTexture.texY + shadowTexture.texH,
        shadowTexture.texX + shadowTexture.texW, shadowTexture.texY + shadowTexture.texH,
        shadowTexture.texX, shadowTexture.texY,
        shadowTexture.texX + shadowTexture.texW, shadowTexture.texY
    };
    memcpy(inVerts, verts, 4*sizeof(M3DVector3f));
    memcpy(inNorms, normals, 4*sizeof(M3DVector3f));
    memcpy(inTexCoords, texCoords, 4*sizeof(M3DVector2f));
}

void ShadowView::setup()
{
//    gltMakeCube(shadowBatch, .2f);//(ballBatch, .1f, 20, 20);
    M3DVector3f verts[4];
    M3DVector3f normals[4];
    M3DVector2f texCoords[4];
    makeMesh(verts, normals, texCoords);
    shadowBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    shadowBatch.CopyVertexData3f(verts);
    shadowBatch.CopyNormalDataf(normals);
    shadowBatch.CopyTexCoordData2f(texCoords, 0);
    shadowBatch.End();
}

void ShadowView::update()
{
//    M3DVector3f verts[4];
//    M3DVector3f normals[4];
//    M3DVector2f texCoords[4];
//    makeMesh(verts, normals, texCoords);
//    shadowBatch.CopyVertexData3f(verts);
//    shadowBatch.CopyNormalDataf(normals);
//    shadowBatch.CopyTexCoordData2f(texCoords, 0);
}

void ShadowView::draw()
{
    Environment::instance().modelViewMatrix.PushMatrix();
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);

    glDisable(GL_DEPTH_TEST);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat color[4] = { 1.f, 1.f, 1.f, .7f };

    glBindTexture(GL_TEXTURE_2D, shadowTexture.textureId);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), color, 0);
    shadowBatch.Draw();
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glDisable(GL_DEPTH_TEST);
    
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
//    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), color);
//    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), color);
//    shadowBatch.Draw();

    
    Environment::instance().modelViewMatrix.PopMatrix();
}