#ifdef OPENGL_ES
precision mediump float;
#endif
varying vec4 vFragColor;

void main(void) { 
    gl_FragColor = vFragColor; 
}