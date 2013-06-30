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
    shaderId = Environment::instance().shaderManager.LoadShaderPairSrcWithAttributes("test", BinaryData::testShader_vs, BinaryData::testShader_fs, 2,
                                                                                     GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
/*/
    M3DVector3f verts[4] = {
        -.25f, 0.f, -.25f,
        .25f, 0.f, -.25f,
        -.25f, 0.f, .25f,
        .25f, 0.f, .25f
    };
    
    M3DVector3f normals[4] = {
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f,
        0.f, 0.f, -1.f
    };

    batch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    batch.CopyVertexData3f(verts);
    batch.CopyNormalDataf(normals);
    batch.End();
//*/
    const int numVerts = 50;
    M3DVector3f verts[numVerts];
    M3DVector3f normals[numVerts];
    verts[0][0] = 0.f;
    verts[0][1] = 0.f;
    verts[0][2] = 0.f;
    normals[0][0] = 0.f;
    normals[0][1] = 0.f;
    normals[0][2] = -1.f;
//    texCoords[0][0] = 0.5f;
//    texCoords[0][1] = 0.5f;
    float r = 0.25f;
    
    for (int i = 1; i < numVerts; ++i)
    {

        float phase = -1.f * ((i - 1) / (float)(numVerts-2));
        float offset = 3.14159f / 2.f;
        verts[i][0] = verts[0][0] + r * cosf(offset + 2*3.14159f*phase);
        verts[i][1] = 0.f;
        verts[i][2] = verts[0][1] + r * sinf(offset + 2*3.14159f*phase);
        normals[i][0] = 0.f;
        normals[i][1] = 0.f;
        normals[i][2] = -1.f;
//        texCoords[i][0] = 0.5f + 0.5f * cosf(-offset + 2*3.14159f*-phase);
//        texCoords[i][1] = 0.5f + 0.5f * sinf(-offset + 2*3.14159f*-phase);
    }

    batch.Begin(GL_TRIANGLE_FAN, numVerts, 1);
    batch.CopyVertexData3f(verts);
    batch.CopyNormalDataf(normals);
    batch.End();

}

void HandView::draw()
{
    Environment::instance().modelViewMatrix.PushMatrix();
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);
    GLfloat color [] = { 0.f, 1.f, 0.f, .5f };
    
    //glEnable(GL_DEPTH_TEST);
    //glCullFace(GL_BACK);
    //glEnable(GL_CULL_FACE);
    
    glUseProgram((GLuint)shaderId);
    GLint iModelViewMatrix = glGetUniformLocation(shaderId, "mvMatrix");
    glUniformMatrix4fv(iModelViewMatrix, 1, GL_FALSE, Environment::instance().transformPipeline.GetModelViewMatrix());
    GLint iProjMatrix = glGetUniformLocation(shaderId, "pMatrix");
    glUniformMatrix4fv(iProjMatrix, 1, GL_FALSE, Environment::instance().transformPipeline.GetProjectionMatrix());
    GLint iColor = glGetUniformLocation(shaderId, "vColor");
    glUniform4fv(iColor, 1, color);
    //    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), color);
    batch.Draw();
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