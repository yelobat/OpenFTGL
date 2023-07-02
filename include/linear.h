#ifndef FTGL_LINEAR_H_
#define FTGL_LINEAR_H_

//requires -lm flag to link the math library
#include <math.h>

typedef enum matrix_type_t {
        LL_MATRIX_MODEL,
        LL_MATRIX_VIEW,
        LL_MATRIX_PROJECTION,
        LL_MATRIX_COUNT
} matrix_type_t;

typedef union vec2_t {
        float data[2];
        struct {
                float x;
                float y;
        };
        struct {
                float r;
                float g;
        };
        struct {
                float s;
                float t;
        };
} vec2_t;

typedef union vec3_t {
        float data[3];
        struct {
                float x;
                float y;
                float z;
        };
        struct {
                float r;
                float g;
                float b;
        };        
} vec3_t;

typedef union vec4_t {
        float data[4];
        struct {
                float x;
                float y;
                float z;
                float w;
        };
        struct {
                float r;
                float g;
                float b;
                float a;
        };
} vec4_t;

typedef union ivec2_t {
        int data[2];
        struct {
                int x;
                int y;
        };
        struct {
                int r;
                int g;
        };
        struct {
                int s;
                int t;
        };
} ivec2_t;

typedef union ivec3_t {
        int data[3];
        struct {
                int x;
                int y;
                int z;
        };
        struct {
                int r;
                int g;
                int b;
        };
} ivec3_t;

typedef union ivec4_t {
        int data[4];
        struct {
                int x;
                int y;
                int z;
                int w;
        };
        struct {
                int r;
                int g;
                int b;
                int a;
        };
} ivec4_t;

typedef union mat2_t {
        float data[4];
        struct {
                float m00, m01;
                float m10, m11;
        };
} mat2_t;

typedef union mat3_t {
        float data[9];
        struct {
                float m00, m01, m02;
                float m10, m11, m12;
                float m20, m21, m22;
        };
} mat3_t;

typedef union mat4_t {
        float data[16];
        struct {
                float m00, m01, m02, m03;
                float m10, m11, m12, m13;
                float m20, m21, m22, m23;
                float m30, m31, m32, m33;
        };
} mat4_t;

vec2_t
ll_vec2_create(float x, float y);
float
ll_vec2_length(vec2_t vec);
vec2_t
ll_vec2_add(vec2_t left, vec2_t right);
vec2_t
ll_vec2_sub(vec2_t left, vec2_t right);
vec2_t
ll_vec2_mul(vec2_t left, vec2_t right);
vec2_t
ll_vec2_div(vec2_t left, vec2_t right);
float
ll_vec2_dot(vec2_t left, vec2_t right);
float
ll_vec2_cross(vec2_t left, vec2_t right);
vec2_t
ll_vec2_normalise(vec2_t vec);

vec3_t
ll_vec3_create(float x, float y, float z);
float
ll_vec3_length(vec3_t vec);
vec3_t
ll_vec3_add(vec3_t left, vec3_t right);
vec3_t
ll_vec3_sub(vec3_t left, vec3_t right);
vec3_t
ll_vec3_mul(vec3_t left, vec3_t right);
vec3_t
ll_vec3_div(vec3_t left, vec3_t right);
float
ll_vec3_dot(vec3_t left, vec3_t right);
vec3_t
ll_vec3_cross(vec3_t left, vec3_t right);
vec3_t
ll_vec3_normalise(vec3_t vec);

vec4_t
ll_vec4_create(float x, float y, float z, float w);
float
ll_vec4_length(vec4_t vec);
vec4_t
ll_vec4_add(vec4_t left, vec4_t right);
vec4_t
ll_vec4_sub(vec4_t left, vec4_t right);
vec4_t
ll_vec4_mul(vec4_t left, vec4_t right);
vec4_t
ll_vec4_div(vec4_t left, vec4_t right);
float
ll_vec4_dot(vec4_t left, vec4_t right);
vec4_t
ll_vec4_normalise(vec4_t vec);

ivec2_t
ll_ivec2_create(int x, int y);
float
ll_ivec2_length(ivec2_t ivec);
ivec2_t
ll_ivec2_add(ivec2_t left, ivec2_t right);
ivec2_t
ll_ivec2_sub(ivec2_t left, ivec2_t right);
ivec2_t
ll_ivec2_mul(ivec2_t left, ivec2_t right);
ivec2_t
ll_ivec2_div(ivec2_t left, ivec2_t right);
float
ll_ivec2_dot(ivec2_t left, ivec2_t right);
float
ll_ivec2_cross(ivec2_t left, ivec2_t right);
ivec2_t
ll_ivec2_normalise(ivec2_t ivec);

ivec3_t
ll_ivec3_create(int x, int y, int z);
float
ll_ivec3_length(ivec3_t ivec);
ivec3_t
ll_ivec3_add(ivec3_t left, ivec3_t right);
ivec3_t
ll_ivec3_sub(ivec3_t left, ivec3_t right);
ivec3_t
ll_ivec3_mul(ivec3_t left, ivec3_t right);
ivec3_t
ll_ivec3_div(ivec3_t left, ivec3_t right);
float
ll_ivec3_dot(ivec3_t left, ivec3_t right);
ivec3_t
ll_ivec3_cross(ivec3_t left, ivec3_t right);
ivec3_t
ll_ivec3_normalise(ivec3_t ivec);

ivec4_t
ll_ivec4_create(int x, int y, int z, int w);
float
ll_ivec4_length(ivec4_t ivec);
ivec4_t
ll_ivec4_add(ivec4_t left, ivec4_t right);
ivec4_t
ll_ivec4_sub(ivec4_t left, ivec4_t right);
ivec4_t
ll_ivec4_mul(ivec4_t left, ivec4_t right);
ivec4_t
ll_ivec4_div(ivec4_t left, ivec4_t right);
float
ll_ivec4_dot(ivec4_t left, ivec4_t right);
ivec4_t
ll_ivec4_normalise(ivec4_t ivec);

void
ll_mat4_multiply(mat4_t *left, mat4_t *right);
void
ll_mat4_copy(mat4_t *to, mat4_t *from);
void
ll_mat4_identity(mat4_t *mat);
void
ll_mat4_translate3f(mat4_t *mat, float dx, float dy, float dz);
void
ll_mat4_translate3fv(mat4_t *mat, vec3_t vec);
void
ll_mat4_scale3f(mat4_t *mat, float w, float h, float d);
void
ll_mat4_scale3fv(mat4_t *mat, vec3_t vec);
void
ll_mat4_rotate3f(mat4_t *mat, float x, float y, float z, float angle);
void
ll_mat4_rotate3fv(mat4_t *mat, vec3_t vec, float angle);
void
ll_mat4_orthographic(mat4_t *mat, float top, float right,
                     float bottom, float left, float near, float far);
void
ll_mat4_perspective(mat4_t *mat, float fovy, float aspect,
                    float near, float far);
void
ll_mat4_frustum(mat4_t *mat, float left, float right,
                float bottom, float top, float near, float far);

void
ll_matrix_mode(matrix_type_t type);
void
ll_matrix_multiply(mat4_t *right);
void
ll_matrix_identity(void);
void
ll_matrix_translate3f(float dx, float dy, float dz);
void
ll_matrix_translate3fv(vec3_t vec);
void
ll_matrix_scale3f(float w, float h, float d);
void
ll_matrix_scale3fv(vec3_t vec);
void
ll_matrix_rotate3f(float x, float y, float z, float angle);
void
ll_matrix_rotate3fv(vec3_t vec, float angle);
void
ll_matrix_orthographic(float top, float right,
                       float bottom, float left, float near, float far);
void
ll_matrix_perspective(float fovy, float aspect,
                      float near, float far);
void
ll_matrix_frustum(float left, float right,
                  float bottom, float top, float near, float far);
mat4_t
ll_matrix_get_copy(void);

#endif /* FTGL_LINEAR_H_ */
