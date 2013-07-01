#include "FingerView.h"
#include "GfxTools.h"
#include "MotionServer.h"

FingerView::FingerView()
: inUse(false)
, id(-1)
, invalid(false)
, shaderId(-1)
{
}

void FingerView::setup()
{
    gltMakeCylinder(coneBatch, 0.f, 0.02f, -.1f, 10, 2);
    gltMakeCylinder(cylinderBatch, .001f, .01f, -.2f, 10, 2);
    
    shaderId = Environment::instance().shaderManager.LoadShaderPairSrcWithAttributes("test", BinaryData::testShader_vs, BinaryData::testShader_fs, 2,
                                                                                            GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
    
    bloomShaderId = gltLoadShaderPairSrcWithAttributes(BinaryData::bloom_vs, BinaryData::bloom_fs, 2,
                                                       GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");
    
    jassert(bloomShaderId != 0);
    
//    gltLoadShaderPairSrcWithAttributes(szTextureModulateVP, szTextureModulateFP, 2,
//                                       GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoord0");
    
    int imageW = 128;
    int imageH = 128;
    Image im(Image::PixelFormat::ARGB, imageW, imageH, true);
    Graphics g(im);
    
    TextureDescription td = GfxTools::loadTextureFromJuceImage(im);
    td.texX = 0.f;
    td.texY = 1.f;
    td.texW = 1.f;
    td.texH = -1.f;
    imageView.setDefaultTexture(td);
}

void FingerView::drawToTexture()
{

    
    glViewport(0,0,imageView.getDefaultTexture().imageW,imageView.getDefaultTexture().imageH);
    
    Environment::instance().modelViewMatrix.PushMatrix();
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);
    GLfloat color [] = { 0.f, 1.f, 0.f, 1.f };
    
    glUseProgram((GLuint)shaderId);
    GLint iModelViewMatrix = glGetUniformLocation(shaderId, "mvMatrix");
    glUniformMatrix4fv(iModelViewMatrix, 1, GL_FALSE, Environment::instance().transformPipeline.GetModelViewMatrix());
    GLint iProjMatrix = glGetUniformLocation(shaderId, "pMatrix");
    glUniformMatrix4fv(iProjMatrix, 1, GL_FALSE, Environment::instance().transformPipeline.GetProjectionMatrix());
    GLint iColor = glGetUniformLocation(shaderId, "vColor");
    glUniform4fv(iColor, 1, color);

    //    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), color);
    GfxTools::drawBatch(&cylinderBatch);
    GfxTools::drawBatch(&coneBatch);
    
    Environment::instance().modelViewMatrix.PopMatrix();
    
    glBindTexture(GL_TEXTURE_2D,imageView.getDefaultTexture().textureId);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, imageView.getDefaultTexture().imageW, imageView.getDefaultTexture().imageH, 0);
    glClearColor(0.0f, 0.0f, 0.5f, 0.5);                // Set The Clear Color To Medium Blue
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0,0,Environment::instance().screenW,Environment::instance().screenH);
}

void FingerView::draw()
{
    if (shaderId == -1)
        return;

    Environment::instance().viewFrustum.SetOrthographic(0, Environment::instance().screenW, 0.0f, Environment::instance().screenH, 800.0f, -800.0f);
	Environment::instance().modelViewMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    
    glDisable(GL_DEPTH_TEST);                   // Disable Depth Testing
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);               // Set Blending Mode
    glEnable(GL_BLEND);                     // Enable Blending
    
    if (imageView.getBounds().w != Environment::instance().screenW ||
        imageView.getBounds().h != Environment::instance().screenH)
        imageView.setBounds(HUDRect(0,0,Environment::instance().screenW,Environment::instance().screenH));
    
    GLfloat imageColor[4] = { 1.f, 1.f, 1.f, 1.f };
    //imageView.setDefaultColor(imageColor);
    imageView.setDefaultBlendMode(GL_SRC_ALPHA,GL_ONE);
    
    //imageView.draw();
    glBindTexture(GL_TEXTURE_2D, imageView.getDefaultTexture().textureId);

    glUseProgram((GLuint)bloomShaderId);
    GLint iModelViewMatrix = glGetUniformLocation(bloomShaderId, "mvpMatrix");
    glUniformMatrix4fv(iModelViewMatrix, 1, GL_FALSE, Environment::instance().transformPipeline.GetModelViewMatrix());
    //GLint iProjMatrix = glGetUniformLocation(shaderId, "pMatrix");
    //glUniformMatrix4fv(iProjMatrix, 1, GL_FALSE, Environment::instance().transformPipeline.GetProjectionMatrix());
    GLint iColor = glGetUniformLocation(bloomShaderId, "vColor");
    glUniform4fv(iColor, 1, imageColor);
    GLint iTextureUnit = glGetUniformLocation(bloomShaderId, "textureUnit0");
    glUniform1i(iTextureUnit, 0);
    //GLint iTextureUniform = glGetUniformLocation(shaderId, "bgl_RenderedTexture");
    //glUniform1i(iTextureUniform, imageView.getDefaultTexture().textureId);
    
 //   Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), color, 0);
    
    imageView.defaultBatch.Draw();

    Environment::instance().viewFrustum.SetPerspective(10.0f, float(Environment::instance().screenW)/float(Environment::instance().screenH), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    Environment::instance().modelViewMatrix.LoadIdentity();
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