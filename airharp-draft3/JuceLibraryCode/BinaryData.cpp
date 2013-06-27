/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== testShader.fs ==================
static const unsigned char temp_92fb9965[] =
"#ifdef OPENGL_ES\r\n"
"precision mediump float;\r\n"
"#endif\r\n"
"varying vec4 vFragColor;\r\n"
"\r\n"
"void main(void) { \r\n"
"    gl_FragColor = vFragColor; \r\n"
"}";

const char* testShader_fs = (const char*) temp_92fb9965;

//================== testShader.vs ==================
static const unsigned char temp_92fb9b55[] =
"uniform mat4 mvMatrix;\r\n"
"uniform mat4 pMatrix;\r\n"
"varying vec4 vFragColor;\r\n"
"attribute vec4 vVertex;\r\n"
"attribute vec3 vNormal;\r\n"
"uniform vec4 vColor;\r\n"
"\r\n"
"void main(void) { \r\n"
"     mat3 mNormalMatrix;\r\n"
"     mNormalMatrix[0] = mvMatrix[0].xyz;\r\n"
"     mNormalMatrix[1] = mvMatrix[1].xyz;\r\n"
"     mNormalMatrix[2] = mvMatrix[2].xyz;\r\n"
"     vec3 vNorm = normalize(mNormalMatrix * vNormal);\r\n"
"     vec3 vLightDir = vec3(0.2, -0.5, 1.0);\r\n"
"     float fDot = max(0.0, dot(vNorm, vLightDir)); \r\n"
"     vFragColor.rgb = vColor.rgb * fDot;\r\n"
"     vFragColor.a = vColor.a;\r\n"
"     mat4 mvpMatrix;\r\n"
"     mvpMatrix = pMatrix * mvMatrix;\r\n"
"     gl_Position = mvpMatrix * vVertex; \r\n"
"}";

const char* testShader_vs = (const char*) temp_92fb9b55;


const char* getNamedResource (const char*, int&) throw();
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes) throw()
{
    unsigned int hash = 0;
    if (resourceNameUTF8 != 0)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0xb1c892f5:  numBytes = 134; return testShader_fs;
        case 0xb1c894e5:  numBytes = 648; return testShader_vs;
        default: break;
    }

    numBytes = 0;
    return 0;
}

}
