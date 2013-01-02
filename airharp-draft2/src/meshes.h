#define MAX_STRINGS 20
#define MAX_FINGERS 20

struct string_mesh {
    GLuint vertex_buffer, element_buffer;
    GLsizei element_count;
    GLuint texture;
    GLuint textureOn;
    GLuint textureOff;
    GLuint stringIndex;
    GLboolean inUse;
};

struct string_vertex {
    GLfloat position[4];
    GLfloat normal[4];
    GLfloat texcoord[2];
    GLfloat shininess;
    GLubyte specular[4];
};

void init_mesh(
    struct string_mesh *out_mesh,
    struct string_vertex const *vertex_data, GLsizei vertex_count,
    GLushort const *element_data, GLsizei element_count,
    GLenum hint
);
struct string_vertex *init_string_mesh(struct string_mesh *out_mesh);
void init_background_mesh(struct string_mesh *out_mesh);
void init_bezel_mesh(struct string_mesh *out_mesh);
void init_low_bezel_mesh(struct string_mesh *out_mesh);
void update_string_mesh(
    struct string_mesh const *mesh,
    struct string_vertex *vertex_data,
    GLfloat time
);

struct string_vertex * init_finger_mesh(struct string_mesh *out_mesh);
void update_finger_mesh(
                        struct string_mesh const *mesh,
                        struct string_vertex *vertex_data,
                        GLfloat time,GLfloat x,GLfloat y,GLfloat z
                        );