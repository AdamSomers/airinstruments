/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== testShader.fs ==================
static const unsigned char temp_3176a3a3[] =
"#ifdef OPENGL_ES\n"
"precision mediump float;\n"
"#endif\n"
"varying vec4 vFragColor;\n"
"\n"
"void main(void) { \n"
"    gl_FragColor = vFragColor; \n"
"}";

const char* testShader_fs = (const char*) temp_3176a3a3;

//================== bloom.fs ==================
static const unsigned char temp_229ca099[] =
"varying vec2 vTex;\n"
"uniform sampler2D textureUnit0;\n"
"uniform vec4 vColor;\n"
"\n"
"void main(void)\n"
"{\r\n"
"    vec4 sum = vec4(0);\n"
"    vec2 texcoord = vTex;\n"
"    int j;\n"
"    int i;\n"
"    \n"
"    for( i= -4 ;i < 4; i++)\n"
"    {\n"
"        for (j = -3; j < 3; j++)\n"
"        {\n"
"            sum += texture2D(textureUnit0, texcoord + vec2(j, i)*0.004) * 0.25;\n"
"        }\n"
"    }\n"
"    if (texture2D(textureUnit0, texcoord).r < 0.3)\n"
"    {\n"
"        gl_FragColor = sum*sum*0.012 + texture2D(textureUnit0, texcoord);\n"
"    }\n"
"    else\n"
"    {\n"
"        if (texture2D(textureUnit0, texcoord).r < 0.5)\n"
"        {\n"
"            gl_FragColor = sum*sum*0.009 + texture2D(textureUnit0, texcoord);\n"
"        }\n"
"        else\n"
"        {\n"
"            gl_FragColor = sum*sum*0.0075 + texture2D(textureUnit0, texcoord);\n"
"        }\n"
"    }\n"
"\n"
"    //gl_FragColor = vColor * texture2D(textureUnit0, vTex);\n"
"}";

const char* bloom_fs = (const char*) temp_229ca099;

//================== bloom.vs ==================
static const unsigned char temp_229ca289[] =
"uniform mat4 mvpMatrix;\n"
"attribute vec4 vVertex;\n"
"attribute vec2 vTexCoord0;\n"
"varying vec2 vTex;\n"
"\n"
"void main(void)\n"
"{\n"
"    vTex = vTexCoord0;\n"
"    gl_Position = mvpMatrix * vVertex;\n"
"}";

const char* bloom_vs = (const char*) temp_229ca289;

//================== testShader.vs ==================
static const unsigned char temp_3176a593[] =
"uniform mat4 mvMatrix;\n"
"uniform mat4 pMatrix;\n"
"varying vec4 vFragColor;\n"
"attribute vec4 vVertex;\n"
"attribute vec3 vNormal;\n"
"uniform vec4 vColor;\n"
"\n"
"void main(void) { \n"
"     mat3 mNormalMatrix;\n"
"     mNormalMatrix[0] = mvMatrix[0].xyz;\n"
"     mNormalMatrix[1] = mvMatrix[1].xyz;\n"
"     mNormalMatrix[2] = mvMatrix[2].xyz;\n"
"     vec3 vNorm = normalize(mNormalMatrix * vNormal);\n"
"     vec3 vLightDir = vec3(0.2, -0.5, 1.0);\n"
"     float fDot = max(0.0, dot(vNorm, vLightDir)); \n"
"     vFragColor.rgb = vColor.rgb * fDot;\n"
"     vFragColor.a = vColor.a;\n"
"     mat4 mvpMatrix;\n"
"     mvpMatrix = pMatrix * mvMatrix;\n"
"     gl_Position = mvpMatrix * vVertex; \n"
"}";

const char* testShader_vs = (const char*) temp_3176a593;


const char* getNamedResource (const char*, int&) throw();
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes) throw()
{
    unsigned int hash = 0;
    if (resourceNameUTF8 != 0)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0xb1c892f5:  numBytes = 127; return testShader_fs;
        case 0xd90d4889:  numBytes = 828; return bloom_fs;
        case 0xd90d4a79:  numBytes = 176; return bloom_vs;
        case 0xb1c894e5:  numBytes = 628; return testShader_vs;
        default: break;
    }

    numBytes = 0;
    return 0;
}

}
