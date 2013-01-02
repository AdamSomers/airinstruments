#include "Environment.h"

GLMatrixStack Environment::modelViewMatrix;
GLShaderManager Environment::shaderManager;
GLGeometryTransform Environment::transformPipeline;
int Environment::screenW = 0;
int Environment::screenH = 0;