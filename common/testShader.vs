uniform mat4 mvMatrix;
uniform mat4 pMatrix;
varying vec4 vFragColor;
attribute vec4 vVertex;
attribute vec3 vNormal;
uniform vec4 vColor;

void main(void) { 
     mat3 mNormalMatrix;
     mNormalMatrix[0] = mvMatrix[0].xyz;
     mNormalMatrix[1] = mvMatrix[1].xyz;
     mNormalMatrix[2] = mvMatrix[2].xyz;
     vec3 vNorm = normalize(mNormalMatrix * vNormal);
     vec3 vLightDir = vec3(0.0, -0.5, 1.0);
     float fDot = max(0.0, dot(vNorm, vLightDir)); 
     vFragColor.rgb = vColor.rgb * fDot;
     vFragColor.a = vColor.a;
     mat4 mvpMatrix;
     mvpMatrix = pMatrix * mvMatrix;
     gl_Position = mvpMatrix * vVertex; 
}