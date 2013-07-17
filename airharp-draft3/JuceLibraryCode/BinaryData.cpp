/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== bloom.fs ==================
static const unsigned char temp_229ca099[] =
"varying vec2 vTex;\n"
"uniform sampler2D textureUnit0;\n"
"uniform vec4 vColor;\n"
"\n"
"void main(void)\n"
"{\n"
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
"}\n"
"\n"
"/*\n"
" void main()\n"
" {\n"
" float pixelWidth = 1.0 / 512.0;\n"
" float pixelHeight = 1.0 / 512.0;\n"
" \n"
" // Current texture coordinate\n"
" vec2 texel = vTex;\n"
" vec4 pixel = vec4(texture2D(textureUnit0, texel));\n"
" \n"
" // Larger constant = bigger glow\n"
" float glow = 4.0 * ((pixelWidth + pixelHeight) / 2.0);\n"
" \n"
" // The vector to contain the new, \"bloomed\" colour values\n"
" vec4 bloom = vec4(0);\n"
" \n"
" // Loop over all the pixels on the texture in the area given by the constant in glow\n"
" float count = 0.0;\n"
" for(float x = texel.x - glow; x < texel.x + glow; x += pixelWidth)\n"
" {\n"
" for(float y = texel.y - glow; y < texel.y + glow; y += pixelHeight)\n"
" {\n"
" // Add that pixel's value to the bloom vector\n"
" bloom += (texture2D(textureUnit0, vec2(x, y)) - 0.4) * 30.0;\n"
" // Add 1 to the number of pixels sampled\n"
" count = count + 1.0;\n"
" }\n"
" }\n"
" // Divide by the number of pixels sampled to average out the value\n"
" // The constant being multiplied with count here will dim the bloom effect a bit, with higher values\n"
" // Clamp the value between a 0.0 to 1.0 range\n"
" vec4 b = bloom;// / (count * 30.0);\n"
" b = clamp(b, 0.0, 1.0);\n"
" \n"
" // Set the current fragment to the original texture pixel, with our bloom value added on\n"
" gl_FragColor = pixel + b;\n"
" }\n"
"*/";

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
        case 0xd90d4889:  numBytes = 2029; return bloom_fs;
        case 0xd90d4a79:  numBytes = 176; return bloom_vs;
        case 0xb1c892f5:  numBytes = 127; return testShader_fs;
        case 0xb1c894e5:  numBytes = 628; return testShader_vs;
        default: break;
    }

    numBytes = 0;
    return 0;
}

}
