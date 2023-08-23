/*
  Author: wwotz
  Add #LINEARLIB_IMPLEMENTATION to the start of the implementation
  file in order to add the implementation code to your project. 
*/

#ifndef LINEARLIB_H_
#define LINEARLIB_H_

//requires -lm flag to link the math library
#include <math.h>

#ifndef LINEARLIBDEF
#ifdef LINEARLIBSTATIC
#define LINEARLIBDEF static
#else /* !defined(LINEARLIBSTATIC) */
#define LINEARLIBDEF extern
#endif /* LINEARLIBSTATIC */
#endif /* LINEARLIBDEF */

/* &optional */
#define LL_USE_MATRIX

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

LINEARLIBDEF vec2_t
ll_vec2_create(float x, float y);
LINEARLIBDEF float
ll_vec2_length(vec2_t vec);
LINEARLIBDEF vec2_t
ll_vec2_add(vec2_t left, vec2_t right);
LINEARLIBDEF vec2_t
ll_vec2_sub(vec2_t left, vec2_t right);
LINEARLIBDEF vec2_t
ll_vec2_mul(vec2_t left, vec2_t right);
LINEARLIBDEF vec2_t
ll_vec2_div(vec2_t left, vec2_t right);
LINEARLIBDEF float
ll_vec2_dot(vec2_t left, vec2_t right);
LINEARLIBDEF float
ll_vec2_cross(vec2_t left, vec2_t right);
LINEARLIBDEF vec2_t
ll_vec2_normalise(vec2_t vec);

LINEARLIBDEF vec3_t
ll_vec3_create(float x, float y, float z);
LINEARLIBDEF float
ll_vec3_length(vec3_t vec);
LINEARLIBDEF vec3_t
ll_vec3_add(vec3_t left, vec3_t right);
LINEARLIBDEF vec3_t
ll_vec3_sub(vec3_t left, vec3_t right);
LINEARLIBDEF vec3_t
ll_vec3_mul(vec3_t left, vec3_t right);
LINEARLIBDEF vec3_t
ll_vec3_div(vec3_t left, vec3_t right);
LINEARLIBDEF float
ll_vec3_dot(vec3_t left, vec3_t right);
LINEARLIBDEF vec3_t
ll_vec3_cross(vec3_t left, vec3_t right);
LINEARLIBDEF vec3_t
ll_vec3_normalise(vec3_t vec);

LINEARLIBDEF vec4_t
ll_vec4_create(float x, float y, float z, float w);
LINEARLIBDEF float
ll_vec4_length(vec4_t vec);
LINEARLIBDEF vec4_t
ll_vec4_add(vec4_t left, vec4_t right);
LINEARLIBDEF vec4_t
ll_vec4_sub(vec4_t left, vec4_t right);
LINEARLIBDEF vec4_t
ll_vec4_mul(vec4_t left, vec4_t right);
LINEARLIBDEF vec4_t
ll_vec4_div(vec4_t left, vec4_t right);
LINEARLIBDEF float
ll_vec4_dot(vec4_t left, vec4_t right);
LINEARLIBDEF vec4_t
ll_vec4_normalise(vec4_t vec);

LINEARLIBDEF ivec2_t
ll_ivec2_create(int x, int y);
LINEARLIBDEF float
ll_ivec2_length(ivec2_t ivec);
LINEARLIBDEF ivec2_t
ll_ivec2_add(ivec2_t left, ivec2_t right);
LINEARLIBDEF ivec2_t
ll_ivec2_sub(ivec2_t left, ivec2_t right);
LINEARLIBDEF ivec2_t
ll_ivec2_mul(ivec2_t left, ivec2_t right);
LINEARLIBDEF ivec2_t
ll_ivec2_div(ivec2_t left, ivec2_t right);
LINEARLIBDEF float
ll_ivec2_dot(ivec2_t left, ivec2_t right);
LINEARLIBDEF float
ll_ivec2_cross(ivec2_t left, ivec2_t right);
LINEARLIBDEF ivec2_t
ll_ivec2_normalise(ivec2_t ivec);

LINEARLIBDEF ivec3_t
ll_ivec3_create(int x, int y, int z);
LINEARLIBDEF float
ll_ivec3_length(ivec3_t ivec);
LINEARLIBDEF ivec3_t
ll_ivec3_add(ivec3_t left, ivec3_t right);
LINEARLIBDEF ivec3_t
ll_ivec3_sub(ivec3_t left, ivec3_t right);
LINEARLIBDEF ivec3_t
ll_ivec3_mul(ivec3_t left, ivec3_t right);
LINEARLIBDEF ivec3_t
ll_ivec3_div(ivec3_t left, ivec3_t right);
LINEARLIBDEF float
ll_ivec3_dot(ivec3_t left, ivec3_t right);
LINEARLIBDEF ivec3_t
ll_ivec3_cross(ivec3_t left, ivec3_t right);
LINEARLIBDEF ivec3_t
ll_ivec3_normalise(ivec3_t ivec);

LINEARLIBDEF ivec4_t
ll_ivec4_create(int x, int y, int z, int w);
LINEARLIBDEF float
ll_ivec4_length(ivec4_t ivec);
LINEARLIBDEF ivec4_t
ll_ivec4_add(ivec4_t left, ivec4_t right);
LINEARLIBDEF ivec4_t
ll_ivec4_sub(ivec4_t left, ivec4_t right);
LINEARLIBDEF ivec4_t
ll_ivec4_mul(ivec4_t left, ivec4_t right);
LINEARLIBDEF ivec4_t
ll_ivec4_div(ivec4_t left, ivec4_t right);
LINEARLIBDEF float
ll_ivec4_dot(ivec4_t left, ivec4_t right);
LINEARLIBDEF ivec4_t
ll_ivec4_normalise(ivec4_t ivec);

LINEARLIBDEF void
ll_mat4_multiply(mat4_t *left, mat4_t *right);
LINEARLIBDEF void
ll_mat4_copy(mat4_t *to, mat4_t *from);
LINEARLIBDEF void
ll_mat4_identity(mat4_t *mat);
LINEARLIBDEF void
ll_mat4_translate3f(mat4_t *mat, float dx, float dy, float dz);
LINEARLIBDEF void
ll_mat4_translate3fv(mat4_t *mat, vec3_t vec);
LINEARLIBDEF void
ll_mat4_scale3f(mat4_t *mat, float w, float h, float d);
LINEARLIBDEF void
ll_mat4_scale3fv(mat4_t *mat, vec3_t vec);
LINEARLIBDEF void
ll_mat4_rotate3f(mat4_t *mat, float x, float y, float z, float angle);
LINEARLIBDEF void
ll_mat4_rotate3fv(mat4_t *mat, vec3_t vec, float angle);
LINEARLIBDEF void
ll_mat4_orthographic(mat4_t *mat, float top, float right,
                     float bottom, float left, float near, float far);
LINEARLIBDEF void
ll_mat4_perspective(mat4_t *mat, float fovy, float aspect,
                    float near, float far);
LINEARLIBDEF void
ll_mat4_frustum(mat4_t *mat, float left, float right,
                float bottom, float top, float near, float far);

#ifdef LL_USE_MATRIX
LINEARLIBDEF void
ll_matrix_mode(matrix_type_t type);
LINEARLIBDEF void
ll_matrix_multiply(mat4_t *right);
LINEARLIBDEF void
ll_matrix_identity(void);
LINEARLIBDEF void
ll_matrix_translate3f(float dx, float dy, float dz);
LINEARLIBDEF void
ll_matrix_translate3fv(vec3_t vec);
LINEARLIBDEF void
ll_matrix_scale3f(float w, float h, float d);
LINEARLIBDEF void
ll_matrix_scale3fv(vec3_t vec);
LINEARLIBDEF void
ll_matrix_rotate3f(float x, float y, float z, float angle);
LINEARLIBDEF void
ll_matrix_rotate3fv(vec3_t vec, float angle);
LINEARLIBDEF void
ll_matrix_orthographic(float top, float right,
                       float bottom, float left, float near, float far);
LINEARLIBDEF void
ll_matrix_perspective(float fovy, float aspect,
                      float near, float far);
LINEARLIBDEF void
ll_matrix_frustum(float left, float right,
                  float bottom, float top, float near, float far);

LINEARLIBDEF mat4_t
ll_matrix_get_copy(void);

#endif /* LL_USE_MATRIX */

#ifdef LINEARLIB_IMPLEMENTATION

/* returns a 2-dimensional vector with (x, y) as it's components */
vec2_t
ll_vec2_create(float x, float y)
{
        return (vec2_t) {x, y};
}

/* returns the length of @vec  */
float
ll_vec2_length(vec2_t vec)
{
        return sqrtf((vec.x * vec.x) + (vec.y * vec.y));
}

/* returns a new vector containing the sum of left and right,
   component-wise sum */
vec2_t
ll_vec2_add(vec2_t left, vec2_t right)
{
        return ll_vec2_create(left.x + right.x, left.y + right.y);
}

/* returns a new vector containing the subtraction of right from left,
   component-wise difference  */
vec2_t
ll_vec2_sub(vec2_t left, vec2_t right)
{
        return ll_vec2_create(left.x - right.x, left.y - right.y);
}

/* returns a new vector containing the product of left and right,
   component-wise multiplication */
vec2_t
ll_vec2_mul(vec2_t left, vec2_t right)
{
        return ll_vec2_create(left.x * right.x, left.y * right.y);
}

/* returns a new vector containing the division of left by right,
   component-wise division */
vec2_t
ll_vec2_div(vec2_t left, vec2_t right)
{
        return ll_vec2_create(left.x / right.x, left.y / right.y);
}

/* returns the dot-product of @left and @right */
float
ll_vec2_dot(vec2_t left, vec2_t right)
{
        return left.x * right.x + left.y * right.y;
}

/* returns the cross-product of @left and @right */
float
ll_vec2_cross(vec2_t left, vec2_t right)
{
        return left.x * right.y - left.y * right.x;
}

/* returns a normalised vector of @vec */
vec2_t
ll_vec2_normalise(vec2_t vec)
{
        float length = ll_vec2_length(vec);
        return (vec2_t) {vec.x / length, vec.y / length};
}

/* returns a 3-dimensional vector with (x, y, z) as it's components */
vec3_t
ll_vec3_create(float x, float y, float z)
{
        return (vec3_t) {x, y, z};
}

/* returns the length of @vec  */
float
ll_vec3_length(vec3_t vec)
{
        return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

/* returns a new vector containing the sum of left and right,
   component-wise sum */
vec3_t
ll_vec3_add(vec3_t left, vec3_t right)
{
        return ll_vec3_create(left.x + right.x, left.y + right.y,
                              left.z + right.z);
}

/* returns a new vector containing the subtraction of right from left,
   component-wise difference  */
vec3_t
ll_vec3_sub(vec3_t left, vec3_t right)
{
        return ll_vec3_create(left.x - right.x, left.y - right.y,
                              left.z - right.z);
}

/* returns a new vector containing the product of left and right,
   component-wise multiplication */
vec3_t
ll_vec3_mul(vec3_t left, vec3_t right)
{
        return ll_vec3_create(left.x * right.x, left.y * right.y,
                              left.z * right.z);
}

/* returns a new vector containing the division of left by right,
   component-wise division */
vec3_t
ll_vec3_div(vec3_t left, vec3_t right)
{
        return ll_vec3_create(left.x / right.x, left.y / right.y,
                              left.z / right.z);
}

/* returns the dot-product of @left and @right */
float
ll_vec3_dot(vec3_t left, vec3_t right)
{
        return left.x * right.x + left.y * right.y + left.z * right.z;
}

/* returns the cross-product of @left and @right */
vec3_t
ll_vec3_cross(vec3_t left, vec3_t right)
{
        return ll_vec3_create(left.y*right.z - left.z*right.y,
                              left.z*right.x - left.x*right.z,
                              left.x*right.y - left.y*right.x);
}

/* returns a normalised vector of @vec */
vec3_t
ll_vec3_normalise(vec3_t vec)
{
        float length = ll_vec3_length(vec);
        return (vec3_t) {vec.x / length, vec.y / length, vec.z / length};
}

/* returns a 4-dimensional vector with (x, y, z, w) as it's components */
vec4_t
ll_vec4_create(float x, float y, float z, float w)
{
        return (vec4_t) {x, y, z, w};
}

/* returns the length of @vec  */
float
ll_vec4_length(vec4_t vec)
{
        return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}

/* returns a new vector containing the sum of left and right,
   component-wise sum */
vec4_t
ll_vec4_add(vec4_t left, vec4_t right)
{
        return ll_vec4_create(left.x + right.x, left.y + right.y,
                              left.z + right.z, left.w + right.w);
}

/* returns a new vector containing the subtraction of right from left,
   component-wise difference  */
vec4_t
ll_vec4_sub(vec4_t left, vec4_t right)
{
        return ll_vec4_create(left.x - right.x, left.y - right.y,
                              left.z - right.z, left.w - right.w);
}

/* returns a new vector containing the product of left and right,
   component-wise multiplication */
vec4_t
ll_vec4_mul(vec4_t left, vec4_t right)
{
        return ll_vec4_create(left.x * right.x, left.y * right.y,
                              left.z * right.z, left.w * right.w);
}

/* returns a new vector containing the division of left by right,
   component-wise division */
vec4_t
ll_vec4_div(vec4_t left, vec4_t right)
{
        return ll_vec4_create(left.x / right.x, left.y / right.y,
                              left.z / right.z, left.w / right.w);
}

/* return the dot-product of @left and @right */
float
ll_vec4_dot(vec4_t left, vec4_t right)
{
        return left.x * right.x + left.y * right.y
                + left.z * right.z + left.w * right.w;
}

/* returns a normalised vector of @vec */
vec4_t
ll_vec4_normalise(vec4_t vec)
{
        float length = ll_vec4_length(vec);
        return (vec4_t) {vec.x / length, vec.y / length,
                         vec.z / length, vec.w / length};
}

ivec2_t
ll_ivec2_create(int x, int y)
{
        return (ivec2_t) { x, y };
}

float
ll_ivec2_length(ivec2_t ivec)
{
        return sqrtf(ivec.x * ivec.x + ivec.y * ivec.y);
}

ivec2_t
ll_ivec2_add(ivec2_t left, ivec2_t right)
{
        return ll_ivec2_create(left.x + right.x, left.y + right.y);
}

ivec2_t
ll_ivec2_sub(ivec2_t left, ivec2_t right)
{
        return ll_ivec2_create(left.x - right.x, left.y - right.y);
}

ivec2_t
ll_ivec2_mul(ivec2_t left, ivec2_t right)
{
        return ll_ivec2_create(left.x * right.x, left.y * right.y);
}

ivec2_t
ll_ivec2_div(ivec2_t left, ivec2_t right)
{
        return ll_ivec2_create(left.x / right.x, left.y / right.y);
}

float
ll_ivec2_dot(ivec2_t left, ivec2_t right)
{
        return left.x * right.y + left.y * right.y;
}

float
ll_ivec2_cross(ivec2_t left, ivec2_t right)
{
        return left.x * right.y - left.y * right.x;
}

ivec2_t
ll_ivec2_normalise(ivec2_t ivec)
{
        float length = ll_ivec2_length(ivec);
        return ll_ivec2_create( ivec.x / length, ivec.y / length);
}


ivec3_t
ll_ivec3_create(int x, int y, int z)
{
        return (ivec3_t) { x, y, z };
}

float
ll_ivec3_length(ivec3_t ivec)
{
        return sqrtf(ivec.x * ivec.x + ivec.y * ivec.y
                     + ivec.z * ivec.z);
}

ivec3_t
ll_ivec3_add(ivec3_t left, ivec3_t right)
{
        return ll_ivec3_create(left.x + right.x, left.y + right.y,
                               left.z + right.z);
}

ivec3_t
ll_ivec3_sub(ivec3_t left, ivec3_t right)
{
        return ll_ivec3_create(left.x - right.x, left.y - right.y,
                               left.z - right.z);
}

ivec3_t
ll_ivec3_mul(ivec3_t left, ivec3_t right)
{
        return ll_ivec3_create(left.x * right.x, left.y * right.y,
                               left.z * right.z);
}

ivec3_t
ll_ivec3_div(ivec3_t left, ivec3_t right)
{
        return ll_ivec3_create(left.x / right.x, left.y / right.y,
                               left.z / right.z);
}

float
ll_ivec3_dot(ivec3_t left, ivec3_t right)
{
        return left.x * right.x + left.y * right.y
                + left.z * right.z;
}

ivec3_t
ll_ivec3_cross(ivec3_t left, ivec3_t right)
{
        return ll_ivec3_create(left.y*right.z - left.z*right.y,
                              left.z*right.x - left.x*right.z,
                              left.x*right.y - left.y*right.x);
}

ivec3_t
ll_ivec3_normalise(ivec3_t ivec)
{
        float length = ll_ivec3_length(ivec);
        return ll_ivec3_create(ivec.x / length, ivec.y / length,
                               ivec.z / length);
}

ivec4_t
ll_ivec4_create(int x, int y, int z, int w)
{
        return (ivec4_t) { x, y, z, w };
}

float
ll_ivec4_length(ivec4_t ivec)
{
        return sqrtf(ivec.x * ivec.x + ivec.y * ivec.y +
                     ivec.z * ivec.z + ivec.w * ivec.w);
}

ivec4_t
ll_ivec4_add(ivec4_t left, ivec4_t right)
{
        return ll_ivec4_create(left.x + right.x, left.y + right.y,
                               left.z + right.z, left.w + right.w);
}

ivec4_t
ll_ivec4_sub(ivec4_t left, ivec4_t right)
{
        return ll_ivec4_create(left.x - right.x, left.y - right.y,
                               left.z - right.z, left.w - right.w);
}

ivec4_t
ll_ivec4_mul(ivec4_t left, ivec4_t right)
{
        return ll_ivec4_create(left.x * right.x, left.y * right.y,
                               left.z * right.z, left.w * right.w);
}

ivec4_t
ll_ivec4_div(ivec4_t left, ivec4_t right)
{
        return ll_ivec4_create(left.x / right.x, left.y / right.y,
                               left.z / right.z, left.w / right.w);
}

float
ll_ivec4_dot(ivec4_t left, ivec4_t right)
{
        return left.x * right.x + left.y * right.y +
                left.z * right.z + left.w * right.w;
}

ivec4_t
ll_ivec4_normalise(ivec4_t ivec)
{
        float length = ll_ivec4_length(ivec);
        return ll_ivec4_create(ivec.x / length, ivec.y / length,
                               ivec.z / length, ivec.w / length);
}

/* performs matrix multiplcation on the matrices @l and @r
   matrix multiplication is not commutative, so order matters
   @left corresponds to the matrix on the left.
   @right corresponds to the matrix on the right. */
void
ll_mat4_multiply(mat4_t *left, mat4_t *right)
{
        mat4_t final;
        if (!left || !right) return;
        for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                        float sum = 0.0;
                        for (int k = 0; k < 4; k++)
                                sum += left->data[i * 4 + k] * right->data[k * 4 + j];
                        final.data[i * 4 + j] = sum;
                }
        }
        ll_mat4_copy(left, &final);
}

/* copy the matrix contents of @from into @to */
void
ll_mat4_copy(mat4_t *to,  mat4_t *from)
{
        if (!to || !from) return;
        for (int i = 0; i < 16; i++)
                to->data[i] = from->data[i];
}

/* stores the identity matrix into @m */
void
ll_mat4_identity(mat4_t *mat)
{
        if (!mat) return;
        for (int i = 0; i < 16; i++)
                mat->data[i] = 0;
        mat->m00 = 1.0;
        mat->m11 = 1.0;
        mat->m22 = 1.0;
        mat->m33 = 1.0;
}

/* stores a translation matrix inside of @mat with @dx, @dy and @dz */
void
ll_mat4_translate3f(mat4_t *mat, float dx, float dy, float dz)
{
        if (!mat) return;
        ll_mat4_identity(mat);
        mat->m30 = dx;
        mat->m31 = dy;
        mat->m32 = dz;
}

/* same as above, but instead allows to supply a 3-dimensional
   vector as the 2nd argument, containing (dx, dy, dz) */
void
ll_mat4_translate3fv(mat4_t *mat, vec3_t vec)
{
        ll_mat4_translate3f(mat, vec.x, vec.y, vec.z);
}

/* multiplies @m by a scaling matrix with components (w, h, d)
   storing the result back into @m */
void
ll_mat4_scale3f(mat4_t *mat, float w, float h, float d)
{
        if (!mat) return;
        ll_mat4_identity(mat);
        mat->m00 = w;
        mat->m11 = h;
        mat->m22 = d;
}

/* same as above, but instead allows to supply a 3-dimensional
   vector as the 2nd arugment, containing (w, h, d) */
void
ll_mat4_scale3fv(mat4_t *mat, vec3_t vec)
{
        ll_mat4_scale3f(mat, vec.x, vec.y, vec.z);
}

/* found whilst reading over rougier/freetype-gl implementation.
   that can be found here: https://github.com/rougier/freetype-gl */
void
ll_mat4_rotate3f(mat4_t *mat, float x, float y, float z, float theta)
{
        float c, s, norm;
        if (!mat) return;

        c =    (float) cos(M_PI * theta/180.0);
        s =    (float) sin(M_PI * theta/180.0);
        norm = (float) sqrt(x*x+y*y+z*z);
        
        x /= norm;
        y /= norm;
        z /= norm;

        ll_mat4_identity(mat);
        
        mat->m00 = x*x*(1-c)+c;
        mat->m10 = y*x*(1-c)-z*s;
        mat->m20 = z*x*(1-c)+y*s;
        mat->m01 = x*y*(1-c)+z*s;
        mat->m11 = y*y*(1-c)+c;
        mat->m21 = z*y*(1-c)-x*s;
        mat->m02 = x*z*(1-c)-y*s;
        mat->m12 = y*z*(1-c)+x*s;
        mat->m22 = z*z*(1-c)+c;
}

/* same as above, but instead allows to supply a 3-dimensional
   vector as the 2nd argument, containing (x, y, z) */
void
ll_mat4_rotate3fv(mat4_t *mat, vec3_t vec, float theta)
{
        ll_mat4_rotate3f(mat, vec.x, vec.y, vec.z, theta);
}

/* stores the orthographic matrix into @m, details of how this works can be found online or
   found at https://github.com/wwotz/linearlib/README.md */
void
ll_mat4_orthographic(mat4_t *mat, float top, float right,
                     float bottom, float left, float near, float far)
{
        if (!mat || left == right || near == far || bottom == top) return;
        ll_mat4_identity(mat);
        mat->m00 = 2.0f/(right-left);
        mat->m30 = -(right+left)/(right-left);
        mat->m11 = 2.0f/(top-bottom);
        mat->m31 = -(top+bottom)/(top-bottom);
        mat->m22 = -2.0f/(far-near);
        mat->m32 = -(far+near)/(far-near);
}

/* stores the perspective matrix into @m, details of how this works can be found online or
   found at https://github.com/wwotz/linearlib/README.md */
void
ll_mat4_perspective(mat4_t *mat, float fovy, float aspect,
                    float near, float far)
{
        float w, h;
        if (!mat || near == far) return;
        
        h = (float) tan(fovy / 360.0 * M_PI) * near;
        w = h * aspect;
        ll_mat4_frustum(mat, -w, w, -h, h, near, far);
}

void
ll_mat4_frustum(mat4_t *mat, float top, float right,
                float bottom, float left, float near, float far)
{
        if (!mat || left == right || bottom == top || near == far) return;
        ll_mat4_identity(mat);
        mat->m00 = (2.0f*near)/(right-left);
        mat->m20 = (right+left)/(right-left);
        mat->m11 = (2.0f*near)/(top-bottom);
        mat->m21 = (top+bottom)/(top-bottom);
        mat->m22 = -(far+near)/(far-near);
        mat->m32 = -(2.0f*far*near)/(far-near);
        mat->m23 = -1.0f;
        mat->m33 = 0.0;
}

#ifdef LL_USE_MATRIX

static mat4_t ll_matrices[LL_MATRIX_COUNT];
static int ll_matrices_idx;

void
ll_matrix_mode(matrix_type_t type)
{
        if (type >= 0 && type < LL_MATRIX_COUNT)
                ll_matrices_idx = type;
}

void
ll_matrix_multiply(mat4_t *right)
{
        ll_mat4_multiply(ll_matrices+ll_matrices_idx, right);
}

void
ll_matrix_identity(void)
{
        ll_mat4_identity(ll_matrices+ll_matrices_idx);
}

void
ll_matrix_translate3f(float dx, float dy, float dz)
{
        mat4_t m;
        ll_mat4_translate3f(&m, dx, dy, dz);
        ll_matrix_multiply(&m);
}

void
ll_matrix_translate3fv(vec3_t vec)
{
        ll_matrix_translate3f(vec.x, vec.y, vec.z);
}

void
ll_matrix_scale3f(float w, float h, float d)
{
        mat4_t m;
        ll_mat4_scale3f(&m, w, h, d);
        ll_matrix_multiply(&m);
}

void
ll_matrix_scale3fv(vec3_t vec)
{
        ll_matrix_scale3f(vec.x, vec.y, vec.z);
}

void
ll_matrix_rotate3f(float x, float y, float z, float angle)
{
        mat4_t m;
        ll_mat4_rotate3f(&m, x, y, z, angle);
        ll_matrix_multiply(&m);
}

void
ll_matrix_rotate3fv(vec3_t vec, float angle)
{
        ll_matrix_rotate3f(vec.x, vec.y, vec.z, angle);
}

void
ll_matrix_orthographic(float top, float right,
                       float bottom, float left, float near, float far)
{
        ll_mat4_orthographic(ll_matrices+ll_matrices_idx, top,
                             right, bottom, left, near, far);
}

void
ll_matrix_perspective(float fovy, float aspect,
                      float near, float far)
{
        ll_mat4_perspective(ll_matrices+ll_matrices_idx,
                            fovy, aspect, near, far);
}

void
ll_matrix_frustum(float left, float right,
                  float bottom, float top, float near, float far)
{
        ll_mat4_frustum(ll_matrices+ll_matrices_idx, left, right,
                        bottom, top, near, far);
}

mat4_t
ll_matrix_get_copy(void)
{
        return ll_matrices[ll_matrices_idx];
}

#endif /* LL_USE_MATRIX */
#endif /* LINEARLIB_IMPLEMENTATION */
#endif /* LINEARLIB_H_ */
