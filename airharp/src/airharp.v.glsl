#version 110

uniform mat4 p_matrix, mv_matrix, shadow_matrix;

attribute vec3 position, normal;
attribute vec2 texcoord;
attribute float shininess;
attribute vec4 specular;

varying vec3 frag_position, frag_normal;
varying vec2 frag_texcoord;
varying float frag_shininess;
varying vec4 frag_specular;
varying vec4 frag_shadow_position;

void main()
{
    vec4 homogeneous_position = vec4(position, 1.0);

    vec4 eye_position = mv_matrix * homogeneous_position;
    gl_Position = p_matrix * eye_position;
    frag_position = eye_position.xyz;
    frag_normal   = (mv_matrix * vec4(normal, 0.0)).xyz;
    frag_texcoord = texcoord;
    frag_shininess = shininess;
    frag_specular = specular;
    frag_shadow_position = shadow_matrix * homogeneous_position;
}
