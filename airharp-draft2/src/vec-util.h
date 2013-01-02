static void vec_cross(GLfloat *out_result, GLfloat const *u, GLfloat const *v)
{
    out_result[0] = u[1]*v[2] - u[2]*v[1];
    out_result[1] = u[2]*v[0] - u[0]*v[2];
    out_result[2] = u[0]*v[1] - u[1]*v[0];
}

static GLfloat vec_dot(GLfloat *v, GLfloat *u)
{
    return v[0]*u[0] + v[1]*u[1] + v[2]*u[2];
}

static GLfloat vec_length(GLfloat *v)
{
    return sqrtf(vec_dot(v, v));
}

static void vec_normalize(GLfloat *inout_v)
{
    GLfloat rlen = 1.0f/vec_length(inout_v);
    inout_v[0] *= rlen;
    inout_v[1] *= rlen;
    inout_v[2] *= rlen;
}

static void vec_project(GLfloat *out_proj, GLfloat *axis, GLfloat *v)
{
    GLfloat dot = vec_dot(axis, v);
    out_proj[0] = axis[0] * dot;
    out_proj[1] = axis[1] * dot;
    out_proj[2] = axis[2] * dot;
}

static void ortho_basis(GLfloat *out_x, GLfloat *out_y, GLfloat const *normal)
{
    GLfloat ax = fabsf(normal[0]), ay = fabsf(normal[1]), az = fabsf(normal[2]);
    if (ax <= ay && ax <= az) {
        GLfloat r = 1.0f/sqrtf(normal[1] * normal[1] + normal[2] * normal[2]);
        out_x[0] =  0.0f;
        out_x[1] = -r*normal[2];
        out_x[2] =  r*normal[1];
    } else if (ay <= ax && ay <= az) {
        GLfloat r = 1.0f/sqrtf(normal[0] * normal[0] + normal[2] * normal[2]);
        out_x[0] = -r*normal[2];
        out_x[1] =  0.0f;
        out_x[2] =  r*normal[0];
    } else if (az <= ax && az <= ay) {
        GLfloat r = 1.0f/sqrtf(normal[0] * normal[0] + normal[1] * normal[1]);
        out_x[0] = -r*normal[1];
        out_x[1] =  r*normal[0];
        out_x[2] =  0.0f;
    }

    vec_cross(out_y, normal, out_x);
}

static const GLfloat IDENTITY_MATRIX[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

#ifndef M_PI
#define M_PI 3.141592653589793
#endif
