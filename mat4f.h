#ifndef ST_MAT4F
#define ST_MAT4F
#include "vec3f.h"

typedef struct Mat4f {
    // column 0
    float m00;
    float m01;
    float m02;
    float m03;

    // column 1
    float m10;
    float m11;
    float m12;
    float m13;

    // column 2
    float m20;
    float m21;
    float m22;
    float m23;

    // column 3
    float m30;
    float m31;
    float m32;
    float m33;
} Mat4f;

#define Mat4f_size (16 * sizeof(float))

Mat4f
mat4fIdentity();

Mat4f
mat4fVec3fTranslate(Mat4f mat, Vec3f vec);

Mat4f
mat4fPerspective(
        float fovy
        , float aspect
        , float zNear
        , float zFar
);

Mat4f
mat4fVec3fRotate(Mat4f mat, float angle, Vec3f vec);

Mat4f
mat4fLookAt(Vec3f position, Vec3f target, Vec3f up);

Mat4f
mat4fScale(Mat4f mat, Vec3f vec);

Mat4f
mat4fMulMat4f(Mat4f a, Mat4f const b);

Mat4f
mat4fMuli(Mat4f mat, int s);

Mat4f
mat4fMulf(Mat4f mat, float s);

Mat4f
mat4fInverse(Mat4f mat);

Mat4f
mat4fInverseTranspose(Mat4f mat);

void
mat4fPrint(Mat4f *mat);
#endif
