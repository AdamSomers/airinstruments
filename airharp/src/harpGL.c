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
#include "file-util.h"
#include "gl-util.h"
#include "vec-util.h"
#include "meshes.h"

static const int STRING_SHADOWMAP_RESOLUTION = 4096;

struct string_attributes {
    GLint position, normal, texcoord, shininess, specular;
};

struct string_shaders {
    GLuint vertex_shader, shadowmap_fragment_shader, string_fragment_shader;
    GLuint shadowmap_program, string_program;
};

static struct {
    struct string_mesh string, background;
    struct string_vertex *string_vertex_array;
    GLuint shadowmap_texture;
    GLuint shadowmap_framebuffer;

    struct string_shaders shaders;

    struct {
        struct {
            GLint p_matrix, mv_matrix, shadow_matrix;
            GLint texture, shadowmap, light_direction;
        } uniforms;

        struct string_attributes attributes;
    } string_program;

    struct {
        struct {
            GLint p_matrix, mv_matrix, shadow_matrix;
        } uniforms;

        struct string_attributes attributes;
    } shadowmap_program;

    GLfloat p_matrix[16], shadow_matrix[16], mv_matrix[16];
    GLfloat light_direction[3];
    GLfloat eye_offset[2];
    GLsizei window_size[2];
} g_resources;

static void init_gl_state(void)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

#define PROJECTION_FOV_RATIO 0.7f
#define PROJECTION_NEAR_PLANE 0.0625f
#define PROJECTION_FAR_PLANE 256.0f

static void update_p_matrix(GLfloat *matrix, int w, int h)
{
    GLfloat wf = (GLfloat)w, hf = (GLfloat)h;
    GLfloat
        r_xy_factor = fminf(wf, hf) * 1.0f/PROJECTION_FOV_RATIO,
        r_x = r_xy_factor/wf,
        r_y = r_xy_factor/hf,
        r_zw_factor = 1.0f/(PROJECTION_FAR_PLANE - PROJECTION_NEAR_PLANE),
        r_z = (PROJECTION_NEAR_PLANE + PROJECTION_FAR_PLANE)*r_zw_factor,
        r_w = -2.0f*PROJECTION_NEAR_PLANE*PROJECTION_FAR_PLANE*r_zw_factor;

    matrix[ 0] = r_x;  matrix[ 1] = 0.0f; matrix[ 2] = 0.0f; matrix[ 3] = 0.0f;
    matrix[ 4] = 0.0f; matrix[ 5] = r_y;  matrix[ 6] = 0.0f; matrix[ 7] = 0.0f;
    matrix[ 8] = 0.0f; matrix[ 9] = 0.0f; matrix[10] = r_z;  matrix[11] = 1.0f;
    matrix[12] = 0.0f; matrix[13] = 0.0f; matrix[14] = r_w;  matrix[15] = 0.0f;
}

static void update_shadow_matrix(GLfloat *matrix, GLfloat const *light_direction)
{
    static const GLfloat SHADOW_RADIUS    = 2.2f;
    static const GLfloat SHADOW_OFFSET[3] = { -0.3f, -0.2f, 0.0f };

    GLfloat x[3], y[3];
    ortho_basis(x, y, light_direction);

    GLfloat recip_radius = 1.0f/SHADOW_RADIUS;

    matrix[ 0] = x[0]*recip_radius; 
    matrix[ 1] = y[0]*recip_radius;
    matrix[ 2] = light_direction[0]*recip_radius;
    matrix[ 3] = 0.0f;

    matrix[ 4] = x[1]*recip_radius; 
    matrix[ 5] = y[1]*recip_radius;
    matrix[ 6] = light_direction[1]*recip_radius;
    matrix[ 7] = 0.0f;

    matrix[ 8] = x[2]*recip_radius; 
    matrix[ 9] = y[2]*recip_radius;
    matrix[10] = light_direction[2]*recip_radius;
    matrix[11] = 0.0f;

    matrix[12] = SHADOW_OFFSET[0];
    matrix[13] = SHADOW_OFFSET[1];
    matrix[14] = SHADOW_OFFSET[2];
    matrix[15] = 1.0f;
}

static void update_mv_matrix(GLfloat *matrix, GLfloat *eye_offset)
{
    static const GLfloat BASE_EYE_POSITION[3]  = { 0.5f, -0.25f, -1.1f  };

    matrix[ 0] = 1.0f; matrix[ 1] = 0.0f; matrix[ 2] = 0.0f; matrix[ 3] = 0.0f;
    matrix[ 4] = 0.0f; matrix[ 5] = 1.0f; matrix[ 6] = 0.0f; matrix[ 7] = 0.0f;
    matrix[ 8] = 0.0f; matrix[ 9] = 0.0f; matrix[10] = 1.0f; matrix[11] = 0.0f;
    matrix[12] = -BASE_EYE_POSITION[0] - eye_offset[0];
    matrix[13] = -BASE_EYE_POSITION[1] - eye_offset[1];
    matrix[14] = -BASE_EYE_POSITION[2];
    matrix[15] = 1.0f;
}

static void render_mesh(
    struct string_mesh const *mesh,
    struct string_attributes const *attributes
) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh->texture);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);
    glVertexAttribPointer(
        attributes->position,
        3, GL_FLOAT, GL_FALSE, sizeof(struct string_vertex),
        (void*)offsetof(struct string_vertex, position)
    );
    glVertexAttribPointer(
        attributes->normal,
        3, GL_FLOAT, GL_FALSE, sizeof(struct string_vertex),
        (void*)offsetof(struct string_vertex, normal)
    );
    glVertexAttribPointer(
        attributes->texcoord,
        2, GL_FLOAT, GL_FALSE, sizeof(struct string_vertex),
        (void*)offsetof(struct string_vertex, texcoord)
    );
    glVertexAttribPointer(
        attributes->shininess,
        1, GL_FLOAT, GL_FALSE, sizeof(struct string_vertex),
        (void*)offsetof(struct string_vertex, shininess)
    );
    glVertexAttribPointer(
        attributes->specular,
        4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct string_vertex),
        (void*)offsetof(struct string_vertex, specular)
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->element_buffer);
    glDrawElements(
        GL_TRIANGLES,
        mesh->element_count,
        GL_UNSIGNED_SHORT,
        (void*)0
    );
}

static void enable_mesh_vertex_attributes(struct string_attributes const *attributes)
{
    glEnableVertexAttribArray(attributes->position);
    glEnableVertexAttribArray(attributes->normal);
    glEnableVertexAttribArray(attributes->texcoord);
    glEnableVertexAttribArray(attributes->shininess);
    glEnableVertexAttribArray(attributes->specular);
}

static void disable_mesh_vertex_attributes(struct string_attributes const *attributes)
{
    glDisableVertexAttribArray(attributes->position);
    glDisableVertexAttribArray(attributes->normal);
    glDisableVertexAttribArray(attributes->texcoord);
    glDisableVertexAttribArray(attributes->shininess);
    glDisableVertexAttribArray(attributes->specular);
}

#define INITIAL_WINDOW_WIDTH  640
#define INITIAL_WINDOW_HEIGHT 480

static void enact_string_programs(struct string_shaders const *shaders)
{
    g_resources.shaders = *shaders;

    g_resources.string_program.uniforms.texture
        = glGetUniformLocation(shaders->string_program, "texture");
    g_resources.string_program.uniforms.shadowmap
        = glGetUniformLocation(shaders->string_program, "shadowmap");
    g_resources.string_program.uniforms.p_matrix
        = glGetUniformLocation(shaders->string_program, "p_matrix");
    g_resources.string_program.uniforms.mv_matrix
        = glGetUniformLocation(shaders->string_program, "mv_matrix");
    g_resources.string_program.uniforms.shadow_matrix
        = glGetUniformLocation(shaders->string_program, "shadow_matrix");
    g_resources.string_program.uniforms.light_direction
        = glGetUniformLocation(shaders->string_program, "light_direction");

    g_resources.string_program.attributes.position
        = glGetAttribLocation(shaders->string_program, "position");
    g_resources.string_program.attributes.normal
        = glGetAttribLocation(shaders->string_program, "normal");
    g_resources.string_program.attributes.texcoord
        = glGetAttribLocation(shaders->string_program, "texcoord");
    g_resources.string_program.attributes.shininess
        = glGetAttribLocation(shaders->string_program, "shininess");
    g_resources.string_program.attributes.specular
        = glGetAttribLocation(shaders->string_program, "specular");

    g_resources.shadowmap_program.uniforms.p_matrix
        = glGetUniformLocation(shaders->shadowmap_program, "p_matrix");
    g_resources.shadowmap_program.uniforms.mv_matrix
        = glGetUniformLocation(shaders->shadowmap_program, "mv_matrix");
    g_resources.shadowmap_program.uniforms.shadow_matrix
        = glGetUniformLocation(shaders->shadowmap_program, "shadow_matrix");

    g_resources.shadowmap_program.attributes.position
        = glGetAttribLocation(shaders->shadowmap_program, "position");
    g_resources.shadowmap_program.attributes.normal
        = glGetAttribLocation(shaders->shadowmap_program, "normal");
    g_resources.shadowmap_program.attributes.texcoord
        = glGetAttribLocation(shaders->shadowmap_program, "texcoord");
    g_resources.shadowmap_program.attributes.shininess
        = glGetAttribLocation(shaders->shadowmap_program, "shininess");
    g_resources.shadowmap_program.attributes.specular
        = glGetAttribLocation(shaders->shadowmap_program, "specular");
}

static int make_string_programs(struct string_shaders *out_shaders)
{
    out_shaders->vertex_shader = make_shader(GL_VERTEX_SHADER, "airharp.v.glsl");
    if (out_shaders->vertex_shader == 0)
        return 0;
    out_shaders->string_fragment_shader = make_shader(GL_FRAGMENT_SHADER, "airharp.f.glsl");
    if (out_shaders->string_fragment_shader == 0)
        return 0;
    out_shaders->shadowmap_fragment_shader
        = make_shader(GL_FRAGMENT_SHADER, "airharp-shadow-map.f.glsl");
    if (out_shaders->shadowmap_fragment_shader == 0)
        return 0;

    out_shaders->string_program
        = make_program(out_shaders->vertex_shader, out_shaders->string_fragment_shader);
    if (out_shaders->string_program == 0)
        return 0;

    out_shaders->shadowmap_program
        = make_program(out_shaders->vertex_shader, out_shaders->shadowmap_fragment_shader);
    if (out_shaders->shadowmap_program == 0)
        return 0;

    return 1;
}

static void delete_string_programs(struct string_shaders const *shaders)
{
    glDetachShader(
        shaders->string_program,
        shaders->vertex_shader
    );
    glDetachShader(
        shaders->string_program,
        shaders->string_fragment_shader
    );
    glDetachShader(
        shaders->shadowmap_program,
        shaders->vertex_shader
    );
    glDetachShader(
        shaders->shadowmap_program,
        shaders->shadowmap_fragment_shader
    );
    glDeleteProgram(shaders->string_program);
    glDeleteProgram(shaders->shadowmap_program);
    glDeleteShader(shaders->vertex_shader);
    glDeleteShader(shaders->string_fragment_shader);
    glDeleteShader(shaders->shadowmap_fragment_shader);
}

static void update_string_programs(void)
{
    printf("reloading program\n");
    struct string_shaders shaders;

    if (make_string_programs(&shaders)) {
        delete_string_programs(&g_resources.shaders);
        enact_string_programs(&shaders);
    }
}

static int make_shadow_framebuffer(GLuint *out_texture, GLuint *out_framebuffer)
{
    glGenTextures(1, out_texture);
    glBindTexture(GL_TEXTURE_2D, *out_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
        GL_TEXTURE_2D, 0,           /* target, level */
        GL_DEPTH_COMPONENT,         /* internal format */
        STRING_SHADOWMAP_RESOLUTION,  /* width */
        STRING_SHADOWMAP_RESOLUTION,  /* height */
        0,                          /* border */
        GL_DEPTH_COMPONENT,         /* external format */
        GL_UNSIGNED_BYTE,           /* type */
        NULL                        /* pixels */
    );

    glGenFramebuffersEXT(1, out_framebuffer);
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, *out_framebuffer);
    glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, *out_framebuffer);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glFramebufferTexture2DEXT(
        GL_DRAW_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
        GL_TEXTURE_2D, *out_texture, 0
    );

    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
    glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
}

static int make_resources(void)
{
    GLuint vertex_shader, fragment_shader, program;

    g_resources.string_vertex_array = init_string_mesh(&g_resources.string);
    init_background_mesh(&g_resources.background);

    g_resources.string.texture = make_texture("string.tga");
    g_resources.background.texture = make_texture("bluegradient.tga");

    if (!make_shadow_framebuffer(
        &g_resources.shadowmap_texture,
        &g_resources.shadowmap_framebuffer
    )) {
        return 0;
    }

    if (g_resources.string.texture == 0 || g_resources.background.texture == 0)
        return 0;

    struct string_shaders shaders;

    if (!make_string_programs(&shaders))
        return 0;

    enact_string_programs(&shaders);

    g_resources.eye_offset[0] = 0.0f;
    g_resources.eye_offset[1] = 0.0f;
    g_resources.window_size[0] = INITIAL_WINDOW_WIDTH;
    g_resources.window_size[1] = INITIAL_WINDOW_HEIGHT;

    g_resources.light_direction[0] =  0.408248;
    g_resources.light_direction[1] = -0.316497;
    g_resources.light_direction[2] =  0.408248;

    update_p_matrix(
        g_resources.p_matrix,
        INITIAL_WINDOW_WIDTH,
        INITIAL_WINDOW_HEIGHT
    );
    update_mv_matrix(g_resources.mv_matrix, g_resources.eye_offset);
    update_shadow_matrix(g_resources.shadow_matrix, g_resources.light_direction);

    return 1;
}

static void update(void)
{
    int milliseconds = glutGet(GLUT_ELAPSED_TIME);
    GLfloat seconds = (GLfloat)milliseconds * (1.0f/1000.0f);

    update_string_mesh(&g_resources.string, g_resources.string_vertex_array, seconds);
    glutPostRedisplay();
    usleep(10000);
}

static void drag(int x, int y)
{
    float w = (float)g_resources.window_size[0];
    float h = (float)g_resources.window_size[1];
    g_resources.eye_offset[0] = (float)x/w - 0.5f;
    g_resources.eye_offset[1] = -(float)y/h + 0.5f;
    update_mv_matrix(g_resources.mv_matrix, g_resources.eye_offset);
}

static void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        g_resources.eye_offset[0] = 0.0f;
        g_resources.eye_offset[1] = 0.0f;
        update_mv_matrix(g_resources.mv_matrix, g_resources.eye_offset);
    }
}

static void keyboard(unsigned char key, int x, int y)
{
    if (key == 'r' || key == 'R') {
        update_string_programs();
    }
}

static void reshape(int w, int h)
{
    g_resources.window_size[0] = w;
    g_resources.window_size[1] = h;
    update_p_matrix(g_resources.p_matrix, w, h);
}

static void render_scene(struct string_attributes const *attributes)
{
    enable_mesh_vertex_attributes(attributes);
    render_mesh(&g_resources.string, attributes);
    render_mesh(&g_resources.background, attributes);
    disable_mesh_vertex_attributes(attributes);
}

static void render_shadowmap()
{
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, g_resources.shadowmap_framebuffer);
    glViewport(0, 0, STRING_SHADOWMAP_RESOLUTION, STRING_SHADOWMAP_RESOLUTION);

    glClear(GL_DEPTH_BUFFER_BIT);

    glUseProgram(g_resources.shaders.shadowmap_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUniformMatrix4fv(
        g_resources.shadowmap_program.uniforms.p_matrix,
        1, GL_FALSE,
        IDENTITY_MATRIX
    );

    glUniformMatrix4fv(
        g_resources.shadowmap_program.uniforms.mv_matrix,
        1, GL_FALSE,
        g_resources.shadow_matrix
    );

    glUniformMatrix4fv(
        g_resources.shadowmap_program.uniforms.shadow_matrix,
        1, GL_FALSE,
        IDENTITY_MATRIX
    );

    render_scene(&g_resources.shadowmap_program.attributes);
}

static void render_string()
{
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
    glViewport(0, 0, g_resources.window_size[0], g_resources.window_size[1]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(g_resources.shaders.string_program);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_resources.shadowmap_texture);

    glUniform1i(g_resources.string_program.uniforms.texture, 0);
    glUniform1i(g_resources.string_program.uniforms.shadowmap, 1);

    glUniformMatrix4fv(
        g_resources.string_program.uniforms.p_matrix,
        1, GL_FALSE,
        g_resources.p_matrix
    );

    glUniformMatrix4fv(
        g_resources.string_program.uniforms.mv_matrix,
        1, GL_FALSE,
        g_resources.mv_matrix
    );

    glUniformMatrix4fv(
        g_resources.string_program.uniforms.shadow_matrix,
        1, GL_FALSE,
        g_resources.shadow_matrix
    );

    glUniform3fv(
        g_resources.string_program.uniforms.light_direction,
        1, 
        g_resources.light_direction
    );

    render_scene(&g_resources.string_program.attributes);
}

static void render()
{
    render_shadowmap();
    render_string();

    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    glutCreateWindow("Flag");
    glutIdleFunc(&update);
    glutDisplayFunc(&render);
    glutReshapeFunc(&reshape);
    glutMotionFunc(&drag);
    glutMouseFunc(&mouse);
    glutKeyboardFunc(&keyboard);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glewInit();
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "OpenGL 2.0 not available\n");
        return 1;
    }
    if (!GLEW_EXT_framebuffer_object) {
        fprintf(stderr, "OpenGL framebuffer object extension not available\n");
        return 1;
    }

    init_gl_state();
    if (!make_resources()) {
        fprintf(stderr, "Failed to load resources\n");
        return 1;
    }

    glutMainLoop();
    return 0;
}
