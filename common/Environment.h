#ifndef h_Environment
#define h_Environment

#include <iostream>

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

class Environment
{
public:
    static Environment& instance( void )
    {
        static Environment s_instance;
        return s_instance;
    }
    
    static GLMatrixStack modelViewMatrix;
    static GLMatrixStack projectionMatrix;
    static GLShaderManager shaderManager;
    static GLGeometryTransform transformPipeline;
    static GLFrustum viewFrustum;
    static GLFrame	cameraFrame;

    static int screenW;
    static int screenH;
private:
};

#endif