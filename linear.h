/**
 * Author: wwotz
 * Add #LINEARLIB_IMPLEMENTATION to the start of the implementation
 * file in order to add the implementation code to your project.
 */

#ifndef LINEARLIB_H_
#define LINEARLIB_H_

//requires -lm flag to link the math library
#include <math.h>
#include <stddef.h>
#include <string.h>

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
        LL_MATRIX_MODEL,       /* Model matrix, used to transform objects to object space */
        LL_MATRIX_VIEW,        /* View matrix, used to transform objects to camera space */
        LL_MATRIX_PROJECTION,  /* Projection matrix, used to transform objects to viewport space */
	LL_MATRIX_CAMERA,      /* Camera matrix, used for implementing separate matrix for camera view */
        LL_MATRIX_COUNT
} matrix_type_t;

/*
 * @description A union which is essentially an array containg 2 floats.
 * The union allows for ease of access of attributes in meaningful ways,
 * rather than simply { v.data[0], v.data[1] }.
 */
typedef union vec2_t {
        float data[2];    /* The 2-components of the vector */
        struct {      
                float x;  /* The x component of the vector (data[0]) */
                float y;  /* The y component of the vector (data[1]) */
        };
        struct {
                float r;  /* The x component of the vector (data[0]) */
                float g;  /* The x component of the vector (data[1]) */
        };
        struct {
                float s;  /* The x component of the vector (data[0]) */ 
                float t;  /* The x component of the vector (data[1]) */
        };
} vec2_t;

/*
 * @description A union which is essentially an array containg 3 floats.
 * The union allows for ease of access of attributes in meaningful ways,
 * rather than simply { v.data[0], v.data[1], v.data[2] }.
 */
typedef union vec3_t {
        float data[3];    /* The 3-components of the vector */
        struct {
                float x;  /* The x component of the vector (data[0]) */
                float y;  /* The y component of the vector (data[1]) */
                float z;  /* The z component of the vector (data[2]) */
        };
        struct {
                float r;  /* The r component of the vector (data[0]) */
                float g;  /* The g component of the vector (data[1]) */
                float b;  /* The b component of the vector (data[2]) */
        };        
} vec3_t;

/*
 * @description A union which is essentially an array containing 4 floats.
 * The union allows for ease of access of attributes in meaningful ways,
 * rather than simply { v.data[0], v.data[1], v.data[2], v.data[3] }.
 */
typedef union vec4_t {
        float data[4];    /* The 4-components of the vector */
        struct {
                float x;  /* The x component of the vector (data[0]) */
                float y;  /* The y component of the vector (data[1]) */
                float z;  /* The z component of the vector (data[2]) */
                float w;  /* The w component of the vector (data[3]) */
        };
        struct {
                float r;  /* The r component of the vector (data[0]) */
                float g;  /* The g component of the vector (data[1]) */
                float b;  /* The b component of the vector (data[2]) */
                float a;  /* The a component of the vector (data[3]) */ 
        };
} vec4_t;

/*
 * @description A union which is essentially an array containing 2 integers.
 * The union allows for ease of access of attributes in meaningful ways,
 * rather than simply { v.data[0], v.data[1] }.
 */
typedef union ivec2_t {
        int data[2];    /* The 2-components of the vector */
        struct {
                int x;  /* The x component of the vector (data[0]) */
                int y;  /* The y component of the vector (data[1]) */
        };
        struct {
                int r;  /* The r component of the vector (data[0]) */
                int g;  /* The g component of the vector (data[1]) */
        };
        struct {
                int s;  /* The s component of the vector (data[0]) */
                int t;  /* The t component of the vector (data[1]) */
        };
} ivec2_t;

/*
 * @description A union which is essentially an array containing 3 integers.
 * The union allows for ease of access of attributes in meaningful ways,
 * rather than simply { v.data[0], v.data[1], v.data[2] }.
 */
typedef union ivec3_t {
        int data[3];    /* The 3-components of the vector */
        struct {
                int x;  /* The x component of the vector (data[0]) */
                int y;  /* The y component of the vector (data[1]) */
                int z;  /* The z component of the vector (data[2]) */
        };
        struct {
                int r;  /* The r component of the vector (data[0]) */
                int g;  /* The g component of the vector (data[1]) */ 
                int b;  /* The b component of the vector (data[2]) */
        };
} ivec3_t;

/*
 * @description A union which is essentially an array containing 4 integers.
 * The union allows for ease of access of attributes in meaningful ways,
 * rather than simply { v.data[0], v.data[1], v.data[2], v.data[3] }.
 */
typedef union ivec4_t {
        int data[4];    /* The 3-components of the vector */
        struct {
                int x;  /* The x component of the vector (data[0]) */
                int y;  /* The y component of the vector (data[1]) */
                int z;  /* The z component of the vector (data[2]) */
                int w;  /* The w component of the vector (data[3]) */
        };
        struct {
                int r;  /* The r component of the vector (data[0]) */
                int g;  /* The g component of the vector (data[1]) */
                int b;  /* The b component of the vector (data[2]) */
                int a;  /* The a component of the vector (data[3]) */
        };
} ivec4_t;

/*
 * @description A union which is essentially an array containing 4 floats.
 * The union allows for ease of access of attributes in meaningful ways.
 * This is a 2x2 matrix.
 */
typedef union mat2_t {
        float data[4];           /* the cells in the matrix, row-major order */
        struct {
                float m00, m01;  /* the top row of the matrix */
                float m10, m11;  /* the bottom row of the matrix */
        };
} mat2_t;

/*
 * @description A union which is essentially an array containing 9 floats.
 * The union allows for ease of access of attributes in meaningful ways.
 * This is a 3x3 matrix.
 */
typedef union mat3_t {
        float data[9];                /* the cells in the matrix, row-major order */
        struct {
                float m00, m01, m02;  /* the top row of the matrix */
                float m10, m11, m12;  /* the middle row of the matrix */
                float m20, m21, m22;  /* the bottom row of the matrix */
        };
} mat3_t;

/**
 * @description A union which is essentially an array containing 16 floats.
 * The union allows for ease of access of attributes in meaningful ways.
 * This is a 4x4 matrix.
 */
typedef union mat4_t {
        float data[16];                    /* the cells in the matrix, row-major order */
        struct { 
                float m00, m01, m02, m03;  /* the top row of the matrix */
                float m10, m11, m12, m13;  /* the second from top row of the matrix */
                float m20, m21, m22, m23;  /* the third from top row of the matrix */
                float m30, m31, m32, m33;  /* the bottom row of the matrix */
        };
} mat4_t;

/**
 * @description A struct which represents a mechanism for 3D rotation, called
 * a quaternion.
 */
typedef struct quaternion_t {
	float s;
	vec3_t v;
} quaternion_t;

LINEARLIBDEF vec2_t
ll_vec2_create2f(float x, float y);
LINEARLIBDEF vec2_t
ll_vec2_create2fv(vec2_t vec);
LINEARLIBDEF float
ll_vec2_length2fv(vec2_t vec);
LINEARLIBDEF float
ll_vec2_length2f(float x, float y);
LINEARLIBDEF float
ll_vec2_length_squared2fv(vec2_t vec);
LINEARLIBDEF float
ll_vec2_length_squared2f(float x, float y);
LINEARLIBDEF vec2_t
ll_vec2_add2fv(vec2_t left, vec2_t right);
LINEARLIBDEF vec2_t
ll_vec2_add2f(vec2_t left, float x, float y);
LINEARLIBDEF vec2_t
ll_vec2_add1f(vec2_t left, float value);
LINEARLIBDEF vec2_t
ll_vec2_sub2fv(vec2_t left, vec2_t right);
LINEARLIBDEF vec2_t
ll_vec2_sub2f(vec2_t left, float x, float y);
LINEARLIBDEF vec2_t
ll_vec2_sub1f(vec2_t left, float value);
LINEARLIBDEF vec2_t
ll_vec2_mul2fv(vec2_t left, vec2_t right);
LINEARLIBDEF vec2_t
ll_vec2_mul2f(vec2_t left, float x, float y);
LINEARLIBDEF vec2_t
ll_vec2_mul1f(vec2_t left, float value);
LINEARLIBDEF vec2_t
ll_vec2_div2fv(vec2_t left, vec2_t right);
LINEARLIBDEF vec2_t
ll_vec2_div2f(vec2_t left, float x, float y);
LINEARLIBDEF vec2_t
ll_vec2_div1f(vec2_t left, float value);
LINEARLIBDEF float
ll_vec2_dot2fv(vec2_t left, vec2_t right);
LINEARLIBDEF float
ll_vec2_dot2f(vec2_t left, float x, float y);
LINEARLIBDEF float
ll_vec2_cross2fv(vec2_t left, vec2_t right);
LINEARLIBDEF float
ll_vec2_cross2f(vec2_t left, float x, float y);
LINEARLIBDEF vec2_t
ll_vec2_normalise2fv(vec2_t vec);
LINEARLIBDEF vec2_t
ll_vec2_normalise2f(float x, float y);
LINEARLIBDEF vec2_t
ll_vec2_apply2fv(vec2_t vec, float (*f)(float));
LINEARLIBDEF vec2_t
ll_vec2_apply2f(float x, float y, float (*f)(float));
LINEARLIBDEF vec2_t
ll_vec2_origin(void);

LINEARLIBDEF vec3_t
ll_vec3_create3f(float x, float y, float z);
LINEARLIBDEF vec3_t
ll_vec3_create3fv(vec3_t ivec);
LINEARLIBDEF float
ll_vec3_length3fv(vec3_t ivec);
LINEARLIBDEF float
ll_vec3_length3f(float x, float y, float z);
LINEARLIBDEF float
ll_vec3_length_squared3fv(vec3_t ivec);
LINEARLIBDEF float
ll_vec3_length_squared3f(float x, float y, float z);
LINEARLIBDEF vec3_t
ll_vec3_add3fv(vec3_t left, vec3_t right);
LINEARLIBDEF vec3_t
ll_vec3_add3f(vec3_t left, float x, float y, float z);
LINEARLIBDEF vec3_t
ll_vec3_add1f(vec3_t left, float value);
LINEARLIBDEF vec3_t
ll_vec3_sub3fv(vec3_t left, vec3_t right);
LINEARLIBDEF vec3_t
ll_vec3_sub3f(vec3_t left, float x, float y, float z);
LINEARLIBDEF vec3_t
ll_vec3_sub1f(vec3_t left, float value);
LINEARLIBDEF vec3_t
ll_vec3_mul3fv(vec3_t left, vec3_t right);
LINEARLIBDEF vec3_t
ll_vec3_mul3f(vec3_t left, float x, float y, float z);
LINEARLIBDEF vec3_t
ll_vec3_mul1f(vec3_t left, float value);
LINEARLIBDEF vec3_t
ll_vec3_div3fv(vec3_t left, vec3_t right);
LINEARLIBDEF vec3_t
ll_vec3_div3f(vec3_t left, float x, float y, float z);
LINEARLIBDEF vec3_t
ll_vec3_div1f(vec3_t left, float value);
LINEARLIBDEF float
ll_vec3_dot3fv(vec3_t left, vec3_t right);
LINEARLIBDEF float
ll_vec3_dot3f(vec3_t left, float x, float y, float z);
LINEARLIBDEF vec3_t
ll_vec3_cross3fv(vec3_t left, vec3_t right);
LINEARLIBDEF vec3_t
ll_vec3_cross3f(vec3_t left, float x, float y, float z);
LINEARLIBDEF vec3_t
ll_vec3_normalise3fv(vec3_t ivec);
LINEARLIBDEF vec3_t
ll_vec3_normalise3f(float x, float y, float z);

LINEARLIBDEF vec4_t
ll_vec4_create4f(float x, float y, float z, float w);
LINEARLIBDEF vec4_t
ll_vec4_create4fv(vec4_t ivec);
LINEARLIBDEF float
ll_vec4_length4fv(vec4_t ivec);
LINEARLIBDEF float
ll_vec4_length4f(float x, float y, float z, float w);
LINEARLIBDEF float
ll_vec4_length_squared4fv(vec4_t ivec);
LINEARLIBDEF float
ll_vec4_length_squared4f(float x, float y, float z, float w);
LINEARLIBDEF vec4_t
ll_vec4_add4fv(vec4_t left, vec4_t right);
LINEARLIBDEF vec4_t
ll_vec4_add4f(vec4_t left, float x, float y, float z, float w);
LINEARLIBDEF vec4_t
ll_vec4_add1f(vec4_t left, float value);
LINEARLIBDEF vec4_t
ll_vec4_sub4fv(vec4_t left, vec4_t right);
LINEARLIBDEF vec4_t
ll_vec4_sub4f(vec4_t left, float x, float y, float z, float w);
LINEARLIBDEF vec4_t
ll_vec4_sub1f(vec4_t left, float value);
LINEARLIBDEF vec4_t
ll_vec4_mul4fv(vec4_t left, vec4_t right);
LINEARLIBDEF vec4_t
ll_vec4_mul4f(vec4_t left, float x, float y, float z, float w);
LINEARLIBDEF vec4_t
ll_vec4_mul1f(vec4_t left, float value);
LINEARLIBDEF vec4_t
ll_vec4_div4fv(vec4_t left, vec4_t right);
LINEARLIBDEF vec4_t
ll_vec4_div4f(vec4_t left, float x, float y, float z, float w);
LINEARLIBDEF vec4_t
ll_vec4_div1f(vec4_t left, float value);
LINEARLIBDEF float
ll_vec4_dot4fv(vec4_t left, vec4_t right);
LINEARLIBDEF float
ll_vec4_dot4f(vec4_t left, float x, float y, float z, float w);
LINEARLIBDEF vec4_t
ll_vec4_normalise4fv(vec4_t ivec);
LINEARLIBDEF vec4_t
ll_vec4_normalise4f(float x, float y, float z, float w);

LINEARLIBDEF ivec2_t
ll_ivec2_create2i(int x, int y);
LINEARLIBDEF ivec2_t
ll_ivec2_create2iv(ivec2_t ivec);
LINEARLIBDEF float
ll_ivec2_length2iv(ivec2_t ivec);
LINEARLIBDEF float
ll_ivec2_length2i(int x, int y);
LINEARLIBDEF ivec2_t
ll_ivec2_add2iv(ivec2_t left, ivec2_t right);
LINEARLIBDEF ivec2_t
ll_ivec2_add2i(ivec2_t left, int x, int y);
LINEARLIBDEF ivec2_t
ll_ivec2_add1i(ivec2_t left, int value);
LINEARLIBDEF ivec2_t
ll_ivec2_sub2iv(ivec2_t left, ivec2_t right);
LINEARLIBDEF ivec2_t
ll_ivec2_sub2i(ivec2_t left, int x, int y);
LINEARLIBDEF ivec2_t
ll_ivec2_sub1i(ivec2_t left, int value);
LINEARLIBDEF ivec2_t
ll_ivec2_mul2iv(ivec2_t left, ivec2_t right);
LINEARLIBDEF ivec2_t
ll_ivec2_mul2i(ivec2_t left, int x, int y);
LINEARLIBDEF ivec2_t
ll_ivec2_mul1i(ivec2_t left, int value);
LINEARLIBDEF ivec2_t
ll_ivec2_div2iv(ivec2_t left, ivec2_t right);
LINEARLIBDEF ivec2_t
ll_ivec2_div2i(ivec2_t left, int x, int y);
LINEARLIBDEF ivec2_t
ll_ivec2_div1i(ivec2_t left, int value);
LINEARLIBDEF float
ll_ivec2_dot2iv(ivec2_t left, ivec2_t right);
LINEARLIBDEF float
ll_ivec2_dot2i(ivec2_t left, int x, int y);
LINEARLIBDEF float
ll_ivec2_cross2iv(ivec2_t left, ivec2_t right);
LINEARLIBDEF float
ll_ivec2_cross2i(ivec2_t left, int x, int y);
LINEARLIBDEF ivec2_t
ll_ivec2_normalise2iv(ivec2_t ivec);
LINEARLIBDEF ivec2_t
ll_ivec2_normalise2i(int x, int y);

LINEARLIBDEF ivec3_t
ll_ivec3_create3i(int x, int y, int z);
LINEARLIBDEF ivec3_t
ll_ivec3_create3iv(ivec3_t ivec);
LINEARLIBDEF float
ll_ivec3_length3iv(ivec3_t ivec);
LINEARLIBDEF float
ll_ivec3_length3i(int x, int y, int z);
LINEARLIBDEF float
ll_ivec3_length_squared3iv(ivec3_t ivec);
LINEARLIBDEF float
ll_ivec3_length_squared3i(int x, int y, int z);
LINEARLIBDEF ivec3_t
ll_ivec3_add3iv(ivec3_t left, ivec3_t right);
LINEARLIBDEF ivec3_t
ll_ivec3_add3i(ivec3_t left, int x, int y, int z);
LINEARLIBDEF ivec3_t
ll_ivec3_add1i(ivec3_t left, int value);
LINEARLIBDEF ivec3_t
ll_ivec3_sub3iv(ivec3_t left, ivec3_t right);
LINEARLIBDEF ivec3_t
ll_ivec3_sub3i(ivec3_t left, int x, int y, int z);
LINEARLIBDEF ivec3_t
ll_ivec3_sub1i(ivec3_t left, int value);
LINEARLIBDEF ivec3_t
ll_ivec3_mul3iv(ivec3_t left, ivec3_t right);
LINEARLIBDEF ivec3_t
ll_ivec3_mul3i(ivec3_t left, int x, int y, int z);
LINEARLIBDEF ivec3_t
ll_ivec3_mul1i(ivec3_t left, int value);
LINEARLIBDEF ivec3_t
ll_ivec3_div3iv(ivec3_t left, ivec3_t right);
LINEARLIBDEF ivec3_t
ll_ivec3_div3i(ivec3_t left, int x, int y, int z);
LINEARLIBDEF ivec3_t
ll_ivec3_div1i(ivec3_t left, int value);
LINEARLIBDEF float
ll_ivec3_dot3iv(ivec3_t left, ivec3_t right);
LINEARLIBDEF float
ll_ivec3_dot3i(ivec3_t left, int x, int y, int z);
LINEARLIBDEF ivec3_t
ll_ivec3_cross3iv(ivec3_t left, ivec3_t right);
LINEARLIBDEF ivec3_t
ll_ivec3_cross3i(ivec3_t left, int x, int y, int z);
LINEARLIBDEF ivec3_t
ll_ivec3_normalise3iv(ivec3_t ivec);
LINEARLIBDEF ivec3_t
ll_ivec3_normalise3i(int x, int y, int z);

LINEARLIBDEF ivec4_t
ll_ivec4_create4i(int x, int y, int z, int w);
LINEARLIBDEF ivec4_t
ll_ivec4_create4iv(ivec4_t ivec);
LINEARLIBDEF float
ll_ivec4_length4iv(ivec4_t ivec);
LINEARLIBDEF float
ll_ivec4_length4i(int x, int y, int z, int w);
LINEARLIBDEF float
ll_ivec4_length_squared4iv(ivec4_t ivec);
LINEARLIBDEF float
ll_ivec4_length_squared4i(int x, int y, int z, int w);
LINEARLIBDEF ivec4_t
ll_ivec4_add4iv(ivec4_t left, ivec4_t right);
LINEARLIBDEF ivec4_t
ll_ivec4_add4i(ivec4_t left, int x, int y, int z, int w);
LINEARLIBDEF ivec4_t
ll_ivec4_add1i(ivec4_t left, int value);
LINEARLIBDEF ivec4_t
ll_ivec4_sub4iv(ivec4_t left, ivec4_t right);
LINEARLIBDEF ivec4_t
ll_ivec4_sub4i(ivec4_t left, int x, int y, int z, int w);
LINEARLIBDEF ivec4_t
ll_ivec4_sub1i(ivec4_t left, int value);
LINEARLIBDEF ivec4_t
ll_ivec4_mul4iv(ivec4_t left, ivec4_t right);
LINEARLIBDEF ivec4_t
ll_ivec4_mul4i(ivec4_t left, int x, int y, int z, int w);
LINEARLIBDEF ivec4_t
ll_ivec4_mul1i(ivec4_t left, int value);
LINEARLIBDEF ivec4_t
ll_ivec4_div4iv(ivec4_t left, ivec4_t right);
LINEARLIBDEF ivec4_t
ll_ivec4_div4i(ivec4_t left, int x, int y, int z, int w);
LINEARLIBDEF ivec4_t
ll_ivec4_div1i(ivec4_t left, int value);
LINEARLIBDEF float
ll_ivec4_dot4iv(ivec4_t left, ivec4_t right);
LINEARLIBDEF float
ll_ivec4_dot4i(ivec4_t left, int x, int y, int z, int w);
LINEARLIBDEF ivec4_t
ll_ivec4_normalise4iv(ivec4_t ivec);
LINEARLIBDEF ivec4_t
ll_ivec4_normalise4i(int x, int y, int z, int w);

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
ll_mat4_translate3f2(mat4_t *mat, float dx, float dy, float dz);
LINEARLIBDEF void
ll_mat4_translate3fv2(mat4_t *mat, vec3_t vec);
LINEARLIBDEF void
ll_mat4_scale3f2(mat4_t *mat, float w, float h, float d);
LINEARLIBDEF void
ll_mat4_scale3fv2(mat4_t *mat, vec3_t vec);
LINEARLIBDEF void
ll_mat4_rotate3f2(mat4_t *mat, float x, float y, float z, float angle);
LINEARLIBDEF void
ll_mat4_rotate3fv2(mat4_t *mat, vec3_t vec, float angle);
LINEARLIBDEF void
ll_mat4_orthographic(mat4_t *mat, float top, float right,
                     float bottom, float left, float near, float far);
LINEARLIBDEF void
ll_mat4_perspective(mat4_t *mat, float fovy, float aspect,
                    float near, float far);
LINEARLIBDEF void
ll_mat4_frustum(mat4_t *mat, float left, float right,
                float bottom, float top, float near, float far);
LINEARLIBDEF void
ll_mat4_lookat(mat4_t *mat, vec3_t x, vec3_t y, vec3_t z,
	       vec3_t lookat);
LINEARLIBDEF void
ll_quaternion_to_mat4(quaternion_t q, mat4_t *mat);

LINEARLIBDEF quaternion_t
ll_quaternion_create3f(float s, float x, float y, float z);
LINEARLIBDEF quaternion_t
ll_quaternion_create3fv(float s, vec3_t v);
LINEARLIBDEF quaternion_t
ll_quaternion_angle_axis3f(float angle, float x, float y, float z);
LINEARLIBDEF quaternion_t
ll_quaternion_angle_axis3fv(float radians, vec3_t v);
LINEARLIBDEF quaternion_t 
ll_quaternion_add(quaternion_t a, quaternion_t b);
LINEARLIBDEF quaternion_t
ll_quaternion_sub(quaternion_t a, quaternion_t b);
LINEARLIBDEF quaternion_t
ll_quaternion_prod(quaternion_t a, quaternion_t b);
LINEARLIBDEF float
ll_quaternion_norm(quaternion_t a);
LINEARLIBDEF quaternion_t
ll_quaternion_normalise(quaternion_t a);
LINEARLIBDEF quaternion_t
ll_quaternion_conjugate(quaternion_t a);
LINEARLIBDEF quaternion_t
ll_quaternion_inverse(quaternion_t a);
LINEARLIBDEF vec3_t
ll_quaternion_rotate3fv(quaternion_t a, vec3_t v);
LINEARLIBDEF vec3_t
ll_quaternion_rotate3f(quaternion_t a, float x, float y, float z);

#ifdef LL_USE_MATRIX

#define LL_MATRIX_STACK_CAPACITY (16)

static size_t ll_matrix_stack_size;
static mat4_t ll_matrix_stack[LL_MATRIX_STACK_CAPACITY];

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

LINEARLIBDEF void
ll_matrix_lookat(vec3_t x, vec3_t y, vec3_t z, vec3_t lookat);

LINEARLIBDEF mat4_t
ll_matrix_get_copy(void);

LINEARLIBDEF int
ll_matrix_stack_pop(mat4_t *mat);

LINEARLIBDEF int
ll_matrix_stack_push(mat4_t *mat);

LINEARLIBDEF void
ll_quaternion_to_matrix(quaternion_t a);

#endif /* LL_USE_MATRIX */

#if defined(LINEARLIB_IMPLEMENTATION) || defined(FTGL_IMPLEMENTATION)

/**
 * @return A vec2_t structure.
 */
LINEARLIBDEF vec2_t
ll_vec2_create2f(float x, float y)
{
	return (vec2_t) {{ x, y }};
}

/**
 * @description Essentially creates a new copy of @vec.
 *
 * @return A vec2_t structure.
 */
LINEARLIBDEF vec2_t
ll_vec2_create2fv(vec2_t vec)
{
	return vec;
}

/**
 * @return The length of @vec.
 */
LINEARLIBDEF float
ll_vec2_length2fv(vec2_t vec)
{
	return sqrtf((vec.x * vec.x) + (vec.y * vec.y));
}

/**
 * @return The length of a vector, represented by { x, y }.
 */
LINEARLIBDEF float
ll_vec2_length2f(float x, float y)
{
	return sqrtf((x * x) + (y * y));
}

/**
 * @return The squared length of @vec.
 */
LINEARLIBDEF float
ll_vec2_length_squared2fv(vec2_t vec)
{
	float length = ll_vec2_length2fv(vec);
	return length*length;
}

/**
 * @return The squared length of vector, represented by { x, y }.
 */
LINEARLIBDEF float
ll_vec2_length_squared2f(float x, float y)
{
	float length = ll_vec2_length2f(x, y);
	return length*length;
}

/**
 * @return A new vector, that is the sum of @left and @right.
 */
LINEARLIBDEF vec2_t
ll_vec2_add2fv(vec2_t left, vec2_t right)
{
	return ll_vec2_create2f(left.x + right.x, left.y + right.y);
}

/**
 * @return A new vector, that is the sum of @left and { x, y }.
 */
LINEARLIBDEF vec2_t
ll_vec2_add2f(vec2_t left, float x, float y)
{
	return ll_vec2_create2f(left.x + x, left.y + y);
}

/**
 * @return A new vector, that is the sum of @left
 * and { value, value }.
 */
LINEARLIBDEF vec2_t
ll_vec2_add1f(vec2_t left, float value)
{
	return ll_vec2_create2f(left.x + value, left.y + value);
}

/**
 * @return A new vector, that is the sum of @left and -@right.
 */
LINEARLIBDEF vec2_t
ll_vec2_sub2fv(vec2_t left, vec2_t right)
{
	return ll_vec2_add2f(left, -right.x, -right.y);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -x, -y }.
 */
LINEARLIBDEF vec2_t
ll_vec2_sub2f(vec2_t left, float x, float y)
{
	return ll_vec2_add2f(left, -x, -y);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -value, -value }.
 */
LINEARLIBDEF vec2_t
ll_vec2_sub1f(vec2_t left, float value)
{
	return ll_vec2_add2f(left, -value, -value);
}

/**
 * @return A new vector that is the component-wise
 * product of @left and @right.
 */
LINEARLIBDEF vec2_t
ll_vec2_mul2fv(vec2_t left, vec2_t right)
{
	return ll_vec2_create2f(left.x * right.x,
				left.y * right.y);
}

/**
 * @return A new vector that is the component-wise
 * product of @left and { x, y }.
 */
LINEARLIBDEF vec2_t
ll_vec2_mul2f(vec2_t left, float x, float y)
{
	return ll_vec2_create2f(left.x * x,
				left.y * y);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and { value, value }
 */
LINEARLIBDEF vec2_t
ll_vec2_mul1f(vec2_t left, float value)
{
	return ll_vec2_create2f(left.x * value,
				left.y * value);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and @right.
 */
LINEARLIBDEF vec2_t
ll_vec2_div2fv(vec2_t left, vec2_t right)
{
	return ll_vec2_create2f(left.x / right.x,
				left.y / right.y);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { x, y }.
 */
LINEARLIBDEF vec2_t
ll_vec2_div2f(vec2_t left, float x, float y)
{
	return ll_vec2_create2f(left.x / x,
				left.y / y);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { value, value }.
 */
LINEARLIBDEF vec2_t
ll_vec2_div1f(vec2_t left, float value)
{
	return ll_vec2_create2f(left.x / value,
				left.y / value);
}

/**
 * @return A new vector, that is the dot product
 * of @left and @right.
 */
LINEARLIBDEF float
ll_vec2_dot2fv(vec2_t left, vec2_t right)
{
	return left.x * right.x + left.y * right.y;
}

/**
 * @return A new vector, that is the dot product
 * of @left and { x, y }.
 */
LINEARLIBDEF float
ll_vec2_dot2f(vec2_t left, float x, float y)
{
	return left.x * x + left.y * y;
}

/**
 * @return A new vector, that is the cross
 * product of @left and @right.
 */
LINEARLIBDEF float
ll_vec2_cross2fv(vec2_t left, vec2_t right)
{
	return left.x * right.y - left.y * right.x;
}

/**
 * @return A new vector, that is the cross
 * product of @left and { x, y }.
 */
LINEARLIBDEF float
ll_vec2_cross2f(vec2_t left, float x, float y)
{
	return left.x * y - left.y * x;
}

/**
 * @return A new vector, that is normalised so
 * that the returned vector's length is 1. It is
 * the unit vector of @vec.
 */
LINEARLIBDEF vec2_t
ll_vec2_normalise2fv(vec2_t vec)
{
	float length = ll_vec2_length2fv(vec);
	return ll_vec2_create2f( vec.x / length, vec.y / length );
}

/**
 * @return A new vector, that is normalised so
 * that the returned vector's length is 1. It is
 * the unit vector of { x, y }
 */
LINEARLIBDEF vec2_t
ll_vec2_normalise2f(float x, float y)
{
	float length = ll_vec2_length2f(x, y);
	return ll_vec2_create2f( x / length, y / length );
}

/**
 * @return A new vector, that is the result of applying @f
 * over all the elements of @vec.
 */
LINEARLIBDEF vec2_t
ll_vec2_apply2fv(vec2_t vec, float (*f)(float))
{
	return (vec2_t) {{ f(vec.x), f(vec.y) }};
}

/**
 * @return A new vector, that is the result of applying @f
 * over @x and @y, inserting their values into the components
 * of the returned vector.
 */
LINEARLIBDEF vec2_t
ll_vec2_apply2f(float x, float y, float (*f)(float))
{
	return (vec2_t) {{ f(x), f(y) }};
}

LINEARLIBDEF vec2_t
ll_vec2_origin(void)
{
	return (vec2_t) {{ 0.0, 0.0 }};
}

/**
 * @return A vec3_t structure.
 */
LINEARLIBDEF vec3_t
ll_vec3_create3f(float x, float y, float z)
{
	return (vec3_t) {{ x, y, z }};
}

/**
 * @return A copy of @vec.
 */
LINEARLIBDEF vec3_t
ll_vec3_create3fv(vec3_t vec)
{
	return vec;
}

/**
 * @return The length of @vec.
 */
LINEARLIBDEF float
ll_vec3_length3fv(vec3_t vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

/**
 * @return The length of @vec.
 */
LINEARLIBDEF float
ll_vec3_length3f(float x, float y, float z)
{
	return sqrtf(x * x + y * y + z * z);
}

/**
 * @return The squared length of @vec.
 */
LINEARLIBDEF float
ll_vec3_length_squared3fv(vec3_t vec)
{
	float length = ll_vec3_length3fv(vec);
	return length * length;
}

/**
 * @return The squared length of the vector { x, y, z }.
 */
LINEARLIBDEF float
ll_vec3_length_squared3f(float x, float y, float z)
{
	float length = ll_vec3_length3f(x, y, z);
	return length * length;
}

/**
 * @return A new vector, that is the sum of @left
 * and @right.
 */
LINEARLIBDEF vec3_t
ll_vec3_add3fv(vec3_t left, vec3_t right)
{
	return ll_vec3_create3f(left.x + right.x, left.y + right.y,
				left.z + right.z);
}

/**
 * @return A new vector, that is the sum of @left
 * and { x, y, z }.
 */
LINEARLIBDEF vec3_t
ll_vec3_add3f(vec3_t left, float x, float y, float z)
{
	return ll_vec3_create3f(left.x + x, left.y + y,
				left.z + z);
}

/**
 * @return A new vector, that is the sum of @left
 * and { value, value, value }.
 */
LINEARLIBDEF vec3_t
ll_vec3_add1f(vec3_t left, float value)
{
	return ll_vec3_create3f(left.x + value, left.y + value,
				left.z + value);
}

/**
 * @return A new vector, that is the sum of @left
 * and -@right.
 */
LINEARLIBDEF vec3_t
ll_vec3_sub3fv(vec3_t left, vec3_t right)
{
	return ll_vec3_create3f(left.x - right.x, left.y - right.y,
				left.z - right.z);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -x, -y, -z }.
 */
LINEARLIBDEF vec3_t
ll_vec3_sub3f(vec3_t left, float x, float y, float z)
{
	return ll_vec3_create3f(left.x - x, left.y - y,
				left.z - z);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -value, -value, -value }.
 */
LINEARLIBDEF vec3_t
ll_vec3_sub1f(vec3_t left, float value)
{
	return ll_vec3_create3f(left.x - value, left.y - value,
				left.z - value);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and @right.
 */
LINEARLIBDEF vec3_t
ll_vec3_mul3fv(vec3_t left, vec3_t right)
{
	return ll_vec3_create3f(left.x * right.x, left.y * right.y,
				left.z * right.z);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and { x, y, z }.
 */
LINEARLIBDEF vec3_t
ll_vec3_mul3f(vec3_t left, float x, float y, float z)
{
	return ll_vec3_create3f(left.x * x, left.y * y,
				left.z * z);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and { value, value, value }.
 */
LINEARLIBDEF vec3_t
ll_vec3_mul1f(vec3_t left, float value)
{
	return ll_vec3_create3f(left.x * value, left.y * value,
				left.z * value);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and @right.
 */
LINEARLIBDEF vec3_t
ll_vec3_div3fv(vec3_t left, vec3_t right)
{
	return ll_vec3_create3f(left.x / right.x, left.y / right.y,
				left.z / right.z);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { x, y, z }.
 */
LINEARLIBDEF vec3_t
ll_vec3_div3f(vec3_t left, float x, float y, float z)
{
	return ll_vec3_create3f(left.x / x, left.y / y,
				left.z / z);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { value, value, value }.
 */
LINEARLIBDEF vec3_t
ll_vec3_div1f(vec3_t left, float value)
{
	return ll_vec3_create3f(left.x / value, left.y / value,
				left.z / value);
}

/**
 * @return The dot product of @left and @right.
 */
LINEARLIBDEF float
ll_vec3_dot3fv(vec3_t left, vec3_t right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

/**
 * @return The dot product of @left and { x, y, z }.
 */
LINEARLIBDEF float
ll_vec3_dot3f(vec3_t left, float x, float y, float z)
{
	return left.x * x + left.y * y + left.z * z;
}

/**
 * @return The cross product of @left and @right,
 * this is essentially a new vector that is perpendicular
 * to @left and @right.
 */
LINEARLIBDEF vec3_t
ll_vec3_cross3fv(vec3_t left, vec3_t right)
{
	return ll_vec3_create3f(left.y*right.z - left.z*right.y,
				left.z*right.x - left.x*right.z,
				left.x*right.y - left.y*right.x);
}

/**
 * @return The cross product of @left and { x, y, z },
 * this is essentially a new vector that is perpendicular
 * to @left and { x, y, z }.
 */
LINEARLIBDEF vec3_t
ll_vec3_cross3f(vec3_t left, float x, float y, float z)
{
	return ll_vec3_create3f(left.y*z - left.z*y,
				left.z*x - left.x*z,
				left.x*y - left.y*x);
}

/**
 * @return A normalised vector, such that it's
 * length is 1. This is essentially the unit vector
 * of @vec.
 */
LINEARLIBDEF vec3_t
ll_vec3_normalise3fv(vec3_t vec)
{
	float length = ll_vec3_length3fv(vec);
	return ll_vec3_create3f( vec.x / length, vec.y / length,
				 vec.z / length );
}

/**
 * @return A normalised vector, such that it's
 * length is 1. This is essentially the unit vector
 * of { x, y, z }.
 */
LINEARLIBDEF vec3_t
ll_vec3_normalise3f(float x, float y, float z)
{
	float length = ll_vec3_length3f(x, y, z);
	return ll_vec3_create3f( x / length, y / length,
				 z / length );
}

/**
 * @return A vec3_t structure.
 */
LINEARLIBDEF vec4_t
ll_vec4_create4f(float x, float y, float z, float w)
{
	return (vec4_t) {{ x, y, z, w }};
}

/**
 * @return A copy of @vec.
 */
LINEARLIBDEF vec4_t
ll_vec4_create4fv(vec4_t vec)
{
	return vec;
}

/**
 * @return The length of @vec.
 */
LINEARLIBDEF float
ll_vec4_length4fv(vec4_t vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z
		     + vec.w * vec.w);
}

/**
 * @return The length of @vec.
 */
LINEARLIBDEF float
ll_vec4_length4f(float x, float y, float z, float w)
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

/**
 * @return The squared length of @vec.
 */
LINEARLIBDEF float
ll_vec4_length_squared4fv(vec4_t vec)
{
	float length = ll_vec4_length4fv(vec);
	return length * length;
}

/**
 * @return The squared length of the vector { x, y, z, w }.
 */
LINEARLIBDEF float
ll_vec4_length_squared4f(float x, float y, float z, float w)
{
	float length = ll_vec4_length4f(x, y, z, w);
	return length * length;
}

/**
 * @return A new vector, that is the sum of @left
 * and @right.
 */
LINEARLIBDEF vec4_t
ll_vec4_add4fv(vec4_t left, vec4_t right)
{
	return ll_vec4_create4f(left.x + right.x, left.y + right.y,
				left.z + right.z, left.w + right.w);
}

/**
 * @return A new vector, that is the sum of @left
 * and { x, y, z, w }.
 */
LINEARLIBDEF vec4_t
ll_vec4_add4f(vec4_t left, float x, float y, float z, float w)
{
	return ll_vec4_create4f(left.x + x, left.y + y,
				left.z + z, left.w + w);
}

/**
 * @return A new vector, that is the sum of @left
 * and { value, value, value, value }.
 */
LINEARLIBDEF vec4_t
ll_vec4_add1f(vec4_t left, float value)
{
	return ll_vec4_create4f(left.x + value, left.y + value,
				left.z + value, left.w + value);
}

/**
 * @return A new vector, that is the sum of @left
 * and -@right.
 */
LINEARLIBDEF vec4_t
ll_vec4_sub4fv(vec4_t left, vec4_t right)
{
	return ll_vec4_create4f(left.x - right.x, left.y - right.y,
				left.z - right.z, left.w - right.w);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -x, -y, -z, -w }.
 */
LINEARLIBDEF vec4_t
ll_vec4_sub4f(vec4_t left, float x, float y, float z, float w)
{
	return ll_vec4_create4f(left.x - x, left.y - y,
				left.z - z, left.w - w);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -value, -value, -value, -value}.
 */
LINEARLIBDEF vec4_t
ll_vec4_sub1f(vec4_t left, float value)
{
	return ll_vec4_create4f(left.x - value, left.y - value,
				left.z - value, left.w - value);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and @right.
 */
LINEARLIBDEF vec4_t
ll_vec4_mul4fv(vec4_t left, vec4_t right)
{
	return ll_vec4_create4f(left.x * right.x, left.y * right.y,
				left.z * right.z, left.w * right.w);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and { x, y, z }.
 */
LINEARLIBDEF vec4_t
ll_vec4_mul4f(vec4_t left, float x, float y, float z, float w)
{
	return ll_vec4_create4f(left.x * x, left.y * y,
				left.z * z, left.w * w);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and { value, value, value, value }.
 */
LINEARLIBDEF vec4_t
ll_vec4_mul1f(vec4_t left, float value)
{
	return ll_vec4_create4f(left.x * value, left.y * value,
				left.z * value, left.w * value);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and @right.
 */
LINEARLIBDEF vec4_t
ll_vec4_div4fv(vec4_t left, vec4_t right)
{
	return ll_vec4_create4f(left.x / right.x, left.y / right.y,
				left.z / right.z, left.w / right.w);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { x, y, z, w }.
 */
LINEARLIBDEF vec4_t
ll_vec4_div4f(vec4_t left, float x, float y, float z, float w)
{
	return ll_vec4_create4f(left.x / x, left.y / y,
				left.z / z, left.w / w);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { value, value, value, value }.
 */
LINEARLIBDEF vec4_t
ll_vec4_div1f(vec4_t left, float value)
{
	return ll_vec4_create4f(left.x / value, left.y / value,
				left.z / value, left.w / value);
}

/**
 * @return The dot product of @left and @right.
 */
LINEARLIBDEF float
ll_vec4_dot4fv(vec4_t left, vec4_t right)
{
	return left.x * right.x + left.y * right.y
		+ left.z * right.z + left.w * right.w;
}

/**
 * @return The dot product of @left and { x, y, z, w }.
 */
LINEARLIBDEF float
ll_vec4_dot4f(vec4_t left, float x, float y, float z, float w)
{
	return left.x * x + left.y * y + left.z * z
		+ left.w * w;
}

/**
 * @return A normalised vector, such that it's
 * length is 1. This is essentially the unit vector
 * of @vec.
 */
LINEARLIBDEF vec4_t
ll_vec4_normalise4fv(vec4_t vec)
{
	float length = ll_vec4_length4fv(vec);
	return ll_vec4_create4f( vec.x / length, vec.y / length,
				 vec.z / length, vec.w / length);
}

/**
 * @return A normalised vector, such that it's
 * length is 1. This is essentially the unit vector
 * of { x, y, z, w }.
 */
LINEARLIBDEF vec4_t
ll_vec4_normalise4f(float x, float y, float z, float w)
{
	float length = ll_vec4_length4f(x, y, z, w);
	return ll_vec4_create4f( x / length, y / length,
				 z / length, w / length );
}

/**
 * @return A ivec2_t structure.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_create2i(int x, int y)
{
	return (ivec2_t) {{ x, y }};
}

/**
 * @description Essentially creates a new copy of @vec.
 *
 * @return A ivec2_t structure.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_create2iv(ivec2_t vec)
{
	return vec;
}

/**
 * @return The length of @vec.
 */
LINEARLIBDEF float
ll_ivec2_length2iv(ivec2_t vec)
{
	return sqrtf((vec.x * vec.x) + (vec.y * vec.y));
}

/**
 * @return The length of a vector, represented by { x, y }.
 */
LINEARLIBDEF float
ll_ivec2_length2i(int x, int y)
{
	return sqrtf((x * x) + (y * y));
}

/**
 * @return The squared length of @vec.
 */
LINEARLIBDEF float
ll_ivec2_length_squared2iv(ivec2_t vec)
{
	float length = ll_ivec2_length2iv(vec);
	return length*length;
}

/**
 * @return The squared length of vector, represented by { x, y }.
 */
LINEARLIBDEF float
ll_ivec2_length_squared2i(int x, int y)
{
	float length = ll_ivec2_length2i(x, y);
	return length*length;
}

/**
 * @return A new vector, that is the sum of @left and @right.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_add2iv(ivec2_t left, ivec2_t right)
{
	return ll_ivec2_create2i(left.x + right.x, left.y + right.y);
}

/**
 * @return A new vector, that is the sum of @left and { x, y }.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_add2i(ivec2_t left, int x, int y)
{
	return ll_ivec2_create2i(left.x + x, left.y + y);
}

/**
 * @return A new vector, that is the sum of @left
 * and { value, value }.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_add1i(ivec2_t left, int value)
{
	return ll_ivec2_create2i(left.x + value, left.y + value);
}

/**
 * @return A new vector, that is the sum of @left and -@right.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_sub2iv(ivec2_t left, ivec2_t right)
{
	return ll_ivec2_add2i(left, -right.x, -right.y);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -x, -y }.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_sub2i(ivec2_t left, int x, int y)
{
	return ll_ivec2_add2i(left, -x, -y);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -value, -value }.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_sub1i(ivec2_t left, int value)
{
	return ll_ivec2_add2i(left, -value, -value);
}

/**
 * @return A new vector that is the component-wise
 * product of @left and @right.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_mul2iv(ivec2_t left, ivec2_t right)
{
	return ll_ivec2_create2i(left.x * right.x,
				 left.y * right.y);
}

/**
 * @return A new vector that is the component-wise
 * product of @left and { x, y }.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_mul2i(ivec2_t left, int x, int y)
{
	return ll_ivec2_create2i(left.x * x,
				 left.y * y);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and { value, value }
 */
LINEARLIBDEF ivec2_t
ll_ivec2_mul1i(ivec2_t left, int value)
{
	return ll_ivec2_create2i(left.x * value,
				 left.y * value);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and @right.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_div2iv(ivec2_t left, ivec2_t right)
{
	return ll_ivec2_create2i(left.x / right.x,
				 left.y / right.y);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { x, y }.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_div2i(ivec2_t left, int x, int y)
{
	return ll_ivec2_create2i(left.x / x,
				 left.y / y);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { value, value }.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_div1i(ivec2_t left, int value)
{
	return ll_ivec2_create2i(left.x / value,
				 left.y / value);
}

/**
 * @return A new vector, that is the dot product
 * of @left and @right.
 */
LINEARLIBDEF float
ll_ivec2_dot2iv(ivec2_t left, ivec2_t right)
{
	return left.x * right.x + left.y * right.y;
}

/**
 * @return A new vector, that is the dot product
 * of @left and { x, y }.
 */
LINEARLIBDEF float
ll_ivec2_dot2i(ivec2_t left, int x, int y)
{
	return left.x * x + left.y * y;
}

/**
 * @return A new vector, that is the cross
 * product of @left and @right.
 */
LINEARLIBDEF float
ll_ivec2_cross2iv(ivec2_t left, ivec2_t right)
{
	return left.x * right.y - left.y * right.x;
}

/**
 * @return A new vector, that is the cross
 * product of @left and { x, y }.
 */
LINEARLIBDEF float
ll_ivec2_cross2i(ivec2_t left, int x, int y)
{
	return left.x * y - left.y * x;
}

/**
 * @return A new vector, that is normalised so
 * that the returned vector's length is 1. It is
 * the unit vector of @vec.
 */
LINEARLIBDEF ivec2_t
ll_ivec2_normalise2iv(ivec2_t vec)
{
	float length = ll_ivec2_length2iv(vec);
	return ll_ivec2_create2i( vec.x / length, vec.y / length );
}

/**
 * @return A new vector, that is normalised so
 * that the returned vector's length is 1. It is
 * the unit vector of { x, y }
 */
LINEARLIBDEF ivec2_t
ll_ivec2_normalise2i(int x, int y)
{
	float length = ll_ivec2_length2i(x, y);
	return ll_ivec2_create2i( x / length, y / length );
}

/**
 * @return A ivec3_t structure.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_create3i(int x, int y, int z)
{
	return (ivec3_t) {{ x, y, z }};
}

/**
 * @return A copy of @vec.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_create3iv(ivec3_t vec)
{
	return vec;
}

/**
 * @return The length of @vec.
 */
LINEARLIBDEF float
ll_ivec3_length3iv(ivec3_t vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

/**
 * @return The length of @vec.
 */
LINEARLIBDEF float
ll_ivec3_length3i(int x, int y, int z)
{
	return sqrtf(x * x + y * y + z * z);
}

/**
 * @return The squared length of @vec.
 */
LINEARLIBDEF float
ll_ivec3_length_squared3iv(ivec3_t vec)
{
	float length = ll_ivec3_length3iv(vec);
	return length * length;
}

/**
 * @return The squared length of the vector { x, y, z }.
 */
LINEARLIBDEF float
ll_ivec3_length_squared3i(int x, int y, int z)
{
	float length = ll_ivec3_length3i(x, y, z);
	return length * length;
}

/**
 * @return A new vector, that is the sum of @left
 * and @right.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_add3iv(ivec3_t left, ivec3_t right)
{
	return ll_ivec3_create3i(left.x + right.x, left.y + right.y,
				left.z + right.z);
}

/**
 * @return A new vector, that is the sum of @left
 * and { x, y, z }.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_add3i(ivec3_t left, int x, int y, int z)
{
	return ll_ivec3_create3i(left.x + x, left.y + y,
				left.z + z);
}

/**
 * @return A new vector, that is the sum of @left
 * and { value, value, value }.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_add1i(ivec3_t left, int value)
{
	return ll_ivec3_create3i(left.x + value, left.y + value,
				left.z + value);
}

/**
 * @return A new vector, that is the sum of @left
 * and -@right.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_sub3iv(ivec3_t left, ivec3_t right)
{
	return ll_ivec3_create3i(left.x - right.x, left.y - right.y,
				left.z - right.z);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -x, -y, -z }.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_sub3i(ivec3_t left, int x, int y, int z)
{
	return ll_ivec3_create3i(left.x - x, left.y - y,
				left.z - z);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -value, -value, -value }.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_sub1i(ivec3_t left, int value)
{
	return ll_ivec3_create3i(left.x - value, left.y - value,
				left.z - value);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and @right.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_mul3iv(ivec3_t left, ivec3_t right)
{
	return ll_ivec3_create3i(left.x * right.x, left.y * right.y,
				left.z * right.z);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and { x, y, z }.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_mul3i(ivec3_t left, int x, int y, int z)
{
	return ll_ivec3_create3i(left.x * x, left.y * y,
				left.z * z);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and { value, value, value }.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_mul1i(ivec3_t left, int value)
{
	return ll_ivec3_create3i(left.x * value, left.y * value,
				left.z * value);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and @right.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_div3iv(ivec3_t left, ivec3_t right)
{
	return ll_ivec3_create3i(left.x / right.x, left.y / right.y,
				left.z / right.z);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { x, y, z }.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_div3i(ivec3_t left, int x, int y, int z)
{
	return ll_ivec3_create3i(left.x / x, left.y / y,
				left.z / z);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { value, value, value }.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_div1i(ivec3_t left, int value)
{
	return ll_ivec3_create3i(left.x / value, left.y / value,
				left.z / value);
}

/**
 * @return The dot product of @left and @right.
 */
LINEARLIBDEF float
ll_ivec3_dot3iv(ivec3_t left, ivec3_t right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

/**
 * @return The dot product of @left and { x, y, z }.
 */
LINEARLIBDEF float
ll_ivec3_dot3i(ivec3_t left, int x, int y, int z)
{
	return left.x * x + left.y * y + left.z * z;
}

/**
 * @return The cross product of @left and @right,
 * this is essentially a new vector that is perpendicular
 * to @left and @right.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_cross3iv(ivec3_t left, ivec3_t right)
{
	return ll_ivec3_create3i(left.y*right.z - left.z*right.y,
				left.z*right.x - left.x*right.z,
				left.x*right.y - left.y*right.x);
}

/**
 * @return The cross product of @left and { x, y, z },
 * this is essentially a new vector that is perpendicular
 * to @left and { x, y, z }.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_cross3i(ivec3_t left, int x, int y, int z)
{
	return ll_ivec3_create3i(left.y*z - left.z*y,
				left.z*x - left.x*z,
				left.x*y - left.y*x);
}

/**
 * @return A normalised vector, such that it's
 * length is 1. This is essentially the unit vector
 * of @vec.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_normalise3iv(ivec3_t vec)
{
	float length = ll_ivec3_length3iv(vec);
	return ll_ivec3_create3i( vec.x / length, vec.y / length,
				 vec.z / length );
}

/**
 * @return A normalised vector, such that it's
 * length is 1. This is essentially the unit vector
 * of { x, y, z }.
 */
LINEARLIBDEF ivec3_t
ll_ivec3_normalise3i(int x, int y, int z)
{
	float length = ll_ivec3_length3i(x, y, z);
	return ll_ivec3_create3i( x / length, y / length,
				  z / length );
}

/**
 * @return A vec3_t structure.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_create4i(int x, int y, int z, int w)
{
	return (ivec4_t) {{ x, y, z, w }};
}

/**
 * @return A copy of @vec.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_create4iv(ivec4_t vec)
{
	return vec;
}

/**
 * @return The length of @vec.
 */
LINEARLIBDEF float
ll_ivec4_length4iv(ivec4_t vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z
		     + vec.w * vec.w);
}

/**
 * @return The length of @vec.
 */
LINEARLIBDEF float
ll_ivec4_length4i(int x, int y, int z, int w)
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

/**
 * @return The squared length of @vec.
 */
LINEARLIBDEF float
ll_ivec4_length_squared4iv(ivec4_t vec)
{
	float length = ll_ivec4_length4iv(vec);
	return length * length;
}

/**
 * @return The squared length of the vector { x, y, z, w }.
 */
LINEARLIBDEF float
ll_ivec4_length_squared4i(int x, int y, int z, int w)
{
	float length = ll_ivec4_length4i(x, y, z, w);
	return length * length;
}

/**
 * @return A new vector, that is the sum of @left
 * and @right.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_add4iv(ivec4_t left, ivec4_t right)
{
	return ll_ivec4_create4i(left.x + right.x, left.y + right.y,
				left.z + right.z, left.w + right.w);
}

/**
 * @return A new vector, that is the sum of @left
 * and { x, y, z, w }.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_add4i(ivec4_t left, int x, int y, int z, int w)
{
	return ll_ivec4_create4i(left.x + x, left.y + y,
				left.z + z, left.w + w);
}

/**
 * @return A new vector, that is the sum of @left
 * and { value, value, value, value }.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_add1i(ivec4_t left, int value)
{
	return ll_ivec4_create4i(left.x + value, left.y + value,
				left.z + value, left.w + value);
}

/**
 * @return A new vector, that is the sum of @left
 * and -@right.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_sub4iv(ivec4_t left, ivec4_t right)
{
	return ll_ivec4_create4i(left.x - right.x, left.y - right.y,
				left.z - right.z, left.w - right.w);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -x, -y, -z, -w }.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_sub4i(ivec4_t left, int x, int y, int z, int w)
{
	return ll_ivec4_create4i(left.x - x, left.y - y,
				left.z - z, left.w - w);
}

/**
 * @return A new vector, that is the sum of @left
 * and { -value, -value, -value, -value}.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_sub1i(ivec4_t left, int value)
{
	return ll_ivec4_create4i(left.x - value, left.y - value,
				left.z - value, left.w - value);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and @right.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_mul4iv(ivec4_t left, ivec4_t right)
{
	return ll_ivec4_create4i(left.x * right.x, left.y * right.y,
				left.z * right.z, left.w * right.w);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and { x, y, z }.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_mul4i(ivec4_t left, int x, int y, int z, int w)
{
	return ll_ivec4_create4i(left.x * x, left.y * y,
				left.z * z, left.w * w);
}

/**
 * @return A new vector, that is the component-wise
 * product of @left and { value, value, value, value }.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_mul1i(ivec4_t left, int value)
{
	return ll_ivec4_create4i(left.x * value, left.y * value,
				left.z * value, left.w * value);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and @right.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_div4iv(ivec4_t left, ivec4_t right)
{
	return ll_ivec4_create4i(left.x / right.x, left.y / right.y,
				left.z / right.z, left.w / right.w);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { x, y, z, w }.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_div4i(ivec4_t left, int x, int y, int z, int w)
{
	return ll_ivec4_create4i(left.x / x, left.y / y,
				left.z / z, left.w / w);
}

/**
 * @return A new vector, that is the component-wise
 * division of @left and { value, value, value, value }.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_div1i(ivec4_t left, int value)
{
	return ll_ivec4_create4i(left.x / value, left.y / value,
				left.z / value, left.w / value);
}

/**
 * @return The dot product of @left and @right.
 */
LINEARLIBDEF float
ll_ivec4_dot4iv(ivec4_t left, ivec4_t right)
{
	return left.x * right.x + left.y * right.y
		+ left.z * right.z + left.w * right.w;
}

/**
 * @return The dot product of @left and { x, y, z, w }.
 */
LINEARLIBDEF float
ll_ivec4_dot4i(ivec4_t left, int x, int y, int z, int w)
{
	return left.x * x + left.y * y + left.z * z
		+ left.w * w;
}

/**
 * @return A normalised vector, such that it's
 * length is 1. This is essentially the unit vector
 * of @vec.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_normalise4iv(ivec4_t vec)
{
	float length = ll_ivec4_length4iv(vec);
	return ll_ivec4_create4i( vec.x / length, vec.y / length,
				 vec.z / length, vec.w / length);
}

/**
 * @return A normalised vector, such that it's
 * length is 1. This is essentially the unit vector
 * of { x, y, z, w }.
 */
LINEARLIBDEF ivec4_t
ll_ivec4_normalise4i(int x, int y, int z, int w)
{
	float length = ll_ivec4_length4i(x, y, z, w);
	return ll_ivec4_create4i( x / length, y / length,
				  z / length, w / length );
}

/**
 * @description performs matrix multiplcation on the matrices @l and @r
 * matrix multiplication is not commutative, so order matters @left
 * corresponds to the matrix on the left. @right corresponds to the
 * matrix on the right.
 */
LINEARLIBDEF void
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

/**
 * @description copy the matrix contents of @from into @to
 */
LINEARLIBDEF void
ll_mat4_copy(mat4_t *to,  mat4_t *from)
{
        for (int i = 0; i < 16; i++)
                to->data[i] = from->data[i];
}

/**
 * @description stores the identity matrix into @m
 */
LINEARLIBDEF void
ll_mat4_identity(mat4_t *mat)
{
	int i;
	memset(mat, 0, sizeof(*mat));
        mat->m00 = 1.0;
        mat->m11 = 1.0;
        mat->m22 = 1.0;
        mat->m33 = 1.0;
}

/**
 * @description stores a translation matrix inside of @mat with @dx, @dy and @dz.
 */
LINEARLIBDEF void
ll_mat4_translate3f(mat4_t *mat, float dx, float dy, float dz)
{
        ll_mat4_identity(mat);
        mat->m30 = dx;
        mat->m31 = dy;
        mat->m32 = dz;
}

/**
 * @description same as above, but instead allows to supply a 3-dimensional
 * vector as the 2nd argument, containing (dx, dy, dz)
 */
LINEARLIBDEF void
ll_mat4_translate3fv(mat4_t *mat, vec3_t vec)
{
        ll_mat4_translate3f(mat, vec.x, vec.y, vec.z);
}

/**
 * @description multiplies @m by a scaling matrix with components (w, h, d)
 * storing the result back into @m
 */
LINEARLIBDEF void
ll_mat4_scale3f(mat4_t *mat, float w, float h, float d)
{
        if (!mat) return;
        ll_mat4_identity(mat);
        mat->m00 = w;
        mat->m11 = h;
        mat->m22 = d;
}

/**
 * @description same as above, but instead allows to supply a 3-dimensional
 * vector as the 2nd arugment, containing (w, h, d)
 */
LINEARLIBDEF void
ll_mat4_scale3fv(mat4_t *mat, vec3_t vec)
{
        ll_mat4_scale3f(mat, vec.x, vec.y, vec.z);
}

/**
 * @description found whilst reading over rougier/freetype-gl implementation.
 * that can be found here: https://github.com/rougier/freetype-gl
 */
LINEARLIBDEF void
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

/**
 * @descriptionsame as above, but instead allows to supply a 3-dimensional
 * vector as the 2nd argument, containing (x, y, z)
 */
LINEARLIBDEF void
ll_mat4_rotate3fv(mat4_t *mat, vec3_t vec, float theta)
{
        ll_mat4_rotate3f(mat, vec.x, vec.y, vec.z, theta);
}

LINEARLIBDEF void
ll_mat4_translate3f2(mat4_t *mat, float dx, float dy, float dz)
{
	mat4_t m;
        ll_mat4_translate3f(&m, dx, dy, dz);
        ll_mat4_multiply(mat, &m);
}

LINEARLIBDEF void
ll_mat4_translate3fv2(mat4_t *mat, vec3_t vec)
{
	ll_mat4_translate3f2(mat, vec.x, vec.y, vec.z);
}

LINEARLIBDEF void
ll_mat4_scale3f2(mat4_t *mat, float w, float h, float d)
{
	mat4_t m;
        ll_mat4_scale3f(&m, w, h, d);
        ll_mat4_multiply(mat, &m);
}

LINEARLIBDEF void
ll_mat4_scale3fv2(mat4_t *mat, vec3_t vec)
{
	ll_mat4_scale3f2(mat, vec.x, vec.y, vec.z);
}

LINEARLIBDEF void
ll_mat4_rotate3f2(mat4_t *mat, float x, float y, float z, float angle)
{
	mat4_t m;
        ll_mat4_rotate3f(&m, x, y, z, angle);
        ll_mat4_multiply(mat, &m);
}

LINEARLIBDEF void
ll_mat4_rotate3fv2(mat4_t *mat, vec3_t vec, float angle)
{
	ll_mat4_rotate3f2(mat, vec.x, vec.y, vec.z, angle);
}

/**
 * @description stores the orthographic matrix into @m,
 * details of how this works can be found online or
 * found at https://github.com/wwotz/linearlib/README.md
 */
LINEARLIBDEF void
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

/**
 * @description stores the perspective matrix into @mat,
 * details of how this works can be found online or
 * found at https://github.com/wwotz/linearlib/README.md
 */
LINEARLIBDEF void
ll_mat4_perspective(mat4_t *mat, float fovy, float aspect,
                    float near, float far)
{
        float w, h;
        if (!mat || near == far) return;
        
        h = (float) tan(fovy / 360.0 * M_PI) * near;
        w = h * aspect;
        ll_mat4_frustum(mat, -h, w, h, -w, near, far);
}

/**
 * @description stores the frustum matrix into @mat,
 * details of how this works can be found online or
 * found at https://github.com/wwotz/linearlib/README.md
 */
LINEARLIBDEF void
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

/**
 * @description stores the lookat matrix into @mat,
 * details of how this works can be found online or
 * found at https://github.com/wwotz/linearlib/README.md
 */
LINEARLIBDEF void
ll_mat4_lookat(mat4_t *mat, vec3_t x, vec3_t y, vec3_t z,
	       vec3_t lookat)
{
	ll_mat4_identity(mat);
	mat->m00 = x.x;
	mat->m01 = y.x;
	mat->m02 = z.x;
	mat->m03 = 0.0;
	mat->m10 = x.y;
	mat->m11 = y.y;
	mat->m12 = z.y;
	mat->m13 = 0.0;
	mat->m20 = x.z;
	mat->m21 = y.z;
	mat->m22 = z.z;
	mat->m23 = 0.0;
	mat->m30 = -ll_vec3_dot3fv(x, lookat);
	mat->m31 = -ll_vec3_dot3fv(y, lookat);
	mat->m32 = -ll_vec3_dot3fv(z, lookat);
	mat->m33 = 1.0;
}

LINEARLIBDEF void
ll_quaternion_to_mat4(quaternion_t q, mat4_t *mat)
{
	float q00, q01, q11, q12, q03, q13, q02, q22, q23, q33;
	q00 = q.s*q.s;
	q01 = q.s*q.v.data[0];
	q11 = q.v.data[0]*q.v.data[0];
	q12 = q.v.data[0]*q.v.data[1];
	q03 = q.s*q.v.data[2];
	q13 = q.v.data[0]*q.v.data[2];
	q02 = q.s*q.v.data[1];
	q22 = q.v.data[1]*q.v.data[1];
	q23 = q.v.data[1]*q.v.data[2];
	q33 = q.v.data[2]*q.v.data[2];
	ll_mat4_identity(mat);
	mat->m00 = 2*(q00 + q11) - 1;
	mat->m01 = 2*(q12 - q03);
	mat->m02 = 2*(q13 + q02);
	mat->m10 = 2*(q12 + q03);
	mat->m11 = 2*(q00 + q22) - 1;
	mat->m12 = 2*(q23 - q01);
	mat->m20 = 2*(q13 - q02);
	mat->m21 = 2*(q23 + q01);
	mat->m22 = 2*(q00 + q33) - 1;
}

/** 
 * @description Create a quaternion with scale, @s and vector
 * components @x, @y and @z.
 *
 * @return A quaternion.
 */
LINEARLIBDEF quaternion_t
ll_quaternion_create3f(float s, float x, float y, float z)
{
	quaternion_t c;
	c.s = s;
	c.v.x = x;
	c.v.y = y;
	c.v.z = z;
	return c;
}

/** 
 * @description Create a quaternion with rotation, @s and 
 * vector @v.
 *
 * @return A quaternion.
 */
LINEARLIBDEF quaternion_t
ll_quaternion_create3fv(float s, vec3_t v)
{
	quaternion_t c;
	c.s = s;
	c.v = v;
	return c;
}

/** 
 * @description Create a quaternion with rotation defined in radians
 * about the axis defined by the vector components @x, @y and @z.
 *
 * @return A quaternion.
 */
LINEARLIBDEF quaternion_t
ll_quaternion_angle_axis3f(float radians, float x, float y, float z)
{
	vec3_t v;
	quaternion_t c;
	radians /= 2.0;
	v = ll_vec3_normalise3f(x, y, z);
	c = ll_quaternion_create3fv(cosf(radians), ll_vec3_mul1f(v, sinf(radians)));
	return c;
}

/** 
 * @description Create a quaternion with rotation defined in radians
 * about the axis defined by the vector components @x, @y and @z.
 *
 * @return A quaternion.
 */
LINEARLIBDEF quaternion_t
ll_quaternion_angle_axis3fv(float radians, vec3_t v)
{
	quaternion_t c;
	radians /= 2.0;
	v = ll_vec3_normalise3fv(v);
	c = ll_quaternion_create3fv(cosf(radians), ll_vec3_mul1f(v, sinf(radians)));
	return c;
}

/** 
 * @description Add quaternions @a and @b to produce a new
 * quaternion.
 *
 * @return A quaternion.
 */
LINEARLIBDEF quaternion_t 
ll_quaternion_add(quaternion_t a, quaternion_t b)
{
	quaternion_t c;
	c.s = a.s + b.s;
	c.v = ll_vec3_add3fv(a.v, b.v);
	return c;
}

/** 
 * @description Subtract quaternions @a and @b to produce a new
 * quaternion.
 *
 * @return A quaternion.
 */
LINEARLIBDEF quaternion_t
ll_quaternion_sub(quaternion_t a, quaternion_t b)
{
	quaternion_t c;
	c.s = a.s - b.s;
	c.v = ll_vec3_sub3fv(a.v, b.v);
	return c;
}

/** 
 * @description Multiply quaternions @a and @b to produce a new
 * quaternion, known as the Hamilton Product.
 *
 * @return A quaternion.
 */
LINEARLIBDEF quaternion_t
ll_quaternion_prod(quaternion_t a, quaternion_t b)
{
	quaternion_t c;
	c.s = a.s*b.s - ll_vec3_dot3fv(a.v, b.v);
	c.v = ll_vec3_add3fv(ll_vec3_mul1f(b.v, a.s),
			     ll_vec3_add3fv(ll_vec3_mul1f(a.v, b.s),
					    ll_vec3_cross3fv(a.v, b.v)));
	return c;
}

/** 
 * @description Multiply quaternions @a and @b to produce a new
 * scalar float, known as the Quaternion Dot Product.
 *
 * @return A scalar float.
 */
LINEARLIBDEF float
ll_quaternion_dot(quaternion_t a, quaternion_t b)
{
	float dot;
	dot = a.s * b.s;
	dot += ll_vec3_dot3fv(a.v, b.v);
	return dot;
}

/** 
 * @description Compute the norm of the quaternion @a.
 *
 * @return A scalar float.
 */
LINEARLIBDEF float
ll_quaternion_norm(quaternion_t a)
{
	return sqrtf(a.s*a.s + ll_vec3_dot3fv(a.v, a.v));
}

/** 
 * @description Normalise the quaternion @a.
 *
 * @return A normalised version of the quaternion @a.
 */
LINEARLIBDEF quaternion_t
ll_quaternion_normalise(quaternion_t a)
{
	int i;
	float norm;
	quaternion_t c;

	norm = ll_quaternion_norm(a);
	c.s = a.s / norm;
	c.v.data[0] = a.v.data[0] / norm;
	c.v.data[1] = a.v.data[1] / norm;
	c.v.data[2] = a.v.data[2] / norm;
	return c;
}

/** 
 * @description Compute the conjugate of the quaternion @a.
 *
 * @return The conjugate of the quaternion @a.
 */
LINEARLIBDEF quaternion_t
ll_quaternion_conjugate(quaternion_t a)
{
	quaternion_t c;
	c.s = a.s;
	c.v = ll_vec3_mul1f(a.v, -1.0);
	return c;
}

/** 
 * @description Compute the inverse of the quaternion @a.
 *
 * @return The inverse of the quaternion @a.
 */
LINEARLIBDEF quaternion_t
ll_quaternion_inverse(quaternion_t a)
{
	quaternion_t c;
	float sqrd_norm;
	c = ll_quaternion_conjugate(a);
	sqrd_norm = a.s*a.s + ll_vec3_dot3fv(a.v, a.v);
	c.s = c.s / sqrd_norm;
	c.v.data[0] = c.v.data[0] / sqrd_norm;
	c.v.data[1] = c.v.data[1] / sqrd_norm;
	c.v.data[2] = c.v.data[2] / sqrd_norm;
	return c;
}

/** 
 * @description Apply the quaternion to the vector components
 * @x, @y and @z.
 *
 * @return A rotated vec3_t.
 */
LINEARLIBDEF vec3_t
ll_quaternion_rotate3f(quaternion_t a, float x, float y, float z)
{
	quaternion_t c, a_conj;
	c = ll_quaternion_create3f(0.0, x, y, z);
	a_conj = ll_quaternion_conjugate(a);
	c = ll_quaternion_prod(ll_quaternion_prod(a, c), a_conj);
	return c.v;
}

/** 
 * @description Apply the quaternion to the vector @v.
 *
 * @return A rotated vec3_t.
 */
LINEARLIBDEF vec3_t
ll_quaternion_rotate3fv(quaternion_t a, vec3_t v)
{
	quaternion_t c, a_conj;
	c = ll_quaternion_create3fv(0.0, v);
	a_conj = ll_quaternion_conjugate(a);
	c = ll_quaternion_prod(ll_quaternion_prod(a, c), a_conj);
	return c.v;
}

#ifdef LL_USE_MATRIX

static mat4_t ll_matrices[LL_MATRIX_COUNT]; /* Model, View, Projection Matrices */
static int ll_matrices_idx;                 /* The currently bound matrix from @ll_matrices */

/**
 * @description Binds the current matrix to @type.
 */
LINEARLIBDEF void
ll_matrix_mode(matrix_type_t type)
{
        if (type >= 0 && type < LL_MATRIX_COUNT)
                ll_matrices_idx = type;
}

/**
 * @description Performs matrix multiplication on the currently
 * bound matrix, with the @right matrix.
 */
LINEARLIBDEF void
ll_matrix_multiply(mat4_t *right)
{
        ll_mat4_multiply(ll_matrices+ll_matrices_idx, right);
}

/**
 * @description Places the identity matrix (I) into the
 * currently bound matrix.
 */
LINEARLIBDEF void
ll_matrix_identity(void)
{
        ll_mat4_identity(ll_matrices+ll_matrices_idx);
}

/**
 * @description Multiplies the translation matrix by the
 * currently bound matrix. The translation matrix is
 * defined as translating some 3D coordinate by the
 * vector { dx, dy, dz }.
 */
LINEARLIBDEF void
ll_matrix_translate3f(float dx, float dy, float dz)
{
        mat4_t m;
        ll_mat4_translate3f(&m, dx, dy, dz);
        ll_matrix_multiply(&m);
}

/**
 * @description Multiplies the translation matrix by the
 * currently bound matrix. The translation matrix is
 * defined as translating some 3D coordinate by the
 * vector @vec.
 */
LINEARLIBDEF void
ll_matrix_translate3fv(vec3_t vec)
{
        ll_matrix_translate3f(vec.x, vec.y, vec.z);
}

/**
 * @description Multiplies the scale matrix by the
 * currently bound matrix. The scale matrix is
 * defined as scaling some 3D coordinate by the
 * vector { w, h, d }. 
 */
LINEARLIBDEF void
ll_matrix_scale3f(float w, float h, float d)
{
        mat4_t m;
        ll_mat4_scale3f(&m, w, h, d);
        ll_matrix_multiply(&m);
}

/**
 * @description Multiplies the scale matrix by the
 * currently bound matrix. The scale matrix is
 * defined as scaling some 3D coordinate by the
 * vector @vec.
 */
LINEARLIBDEF void
ll_matrix_scale3fv(vec3_t vec)
{
        ll_matrix_scale3f(vec.x, vec.y, vec.z);
}

/**
 * @description Multiplies the rotation matrix by the
 * currently bound matrix. The rotation matrix is
 * defined as rotating some 3D coordinate around the
 * axis v = { x, y, z }, where |v| = 1. By an angle
 * @angle. @angle is defined in degrees [0, 360].
 */
LINEARLIBDEF void
ll_matrix_rotate3f(float x, float y, float z, float angle)
{
        mat4_t m;
        ll_mat4_rotate3f(&m, x, y, z, angle);
        ll_matrix_multiply(&m);
}

/**
 * @description Multiplies the rotation matrix by the
 * currently bound matrix. The rotation matrix is
 * defined as rotating some 3D coordinate around the
 * axis @vec, where |@vec| = 1. By an angle
 * @angle. @angle is defined in degrees [0, 360].
 */
LINEARLIBDEF void
ll_matrix_rotate3fv(vec3_t vec, float angle)
{
        ll_matrix_rotate3f(vec.x, vec.y, vec.z, angle);
}

/**
 * @description Stores the orthographic matrix
 * into the currently bound matrix. 
 */
LINEARLIBDEF void
ll_matrix_orthographic(float top, float right,
                       float bottom, float left, float near, float far)
{
        ll_mat4_orthographic(ll_matrices+ll_matrices_idx, top,
                             right, bottom, left, near, far);
}

/**
 * @description Stores the orthographic matrix
 * into the currently bound matrix. 
 */
LINEARLIBDEF void
ll_matrix_perspective(float fovy, float aspect,
                      float near, float far)
{
        ll_mat4_perspective(ll_matrices+ll_matrices_idx,
                            fovy, aspect, near, far);
}

/**
 * @description Stores the frustum matrix
 * into the currently bound matrix. 
 */
LINEARLIBDEF void
ll_matrix_frustum(float left, float right,
                  float bottom, float top, float near, float far)
{
        ll_mat4_frustum(ll_matrices+ll_matrices_idx, left, right,
                        bottom, top, near, far);
}

/**
 * @description Stores the lookat matrix
 * into the currently bound matrix. 
 */
LINEARLIBDEF void
ll_matrix_lookat(vec3_t x, vec3_t y, vec3_t z, vec3_t lookat)
{
	ll_mat4_lookat(ll_matrices+ll_matrices_idx,
		       x, y, z, lookat);
}

/**
 * @return A copy of the currently bound matrix.
 * prevents modification of the internal matrix
 * that is bound.
 */
LINEARLIBDEF mat4_t
ll_matrix_get_copy(void)
{
        return ll_matrices[ll_matrices_idx];
}

LINEARLIBDEF int
ll_matrix_stack_pop(mat4_t *mat)
{
	if (ll_matrix_stack_size <= 0 || !mat) return -1;
	*mat = ll_matrix_stack[--ll_matrix_stack_size];
	return 0;
}

LINEARLIBDEF int
ll_matrix_stack_push(mat4_t *mat)
{
	if (ll_matrix_stack_size >= LL_MATRIX_STACK_CAPACITY || !mat) return -1;
	ll_matrix_stack[ll_matrix_stack_size++] = *mat;
	return 0;
}

/**
 * @description Converts a quaternion to a matrix and loads
 * it into the currently bound matrix.
 */
LINEARLIBDEF void
ll_quaternion_to_matrix(quaternion_t a)
{
	ll_quaternion_to_mat4(a, ll_matrices+ll_matrices_idx);
}

#endif /* LL_USE_MATRIX */
#endif /* LINEARLIB_IMPLEMENTATION */
#endif /* LINEARLIB_H_ */
