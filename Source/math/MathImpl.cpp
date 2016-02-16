#include "TheEngine/math/all.h"

const vec2 vec2_zero(0,0);
const vec3 vec3_zero(0,0,0);
const vec4 vec4_zero(0,0,0,1);

const mat2 mat2_zero(0,0,0,0);
const mat2 mat2_identity(1,0,0,1);

const mat3 mat3_zero(0,0,0,0,0,0,0,0,0);
const mat3 mat3_identity(1,0,0,0,1,0,0,0,1);

const mat4 mat4_zero(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
const mat4 mat4_identity(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

namespace the { namespace math {
const float EPSILON                 = 1e-6;
const float MATRIX_INVERSE_EPSILON  = 1e-14;
const float MATRIX_EPSILON          = 1e-6;
const float PI                      = 3.141592654;
const float TWO_PI                  = 2.0f * PI;
const float HALF_PI                 = 0.5f * PI;
const float SQRT_TWO                = 1.41421356237309504880f;
const float SQRT_THREE              = 1.73205080756887729352f;
const float DEG2RAD                 = PI/180.0;
const float RAD2DEG                 = 180.0/PI;
const float INF                     = 1e+9;
}}
///////////////////////////////////////////////// Matrix implementations

bool mat2::inverseSelf()
{
    double det, invDet, a;
    det = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
    if (the::math::abs(det) < the::math::MATRIX_INVERSE_EPSILON)
        return false;
    invDet = 1.0f / det;
    a = mat[0][0];
    mat[0][0] = mat[1][1] * invDet;
    mat[0][1] = -mat[0][1] * invDet;
    mat[1][0] = -mat[1][0] * invDet;
    mat[1][1] = a * invDet;
    return true;
}

bool mat4::inverseSelf()
{
    double det, invDet;

    float det2_01_01 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
    float det2_01_02 = mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0];
    float det2_01_03 = mat[0][0] * mat[1][3] - mat[0][3] * mat[1][0];
    float det2_01_12 = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
    float det2_01_13 = mat[0][1] * mat[1][3] - mat[0][3] * mat[1][1];
    float det2_01_23 = mat[0][2] * mat[1][3] - mat[0][3] * mat[1][2];

    float det3_201_012 = mat[2][0] * det2_01_12 - mat[2][1] * det2_01_02 + mat[2][2] * det2_01_01;
    float det3_201_013 = mat[2][0] * det2_01_13 - mat[2][1] * det2_01_03 + mat[2][3] * det2_01_01;
    float det3_201_023 = mat[2][0] * det2_01_23 - mat[2][2] * det2_01_03 + mat[2][3] * det2_01_02;
    float det3_201_123 = mat[2][1] * det2_01_23 - mat[2][2] * det2_01_13 + mat[2][3] * det2_01_12;

    det = ( - det3_201_123 * mat[3][0] + det3_201_023 * mat[3][1] - det3_201_013 * mat[3][2] + det3_201_012 * mat[3][3] );

    if (the::math::abs(det) < the::math::MATRIX_INVERSE_EPSILON ) return false;

    invDet = 1.0f / det;

    float det2_03_01 = mat[0][0] * mat[3][1] - mat[0][1] * mat[3][0];
    float det2_03_02 = mat[0][0] * mat[3][2] - mat[0][2] * mat[3][0];
    float det2_03_03 = mat[0][0] * mat[3][3] - mat[0][3] * mat[3][0];
    float det2_03_12 = mat[0][1] * mat[3][2] - mat[0][2] * mat[3][1];
    float det2_03_13 = mat[0][1] * mat[3][3] - mat[0][3] * mat[3][1];
    float det2_03_23 = mat[0][2] * mat[3][3] - mat[0][3] * mat[3][2];

    float det2_13_01 = mat[1][0] * mat[3][1] - mat[1][1] * mat[3][0];
    float det2_13_02 = mat[1][0] * mat[3][2] - mat[1][2] * mat[3][0];
    float det2_13_03 = mat[1][0] * mat[3][3] - mat[1][3] * mat[3][0];
    float det2_13_12 = mat[1][1] * mat[3][2] - mat[1][2] * mat[3][1];
    float det2_13_13 = mat[1][1] * mat[3][3] - mat[1][3] * mat[3][1];
    float det2_13_23 = mat[1][2] * mat[3][3] - mat[1][3] * mat[3][2];

    // remaining 3x3 sub-determinants
    float det3_203_012 = mat[2][0] * det2_03_12 - mat[2][1] * det2_03_02 + mat[2][2] * det2_03_01;
    float det3_203_013 = mat[2][0] * det2_03_13 - mat[2][1] * det2_03_03 + mat[2][3] * det2_03_01;
    float det3_203_023 = mat[2][0] * det2_03_23 - mat[2][2] * det2_03_03 + mat[2][3] * det2_03_02;
    float det3_203_123 = mat[2][1] * det2_03_23 - mat[2][2] * det2_03_13 + mat[2][3] * det2_03_12;

    float det3_213_012 = mat[2][0] * det2_13_12 - mat[2][1] * det2_13_02 + mat[2][2] * det2_13_01;
    float det3_213_013 = mat[2][0] * det2_13_13 - mat[2][1] * det2_13_03 + mat[2][3] * det2_13_01;
    float det3_213_023 = mat[2][0] * det2_13_23 - mat[2][2] * det2_13_03 + mat[2][3] * det2_13_02;
    float det3_213_123 = mat[2][1] * det2_13_23 - mat[2][2] * det2_13_13 + mat[2][3] * det2_13_12;

    float det3_301_012 = mat[3][0] * det2_01_12 - mat[3][1] * det2_01_02 + mat[3][2] * det2_01_01;
    float det3_301_013 = mat[3][0] * det2_01_13 - mat[3][1] * det2_01_03 + mat[3][3] * det2_01_01;
    float det3_301_023 = mat[3][0] * det2_01_23 - mat[3][2] * det2_01_03 + mat[3][3] * det2_01_02;
    float det3_301_123 = mat[3][1] * det2_01_23 - mat[3][2] * det2_01_13 + mat[3][3] * det2_01_12;

    mat[0][0] = - det3_213_123 * invDet;
    mat[1][0] = + det3_213_023 * invDet;
    mat[2][0] = - det3_213_013 * invDet;
    mat[3][0] = + det3_213_012 * invDet;

    mat[0][1] = + det3_203_123 * invDet;
    mat[1][1] = - det3_203_023 * invDet;
    mat[2][1] = + det3_203_013 * invDet;
    mat[3][1] = - det3_203_012 * invDet;

    mat[0][2] = + det3_301_123 * invDet;
    mat[1][2] = - det3_301_023 * invDet;
    mat[2][2] = + det3_301_013 * invDet;
    mat[3][2] = - det3_301_012 * invDet;

    mat[0][3] = - det3_201_123 * invDet;
    mat[1][3] = + det3_201_023 * invDet;
    mat[2][3] = - det3_201_013 * invDet;
    mat[3][3] = + det3_201_012 * invDet;
    return true;
}

///////////////////////////////////////////////// Quaternion implementations

quaternion::quaternion() {}
quaternion::quaternion(float real, float x, float y, float z): s(real), v(x, y, z) {}
quaternion::quaternion(float real, const vec3 &i): s(real), v(i) {}

float quaternion::length() const
{
    return the::math::sqrt(s * s + v * v);
}
void quaternion::normalize()
{
    *this = *this / length();
}
quaternion quaternion::normalized() const
{
    return  *this / length();
}

quaternion quaternion::slerp(const quaternion &q1, const quaternion &q2, float t)
{
    quaternion q3;
    float dot = quaternion::dot(q1, q2);
    if (dot < 0)
    {
        dot = -dot;
        q3 = -q2;
    }
    else q3 = q2;

    if (dot < 0.95f)
    {
        float angle = acosf(dot);
        return (q1 * sinf(angle * (1 - t)) + q3 * sinf(angle * t)) / sinf(angle);
    }
    else
        return lerp(q1, q3, t);
}

vec3 quaternion::euler(bool homogenous) const
{
    float sqw = s * s;
    float sqx = v.x * v.x;
    float sqy = v.y * v.y;
    float sqz = v.z * v.z;

    vec3 euler;
    if (homogenous)
    {
        euler.x = atan2f(2.f * (v.x * v.y + v.z * s), sqx - sqy - sqz + sqw);
        euler.y = asinf(-2.f * (v.x * v.z - v.y * s));
        euler.z = atan2f(2.f * (v.y * v.z + v.x * s), -sqx - sqy + sqz + sqw);
    }
    else
    {
        euler.x = atan2f(2.f * (v.z * v.y + v.x * s), 1 - 2 * (sqx + sqy));
        euler.y = asinf(-2.f * (v.x * v.z - v.y * s));
        euler.z = atan2f(2.f * (v.x * v.y + v.z * s), 1 - 2 * (sqy + sqz));
    }
    return euler;
}

mat4 quaternion::getMat4()
{
    return  mat4(
                1 - 2 * (v.y * v.y + v.z * v.z),  2 * (v.x * v.y - s * v.z),      2 * (v.x * v.z + s * v.y),  0,
                2 * (v.x * v.y + s * v.z),      1 - 2 * (v.x * v.x + v.z * v.z),  2 * (v.y * v.z - s * v.x),  0,
                2 * (v.x * v.z - s * v.y),      2 * (v.y * v.z + s * v.x), 1 - 2 * (v.x * v.x + v.y * v.y),    0,
                0,                          0,                  0,              1);

}   

void quaternion::fromAxisAngle(vec3 axis, float angle)
{
    float sin_a = sin( angle / 2 );
    float cos_a = cos( angle / 2 );

    v.x = axis.x * sin_a;
    v.y = axis.y * sin_a;
    v.z = axis.z * sin_a;
    s = cos_a;

    normalize();
}

void quaternion::fromEuler(float xAngle, float yAngle, float zAngle)
{
    vec3 vx = { 0, 0, 1 }, vy = { 0, 1, 0 }, vz = { 1, 0, 0 };
    quaternion qx, qy, qz, qt;

    qx.fromAxisAngle(vx, xAngle);
    qy.fromAxisAngle(vy, yAngle);
    qz.fromAxisAngle(vz, zAngle);
    qt = qx * qy;
    qt = qt * qz;
    v = qt.v;
    s = qt.s;
}

int    the::math::floor(float v)     {return std::floor(v);}
int    the::math::abs(int v)         {return std::abs(v);}
float  the::math::abs(float v)       {return std::fabs(v);}
double the::math::abs(double v)      {return std::fabs(v);}
float  the::math::sqrt(float  v)     {return std::sqrt(v);}
float  the::math::sin(float  angle)  {return std::sin(angle);}
float  the::math::cos(float  angle)  {return std::cos(angle);}
float  the::math::acos(float v)      {return std::acos(v);}
float  the::math::asin(float v)      {return std::asin(v);}
float  the::math::atan(float v)      {return std::atan(v);}
float  the::math::atan2(float v1,float v2)  {return std::atan2(v1,v2);}
float  the::math::rnd() {return static_cast<float>(rand())/static_cast<float>(RAND_MAX);}
float  the::math::radians(float deg) {return deg * DEG2RAD;}
float  the::math::degrees(float rad) {return rad * RAD2DEG;}

float the::math::rsqrt(float  x)  
{
    long i;
    float y, r;

    y = x * 0.5f;
    i = *reinterpret_cast<long *>(&x);
    i = 0x5f3759df - ( i >> 1 );
    r = *reinterpret_cast<float *>(&i);
    r = r * ( 1.5f - r * r * y );
    return r;
}

bool the::math::isPowerOfTwo(int x) {return ( x & ( x - 1 ) ) == 0 && x > 0;}  
