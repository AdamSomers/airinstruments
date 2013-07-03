uniform mat4 mvpMatrix;
attribute vec4 vVertex;
attribute vec2 vTexCoord0;
varying vec2 vTex;

void main(void)
{
    vTex = vTexCoord0;
    gl_Position = mvpMatrix * vVertex;
}