//
//  View2d.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/22/13.
//
//

#include "View2d.h"
#include "Environment.h"

View2d::View2d()
: didSetup(false)
, defaultColorSet(false)
, isVisible(true)
, opacity(1.f)
, fadeTime(0)
, needsSetup(false)
{
    defaultColor[0] = 1.f;
    defaultColor[1] = 1.f;
    defaultColor[2] = 1.f;
    defaultColor[3] = 1.f;
}

View2d::~View2d()
{
    
}


void View2d::draw()
{
    if (needsSetup) {
        setup();
        needsSetup = false;
    }

    if (isVisible && opacity < 1.f)
    {
        RelativeTime timeSinceLateVisibiltyChange = Time::getCurrentTime() - lastVisibilityChange;
        float deltaTime = timeSinceLateVisibiltyChange.inMilliseconds() / (float)fadeTime;
        opacity = deltaTime*deltaTime;
        if (opacity > 1.f) opacity = 1.f;
    }
    else if (!isVisible && opacity > 0.f)
    {
        RelativeTime timeSinceLateVisibiltyChange = Time::getCurrentTime() - lastVisibilityChange;
        float deltaTime = timeSinceLateVisibiltyChange.inMilliseconds() / (float)fadeTime;
        opacity = 1 - (deltaTime*deltaTime);
        if (opacity < 0.f) opacity = 0.f;
    }

    if (0 != defaultTexture.textureId)
    {
        GLfloat color[4] = { defaultColor[0], defaultColor[1], defaultColor[2], defaultColor[3] * opacity };
        
        glBindTexture(GL_TEXTURE_2D, defaultTexture.textureId);
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), color, 0);
        defaultBatch.Draw();
    }
    else
    {
        GLfloat color[4] = { defaultColor[0], defaultColor[1], defaultColor[2], defaultColor[3] * opacity };
        
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode[0]);
        if (!defaultColorSet)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), color);
        glLineWidth(1.f);
        defaultBatch.Draw();
        glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
    }
}

void View2d::setup()
{
    M3DVector3f verts[4] = {
        bounds.x, bounds.y, 0.f,
        bounds.x + bounds.w, bounds.y, 0.f,
        bounds.x, bounds.y + bounds.h, 0.f,
        bounds.x + bounds.w, bounds.y + bounds.h, 0.f
    };
    
    M3DVector3f normals[4] = {
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f
    };
    
    M3DVector2f texCoords[4] = {
        0.f, defaultTexture.texY,
        defaultTexture.texX, defaultTexture.texY,
        0.f, 0.f,
        defaultTexture.texX, 0.f
    };
    
    if (!didSetup)
        defaultBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    defaultBatch.CopyVertexData3f(verts);
    defaultBatch.CopyTexCoordData2f(texCoords, 0);
    defaultBatch.CopyNormalDataf(normals);
    if (!didSetup)
        defaultBatch.End();
    
    didSetup = true;
}

void View2d::boundsChanged()
{
    setup();
}

void View2d::setBounds(const HUDRect& b)
{
    bounds = b;
    boundsChanged();
}

void View2d::loadTextures()
{
    
}

void View2d::setDefaultTexture(TextureDescription texture)
{
    if (defaultTexture != texture) {
        defaultTexture = texture;
        needsSetup = true;
    }
}

void View2d::setDefaultColor(GLfloat* color)
{
    memcpy(defaultColor, color, 4 * sizeof(GLfloat));
    defaultColorSet = true;
}


void View2d::setVisible(bool shouldBeVisible, int fadeTimeMs /* = 500 */)
{
    isVisible = shouldBeVisible;
    lastVisibilityChange = Time::getCurrentTime();
    fadeTime = fadeTimeMs;
}
