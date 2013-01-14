#include "Environment.h"

GLMatrixStack Environment::modelViewMatrix;
GLShaderManager Environment::shaderManager;
GLGeometryTransform Environment::transformPipeline;
GLMatrixStack Environment::projectionMatrix;
GLFrustum Environment::viewFrustum;
GLFrame	Environment::cameraFrame;

int Environment::screenW = 0;
int Environment::screenH = 0;

bool Environment::ready = false;