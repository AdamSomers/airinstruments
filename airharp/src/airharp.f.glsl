#version 110

uniform mat4 p_matrix, mv_matrix, shadow_matrix;
uniform vec3 light_direction;
uniform sampler2D texture, shadowmap, texture2;

varying vec3 frag_position, frag_normal;
varying vec2 frag_texcoord;
varying float frag_shininess;
varying vec4 frag_specular;
varying vec4 frag_shadow_position;

const vec4 light_diffuse = vec4(0.8, 0.8, 0.8, 0.0);
const vec4 light_ambient = vec4(0.8, 0.8, 0.8, 1.0);
const vec4 light_specular = vec4(1.0, 1.0, 1.0, 1.0);

const float SHADOW_EPSILON = 0.01;

void main()
{
    vec3 mv_light_direction = (mv_matrix * vec4(light_direction, 0.0)).xyz,
         normal = normalize(frag_normal),
         eye = normalize(frag_position),
         reflection = reflect(mv_light_direction, normal);

    vec3 shadow_coord = vec3(
        frag_shadow_position.x,
        frag_shadow_position.y,
        frag_shadow_position.z
    )/frag_shadow_position.w;

    vec4 diffuse_factor, specular_factor;
    vec4 frag_diffuse = texture2D(texture, frag_texcoord);

    float shadow_depth = texture2D(shadowmap, shadow_coord.xy*0.5 + vec2(0.5)).x;
    shadow_depth = shadow_depth * 2.0 - 1.0 + SHADOW_EPSILON;
    if (shadow_coord.z <= shadow_depth) {
        diffuse_factor
            = max(-dot(normal, mv_light_direction), 0.0) * light_diffuse;
        specular_factor
            = max(pow(-dot(reflection, eye), frag_shininess), 0.0) * light_specular;
    } else {
        diffuse_factor = vec4(0.0);
        specular_factor = vec4(0.0);
    }
    vec4 ambient_diffuse_factor = diffuse_factor + light_ambient;
    
    gl_FragColor = specular_factor * frag_specular
        + ambient_diffuse_factor * frag_diffuse;
}
