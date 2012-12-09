#include <stdlib.h>
#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include "meshes.h"
#include "vec-util.h"

void init_mesh(
    struct string_mesh *out_mesh,
    struct string_vertex const *vertex_data, GLsizei vertex_count,
    GLushort const *element_data, GLsizei element_count,
    GLenum hint
) {
    glGenBuffers(1, &out_mesh->vertex_buffer);
    glGenBuffers(1, &out_mesh->element_buffer);
    out_mesh->element_count = element_count;

    glBindBuffer(GL_ARRAY_BUFFER, out_mesh->vertex_buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertex_count * sizeof(struct string_vertex),
        vertex_data,
        hint
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_mesh->element_buffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        element_count * sizeof(GLushort),
        element_data,
        GL_STATIC_DRAW
    );
}

static void calculate_string_vertex(
    struct string_vertex *v,
    GLfloat s, GLfloat t, GLfloat time
) {
    GLfloat
        sgrad[3] = {
            1.0f + 0.5f*(0.0625f+0.03125f*sinf((GLfloat)M_PI*time))*t*(t - 1.0f),
            0.0f,
            0.125f*(
                sinf(1.5f*(GLfloat)M_PI*(time + s)) 
                + s*cosf(1.5f*(GLfloat)M_PI*(time + s))*(1.5f*(GLfloat)M_PI)
            )
        },
        tgrad[3] = {
            -(0.0625f+0.03125f*sinf((GLfloat)M_PI*time))*(1.0f - s)*(2.0f*t - 1.0f),
            0.75f,
            0.0f
        };

    v->position[0] = s/45.f - (0.0625f+0.13125f*sinf((GLfloat)M_PI*time))*t*(t-1.0f);//*(1.0f - 0.5f*s);
    v->position[1] = 1.5*t-1;////0.75f*t - 0.375f;
    v->position[2] = 0;//0.125f*(s*sinf(1.5f*(GLfloat)M_PI*(time + s)));
    v->position[3] = 0.0f;

    vec_cross(v->normal, tgrad, sgrad);
    vec_normalize(v->normal);
    v->normal[3] = 0.0f;
}

#define string_X_RES 100
#define string_Y_RES 100
#define string_S_STEP (1.0f/((GLfloat)(string_X_RES - 1)))
#define string_T_STEP (1.0f/((GLfloat)(string_Y_RES - 1)))
#define string_VERTEX_COUNT (string_X_RES * string_Y_RES)

struct string_vertex *init_string_mesh(struct string_mesh *out_mesh)
{
    struct string_vertex *vertex_data
        = (struct string_vertex*) malloc(string_VERTEX_COUNT * sizeof(struct string_vertex));
    GLsizei element_count = 6 * (string_X_RES - 1) * (string_Y_RES - 1);
    GLushort *element_data
        = (GLushort*) malloc(element_count * sizeof(GLushort));
    GLsizei s, t, i;
    GLushort index;

    for (t = 0, i = 0; t < string_Y_RES; ++t)
        for (s = 0; s < string_X_RES; ++s, ++i) {
            GLfloat ss = string_S_STEP * s, tt = string_T_STEP * t;

            calculate_string_vertex(&vertex_data[i], ss, tt, 0.0f);

            vertex_data[i].texcoord[0] = ss;
            vertex_data[i].texcoord[1] = tt;
            vertex_data[i].shininess   = 0.0f;
            vertex_data[i].specular[0] = 0;
            vertex_data[i].specular[1] = 0;
            vertex_data[i].specular[2] = 0;
            vertex_data[i].specular[3] = 0;
        }

    for (t = 0, i = 0, index = 0; t < string_Y_RES - 1; ++t, ++index)
        for (s = 0; s < string_X_RES - 1; ++s, ++index) {
            element_data[i++] = index             ;
            element_data[i++] = index           +1;
            element_data[i++] = index+string_X_RES  ;
            element_data[i++] = index           +1;
            element_data[i++] = index+string_X_RES+1;
            element_data[i++] = index+string_X_RES  ;
        }

    init_mesh(
        out_mesh,
        vertex_data, string_VERTEX_COUNT,
        element_data, element_count,
        GL_STREAM_DRAW
    );

    free((void*)element_data);
    return vertex_data;
}

#define STRING_TRUCK_TOP            0.5f
#define STRING_TRUCK_CROWN          0.41f
#define STRING_TRUCK_BOTTOM         0.38f
#define STRING_SHAFT_TOP            0.3775f
#define STRING_SHAFT_BOTTOM        -1.0f
#define STRING_TRUCK_TOP_RADIUS     0.005f
#define STRING_TRUCK_CROWN_RADIUS   0.020f
#define STRING_TRUCK_BOTTOM_RADIUS  0.015f
#define STRING_SHAFT_RADIUS         0.010f
#define STRING_SHININESS            4.0f

void init_background_mesh(struct string_mesh *out_mesh)
{
    static const GLsizei STRING_RES = 16, STRING_SLICE = 6;
    GLfloat STRING_AXIS_XZ[2] = { -STRING_SHAFT_RADIUS, 0.0f };
    static const GLubyte STRING_SPECULAR[4] = { 255, 255, 192, 0 };

    GLfloat
        GROUND_LO[3] = { -0.875f, STRING_SHAFT_BOTTOM, -0.25f },
        GROUND_HI[3] = {  1.875f, STRING_SHAFT_BOTTOM,  .05f },
        WALL_LO[3] = { GROUND_LO[0], STRING_SHAFT_BOTTOM, GROUND_HI[2] },
        WALL_HI[3] = { GROUND_HI[0], STRING_SHAFT_BOTTOM + 2.0f, GROUND_HI[2] };

    static GLfloat
        TEX_STRING_LO[2] = { 0.0f,    0.0f },
        TEX_STRING_HI[2] = { 0.03125f,  1.0f },
        TEX_GROUND_LO[2]   = { 0.f,  0.f },
        TEX_GROUND_HI[2]   = { 0.99f, 0.9921875f },
        TEX_WALL_LO[2]     = { 0.99f, 0.99f },
        TEX_WALL_HI[2]     = { 0.0f,      0.f };

#define _STRING_T(y) \
    (TEX_STRING_LO[1] \
        + (TEX_STRING_HI[1] - TEX_STRING_LO[1]) \
        * ((y) - STRING_TRUCK_TOP)/(STRING_SHAFT_BOTTOM - STRING_TRUCK_TOP) \
    )

    GLfloat
        theta_step = 2.0f * (GLfloat)M_PI / (GLfloat)STRING_RES,
        s_step = (TEX_STRING_HI[0] - TEX_STRING_LO[0]) / (GLfloat)STRING_RES,
        t_truck_top    = TEX_STRING_LO[1],
        t_truck_crown  = _STRING_T(STRING_TRUCK_CROWN),
        t_truck_bottom = _STRING_T(STRING_TRUCK_BOTTOM),
        t_shaft_top    = _STRING_T(STRING_SHAFT_TOP),
        t_shaft_bottom = _STRING_T(STRING_SHAFT_BOTTOM);

#undef _STRING_T

    GLsizei
        stringpole_vertex_count = 2 + STRING_RES * STRING_SLICE,
        wall_vertex_count = 4,
        ground_vertex_count = 4,
        vertex_count = wall_vertex_count
            + ground_vertex_count;

    GLsizei vertex_i = 0, element_i, i;

    GLsizei
        wall_element_count = 6,
        ground_element_count = 6,
        element_count = wall_element_count
            + ground_element_count;

    struct string_vertex *vertex_data
        = (struct string_vertex*) malloc(vertex_count * sizeof(struct string_vertex));

    GLushort *element_data
        = (GLushort*) malloc(element_count * sizeof(GLushort));

    vertex_data[0].position[0] = GROUND_LO[0];
    vertex_data[0].position[1] = GROUND_LO[1];
    vertex_data[0].position[2] = GROUND_LO[2];
    vertex_data[0].position[3] = 1.0f;
    vertex_data[0].normal[0]   = 0.0f;
    vertex_data[0].normal[1]   = 1.0f;
    vertex_data[0].normal[2]   = 0.0f;
    vertex_data[0].normal[3]   = 0.0f;
    vertex_data[0].texcoord[0] = TEX_GROUND_LO[0];
    vertex_data[0].texcoord[1] = TEX_GROUND_LO[1];
    vertex_data[0].shininess   = 0.0f;
    vertex_data[0].specular[0] = 0;
    vertex_data[0].specular[1] = 0;
    vertex_data[0].specular[2] = 0;
    vertex_data[0].specular[3] = 0;

    vertex_data[1].position[0] = GROUND_HI[0];
    vertex_data[1].position[1] = GROUND_LO[1];
    vertex_data[1].position[2] = GROUND_LO[2];
    vertex_data[1].position[3] = 1.0f;
    vertex_data[1].normal[0]   = 0.0f;
    vertex_data[1].normal[1]   = 1.0f;
    vertex_data[1].normal[2]   = 0.0f;
    vertex_data[1].normal[3]   = 0.0f;
    vertex_data[1].texcoord[0] = TEX_GROUND_HI[0];
    vertex_data[1].texcoord[1] = TEX_GROUND_LO[1];
    vertex_data[1].shininess   = 0.0f;
    vertex_data[1].specular[0] = 0;
    vertex_data[1].specular[1] = 0;
    vertex_data[1].specular[2] = 0;
    vertex_data[1].specular[3] = 0;

    vertex_data[2].position[0] = GROUND_HI[0];
    vertex_data[2].position[1] = GROUND_LO[1];
    vertex_data[2].position[2] = GROUND_HI[2];
    vertex_data[2].position[3] = 1.0f;
    vertex_data[2].normal[0]   = 0.0f;
    vertex_data[2].normal[1]   = 1.0f;
    vertex_data[2].normal[2]   = 0.0f;
    vertex_data[2].normal[3]   = 0.0f;
    vertex_data[2].texcoord[0] = TEX_GROUND_HI[0];
    vertex_data[2].texcoord[1] = TEX_GROUND_HI[1];
    vertex_data[2].shininess   = 0.0f;
    vertex_data[2].specular[0] = 0;
    vertex_data[2].specular[1] = 0;
    vertex_data[2].specular[2] = 0;
    vertex_data[2].specular[3] = 0;

    vertex_data[3].position[0] = GROUND_LO[0];
    vertex_data[3].position[1] = GROUND_LO[1];
    vertex_data[3].position[2] = GROUND_HI[2];
    vertex_data[3].position[3] = 1.0f;
    vertex_data[3].normal[0]   = 0.0f;
    vertex_data[3].normal[1]   = 1.0f;
    vertex_data[3].normal[2]   = 0.0f;
    vertex_data[3].normal[3]   = 0.0f;
    vertex_data[3].texcoord[0] = TEX_GROUND_LO[0];
    vertex_data[3].texcoord[1] = TEX_GROUND_HI[1];
    vertex_data[3].shininess   = 0.0f;
    vertex_data[3].specular[0] = 0;
    vertex_data[3].specular[1] = 0;
    vertex_data[3].specular[2] = 0;
    vertex_data[3].specular[3] = 0;

    vertex_data[4].position[0] = WALL_LO[0];
    vertex_data[4].position[1] = WALL_LO[1];
    vertex_data[4].position[2] = WALL_LO[2];
    vertex_data[4].position[3] = 1.0f;
    vertex_data[4].normal[0]   = 0.0f;
    vertex_data[4].normal[1]   = 0.0f;
    vertex_data[4].normal[2]   = -1.0f;
    vertex_data[4].normal[3]   = 0.0f;
    vertex_data[4].texcoord[0] = TEX_WALL_LO[0];
    vertex_data[4].texcoord[1] = TEX_WALL_LO[1];
    vertex_data[4].shininess   = 0.0f;
    vertex_data[4].specular[0] = 0;
    vertex_data[4].specular[1] = 0;
    vertex_data[4].specular[2] = 0;
    vertex_data[4].specular[3] = 0;

    vertex_data[5].position[0] = WALL_HI[0];
    vertex_data[5].position[1] = WALL_LO[1];
    vertex_data[5].position[2] = WALL_LO[2];
    vertex_data[5].position[3] = 1.0f;
    vertex_data[5].normal[0]   = 0.0f;
    vertex_data[5].normal[1]   = 0.0f;
    vertex_data[5].normal[2]   = -1.0f;
    vertex_data[5].normal[3]   = 0.0f;
    vertex_data[5].texcoord[0] = TEX_WALL_HI[0];
    vertex_data[5].texcoord[1] = TEX_WALL_LO[1];
    vertex_data[5].shininess   = 0.0f;
    vertex_data[5].specular[0] = 0;
    vertex_data[5].specular[1] = 0;
    vertex_data[5].specular[2] = 0;
    vertex_data[5].specular[3] = 0;

    vertex_data[6].position[0] = WALL_HI[0];
    vertex_data[6].position[1] = WALL_HI[1];
    vertex_data[6].position[2] = WALL_LO[2];
    vertex_data[6].position[3] = 1.0f;
    vertex_data[6].normal[0]   = 0.0f;
    vertex_data[6].normal[1]   = 0.0f;
    vertex_data[6].normal[2]   = -1.0f;
    vertex_data[6].normal[3]   = 0.0f;
    vertex_data[6].texcoord[0] = TEX_WALL_HI[0];
    vertex_data[6].texcoord[1] = TEX_WALL_HI[1];
    vertex_data[6].shininess   = 0.0f;
    vertex_data[6].specular[0] = 0;
    vertex_data[6].specular[1] = 0;
    vertex_data[6].specular[2] = 0;
    vertex_data[6].specular[3] = 0;

    vertex_data[7].position[0] = WALL_LO[0];
    vertex_data[7].position[1] = WALL_HI[1];
    vertex_data[7].position[2] = WALL_LO[2];
    vertex_data[7].position[3] = 1.0f;
    vertex_data[7].normal[0]   = 0.0f;
    vertex_data[7].normal[1]   = 0.0f;
    vertex_data[7].normal[2]   = -1.0f;
    vertex_data[7].normal[3]   = 0.0f;
    vertex_data[7].texcoord[0] = TEX_WALL_LO[0];
    vertex_data[7].texcoord[1] = TEX_WALL_HI[1];
    vertex_data[7].shininess   = 0.0f;
    vertex_data[7].specular[0] = 0;
    vertex_data[7].specular[1] = 0;
    vertex_data[7].specular[2] = 0;
    vertex_data[7].specular[3] = 0;

    vertex_data[8].position[0] = STRING_AXIS_XZ[0];
    vertex_data[8].position[1] = STRING_TRUCK_TOP;
    vertex_data[8].position[2] = STRING_AXIS_XZ[1];
    vertex_data[8].position[3] = 1.0f;
    vertex_data[8].normal[0]   = 0.0f;
    vertex_data[8].normal[1]   = 1.0f;
    vertex_data[8].normal[2]   = 0.0f;
    vertex_data[8].normal[3]   = 0.0f;
    vertex_data[8].texcoord[0] = TEX_STRING_LO[0];
    vertex_data[8].texcoord[1] = t_truck_top;
    vertex_data[8].shininess   = STRING_SHININESS;
    vertex_data[8].specular[0] = 0;
    vertex_data[8].specular[1] = 0;
    vertex_data[8].specular[2] = 0;
    vertex_data[8].specular[3] = 0;


    element_i = 0;

    element_data[element_i++] = 0;
    element_data[element_i++] = 1;
    element_data[element_i++] = 2;

    element_data[element_i++] = 0;
    element_data[element_i++] = 2;
    element_data[element_i++] = 3;

    element_data[element_i++] = 4;
    element_data[element_i++] = 5;
    element_data[element_i++] = 6;

    element_data[element_i++] = 4;
    element_data[element_i++] = 6;
    element_data[element_i++] = 7;

 

    init_mesh(
        out_mesh,
        vertex_data, vertex_count,
        element_data, element_count,
        GL_STATIC_DRAW
    );

    free(element_data);
    free(vertex_data);
}

void update_string_mesh(
    struct string_mesh const *mesh,
    struct string_vertex *vertex_data,
    GLfloat time
) {
    GLsizei s, t, i;
    for (t = 0, i = 0; t < string_Y_RES; ++t)
        for (s = 0; s < string_X_RES; ++s, ++i) {
            GLfloat ss = string_S_STEP * s, tt = string_T_STEP * t;

            calculate_string_vertex(&vertex_data[i], ss, tt, time);
        }

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        string_VERTEX_COUNT * sizeof(struct string_vertex),
        vertex_data,
        GL_STREAM_DRAW
    );
}

