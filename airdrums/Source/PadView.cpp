#include "PadView.h"
#include "SkinManager.h"
#include "Main.h"

#define FADE_TIME 300

GLFrame PadView::padSurfaceFrame;

PadView::PadView()
:
  padNum(0)
, padWidth(0.1f)
, padHeight(0.1f)
, fade(0.f)
, hoverFade(0.f)
, iconRotation(0.f)
, padDepth(0.025f)
, selectedMidiNote(0)
, hovering(false)
{
    backgroundOnTexture = SkinManager::instance().getSelectedSkin().getTexture("pad_on");
    backgroundOffTexture = SkinManager::instance().getSelectedSkin().getTexture("pad_off");
    setSelectedMidiNote(0);
    iconColor[0] = 1.f;
    iconColor[1] = 1.f;
    iconColor[2] = 1.f;
    iconColor[3] = 1.f;
}

PadView::~PadView()
{
}

void PadView::makePadMesh(M3DVector3f* inVerts, M3DVector3f* inNorms)
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

void PadView::makeSurfaceMesh(M3DVector3f* inVerts, M3DVector3f* inNorms)
{
    float top = padHeight / 2.f;
    float bottom = -padHeight / 2.f;
    float left = -padWidth / 2.f;
    float right = padWidth / 2.f;
    float depth = 0.1f;
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
    
    memcpy(inVerts, verts, 4*sizeof(M3DVector3f));
    memcpy(inNorms, normals, 4*sizeof(M3DVector3f));
}

void PadView::makeIconMesh(M3DVector3f* inVerts, M3DVector3f* inNorms)
{
    float wobble = 0.f;
    if (fade > 0.f)
        wobble = sinf(4*3.14159f*(1-fade)) * fade * 0.03f;

    float top = padHeight / 4.f + padHeight / 10.f + wobble;
    float bottom = -padHeight / 4.f + padHeight / 10.f - wobble;
    
    //deliberately stretch the icon vertically to compensate for perspective compression
    bottom -= + padHeight / 10.f;

    float left = -padWidth / 4.f - wobble;
    float right = padWidth / 4.f + wobble;
    float depth = 0.1f;
    //float front = -depth / 4.f;
    float back = depth / 4.f;
    
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
    
    memcpy(inVerts, verts, 4*sizeof(M3DVector3f));
    memcpy(inNorms, normals, 4*sizeof(M3DVector3f));
}

void PadView::makeTextMesh(M3DVector3f* inVerts, M3DVector3f* inNorms)
{
    float bottom = -padHeight / 2.f + padHeight / 20.f;
    float aspectRatio = textTexture.imageW / (float)textTexture.imageH;
    float top = bottom + padHeight / 10.f;

    float width = (padHeight / 10.f) * aspectRatio;

    // deliberately stretch the text vertically to compensade for perspective compression
    top += padHeight / 40.f;

    float left = -width / 2.f;
    float right = width / 2.f;
    float depth = 0.1f;
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
    
    memcpy(inVerts, verts, 4*sizeof(M3DVector3f));
    memcpy(inNorms, normals, 4*sizeof(M3DVector3f));
}

void PadView::setup()
{
 
    M3DVector3f verts[numVerts];
    M3DVector3f normals[numVerts];
    makePadMesh(verts, normals);
    padBatch.Begin(GL_TRIANGLES, numVerts);
    padBatch.CopyVertexData3f(verts);
    padBatch.CopyNormalDataf(normals);
    padBatch.End();
    
    M3DVector3f bgVerts[4];
    M3DVector3f bgNormals[4];
    M3DVector2f bgTexCoords[4] = {
        backgroundOnTexture.texX, backgroundOnTexture.texY + backgroundOnTexture.texH,
        backgroundOnTexture.texX + backgroundOnTexture.texW, backgroundOnTexture.texY + backgroundOnTexture.texH,
        backgroundOnTexture.texX, backgroundOnTexture.texY,
        backgroundOnTexture.texX + backgroundOnTexture.texW, backgroundOnTexture.texY
    };
    makeSurfaceMesh(bgVerts, bgNormals);
    bgBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    bgBatch.CopyVertexData3f(bgVerts);
    bgBatch.CopyNormalDataf(bgNormals);
    bgBatch.CopyTexCoordData2f(bgTexCoords, 0);
    bgBatch.End();
    
    M3DVector3f iconVerts[4];
    M3DVector3f iconNormals[4];
    M3DVector2f iconTexCoords[4] = {
        iconTexture.texX, iconTexture.texY + iconTexture.texH,
        iconTexture.texX + iconTexture.texW, iconTexture.texY + iconTexture.texH,
        iconTexture.texX, iconTexture.texY,
        iconTexture.texX + iconTexture.texW, iconTexture.texY
    };
    makeIconMesh(iconVerts, iconNormals);
    iconBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    iconBatch.CopyVertexData3f(iconVerts);
    iconBatch.CopyNormalDataf(iconNormals);
    iconBatch.CopyTexCoordData2f(iconTexCoords, 0);
    iconBatch.End();
    
    M3DVector3f textVerts[4];
    M3DVector3f textnNormals[4];
    M3DVector2f textTexCoords[4] = {
        textTexture.texX, textTexture.texY + textTexture.texH,
        textTexture.texX + textTexture.texW, textTexture.texY + textTexture.texH,
        textTexture.texX, textTexture.texY,
        textTexture.texX + textTexture.texW, textTexture.texY
    };
    makeTextMesh(textVerts, textnNormals);
    textBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    textBatch.CopyVertexData3f(textVerts);
    textBatch.CopyNormalDataf(textnNormals);
    textBatch.CopyTexCoordData2f(textTexCoords, 0);
    textBatch.End();
}

void PadView::update()
{
    M3DVector3f padVerts[numVerts];
    M3DVector3f padNormals[numVerts];
    makePadMesh(padVerts, padNormals);
    padBatch.CopyVertexData3f(padVerts);
    padBatch.CopyNormalDataf(padNormals);
    
    M3DVector3f bgVerts[4];
    M3DVector3f bgNormals[4];
    makeSurfaceMesh(bgVerts, bgNormals);
    bgBatch.CopyVertexData3f(bgVerts);
    bgBatch.CopyNormalDataf(bgNormals);
    
    M3DVector3f iconVerts[4];
    M3DVector3f iconNormals[4];
    makeIconMesh(iconVerts, iconNormals);
    M3DVector2f iconTexCoords[4] = {
        iconTexture.texX, iconTexture.texY + iconTexture.texH,
        iconTexture.texX + iconTexture.texW, iconTexture.texY + iconTexture.texH,
        iconTexture.texX, iconTexture.texY,
        iconTexture.texX + iconTexture.texW, iconTexture.texY
    };
    iconBatch.CopyVertexData3f(iconVerts);
    iconBatch.CopyNormalDataf(iconNormals);
    iconBatch.CopyTexCoordData2f(iconTexCoords, 0);
    
    M3DVector3f textVerts[4];
    M3DVector3f textnNormals[4];
    M3DVector2f textTexCoords[4] = {
        textTexture.texX, textTexture.texY + textTexture.texH,
        textTexture.texX + textTexture.texW, textTexture.texY + textTexture.texH,
        textTexture.texX, textTexture.texY,
        textTexture.texX + textTexture.texW, textTexture.texY
    };
    makeTextMesh(textVerts, textnNormals);
    textBatch.CopyVertexData3f(textVerts);
    textBatch.CopyNormalDataf(textnNormals);
    textBatch.CopyTexCoordData2f(textTexCoords, 0);
}

void PadView::draw()
{
    Environment::instance().modelViewMatrix.PushMatrix();
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    Environment::instance().modelViewMatrix.MultMatrix(mObjectFrame);
    
    GLfloat padColor [] = { 1.f, 1.f, 1.f, 0.3f + fade * 0.25f };
    if (hovering) {
        if (hoverFade < 1.f)
            hoverFade += 0.3f;
    }
    else if (hoverFade > 0.f)
        hoverFade -= 0.05f;
    
    RelativeTime diff = Time::getCurrentTime() - fadeStartTime;
    if (diff < RelativeTime::milliseconds(FADE_TIME))
        fade = 1.f - diff.inMilliseconds() / (float)FADE_TIME;
    else
        fade = 0;
    
    glEnable(GL_DEPTH_TEST);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, Environment::instance().transformPipeline.GetModelViewMatrix(), Environment::instance().transformPipeline.GetProjectionMatrix(), padColor);
    padBatch.Draw();
    
    glDisable(GL_DEPTH_TEST);
    
    GLfloat bgOffColor [] = { 1.f, 1.f, 1.f, 1.f };
    glBindTexture(GL_TEXTURE_2D, backgroundOffTexture.textureId);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), bgOffColor, 0);
    bgBatch.Draw();

    GLfloat bgOnColor [] = { 1.f, 1.f, 1.f, jmin(1.f, fade + hoverFade * 0.5f)};
    glBindTexture(GL_TEXTURE_2D, backgroundOnTexture.textureId);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), bgOnColor, 0);
    bgBatch.Draw();

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    Environment::instance().modelViewMatrix.PushMatrix();
    Environment::instance().modelViewMatrix.Rotate(iconRotation * fade, 0.0f, 0.0f, 1.f);
    glBindTexture(GL_TEXTURE_2D, iconTexture.textureId);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), iconColor, 0);
    iconBatch.Draw();
    Environment::instance().modelViewMatrix.PopMatrix();

    glBindTexture(GL_TEXTURE_2D, textTexture.textureId);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewProjectionMatrix(), bgOffColor, 0);
    textBatch.Draw();
    
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    

    Environment::instance().modelViewMatrix.PopMatrix();
}

void PadView::triggerDisplay(float amount)
{
    fade = amount;
    fadeStartTime = Time::getCurrentTime();

    iconRotation = (Random::getSystemRandom().nextFloat() * 2.f - 1.f) * 2;
}

void PadView::setSelectedMidiNote(int note)
{
    if (note >= Drums::instance().getNumNotes())
        note = 0;
    else if (note < 0)
        note = Drums::instance().getNumNotes() - 1;
    selectedMidiNote = note;
    
    TextureDescription iconTextureDesc;
    TextureDescription categoryTextureDesc;
    String kitUuidString = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("kitUuid", "Default");
    if (kitUuidString != "Default") {
        Uuid kitUuid(kitUuidString);
        iconTexture = KitManager::GetInstance().GetItem(kitUuid)->GetSample(selectedMidiNote)->GetTexture(true);
        String category = KitManager::GetInstance().GetItem(kitUuid)->GetSample(selectedMidiNote)->GetCategory();
        textTexture = SkinManager::instance().getSelectedSkin().getTexture("Text_" + category);
        
    }
    else {
        iconTexture = KitManager::GetInstance().GetItem(0)->GetSample(selectedMidiNote)->GetTexture(true);
        String category = KitManager::GetInstance().GetItem(0)->GetSample(selectedMidiNote)->GetCategory();
        textTexture = SkinManager::instance().getSelectedSkin().getTexture("Text_" + category);
    }
}

void PadView::setColor(const Colour& color)
{
    iconColor[0] = color.getFloatRed();
    iconColor[1] = color.getFloatGreen();
    iconColor[2] = color.getFloatBlue();
    iconColor[3] = color.getFloatAlpha();
}

void PadView::getScreenPos(M3DVector2f& inVec)
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
    inVec[1] = win[1];
}

void PadView::tap(int midiNote)
{
    if (midiNote == selectedMidiNote) {
        triggerDisplay();
    }
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
