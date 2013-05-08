#include "Environment.h"

GLMatrixStack Environment::modelViewMatrix;
GLShaderManager Environment::shaderManager;
GLGeometryTransform Environment::transformPipeline;
GLMatrixStack Environment::projectionMatrix;
GLFrustum Environment::viewFrustum;
GLFrame	Environment::cameraFrame;

OpenGLContext Environment::openGLContext;

int Environment::screenW = 0;
int Environment::screenH = 0;

bool Environment::ready = false;

const String Environment::getDefaultFont() const
{
    String fontName = "Futura";
    StringArray typefaceNames = Font::findAllTypefaceNames();
    if (!typefaceNames.contains(fontName)) {
        Logger::outputDebugString("Did not find " + fontName + ", trying Trebuchet MS");
        fontName = "Trebuchet MS";
    }
    if (!typefaceNames.contains(fontName)) {
        Logger::outputDebugString("Did not find " + fontName + ", trying Verdana");
        fontName = "Verdana";
    }
    return fontName;
}