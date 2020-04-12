#include "mat4f.h"
#include "vec4f.h"
#include <math.h>
#include <stdio.h>

Mat4f
mat4fIdentity() {
    Mat4f ret;
    ret.m00 = 1;
    ret.m01 = 0;
    ret.m02 = 0;
    ret.m03 = 0;

    ret.m10 = 0;
    ret.m11 = 1;
    ret.m12 = 0;
    ret.m13 = 0;

    ret.m20 = 0;
    ret.m21 = 0;
    ret.m22 = 1;
    ret.m23 = 0;

    ret.m30 = 0;
    ret.m31 = 0;
    ret.m32 = 0;
    ret.m33 = 1;
    return ret;
}

Mat4f
mat4fVec3fTranslate(Mat4f mat, Vec3f vec) {
    Mat4f ret = mat;
    ret.m30 += vec.x;
    ret.m31 += vec.y;
    ret.m32 += vec.z;
    return ret;
}

Mat4f
mat4fPerspective(
        float fovy
        , float aspect
        , float zNear
        , float zFar
) {
    float tanHalfFovy = tanf(fovy / 2.0);
    float zDiff = zFar - zNear;

    Mat4f ret;
    ret.m00 = 1.0 / (aspect * tanHalfFovy);
    ret.m01 = 0;
    ret.m02 = 0;
    ret.m03 = 0;

    ret.m10 = 0;
    ret.m11 = 1.0 / tanHalfFovy;
    ret.m12 = 0;
    ret.m13 = 0;

    ret.m20 = 0;
    ret.m21 = 0;
    ret.m22 = -(zFar + zNear) / zDiff;
    ret.m23 = -1.0;

    ret.m30 = 0;
    ret.m31 = 0;
    ret.m32 = -(2.0 * zFar * zNear) / zDiff;
    ret.m33 = 0;
    return ret;
}

// source: https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
Mat4f
mat4fVec3fRotate(Mat4f m, float angle, Vec3f vec) {
    float a = angle;
    float c = cos(a);
    float s = sin(a);

    Vec3f axis = vec3fNormalize(vec);

    Vec3f temp = vec3fMulf(axis, 1.0 - c);

    Mat4f rot;
    rot.m00 = c + temp.x * axis.x;
    rot.m01 = temp.x * axis.y + s * axis.z;
    rot.m02 = temp.x * axis.z - s * axis.y;

    rot.m10 = temp.y * axis.x - s * axis.z;
    rot.m11 = c + temp.y * axis.y;
    rot.m12 = temp.y * axis.z + s * axis.x;

    rot.m20 = temp.z * axis.x + s * axis.y;
    rot.m21 = temp.z * axis.y - s * axis.x;
    rot.m22 = c + temp.z * axis.z;

    Mat4f ret;
    ret.m00 = m.m00 * rot.m00 + m.m10 * rot.m01 + m.m20 * rot.m02;
    ret.m01 = m.m01 * rot.m00 + m.m11 * rot.m01 + m.m21 * rot.m02;
    ret.m02 = m.m02 * rot.m00 + m.m12 * rot.m01 + m.m22 * rot.m02;
    ret.m03 = m.m03 * rot.m00 + m.m13 * rot.m01 + m.m23 * rot.m02;

    ret.m10 = m.m00 * rot.m10 + m.m10 * rot.m11 + m.m20 * rot.m12;
    ret.m11 = m.m01 * rot.m10 + m.m11 * rot.m11 + m.m21 * rot.m12;
    ret.m12 = m.m02 * rot.m10 + m.m12 * rot.m11 + m.m22 * rot.m12;
    ret.m13 = m.m03 * rot.m10 + m.m13 * rot.m11 + m.m23 * rot.m12;

    ret.m20 = m.m00 * rot.m20 + m.m10 * rot.m21 + m.m20 * rot.m22;
    ret.m21 = m.m01 * rot.m20 + m.m11 * rot.m21 + m.m21 * rot.m22;
    ret.m22 = m.m02 * rot.m20 + m.m12 * rot.m21 + m.m22 * rot.m22;
    ret.m23 = m.m03 * rot.m20 + m.m13 * rot.m21 + m.m23 * rot.m22;

    ret.m30 = m.m30;
    ret.m31 = m.m31;
    ret.m32 = m.m32;
    ret.m33 = m.m33;
    return ret;
}

Mat4f
mat4fLookAt(Vec3f position, Vec3f target, Vec3f up) {
    Vec3f f = vec3fNormalize(vec3fSub(target, position));

    Vec3f s = vec3fNormalize(vec3fCrossProduct(f, up));

    Vec3f u = vec3fCrossProduct(s, f);

    float sd = vec3fDotProduct(s, position);
    float ud = vec3fDotProduct(u, position);
    float fd = vec3fDotProduct(f, position);

    Mat4f ret;
    ret.m00 = s.x;
    ret.m01 = u.x;
    ret.m02 = -f.x;
    ret.m03 = 0;

    ret.m10 = s.y;
    ret.m11 = u.y;
    ret.m12 = -f.y;
    ret.m13 = 0;

    ret.m20 = s.z;
    ret.m21 = u.z;
    ret.m22 = -f.z;
    ret.m23 = 0;

    ret.m30 = -sd;
    ret.m31 = -ud;
    ret.m32 = fd;
    ret.m33 = 1;
    return ret;
}

Mat4f
mat4fScale(Mat4f mat, Vec3f vec) {
    Mat4f ret = mat;
    ret.m00 *= vec.x;
    ret.m01 *= vec.x;
    ret.m02 *= vec.x;

    ret.m10 *= vec.y;
    ret.m11 *= vec.y;
    ret.m12 *= vec.y;

    ret.m20 *= vec.z;
    ret.m21 *= vec.z;
    ret.m22 *= vec.z;

    return ret;
}

Mat4f
mat4fMulMat4f(Mat4f a, Mat4f b) {
    Mat4f ret;
    ret.m00 = a.m00*b.m00 + a.m10*b.m01 + a.m20*b.m02 + a.m30*b.m03;
    ret.m10 = a.m00*b.m10 + a.m10*b.m11 + a.m20*b.m12 + a.m30*b.m13;
    ret.m20 = a.m00*b.m20 + a.m10*b.m21 + a.m20*b.m22 + a.m30*b.m23;
    ret.m30 = a.m00*b.m30 + a.m10*b.m31 + a.m20*b.m32 + a.m30*b.m33;

    ret.m01 = a.m01*b.m00 + a.m11*b.m01 + a.m21*b.m02 + a.m31*b.m03;
    ret.m11 = a.m01*b.m10 + a.m11*b.m11 + a.m21*b.m12 + a.m31*b.m13;
    ret.m21 = a.m01*b.m20 + a.m11*b.m21 + a.m21*b.m22 + a.m31*b.m23;
    ret.m31 = a.m01*b.m30 + a.m11*b.m31 + a.m21*b.m32 + a.m31*b.m33;

    ret.m02 = a.m02*b.m00 + a.m12*b.m01 + a.m22*b.m02 + a.m32*b.m03;
    ret.m12 = a.m02*b.m10 + a.m12*b.m11 + a.m22*b.m12 + a.m32*b.m13;
    ret.m22 = a.m02*b.m20 + a.m12*b.m21 + a.m22*b.m22 + a.m32*b.m23;
    ret.m32 = a.m02*b.m30 + a.m12*b.m31 + a.m22*b.m32 + a.m32*b.m33;

    ret.m03 = a.m03*b.m00 + a.m13*b.m01 + a.m23*b.m02 + a.m33*b.m03;
    ret.m13 = a.m03*b.m10 + a.m13*b.m11 + a.m23*b.m12 + a.m33*b.m13;
    ret.m23 = a.m03*b.m20 + a.m13*b.m21 + a.m23*b.m22 + a.m33*b.m23;
    ret.m33 = a.m03*b.m30 + a.m13*b.m31 + a.m23*b.m32 + a.m33*b.m33;
    return ret;
}

Mat4f
mat4fMuli(Mat4f mat, int s) {
    Mat4f ret = mat;
    ret.m00 *= s;
    ret.m01 *= s;
    ret.m02 *= s;
    ret.m03 *= s;

    ret.m10 *= s;
    ret.m11 *= s;
    ret.m12 *= s;
    ret.m13 *= s;

    ret.m20 *= s;
    ret.m21 *= s;
    ret.m22 *= s;
    ret.m23 *= s;

    ret.m30 *= s;
    ret.m31 *= s;
    ret.m32 *= s;
    ret.m33 *= s;
    return ret;
}

Mat4f
mat4fMulf(Mat4f mat, float s) {
    Mat4f ret = mat;
    ret.m00 *= s;
    ret.m01 *= s;
    ret.m02 *= s;
    ret.m03 *= s;

    ret.m10 *= s;
    ret.m11 *= s;
    ret.m12 *= s;
    ret.m13 *= s;

    ret.m20 *= s;
    ret.m21 *= s;
    ret.m22 *= s;
    ret.m23 *= s;

    ret.m30 *= s;
    ret.m31 *= s;
    ret.m32 *= s;
    ret.m33 *= s;
    return ret;
}

Mat4f
mat4fDivf(Mat4f mat, float s) {
    Mat4f ret = mat;
    ret.m00 /= s;
    ret.m01 /= s;
    ret.m02 /= s;
    ret.m03 /= s;

    ret.m10 /= s;
    ret.m11 /= s;
    ret.m12 /= s;
    ret.m13 /= s;

    ret.m20 /= s;
    ret.m21 /= s;
    ret.m22 /= s;
    ret.m23 /= s;

    ret.m30 /= s;
    ret.m31 /= s;
    ret.m32 /= s;
    ret.m33 /= s;
    return ret;
}

/**
 * Source: https://github.com/g-truc/glm/blob/master/glm/detail/func_matrix.inl
 **/
Mat4f
mat4fInverse(Mat4f m) {
    float coef00 = m.m22 * m.m33 - m.m32 * m.m23;
    float coef02 = m.m12 * m.m33 - m.m32 * m.m13;
    float coef03 = m.m12 * m.m23 - m.m22 * m.m13;

    float coef04 = m.m21 * m.m33 - m.m31 * m.m23;
    float coef06 = m.m11 * m.m33 - m.m31 * m.m13;
    float coef07 = m.m11 * m.m23 - m.m21 * m.m13;

    float coef08 = m.m21 * m.m32 - m.m31 * m.m22;
    float coef10 = m.m11 * m.m32 - m.m31 * m.m12;
    float coef11 = m.m11 * m.m22 - m.m21 * m.m12;

    float coef12 = m.m20 * m.m33 - m.m30 * m.m23;
    float coef14 = m.m10 * m.m33 - m.m30 * m.m13;
    float coef15 = m.m10 * m.m23 - m.m20 * m.m13;

    float coef16 = m.m20 * m.m32 - m.m30 * m.m22;
    float coef18 = m.m10 * m.m32 - m.m30 * m.m12;
    float coef19 = m.m10 * m.m22 - m.m20 * m.m12;

    float coef20 = m.m20 * m.m31 - m.m30 * m.m21;
    float coef22 = m.m10 * m.m31 - m.m30 * m.m11;
    float coef23 = m.m10 * m.m21 - m.m20 * m.m11;

    Vec4f fac0 = {coef00, coef00, coef02, coef03};
    Vec4f fac1 = {coef04, coef04, coef06, coef07};
    Vec4f fac2 = {coef08, coef08, coef10, coef11};
    Vec4f fac3 = {coef12, coef12, coef14, coef15};
    Vec4f fac4 = {coef16, coef16, coef18, coef19};
    Vec4f fac5 = {coef20, coef20, coef22, coef23};

    Vec4f vec0 = {m.m10, m.m00, m.m00, m.m00};
    Vec4f vec1 = {m.m11, m.m01, m.m01, m.m01};
    Vec4f vec2 = {m.m12, m.m02, m.m02, m.m02};
    Vec4f vec3 = {m.m13, m.m03, m.m03, m.m03};

    Vec4f mul0 = vec4fMulVec4f(vec1, fac0);
    Vec4f mul1 = vec4fMulVec4f(vec2, fac1);
    Vec4f mul2 = vec4fMulVec4f(vec3, fac2);
    Vec4f sub0 = vec4fSubVec4f(mul0, mul1);
    Vec4f inv0 = vec4fAddVec4f(sub0, mul2);

    Vec4f mul3 = vec4fMulVec4f(vec0, fac0);
    Vec4f mul4 = vec4fMulVec4f(vec2, fac3);
    Vec4f mul5 = vec4fMulVec4f(vec3, fac4);
    Vec4f sub1 = vec4fSubVec4f(mul3, mul4);
    Vec4f inv1 = vec4fAddVec4f(sub1, mul5);

    Vec4f mul6 = vec4fMulVec4f(vec0, fac1);
    Vec4f mul7 = vec4fMulVec4f(vec1, fac3);
    Vec4f mul8 = vec4fMulVec4f(vec3, fac5);
    Vec4f sub2 = vec4fSubVec4f(mul6, mul7);
    Vec4f inv2 = vec4fAddVec4f(sub2, mul8);

    Vec4f mul9 = vec4fMulVec4f(vec0, fac2);
    Vec4f mul10 = vec4fMulVec4f(vec1, fac4);
    Vec4f mul11 = vec4fMulVec4f(vec2, fac5);
    Vec4f sub3 = vec4fSubVec4f(mul9, mul10);
    Vec4f inv3 = vec4fAddVec4f(sub3, mul11);

    Vec4f signA = {+1, -1, +1, -1};
    Vec4f signB = {-1, +1, -1, +1};

    Mat4f inverse;
    inverse.m00 = inv0.x * signA.x;
    inverse.m01 = inv0.y * signA.y;
    inverse.m02 = inv0.z * signA.z;
    inverse.m03 = inv0.w * signA.w;

    inverse.m10 = inv1.x * signB.x;
    inverse.m11 = inv1.y * signB.y;
    inverse.m12 = inv1.z * signB.z;
    inverse.m13 = inv1.w * signB.w;

    inverse.m20 = inv2.x * signA.x;
    inverse.m21 = inv2.y * signA.y;
    inverse.m22 = inv2.z * signA.z;
    inverse.m23 = inv2.w * signA.w;

    inverse.m30 = inv3.x * signB.x;
    inverse.m31 = inv3.y * signB.y;
    inverse.m32 = inv3.z * signB.z;
    inverse.m33 = inv3.w * signB.w;

    Vec4f row0 = {inverse.m00, inverse.m10, inverse.m20, inverse.m30};

    Vec4f dot0 = {
        .x = m.m00 * row0.x,
        .y = m.m01 * row0.y,
        .z = m.m02 * row0.z,
        .w = m.m03 * row0.w,
    };

    float dot1 = dot0.x + dot0.y + dot0.z + dot0.w;

    float oneOverDeterminant = 1.0 / dot1;

    return mat4fMulf(inverse, oneOverDeterminant);
}

/**
 * "Inspired" by glm's inverseTranspose
 * https://github.com/g-truc/glm/blob/master/glm/gtc/matrix_inverse.inl
 **/
Mat4f
mat4fInverseTranspose(Mat4f m) {
    float subFactor00 = m.m22 * m.m33 - m.m32 * m.m23;
    float subFactor01 = m.m21 * m.m33 - m.m31 * m.m23;
    float subFactor02 = m.m21 * m.m32 - m.m31 * m.m22;
    float subFactor03 = m.m20 * m.m33 - m.m30 * m.m23;
    float subFactor04 = m.m20 * m.m32 - m.m30 * m.m22;
    float subFactor05 = m.m20 * m.m31 - m.m30 * m.m21;
    float subFactor06 = m.m12 * m.m33 - m.m32 * m.m13;
    float subFactor07 = m.m11 * m.m33 - m.m31 * m.m13;
    float subFactor08 = m.m11 * m.m32 - m.m31 * m.m12;
    float subFactor09 = m.m10 * m.m33 - m.m30 * m.m13;
    float subFactor10 = m.m10 * m.m32 - m.m30 * m.m12;
    float subFactor11 = m.m10 * m.m31 - m.m30 * m.m11;
    float subFactor12 = m.m12 * m.m23 - m.m22 * m.m13;
    float subFactor13 = m.m11 * m.m23 - m.m21 * m.m13;
    float subFactor14 = m.m11 * m.m22 - m.m21 * m.m12;
    float subFactor15 = m.m10 * m.m23 - m.m20 * m.m13;
    float subFactor16 = m.m10 * m.m22 - m.m20 * m.m12;
    float subFactor17 = m.m10 * m.m21 - m.m20 * m.m11;

    Mat4f inverse;
    inverse.m00 = + (m.m11 * subFactor00 - m.m12 * subFactor01 + m.m13 * subFactor02);
    inverse.m01 = - (m.m10 * subFactor00 - m.m12 * subFactor03 + m.m13 * subFactor04);
    inverse.m02 = + (m.m10 * subFactor01 - m.m11 * subFactor03 + m.m13 * subFactor05);
    inverse.m03 = - (m.m10 * subFactor02 - m.m11 * subFactor04 + m.m12 * subFactor05);

    inverse.m10 = - (m.m01 * subFactor00 - m.m02 * subFactor01 + m.m03 * subFactor02);
    inverse.m11 = + (m.m00 * subFactor00 - m.m02 * subFactor03 + m.m03 * subFactor04);
    inverse.m12 = - (m.m00 * subFactor01 - m.m01 * subFactor03 + m.m03 * subFactor05);
    inverse.m13 = + (m.m00 * subFactor02 - m.m01 * subFactor04 + m.m02 * subFactor05);

    inverse.m20 = + (m.m01 * subFactor06 - m.m02 * subFactor07 + m.m03 * subFactor08);
    inverse.m21 = - (m.m00 * subFactor06 - m.m02 * subFactor09 + m.m03 * subFactor10);
    inverse.m22 = + (m.m00 * subFactor07 - m.m01 * subFactor09 + m.m03 * subFactor11);
    inverse.m23 = - (m.m00 * subFactor08 - m.m01 * subFactor10 + m.m02 * subFactor11);

    inverse.m30 = - (m.m01 * subFactor12 - m.m02 * subFactor13 + m.m03 * subFactor14);
    inverse.m31 = + (m.m00 * subFactor12 - m.m02 * subFactor15 + m.m03 * subFactor16);
    inverse.m32 = - (m.m00 * subFactor13 - m.m01 * subFactor15 + m.m03 * subFactor17);
    inverse.m33 = + (m.m00 * subFactor14 - m.m01 * subFactor16 + m.m02 * subFactor17);

    float determinant = + m.m00 * inverse.m00 + m.m01 * inverse.m01
        + m.m02 * inverse.m02 + m.m03 * inverse.m03;

    return mat4fDivf(inverse, determinant);
}

void
mat4fPrint(Mat4f *m) {
    printf(
        "%f %f %f %f\n"
        "%f %f %f %f\n"
        "%f %f %f %f\n"
        "%f %f %f %f\n",
        m->m00, m->m10, m->m20, m->m30,
        m->m00, m->m11, m->m21, m->m31,
        m->m00, m->m12, m->m22, m->m32,
        m->m00, m->m13, m->m23, m->m33
    );
}
